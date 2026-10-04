#!/usr/bin/env python3
"""Flag an agent run's tool calls that reach outside its workspace, start processes meant to outlive it, or talk to
host services.

    python3 tools/agent_audit.py events.jsonl WORKSPACE [BASE]        exit 1 if anything is flagged

Reads the agent's JSON event log (tool_execution_start events: bash commands, and read/write/edit paths) and lists:
  WRITE-OUTSIDE   a write, copy, move, mkdir, rm or redirect whose target is outside WORKSPACE (or a write/edit tool
                  call on such a path)
  EXEC-OUTSIDE    a program or script run from under /home outside WORKSPACE (e.g. ~/permuter's tools)
  BACKGROUND      nohup, setsid, disown, screen, tmux, at/batch/crontab, or a command backgrounded with `&`
  HOST-SERVICE    docker/podman/containerd, systemctl/systemd-run, D-Bus or X clients: ways past the sandbox
  SIGNAL          pkill/killall, `kill $(pgrep ...)`, `pgrep | xargs kill`, kill -1 or a negative pid
  EVENTS-CORRUPT  an unparseable line, except one truncated final line at a timeout boundary in the host's sibling
                  passes.txt (byte offset, exit code and host timeout flag). Legacy logs without that file retain marker
                  and marker-free final-line tolerance. Recovered calls are audited; malformed actual tool events always fail
  ASSIGN-OUTSIDE  a shell variable set to a path under /home outside WORKSPACE, BASE and the shared read-only paths
                  (reported, not counted: a hint that it may be used to write)
  READ-OUTSIDE    a read of a path under /home, /root, /etc, /var, /opt, /srv, /mnt, /media or /run outside
                  WORKSPACE (reported, not counted: the sandbox allows reads of what it shows)
Commands are followed into `bash -c`/`sh -c`/`eval` strings, `$(...)`, `find -exec`, `xargs`, shell heredocs, and
python run with -c or a heredoc (os.system/subprocess strings are audited as shell; other code by its path literals).
Relative paths are resolved against the directory `cd` left. This is a record of attempts and a hint, not the
defence: the sandbox (tools/wave_launch.py) is what blocks them."""
import json
import posixpath
import re
import shlex
import sys
from pathlib import Path

HOME = "/home/isaac"
WRITE_CMDS = {"cp", "mv", "mkdir", "rm", "rmdir", "touch", "tee", "rsync", "ln", "install", "dd", "truncate", "chmod",
              "chown", "unlink", "shred", "scp", "tar", "unzip", "patch", "git"}
BG_CMDS = {"nohup", "setsid", "disown", "screen", "tmux", "at", "batch", "crontab", "daemonize", "start-stop-daemon"}
SERVICE_CMDS = {"docker", "podman", "ctr", "nerdctl", "containerd", "dockerd", "systemctl", "systemd-run", "busctl",
                "dbus-send", "gdbus", "dbus-monitor", "loginctl", "xdotool", "xdg-open", "xauth", "xinput", "xsel", "xclip"}
SHELLS = {"bash", "sh", "zsh", "dash", "ash", "ksh", "mksh", "busybox"}
PYTHONS = re.compile(r"^python(\d+(\.\d+)?)?$")
# words skipped in front of the real command (with how many arguments each takes, when known)
PREFIX = {"do", "then", "else", "elif", "if", "while", "until", "{", "(", "!", "time", "sudo", "exec", "command",
          "builtin", "stdbuf", "unbuffer", "nice", "ionice", "env", "timeout", "xargs", "chronic", "flock"}
READ_ROOTS = ("/home/", "/root", "/etc/", "/var/", "/opt/", "/srv/", "/mnt/", "/media/", "/run/")
# shared read-only parts of the checkout every workspace links to: running these is ordinary work
SHARED = (HOME + "/ttyd/build/compilers/", HOME + "/ttyd/.venv/", HOME + "/ttyd/orig/")
# what the sandbox mounts back read-only under ~ (tools/wave_launch.py HOME_RO, and the checkout itself): a variable
# pointing there is ordinary setup (TTYD_BASE=~/ttyd, PATH=$HOME/.local/bin:$PATH)
READONLY = SHARED + (HOME + "/ttyd/", HOME + "/.local/", HOME + "/.nvm/", HOME + "/ttyd-pylib/")
NOT_COUNTED = ("READ-OUTSIDE", "ASSIGN-OUTSIDE")
BASE = ""  # set by main() from the optional BASE argument: the agent's base checkout, read-only in the sandbox
OPS = ["&&", "||", ";;", "|&", "\n", ";", "|", "&"]


