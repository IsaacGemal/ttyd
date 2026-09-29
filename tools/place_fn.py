#!/usr/bin/env python3
"""Try a function at every top-level position of a unit's source and report which position makes the object equal the
original (rel_cmp.py), e.g. when the pool order of literals depends on where the function sits.
    python3 tools/place_fn.py SOURCE.c FUNC MOD UNIT     (UNIT as objdiff names it, e.g. unit/unit_kuriboo)"""
import re, subprocess, sys
path, fn, mod, unit = sys.argv[1:5]
text = open(path).read()
m = re.search(rf"^static USER_FUNC\({fn}\) {{\n.*?\n}}\n", text, re.M | re.S)
body = m.group(0)
rest = text[:m.start()] + text[m.end():]
# top-level chunk boundaries: blank line followed by a line at column 0 that starts a definition or a comment/pragma
cuts = [x.start() for x in re.finditer(r"\n\n(?=\S)", rest)]
obj = None
import json
obj = next(u["base_path"] for u in json.load(open("objdiff.json"))["units"] if u["name"] == f"{mod}/{unit}")
good = []
try:
    for c in cuts:
        cand = rest[:c + 2] + body + "\n" + rest[c + 2:]
        open(path, "w").write(cand)
        subprocess.run(["ninja", obj], capture_output=True)
        out = subprocess.run(["python3", "tools/rel_cmp.py", mod, unit], capture_output=True, text=True).stdout.strip().splitlines()
        real = [l for l in out if not re.match(r"\.(data|rodata|bss) size 0x", l)]
        line = rest[:c].count("\n") + 2
        print(line, "OK" if not real else real[0][:60], flush=True)
        if not real:
            good.append(line)
finally:
    open(path, "w").write(text)
print("positions that match:", good)
