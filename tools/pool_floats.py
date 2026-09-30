#!/usr/bin/env python3
"""Turn a dol unit's named f32/f64 pool constants (extern declarations or definitions) into literals at their uses.

    python3 tools/pool_floats.py UNIT [--keep=NAME,NAME]   e.g. effect/n64/eff_kemuri12_n64  (edits src/UNIT.c in place)

Values come from the unit's disassembly. The compiler then pools them itself, in first-use order, which is the target's
order when the named definitions (placed before pooled literals) would have put a value in the wrong slot. Only f32/f64
names the disassembly defines in this unit are touched; strings, Vec, GXColor and .sbss2 objects are left alone."""
import re
import struct
import sys

unit = sys.argv[1]
src = f"src/{unit}.c"
asm = open(f"build/G8MJ01/asm/{unit}.s", errors="replace").read()
text = open(src).read()


def fmt_float(bits):
    (f,) = struct.unpack(">f", struct.pack(">I", bits))
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
    return s + "f"


vals = {}
for m in re.finditer(r"# \.(?:sdata2|rodata):0x[0-9A-F]+ \| 0x[0-9A-F]+ \| size: 0x([0-9A-F]+)\n\.obj (\w+), \w+\n(.*?)\.endobj", asm, re.S):
    size, name, body = int(m.group(1), 16), m.group(2), m.group(3)
    words = re.findall(r"\.4byte (0x[0-9A-Fa-f]+)", body)
    fl = re.findall(r"\.float (\S+)", body)
    if name.startswith("dat_"):
        continue  # a GXColor/word constant, not a float
    if size == 4 and len(fl) == 1 and fl[0] not in ("inf", "-inf", "nan"):
        words = ["0x%08X" % struct.unpack(">I", struct.pack(">f", float(fl[0])))[0]]
    if size == 4 and len(words) == 1 and not name.startswith("double_to_int"):
        bits = int(words[0], 16)
        (f,) = struct.unpack(">f", struct.pack(">I", bits))
        if f == f and abs(f) != float("inf"):
            vals[name] = fmt_float(bits)
keep = [a.split("=", 1)[1] for a in sys.argv[2:] if a.startswith("--keep=")]  # names that stay named (comma separated)
keep = {k for a in keep for k in a.split(",")}
n = 0
for name, lit in vals.items():
    if name in keep:
        continue
    pat = rf"^(?:extern )?const f32 {name}(?: = [^;]+)?;\n"
    if not re.search(pat, text, re.M) and not re.search(rf"\b{name}\b", text):
        continue  # neither declared nor used here
    text = re.sub(pat, "", text, flags=re.M)
    text = re.sub(rf"\b{name}\b", lit, text)
    n += 1
open(src, "w").write(text)
print(f"{unit}: {n} constants became literals")
