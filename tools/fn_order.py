#!/usr/bin/env python3
"""Put a dol unit's functions in the source order that emits the target's .text order.

    python3 tools/fn_order.py UNIT [--write]     e.g. effect/eff_bomb

MWCC emits non-asm functions in reverse source order, so the source lists them from the highest target address to the lowest.
Prints the order the unit has now and the order it should have. With --write, moves each function (with the comment lines
right above it, and a `#pragma NAME ...` / `#pragma NAME reset` pair that encloses only that function) into the slot the right order
gives it and leaves everything else (data, helpers, declarations) where it is. A pragma above a function whose `reset` does not
directly follow it stops --write: move those by hand. `#if` lines between functions are left alone: check them by hand. Rebuild afterwards with
tools/agent_check.py UNIT, and keep the change only if no function's score dropped."""
import os
import re
import sys

from elftools.elf.elffile import ELFFile

unit = sys.argv[1]
write = "--write" in sys.argv
src = f"src/{unit}.c"
target = f"build/G8MJ01/obj/{unit}.o"
_mod, _, _rest = unit.partition("/")
if _rest and os.path.isdir(f"rels/{_mod}/src"):  # a REL unit: MOD/UNIT
    src = f"rels/{_mod}/src/{_rest}.c"
    target = f"build/G8MJ01/{_mod}/obj/{_rest}.o"


def text_order(path):
    with open(path, "rb") as f:
        e = ELFFile(f)
        out = []
        for s in e.get_section_by_name(".symtab").iter_symbols():
            if s["st_info"]["type"] == "STT_FUNC" and s["st_shndx"] != "SHN_UNDEF":
                sec = e.get_section(s["st_shndx"]).name
                if sec == ".text":
                    out.append((s["st_value"], s.name))
        return [n for _, n in sorted(out)]


def split(text):
    """[(kind, text, name)] with kind 'gap' or 'fn' in file order."""
    segs, i, n, depth, start = [], 0, len(text), 0, 0
    head_from = 0  # start of the text that precedes the current '{'
    fn_start = None
    while i < n:
        c = text[i]
        two = text[i:i + 2]
        if two == "//":
            i = text.find("\n", i)
            i = n if i < 0 else i
            continue
        if two == "/*":
            i = text.find("*/", i) + 2
            continue
        if c in "\"'":
            j = i + 1
            while text[j] != c:
                j += 2 if text[j] == "\\" else 1
            i = j + 1
            continue
        if c == "#" and (i == 0 or text[i - 1] == "\n"):
            while True:
                j = text.find("\n", i)
                j = n if j < 0 else j
                if text[i:j].endswith("\\"):
                    i = j + 1
                    continue
                i = j
                break
            head_from = i
            continue
        if c == "{":
            if depth == 0:
                before = re.sub(r"//[^\n]*|/\*.*?\*/", "", text[head_from:i], flags=re.S).rstrip()
                fn_start = i if before.endswith(")") and not re.match(r"\s*(typedef|struct|enum|union)\b", before) else None
                if fn_start is not None:
                    segs.append(("head", head_from, i))
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                if fn_start is not None:
                    segs.append(("end", i + 1, None))
                    fn_start = None
                head_from = i + 1
        elif c == ";" and depth == 0:
            head_from = i + 1
        i += 1
    return segs


text = open(src).read()
if re.search(r"^\s*#\s*(pragma|if|ifdef|ifndef|else|endif)\b", text, re.M):
    # allow the standard header guards / MUST_MATCH blocks inside function bodies only
    pass
segs = split(text)
fns = []  # (start, end, name)
for k in range(0, len(segs), 2):
    _, head_from, brace = segs[k]
    end = segs[k + 1][1]
    head = re.sub(r"//[^\n]*|/\*.*?\*/", "", text[head_from:brace], flags=re.S)
    m = re.search(r"(\w+)\s*\([^()]*(?:\([^()]*\)[^()]*)*\)\s*$", head.rstrip())
    name = m.group(1) if m else None
    if name == "USER_FUNC" or name == "EVT_DEFINE_USER_FUNC":
        name = re.search(r"\(\s*(\w+)", head[head.rindex(name):]).group(1)
    # the comment/pragma lines directly above belong to the function
    s = head_from
    lines = text[:s].split("\n")
    # walk back over the text between the previous statement end and the head
    pre = text[head_from:brace]
    first = re.search(r"\S", pre)
    fstart = head_from + (first.start() if first else 0)
    # extend upward over contiguous comment/pragma lines
    before = text[:fstart].split("\n")
    take = 0
    opened = []  # `#pragma NAME ...` lines directly above (not `reset`): their reset must follow the function
    for ln in reversed(before[:-1]):
        t = ln.strip()
        pm = re.match(r"#\s*pragma\s+(\w+)\s*(\w*)", t)
        if pm:
            if pm.group(2) != "reset":
                opened.append(pm.group(1))
            take += 1
        elif t.startswith("//") or t.startswith("/*") or t.startswith("*") or t.endswith("*/"):
            take += 1
        else:
            break
    fstart -= sum(len(l) + 1 for l in before[len(before) - 1 - take:len(before) - 1])
    if opened:
        for name_ in opened:
            m2 = re.match(rf"[ \t]*\n?[ \t]*#\s*pragma\s+{name_}\s+reset[^\n]*\n?", text[end:])
            if not m2:
                sys.exit(f"{unit}: `#pragma {name_}` above a function without its `reset` right after it: move it by hand")
            end += m2.end()
    # a function's leading text must start at a line start
    ls = text.rfind("\n", 0, fstart) + 1
    if text[ls:fstart].strip() == "":
        fstart = ls
    fns.append([fstart, end, name])
tgt = text_order(target)
present = [f for f in fns if f[2] in tgt]
have = [f[2] for f in present]
want = [n for n in reversed(tgt) if n in have]
print(f"{unit}: source order {len(have)} functions")
if have == want:
    print("already in order")
    sys.exit(0)
print("now :", " ".join(have))
print("want:", " ".join(want))
if len(set(have)) != len(have):
    sys.exit("duplicate names: not handled")
if not write:
    sys.exit(0)
# spans of the functions to move, in file order; refill the same slots with the wanted order
bodies = {f[2]: text[f[0]:f[1]] for f in present}
out, pos = [], 0
for slot, f in zip(present, present):
    pass
new = []
last = 0
for f, wname in zip(present, want):
    new.append(text[last:f[0]])
    if "".join(new[-2:]) and not "".join(new[-2:]).endswith("\n"):
        new.append("\n")
    new.append(bodies[wname])
    last = f[1]
new.append(text[last:])
open(src, "w").write("".join(new))
print("written")
