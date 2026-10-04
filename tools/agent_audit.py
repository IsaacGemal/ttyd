#!/usr/bin/env python3
"""Flag an agent run's tool calls that reach outside its workspace or start processes meant to outlive it.

    python3 tools/agent_audit.py events.jsonl WORKSPACE        exit 1 if anything is flagged

Reads the agent's JSON event log (tool_execution_start events: bash commands, and read/write/edit paths) and lists:
  WRITE-OUTSIDE   a write, copy, move, mkdir, rm or redirect whose target is outside WORKSPACE (or a write/edit tool
                  call on such a path)
  BACKGROUND      nohup, setsid, disown, screen, tmux, systemd-run, at/crontab, or a command backgrounded with `&`
  SIGNAL          pkill/killall (host-wide name matches) or kill of a pid not started in the same command
  READ-OUTSIDE    a read of a path outside WORKSPACE (reported, not counted as a failure: the sandbox allows reads)
The sandbox (tools/wave_launch.py) already blocks the writes and ends background processes; this is the record of
attempts, so a batch report can name the agent."""
import json
import re
import shlex
import sys

WRITE_CMDS = {"cp", "mv", "mkdir", "rm", "rmdir", "touch", "tee", "rsync", "ln", "install", "dd", "truncate", "chmod"}
BG_WORDS = re.compile(r"(?<![\w-])(nohup|setsid|disown|screen|tmux|systemd-run|crontab|at)(?![\w-])")
SIG_WORDS = re.compile(r"(?<![\w-])(pkill|killall)(?![\w-])")
PATH_RE = re.compile(r"(?:^|[\s=:'\"(<>|])((?:~|/home/|/tmp/|/var/|/etc/|/opt/|/srv/|/root/|/mnt/|/media/)[^\s;|&'\"<>)]*)")
HOME = "/home/"


def outside(path, ws):
    p = path.replace("~", "/home/isaac", 1) if path.startswith("~") else path
    p = re.sub(r"/+", "/", p)
    if p == ws or p.startswith(ws.rstrip("/") + "/"):
        return False
    if p.startswith("/tmp") or p.startswith("/dev/null") or p.startswith("/proc/self"):
        return False  # the agent's own /tmp is private inside the sandbox
    return True


HEREDOC = re.compile(r"<<-?\s*['\"]?(\w+)['\"]?[^\n]*\n.*?\n\s*\1\s*(?:\n|$)", re.S)


def shell_only(cmd):
    """The command with heredoc bodies and quoted strings removed: what the shell itself acts on."""
    c = HEREDOC.sub(" <<HEREDOC\n", cmd)
    c = re.sub(r"'[^']*'", "''", c)
    c = re.sub(r'"(?:[^"\\]|\\.)*"', '""', c)
    return c


def background(cmd):
    if BG_WORDS.search(cmd):
        return True
    # a single `&` (not `&&`, not `>&` or `&>` redirects) that sends something to the background
    for m in re.finditer(r"(?<![&>|])&(?![&>])", cmd):
        before = cmd[: m.start()].rstrip()
        if before and not before.endswith(("2>", "1>", ">")):
            return True
    return False


def audit(events, ws):
    flags = []
    for n, line in enumerate(events, 1):
        try:
            e = json.loads(line)
        except ValueError:
            continue
        if e.get("type") != "tool_execution_start":
            continue
        tool, args = e.get("toolName"), e.get("args") or {}
        if tool in ("write", "edit"):
            p = args.get("path", "")
            if p and outside(p, ws):
                flags.append(("WRITE-OUTSIDE", n, f"{tool} {p}"))
            continue
        if tool == "read":
            p = args.get("path", "")
            if p and outside(p, ws):
                flags.append(("READ-OUTSIDE", n, f"read {p}"))
            continue
        full = args.get("command", "")
        if not full:
            continue
        short = full if len(full) < 200 else full[:200] + "..."
        cmd = shell_only(full)
        if background(cmd):
            flags.append(("BACKGROUND", n, short))
        if SIG_WORDS.search(cmd):
            flags.append(("SIGNAL", n, short))
        kind = None
        for seg in re.split(r"&&|\|\||;|\|", cmd):
            try:
                words = shlex.split(seg, posix=True)
            except ValueError:
                words = seg.split()
            while words and words[0] in ("do", "then", "else", "{", "(", "time", "sudo", "nice", "nohup", "exec"):
                words = words[1:]
            if not words:
                continue
            paths = [m.group(1) for m in PATH_RE.finditer(" " + seg)]
            if not any(outside(p, ws) for p in paths):
                continue
            redirect = re.search(r">>?\s*((?:~|/)[^\s;|&]*)", seg)
            wrote = (words[0] in WRITE_CMDS) or (redirect and outside(redirect.group(1), ws))
            if wrote:
                kind = "WRITE-OUTSIDE"
                break
            kind = kind or "READ-OUTSIDE"
        if kind:
            flags.append((kind, n, short))
    return flags


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    ws = sys.argv[2].rstrip("/")
    flags = audit(open(sys.argv[1], errors="replace"), ws)
    bad = [f for f in flags if f[0] != "READ-OUTSIDE"]
    seen = set()
    for kind, n, what in flags:
        if (kind, what) in seen:
            continue
        seen.add((kind, what))
        print(f"{kind:13s} event {n}: {what}")
    print(f"{len(bad)} flagged, {len(flags) - len(bad)} outside reads")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
