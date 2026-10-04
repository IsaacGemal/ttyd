#!/usr/bin/env python3
"""Launch a wave of decomp agents on the build tower, one workspace per spec entry.

Per-wave configs (scratch/waveNN/launch.py, untracked) call main(...) with the wave's name, base checkout, spec and
the agent's model settings; everything that decides what an agent may touch lives here so it is versioned and reviewed.

Each agent runs in a workspace ~/ttyd-agents/<PFX><tag> (an rsync of the base checkout, shared read-only parts linked)
inside a sandbox:
  systemd-run --user --scope -p CPUQuota=<CPU_QUOTA>   one agent can't take the tower
  bwrap --ro-bind / /                                   the whole filesystem read-only, except
        --bind $WS $WS --bind $WS/tmp /tmp              its own workspace and a private /tmp
        --unshare-pid --die-with-parent --new-session   everything it starts dies with it
The agent's sessions are kept in the workspace (pi --session-dir), and the API key is read from a 0600 file on the
host, never put on a command line. After each agent, tools/agent_audit.py scans its events for commands that reach
outside the workspace or start background processes; flagged agents are marked in the launcher log.

Environment: KEY (the API key), PFX (workspace prefix, one per key), PFXRE (every prefix sharing this key's slots),
PART ("k/n": this launcher's share of the spec), CAP/QUIET_FROM/QUIET_TO/QUIET_CAP, WORKERS, DEADLINE, ONLY, SPECFILE.
"""
import json, os, queue, shlex, subprocess, sys, threading, time
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

PROMPT = """You are one of 24 agents making progress on a matching decompilation of Paper Mario: The
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

LINK_PROMPT = """You are one of 24 agents working on a matching decompilation of Paper Mario: The Thousand-Year Door
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


def cap_now():
    # the tower's permuter needs load under its gate overnight: a lower cap inside the quiet window (which wraps midnight)
    t = time.strftime("%H:%M")
    quiet = (QUIET_FROM <= t or t < QUIET_TO) if QUIET_FROM > QUIET_TO else (QUIET_FROM <= t < QUIET_TO)
    return QUIET_CAP if quiet else CAP
# functions reviews took out of the tree (wave 50-52 ledger chapters): agents must not write them back
REMOVED = []  # (unit, function, why): functions reviews took out; agents must not write them back
HINTS = {}  # unit -> hint text
WHY = {}  # source path -> why the unit does not link
slot_lock = threading.Lock()


def running():
    r = subprocess.run(["ssh", HOST, f"for p in $(pgrep -f 'pi --mode json'); do readlink /proc/$p/cwd; done | grep -E 'ttyd-agents/({PFXRE})' | sort -u | wc -l"],
                       capture_output=True, text=True)
    try:
        return int(r.stdout.strip() or 0)
    except ValueError:
        return CAP


DEADLINE = os.environ.get("DEADLINE", "")


def claim(unit):
    out = subprocess.run(["python3", str(REPO / "tools/claims.py"), "claim", WAVE, unit], capture_output=True, text=True).stdout
    return "CLAIMED" in out


KEY_FILE = ""  # set by main(): ~/.config/ttyd-keys/<PFX>, mode 0600
AUDIT = ""  # set by main(): the base's tools/agent_audit.py


def sandbox():
    """The command prefix every agent run goes through (expanded by the remote shell, so $PWD is the workspace)."""
    return (f"systemd-run --user --scope --quiet -p CPUQuota={CPU_QUOTA} "
            "bwrap --ro-bind / / --dev /dev --proc /proc --bind \"$PWD\" \"$PWD\" --bind \"$PWD/tmp\" /tmp "
            # the agent CLI writes lock files next to its settings: it gets a private copy made by prepare()
            "--bind \"$PWD/.pi-agent\" \"$HOME/.pi/agent\" "
            "--unshare-pid --die-with-parent --new-session --")


def install_key(key):
    """Put the key in a 0600 file on the host once (over stdin), so no command line ever carries it."""
    subprocess.run(["ssh", HOST, f"umask 077 && mkdir -p ~/.config/ttyd-keys && cat > {KEY_FILE}"], input=key, text=True, check=True)


