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
import hashlib, json, os, queue, re, shlex, subprocess, sys, threading, time
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
MEM_HIGH = os.environ.get("MEM_HIGH", "10G")
SWAP_MAX = os.environ.get("SWAP_MAX", "1G")
TASKS_MAX = os.environ.get("TASKS_MAX", "1024")
# Disk: a watchdog stops an agent's scopes when its workspace, private and output dirs together pass DISK_MB; the
# event and stderr pipes are cut at these sizes per pass (the largest event log so far was 85 MB)
DISK_MB = int(os.environ.get("DISK_MB", 4096))
DISK_INODES = int(os.environ.get("DISK_INODES", 500000))
DISK_FLOOR_GB = int(os.environ.get("DISK_FLOOR_GB", 100))  # free space on the home filesystem: under it, launches pause
INODE_FLOOR = int(os.environ.get("INODE_FLOOR", 10_000_000))
EVENTS_MAX = 512 * 2**20  # per agent, over all its passes
STDERR_MAX = 64 * 2**20
TAKE_MAX = "64m"  # largest file copied back from the output dir
# every sandboxed step's scope name suffix: the reaper stops exactly these
STEPS = ["run", "cont0", "cont1", "cont2", "check0", "check1", "check2", "finish", "link", "collect"]
# What the sandbox shows of $HOME (everything else under it is an empty tmpfs): the agent CLI and node, the venv's
# python, and the shared read-only parts of the checkout the workspaces link to. No credentials live in any of these.
HOME_RO = [".local/bin", ".local/lib/node_modules", ".local/share/uv/python", ".nvm/versions/node",
           "ttyd/orig", "ttyd/.venv", "ttyd/build/compilers", "ttyd-pylib"]
# Host sockets the sandbox replaces with /dev/null: docker (the docker group is root), the system bus. The user bus,
# systemd's private socket and any X/ICE authority are under /run/user/<uid>, which becomes an empty tmpfs.
HIDE_SOCKETS = ["/run/docker.sock", "/run/dbus/system_bus_socket"]
KEY_FILE = ""  # set by main(): ~/.config/ttyd-keys/<PFX>, mode 0600
KEY_HASH = ""  # set by main(): names this key's slot directory, whatever PFXRE says
UID = ""  # set by main(): the host user's uid
NODE_BIN = ""  # set by main(): the host's node directory (absolute; the launcher refuses to start without it)


def ssh(cmd, timeout=1800, check=True):
    """Run cmd under bash on the host. Every call has a deadline: a launcher thread once waited 16 h on a hung compile."""
    return subprocess.run(["ssh", HOST, "bash -c " + shlex.quote(cmd)], capture_output=True, text=True,
                          timeout=timeout, check=check)


def rsync(*args, timeout=600):
    return subprocess.run(["rsync", "-az", *args], check=True, timeout=timeout)


SLOTS = "$HOME/ttyd-agents-slots"
# Present while the disk floor has tripped: no launcher starts an agent or a pass until a person removes it (why.txt
# inside says what tripped and which scope the watchdog stopped)
PAUSED = f"{SLOTS}/PAUSED"
COLLECT_SLICE = "ttyd-collect.slice"  # collect steps: outside the agents' slice, so nothing aimed at agents stops them
SLOT_STALE_MIN = 360  # a slot file older than any agent's budget plus its finisher is a dead launcher's


def slot_dir():
    """One slot pool for every key: CAP and QUIET_CAP bound the tower's total agents (the permuter needs the room).
    Slot files carry the key's hash, so per-key counts can still be read off the names."""
    return f"{SLOTS}/all"


def slot_file(tag):
    return f"{slot_dir()}/{KEY_HASH}_{PFX}{tag}.slot"


