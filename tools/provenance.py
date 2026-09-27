#!/usr/bin/env python3
"""List what an agent fetched or looked at outside its workspace, from its Pi transcript (events.jsonl, or .zst).

  python3 tools/provenance.py EVENTS WORKSPACE

Prints one line per network fetch (URL), clone, and folder outside WORKSPACE that the agent's commands or its
read/edit/write tools touched (with the paths under it); nothing if there were none. System paths (/usr, /bin, /dev,
/proc, ...) and the shared base checkout named by $TTYD_BASE are not listed. The launcher saves this as
provenance.txt next to the agent's results; tools/merge_agent.sh shows it and records it in the commit, so reused
code is on the record and its license can be checked before merging. Exits 1 if the transcript can't be read or
has no tool calls. Read-only.
"""
import json
import os
import re
import shlex
import subprocess
import sys

URL = re.compile(r"(?:https?://|ftp://|\b[\w.-]+@[\w.-]+:(?=[\w.-]+/)|\b(?=(?:[\w-]+\.)*(?:github\.com|githubusercontent\.com"
                 r"|gitlab\.com|codeberg\.org|bitbucket\.org|decomp\.me|sourceforge\.net)/))[^\s'\"\\)|;&<>]+", re.I)
CLONE = re.compile(r"\b(?:git\s+clone|gh\s+repo\s+clone|svn\s+(?:checkout|co)|hg\s+clone)\s+([^;&|\n]+)")
SYSTEM = ("/usr/", "/bin/", "/sbin/", "/lib/", "/lib64/", "/dev/", "/proc/", "/sys/", "/etc/")
PATTERN_FIRST = {"sed", "awk", "gawk", "grep", "egrep", "fgrep", "rg"}  # their first non-option word is a pattern
PATH_KEYS = ("path", "file_path", "filePath", "directory", "dir", "cwd")


def events(path):
    if path.endswith(".zst"):
        r = subprocess.run(["zstd", "-dc", path], capture_output=True)
        if r.returncode:
            raise OSError(f"zstd -dc {path}: {r.stderr.decode(errors='replace').strip()}")
        data = r.stdout
    else:
        data = open(path, "rb").read()
    found = False
    for line in data.decode(errors="replace").splitlines():
        try:
            e = json.loads(line)
        except ValueError:
            continue
        if e.get("type") == "tool_execution_start":
            found = True
            yield e.get("toolName"), e.get("args") or {}
    if not found:
        raise OSError(f"{path}: no tool calls found (not a Pi transcript, or empty)")


def inside(p, roots):
    p = os.path.normpath(p)
    return any(p == r or p.startswith(r.rstrip("/") + "/") for r in roots)


def system(p):
    return (p + "/").startswith(SYSTEM) and not p.startswith("/usr/local/")


def expand(command):
    """$HOME/${HOME}, simple VAR=value assignments made earlier in the command, and one level of {a,b} braces."""
    env = {"HOME": os.path.expanduser("~")}
    for m in re.finditer(r"(?:^|[\s;&|(])([A-Za-z_]\w*)=([^\s;&|]+)", command):
        env[m.group(1)] = m.group(2).strip("'\"")
    command = re.sub(r"\$\{?([A-Za-z_]\w*)\}?", lambda m: env.get(m.group(1), m.group(0)), command)
    return re.sub(r"(\S*)\{([^{}\s]*,[^{}\s]*)\}(\S*)",
                  lambda m: " ".join(m.group(1) + x + m.group(3) for x in m.group(2).split(",")), command)


def outside_paths(command, cwd, roots):
    try:
        words = shlex.split(command, comments=True)
    except ValueError:
        words = command.split()
    words = [w for word in words for w in re.split(r"[|<>;&]+", word) if w]
    pattern_next, prev = False, ""
    for w, prev in zip(words, [""] + words):
        if prev in ("-e", "--expression", "--regexp", "-f"):
            continue
        if os.path.basename(w) in PATTERN_FIRST:
            pattern_next = True
            continue
        if pattern_next and w.startswith("-"):
            continue
        if pattern_next:
            pattern_next = False  # sed/awk script or grep pattern, not a path
            continue
        for part in [w] if w.startswith(("/", "~", ".")) else re.split(r"[=:,]", w):
            if not part.startswith(("/", "~", "..")) or part.startswith("//") or re.search(r"[\[\]()]", part):
                continue  # not a path, or code quoted in a command (python -c, sed s///)
            p = os.path.normpath(os.path.join(cwd, os.path.expanduser(part)))
            if not (inside(p, roots) or system(p)):
                yield p


def area(p):
    """The folder a path belongs to: ~/X, ~/ttyd-agents/X (a workspace), /X/Y for a folder Y (loose files in /X
    group under /X); / and ~ themselves."""
    parts = p.strip("/").split("/")
    home = os.path.expanduser("~").strip("/").split("/")
    if parts[:len(home)] == home:
        depth = len(home) + (2 if parts[len(home):len(home) + 1] == ["ttyd-agents"] else 1)
    else:
        depth = 2 if len(parts) > 2 else 1
    return "/" + "/".join(parts[:depth]) if parts != [""] else "/"


def strings(v):
    if isinstance(v, str):
        yield v
    elif isinstance(v, dict):
        for x in v.values():
            yield from strings(x)
    elif isinstance(v, list):
        for x in v:
            yield from strings(x)


def findings(path, workspace):
    roots = [os.path.normpath(workspace)] + [os.path.normpath(b) for b in os.environ.get("TTYD_BASE", "").split(":") if b]
    seen, areas = [], {}
    for tool, args in events(path):
        if tool == "bash":
            text = expand(args.get("command", ""))
            seen += [f"fetch  {m.group(0)}" for m in URL.finditer(text)]
            seen += [f"clone  {m.group(1).strip()}" for m in CLONE.finditer(text)]
            paths = list(outside_paths(text, workspace, roots))
        else:
            if tool not in ("write", "edit"):  # file contents aren't fetches
                seen += [f"fetch  {m.group(0)}" for s in strings(args) for m in URL.finditer(s)]
            paths = [os.path.normpath(os.path.join(workspace, os.path.expanduser(args[k])))
                     for k in PATH_KEYS if isinstance(args.get(k), str)]
            paths = [p for p in paths if not (inside(p, roots) or system(p))]
        for p in paths:
            areas.setdefault(area(p), set()).add(p)
    for a, ps in sorted(areas.items()):
        ps = sorted(ps)
        more = f", +{len(ps) - 5} more" if len(ps) > 5 else ""
        seen.append(f"path   {a}: {', '.join(ps[:5])}{more}")
    return list(dict.fromkeys(seen))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    try:
        print("\n".join(findings(sys.argv[1], sys.argv[2])))
    except OSError as e:
        sys.exit(f"provenance.py: {e}")