def MODEL_LINE_FIX(text):
    return text.replace("@@MODEL_LINE@@", MODEL_LINE)


def ssh(cmd, **kw):
    return subprocess.run(["ssh", HOST, cmd], capture_output=True, text=True, **kw)


def prepare(tag, spec):
    unit = spec["unit"]
    ws = f"~/ttyd-agents/{PFX}{tag}"
    # Shared, never-modified parts are links, not copies: the disc (orig/), the venv, the compilers and the build
    # tools (~190 MB per workspace otherwise). Build outputs stay per workspace: each agent builds its own code.
    # as tools/wave_launch.py: no permuter entry points (it was most of the tower's CPU), DOL asm linked from the base
    ssh(f"mkdir -p ~/ttyd-agents && rsync -a --delete --delete-excluded --exclude orig/ --exclude .venv/ --exclude build/compilers/ "
        f"--exclude build/tools/ --exclude build/G8MJ01/asm/ --exclude 'tools/perm_*.py' --exclude .git/ {BASE}/ {ws}/ "
        f"&& ln -sfn {BASE}/build/G8MJ01/asm {ws}/build/G8MJ01/asm && ln -sfn ~/ttyd/orig {ws}/orig && ln -sfn ~/ttyd/.venv {ws}/.venv "
        f"&& ln -sfn ~/ttyd/build/compilers {ws}/build/compilers && ln -sfn {BASE}/build/tools {ws}/build/tools")
    src = spec['src']
    ssh(f"cp {ws}/tools/agent_check.py {ws}/check.py && cp {ws}/{src} {ws}/orig_file.c && mkdir -p {ws}/tmp "
        # the agent CLI's settings and credential store, without the shared session history; bound over ~/.pi/agent
        f"&& rsync -a --delete --exclude sessions/ ~/.pi/agent/ {ws}/.pi-agent/")
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
        subprocess.run(["rsync", "-az", spec["start_c"], f"{HOST}:{ws}/{src}"], check=True)
        subprocess.run(["rsync", "-az", spec["start_h"], f"{HOST}:{ws}/{spec['hdr']}"], check=True)
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
                subprocess.run(["rsync", "-az", str(REPO / pr), f"{HOST}:{ws}/prev_REPORT_{i + 1}.md"], check=True)
            hint += ("\n- An earlier agent already tried to link this unit; its notes are in "
                     + " ".join(f"prev_REPORT_{i + 1}.md" for i in range(len(spec["prev"])))
                     + ". Read them first and don't repeat what didn't work.")
        (p / "PROMPT.md").write_text(MODEL_LINE_FIX(LINK_PROMPT.format(src=src, unit=unit, part=part, why=why, cfg=cfg, build=build,
                                                        asm=asm, done=done, hint=hint)) + "\n")
        subprocess.run(["rsync", "-az", str(p / "PROMPT.md"), f"{HOST}:{ws}/PROMPT.md"], check=True)
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
        subprocess.run(["rsync", "-az", str(REPO / spec["seed"]), f"{HOST}:{ws}/salvaged_version.c"], check=True)
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
        subprocess.run(["rsync", "-az", str(REPO / spec["seed"]), f"{HOST}:{ws}/salvaged_version.c"], check=True)
        fn = spec["open"][0][0]
        note += (f"\n./salvaged_version.c is an earlier agent's version of `{fn}` (with its comments and any helpers/pragmas"
                 f" it needed) that never landed: {spec['seed_why']}. Start from it: bring it into the file in place of the"
                 " current version, make it build, and fix that problem (the unit's other functions and its data sections"
                 " must not end lower than they started; usually that means keeping the file's existing named constants,"
                 " strings and their order instead of the version's own). Check its code against the asm as carefully as"
                 " new code: it has never been reviewed. Partial scores barely notice wrong compares: `cror eq,gt,eq` is"
                 " `>=` and `cror eq,lt,eq` is `<=`; check every comparison and every read against the store that sets it.")
    if spec.get("draft"):
        subprocess.run(["rsync", "-az", str(REPO / spec["draft"]), f"{HOST}:{ws}/m2c_draft.c"], check=True)
        fn = spec["open"][0][0]
        if spec.get("seed"):
            subprocess.run(["rsync", "-az", str(REPO / spec["seed"]), f"{HOST}:{ws}/salvaged_version.c"], check=True)
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
            subprocess.run(["rsync", "-az", str(REPO / pr), f"{HOST}:{ws}/prev_REPORT_{i + 1}.md"], check=True)
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
    subprocess.run(["rsync", "-az", str(p / "PROMPT.md"), f"{HOST}:{ws}/PROMPT.md"], check=True)