def reserve(tag):
    """Take one of the tower's agent slots, under a lock every launcher shares, so two launchers can't both see a free
    slot. Agents in progress are the union of slot files and active agent scopes (an agent can't create a scope).
    Returns "TAKEN", "FULL" or "PAUSED" (the disk floor; checked inside the lock, so no slot is handed out after a
    pause); no answer from the host counts as "FULL"."""
    d = slot_dir()
    try:
        r = ssh(f"mkdir -p {d} && exec 8>{d}/.lock && flock -w 120 8 || exit 3; "
                f"[ -e {PAUSED} ] && {{ echo PAUSED; exit 0; }}; "
                f"find {d} -maxdepth 1 -type f -name '*.slot' -mmin +{SLOT_STALE_MIN} -delete; "
                f"u=$(systemctl --user list-units --plain --no-legend 'ttyd-*-run.scope' 'ttyd-*-cont?.scope' 'ttyd-*-finish.scope') || exit 3; "
                f"n=$({{ find {d} -maxdepth 1 -type f -name '*.slot' -printf '%f\\n' | sed 's/^[^_]*_//; s/[.]slot$//'; "
                f"printf '%s\\n' \"$u\" | awk '{{print $1}}' | sed -E 's/^ttyd-//; s/-[a-z]+[0-9]?[.]scope$//'; }} | grep . | sort -u | wc -l); "
                f"if [ \"$n\" -lt {cap_now()} ]; then touch {slot_file(tag)} && echo TAKEN; else echo FULL; fi",
                timeout=200, check=False)
    except subprocess.TimeoutExpired:
        return "FULL"
    out = r.stdout.strip()
    return out if r.returncode == 0 and out in ("TAKEN", "PAUSED") else "FULL"


def paused():
    """The pause marker's reason, or "" if launches aren't paused (no answer counts as paused)."""
    try:
        r = ssh(f"[ -e {PAUSED} ] && {{ cat {PAUSED}/why.txt 2>/dev/null; echo PAUSED; }} || echo RUNNING", timeout=60, check=False)
    except subprocess.TimeoutExpired:
        return "the host didn't answer"
    out = r.stdout.strip()
    return "" if out == "RUNNING" else (out or "the host didn't answer")


def free(tag):
    try:
        ssh(f"rm -f {slot_file(tag)}", timeout=60, check=False)
    except subprocess.TimeoutExpired:
        pass


def claim(unit):
    out = subprocess.run(["python3", str(REPO / "tools/claims.py"), "claim", WAVE, unit], capture_output=True, text=True,
                         timeout=60).stdout
    return "CLAIMED" in out


