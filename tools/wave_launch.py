#!/usr/bin/env python3
"""Launch a decomp wave from a JSON list of {tag, unit, func, size, score, has_src}.

Usage: KEY=... SPEC=scratch/wave47/units.json HOSTS=host1,host2 WORKERS=48 python3 tools/wave_launch.py
WORKERS is the total concurrency, split round-robin across HOSTS. START_STAGGER (seconds) defaults to 2.
The base checkout must already exist on each host at BASE (required). Workspaces are ~/ttyd-agents/w<NN>_<tag>.
"""
import json, os, queue, shlex, subprocess, sys, threading, time
from pathlib import Path

SPEC_PATH = Path(os.environ.get("SPEC", "scratch/units.json")).resolve()
HERE = SPEC_PATH.parent
HOSTS = [h for h in os.environ.get("HOSTS", "pop-os").replace(",", " ").split() if h]
TIMEOUT = 3600
WORKERS = int(os.environ.get("WORKERS", 24))
START_STAGGER = float(os.environ.get("START_STAGGER", 2))
MODEL = "featherless/deepseek-ai/DeepSeek-V4.1-Flash"
BASE = os.environ.get("BASE") or sys.exit("BASE not set (the base checkout on every host, e.g. ~/ttyd-w48base)")
WS_PREFIX = "w" + HERE.name.removeprefix("wave") + "_"
ARCHIVE = os.environ.get("ARCHIVE", "~/ttyd-archive/scratch/" + HERE.name)
SPEC = json.load(open(SPEC_PATH))
if os.environ.get("ONLY"): SPEC = [x for x in SPEC if x["unit"] in os.environ["ONLY"].split()]

PROMPT = """You are one of {workers} agents making progress on a matching decompilation of Paper Mario: The
Thousand-Year Door (GameCube, PowerPC, Metrowerks C compiler). A function counts as done only when it compiles to
exactly the original bytes (100%).

YOUR TASK: FINISH ONE FUNCTION, `{func}` ({size} bytes), in src/{unit}.c. It is already written and scores
{score:.2f}%; get it to 100%. Aim for the whole file to link (every function at 100% and the unit identical to the original); at minimum, get this function to 100%. Read its `// NONMATCHING` comment for what someone already found, and the brief's
"Code shapes that fixed near-misses" section first: those shapes fixed many functions like this one.
If the only differences are which float constant loads from which pool slot (the pool order), know that
within a statement the constants a call's arguments need are pooled after the statement's other constants:
storing the call's result in a temp first (`s = sin(a); x = 0.04f * s + 0.5f;`) swaps that order without changing
the code. Across functions, the pool follows the .text order (the reverse of the source order).
{filenote}
Only this file may change, with one exception: FUNCTION PROTOTYPES GO IN HEADERS. For a function defined in
another file, include that file's header (include/<path>.h for src/<path>.c); if the header doesn't declare it, ADD
the prototype there (create the header if it doesn't exist), with the definition's exact signature, or, if the
function isn't decompiled yet, the one the original's callee and callers show; one line per prototype. A function
the original keeps local to one file (`.fn NAME, local` in the asm) is declared static in that file. Only add
lines to headers: never change or remove existing ones. Structs and data externs you need may stay local in your
file; list them in the report. Don't edit configure.py or other source files.

Where to look:
- The original assembly: build/G8MJ01/asm/{unit}.s (find `.fn {func}`). Read the whole function first.
- Its callers and callees elsewhere in src/ and include/ tell you the types and what it's for; grep for them.
- docs/AGENT_BRIEF.md (attached) has the rules and the compiler idioms this codebase has learned.
- Where it goes in the file: this compiler emits a file's functions in REVERSE source order (brief rule 18), so
  place `{func}` relative to the file's existing functions in the reverse of their order in the asm.

Tools (run from this directory, each call rebuilds incrementally in a few seconds):
- `python3 check.py {unit} {func}` shows the original instructions (left) next to yours (right); `!!` marks
  differences. `python3 check.py {unit}` lists the file's functions with their match %.

Work in small steps: write a first version, run the check, fix, repeat. Keep the file building: a build error
fails every function in it. Get `{func}` to 100% if you can. Never lower its score: if nothing you try beats {score:.2f}%, restore the
version from ./orig_file.c (the file as you got it) and say what you tried. Keep its `// NONMATCHING` comment accurate. Then, if time is
left, check it once more against brief rule 17 (the wrong code reviews keep finding), and stop: don't start on
other functions.

Hard rules:
- No network: don't fetch, search or clone anything from the web. Everything you need is here: the asm, the
  source, include/, docs/.
- Stay inside this directory: don't read or list anything outside it (not ~/ttyd, not other agents' workspaces).
  Scratch files go in ./tmp.
- No inline `asm` and no changes to compiler flags. A match produced by pasting assembly is worthless.
- `#pragma` only of the kinds this codebase already uses, each with a one-line comment saying why, and only if the
  function doesn't match without it.
- Never delete, rename or change an existing function in the file. Never write an empty or near-empty body as a
  stand-in for real code.
- Use real struct fields where a header declares them; raw offsets only where the field isn't declared, listed
  in the report.

Write REPORT.md in this directory and update it as you go (you can be stopped at any moment). First line
exactly: `Model: DeepSeek V4.1 Flash (deepseek-ai/DeepSeek-V4.1-Flash, Featherless)`. Then: the function, its
size, its final %, what made it match or what still differs, and any header lines you added.

BEFORE YOU STOP: run `python3 check.py --final {unit}`. It rebuilds and runs the same checks the merge runs. Fix
everything it lists, run it again, and only then finish."""