def norm(path, cwd):
    p = path
    for h in ("${HOME}", "$HOME"):
        if p == h or p.startswith(h + "/"):
            p = HOME + p[len(h):]
    if p == "~" or p.startswith("~/"):
        p = HOME + p[1:]
    if not p.startswith("/"):
        p = posixpath.join(cwd, p)
    return posixpath.normpath(p)


def outside(path, ws, cwd=None):
    p = norm(path, cwd or ws)
    if p == ws or p.startswith(ws + "/"):
        return False
    if p == "/tmp" or p.startswith("/tmp/"):
        return False  # the agent's own /tmp is private inside the sandbox
    if p in ("/dev/null", "/dev/stdout", "/dev/stderr", "/dev/tty") or p.startswith("/proc/self") or p.startswith("/dev/fd"):
        return False
    return True


def pathlike(w):
    return w.startswith(("/", "~", "$HOME", "${HOME}", "./", "../")) or w in (".", "..") or ("/" in w and not w.startswith("-") and "://" not in w)


HEREDOC = re.compile(r"<<-?\s*(['\"]?)(\w+)\1([^\n]*)\n(.*?)\n[ \t]*\2[ \t]*(?=\n|$)", re.S)


def heredocs(cmd):
    """(shell text with each heredoc body removed, [(line that feeds it, body)])."""
    docs = []

    def cut(m):
        start = cmd.rfind("\n", 0, m.start()) + 1
        docs.append((cmd[start:m.start()] + m.group(3), m.group(4)))
        return "<<HEREDOC" + m.group(3) + "\n"
    return HEREDOC.sub(cut, cmd), docs


def substitutions(cmd):
    """Contents of $(...) and `...` outside single quotes (nested ones are found when those are audited)."""
    out, i, q = [], 0, None
    while i < len(cmd):
        c = cmd[i]
        if q == "'":
            q = None if c == "'" else q
        elif c == "\\":
            i += 1
        elif c == "'" and q is None:
            q = "'"
        elif c == '"':
            q = None if q == '"' else '"'
        elif cmd.startswith("$(", i) and not cmd.startswith("$((", i):
            depth, j = 1, i + 2
            while j < len(cmd) and depth:
                depth += cmd[j] == "("
                depth -= cmd[j] == ")"
                j += 1
            out.append(cmd[i + 2:j - 1])
            i = j
            continue
        elif c == "`":
            j = cmd.find("`", i + 1)
            if j < 0:
                break
            out.append(cmd[i + 1:j])
            i = j
        i += 1
    return out


def segments(cmd):
    """Split on shell operators outside quotes: [(segment, operator after it)]."""
    segs, cur, i, q, depth = [], [], 0, None, 0
    while i < len(cmd):
        c = cmd[i]
        if q:
            cur.append(c)
            if c == q:
                q = None
            elif c == "\\" and q == '"' and i + 1 < len(cmd):
                cur.append(cmd[i + 1])
                i += 1
            i += 1
            continue
        if c in "'\"":
            q = c
        elif c == "#" and (not cur or cur[-1] in " \t\n;(") and not depth:
            j = cmd.find("\n", i)  # a comment, to the end of the line
            i = len(cmd) if j < 0 else j
            continue
        elif c == "\\" and i + 1 < len(cmd):
            cur += [c, cmd[i + 1]]
            i += 2
            continue
        elif cmd.startswith("$(", i):
            depth += 1
        elif c == ")" and depth:
            depth -= 1
        elif not depth:
            op = next((o for o in OPS if cmd.startswith(o, i)), None)
            # `&` in a redirect (2>&1, &>, >&) is not an operator
            if op == "&" and (cmd[i - 1:i] in (">", "<") or cmd[i + 1:i + 2] == ">"):
                op = None
            if op:
                segs.append(("".join(cur), op))
                cur = []
                i += len(op)
                continue
        cur.append(c)
        i += 1
    segs.append(("".join(cur), ""))
    return segs