def release(unit):
    try:
        subprocess.run(["python3", str(REPO / "tools/claims.py"), "release", WAVE, unit], capture_output=True, timeout=60)
    except (OSError, subprocess.TimeoutExpired):
        pass


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
           "PATH": f"$HOME/.local/bin:$HOME/ttyd/.venv/bin:{NODE_BIN}:/usr/local/bin:/usr/bin:/bin"}
    a = [f"systemd-run --user --scope --quiet --collect --unit={scope(tag, step)} --slice={SLICE if mode == 'agent' else COLLECT_SLICE}",
         f"-p CPUQuota={CPU_QUOTA} -p MemoryMax={MEM_MAX} -p MemoryHigh={MEM_HIGH} -p MemorySwapMax={SWAP_MAX}",
         f"-p TasksMax={TASKS_MAX}",
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


def scope_stop(tag, step):
    """Bounded stop/retry/KILL with a verified inactive state; every failure survives in limits.txt."""
    _, out, _, _ = paths(tag)
    unit = f"{scope(tag, step)}.scope"
    return (f"scope_inactive() {{ local state rc; "
            f"state=$(timeout -k 1 5 systemctl --user is-active {unit} 2>/dev/null); rc=$?; "
            f"case $state:$rc in inactive:3|failed:3|inactive:0|failed:0) return 0;; esac; return 1; }}; "
            f"stop_scope() {{ local attempt; for attempt in 1 2; do "
            f"timeout -k 1 5 systemctl --user stop {unit} 2>/dev/null || "
            f"echo '{step}: scope stop failed' >> {out}/limits.txt; "
            f"scope_inactive && return 0; done; "
            f"echo '{step}: scope still owned after stop retries: escalating to KILL' >> {out}/limits.txt; "
            f"timeout -k 1 5 systemctl --user kill --kill-who=all --signal=KILL {unit} 2>/dev/null || "
            f"echo '{step}: scope KILL failed' >> {out}/limits.txt; "
            f"timeout -k 1 5 systemctl --user stop {unit} 2>/dev/null || "
            f"echo '{step}: scope stop after KILL failed' >> {out}/limits.txt; "
            f"for attempt in 1 2 3; do scope_inactive && return 0; sleep 1; done; "
            f"echo '{step}: could not verify scope inactive after KILL' >> {out}/limits.txt; return 1; }}; ")


def watchdog(tag, step):
    """A host-side loop beside one sandboxed step, every 10 s, while the step's shell ($$) lives and its scope is active:
    - the agent's dirs (workspace, private, output) over DISK_MB or DISK_INODES (du's sum; files vanishing mid-scan
      are normal build churn), or a directory in them it can't read or search (it would hide its size): notes it in the
      output dir's limits.txt and stops the step's scope;
    - the home filesystem under DISK_FLOOR_GB / INODE_FLOOR: the first watchdog to see it creates PAUSED (no launcher
      starts another agent or pass), stops only the scope holding the most deleted-but-open space
      (tools/floor_stop.py), and says so in PAUSED/why.txt.
    run() reads limits.txt and PAUSED, and starts no later step once either exists."""
    ws, out, _, _ = paths(tag)
    dirs = f"{ws} {priv(tag)} {out}"
    unit = f"{scope(tag, step)}.scope"
    # its own statement (an `&` after `a && b` would background the whole list), no file descriptors of the ssh
    # session (or ssh would wait for it), its cwd outside the workspace; the step kills it and all its descendants
    # A failed query is neither proof of an active owner nor proof it ended. Three consecutive failures trip the
    # owning step fail-closed; a successful active/inactive answer resets the counter. Recheck after every scan.
    owner_check = 'wd_owner || { [ "$owner_errors" -ge 3 ] && break; continue; }; '
    busy, done = f"{out}/watchdog-{step}.busy", f"{out}/watchdog-{step}.done"
    # Teardown freezes the watchdog before checking this flag, so it cannot race with entering an action.
    # It requests loop exit before resuming an action, then waits for that action to finish.
    return (f"rm -f {busy} {done}; ( cd /; {scope_stop(tag, step)}owner_errors=0; wd_owner() {{ local state query_rc; "
            f"state=$(timeout -k 1 5 systemctl --user is-active {unit} 2>/dev/null); query_rc=$?; "
            f"case $state:$query_rc in active:0|activating:0|reloading:0|deactivating:0|activating:3|reloading:3|deactivating:3) owner_errors=0; return 0;; "
            f"inactive:3|failed:3|inactive:0|failed:0) owner_errors=0; return 1;; esac; "
            f"owner_errors=$((owner_errors + 1)); [ $owner_errors -lt 3 ] && return 1; "
            f"touch {busy}; echo '{step}: owner scope query failed 3 consecutive times: stopping' >> {out}/limits.txt; "
            f"stop_scope; return 1; }}; "
            f"while [ ! -e {done} ] && kill -0 $$ 2>/dev/null && sleep 10 && kill -0 $$ 2>/dev/null; do {owner_check}"
            f"dm=$(LC_ALL=C du -smx {dirs} 2>&1); di=$(LC_ALL=C du -sx --inodes {dirs} 2>&1); "
            f"m=$(printf '%s\\n' \"$dm\" | awk '$1 ~ /^[0-9]+$/ {{s+=$1}} END{{print s+0}}'); "
            f"i=$(printf '%s\\n' \"$di\" | awk '$1 ~ /^[0-9]+$/ {{s+=$1}} END{{print s+0}}'); "
            f"denied=$(printf '%s\\n' \"$dm\" \"$di\" | awk '/Permission denied/ && !/No such file or directory/ {{print; exit}}'); "
            f"x=$(find {dirs} -xdev -type d \\( ! -readable -o ! -executable \\) -print -quit 2>/dev/null); "
            f"[ -n \"$x\" ] && [ -n \"$denied\" ] && denied=', du: Permission denied' || denied=; "
            f"{owner_check}"
            f"if [ -n \"$x$denied\" ] || [ \"$m\" -gt {DISK_MB} ] || [ \"$i\" -gt {DISK_INODES} ]; then "
            f"touch {busy}; echo \"{step}: $m MB, $i inodes (limits {DISK_MB} MB, {DISK_INODES})${{x:+, inaccessible $x}}$denied: stopped\" >> {out}/limits.txt; "
            f"stop_scope; break; fi; "
            f"set -- $(df -Pk \"$HOME\" | awk 'NR==2{{print $4}}') $(df -Pi \"$HOME\" | awk 'NR==2{{print $4}}'); "
            f"{owner_check}"
            f"if [ -n \"$2\" ] && {{ [ \"$1\" -lt {DISK_FLOOR_GB * 1024 * 1024} ] || [ \"$2\" -lt {INODE_FLOOR} ]; }} "
            f"&& mkdir {PAUSED} 2>/dev/null; then "
            f"touch {busy}; {{ echo \"$(date '+%F %T') home filesystem at $1 KB / $2 inodes free (floor {DISK_FLOOR_GB} GB / {INODE_FLOOR}); "
            f"remove this directory to resume launches\"; python3 {BASE}/tools/floor_stop.py --act {SLICE}; }} > {PAUSED}/why.txt; "
            f"rm -f {busy}; fi; done ) </dev/null >/dev/null 2>&1 & wd=$!; ")


def watchdog_teardown(tag, step):
    """Freeze before testing the action flag; finish any stop escalation before killing the tree.

    scope_stop's timeouts total at most 57 seconds. Allow 90 seconds, logging a failure if the host cannot
    finish in that bound. The same protection lets an in-progress disk-floor action finish and log its result.
    """
    _, out, _, _ = paths(tag)
    busy, done = f"{out}/watchdog-{step}.busy", f"{out}/watchdog-{step}.done"
    return (f'kill -STOP "$wd" 2>/dev/null; '
            f'for wd_freeze in {{1..100}}; do wd_state=$(ps -o stat= -p "$wd"); '
            f'case "$wd_state" in *T*|"") break;; esac; sleep 0.01; done; '
            f'if [ -e {busy} ]; then '
            f'touch {done}; kill -CONT "$wd" 2>/dev/null; '
            f'for wd_wait in {{1..90}}; do kill -0 "$wd" 2>/dev/null || break; sleep 1; done; '
            f'kill -0 "$wd" 2>/dev/null && '
            f"echo '{step}: watchdog action did not finish within 90 seconds' >> {out}/limits.txt; fi; "
            f'kt $wd; wait "$wd" 2>/dev/null; rm -f {busy} {done}; ')


# kill a process and all its descendants (the watchdog's sleep and du): stopped first, so it can't start another child
# between listing its children and being killed
KILL_TREE = ("kt() { local c; kill -STOP \"$1\" 2>/dev/null; for c in $(pgrep -P \"$1\"); do kt \"$c\"; done; "
             "kill -KILL \"$1\" 2>/dev/null; }; ")


def step_guard(tag):
    """First thing in every step: nothing runs once a limit tripped for this agent or launches are paused."""
    _, out, _, _ = paths(tag)
    return f"[ -e {out}/limits.txt ] && exit 7; [ -e {PAUSED} ] && exit 8; "


def agent_cmd(tag, secs, args, step):
    """Run the agent CLI in the sandbox for at most secs. Its stdout and stderr go through pipes to files in the output
    dir, which the sandbox can't reach, capped over the agent's whole run (EVENTS_MAX, STDERR_MAX; reaching a cap is
    recorded in limits.txt and stops the scope immediately). After the pass the host records the event byte offset,
    exit status and a host timeout flag in passes.txt, outside the agent sandbox: only these boundaries can excuse
    a truncated line."""
    ws, out, _, _ = paths(tag)
    unit = f"{scope(tag, step)}.scope"
    return (f"{KILL_TREE}{step_guard(tag)}cd {ws} && {bind_check(tag, 'agent')} || exit 4; "
            f"{scope_stop(tag, step)}"
            f"er=$(( {EVENTS_MAX} - $(stat -c %s {out}/events.jsonl 2>/dev/null || echo 0) )); "
            f"sr=$(( {STDERR_MAX} - $(stat -c %s {out}/stderr.log 2>/dev/null || echo 0) )); "
            f"[ $er -gt 0 ] && [ $sr -gt 0 ] || {{ echo '{step}: a log was already at its cap' >> {out}/limits.txt; exit 7; }}; "
            # A host-written newline before each continuation keeps provenance's physical lines separate. It belongs
            # to the next pass; the previous end offset stays at the producer's final byte. Charge it to the log cap.
            f"if [ -s {out}/events.jsonl ]; then [ $er -gt 1 ] || "
            f"{{ echo '{step}: no event log room for a pass separator' >> {out}/limits.txt; exit 7; }}; "
            f"printf '\\n' >> {out}/events.jsonl; er=$((er - 1)); fi; "
            # Each reader acts as soon as head exits at its cap, while the producer may still be running (or asleep).
            # Kill every member first so a process ignoring SIGTERM cannot continue through systemd's stop grace.
            f"capped_log() {{ local file=$1 cap=$2 remaining=$3; head -c \"$remaining\" >> {out}/\"$file\"; "
            f"if [ $(stat -c %s {out}/\"$file\") -ge \"$cap\" ]; then "
            f"echo \"{step}: $file reached its $cap-byte cap: cut\" >> {out}/limits.txt; "
            f"timeout -k 1 5 systemctl --user kill --kill-who=all --signal=KILL {unit} 2>/dev/null || "
            f"echo '{step}: log cap scope KILL failed' >> {out}/limits.txt; stop_scope; fi; }}; "
            f"{watchdog(tag, step)}"
            # stderr into one capped pipe and stdout (through fd 3) into the other: both caps are members of the
            # pipelines, so the shell waits for them, and the agent's exit status comes out of the inner subshell
            # timeout's own diagnostics go only to a host file. The child forwards its stderr to the capped pipe
            # through fd 4, then closes it before entering the sandbox; an agent exit of 124/137 cannot forge this.
            f"( LC_ALL=C timeout --verbose -k 30 {int(secs)} /bin/sh -c 'exec \"$@\" 2>&4 4>&-' sh {sandbox(tag, step, 'agent')} "
            f"pi --mode json --session-dir {ws}/.pi-sessions --model {MODEL} {args} </dev/null 9<{KEY_FILE} 4>&1 2>{out}/timeout-{step}.txt 1>&3 3>&- "
            f"| capped_log stderr.log {STDERR_MAX} $sr; exit ${{PIPESTATUS[0]}} ) 3>&1 | capped_log events.jsonl {EVENTS_MAX} $er; "
            f"rc=${{PIPESTATUS[0]}}; {watchdog_teardown(tag, step)}"
            f"timed_out=0; grep -q '^timeout: sending signal .* to command' {out}/timeout-{step}.txt && timed_out=1; "
            f"rm -f {out}/timeout-{step}.txt; "
            f"printf '%s %s %s\\n' \"$(stat -c %s {out}/events.jsonl)\" \"$rc\" \"$timed_out\" >> {out}/passes.txt; "
            f"echo rc=$rc")


def box_cmd(tag, step, cmd, secs=600):
    """A non-agent step on the workspace (check.py, rel_link.py): the same sandbox, since it runs the agent's files."""
    ws, _, _, _ = paths(tag)
    return (f"{KILL_TREE}{step_guard(tag)}cd {ws} && {bind_check(tag, 'agent')} || exit 4; {watchdog(tag, step)}"
            f"timeout -k 30 {secs} {sandbox(tag, step, 'agent')} /bin/sh -c {shlex.quote(cmd)} 9</dev/null; "
            f"rc=$?; {watchdog_teardown(tag, step)}exit $rc")


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
    """Stop this agent's sandbox scopes (everything in them dies), then kill stragglers in the workspace. True only if
    the host answered and none of the scopes is still active: collection waits for that."""
    ws, _, _, _ = paths(tag)
    units = " ".join(f"{scope(tag, st)}.scope" for st in STEPS)
    try:
        # REAPED only if systemd says every one of the scopes is inactive or failed; no answer is not reaped
        r = ssh(f"for u in {units}; do systemctl --user stop \"$u\" 2>/dev/null; done; ( {reap_script(ws)} ); "
                f"for u in {units}; do case $(systemctl --user is-active \"$u\" 2>/dev/null) in inactive|failed) ;; *) exit 1;; esac; done; "
                f"echo REAPED", timeout=180, check=False)
    except subprocess.TimeoutExpired:
        return False
    return r.returncode == 0 and r.stdout.strip().endswith("REAPED")


def host_state():
    """Containers and user units (other than the agents' own scopes): an agent that got out would show up here."""
    try:
        r = ssh("docker ps -q --no-trunc 2>/dev/null | sed 's/^/container /'; "
                "systemctl --user list-units --all --plain --no-legend 2>/dev/null | awk '{print \"unit \" $1}' | grep -v ' ttyd-'",
                timeout=60, check=False)
        if r.returncode != 0:
            return None
        return set(r.stdout.split("\n")) - {""}
    except subprocess.TimeoutExpired:
        return None  # unknown, not empty: no comparison


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
    set_aside(tag)  # an earlier run's results, so nothing from it can be merged as this run's
    RUN_IDS[tag] = time.strftime("%Y%m%d-%H%M%S-") + os.urandom(3).hex()
    p.mkdir(exist_ok=True)
    # this launcher made the folder: tools/collect_gate.py never admits it as a pre-audit result
    (p / "LAUNCHED").write_text(RUN_IDS[tag] + "\n")
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
    tripped = lambda: ssh(f"{{ test -e {out}/limits.txt || test -e {PAUSED}; }} && echo yes", timeout=60, check=False).stdout.strip() != ""
    for k in range(3):  # continuation passes, moderate
        if tripped():
            return rcs + " LIMIT (no further steps; see limits.txt, or PAUSED on the host)"
        if spec.get("mode") == "link":
            cfg = src[len("src/"):] if spec["dol"] else src.rsplit("/src/", 1)[1]
            linked = ssh(f"[ -f {ws}/configure.py ] && [ ! -L {ws}/configure.py ] && [ $(stat -c %s {ws}/configure.py) -lt 4000000 ] && "
                         f"timeout 20 grep -qF 'Object(Matching, \"{cfg}\"' {ws}/configure.py && echo yes", timeout=60, check=False)
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
    if tripped():
        return rcs + " LIMIT (no further steps; see limits.txt, or PAUSED on the host)"
    done = ssh(f"test -s {ws}/REPORT.md && ! test -L {ws}/REPORT.md && grep -q -- '--final' {out}/events.jsonl && echo yes",
               timeout=60, check=False)
    if done.stdout.strip() != "yes":
        r3 = ssh(agent_cmd(tag, 900, "-c " + shlex.quote(finish), "finish"), timeout=1500, check=False)
        rcs += " finisher" + r3.stdout.strip()
        if tripped():
            return rcs + " LIMIT (no further steps; see limits.txt, or PAUSED on the host)"
    ssh(box_cmd(tag, "link", f"python3 tools/rel_link.py {unit} > link.txt 2>&1"), timeout=700, check=False)
    return rcs


RUN_IDS = {}  # tag -> this run's id (its start time), so a rerun's archived events don't replace the last run's


def collect(tag, spec):
    """Gather the results with tools/agent_collect.sh from the base, in the collect sandbox; archive the events and
    copy the output dir back (no links followed). Returns the collect's exit status (0 clean, see agent_collect.sh)."""
    ws, out, _, rout = paths(tag)
    arch = f"$HOME/{ARCHIVE}/{tag}/{RUN_IDS.get(tag, 'run')}"
    c = ssh(f"cd {out} && {bind_check(tag, 'collect')} && timeout -k 30 600 {sandbox(tag, 'collect', 'collect')} "
            f"bash {BASE}/tools/agent_collect.sh {BASE} {ws} {out} {spec['src']} {spec.get('mode', '')}; echo audit=$?; "
            f"[ -f {out}/events.jsonl ] && [ ! -L {out}/events.jsonl ] && mkdir -p \"{arch}\" && "
            f"zstd -q -3 -f {out}/events.jsonl -o \"{arch}/events.jsonl.zst\" && "
            f"{{ [ ! -f {out}/passes.txt ] || cp {out}/passes.txt \"{arch}/passes.txt\"; }}",
            timeout=900, check=False)
    # the manifest last, after everything it lists (tools/collect_gate.py checks the files against it)
    rsync("--no-links", f"--max-size={TAKE_MAX}", "--exclude", "events.jsonl", "--exclude", "COLLECT-OK*",
          f"{HOST}:{rout}/", str(HERE / tag) + "/")
    try:
        rsync("--no-links", f"{HOST}:{rout}/COLLECT-OK", str(HERE / tag) + "/")
    except subprocess.CalledProcessError:
        pass  # no manifest: the collect failed or flagged, and the gate refuses the folder
    m = re.search(r"audit=(\d+)", c.stdout)
    return int(m.group(1)) if m else -1


def archive_rel(a):
    """The archive dir relative to the host's home ("~/x", "$HOME/x" or "x"), for both its shell and rsync spellings:
    plain path characters only, no `..`, nothing outside the home directory."""
    for pre in ("~/", "$HOME/", "${HOME}/"):
        if a.startswith(pre):
            a = a[len(pre):]
            break
    if not re.fullmatch(r"[A-Za-z0-9_.-]+(/[A-Za-z0-9_.-]+)*", a) or ".." in a.split("/"):
        sys.exit(f"ARCHIVE must be a plain path under the host's home: {a}")
    return a


def set_aside(tag, keep=2):
    """Move an earlier run's local results for tag to <tag>.prevN, keeping the newest `keep` of those (older ones go
    to the trash)."""
    p = HERE / tag
    if not p.exists() or not any(f.name != "PROMPT.md" for f in p.iterdir()):
        return
    num = lambda q: int(q.name.rsplit(".prev", 1)[1])
    prevs = sorted((q for q in HERE.glob(f"{tag}.prev*") if re.fullmatch(r"\d+", q.name.rsplit(".prev", 1)[1])), key=num)
    p.rename(HERE / f"{tag}.prev{(num(prevs[-1]) if prevs else 0) + 1}")  # the newest always has the highest number
    prevs = sorted((q for q in HERE.glob(f"{tag}.prev*") if re.fullmatch(r"\d+", q.name.rsplit(".prev", 1)[1])), key=num)
    for old in prevs[:-keep]:
        subprocess.run(["trash", str(old)], capture_output=True, timeout=60)


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
    global KEY_HASH, NODE_BIN
    HERE, WAVE, MODEL, KEY_ENV, MODEL_LINE, CPU_QUOTA = Path(here), wave, model, key_env, model_line, cpu_quota
    BASE = re.sub(r"^~(?=/)", "$HOME", os.environ.get("BASE") or base)
    ARCHIVE = archive_rel(os.environ.get("ARCHIVE") or archive or "~/ttyd-archive/scratch/" + HERE.name)
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
    KEY_HASH = hashlib.sha256(key.encode()).hexdigest()[:12]
    UID = ssh("id -u", timeout=60).stdout.strip()
    NODE_BIN = ssh('dirname "$(readlink -f "$(command -v node)")"', timeout=60, check=False).stdout.strip()
    if not NODE_BIN.startswith("/") or NODE_BIN in ("/", "/."):
        sys.exit(f"node not found on {HOST} (got {NODE_BIN!r}): the sandbox's PATH needs its absolute directory")
    wave_start = time.time()
    ssh(f"systemctl --user set-property --runtime {SLICE} {SLICE_PROPS}", timeout=60)
    lock = threading.Lock()

    def log(msg):
        with lock:
            print(time.strftime("%H:%M:%S"), msg, flush=True)

    q = queue.Queue()
    for s in SPEC:
        q.put(s)

    stop_all = threading.Event()  # set once launches are paused: every worker stops taking work

    def worker():
        while not stop_all.is_set():
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
                    state = "FULL"
                    while not (DEADLINE and time.time() >= DEADLINE):
                        state = reserve(tag)
                        if state == "TAKEN":
                            slot = True  # at once: every path from here frees it
                            break
                        if state == "PAUSED":
                            break
                        time.sleep(60)
                if state == "PAUSED":
                    if not stop_all.is_set():
                        stop_all.set()
                        log(f"LAUNCHES PAUSED on the host ({PAUSED}): the disk floor tripped; starting no more agents. "
                            "Remove that directory once there is room, then rerun the launcher for what is left.")
                    return
                if not slot:
                    log(f"{tag} skipped: past the deadline")
                    continue
                if not claim(s["unit"]):
                    log(f"{tag} skipped: {s['unit']} is claimed by another wave")
                    continue
                claimed = True
                reap(tag)  # a scope left from an earlier run of this tag would block the new one's name
                prepare(tag, s)
                if DEADLINE and time.time() >= DEADLINE:
                    log(f"{tag} skipped: past the deadline after preparing")
                    release(s["unit"])
                    continue
                started = True
                before = host_state()
                t0 = time.time()
                rcs = run(tag, s["unit"], s["src"], s, log)
                log(f"{tag} finished in {time.time() - t0:.0f}s: {rcs}")
                after = host_state()
                if before is None or after is None:
                    log(f"{tag} host state unknown (docker/systemctl didn't answer): no before/after comparison")
                new = (after - before) if before is not None and after is not None else set()
                if new:
                    (HERE / tag / "host_changes.txt").write_text("\n".join(sorted(new)) + "\n")
                    log(f"{tag} NEW ON HOST while it ran (another session may have started these): {', '.join(sorted(new))}")
            except Exception as e:  # including every ssh deadline (subprocess.TimeoutExpired) and failed step
                log(f"{tag} ERROR {e}")
                if claimed and not started:
                    release(s["unit"])
            finally:
                reaped = reap(tag) if claimed else True  # before collecting: nothing of the agent may still be writing
                if started and not reaped:
                    log(f"{tag} REAP FAILED: not collected (a scope may still run; collect by hand once it is stopped)")
                elif started:
                    try:
                        audit = collect(tag, s)
                        if audit != 0:
                            log(f"{tag} FLAGGED (collect exit {audit}; see audit.txt, collect.txt, limits.txt)")
                            rsync(f"{HOST}:{ARCHIVE}/{tag}/{RUN_IDS.get(tag, 'run')}/events.jsonl.zst", str(HERE / tag) + "/")
                    except Exception as e:
                        log(f"{tag} COLLECT ERROR {e}")
                    reap(tag)
                if slot:
                    free(tag)

    why = paused()
    if why:
        sys.exit(f"launches are paused on the host ({PAUSED}):\n{why}\nRemove that directory once there is room.")
    for s in SPEC:
        set_aside(s["tag"])  # at queue time: no earlier run's results sit where this run's will land
    log(f"{len(SPEC)} files, {WORKERS} at a time")
    threads = [threading.Thread(target=worker) for _ in range(WORKERS)]
    for th in threads:
        th.start()
        time.sleep(START_STAGGER)  # stagger the workspace copies
    for th in threads:
        th.join()
    since = int((time.time() - wave_start) / 60) + 10  # the whole wave
    try:
        c = ssh(f"python3 {BASE}/tools/tower_check.py --since {since}", timeout=600, check=False)
        verdict = {0: "clean", 1: "FINDINGS"}.get(c.returncode, f"FAILED (exit {c.returncode})")
        log(f"all done; tower_check {verdict}:\n" + c.stdout.strip() + ("\n" + c.stderr.strip() if c.returncode > 1 else ""))
    except subprocess.TimeoutExpired:
        log("all done; tower_check FAILED (timed out)")


if __name__ == "__main__":
    sys.exit("run a wave's config (scratch/waveNN/launch.py), which calls main(...)")