NEWFILE = ("THE FILE DOESN'T EXIST YET: create src/{unit}.c with the includes it needs and just this one function\n"
           "(the build already lists it; until the file exists every function in it scores 0%). Other functions in it\n"
           "stay unwritten; don't add them.")
OLDFILE = "Change only `{func}` (and, if it needs one, a new static inline helper right above it). Other agents are working on other functions in this same file at the same time, so leave every other function exactly as it is: only your function will be taken from your copy."



def src_path(unit):
    """The unit's source as the repo lays it out: src/<unit>.c, or rels/<mod>/src/<rest>.c for REL units."""
    mod, _, rest = unit.partition("/")
    rel = Path(f"rels/{mod}/src/{rest}.c")
    return str(rel) if rest and rel.exists() else f"src/{unit}.c"

def ssh(host, cmd, **kw):
    return subprocess.run(["ssh", host, cmd], capture_output=True, text=True, check=True, **kw)


def prepare(host, tag, spec):
    unit = spec["unit"]
    ws = f"~/ttyd-agents/{WS_PREFIX}{tag}"
    # Keep full source and per-agent build objects for --final. The original asm is read-only and shared: a link
    # to the base's copy (agents read callers' asm in other units for prototypes). Permuter entry points are
    # deliberately absent from wave workspaces (the dedicated loop keeps them).
    ssh(host, f"mkdir -p ~/ttyd-agents && rsync -a --delete --delete-excluded --exclude orig/ --exclude .venv/ "
        f"--exclude build/compilers/ --exclude build/tools/ --exclude build/G8MJ01/asm/ "
        f"--exclude 'tools/perm_*.py' --exclude .git/ {BASE}/ {ws}/ "
        f"&& ln -sfn {BASE}/build/G8MJ01/asm {ws}/build/G8MJ01/asm "
        f"&& ln -sfn ~/ttyd/orig {ws}/orig && ln -sfn ~/ttyd/.venv {ws}/.venv "
        f"&& ln -sfn ~/ttyd/build/compilers {ws}/build/compilers && ln -sfn {BASE}/build/tools {ws}/build/tools")
    ssh(host, f"cp {ws}/tools/agent_check.py {ws}/check.py && "
        f"if test -f {ws}/{src_path(unit)}; then cp {ws}/{src_path(unit)} {ws}/orig_file.c; fi")
    p = HERE / tag
    p.mkdir(exist_ok=True)
    (p / "PROMPT.md").write_text(PROMPT.format(workers=WORKERS, unit=unit, func=spec["func"], size=spec["size"], score=spec["score"], filenote=(OLDFILE if spec["has_src"] else NEWFILE).format(unit=unit, func=spec["func"])) + "\n")
    subprocess.run(["rsync", "-az", str(p / "PROMPT.md"), f"{host}:{ws}/PROMPT.md"], check=True)


