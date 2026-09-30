#!/usr/bin/env python3
"""Define a dol unit's `extern const f32/f64/u32/GXColor NAME;` pool constants, after the code, in the original's order.

    python3 tools/pool_define.py UNIT          e.g. effect/n64/eff_kemuri7_n64  (edits src/UNIT.c in place)

Unlike tools/pool_literals.py (which replaces the names by literals and lets the compiler pool them, and can move the
FP loads of a function that was 100%), the code keeps loading the same named objects, so no function changes; only the
unit's .sdata2/.rodata now hold them. Values come from the unit's disassembly (build/G8MJ01/asm/UNIT.s); the definitions
are written at the end of the file (after the code that loads them: defined earlier, the compiler folds the values and changes
the code), sorted by the original address, so they land in the original order. The extern declarations stay. An extern
that the disassembly does not define in this unit is left alone. Check with tools/agent_check.py UNIT and
tools/pool_compare.py UNIT."""
import re
import struct
import sys

unit = sys.argv[1]
src = f"src/{unit}.c"
asm = open(f"build/G8MJ01/asm/{unit}.s", errors="replace").read()
text = open(src).read()


def fmt_float(bits):
    (f,) = struct.unpack(">f", struct.pack(">I", bits))
    if f != f or f in (float("inf"), float("-inf")):
        return None
    for prec in range(1, 10):
        s = f"{f:.{prec}g}"
        if struct.unpack(">I", struct.pack(">f", float(s)))[0] == bits:
            break
    else:
        s = repr(f)
    if "e" in s:
        s = f"{float(s):.20f}".rstrip("0")
    if "." not in s:
        s += ".0"
    if s.endswith("."):
        s += "0"
    return s + "f"


def fmt_double(bits):
    (f,) = struct.unpack(">d", struct.pack(">Q", bits))
    s = repr(f)
    return s if "e" not in s else f"{f:.30f}".rstrip("0")


objs = []
for m in re.finditer(r"# \.(sdata2|rodata):0x[0-9A-F]+ \| 0x([0-9A-F]+) \| size: 0x([0-9A-F]+)\n\.obj (\w+), \w+\n(.*?)\.endobj", asm, re.S):
    sec, addr, size, name, body = m.groups()
    words = re.findall(r"\.4byte (0x[0-9A-Fa-f]+)", body)
    fl = re.findall(r"\.float (\S+)", body)
    db = re.findall(r"\.double (\S+)", body)
    objs.append((int(addr, 16), sec, int(size, 16), name, words, fl, db, body))
objs.sort()
defs = {}
for addr, sec, size, name, words, fl, db, body in objs:
    if size == 4 and len(words) == 1:
        bits = int(words[0], 16)
        f = fmt_float(bits)
        defs[name] = ("f32", f, bits) if f else ("u32", words[0], bits)
    elif size == 4 and len(fl) == 1:
        bits = struct.unpack(">I", struct.pack(">f", float(fl[0])))[0] if fl[0] not in ("inf", "-inf", "nan") else None
        if bits is not None:
            defs[name] = ("f32", fmt_float(bits), bits)
    elif size == 12 and len(fl) == 3:
        defs[name] = ("Vec", ", ".join((v if "." in v else v + ".0") + "f" for v in fl), None)
    elif size == 12 and len(words) == 3:
        vals = [fmt_float(int(w, 16)) for w in words]
        if all(vals):
            defs[name] = ("Vec", ", ".join(vals), None)
    elif size == 8 and len(words) == 2:
        bits = (int(words[0], 16) << 32) | int(words[1], 16)
        defs[name] = ("f64", fmt_double(bits), bits)
    elif size == 8 and len(db) == 1:
        bits = struct.unpack(">Q", struct.pack(">d", float(db[0])))[0]
        defs[name] = ("f64", fmt_double(bits), bits)

ext_re = re.compile(r"^extern const (volatile )?(f32|f64|u32|GXColor|Vec) (\w+);\n", re.M)
found = [(m.start(), m.end(), (m.group(1) or "") + m.group(2), m.group(3)) for m in ext_re.finditer(text)
         if m.group(3) in defs and not m.group(3).startswith("double_to_int")]  # the compiler pools its own conversion constants
if not found:
    sys.exit(f"{unit}: no extern pool constants to define")
order = {name: i for i, (_, _, _, _, _, _, _, _) in enumerate(objs) for name in [objs[i][3]]}
names = sorted({n for _, _, _, n in found}, key=lambda n: order[n])
lines = []
for n in names:
    t, val, bits = defs[n]
    declared = next(ty for _, _, ty, nm in found if nm == n)
    if declared == "Vec":
        lines.append(f"const Vec {n} = {{ {val} }};")
    elif declared.startswith("volatile "):
        base = declared.split()[1]  # a volatile extern is only a match-view of the object: define it plain
        lines.append(f"const {base} {n} = " + (f"0x{bits:08X};" if base == "u32" else f"{val};"))
    elif declared == "GXColor":
        lines.append(f"const GXColor {n} = {{ 0x{bits >> 24:02X}, 0x{(bits >> 16) & 255:02X}, 0x{(bits >> 8) & 255:02X}, 0x{bits & 255:02X} }};")
    elif declared == "u32":
        lines.append(f"const u32 {n} = 0x{bits:08X};")
    else:
        lines.append(f"const {declared} {n} = {val};")
text = text.rstrip("\n") + "\n\n// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).\n" + "\n".join(lines) + "\n"
open(src, "w").write(text)
print(f"{unit}: defined {len(names)} constants")
