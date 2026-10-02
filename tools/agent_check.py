#!/usr/bin/env python3
"""Agent helper for TTYD. Run from the workspace root.

  python3 check.py UNIT          rebuild, then list UNIT's functions with match % (smallest unmatched first)
  python3 check.py UNIT FUNC     rebuild, then show FUNC's instructions: original (left) vs yours (right)
  python3 check.py --final UNIT  before you stop: rebuild, then run the merge's checks and list what to fix

UNIT is the source path without src/ and .c, e.g. driver/mapdrv. 100% means byte-identical.
"""
import json, re, subprocess, sys, time

FINAL = sys.argv[1] == "--final"
if FINAL:
    sys.argv.pop(1)
unit_path = sys.argv[1]
import glob, os
# a REL unit is MOD/UNIT (e.g. gor/gor_12), its source rels/MOD/src/UNIT.c; a dol unit is src/UNIT.c
mod, _, rest = unit_path.partition("/")
REL = bool(rest) and os.path.isdir(f"config/G8MJ01/{mod}") and mod != "G8MJ01"
unit = unit_path if REL else "MarioSt/" + unit_path
# Read the project once; the report and the build target use the same entry.
project = json.load(open("objdiff.json"))
entry = next((e for e in project["units"] if e["name"] == unit), None)
# the unit's source, as objdiff.json names it (libs/<lib>/src/..., any extension); else src/UNIT.c or a library's
src = entry.get("metadata", {}).get("source_path") if entry else None
src = src or next(iter(glob.glob(f"libs/*/src/{unit_path}.[cC]")), None) if not os.path.exists(f"src/{unit_path}.c") else src
src = src or (f"rels/{mod}/src/{rest}.c" if REL else f"src/{unit_path}.c")
if os.path.exists(src) and src not in open("build.ninja").read():
    subprocess.run(["python3", "configure.py"], capture_output=True)  # a new source file needs a build rule
# Build and score only this unit: its object, then objdiff's report for a one-unit copy of objdiff.json (the same
# scoring as the full report, which re-diffs every object in the project: ~95% of a check's CPU with 48 agents).
# --final also compiles every source (a header change could break another file), and the merge builds everything.
target = entry.get("base_path") if entry else None
jobs = ["-j2"]  # two waves of agents share the tower's 16 cores
# `#pragma scheduling <cpu>` (601, 801, ...) makes this mwcceppc loop forever (waves 53 and 57: compiles hung for a day).
if os.path.exists(src) and re.search(r"^\s*#\s*pragma\s+scheduling\s+\d", open(src, errors="replace").read(), re.M):
    print("BUILD REFUSED: `#pragma scheduling <cpu number>` makes this compiler hang forever. Remove it; use "
          "`#pragma scheduling off` / `reset` if you need to change scheduling.")
    sys.exit(1)
t0 = time.time()
b = subprocess.run(["ninja", *jobs] + ([target] if target else []), capture_output=True, text=True)
if b.returncode == 0 and FINAL:
    b = subprocess.run(["ninja", *jobs, "all_source"], capture_output=True, text=True)
if b.returncode:
    out = b.stdout + b.stderr
    # tools/project.py caps every compile at TTYD_COMPILE_TIMEOUT seconds; a build that failed after that long was
    # killed there: the compiler loops forever on this source.
    if time.time() - t0 >= int(os.environ.get("TTYD_COMPILE_TIMEOUT", 300)) - 1:
        print("BUILD FAILED: THE COMPILER HUNG on this source and was killed after "
              f"{int(time.time() - t0)} s (a normal compile takes about a second). Your last edit makes mwcceppc loop "
              "forever: undo it (lastgood.c is the last version that built) and write that part differently.")
        sys.exit(1)
    errs = [l for l in out.splitlines() if "rror" in l or "###" in l or l.startswith("#")]
    print("BUILD FAILED\n" + "\n".join((errs or out.splitlines())[-40:]))
    sys.exit(1)
if os.path.exists(src):
    import shutil
    shutil.copy(src, "lastgood.c")  # the last version that built: survives an agent cut off mid-edit
