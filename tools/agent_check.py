#!/usr/bin/env python3
"""Agent helper for TTYD. Run from the workspace root.

  python3 check.py UNIT          rebuild, then list UNIT's functions with match % (smallest unmatched first)
  python3 check.py UNIT FUNC     rebuild, then show FUNC's instructions: original (left) vs yours (right)

UNIT is the source path without src/ and .c, e.g. driver/mapdrv. 100% means byte-identical.
"""
import json, subprocess, sys

unit_path = sys.argv[1]
unit = "MarioSt/" + unit_path
import os
if os.path.exists(f"src/{unit_path}.c") and f"src/{unit_path}.c" not in open("build.ninja").read():
    subprocess.run(["python3", "configure.py"], capture_output=True)  # a new source file needs a build rule
b = subprocess.run(["ninja"], capture_output=True, text=True)
if b.returncode:
    out = b.stdout + b.stderr
    errs = [l for l in out.splitlines() if "rror" in l or "###" in l or l.startswith("#")]
    print("BUILD FAILED\n" + "\n".join((errs or out.splitlines())[-40:]))
    sys.exit(1)
report = json.load(open("build/G8MJ01/report.json"))
u = next((x for x in report["units"] if x["name"] == unit), None)
if u is None:
    sys.exit(f"unit {unit} not in report")
fns = {f["name"]: f for f in u.get("functions", [])}
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
subprocess.run(["build/tools/objdiff-cli", "diff", "-p", ".", "-u", unit, func, "-o", "build/_od.json",
                "--format", "json"], capture_output=True)
try:
    d = json.load(open("build/_od.json"))
except Exception:
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
