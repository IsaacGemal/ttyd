#!/usr/bin/env python3
"""Compare the shape of a function against the original, ignoring register names and stack offsets.

  python3 tools/diff_shape.py UNIT FUNC

Runs tools/agent_check.py UNIT FUNC and prints (1) whether the sequence of `bl` targets is the same, (2) the
immediates (li/cmpwi/cmplwi/ori/addi/rlwinm ...) the original has that yours lacks, and the reverse. A function at
80% whose calls and immediates all match differs only in scheduling and registers; one where they differ has a
structural difference (a missing branch, an inlined helper, a duplicated tail, a wrong constant) to fix first.
"""
import collections, re, subprocess, sys

out = subprocess.run(["python3", "tools/agent_check.py", sys.argv[1], sys.argv[2]], capture_output=True, text=True).stdout
orig, ours = [], []
for line in out.splitlines():
    if len(line) < 90:
        continue
    a, b = line[3:46].strip(), line[48:90].strip()
    if a:
        orig.append(a)
    if b:
        ours.append(b)
print(out.splitlines()[0] if out else "no output")


def calls(x):
    return [i.split()[1] for i in x if i.startswith("bl ")]


def imms(x):
    c = collections.Counter()
    for i in x:
        m = re.match(r"(li|cmpwi|cmplwi|ori|oris|rlwinm\.?|addi|subi|clrlwi\.?|clrrwi\.?|andi\.?) .*?(-?0x[0-9a-f]+|-?\d+)$", i)
        if m and "(r1)" not in i and ", r1," not in i:
            c[(m.group(1).rstrip("."), m.group(2))] += 1
    return c


co, cu = calls(orig), calls(ours)
print("calls:", "same" if co == cu else "DIFFER", len(co), len(cu))
if co != cu:
    print("  original:", co)
    print("  yours:   ", cu)
a, b = imms(orig), imms(ours)
print("only in original:", dict(a - b))
print("only in yours:   ", dict(b - a))
