#!/usr/bin/env python3
"""Launch a wave of decomp agents on the build tower, one workspace per spec entry.

Per-wave configs (scratch/waveNN/launch.py, untracked) call main(...) with the wave's name, base checkout, spec and
the agent's model settings; everything that decides what an agent may touch lives here so it is versioned and reviewed.

Each agent runs in a workspace ~/ttyd-agents/<PFX><tag> (an rsync of the base checkout, shared read-only parts linked).
Everything that runs the agent's files (the agent, check.py between passes, rel_link.py) runs in sandbox():
  systemd-run --user --scope in ttyd-agents.slice    CPU, memory and task limits per agent and for all agents together
  bwrap --ro-bind / / --tmpfs $HOME --tmpfs /run/user/<uid>   read-only filesystem, no credentials, no user bus/X auth
        --bind $WS $WS --bind $WS/tmp /tmp                   writable: its own workspace and a private /tmp
        docker and system bus sockets replaced by /dev/null, a clean environment
        --unshare-pid --unshare-ipc --die-with-parent        everything it starts dies with it
The API key reaches the agent CLI through a file on a tmpfs inside the sandbox, never a command line. The agent's
stdout/stderr go through pipes into ~/ttyd-agents-out/<PFX><tag>, which no sandbox can write. After the agent,
tools/agent_collect.sh (from the base, never the workspace) gathers the results there in a second sandbox that sees
the workspace read-only, and runs tools/agent_audit.py; only that directory is copied back, without links.

Environment: KEY (the API key), PFX (workspace prefix, one per key), PFXRE (every prefix sharing this key's slots),
PART ("k/n": this launcher's share of the spec), CAP/QUIET_FROM/QUIET_TO/QUIET_CAP, WORKERS, START_STAGGER,
DEADLINE ("HH:MM" next occurrence, or "YYYY-MM-DD HH:MM"), ONLY, SPECFILE, BASE, ARCHIVE, HOST,
SLICE_PROPS/MEM_MAX/TASKS_MAX.
"""
import json, os, queue, re, shlex, subprocess, sys, threading, time
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
HOST = os.environ.get("HOST", "pop-os")
# set by main(): the per-wave config
HERE = None
WAVE = ""
MODEL = ""
KEY_ENV = ""
MODEL_LINE = ""
CPU_QUOTA = "200%"
WORKERS = int(os.environ.get("WORKERS", 24))
BASE = ""
PFX = os.environ.get("PFX", "")  # one launcher per key
PFXRE = os.environ.get("PFXRE", PFX)  # every prefix on this key, e.g. "w62a_|w71a_"
PART = os.environ.get("PART", "")  # "0/2" or "1/2": this launcher's share of units.json
ARCHIVE = ""
SPEC = []

PROMPT = """You are one of several agents making progress on a matching decompilation of Paper Mario: The
Thousand-Year Door (GameCube, PowerPC, Metrowerks C compiler). A function counts as done only when it compiles to
exactly the original bytes (100%).

YOUR TASK: FINISH THE OPEN FUNCTIONS of {src} (unit {unit}, {part}). These are below 100%, closest first (name, size, current %):
{funcs}
{todo}{group}{extra}Aim for the whole file linking (every function at 100% and the unit identical to the original); matching
functions is the minimum. Work through them in that order: get each to 100% if you can, then move on. Read each one's `// NONMATCHING`
comment for what someone already found, and the brief's "Code shapes that fixed near-misses" section first.
Change only these functions (and, if one needs it, a new static inline helper right above it). Every other
function must keep its score, and none of these may end lower than it started.{note}
Only this file may change, with one exception: FUNCTION PROTOTYPES GO IN HEADERS. For a function defined in
another file, include that file's header (include/<path>.h for src/<path>.c); if the header doesn't declare it, ADD
the prototype there (create the header if it doesn't exist), with the definition's exact signature, or, if the
function isn't decompiled yet, the one the original's callee and callers show; one line per prototype. A function
the original keeps local to one file (`.fn NAME, local` in the asm) is declared static in that file. Only add
lines to headers: never change or remove existing ones. Structs and data externs you need may stay local in your
file; list them in the report. Don't edit configure.py, config/ or other source files.{shared}

Where to look:
- The original assembly: {asm} (find each `.fn NAME`). Read the whole function first.
- Its callers and callees elsewhere in src/ and include/ tell you the types and what it's for; grep for them.
- docs/AGENT_BRIEF.md (attached) has the rules and the compiler idioms this codebase has learned.
- Written functions are already in place: don't move them. A `// TODO: write NAME` comment marks where an
  unwritten one goes.{order}

Tools (run from this directory, each call rebuilds incrementally in a few seconds):
- `python3 check.py {unit} NAME` shows the original instructions (left) next to yours (right); `!!` marks
  differences. `python3 check.py {unit}` lists the file's functions with their match %.

Work in small steps: write a first version, run the check, fix, repeat. Keep the file building: a build error
fails every function in it. Never lower a score: if nothing you try beats a function's starting %, restore its version from ./orig_file.c
(the file as you got it) and say what you tried. Keep every `// NONMATCHING` comment accurate. Check each function you
change against brief rule 17 (the wrong code reviews keep finding). If the file reaches 100%, run
`python3 check.py --final {unit}` until it prints LINKED.

Hard rules:{removed}
- No network: don't fetch, search or clone anything from the web. Everything you need is here: the asm, the
  source, include/, docs/.
- objdiff does not compare data VALUES. For every data object you add or change, compare its bytes against the asm
  .data/.rodata bytes before finishing: `python3 tools/rodatalayout.py UNIT .data` (also `.rodata`, `.sdata`,
  `.sdata2`) shows the original's words next to yours, `|` on rows that differ. Check the argument order of every
  call with more than two arguments against the asm register order.
- Stay inside this directory: don't read or list anything outside it (not ~/ttyd, not other agents' workspaces).
  Scratch files go in ./tmp.
- Work only inside this directory. Start no background or detached processes (no `&`, nohup, setsid, screen,
  tmux, disown) and make no copies of this workspace elsewhere. If you think more parallelism would help, say so in
  REPORT.md and stop; the sandbox blocks writes outside this directory and ends every process when you stop.
- Never `#pragma scheduling <cpu number>` (e.g. 601, 801): it sends the compiler into an endless loop.
- No inline `asm` and no changes to compiler flags. A match produced by pasting assembly is worthless.
- Source comments describe the code and the original, nothing else: no model, tool or agent names, no
  "pass"/"wave"/"attempt" history, no references to REPORT.md or this prompt (put those in REPORT.md).
- `#pragma` only of the kinds this codebase already uses, each with a one-line comment saying why, and only if the
  function doesn't match without it.
- Never delete, rename or change an existing function in the file. Never write an empty or near-empty body as a
  stand-in for real code.
- Use real struct fields where a header declares them; raw offsets only where the field isn't declared, listed
  in the report.

Write REPORT.md in this directory and update it as you go (you can be stopped at any moment). First line
exactly: `@@MODEL_LINE@@`. Then: the function, its
size, its final %, what made it match or what still differs, and any header lines you added.

BEFORE YOU STOP: run `python3 check.py --final {unit}`. It rebuilds and runs the same checks the merge runs. Fix
everything it lists, run it again, and only then finish."""

