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

ext_re = re.compile(r"^extern ((?:const |volatile )+)(f32|f64|u32|GXColor|Vec) (\w+);\n", re.M)
def referenced(name, decl_end, decl_start):
    """The name is used by code, not just declared: a constant the code no longer loads by name (it was turned into a
    literal) must not be defined, or the unit gets a second copy of the value in its pool."""
    rest = text[:decl_start] + text[decl_end:]
    return re.search(rf"\b{re.escape(name)}\b", rest) is not None


found = [(m.start(), m.end(), ("volatile " if "volatile" in m.group(1) else "") + m.group(2), m.group(3)) for m in ext_re.finditer(text)
         if m.group(3) in defs and "const" in m.group(1) and not m.group(3).startswith("double_to_int")
         and referenced(m.group(3), m.end(), m.start())]  # the compiler pools its own conversion constants
if not found:
    sys.exit(f"{unit}: no extern pool constants to define")
order = {name: i for i, (_, _, _, _, _, _, _, _) in enumerate(objs) for name in [objs[i][3]]}
names = sorted({n for _, _, _, n in found}, key=lambda n: order[n])
volatile = {n for _, _, ty, n in found if ty.startswith("volatile ")}  # a match-only view of the object under MUST_MATCH
lines = []
for n in names:
    t, val, bits = defs[n]
    decls = [ty for _, _, ty, nm in found if nm == n]
    plain = [ty for ty in decls if not ty.startswith("volatile ")]
    declared = plain[0] if plain else decls[0].split()[1]

    def define(qual):
        if declared == "Vec":
            return f"const {qual}Vec {n} = {{ {val} }};"
        if declared == "GXColor":
            return (f"const {qual}GXColor {n} = {{ 0x{bits >> 24:02X}, 0x{(bits >> 16) & 255:02X}, "
                    f"0x{(bits >> 8) & 255:02X}, 0x{bits & 255:02X} }};")
        if declared == "u32":
            return f"const {qual}u32 {n} = 0x{bits:08X};"
        return f"const {qual}{declared} {n} = {val};"

    if n in volatile:
        lines.append("#ifdef MUST_MATCH\n" + define("volatile ") + "\n#else\n" + define("") + "\n#endif")
    else:
        lines.append(define(""))
text = text.rstrip("\n") + "\n\n// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).\n" + "\n".join(lines) + "\n"
open(src, "w").write(text)
print(f"{unit}: defined {len(names)} constants")