def split_words(seg):
    try:
        return shlex.split(seg, posix=True)
    except ValueError:
        return seg.split()


def strip_prefix(words):
    """Drop leading assignments and wrapper commands (with their options), so words[0] is what actually runs."""
    while words:
        w = words[0]
        name = posixpath.basename(w)
        if re.match(r"^\w+=", w):
            words = words[1:]
        elif name in PREFIX:
            words = words[1:]
            # options of the wrapper, and the numeric/duration argument of timeout/nice
            while words and (words[0].startswith("-") or re.match(r"^\w+=", words[0]) or
                             (name in ("timeout", "nice", "ionice", "flock") and re.match(r"^[\d.]+[smhd]?$", words[0]))):
                takes_arg = words[0] in ("-k", "-s", "-n", "-c", "-I", "-P", "-L", "-d", "--signal", "--kill-after", "-u")
                words = words[2:] if takes_arg else words[1:]
        else:
            break
    return words


REDIRECT = re.compile(r"(?<![<&>])(?:\d|&)?>>?\|?\s*(?!&)(['\"]?)([^\s;|&'\"<>()]+)\1")


def redirects(seg):
    return [m.group(2) for m in REDIRECT.finditer(seg)]


def editor_files(name, args):
    """The file operands of `sed`/`perl` (in-place): without -e/-f (sed) or -e/-E (perl) the first operand is the
    script, not a file."""
    files, script_given, i = [], False, 0
    while i < len(args):
        a = args[i]
        if a in ("-e", "-f", "--expression", "--file") or (name == "perl" and a in ("-E", "-M", "-I")):
            script_given = script_given or a not in ("-M", "-I")
            i += 2
            continue
        if a.startswith("--expression=") or a.startswith("--file="):
            script_given = True
        elif a.startswith("-") and a != "-":
            if name == "perl" and re.match(r"^-[A-Za-z]*e$", a):  # -pie / -ne: the next argument is the script
                script_given = True
                i += 2
                continue
        else:
            files.append(a)
        i += 1
    return files if script_given else files[1:]


def audit_python(code, ws, cwd):
    flags = []
    for m in re.finditer(r"(?:os\.system|os\.popen|subprocess\.\w+|Popen)\s*\(\s*[rbf]?(['\"])((?:\\.|(?!\1).)*)\1", code, re.S):
        flags += audit_shell(m.group(2), ws, cwd)
    if re.search(r"start_new_session\s*=\s*True|os\.setsid|os\.fork\(|os\.daemon|nohup|setsid", code):
        flags.append(("BACKGROUND", "python: detached process"))
    lits = [m.group(2) for m in re.finditer(r"(['\"])((?:/home/|~/|/root|/etc/|/var/|/opt/|/run/)[^'\"]*)\1", code)]
    if re.search(r"expanduser|Path\.home\(\)|os\.environ\[.HOME.\]|getenv\(.HOME.\)", code):
        lits.append(HOME + "/?")
    out = [p for p in lits if outside(p, ws, cwd)]
    if out:
        writes = re.search(r"open\([^)]*['\"][wax]b?\+?['\"]|write_text|write_bytes|shutil\.|os\.(makedirs|mkdir|rename|replace|remove|unlink|symlink|link|chmod)|copy", code)
        execs = re.search(r"os\.system|subprocess|Popen|os\.exec|os\.spawn|runpy|exec\(", code)
        if writes:
            flags.append(("WRITE-OUTSIDE", f"python: {out[0]}"))
        elif execs and any(p.startswith(("/home/", "~/")) for p in out):
            flags.append(("EXEC-OUTSIDE", f"python: {out[0]}"))
        elif any(p.startswith(READ_ROOTS) or p.startswith("~/") for p in out):
            flags.append(("READ-OUTSIDE", f"python: {out[0]}"))
    return flags


