#!/usr/bin/env python3
"""Try a function at every top-level position of a unit's source and report which position makes the object equal the
original (rel_cmp.py), e.g. when the pool order of literals depends on where the function sits.
    python3 tools/place_fn.py SOURCE.c FUNC MOD UNIT     (UNIT as objdiff names it, e.g. unit/unit_kuriboo)"""
import re, subprocess, sys
path, fn, mod, unit = sys.argv[1:5]
text = open(path).read()
m = re.search(rf"^static USER_FUNC\({fn}\) {{\n.*?\n}}\n", text, re.M | re.S)
if not m:
    sys.exit(f"{fn}: no `static USER_FUNC({fn}) {{ ... }}` definition in {path}")
body = m.group(0)
rest = text[:m.start()] + text[m.end():]
# top-level chunk boundaries: blank line followed by a line at column 0 that starts a definition or a comment/pragma
cuts = [x.start() for x in re.finditer(r"\n\n(?=\S)", rest)]
obj = None
import json
obj = next(u["base_path"] for u in json.load(open("objdiff.json"))["units"] if u["name"] == f"{mod}/{unit}")
# where the original's last real object in each section ends: a shorter section is only padding if it keeps them all
ends = {}
for m in re.finditer(r"^# (\.\w+):(0x[0-9A-F]+) \| \S+ \| size: (0x[0-9A-F]+)\n\.obj (\S+?),", open(f"build/G8MJ01/{mod}/asm/{unit}.s").read(), re.M):
    if not m.group(4).startswith("gap_"):
        ends[m.group(1)] = max(ends.get(m.group(1), 0), int(m.group(2), 16) + int(m.group(3), 16))
good = []
try:
    for c in cuts:
        cand = rest[:c + 2] + body + "\n" + rest[c + 2:]
        open(path, "w").write(cand)
        line = rest[:c].count("\n") + 2
        b = subprocess.run(["ninja", obj], capture_output=True, text=True)
        if b.returncode:
            print(line, "build fails", flush=True)
            continue
        r = subprocess.run(["python3", "tools/rel_cmp.py", mod, unit], capture_output=True, text=True)
        out = [l for l in r.stdout.strip().splitlines() if l != "OK"]
        # a section a few bytes short is its trailing alignment padding (the linker adds it back); anything else differs
        pad = lambda l: (lambda m: m and 0 < int(m.group(2), 16) - int(m.group(3), 16) < 8
                         and int(m.group(3), 16) >= ends.get(m.group(1), 0))(
            re.match(r"(\.(?:data|rodata|bss)) size (0x[0-9a-f]+) \(original\) vs (0x[0-9a-f]+)$", l))
        real = [l for l in out if not pad(l)]
        ok = (r.returncode == 0 and "OK" in r.stdout.splitlines()) or (r.returncode == 1 and out and not real)
        if r.returncode not in (0, 1) or not (ok or real):
            real = [f"rel_cmp failed: {(r.stderr.strip().splitlines() or ['no output'])[-1]}"]
        print(line, "OK" if ok else real[0][:60], flush=True)
        if ok:
            good.append(line)
finally:
    open(path, "w").write(text)
    subprocess.run(["ninja", obj], capture_output=True)  # don't leave the last trial's object behind
print("positions that match:", good)
