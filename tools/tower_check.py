#!/usr/bin/env python3
"""Look for agent escapes on the build host: unexpected directories in ~ and agent processes outside their workspace.

    python3 tools/tower_check.py [--allow FILE]        run on the host; exit 1 if anything is found

1. Every entry in the home directory that matches no allowlist pattern (fnmatch, one per line in FILE, default
   ~/.config/ttyd-tower-allowlist; # comments). New directories made by an agent that wrote outside its workspace
   show up here (e.g. ~/par, ~/permuter-msgdisp*).
2. Every process whose ancestor chain contains an agent sandbox (a bwrap whose arguments bind a ~/ttyd-agents/<ws>
   directory) but whose working directory is outside that workspace, and every process whose working directory is in
   a workspace without such an ancestor (reported, not counted: the launcher's own post-run checks run unsandboxed in
   the workspace for a few seconds; a long-lived one outlived or escaped its sandbox)."""
import fnmatch
import os
import re
import sys

HOME = os.path.expanduser("~")
DEFAULT_ALLOW = os.path.join(HOME, ".config", "ttyd-tower-allowlist")


def allowlist(path):
    if not os.path.exists(path):
        sys.exit(f"no allowlist at {path}: list the expected entries of ~ there, one fnmatch pattern per line")
    return [l.strip() for l in open(path) if l.strip() and not l.lstrip().startswith("#")]


def procs():
    out = {}
    for pid in os.listdir("/proc"):
        if not pid.isdigit():
            continue
        try:
            stat = open(f"/proc/{pid}/stat").read()
            ppid = int(stat[stat.rindex(")") + 2:].split()[1])
            args = open(f"/proc/{pid}/cmdline", "rb").read().split(b"\0")
            cwd = os.readlink(f"/proc/{pid}/cwd")
        except (OSError, ValueError):
            continue
        out[int(pid)] = (ppid, [a.decode(errors="replace") for a in args if a], cwd)
    return out


def sandbox_ws(args):
    """The workspace a bwrap agent sandbox binds, or None."""
    if not args or not args[0].endswith("bwrap"):
        return None
    for i, a in enumerate(args[:-1]):
        if a == "--bind" and "/ttyd-agents/" in args[i + 1] and not args[i + 1].endswith(("/tmp", "/.pi-agent")):
            return args[i + 1]
    return None


def main():
    allow = allowlist(sys.argv[sys.argv.index("--allow") + 1] if "--allow" in sys.argv else DEFAULT_ALLOW)
    found = []
    for name in sorted(os.listdir(HOME)):
        if not any(fnmatch.fnmatch(name, pat) for pat in allow):
            p = os.path.join(HOME, name)
            found.append(f"HOME-ENTRY    ~/{name}  (modified {os.path.getmtime(p):.0f})")
    ps = procs()
    agents_root = os.path.join(HOME, "ttyd-agents") + "/"
    wrappers = set()  # the shell/timeout/systemd-run chain that starts a sandbox runs in the workspace by design
    for pid, (ppid, args, cwd) in ps.items():
        if sandbox_ws(args):
            p = ppid
            while p in ps and p > 1:
                wrappers.add(p)
                p = ps[p][0]
    for pid, (ppid, args, cwd) in ps.items():
        if pid in wrappers:
            continue
        chain, p, ws = [], ppid, None
        while p in ps and p > 1 and len(chain) < 64:
            ws = ws or sandbox_ws(ps[p][1])
            chain.append(p)
            p = ps[p][0]
        ws = sandbox_ws(args) or ws
        if ws and not (cwd == ws or cwd.startswith(ws + "/") or cwd == "/"):
            found.append(f"OUTSIDE-WS    pid {pid} cwd {cwd} (sandbox {ws}): {' '.join(args)[:160]}")
        elif not ws and cwd.startswith(agents_root):
            found.append(f"NO-SANDBOX    pid {pid} cwd {cwd}: {' '.join(args)[:160]}")
    for f in found:
        print(f)
    bad = [f for f in found if not f.startswith("NO-SANDBOX")]
    print(f"{len(bad)} finding(s), {len(found) - len(bad)} unsandboxed process(es) in workspaces")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