def audit_shell(cmd, ws, cwd=None, depth=0):
    """Flags for one shell command string: [(kind, detail)]."""
    if depth > 6:
        return []
    cwd = cwd or ws
    flags = []
    text, docs = heredocs(cmd)
    for line, body in docs:
        w = strip_prefix(split_words(line.split("<<")[0]))
        name = posixpath.basename(w[0]) if w else ""
        if name in SHELLS:
            flags += audit_shell(body, ws, cwd, depth + 1)
        elif PYTHONS.match(name) or name == "node":
            flags += audit_python(body, ws, cwd)
    for sub in substitutions(text):
        flags += audit_shell(sub, ws, cwd, depth + 1)
    pids_from_pgrep = False
    segs = segments(text)
    for idx, (seg, op) in enumerate(segs):
        raw = split_words(seg)
        words = strip_prefix(list(raw))
        # a variable holding an outside path under /home (`p=/home/...; touch "$p"`): a hint, counted
        for w in raw:
            m = re.match(r"^(?:export\s+)?[A-Za-z_]\w*=(.+)$", w)
            if m and m.group(1).startswith(("/home/", "~/", "$HOME/", "${HOME}/")):
                q = norm(m.group(1).split(":")[0], cwd)
                if outside(q, ws, cwd) and not (q + "/").startswith(READONLY) \
                        and not (BASE and (q + "/").startswith(BASE.rstrip("/") + "/")):
                    flags.append(("ASSIGN-OUTSIDE", seg.strip()))
        if op == "&" and words:
            flags.append(("BACKGROUND", seg.strip()))
        if not words:
            continue
        name = posixpath.basename(words[0])
        args = words[1:]
        # redirects apply whatever the command is (also `bash -c '...' > X`, `command -v x > X`)
        if any(outside(t, ws, cwd) for t in redirects(seg) if not t.startswith("&")):
            flags.append(("WRITE-OUTSIDE", seg.strip()))
        if raw and posixpath.basename(raw[0]) == "command" and raw[1:2] and raw[1] in ("-v", "-V"):
            continue  # a lookup, not a run
        # wrappers that carry a command in their arguments (-c, also inside combined flags: -lc, -ec, -xec)
        copt = next((i for i, a in enumerate(args) if re.match(r"^-[A-Za-z]*c[A-Za-z]*$", a)), None)
        if name in SHELLS and copt is not None:
            rest = [a for a in args[copt + 1:] if not a.startswith("-")]
            if rest:
                flags += audit_shell(rest[0], ws, cwd, depth + 1)
            continue
        if name == "eval":
            flags += audit_shell(" ".join(args), ws, cwd, depth + 1)
            continue
        if PYTHONS.match(name) and "-c" in args[:3]:
            i = args.index("-c")
            if i + 1 < len(args):
                flags += audit_python(args[i + 1], ws, cwd)
        if name == "find" and any(a in ("-exec", "-execdir", "-ok", "-delete") for a in args):
            if "-delete" in args and any(outside(a, ws, cwd) for a in args[:1] if not a.startswith("-")):
                flags.append(("WRITE-OUTSIDE", seg.strip()))
            for i, a in enumerate(args):
                if a in ("-exec", "-execdir", "-ok"):
                    inner = [x for x in args[i + 1:] if x not in (";", "\\;", "+")]
                    flags += audit_shell(" ".join(shlex.quote(x) for x in inner), ws, cwd, depth + 1)
        if name == "cd":
            cwd = norm(args[0], cwd) if args and args[0] != "-" else ws
            continue
        if name in BG_CMDS:
            flags.append(("BACKGROUND", seg.strip()))
        if name in SERVICE_CMDS:
            flags.append(("HOST-SERVICE", seg.strip()))
        if name in ("pkill", "killall"):
            flags.append(("SIGNAL", seg.strip()))
        if name == "pgrep" and op == "|":
            pids_from_pgrep = True
        if name == "kill":
            targets = list(args)
            if targets and targets[0] in ("-s", "-n"):
                targets = targets[2:]
            elif targets and re.match(r"^-(\d+|[A-Z]+\d*|SIG\w+)$", targets[0]):
                targets = targets[1:]
            targets = [t for t in targets if t != "--"]
            if pids_from_pgrep or "$(" in seg or "`" in seg or any(t in ("-1", "0") or t.startswith("-") for t in targets) \
                    or (targets and not all(t.startswith(("$!", "%")) for t in targets)):
                flags.append(("SIGNAL", seg.strip()))
        if pids_from_pgrep and name != "pgrep" and name != "kill" and op != "|":
            pids_from_pgrep = False
        # what runs: a program or script under /home outside the workspace
        script, interp = words[0], PYTHONS.match(name) or name in SHELLS or name in ("node", "perl")
        if interp and args:
            script = next((a for a in args if not a.startswith("-")), "")
        # an interpreter's script argument is a path even without a slash; `python3 -m` runs code from the cwd
        runs_cwd = PYTHONS.match(name) and "-m" in args
        target = cwd if runs_cwd else (norm(script, cwd) if script and (pathlike(script) or interp) else "")
        if target and target.startswith("/home/") and outside(target, ws) and not (target + "/").startswith(SHARED):
            flags.append(("EXEC-OUTSIDE", seg.strip()))
        targets = [r for r in redirects(seg) if not r.startswith("&")]
        wrote = any(outside(t, ws, cwd) for t in targets)
        if name in WRITE_CMDS and not wrote:
            operands = [a for a in args if not a.startswith("-")]
            if name == "git":
                operands = operands[1:] if operands and operands[0] in ("clone", "init", "worktree") else []
            if name == "dd":
                operands = [a[3:] for a in args if a.startswith("of=")]
            wrote = any(outside(a, ws, cwd) for a in operands)
        # in-place editors: every file operand is written (resolved against the tracked cwd, slash or not)
        if not wrote and name in ("sed", "perl") and any(re.match(r"^-[A-Za-z]*i", a) or a.startswith("--in-place") for a in args):
            wrote = any(outside(f, ws, cwd) for f in editor_files(name, args))
        if wrote:
            flags.append(("WRITE-OUTSIDE", seg.strip()))
            continue
        reads = [a for a in args + words[:1] if pathlike(a) and outside(a, ws, cwd) and norm(a, cwd).startswith(READ_ROOTS)]
        if reads:
            flags.append(("READ-OUTSIDE", seg.strip()))
    return flags