LINK_PROMPT = """You are one of several agents working on a matching decompilation of Paper Mario: The Thousand-Year Door
(GameCube, PowerPC, Metrowerks C compiler).

YOUR TASK: MAKE {src} (unit {unit}, {part}) LINK. Every function in it already compiles to exactly the original bytes
(100%), but the unit is still `NonMatching` in configure.py: flipped to `Matching`, the build's checksum fails
({why}). The cause is the unit's data: its .rodata/.sdata2/.data/.sdata/.bss come out in a different order, size or
content than the original's, or an object it needs isn't defined. SUCCESS MEANS THE UNIT LINKS: `Matching` in
configure.py and the build's checksums pass. Match percentages don't count, but every function must still be at 100%
when you finish.

Steps:
1. In configure.py change `Object(NonMatching, "{cfg}")` to `Object(Matching, "{cfg}")` (that one line only), run
   `python3 configure.py`, then build: {build}.
2. State the target before changing anything: `python3 tools/link_diag.py {unit}` gives the word-level diff of every
   data section and the first differing address; `python3 tools/rodatalayout.py {unit} [SECTION]` shows the original
   and built layout side by side with symbol names (`|` marks differing rows). Write in REPORT.md what differs:
   which section, which object is extra/missing/out of order.
3. First check the function order: `python3 tools/fn_order_check.py {unit} {src}`. If it prints `want:`/`have:`
   (the order differs), run it again with `--write`, forward-declare whatever the reordered code now calls before
   its definition (in the unit's header if it has one), rebuild, and check again; three of the last four units that
   linked needed only this. Then fix the rest with the brief's linking recipes (docs/AGENT_BRIEF.md: "Linking a unit whose functions are all 100%", "Pool
   recipes", "Where pool entries come from", "Pool and string order"): function order (`python3 tools/fn_order.py
   {unit}`, `--write`), literal-pool order (named vs literal constants, where the named ones are defined), string and
   template order, `__declspec(section ...)`, `#pragma force_active on` for unreferenced tables, uninitialised
   globals declared in the REVERSE of the original's .bss address order, trailing `gap_*` objects. Port missing data
   from the asm ({asm}) and config/G8MJ01/symbols.txt with the original names.
4. Rebuild and re-run link_diag after each change; keep a change only if the diff shrinks. Check functions with
   `python3 check.py {unit}`: none may end below 100%.
5. Done when {done}. Leave it `Matching`. If you can't get there, set it back to `NonMatching`, make sure every function
   is at 100%, and write in REPORT.md exactly what still differs (section, offset, original vs built words) and what you tried.

Only this file and configure.py's one Object line may change, with one exception: function prototypes go in headers
(add lines only; never change or remove existing ones). Don't edit config/ or other source files.

Hard rules:
- No network: don't fetch, search or clone anything from the web.
- objdiff does not compare data VALUES. For every data object you add or change, compare its bytes against the asm
  .data/.rodata bytes before finishing: `python3 tools/rodatalayout.py UNIT .data` (also `.rodata`, `.sdata`,
  `.sdata2`) shows the original's words next to yours, `|` on rows that differ. Check the argument order of every
  call with more than two arguments against the asm register order.
- Stay inside this directory: don't read or list anything outside it (not ~/ttyd, not other agents' workspaces).
  Scratch files go in ./tmp.
- Work only inside this directory. Start no background or detached processes (no `&`, nohup, setsid, screen,
  tmux, disown) and make no copies of this workspace elsewhere. If you think more parallelism would help, say so in
  REPORT.md and stop; the sandbox blocks writes outside this directory and ends every process when you stop.
- Never `#pragma scheduling <cpu number>` (e.g. 601, 801): it sends the compiler into an endless loop.
- No inline `asm` and no changes to compiler flags.
- Source comments describe the code and the original, nothing else: no model, tool or agent names, no
  "pass"/"wave"/"attempt" history, no references to REPORT.md or this prompt (put those in REPORT.md).
- Never delete, rename or change what an existing function does. Reach a named object by its name, never as another
  object's address plus an offset.{hint}

Write REPORT.md in this directory and update it as you go (you can be stopped at any moment). First line
exactly: `@@MODEL_LINE@@`. Then: the target (what
link_diag/rodatalayout showed), each change and its effect, and the final state: LINKED, or what still differs.

BEFORE YOU STOP: run `python3 check.py --final {unit}` and fix everything it lists."""


CAP = int(os.environ.get("CAP", 24))
QUIET_FROM = os.environ.get("QUIET_FROM", "23:00")
QUIET_TO = os.environ.get("QUIET_TO", "05:00")
QUIET_CAP = int(os.environ.get("QUIET_CAP", 6))
START_STAGGER = float(os.environ.get("START_STAGGER", 2))