def run(host, tag, unit, key, log):
    ws = f"~/ttyd-agents/{WS_PREFIX}{tag}"
    t0 = time.time()
    # Each agent sees its own ./tmp as /tmp (bubblewrap; the prompt alone left agents writing the shared /tmp).
    # If it stops without REPORT.md or without running check.py --final, one short continuation asks for just that.
    box = f"bwrap --dev-bind / / --bind $PWD/tmp /tmp --"
    finish = (f"You stopped before finishing. Do only this: run python3 check.py --final {unit}, fix what it lists, "
              f"and write REPORT.md as PROMPT.md describes. Don't start other work.")
    r = ssh(host, f"export FEATHERLESS_API_KEY={shlex.quote(key)} TTYD_BASE={BASE}; export PATH=$HOME/.local/bin:$HOME/ttyd/.venv/bin:$PATH; "
            f"mkdir -p {ws}/tmp && export TMPDIR=/tmp; "
            f"cd {ws} && timeout {TIMEOUT} {box} pi --mode json --model {MODEL} @PROMPT.md @docs/AGENT_BRIEF.md "
            f"'Do the task described in the attached PROMPT.md, following docs/AGENT_BRIEF.md.' </dev/null >events.jsonl 2>stderr.log; echo rc=$?; "
            f"if ! test -s REPORT.md || ! grep -q -- '--final' events.jsonl; then echo finisher; "
            f"timeout 900 {box} pi --mode json -c --model {MODEL} {shlex.quote(finish)} </dev/null >>events.jsonl 2>>stderr.log; fi; "
            f"cp {src_path(unit)} final.c 2>/dev/null; "
            f"{{ diff -rq {BASE}/src src; diff -rq {BASE}/libs libs; diff -rq {BASE}/rels rels; }} | grep -v -F '{src_path(unit)} differ' "
            f"| grep -v -E 'rels/[^/ ]+/include[/:]' > other_changes.txt; "  # merge_agent.sh allows the agent's own file
            # header additions (include/ and each REL's rels/<mod>/include/) come back as one patch (a/... -> b/...);
            # tools/merge_agent.sh checks and applies it
            f"{{ diff -ruN {BASE}/include include; for d in rels/*/include; do diff -ruN {BASE}/$d $d; done; }} "
            f"| sed -e 's#^--- {BASE}/#--- a/#' -e 's#^+++ include/#+++ b/include/#' -e 's#^+++ rels/#+++ b/rels/#' > include.patch; "
            # what it fetched or searched outside the workspace, for the merge (tools/merge_agent.sh shows it)
            f"python3 tools/provenance.py events.jsonl $(pwd) > provenance.txt 2>&1 "
            f"|| echo 'ERROR tools/provenance.py failed; check events.jsonl by hand' >> provenance.txt; "
            f"mkdir -p {ARCHIVE}/{tag} && zstd -q -3 -f events.jsonl -o {ARCHIVE}/{tag}/events.jsonl.zst; true")
    subprocess.run(["rsync", "-az", f"{host}:{ws}/REPORT.md", f"{host}:{ws}/final.c", f"{host}:{ws}/lastgood.c",
                    f"{host}:{ws}/other_changes.txt", f"{host}:{ws}/include.patch", f"{host}:{ws}/provenance.txt", f"{host}:{ws}/stderr.log",
                    str(HERE / tag) + "/"])
    log(f"{host}:{tag} finished in {time.time() - t0:.0f}s: {r.stdout.strip()}")


def main():
    key = os.environ.get("KEY") or sys.exit("KEY not set")
    lock = threading.Lock()

    def log(msg):
        with lock:
            print(time.strftime("%H:%M:%S"), msg, flush=True)

    q = queue.Queue()
    for s in SPEC:
        q.put(s)

    def worker(host):
        while True:
            try:
                s = q.get_nowait()
            except queue.Empty:
                return
            tag = s["tag"]
            try:
                prepare(host, tag, s)
                run(host, tag, s["unit"], key, log)
            except Exception as e:
                log(f"{host}:{tag} ERROR {e}")

    log(f"{len(SPEC)} functions, {WORKERS} at a time across {', '.join(HOSTS)}")
    threads = [threading.Thread(target=worker, args=(HOSTS[i % len(HOSTS)],)) for i in range(WORKERS)]
    for th in threads:
        th.start()
        time.sleep(START_STAGGER)  # stagger copies and pi startup on every host
    for th in threads:
        th.join()
    log("all done")


if __name__ == "__main__":
    main()
