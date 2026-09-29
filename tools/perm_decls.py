#!/usr/bin/env python3
"""Search local-variable declaration orders for a function (MWCC assigns registers partly by declaration order).

    python3 tools/perm_decls.py SOURCE.c AGENT_UNIT FUNC [N]
AGENT_UNIT is what tools/agent_check.py takes (e.g. aji/unit/unit_barriern_satellite). Tries every single move of one
declaration plus N random shuffles (default 40), keeps the best order in the file and prints the scores.
Only plain declaration lines (no initializer) directly after the function's opening brace are permuted."""
import random, re, subprocess, sys

path, unit, func = sys.argv[1:4]
count = int(sys.argv[4]) if len(sys.argv) > 4 else 40
text = open(path).read()
m = re.search(rf"\b{re.escape(func)}\b[^;{{]*\)\s*{{\n", text)
if not m:
    sys.exit("function not found")
lines = text[m.end():].split("\n")
n = 0
while n < len(lines) and re.match(r"^\s+[\w\s\*]+(\[[^\]]*\])?(,\s*\**\w+(\[[^\]]*\])?)*;\s*(//.*)?$", lines[n]) and "=" not in lines[n]:
    n += 1
decls = lines[:n]
head, tail = text[:m.end()], "\n".join(lines[n:])
if n < 2:
    sys.exit("nothing to permute")


def score(order):
    open(path, "w").write(head + "\n".join(order) + "\n" + tail)
    out = subprocess.run(["python3", "tools/agent_check.py", unit, func], capture_output=True, text=True,
                         stdin=subprocess.DEVNULL).stdout
    r = re.match(rf"{re.escape(func)}: ([\d.]+)%", out)
    return float(r.group(1)) if r else -1


cands = []
for i in range(n):
    for pos in range(n):
        d = decls[:i] + decls[i + 1:]
        d.insert(pos, decls[i])
        if d != decls and d not in cands:
            cands.append(d)
random.seed(1)
for _ in range(count):
    d = decls[:]
    random.shuffle(d)
    cands.append(d)
best = (score(decls), decls)
try:
    print("base", best[0], flush=True)
    for d in cands:
        if best[0] >= 100:
            break
        s = score(d)
        if s > best[0]:
            best = (s, d)
            print(s, [x.split()[-1] for x in d], flush=True)
finally:
    score(best[1])
print("BEST", best[0])