def cap_now():
    # the tower's permuter needs load under its gate overnight: a lower cap inside the quiet window (which wraps midnight)
    t = time.strftime("%H:%M")
    quiet = (QUIET_FROM <= t or t < QUIET_TO) if QUIET_FROM > QUIET_TO else (QUIET_FROM <= t < QUIET_TO)
    return QUIET_CAP if quiet else CAP


REMOVED = []  # (unit, function, why): functions reviews took out; agents must not write them back
HINTS = {}  # unit -> hint text
WHY = {}  # source path -> why the unit does not link
slot_lock = threading.Lock()
DEADLINE = None  # set by main() from $DEADLINE: epoch seconds after which no new agent starts

# Resource limits: per agent (its sandbox's scope) and for every agent together (one slice, set at start, runtime only)
SLICE = "ttyd-agents.slice"
SLICE_PROPS = os.environ.get("SLICE_PROPS", "CPUQuota=1000% MemoryMax=40G TasksMax=8192")
MEM_MAX = os.environ.get("MEM_MAX", "12G")
TASKS_MAX = os.environ.get("TASKS_MAX", "1024")
# What the sandbox shows of $HOME (everything else under it is an empty tmpfs): the agent CLI and node, the venv's
# python, and the shared read-only parts of the checkout the workspaces link to. No credentials live in any of these.
HOME_RO = [".local/bin", ".local/lib/node_modules", ".local/share/uv/python", ".nvm/versions/node",
           "ttyd/orig", "ttyd/.venv", "ttyd/build/compilers", "ttyd-pylib"]
# Host sockets the sandbox replaces with /dev/null: docker (the docker group is root), the system bus. The user bus,
# systemd's private socket and any X/ICE authority are under /run/user/<uid>, which becomes an empty tmpfs.
HIDE_SOCKETS = ["/run/docker.sock", "/run/dbus/system_bus_socket"]
KEY_FILE = ""  # set by main(): ~/.config/ttyd-keys/<PFX>, mode 0600
UID = ""  # set by main(): the host user's uid


def ssh(cmd, timeout=1800, check=True):
    """Run cmd under bash on the host. Every call has a deadline: a launcher thread once waited 16 h on a hung compile."""
    return subprocess.run(["ssh", HOST, "bash -c " + shlex.quote(cmd)], capture_output=True, text=True,
                          timeout=timeout, check=check)


def rsync(*args, timeout=600):
    return subprocess.run(["rsync", "-az", *args], check=True, timeout=timeout)


SLOTS = "$HOME/ttyd-agents-slots"
SLOT_STALE_MIN = 360  # a slot file older than any agent's budget plus its finisher is a dead launcher's


def slot_dir():
    return f"{SLOTS}/" + re.sub(r"[^A-Za-z0-9_]", "", "".join(sorted(PFXRE.split("|"))))


def reserve(tag):
    """Take one of this key's agent slots on the host, under a lock every launcher on the key shares, so two launchers
    can't both see a free slot. Running agents without a slot file (an older launcher's) count too. True if taken,
    False if full or the host didn't answer (no answer is no capacity)."""
    d = slot_dir()
    try:
        r = ssh(f"mkdir -p {d} && exec 8>{d}/.lock && flock -w 120 8 || exit 3; "
                f"find {d} -maxdepth 1 -type f -name '*.slot' -mmin +{SLOT_STALE_MIN} -delete; "
                f"n=$(find {d} -maxdepth 1 -type f -name '*.slot' | wc -l); "
                f"p=$(for p in $(pgrep -f 'pi --mode json'); do readlink /proc/$p/cwd; done | grep -E 'ttyd-agents/({PFXRE})' | sort -u | wc -l); "
                f"[ \"$p\" -gt \"$n\" ] && n=$p; "
                f"if [ \"$n\" -lt {cap_now()} ]; then touch {d}/{PFX}{tag}.slot && echo TAKEN; else echo FULL; fi",
                timeout=200, check=False)
    except subprocess.TimeoutExpired:
        return False
    return r.returncode == 0 and r.stdout.strip() == "TAKEN"


def free(tag):
    try:
        ssh(f"rm -f {slot_dir()}/{PFX}{tag}.slot", timeout=60, check=False)
    except subprocess.TimeoutExpired:
        pass


def claim(unit):
    out = subprocess.run(["python3", str(REPO / "tools/claims.py"), "claim", WAVE, unit], capture_output=True, text=True,
                         timeout=60).stdout
    return "CLAIMED" in out


def release(unit):
    subprocess.run(["python3", str(REPO / "tools/claims.py"), "release", WAVE, unit], capture_output=True, timeout=60)


def paths(tag):
    """(workspace, output dir) as the host's shell spells them, and the same relative to ~ for rsync."""
    name = f"{PFX}{tag}"
    return f"$HOME/ttyd-agents/{name}", f"$HOME/ttyd-agents-out/{name}", f"ttyd-agents/{name}", f"ttyd-agents-out/{name}"


def priv(tag):
    """The launcher's own directory for this agent: the sandbox's /tmp and the agent CLI's settings copy. Outside the
    workspace and hidden from every sandbox, so the agent can't swap either for a link before the next pass binds it."""
    return f"$HOME/ttyd-agents-priv/{PFX}{tag}"


def scope(tag, step):
    return "ttyd-" + re.sub(r"[^A-Za-z0-9_.-]", "_", f"{PFX}{tag}") + "-" + step