def tool_flags(e, ws, n):
    """Audit a parsed event, including events recovered from a damaged line."""
    if not isinstance(e, dict) or e.get("type") != "tool_execution_start":
        return []
    tool, args = e.get("toolName"), e.get("args") or {}
    if not isinstance(args, dict):
        return [("EVENTS-CORRUPT", n, "tool call arguments are not an object")]
    if tool in ("write", "edit", "read"):
        p = args.get("path", "")
        if p and outside(p, ws):
            return [("READ-OUTSIDE" if tool == "read" else "WRITE-OUTSIDE", n, f"{tool} {p}")]
        return []
    full = args.get("command", "")
    if not full:
        return []
    short = full if len(full) < 200 else full[:200] + "..."
    kinds = []
    for kind, _ in audit_shell(full, ws):
        if kind not in kinds:
            kinds.append(kind)
    if "WRITE-OUTSIDE" in kinds and "READ-OUTSIDE" in kinds:
        kinds.remove("READ-OUTSIDE")
    return [(k, n, short) for k in kinds]


# Decode only plausible objects, charging the entire suffix against a linear work budget. A damaged nested
# prefix can otherwise cause raw_decode to rescan the same megabytes once for every opening brace.
OBJECT_START = re.compile(r'\{\s*"')
JSON_STRING = r'"(?:[^"\\]|\\.)*"'
TYPE_ATOM = JSON_STRING + r'|(?:[A-Za-z0-9_]|\\u[0-9a-fA-F]{4})+'
TYPE_VALUE = TYPE_ATOM + r'|"(?:[^"\\]|\\.)*$'
TYPE_PAIR = re.compile(r'(?:\{|,)\s*(' + TYPE_ATOM + r')\s*:\s*(' + TYPE_VALUE + r')')
NUMBER = r'-?(?:0|[1-9][0-9]*)'
JSON_NUMBER = re.compile(NUMBER + r'(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?')
NUMBER_PREFIX = re.compile(r'-|' + NUMBER + r'\.[0-9]*|' + NUMBER + r'(?:\.[0-9]+)?[eE][+-]?[0-9]*')


