#!/usr/bin/env python3
"""List what an agent fetched or looked at outside its workspace, from its Pi transcript (events.jsonl, or .zst).

  python3 tools/provenance.py EVENTS WORKSPACE

Prints one line per network fetch (URL), clone, or file path outside WORKSPACE that the agent's commands or its
read/edit/write tools touched; nothing if there were none. System paths (/usr, /bin, /dev, /proc, ...) and the
shared base checkout named by $TTYD_BASE are not listed. The launcher saves this as provenance.txt next to the
agent's results; tools/merge_agent.sh shows it and records it in the commit, so reused code is on the record and its
license can be checked before merging. Exits 1 if the transcript can't be read. Read-only.
"""
import json
import os
import re
import shlex
import subprocess
import sys

URL = re.compile(r"(?:https?://|\b(?=(?:[\w-]+\.)*(?:github\.com|githubusercontent\.com|gitlab\.com|codeberg\.org"
                 r"|bitbucket\.org|decomp\.me|sourceforge\.net)/))[^\s'\"\\)]+")
CLONE = re.compile(r"\b(?:git\s+clone|gh\s+repo\s+clone|svn\s+(?:checkout|co)|hg\s+clone)\s+(?:-\S+\s+)*(\S+)")
SYSTEM = ("/usr/", "/bin/", "/sbin/", "/lib/", "/lib64/", "/dev/", "/proc/", "/sys/", "/etc/", "/opt/")


def events(path):
    if path.endswith(".zst"):
        r = subprocess.run(["zstd", "-dc", path], capture_output=True)
        if r.returncode:
            raise OSError(f"zstd -dc {path}: {r.stderr.decode(errors='replace').strip()}")
        data = r.stdout
    else:
        data = open(path, "rb").read()
    for line in data.decode(errors="replace").splitlines():
        try:
            e = json.loads(line)
        except ValueError:
            continue
        if e.get("type") == "tool_execution_start":
            yield e.get("toolName"), e.get("args") or {}


def inside(p, roots):
    p = os.path.normpath(p)
    return any(p == r or p.startswith(r.rstrip("/") + "/") for r in roots)


def outside_paths(command, cwd, roots):
    try:
        words = shlex.split(command, comments=True)
    except ValueError:
        words = command.split()
    for w in words:
        for part in re.split(r"[=:]", w) if not w.startswith(("/", "~", ".")) else [w]:
            part = part.rstrip(";&|)")
            if not part.startswith(("/", "~", "..")) or part.startswith("//") or re.search(r"[\s,{}\\^|]", part):
                continue  # not a path, or a sed/awk address like /re/,/re/p
            p = os.path.normpath(os.path.join(cwd, os.path.expanduser(part)))
            if not (inside(p, roots) or (p + "/").startswith(SYSTEM)):
                yield p


def area(p):
    """The directory a path belongs to: ~/X, ~/ttyd-agents/X (a workspace), /X/Y for a directory Y (loose files in
    /X group under /X); / and ~ themselves."""
    parts = p.strip("/").split("/")
    home = os.path.expanduser("~").strip("/").split("/")
    if parts[:len(home)] == home:
        depth = len(home) + (2 if parts[len(home):len(home) + 1] == ["ttyd-agents"] else 1)
    else:
        depth = 2 if len(parts) > 2 else 1
    return "/" + "/".join(parts[:depth]) if parts != [""] else "/"


def findings(path, workspace):
    roots = [os.path.normpath(workspace)] + [os.path.normpath(b) for b in os.environ.get("TTYD_BASE", "").split(":") if b]
    seen, areas = [], {}
    for tool, args in events(path):
        if tool == "bash":
            text = args.get("command", "")
            seen += [f"fetch  {m.group(0)}" for m in URL.finditer(text)]
            seen += [f"clone  {m.group(1)}" for m in CLONE.finditer(text)]
            paths = list(outside_paths(text, workspace, roots))
        else:
            p = args.get("path") and os.path.normpath(os.path.join(workspace, args["path"]))
            paths = [p] if p and not inside(p, roots) else []
        for p in paths:
            areas.setdefault(area(p), set()).add(p)
    seen += [f"path   {a}  ({len(ps)} path{'s' * (len(ps) > 1)}, e.g. {min(ps, key=len)})" for a, ps in sorted(areas.items())]
    return list(dict.fromkeys(seen))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    try:
        print("\n".join(findings(sys.argv[1], sys.argv[2])))
    except OSError as e:
        sys.exit(f"provenance.py: {e}")