def sandbox(tag, step, mode):
    """The command prefix for one sandboxed step, expanded by the host's bash.

    mode "agent": the workspace writable, a private /tmp and copy of the agent CLI's settings (priv()), and the key
    as a file on a tmpfs (fd 9 must be the key file). mode "collect": the workspace read-only and the output dir the
    only writable place. Both: the whole filesystem read-only, $HOME and /run/user/<uid> empty tmpfs (credentials,
    the user bus, systemd's socket, X authority) with HOME_RO mounted back, docker and system bus sockets hidden, a
    clean environment, and new pid/ipc/uts namespaces so nothing started inside outlives the step or signals outside."""
    ws, out, _, _ = paths(tag)
    run_user = f"/run/user/{UID}"
    env = {"HOME": "$HOME", "USER": "$USER", "LANG": "C.UTF-8", "TMPDIR": "/tmp", "TTYD_BASE": BASE,
           "PYTHONPATH": "$HOME/ttyd-pylib",
           "PATH": "$HOME/.local/bin:$HOME/ttyd/.venv/bin:$(dirname \"$(readlink -f \"$(command -v node)\")\"):/usr/local/bin:/usr/bin:/bin"}
    a = [f"systemd-run --user --scope --quiet --collect --unit={scope(tag, step)} --slice={SLICE}",
         f"-p CPUQuota={CPU_QUOTA} -p MemoryMax={MEM_MAX} -p TasksMax={TASKS_MAX}",
         "bwrap --die-with-parent --new-session --unshare-pid --unshare-ipc --unshare-uts --clearenv"]
    a += [f'--setenv {k} "{v}"' for k, v in env.items()]
    a += ["--ro-bind / / --dev /dev --proc /proc", '--tmpfs "$HOME"', f"--tmpfs {run_user}"]
    a += [f"$([ -e {s} ] && echo --ro-bind /dev/null {s})" for s in HIDE_SOCKETS]
    a += [f'--ro-bind-try "$HOME/{p}" "$HOME/{p}"' for p in HOME_RO]
    a.append(f"--ro-bind {BASE} {BASE}")
    if mode == "agent":
        a += [f"--bind {ws} {ws} --bind {priv(tag)}/tmp /tmp", f'--bind {priv(tag)}/pi-agent "$HOME/.pi/agent"',
              f"--file 9 {run_user}/key", "--chdir", ws, "--"]
        # the key goes from the tmpfs file into the agent CLI's environment, never onto a command line
        a.append(shlex.quote(f'export {KEY_ENV}="$(cat {run_user}/key)"; exec "$@"').join(["/bin/sh -c ", " sh"]))
    else:
        a += [f"--ro-bind {ws} {ws} --tmpfs /tmp --bind {out} {out}", "--chdir", out, "--"]
    return " ".join(a)


def bind_check(tag, mode):
    """Shell test run on the host before a sandboxed step: every writable bind source is a real directory, not a link."""
    ws, out, _, _ = paths(tag)
    dirs = [ws, f"{priv(tag)}/tmp", f"{priv(tag)}/pi-agent"] if mode == "agent" else [out]
    test = " && ".join(f"[ -d {d} ] && [ ! -L {d} ]" for d in dirs)
    return f"{{ {test} || {{ echo 'a bind source is a link or missing' >&2; exit 4; }}; }}"


def agent_cmd(tag, secs, args, step):
    """Run the agent CLI in the sandbox for at most secs. Its stdout and stderr go through pipes to files in the output
    dir, which the sandbox can't reach: the agent can add lines to its event log but not change or remove any."""
    ws, out, _, _ = paths(tag)
    return (f"cd {ws} && {bind_check(tag, 'agent')} && timeout -k 30 {int(secs)} {sandbox(tag, step, 'agent')} "
            f"pi --mode json --session-dir {ws}/.pi-sessions --model {MODEL} {args} </dev/null 9<{KEY_FILE} "
            f"2> >(cat >> {out}/stderr.log) | cat >> {out}/events.jsonl; echo rc=${{PIPESTATUS[0]}}")


def box_cmd(tag, step, cmd, secs=600):
    """A non-agent step on the workspace (check.py, rel_link.py): the same sandbox, since it runs the agent's files."""
    ws, _, _, _ = paths(tag)
    return (f"cd {ws} && {bind_check(tag, 'agent')} && timeout -k 30 {secs} {sandbox(tag, step, 'agent')} "
            f"/bin/sh -c {shlex.quote(cmd)} 9</dev/null")


# Programs the reaper kills in a workspace: a person's shell that happens to sit there is left alone.
REAP_NAMES = "wibo|ninja|python*|pi|node|bwrap|timeout|sh|objdiff-cli|dtk|nice|mwcceppc*"


def reap_script(ws, proc="/proc"):
    """Kill what still runs with its cwd in workspace ws. Nothing happens unless ws exists: `cd` failing must not
    turn the match into every process."""
    if not ws.strip() or ws.strip() == "/":
        return "true"
    return (f"cd {ws} 2>/dev/null || exit 0; d=$(pwd -P); [ -n \"$d\" ] && [ \"$d\" != / ] && [ \"$d\" != \"$(cd ~ && pwd -P)\" ] || exit 0; "
            f"for p in {proc}/[0-9]*; do "
            f"case \"$(readlink $p/cwd 2>/dev/null)\" in \"$d\"|\"$d\"/*) ;; *) continue;; esac; "
            f"case \"$(cat $p/comm 2>/dev/null)\" in {REAP_NAMES}) "
            f"[ \"${{p##*/}}\" != $$ ] && kill -9 \"${{p##*/}}\" 2>/dev/null;; esac; done; true")


def reap(tag):
    """Stop this agent's sandbox scopes (everything in them dies), then kill stragglers in the workspace."""
    ws, _, _, _ = paths(tag)
    try:
        ssh(f"systemctl --user stop '{scope(tag, '')}*' 2>/dev/null; " + reap_script(ws), timeout=120, check=False)
    except subprocess.TimeoutExpired:
        pass


def host_state():
    """Containers and user units (other than the agents' own scopes): an agent that got out would show up here."""
    try:
        r = ssh("docker ps -q --no-trunc 2>/dev/null | sed 's/^/container /'; "
                "systemctl --user list-units --all --plain --no-legend 2>/dev/null | awk '{print \"unit \" $1}' | grep -v ' ttyd-'",
                timeout=60, check=False)
        return set(r.stdout.split("\n")) - {""}
    except subprocess.TimeoutExpired:
        return set()


