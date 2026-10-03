#!/usr/bin/env python3
"""Compare a unit's function order in .text against the original object, and optionally reorder its source.

    uv run --with pyelftools python3 tools/fn_order_check.py UNIT [SRC] [--write]     e.g. unit/unit_lecture_frankli_sac rels/gor/src/unit/unit_lecture_frankli_sac.c

objdiff scores each function on its own, so a unit whose functions are all 100% can still have them in the wrong order
(a REL then fails build.sha1 with nothing else wrong; unit_lecture_frankli_sac was in this state). The tool prints whether
the built object's order equals the original's. With SRC and --write it permutes the function bodies of SRC among their
own slots into the order that emits the original .text (source order is the reverse of .text), leaving data and
inline helpers where they are, then rebuild and run it again."""
import glob
import re
import sys

from elftools.elf.elffile import ELFFile

unit = sys.argv[1]
args = [a for a in sys.argv[2:] if not a.startswith("--")]
orig = (glob.glob(f"build/G8MJ01/*/obj/{unit}.o") + glob.glob(f"build/G8MJ01/obj/{unit}.o"))[0]
built = f"build/G8MJ01/src/{unit}.o"


def order(path):
    f = ELFFile(open(path, "rb"))
    st, idx = f.get_section_by_name(".symtab"), f.get_section_index(".text")
    fs = [(s["st_value"], s.name) for s in st.iter_symbols() if s["st_shndx"] == idx and s["st_info"]["type"] == "STT_FUNC"]
    return [n for _, n in sorted(fs)]


want_text, have_text = order(orig), order(built)
print(unit, "order matches" if want_text == have_text else "ORDER DIFFERS")
if want_text != have_text:
    print(" want:", want_text)
    print(" have:", have_text)
if "--write" not in sys.argv or want_text == have_text:
    sys.exit(0 if want_text == have_text else 1)

src = args[0]
want = list(reversed(want_text))
L = open(src).read().split("\n")
blocks, i = [], 0
while i < len(L):
    l = L[i]
    m = re.match(r"^(?:static\s+)?USER_FUNC\((\w+)\)\s*\{\s*$", l) or re.match(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\(.*\)\s*\{\s*$", l)
    if m and not l.startswith(("if", "for", "while", "switch", "else", "typedef", "struct", "enum", "union", "return")) and "=" not in l.split("(")[0]:
        j = i
        while j < len(L) and L[j] != "}":
            j += 1
        s = i
        while s > 0 and L[s - 1].startswith(("//", "#pragma", "/*", " *")):
            s -= 1
        blocks.append((s, j + 1, m.group(1), l))
        i = j + 1
    else:
        i += 1
fixed = [b for b in blocks if b[3].startswith(("static inline", "inline")) or b[2] not in want]
mv = [b for b in blocks if b not in fixed]
by = {b[2]: b for b in mv}
order_b = [by[n] for n in want if n in by]
assert len(order_b) == len(mv), set(by) - set(want)
slots = sorted(mv, key=lambda b: b[0])
out, pos = [], 0
for slot, nb in zip(slots, order_b):
    out += L[pos:slot[0]] + L[nb[0]:nb[1]]
    pos = slot[1]
out += L[pos:]
open(src, "w").write("\n".join(out))
print("reordered", len(mv), "functions; fixed in place:", [b[2] for b in fixed])
