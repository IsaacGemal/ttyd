#!/usr/bin/env python3
"""Place several functions independently (non-deferred modules: each function's literals pool where it sits).
    python3 tools/place_fns.py SOURCE.c MOD UNIT FUNC [FUNC ...]      FUNCs in text (= source) order
Greedy: puts the first function at the top-level position that matches the original .rodata longest (all later functions
at the end of the file), fixes it, then the next, and so on; applies the result and prints the final compare."""
import re, subprocess, sys, json
path, mod, unit, *fns = sys.argv[1:]
text = original = open(path).read()
obj = next(u["base_path"] for u in json.load(open("objdiff.json"))["units"] if u["name"] == f"{mod}/{unit}")
def grab(t, fn):
    m = re.search(rf"(?:#pragma dont_inline on\n)?(?:static )?(?:USER_FUNC\({fn}\)|\w+ {fn}\([^)]*\)) \{{\n.*?\n\}}\n(?:#pragma dont_inline reset\n)?", t, re.S)
    return m
blocks = {}
for fn in fns:
    m = grab(text, fn)
    if not m:
        sys.exit(f"{fn}: no definition found in {path}")
    blocks[fn] = m.group(0)
    text = text[:m.start()] + text[m.end():]
def orig_rodata():
    from elftools.elf.elffile import ELFFile
    f = ELFFile(open(f"build/G8MJ01/{mod}/obj/{unit}.o", "rb"))
    s = f.get_section_by_name(".rodata")
    return s.data() if s else b""
def ours_rodata():
    from elftools.elf.elffile import ELFFile
    f = ELFFile(open(obj, "rb"))
    s = f.get_section_by_name(".rodata")
    return s.data() if s else b""
want = orig_rodata()
def build(t):
    """The trial's .rodata, or None if it doesn't build (a stale object must not be scored)."""
    open(path, "w").write(t)
    if subprocess.run(["ninja", obj], capture_output=True).returncode:
        return None
    return ours_rodata()
def prefix(a, b):
    n = 0
    while n < min(len(a), len(b)) and a[n] == b[n]:
        n += 1
    return n
placed = {}   # fn -> cut offset in `text`
def assemble(pl):
    out = text
    # insert from the last cut to the first so offsets stay valid; functions with the same cut keep text order
    for fn, cut in sorted(pl.items(), key=lambda kv: (-kv[1], -fns.index(kv[0]))):
        out = out[:cut] + blocks[fn] + "\n" + out[cut:]
    return out
cuts = [x.start() + 2 for x in re.finditer(r"\n\n(?=\S)", text)] + [len(text)]
try:
    for k, fn in enumerate(fns):
        best = (-1, None)
        for c in cuts:
            trial = dict(placed)
            trial[fn] = c
            for later in fns[k + 1:]:
                trial[later] = len(text)
            got = build(assemble(trial))
            p = -1 if got is None else prefix(want, got)
            if p > best[0]:
                best = (p, c)
            if p == len(want):
                break
        if best[1] is None:
            sys.exit(f"{fn}: no position builds")
        placed[fn] = best[1]
        print(fn, "at line", text[:best[1]].count("\n") + 1, "prefix", best[0], "of", len(want), flush=True)
    final = assemble({**placed})
    open(path, "w").write(final)
    done = True
finally:
    if not globals().get("done"):
        open(path, "w").write(original)  # interrupted or failed: leave the source as it was
if subprocess.run(["ninja", obj], capture_output=True).returncode:
    sys.exit("the chosen placement doesn't build")
print(subprocess.run(["python3", "tools/rel_cmp.py", mod, unit], capture_output=True, text=True).stdout.strip()[:300])