def install_key(key):
    """Put the key in a 0600 file on the host (over stdin, written to a temp file and renamed into place)."""
    subprocess.run(["ssh", HOST, "umask 077 && mkdir -p ~/.config/ttyd-keys && chmod 700 ~/.config/ttyd-keys && "
                    "t=$(mktemp ~/.config/ttyd-keys/.new.XXXXXX) && cat > \"$t\" && chmod 600 \"$t\" && "
                    f"mv -f \"$t\" {KEY_FILE}"], input=key, text=True, check=True, timeout=60)


def MODEL_LINE_FIX(text):
    return text.replace("@@MODEL_LINE@@", MODEL_LINE)


def prepare(tag, spec):
    unit = spec["unit"]
    ws, out, rws, _ = paths(tag)
    # Shared, never-modified parts are links, not copies: the disc (orig/), the venv, the compilers and the build
    # tools (~190 MB per workspace otherwise). Build outputs stay per workspace: each agent builds its own code.
    # No permuter entry points (it was most of the tower's CPU); DOL asm linked from the base.
    ssh(f"mkdir -p ~/ttyd-agents && rsync -a --delete --delete-excluded --exclude orig/ --exclude .venv/ --exclude build/compilers/ "
        f"--exclude build/tools/ --exclude build/G8MJ01/asm/ --exclude 'tools/perm_*.py' --exclude .git/ {BASE}/ {ws}/ "
        f"&& ln -sfn {BASE}/build/G8MJ01/asm {ws}/build/G8MJ01/asm && ln -sfn ~/ttyd/orig {ws}/orig && ln -sfn ~/ttyd/.venv {ws}/.venv "
        f"&& ln -sfn ~/ttyd/build/compilers {ws}/build/compilers && ln -sfn {BASE}/build/tools {ws}/build/tools")
    src = spec['src']
    # the agent CLI gets its settings and model list only (no credential store, no shared session history)
    pv = priv(tag)
    ssh(f"cp {ws}/tools/agent_check.py {ws}/check.py && cp {ws}/{src} {ws}/orig_file.c && mkdir -p {ws}/tmp "
        # the launcher's dirs (output, private /tmp and settings), outside every sandbox's view, emptied for this run
        f"&& mkdir -p {out} {pv} && find {out} {pv} -mindepth 1 -delete && mkdir {pv}/tmp {pv}/pi-agent "
        f"&& cp ~/.pi/agent/settings.json ~/.pi/agent/models.json {pv}/pi-agent/")
    p = HERE / tag
    p.mkdir(exist_ok=True)
    if spec["dol"]:
        asm = f"build/G8MJ01/asm/{unit}.s"
        part = "part of the game's main executable"
        order = (" This compiler emits a file's functions in REVERSE source order (brief rule 18): a function you write goes"
                 " at its placeholder; one with no placeholder goes where the reverse of the asm's order puts it among the file's"
                 " existing functions.")
    else:
        mod, rel = unit.split("/", 1)
        asm = f"build/G8MJ01/{mod}/asm/{rel}.s"
        part = "part of a REL, a module the game loads per area"
        order = (" (This module is built without deferred inlining, so its functions are emitted in source order.)"
                 if spec["source_order"] else "")
    if spec.get("start_c"):
        rsync(spec["start_c"], f"{HOST}:{rws}/{src}")
        rsync(spec["start_h"], f"{HOST}:{rws}/{spec['hdr']}")
        ssh(f"cp {ws}/{src} {ws}/orig_file.c")
    if spec.get("mode") == "link":
        cfg = src[len("src/"):] if spec["dol"] else src.rsplit("/src/", 1)[1]
        why = next((v for k, v in WHY.items() if src.endswith("/" + k) or src == k), "checksum mismatch")
        build = ("`ninja` (it links MarioSt.dol and checks it; `FAILED: build/G8MJ01/ok` or a sha1 mismatch means not linked)"
                 if spec["dol"] else "`ninja`, then `python3 tools/rel_link.py " + unit + "`")
        done = ("`ninja` finishes with the checksum passing" if spec["dol"]
                else "`python3 tools/rel_link.py " + unit + "` prints LINKED")
        hint = ("\n- HINT for this file: " + HINTS[unit]) if unit in HINTS else ""
        if spec.get("prev"):
            for i, pr in enumerate(spec["prev"]):
                rsync(str(REPO / pr), f"{HOST}:{rws}/prev_REPORT_{i + 1}.md")
            hint += ("\n- An earlier agent already tried to link this unit; its notes are in "
                     + " ".join(f"prev_REPORT_{i + 1}.md" for i in range(len(spec["prev"])))
                     + ". Read them first and don't repeat what didn't work.")
        (p / "PROMPT.md").write_text(MODEL_LINE_FIX(LINK_PROMPT.format(src=src, unit=unit, part=part, why=why, cfg=cfg, build=build,
                                                        asm=asm, done=done, hint=hint)) + "\n")
        rsync(str(p / "PROMPT.md"), f"{HOST}:{rws}/PROMPT.md")
        return
    shared = ""
    if "/common/" in src:
        shared = (f" This is a SHARED source (rels/common) built into several modules; check it as {unit}, and keep the "
                  "-DUNIT_DATA_SYM / -DUNIT_REF_* macros.")
    funcs = "\n".join(f"- `{n}` ({sz} bytes, {sc:.2f}%)" for n, sz, sc in spec["open"])
    todo = (f"Still unwritten (at a `// TODO: write` placeholder): {', '.join(spec['todo'])}. Write these from the asm.\n"
            if spec["todo"] else "")
    note = ""
    if spec.get("start_c"):
        note = ("\nThis file already holds an earlier agent's work (it's what you start from, and what orig_file.c is): several "
                "functions are written. The scores listed are the repository's, before that work; run `python3 check.py "
                f"{unit}` first for where each function stands now. Finish the rest, and keep the file's data sections "
                "(.data/.rodata) as close to the original as they were: the earlier work was held back because a data section "
                "scored lower than the repository's.")
    if spec.get("mode") == "dataport":
        rsync(str(REPO / spec["seed"]), f"{HOST}:{rws}/salvaged_version.c")
        fn = spec["open"][0][0]
        note += (f"\n./salvaged_version.c is an earlier agent's version of `{fn}` that never landed: {spec['seed_why']}."
                 " YOUR GOAL: get it into the file so the function scores that high AND no data section (.rodata/.sdata2/"
                 ".data/.sdata) and no other function of the unit ends lower than it started. The section drops because the"
                 " version's literals/strings/constants change the unit's pool or data layout, so porting the unit's data is"
                 " in scope (this overrides \"change only these functions\" for the unit's data objects and function order):"
                 " name or reorder constants and strings, define missing data objects with their original names"
                 " (config/G8MJ01/symbols.txt), move functions into the original order (`python3 tools/fn_order_check.py"
                 f" {unit} {src}`). `python3 tools/rodatalayout.py {unit} [SECTION]` shows the original and built layout side"
                 " by side (`|` marks differing rows) and `python3 tools/link_diag.py {unit}` the word-level data diff; the"
                 " brief's linking recipes (\"Pool recipes\", \"Where pool entries come from\", \"Pool and string order\") are"
                 " the methods. Use `python3 check.py {unit}` for function scores and rodatalayout/link_diag for the sections"
                 " before and after. If the unit then links (all functions 100% and the data identical), that's a bonus: flip"
                 " it to Matching in configure.py (that one Object line is the only configure.py change allowed) only if `ninja`"
                 " passes its checksum. Check the version's code against the"
                 " asm as carefully as new code (compare direction: `cror eq,gt,eq` is `>=`, `cror eq,lt,eq` is `<=`;"
                 " every read against the store that sets it): it has never been reviewed.")
    if spec.get("mode") == "dataport":
        note += ("\nBefore finishing, for every store in the function you wrote, name the field offset the asm writes"
                 " (stw/stfs rX, OFF(rY)) and confirm your C writes the same field. For every float constant, confirm the"
                 " symbol or address the asm loads. Fuzzy scores can't see either.")
    if spec.get("mode") == "dataport" and spec.get("draft"):
        pass
    elif spec.get("seed") and not spec.get("draft") and spec.get("mode") != "dataport":
        rsync(str(REPO / spec["seed"]), f"{HOST}:{rws}/salvaged_version.c")
        fn = spec["open"][0][0]
        note += (f"\n./salvaged_version.c is an earlier agent's version of `{fn}` (with its comments and any helpers/pragmas"
                 f" it needed) that never landed: {spec['seed_why']}. Start from it: bring it into the file in place of the"
                 " current version, make it build, and fix that problem (the unit's other functions and its data sections"
                 " must not end lower than they started; usually that means keeping the file's existing named constants,"
                 " strings and their order instead of the version's own). Check its code against the asm as carefully as"
                 " new code: it has never been reviewed. Partial scores barely notice wrong compares: `cror eq,gt,eq` is"
                 " `>=` and `cror eq,lt,eq` is `<=`; check every comparison and every read against the store that sets it.")
    if spec.get("draft"):
        rsync(str(REPO / spec["draft"]), f"{HOST}:{rws}/m2c_draft.c")
        fn = spec["open"][0][0]
        if spec.get("seed"):
            rsync(str(REPO / spec["seed"]), f"{HOST}:{rws}/salvaged_version.c")
            note += (f"\n`{fn}` has NO DEFINITION in the file yet. START FROM ./salvaged_version.c: an earlier agent's version of"
                     f" it (with its comments and any helpers/pragmas it needed). It never landed: {spec['seed_why']}."
                     " Put it in the file, make it build, and fix that problem without lowering its score more than you"
                     " must; ./m2c_draft.c (an untyped m2c decompilation of the asm) is there for reference. Check the"
                     " earlier version's code against the asm as carefully as new code: it has never been reviewed.")
        else:
            note += (f"\n`{fn}` has NO DEFINITION in the file yet: you write it from scratch. ./m2c_draft.c is an m2c decompilation"
                     " of its asm (no type context: rename its variables and types to the codebase's structs, fields and the"
                     " file's existing helpers/inlines; don't paste it in as is).")
        note += (" Put the function where the reverse of the asm's"
                 " function order puts it among the file's existing functions, and add it even if it doesn't reach 100%:"
                 " success here is the function compiling in place with as high a score as you can get, every other"
                 " function unchanged. Partial scores barely notice wrong compares, so the code must be right even where"
                 " the score can't tell: m2c's `fcmpo`/`cmpw` + `cror eq,gt,eq` is `>=` and `cror eq,lt,eq` is `<=`; check"
                 " EVERY comparison's direction and signedness, and every read of a variable against the call or store"
                 " that sets it (statement order around calls), against the asm by hand before you finish. A path that"
                 " your conditions make unreachable is a bug.")
    if spec.get("prev"):
        for i, pr in enumerate(spec["prev"]):
            rsync(str(REPO / pr), f"{HOST}:{rws}/prev_REPORT_{i + 1}.md")
        names = " ".join(f"prev_REPORT_{i + 1}.md" for i in range(len(spec["prev"])))
        note += (f"\nEarlier agents already worked on this file (wave 61); their notes are in {names} in this directory. Read them "
                 "first: don't repeat what they say didn't work, and build on what they found. Their work that kept every score "
                 "is already in the file.")
    if "gundan_zako_group2" in unit:
        note += ("\nThis file uses ZakogunEntry from a shared header: use its existing field names (they may not describe this "
                "unit well); don't change or redefine the struct.")
    extra = ""
    if spec["extra"]:
        extra = ("These are all of the file's functions below 80%. If you finish them, go on to its near-misses, closest first: "
                 + ", ".join(f"`{n}` ({sz} bytes, {sc:.2f}%)" for n, sz, sc in spec["extra"]) + ".\n")
    elif spec["others"]:
        extra = ("Other functions in this file are open too (" + ", ".join(spec["others"][:30]) + ("..." if len(spec["others"]) > 30 else "")
                 + "): leave them exactly as they are; other agents may be working on them.\n")
    group = ""
    if spec.get("group"):
        group = ("THIS FILE IS SPLIT BETWEEN SEVERAL AGENTS: the others are writing its other functions in parallel, each in their own "
                 "copy. Write ONLY the functions listed above; leave every other `// TODO: write` placeholder exactly as it is. "
                 "Your goal is each listed function at 100% (`python3 check.py UNIT NAME`); still run `check.py --final` before "
                 "you stop, but its link result needs the other groups too. Don't change shared structs in headers; for a "
                 "field a struct doesn't declare, use a raw offset access with a comment.\n")
    target = spec["open"][0][0] if spec.get("draft") else None
    gone = [(n, why) for u, n, why in REMOVED if u == unit and n != target]
    if target and any(u == unit and n == target for u, n, _ in REMOVED):
        why_t = next(w for u, n, w in REMOVED if u == unit and n == target)
        note += (f"\nA review took an earlier version of `{target}` out because it behaved differently from the original: {why_t}."
                 " Your version must not repeat that.")
    removed = ("".join(f"\n- DON'T WRITE `{n}`: a review took it out because the written version behaved differently from the "
                       f"original ({why}). Leave it unwritten." for n, why in gone)
               + "\n- Reach a named object by its name, never as another object's address plus an offset."
               + ("\n- HINT for this file: " + HINTS[unit] if unit in HINTS else ""))
    (p / "PROMPT.md").write_text(MODEL_LINE_FIX(PROMPT.format(removed=removed, unit=unit, src=src, asm=asm, funcs=funcs, todo=todo, note=note, group=group, part=part, extra=extra,
                                               order=order, shared=shared)) + "\n")
    rsync(str(p / "PROMPT.md"), f"{HOST}:{rws}/PROMPT.md")