def json_object_prefix(line):
    """Recognize an unfinished prefix of a JSON object in linear time, without recursive decoding."""
    i, size = 0, len(line)
    stack = []
    while i < size and line[i] in ' \t\r\n':
        i += 1
    if i == size or line[i] != '{':
        return False
    stack.append('key_or_end')
    i += 1
    while True:
        while i < size and line[i] in ' \t\r\n':
            i += 1
        if i == size:
            return bool(stack)
        if not stack:
            return False  # a complete object followed by garbage cannot be a timeout cut
        state, char = stack[-1], line[i]
        if state in ('key_or_end', 'array_value_or_end', 'object_comma', 'array_comma'):
            end = '}' if state in ('key_or_end', 'object_comma') else ']'
            if char == end:
                stack.pop()
                i += 1
                continue
        if state in ('object_comma', 'array_comma'):
            if char != ',':
                return False
            stack[-1] = 'key' if state == 'object_comma' else 'array_value'
            i += 1
            continue
        if state == 'colon':
            if char != ':':
                return False
            stack[-1] = 'object_value'
            i += 1
            continue
        key = state in ('key', 'key_or_end')
        stack[-1] = 'colon' if key else ('object_comma' if state == 'object_value' else 'array_comma')
        if char == '"':
            i += 1
            while i < size:
                char = line[i]
                i += 1
                if char == '"':
                    break
                if ord(char) < 32:
                    return False
                if char == '\\':
                    if i == size:
                        return True
                    escape = line[i]
                    i += 1
                    if escape == 'u':
                        for _ in range(4):
                            if i == size:
                                return True
                            if line[i] not in '0123456789abcdefABCDEF':
                                return False
                            i += 1
                    elif escape not in '"\\/bfnrt':
                        return False
            else:
                return True
            continue
        if key:
            return False
        if char in '{[':
            stack.append('key_or_end' if char == '{' else 'array_value_or_end')
            i += 1
            continue
        if char in '-0123456789':
            start = i
            while i < size and line[i] in '-+0123456789.eE':
                i += 1
            token = line[start:i]
            if JSON_NUMBER.fullmatch(token):
                continue
            return i == size and NUMBER_PREFIX.fullmatch(token) is not None
        literal = {'t': 'true', 'f': 'false', 'n': 'null'}.get(char)
        if literal is None:
            return False
        remaining = line[i:i + len(literal)]
        if remaining != literal:
            return i + len(remaining) == size and literal.startswith(remaining)
        i += len(literal)


def recover_events(line, ws, n):
    """Recover calls and identify actual type keys (including JSON escapes), with bounded decode work."""
    flags, has_tool, capped = [], False, False
    for pair in TYPE_PAIR.finditer(line):
        try:
            atoms = [atom if atom.startswith('"') else '"' + atom + '"' for atom in pair.groups()]
            # A cut immediately after the complete tool type's text still identifies a tool event.
            if not atoms[1].endswith('"'):
                atoms[1] += '"'
            key, value = map(json.loads, atoms)
            if key == "type" and value == "tool_execution_start":
                has_tool = True
        except ValueError:
            pass
    decoder = json.JSONDecoder()
    budget = 4 * len(line)
    for start in OBJECT_START.finditer(line):
        budget -= len(line) - start.start()
        if budget < 0:
            capped = True
            break
        try:
            e, _ = decoder.raw_decode(line, start.start())
        except (ValueError, RecursionError):
            continue
        if isinstance(e, dict) and e.get("type") == "tool_execution_start":
            has_tool = True
        flags += tool_flags(e, ws, n)
    return flags, has_tool, capped


