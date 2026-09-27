#!/usr/bin/env python3
"""Agent helper for TTYD. Run from the workspace root.

  python3 check.py UNIT          rebuild, then list UNIT's functions with match % (smallest unmatched first)
  python3 check.py UNIT FUNC     rebuild, then show FUNC's instructions: original (left) vs yours (right)
  python3 check.py --final UNIT  before you stop: rebuild, then run the merge's checks and list what to fix

UNIT is the source path without src/ and .c, e.g. driver/mapdrv. 100% means byte-identical.
"""
import json, subprocess, sys

FINAL = sys.argv[1] == "--final"
if FINAL:
    sys.argv.pop(1)
unit_path = sys.argv[1]
unit = "MarioSt/" + unit_path
import glob, os
# the unit's source, as objdiff.json names it (libs/<lib>/src/..., any extension); else src/UNIT.c or a library's
src = next((u.get("metadata", {}).get("source_path") for u in json.load(open("objdiff.json"))["units"]
            if u["name"] == unit), None) if os.path.exists("objdiff.json") else None
src = src or next(iter(glob.glob(f"libs/*/src/{unit_path}.[cC]")), None) if not os.path.exists(f"src/{unit_path}.c") else src
src = src or f"src/{unit_path}.c"
if os.path.exists(src) and src not in open("build.ninja").read():
    subprocess.run(["python3", "configure.py"], capture_output=True)  # a new source file needs a build rule
# Build and score only this unit: its object, then objdiff's report for a one-unit copy of objdiff.json (the same
# scoring as the full report, which re-diffs every object in the project: ~95% of a check's CPU with 48 agents).
# --final also compiles every source (a header change could break another file), and the merge builds everything.
project = json.load(open("objdiff.json"))
entry = next((e for e in project["units"] if e["name"] == unit), None)
target = entry.get("base_path") if entry else None
jobs = ["-j2"]  # two waves of agents share the tower's 16 cores
b = subprocess.run(["ninja", *jobs] + ([target] if target else []), capture_output=True, text=True)
if b.returncode == 0 and FINAL:
    b = subprocess.run(["ninja", *jobs, "all_source"], capture_output=True, text=True)
if b.returncode:
    out = b.stdout + b.stderr
    errs = [l for l in out.splitlines() if "rror" in l or "###" in l or l.startswith("#")]
    print("BUILD FAILED\n" + "\n".join((errs or out.splitlines())[-40:]))
    sys.exit(1)
if os.path.exists(src):
    import shutil
    shutil.copy(src, "lastgood.c")  # the last version that built: survives an agent cut off mid-edit
if target:
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
    lint = subprocess.run(["python3", "tools/lint_patch.py", src, base], capture_output=True, text=True)
    fns = u.get("functions", [])
    done = sum(1 for f in fns if f.get("fuzzy_match_percent", 0) >= 100)
    print(f"{unit}: builds; {done}/{len(fns)} functions at 100%.")
    todo = [l for l in lint.stdout.splitlines() if l.strip()]
    # Function prototypes belong in the header of the file that defines the function, not in your .c file.
    sys.path.insert(0, "tools")
    import local_prototypes
    exc = [e for e in json.load(open("tools/local_prototype_exceptions.json")) if e["path"] in (src, "*")]
    # libraries (libs/) keep their own declarations; the header rule is for game code
    protos = local_prototypes.check_sources({src: open(src).read()}, exc) if src.startswith("src/") else []
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
    sys.exit(1 if lint.returncode or protos else 0)
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
r = subprocess.run(["build/tools/objdiff-cli", "diff", "-p", ".", "-u", unit, func, "-o", "-", "--format", "json"],
                   capture_output=True, text=True, env={**os.environ, "RAYON_NUM_THREADS": "1"})
try:
    d = json.loads(r.stdout) if r.returncode == 0 else None
except ValueError:
    d = None
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
for i in range(max(len(left), len(right))):
    a = left[i] if i < len(left) else {}
    c = right[i] if i < len(right) else {}
    fa = a.get("instruction", {}).get("formatted", "")
    fc = c.get("instruction", {}).get("formatted", "")
    kind = a.get("diff_kind") or c.get("diff_kind") or ""
    mark = "  " if not kind or fa == fc else "!!"
    print(f"{mark} {fa:42} | {fc:42} {kind.replace('DIFF_', '').lower() if mark == '!!' else ''}")