def signature():
    # A source edit can compile to identical bytes. The scorer only needs the two objects and project settings.
    paths = ("objdiff.json", target, entry.get("target_path") if entry else None)
    return [[p, os.stat(p).st_size, os.stat(p).st_mtime_ns] for p in paths if p and os.path.exists(p)]


cache_path = os.path.join(".cache", "check-cache-" + unit.replace("/", "_") + ".json")
key = signature()
try:
    cache = json.load(open(cache_path))
    if cache.get("key") != key:
        cache = {"key": key}
except (OSError, ValueError):
    cache = {"key": key}


def save_cache():
    os.makedirs(".cache", exist_ok=True)
    with open(cache_path, "w") as out:
        json.dump(cache, out)


if target:
    report = cache.get("report")
    if report is None:
        import tempfile
        one = dict(project, units=[dict(entry, **{k: os.path.abspath(entry[k]) for k in ("target_path", "base_path")
                                                    if entry.get(k)})])
        with tempfile.TemporaryDirectory() as d:
            json.dump(one, open(os.path.join(d, "objdiff.json"), "w"))
            r = subprocess.run(["build/tools/objdiff-cli", "report", "generate", "-p", d, "-o", "-"], capture_output=True,
                               text=True, env={**os.environ, "RAYON_NUM_THREADS": "1"})
        if r.returncode:
            sys.exit("objdiff report failed:\n" + r.stderr[-2000:])
        report = json.loads(r.stdout)
        cache["report"] = report
        save_cache()
else:
    report = json.load(open("build/G8MJ01/report.json"))  # no source object yet: the full build's report
u = next((x for x in report["units"] if x["name"] == unit), None)
if u is None:
    sys.exit(f"unit {unit} not in report")
fns = {f["name"]: f for f in u.get("functions", [])}
if FINAL:
    # The same checks the merge runs (tools/lint_patch.py), against the clean copy of the file this workspace was
    # made from, or /dev/null for a new file.
    base = os.path.join(os.path.expanduser(os.environ.get("TTYD_BASE", "~/ttyd")), src)
    base = base if os.path.exists(base) else "/dev/null"
    linked = None
    if REL:
        # a REL unit is done when it links: its object equals the original and the REL's checksum passes
        link = subprocess.run(["python3", "tools/rel_link.py", unit_path], capture_output=True, text=True)
        linked = link.stdout.strip() if link.returncode == 0 else ("NOT LINKED: tools/rel_link.py failed (exit "
                  f"{link.returncode})\n" + (link.stdout + link.stderr).strip()[-2000:])
    import tempfile
    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False) as rep:
        json.dump(report, rep)  # this unit's fresh report, so the merge checks see the build just made
    try:
        lint = subprocess.run(["python3", "tools/lint_patch.py", src, base], capture_output=True, text=True,
                              env={**os.environ, "LINT_PATCH_REPORT": rep.name})
    finally:
        os.unlink(rep.name)
    fns = u.get("functions", [])
    done = sum(1 for f in fns if f.get("fuzzy_match_percent", 0) >= 100)
    print(f"{unit}: builds; {done}/{len(fns)} functions at 100%.")
    todo = [l for l in lint.stdout.splitlines() if l.strip()]
    if linked is not None:
        print(linked)
        if not linked.startswith("LINKED"):
            todo.append("LINK  not linked yet (see above)")
    # Function prototypes belong in the header of the file that defines the function, not in your .c file.
    sys.path.insert(0, "tools")
    import local_prototypes
    exc = [e for e in json.load(open("tools/local_prototype_exceptions.json")) if e["path"] in (src, "*")]
    # libraries (libs/) keep their own declarations; the header rule is for game code, in the dol and the RELs
    protos = local_prototypes.check_sources({src: open(src).read()}, exc) if src.startswith(("src/", "rels/")) else []
    for p in protos:
        if "exception must match" in p:
            todo.append("PROTOTYPE  " + p + "  -> a documented exception no longer matches your file; restore that"
                        " declaration exactly, or say in your report why it should go (only the orchestrator edits"
                        " the exceptions list)")
        elif "static" in p:
            todo.append("PROTOTYPE  " + p)
        else:
            todo.append("PROTOTYPE  " + p + "  -> move it to the defining file's header (include/<path>.h)")
    if not todo:
        print("Merge checks: nothing to fix.")
        sys.exit(0)
    print("Merge checks: fix these before you stop (STUB rejects the whole file; the rest reviewers will flag):")
    for l in todo:
        print("  " + l)
    print("UNLABELLED: add `// NONMATCHING (NN%): what differs` above it. OFFSET: use the header's field if it has one.\n"
          "EXTERN/STRUCT: a function or struct declared locally should come from its header if one declares it (data\n"
          "externs such as float constants are fine). PRAGMA: give it a reason or remove it.")
    sys.exit(1 if lint.returncode or protos or (linked is not None and not linked.startswith("LINKED")) else 0)