def limit(spec):
    if spec.get("mode") == "link":
        return 10800
    if spec.get("draft"):
        return 14400 if spec["open"][0][1] >= 6000 else 10800 if spec["open"][0][1] >= 2500 else 7200
    b = sum(sz for _, sz, _ in spec["open"])
    return 5400 if b <= 2500 else 7200 if b <= 5000 else 10800



def run(tag, unit, src, spec, log):
    ws, out, _, _ = paths(tag)
    t0 = time.time()
    budget = limit(spec)
    left = lambda: budget - (time.time() - t0)
    finish = (f"You stopped before finishing. Do only this: run python3 check.py --final {unit}, fix what it lists, "
              f"and write REPORT.md as PROMPT.md describes. Don't start other work.")
    r = ssh(agent_cmd(tag, budget, "@PROMPT.md @docs/AGENT_BRIEF.md "
                      "'Do the task described in the attached PROMPT.md, following docs/AGENT_BRIEF.md.'", "run"),
            timeout=budget + 600, check=False)
    rcs = r.stdout.strip()
    names = {n for n, _, _ in spec["open"]}
    for k in range(3):  # continuation passes, moderate
        if spec.get("mode") == "link":
            cfg = src[len("src/"):] if spec["dol"] else src.rsplit("/src/", 1)[1]
            linked = ssh(f"grep -qF 'Object(Matching, \"{cfg}\"' {ws}/configure.py && echo yes", timeout=60, check=False)
            if linked.stdout.strip() == "yes" or left() < 1800 or k >= 2:
                break
            more = (f"Keep going: you still have about {int(left() // 60)} minutes and the unit doesn't link yet. Continue as "
                    f"PROMPT.md describes (re-run tools/link_diag.py {unit} for where it stands), keep REPORT.md current, and "
                    f"run python3 check.py --final {unit} before you stop.")
            log(f"{tag} link continuation {k + 1}, {int(left())}s left")
        else:
            c = ssh(box_cmd(tag, f"check{k}", f"python3 check.py {unit}"), timeout=700, check=False)
            still = [l.split()[-1] for l in c.stdout.splitlines()[1:]
                     if l.split() and l.split()[-1] in names and not l.strip().startswith("100.00%")]
            if not still or left() < 900:
                break
            more = (f"Keep going: you still have about {int(left() // 60)} minutes. These functions are still below 100%: "
                    f"{', '.join(still)}. Continue with them as PROMPT.md describes (same rules; never lower a score), "
                    f"keep REPORT.md current, and run python3 check.py --final {unit} before you stop.")
            log(f"{tag} continuation {k + 1}: {len(still)} open, {int(left())}s left")
        secs = left()  # measured after the check, so the pass can't overrun the budget
        r2 = ssh(agent_cmd(tag, secs, "-c " + shlex.quote(more), f"cont{k}"), timeout=secs + 600, check=False)
        rcs += " cont" + r2.stdout.strip()
    done = ssh(f"test -s {ws}/REPORT.md && grep -q -- '--final' {out}/events.jsonl && echo yes", timeout=60, check=False)
    if done.stdout.strip() != "yes":
        r3 = ssh(agent_cmd(tag, 900, "-c " + shlex.quote(finish), "finish"), timeout=1500, check=False)
        rcs += " finisher" + r3.stdout.strip()
    ssh(box_cmd(tag, "link", f"python3 tools/rel_link.py {unit} > link.txt 2>&1"), timeout=700, check=False)
    return rcs