def limit(spec):
    if spec.get("mode") == "link":
        return 10800
    if spec.get("draft"):
        return 14400 if spec["open"][0][1] >= 6000 else 10800 if spec["open"][0][1] >= 2500 else 7200
    b = sum(sz for _, sz, _ in spec["open"])
    return 5400 if b <= 2500 else 7200 if b <= 5000 else 10800


def run(tag, unit, src, spec, key, log):
    ws = f"~/ttyd-agents/{PFX}{tag}"
    t0 = time.time()
    # The sandbox: the whole filesystem read-only except this workspace and a private /tmp, a pid namespace so
    # nothing the agent starts (nohup, &, setsid) outlives it or can signal anything outside, and a CPU quota.
    # If it stops without REPORT.md or without running check.py --final, one short continuation asks for just that.
    box = sandbox()
    finish = (f"You stopped before finishing. Do only this: run python3 check.py --final {unit}, fix what it lists, "
              f"and write REPORT.md as PROMPT.md describes. Don't start other work.")
    r = ssh(f"export {KEY_ENV}=\"$(cat {KEY_FILE})\" TTYD_BASE={BASE} PYTHONPATH=$HOME/ttyd-pylib; export PATH=$HOME/.local/bin:$HOME/ttyd/.venv/bin:$PATH; "
            f"mkdir -p {ws}/tmp && export TMPDIR=/tmp; "
            f"cd {ws} && timeout {limit(spec)} {box} pi --mode json --session-dir $PWD/.pi-sessions --model {MODEL} @PROMPT.md @docs/AGENT_BRIEF.md "
            f"'Do the task described in the attached PROMPT.md, following docs/AGENT_BRIEF.md.' </dev/null >events.jsonl 2>stderr.log; echo rc=$?; "
            f"true")
    names = {n for n, _, _ in spec["open"]}
    for k in range(3):  # continuation passes, moderate
        left = limit(spec) - (time.time() - t0)
        # timeout: an mwcceppc hang once blocked this call (and its worker thread) for 16 hours
        if spec.get("mode") == "link":
            cfg = src[len("src/"):] if spec["dol"] else src.rsplit("/src/", 1)[1]
            if ssh(f"grep -qF 'Object(Matching, \"{cfg}\"' {ws}/configure.py && echo yes").stdout.strip() == "yes" or left < 1800:
                break
            more = (f"Keep going: you still have about {int(left // 60)} minutes and the unit doesn't link yet. Continue as "
                    f"PROMPT.md describes (re-run tools/link_diag.py {unit} for where it stands), keep REPORT.md current, and "
                    f"run python3 check.py --final {unit} before you stop.")
            if k >= 2:
                break
            log(f"{tag} link continuation {k + 1}, {int(left)}s left")
            r2 = ssh(f"export {KEY_ENV}=\"$(cat {KEY_FILE})\" TTYD_BASE={BASE} PYTHONPATH=$HOME/ttyd-pylib; export PATH=$HOME/.local/bin:$HOME/ttyd/.venv/bin:$PATH; "
                     f"export TMPDIR=/tmp; cd {ws} && timeout {int(left)} {box} pi --mode json --session-dir $PWD/.pi-sessions -c --model {MODEL} {shlex.quote(more)} "
                     f"</dev/null >>events.jsonl 2>>stderr.log; echo rc=$?")
            r = subprocess.CompletedProcess(r.args, 0, r.stdout + " cont" + r2.stdout.strip(), "")
            continue
        c = ssh(f"cd {ws} && PATH=$HOME/ttyd/.venv/bin:$PATH timeout -k 10 600 python3 check.py {unit}")
        still = [l.split()[-1] for l in c.stdout.splitlines()[1:] if l.split() and l.split()[-1] in names and not l.strip().startswith("100.00%")]
        if not still or left < 900:
            break
        more = (f"Keep going: you still have about {int(left // 60)} minutes. These functions are still below 100%: "
                f"{', '.join(still)}. Continue with them as PROMPT.md describes (same rules; never lower a score), "
                f"keep REPORT.md current, and run python3 check.py --final {unit} before you stop.")
        log(f"{tag} continuation {k + 1}: {len(still)} open, {int(left)}s left")
        r2 = ssh(f"export {KEY_ENV}=\"$(cat {KEY_FILE})\" TTYD_BASE={BASE} PYTHONPATH=$HOME/ttyd-pylib; export PATH=$HOME/.local/bin:$HOME/ttyd/.venv/bin:$PATH; "
                 f"export TMPDIR=/tmp; cd {ws} && timeout {int(left)} {box} pi --mode json --session-dir $PWD/.pi-sessions -c --model {MODEL} {shlex.quote(more)} "
                 f"</dev/null >>events.jsonl 2>>stderr.log; echo rc=$?")
        r = subprocess.CompletedProcess(r.args, 0, r.stdout + " cont" + r2.stdout.strip(), "")
    r3 = ssh(f"export {KEY_ENV}=\"$(cat {KEY_FILE})\" TTYD_BASE={BASE} PYTHONPATH=$HOME/ttyd-pylib; export PATH=$HOME/.local/bin:$HOME/ttyd/.venv/bin:$PATH; "
            f"export TMPDIR=/tmp; cd {ws} && "
            f"if ! test -s REPORT.md || ! grep -q -- '--final' events.jsonl; then echo finisher; "
            f"timeout 900 {box} pi --mode json --session-dir $PWD/.pi-sessions -c --model {MODEL} {shlex.quote(finish)} </dev/null >>events.jsonl 2>>stderr.log; fi; "
            f"cp {src} final.c 2>/dev/null; timeout -k 10 600 python3 tools/rel_link.py {unit} > link.txt 2>&1; git -C . status >/dev/null 2>&1; "
            f"{{ diff -rq {BASE}/src src; diff -rq {BASE}/libs libs; diff -rq {BASE}/rels rels; diff -q {BASE}/configure.py configure.py; }} | grep -v -F '{src} differ' > other_changes.txt; "  # merge_agent.sh allows the agent's own file
            # header additions come back as a patch (a/include -> b/include); tools/merge_agent.sh checks and applies it
            f"diff -u {BASE}/configure.py configure.py > configure.patch; "
            f"diff -ruN {BASE}/include include | sed -e 's#^--- /.*/include/#--- a/include/#' "
            f"-e 's#^+++ include/#+++ b/include/#' > include.patch; "
            f"for d in rels/*/include; do diff -ruN {BASE}/$d $d; done > rels_include.patch; "
            # what it fetched or searched outside the workspace, for the merge (tools/merge_agent.sh shows it)
            f"python3 tools/provenance.py events.jsonl $(pwd) > provenance.txt 2>&1 "
            f"|| echo 'ERROR tools/provenance.py failed; check events.jsonl by hand' >> provenance.txt; "
            # commands that reached outside the workspace or started background processes
            f"python3 {AUDIT} events.jsonl $(pwd) > audit.txt 2>&1; echo audit=$?; "
            f"mkdir -p {ARCHIVE}/{tag} && zstd -q -3 -f events.jsonl -o {ARCHIVE}/{tag}/events.jsonl.zst; true")
    subprocess.run(["rsync", "-az", f"{HOST}:{ws}/REPORT.md", f"{HOST}:{ws}/final.c", f"{HOST}:{ws}/link.txt",
                    f"{HOST}:{ws}/other_changes.txt", f"{HOST}:{ws}/include.patch", f"{HOST}:{ws}/rels_include.patch", f"{HOST}:{ws}/provenance.txt", f"{HOST}:{ws}/stderr.log", f"{HOST}:{ws}/configure.patch", f"{HOST}:{ws}/audit.txt",
                    str(HERE / tag) + "/"])
    # kill anything still running from this workspace (a hung mwcceppc/wibo outlives the agent otherwise)
    ssh(f"d=$(cd {ws} && pwd); for p in $(pgrep -u $(id -u)); do case $(readlink /proc/$p/cwd 2>/dev/null) in $d|$d/*) kill -9 $p;; esac; done; true")
    flagged = ""
    if "audit=1" in r3.stdout:
        flagged = " AUDIT FLAGGED (see audit.txt)"
        # keep the flagged session's events with the results: the host's workspaces are pruned
        subprocess.run(["rsync", "-az", f"{HOST}:{ARCHIVE}/{tag}/events.jsonl.zst", str(HERE / tag) + "/"])
    log(f"{tag} finished in {time.time() - t0:.0f}s: {r.stdout.strip()} {r3.stdout.strip()}{flagged}")


