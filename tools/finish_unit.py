#!/usr/bin/env python3
"""Finish a code-complete dol unit's data so that it links: the mechanical steps of the pool-finisher method.

    python3 tools/finish_unit.py UNIT [--data-only]        e.g. effect/n64/eff_kemuri4_n64

UNIT must have every function at 100%, or pass --data-only: then any unit is accepted, a step is kept when no function scores
lower than it did before and no section score falls, and the link is not attempted (the point is the unit's data sections). The tool tries, in order, keeping a step only if no function drops below 100% and no
section score falls:
  1. tools/pool_data.py     vertex tables of the unit's own .data
  2. math.h -> math_ext.h   (math.h's sqrt inline emits two extra doubles into .rodata)
  3. tools/pool_define.py   named .sdata2/.rodata pool constants, defined after the code
  4. named string objects -> literals (`const char str_XXXX[] = "..."` and its uses)
  5. the .sbss2 idiom       for uninitialised `GXColor unk_804...;` scratch objects
then runs tools/autolink.py on it. It prints each step's result and, when the link still fails, which sections differ from
the original object. What it cannot do (event scripts and other typed tables: tools/dol_gen.py --append; base-reached data:
`#pragma force_active`; scheduling differences) is listed at the end. Run from the repo root; edits src/UNIT.c in place."""
import json
import os
import re
import subprocess
import sys
import tempfile

unit = sys.argv[1]
DATA_ONLY = "--data-only" in sys.argv
src = f"src/{unit}.c"
project = json.load(open("objdiff.json"))
entry = next(e for e in project["units"] if e["name"] == "MarioSt/" + unit)


def measure():
    """({function: %}, {section: %}, build ok) of the unit as it is now."""
    b = subprocess.run(["ninja", entry["base_path"]], capture_output=True, text=True)
    if b.returncode:
        return None, None, False
    one = dict(project, units=[dict(entry, **{k: os.path.abspath(entry[k]) for k in ("target_path", "base_path") if entry.get(k)})])
    with tempfile.TemporaryDirectory() as d:
        json.dump(one, open(os.path.join(d, "objdiff.json"), "w"))
        r = subprocess.run(["build/tools/objdiff-cli", "report", "generate", "-p", d, "-o", "-"], capture_output=True, text=True,
                           env={**os.environ, "RAYON_NUM_THREADS": "1"})
    u = json.loads(r.stdout)["units"][0]
    fns = {f["name"]: f.get("fuzzy_match_percent", 0.0) for f in u.get("functions", [])}
    secs = {s["name"]: s.get("fuzzy_match_percent", 0.0) for s in u.get("sections", [])}
    return fns, secs, True


def summary(secs):
    return " ".join(f"{k}:{v:.0f}" for k, v in secs.items() if k != ".text")


fns, secs, ok = measure()
if not ok:
    sys.exit(f"{unit} does not build")
worst = min(fns.values()) if fns else 0
print(f"{unit}: {len(fns)} functions, worst {worst:.2f}%; sections {summary(secs)}")
base_fns = dict(fns)
if worst < 100 and not DATA_ONLY:
    sys.exit("not every function is at 100%: nothing to finish yet")
todo = []


def step(name, fn):
    global fns, secs
    before_text = open(src).read()
    before_secs = secs
    try:
        changed = fn()
    except Exception as e:  # a step that cannot run is simply skipped
        print(f"  {name}: skipped ({e})")
        return
    if changed is False:
        print(f"  {name}: nothing to do")
        return
    f2, s2, ok2 = measure()
    fn_ok = all(f2.get(k, 0) >= v - 1e-9 for k, v in base_fns.items()) if DATA_ONLY else min(f2.values()) >= 100
    good = ok2 and fn_ok and all(s2.get(k, 0) >= v - 1e-9 for k, v in before_secs.items())
    if good:
        fns, secs = f2, s2
        print(f"  {name}: kept; sections {summary(secs)}")
    else:
        open(src, "w").write(before_text)
        print(f"  {name}: reverted ({'build failed' if not ok2 else 'a function or section dropped'})")


def run_tool(tool):
    def go():
        r = subprocess.run(["python3", f"tools/{tool}.py", unit], capture_output=True, text=True)
        if r.returncode:
            raise RuntimeError((r.stdout + r.stderr).strip().splitlines()[-1] if (r.stdout + r.stderr).strip() else tool)
        return True
    return go


def math_ext():
    t = open(src).read()
    if "#include <math.h>" not in t:
        return False
    open(src, "w").write(t.replace("#include <math.h>", '#include "math_ext.h"'))


def literal_strings():
    t = open(src).read()
    defs = re.findall(r'^const char (str_\w+)\[\d*\] = ("(?:[^"\\]|\\.)*");\n', t, re.M)
    if not defs:
        return False
    for name, lit in defs:
        t = re.sub(rf'^const char {name}\[\d*\] = {re.escape(lit)};\n', "", t, flags=re.M)
        t = re.sub(rf'^extern const char {name}\[\];\n', "", t, flags=re.M)
        t = re.sub(rf"\b{name}\b", lit.replace("\\", "\\\\"), t)
    open(src, "w").write(t)


def sbss2():
    t = open(src).read()
    m = re.findall(r"^(?:GXColor|u32) (unk_8042\w+);\n", t, re.M)
    if not m or "sbss2" in t:
        return False
    names = sorted(m, reverse=True)  # declared in reverse: they are placed in the reverse of their declaration order
    decl = "".join(f'__declspec(section ".sdata") GXColor {n};\n' for n in names)
    for n in m:
        t = re.sub(rf"^(?:GXColor|u32) {n};\n", "", t, flags=re.M)
    first = t.index("\n\n") if "\n\n" in t else 0
    t = t[:first] + '\n\n// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.\n#pragma section ".sdata" ".sbss2"\n' + decl + t[first:]
    open(src, "w").write(t)


if secs.get(".data", 100) < 100 or secs.get(".sdata", 100) < 100:
    step("pool_data", run_tool("pool_data"))
step("math.h -> math_ext.h", math_ext)
if secs.get(".sdata2", 100) < 100 or secs.get(".rodata", 100) < 100:
    step("pool_define", run_tool("pool_define"))
if secs.get(".rodata", 100) < 100:
    step("literal strings", literal_strings)
if secs.get(".sbss2", 100) < 100:
    step("sbss2 idiom", sbss2)
if DATA_ONLY:
    print("done; sections", summary(secs))
    sys.exit(0)
r = subprocess.run(["python3", "tools/autolink.py", unit + ".c"], capture_output=True, text=True)
tail = (r.stdout + r.stderr).strip().splitlines()[-2:]
print("  autolink:", " / ".join(tail))
subprocess.run(["git", "checkout", "include"], capture_output=True)
bad = {k: v for k, v in secs.items() if v < 100}
if "LINKED" in "".join(tail):
    print("linked")
else:
    print("still not linked; sections below 100%:", bad or "none")
    if not bad:
        print("  every section is 100%: look for base-reached data (`#pragma force_active on/off`), alignment padding at the ends "
              "of .data/.sdata (the next unit's alignment), or a literal that the target loads by name (dol diff: "
              "`python3 tools/split_scan.py`, ledger Grind 27-29)")
    if secs.get(".data", 100) < 100 or secs.get(".sdata", 100) < 100:
        print("  .data/.sdata: event scripts or typed tables? python3 tools/dol_gen.py UNIT --append src/UNIT.c > /tmp/new.c")