if len(sys.argv) == 2:
    rows = sorted(fns.values(), key=lambda f: (f.get("fuzzy_match_percent", 0) >= 100, int(f["size"])))
    done = sum(1 for f in rows if f.get("fuzzy_match_percent", 0) >= 100)
    print(f"{unit}: {done}/{len(rows)} functions at 100%")
    for f in rows:
        print(f"  {f.get('fuzzy_match_percent', 0):6.2f}%  {int(f['size']):5d} B  {f['name']}")
    sys.exit(0)
func = sys.argv[2]
f = fns.get(func)
print(f"{func}: {f.get('fuzzy_match_percent', 0):.2f}% ({f['size']} B)" if f else f"{func}: not in unit report")
# instructions only: the percentage above comes from the report (objdiff's diff can score a function differently)
d = cache.get("diffs", {}).get(func) if target else None
if d is None:
    r = subprocess.run(["build/tools/objdiff-cli", "diff", "-p", ".", "-u", unit, func, "-o", "-", "--format", "json"],
                       capture_output=True, text=True, env={**os.environ, "RAYON_NUM_THREADS": "1"})
    try:
        d = json.loads(r.stdout) if r.returncode == 0 else None
    except ValueError:
        d = None
    if d is not None and target:
        # The CLI returns every symbol in the unit; retain only the requested function's instructions.
        cache.setdefault("diffs", {})[func] = {
            side: {"symbols": [s for s in d.get(side, {}).get("symbols", []) if s.get("name") == func]}
            for side in ("left", "right")
        }
        save_cache()
if d is None:
    sys.exit("objdiff produced no diff (is the function defined in your source?)")


def insts(side):
    for s in d.get(side, {}).get("symbols", []):
        if s.get("name") == func:
            return s.get("instructions", [])
    return []


left, right = insts("left"), insts("right")
if not right:
    print("(not compiled from your source yet: define it)")


def no_symbols(text):
    """A line with the symbol operands blanked: the unit is not linked, so the original names data by symbol
    (float_0_804210b0@sda21, str_802EF8B0@ha) where our object has a local one (@1910@ha, ...rodata.0@l)."""
    text = re.sub(r"@\d+(@ha|@l|@sda21)?", "SYM", text)
    text = re.sub(r"[A-Za-z_.$][\w$.]*@(ha|l|sda21|sda2)", "SYM", text)
    return re.sub(r"\s+", " ", text).strip()


real = symbol_only = 0
rows = []
for i in range(max(len(left), len(right))):
    a = left[i] if i < len(left) else {}
    c = right[i] if i < len(right) else {}
    fa = a.get("instruction", {}).get("formatted", "")
    fc = c.get("instruction", {}).get("formatted", "")
    kind = a.get("diff_kind") or c.get("diff_kind") or ""
    if not kind or fa == fc:
        mark = "  "
    elif fa and fc and no_symbols(fa) == no_symbols(fc):
        mark, symbol_only = "~~", symbol_only + 1  # only the symbol differs: not a diff once the unit links
    else:
        mark, real = "!!", real + 1
    rows.append(f"{mark} {fa:42} | {fc:42} {kind.replace('DIFF_', '').lower() if mark == '!!' else 'symbol-only' if mark == '~~' else ''}")
print("\n".join(rows))
if f and (real or symbol_only):
    print(f"-- {real} instruction diff(s) to fix" + (f", {symbol_only} that differ only in the data symbol (!! is real, ~~ is "
          "not: it disappears when the unit links; do not chase it)" if symbol_only else ""))