def main(wave, here, base, model, key_env, model_line, specfile="units.json", removed=None, hints=None, why=None,
         cpu_quota="200%", archive=None):
    """Run a wave. here: the wave's scratch directory (spec, prompts and results live there). removed/hints/why: paths
    to removed.json (unit, function, why), hints.json (unit -> text) and an autolink failure list, if any."""
    global HERE, WAVE, BASE, MODEL, KEY_ENV, MODEL_LINE, CPU_QUOTA, ARCHIVE, SPEC, REMOVED, HINTS, WHY, KEY_FILE, AUDIT
    HERE, WAVE, BASE, MODEL, KEY_ENV, MODEL_LINE, CPU_QUOTA = Path(here), wave, base, model, key_env, model_line, cpu_quota
    ARCHIVE = archive or "~/ttyd-archive/scratch/" + HERE.name
    SPEC = json.load(open(HERE / os.environ.get("SPECFILE", specfile)))
    if os.environ.get("ONLY"):
        SPEC = [x for x in SPEC if x["unit"] in os.environ["ONLY"].split()]
    if os.environ.get("PART"):
        k, n = map(int, os.environ["PART"].split("/"))
        SPEC = SPEC[k::n]
    REMOVED = [tuple(x) for x in json.load(open(removed))] if removed else []
    HINTS = json.load(open(hints)) if hints else {}
    WHY = ({l.split()[1].rstrip(":"): l.split(None, 2)[2].strip() for l in open(why) if len(l.split()) > 2} if why else {})
    if not PFX:
        sys.exit("PFX not set")
    KEY_FILE = f"~/.config/ttyd-keys/{PFX.rstrip('_')}"
    AUDIT = f"{BASE}/tools/agent_audit.py"
    key = os.environ.get("KEY") or sys.exit("KEY not set")
    install_key(key)
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
            try:
                if DEADLINE and DEADLINE <= time.strftime("%H:%M") < "12:00":  # an after-midnight deadline
                    log(f"{tag} skipped: past {DEADLINE}")
                    continue
                with slot_lock:  # one start at a time across all w5x waves, so each agent shows up before the next check
                    while running() >= cap_now():
                        time.sleep(60)
                    if not claim(s["unit"]):
                        log(f"{tag} skipped: {s['unit']} is claimed by another wave")
                        continue
                    prepare(tag, s)
                    time.sleep(20)
                run(tag, s["unit"], s["src"], s, key, log)
            except Exception as e:
                log(f"{tag} ERROR {e}")

    log(f"{len(SPEC)} files, {WORKERS} at a time")
    threads = [threading.Thread(target=worker) for _ in range(WORKERS)]
    for th in threads:
        th.start()
        time.sleep(2)  # stagger the workspace copies
    for th in threads:
        th.join()
    log("all done")


if __name__ == "__main__":
    sys.exit("run a wave's config (scratch/waveNN/launch.py), which calls main(...)")