def collect(tag, spec):
    """Gather the results with tools/agent_collect.sh from the base, in the collect sandbox; archive the events and
    copy the output dir back (no links followed). Returns the audit's exit status (0 clean)."""
    ws, out, _, rout = paths(tag)
    c = ssh(f"cd {out} && {bind_check(tag, 'collect')} && timeout -k 30 600 {sandbox(tag, 'collect', 'collect')} "
            f"bash {BASE}/tools/agent_collect.sh {BASE} {ws} {out} {spec['src']} {spec.get('mode', '')}; echo audit=$?; "
            f"mkdir -p {ARCHIVE}/{tag} && zstd -q -3 -f {out}/events.jsonl -o {ARCHIVE}/{tag}/events.jsonl.zst",
            timeout=900, check=False)
    rsync("--no-links", "--exclude", "events.jsonl", f"{HOST}:{rout}/", str(HERE / tag) + "/")
    m = re.search(r"audit=(\d+)", c.stdout)
    return int(m.group(1)) if m else -1


def parse_deadline(s, now=None):
    """$DEADLINE as epoch seconds: "YYYY-MM-DD HH:MM", or "HH:MM" meaning its next occurrence after now."""
    if not s:
        return None
    now = now or time.time()
    if len(s) > 5:
        return time.mktime(time.strptime(s, "%Y-%m-%d %H:%M"))
    h, m = map(int, s.split(":"))
    t = time.localtime(now)
    d = time.mktime((t.tm_year, t.tm_mon, t.tm_mday, h, m, 0, 0, 0, -1))
    return d if d > now else d + 86400


