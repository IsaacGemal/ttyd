#!/usr/bin/env python3
"""Find how many of a dol unit's named .sdata2 f32 constants (in address order) must stay named for the code to match.

    python3 tools/pool_prefix.py UNIT [--apply]          e.g. effect/n64/eff_kemuri9_n64

Named constants (defined after the code) precede every pooled literal, so the pool of the original is reproduced by a named
prefix and literals for the rest. For k = 0..n it runs tools/pool_floats.py UNIT --keep=<first k names>, builds, and
reports the functions' scores and whether the .sdata2 bytes equal the original's. With --apply the smallest k with every
function at 100% and .sdata2 equal is left in the source. Start from the unit as it is on main (named extern constants)."""
import os
import re
import subprocess
import sys

unit = sys.argv[1]
apply = "--apply" in sys.argv
src = f"src/{unit}.c"
asm = open(f"build/G8MJ01/asm/{unit}.s", errors="replace").read()
names = []
for m in re.finditer(r"# \.sdata2:0x[0-9A-F]+ \| 0x[0-9A-F]+ \| size: 0x4\n\.obj (float_\w+), ", asm):
    names.append(m.group(1))
orig = open(src).read()
if "--no-define" not in sys.argv:  # the named constants must have definitions (defined after the code) before the search
    subprocess.run(["python3", "tools/pool_define.py", unit], capture_output=True, text=True)
    orig = open(src).read()


def run(k):
    open(src, "w").write(orig)
    keep = ",".join(names[:k])
    cmd = ["python3", "tools/pool_floats.py", unit] + ([f"--keep={keep}"] if keep else [])
    subprocess.run(cmd, capture_output=True, text=True)
    r = subprocess.run(["python3", "tools/agent_check.py", unit], capture_output=True, text=True).stdout
    if "BUILD FAILED" in r:
        return None, False
    scores = [float(x) for x in re.findall(r"([\d.]+)%\s+\d+ B", r)]
    o = subprocess.run(["python3", "/tmp/g/rod.py", unit, "x"], capture_output=True, text=True).stdout
    eq = any(l.startswith(".sdata2") and l.rstrip().endswith("EQ") for l in o.split("\n"))
    return min(scores) if scores else None, eq


best = None
for k in range(len(names) + 1):
    worst, eq = run(k)
    print(f"keep {k:2d} named: worst function {worst}  sdata2 {'EQ' if eq else 'differs'}", flush=True)
    if worst == 100.0 and eq and best is None:
        best = k
        if apply:
            break
if apply and best is not None:
    run(best)
    print(f"applied: {best} named")
else:
    open(src, "w").write(orig)