def audit(events, ws, passes=None):
    """Read physical lines, splitting at host (byte offset, exit code, timeout fired) pass ends.

    Two-column boundary files remain readable but cannot prove a timeout. Only logs without passes.txt get the
    legacy timeout-marker and single marker-free final-garbled-line tolerances. Byte splitting also handles partial UTF-8.
    """
    ws = ws.rstrip("/")
    flags = []
    boundaries = []
    try:
        for p in (() if passes is None else passes):
            if len(p) not in (2, 3) or any(type(v) is not int for v in p):
                raise ValueError
            end, rc = p[:2]
            fired = p[2] if len(p) == 3 else 0
            if end < 0 or not 0 <= rc <= 255 or fired not in (0, 1) or (boundaries and end < boundaries[-1][0]):
                raise ValueError
            boundaries.append((end, rc, fired))
    except (ValueError, TypeError):
        flags.append(("EVENTS-CORRUPT", 0, "invalid pass boundaries"))
        boundaries = []  # still recover and audit every event
    offset, pass_index, n = 0, 0, 0
    bad = []
    legacy_pending = None
    legacy_marker = False
    for raw in events:
        raw = raw.encode("utf-8") if isinstance(raw, str) else raw
        while raw:
            while pass_index < len(boundaries) and boundaries[pass_index][0] == offset:
                pass_index += 1  # empty passes (or a boundary consumed by the previous line)
            end, rc, fired = boundaries[pass_index] if pass_index < len(boundaries) else (offset + len(raw), None, 0)
            piece, raw = raw[:end - offset], raw[end - offset:]
            offset += len(piece)
            n += 1
            line = piece.decode("utf-8", errors="replace")
            if not line.strip():
                continue
            try:
                e = json.loads(line)
            except (ValueError, RecursionError):
                recovered, has_tool, _ = recover_events(line, ws, n)
                flags += recovered
                # Recovery's work cap does not change whether a non-tool line is a legitimate timeout cut.
                truncated = (offset == end and fired == 1 and not piece.endswith(b"\n")
                             and not has_tool and json_object_prefix(line))
                if not truncated:
                    bad.append(n)
                legacy_pending = n if not has_tool else None
            else:
                flags += tool_flags(e, ws, n)
                if passes is None and isinstance(e, dict) and e.get("type") == "launcher_pass_end":
                    legacy_marker = True
                    if legacy_pending is not None and e.get("rc") in (124, 137):
                        bad.remove(legacy_pending)
                legacy_pending = None
    if passes is not None and (not boundaries or boundaries[-1][0] != offset):
        flags.append(("EVENTS-CORRUPT", n, "pass boundaries do not cover the event log"))
    if passes is None and not legacy_marker and len(bad) == 1 and legacy_pending == bad[0]:
        bad.clear()
    if bad:
        flags.append(("EVENTS-CORRUPT", bad[0],
                      f"{len(bad)} unparseable line(s) in the event log (first: line {bad[0]})"))
    return flags


def main():
    global BASE
    if len(sys.argv) not in (3, 4):
        sys.exit(__doc__)
    ws = sys.argv[2].rstrip("/")
    BASE = sys.argv[3] if len(sys.argv) == 4 else ""
    pass_file = Path(sys.argv[1]).with_name("passes.txt")
    boundary_errors = []
    try:
        passes = [tuple(map(int, line.split())) for line in pass_file.read_text().splitlines()]
        if any(len(p) not in (2, 3) for p in passes):
            raise ValueError("expected byte offset, exit code and optional timeout flag")
    except FileNotFoundError:
        passes = None  # older logs retain their in-stream marker tolerance
    except (OSError, ValueError) as e:
        passes = []
        boundary_errors.append(("EVENTS-CORRUPT", 0, f"cannot read pass boundaries: {e}"))
    with open(sys.argv[1], "rb") as events:
        flags = boundary_errors + audit(events, ws, passes)
    bad = [f for f in flags if f[0] not in NOT_COUNTED]
    seen = set()
    for kind, n, what in flags:
        if (kind, what) in seen:
            continue
        seen.add((kind, what))
        print(f"{kind:13s} event {n}: {what}")
    print(f"{len(bad)} flagged, {len(flags) - len(bad)} outside reads or hints")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
