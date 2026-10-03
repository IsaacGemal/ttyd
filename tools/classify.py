#!/usr/bin/env python3
"""Bucket every unfinished function by what still differs, and list the units closest to complete.

    python3 tools/classify.py [--min 90] [--units]        needs build/G8MJ01/report.json (ninja report) first

Idea from zcanann/bfbb's tools/classify.py (CC0). For each function scoring below 100% (and at least --min), the
instruction diff from tools/agent_check.py is read and the function lands in one bucket:

  MISSING  scores 0 (no code yet)
  POOL     only symbol names/addresses differ (pooled constant, string or template at another address)
  REGS     same instructions, only register numbers differ: permuter work
  SCHED    the same instructions in a different order (equal inserts and deletes)
  SIZE     a different number of instructions
  OTHER    operands or opcodes differ

Units are ranked by how many unfinished functions they have; a unit whose only unfinished function is POOL or REGS is
the cheapest to finish. Output is plain text so it can go in a PR body."""
import json
import re
import subprocess
import sys
from collections import defaultdict

args = sys.argv[1:]
MIN = float(args[args.index("--min") + 1]) if "--min" in args else 90.0
report = json.load(open("build/G8MJ01/report.json"))["units"]
REG = re.compile(r"\b[rf]\d+\b")


def classify(unit, fn):
    mod = unit.split("/", 1)
    path = mod[1] if mod[0] == "MarioSt" else unit
    out = subprocess.run(["python3", "tools/agent_check.py", path, fn], capture_output=True, text=True, stdin=subprocess.DEVNULL).stdout
    ins = dele = 0
    regs = other = pool = 0
    for l in out.splitlines():
        if l.startswith("~~"):
            pool += 1
        elif l.startswith("!!"):
            if l.rstrip().endswith("insert"):
                ins += 1
            elif l.rstrip().endswith("delete"):
                dele += 1
            else:
                parts = l[2:].split("|")
                if len(parts) < 2:
                    continue
                a, b = parts[0].strip(), re.sub(r"\s+\S+$", "", parts[1].strip())
                if REG.sub("R", a) == REG.sub("R", b):
                    regs += 1
                else:
                    other += 1
    if ins != dele:
        return "SIZE"
    if ins and ins == dele and not regs and not other:
        return "SCHED"
    if other or ins:
        return "OTHER"
    if regs:
        return "REGS"
    if pool:
        return "POOL"
    return "OTHER"


buckets = defaultdict(list)
per_unit = defaultdict(list)
for u in report:
    for f in u.get("functions", []):
        pct = f.get("fuzzy_match_percent", 100)
        if pct >= 100:
            continue
        per_unit[u["name"]].append((f["name"], pct))
        if pct == 0:
            buckets["MISSING"].append((u["name"], f["name"], pct))
        elif pct >= MIN:
            buckets[classify(u["name"], f["name"])].append((u["name"], f["name"], pct))
for b in ("MISSING", "POOL", "REGS", "SCHED", "SIZE", "OTHER"):
    print(f"{b}: {len(buckets[b])}")
for b in ("POOL", "REGS", "SCHED", "SIZE", "OTHER"):
    print(f"\n== {b} ==")
    for unit, fn, pct in sorted(buckets[b], key=lambda x: -x[2]):
        print(f"  {pct:6.2f}  {unit}  {fn}")
print("\n== units with exactly one unfinished function ==")
one = sorted(((fs[0][1], n, fs[0][0]) for n, fs in per_unit.items() if len(fs) == 1), reverse=True)
for pct, n, fn in one[:40]:
    print(f"  {pct:6.2f}  {n}  {fn}")