def main(wave, here, base, model, key_env, model_line, specfile="units.json", removed=None, hints=None, why=None,
         cpu_quota="200%", archive=None):
    """Run a wave. here: the wave's scratch directory (spec, prompts and results live there). removed/hints/why: paths
    to removed.json (unit, function, why), hints.json (unit -> text) and an autolink failure list, if any.
    $BASE and $ARCHIVE override base and archive."""
    global HERE, WAVE, BASE, MODEL, KEY_ENV, MODEL_LINE, CPU_QUOTA, ARCHIVE, SPEC, REMOVED, HINTS, WHY, KEY_FILE, UID, DEADLINE
    HERE, WAVE, MODEL, KEY_ENV, MODEL_LINE, CPU_QUOTA = Path(here), wave, model, key_env, model_line, cpu_quota
    BASE = re.sub(r"^~(?=/)", "$HOME", os.environ.get("BASE") or base)
    ARCHIVE = os.environ.get("ARCHIVE") or archive or "~/ttyd-archive/scratch/" + HERE.name
    SPEC = json.load(open(HERE / os.environ.get("SPECFILE", specfile)))
    if os.environ.get("ONLY"):
        SPEC = [x for x in SPEC if x["unit"] in os.environ["ONLY"].split()]
    if os.environ.get("PART"):
        k, n = map(int, os.environ["PART"].split("/"))
        SPEC = SPEC[k::n]
    REMOVED = [tuple(x) for x in json.load(open(removed))] if removed else []
    HINTS = json.load(open(hints)) if hints else {}
    WHY = ({l.split()[1].rstrip(":"): l.split(None, 2)[2].strip() for l in open(why) if len(l.split()) > 2} if why else {})
    DEADLINE = parse_deadline(os.environ.get("DEADLINE", ""))
    if not PFX:
        sys.exit("PFX not set")
    if len(os.environ.get("HOSTS", HOST).replace(",", " ").split()) > 1:
        sys.exit("one host per launcher (slots are counted per host): set HOST")
    KEY_FILE = f"~/.config/ttyd-keys/{PFX.rstrip('_')}"
    key = os.environ.get("KEY") or sys.exit("KEY not set")
    install_key(key)
    UID = ssh("id -u", timeout=60).stdout.strip()
    ssh(f"systemctl --user set-property --runtime {SLICE} {SLICE_PROPS}", timeout=60)
    lock = threading.Lock()

    def log(msg):
        with lock:
            print(time.strftime("%H:%M:%S"), msg, flush=True)

    q = queue.Queue()
    for s in SPEC:
        q.put(s)

    def worker():
        while True:
            try:
                s = q.get_nowait()
            except queue.Empty:
                return
            tag = s["tag"]
            slot = claimed = started = False
            try:
                if DEADLINE and time.time() >= DEADLINE:
                    log(f"{tag} skipped: past the deadline")
                    continue
                with slot_lock:  # one reservation at a time from this launcher; the host's lock covers the others
                    while not reserve(tag):
                        time.sleep(60)
                slot = True
                if not claim(s["unit"]):
                    log(f"{tag} skipped: {s['unit']} is claimed by another wave")
                    continue
                claimed = True
                reap(tag)  # a scope left from an earlier run of this tag would block the new one's name
                prepare(tag, s)
                started = True
                before = host_state()
                t0 = time.time()
                rcs = run(tag, s["unit"], s["src"], s, log)
                log(f"{tag} finished in {time.time() - t0:.0f}s: {rcs}")
                new = host_state() - before
                if new:
                    (HERE / tag / "host_changes.txt").write_text("\n".join(sorted(new)) + "\n")
                    log(f"{tag} NEW ON HOST while it ran (another session may have started these): {', '.join(sorted(new))}")
            except Exception as e:  # including every ssh deadline (subprocess.TimeoutExpired) and failed step
                log(f"{tag} ERROR {e}")
                if claimed and not started:
                    release(s["unit"])
            finally:
                if started:
                    try:
                        audit = collect(tag, s)
                        if audit != 0:
                            log(f"{tag} AUDIT FLAGGED (exit {audit}; see audit.txt)")
                            rsync(f"{HOST}:{ARCHIVE.replace('~/', '', 1)}/{tag}/events.jsonl.zst", str(HERE / tag) + "/")
                    except Exception as e:
                        log(f"{tag} COLLECT ERROR {e}")
                if claimed:
                    reap(tag)
                if slot:
                    free(tag)

    log(f"{len(SPEC)} files, {WORKERS} at a time")
    threads = [threading.Thread(target=worker) for _ in range(WORKERS)]
    for th in threads:
        th.start()
        time.sleep(START_STAGGER)  # stagger the workspace copies
    for th in threads:
        th.join()
    c = ssh(f"python3 {BASE}/tools/tower_check.py", timeout=600, check=False)
    log("all done; tower_check: " + (c.stdout.strip() or "clean"))


if __name__ == "__main__":
    sys.exit("run a wave's config (scratch/waveNN/launch.py), which calls main(...)")
