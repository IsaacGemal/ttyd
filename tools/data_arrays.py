#!/usr/bin/env python3
"""Define a dol unit's `extern u8/u32/s32 NAME[];` .data tables from its disassembly (all-numeric objects only).

    python3 tools/data_arrays.py UNIT          e.g. effect/n64/eff_star_power_n64   (edits src/UNIT.c in place)

For each `extern u8|u32|s32|u16 NAME[];` (in the source or the unit's own header) whose object in build/G8MJ01/asm/UNIT.s is a
.data object made only of numeric `.4byte`/`.2byte`/`.byte` words (no relocations), the declaration is replaced by a definition
with the declared element type, written right after the includes (definition order = address order for the unit's first
objects; move it if the unit has several). Objects with symbols in them (event scripts, pointers) are skipped: use dol_gen.py."""
import re
import sys

unit = sys.argv[1]
src = f"src/{unit}.c"
asm = open(f"build/G8MJ01/asm/{unit}.s", errors="replace").read()
text = open(src).read()
objs = {}
for m in re.finditer(r"# \.data:0x[0-9A-F]+ \| 0x([0-9A-F]+) \| size: 0x([0-9A-F]+)\n\.obj (\w+), \w+\n(.*?)\.endobj", asm, re.S):
    addr, size, name, body = m.groups()
    ws = re.findall(r"^\s*\.(4byte|2byte|byte) (\S+)$", body, re.M)
    lines = [l for l in body.split("\n") if l.strip() and not l.strip().startswith(".hidden")]
    if len(ws) != len(lines) or not all(re.fullmatch(r"0x[0-9A-Fa-f]+|\d+", v) for _, v in ws):
        continue
    objs[name] = (int(size, 16), [(k, int(v, 0)) for k, v in ws])
decl = re.compile(r"^extern (u8|u16|u32|s32) (\w+)\[\];\n", re.M)
hdr_text = ""
mh = re.search(r'#include "([^"]+)"', text)
if mh:
    try:
        hdr_text = open("include/" + mh.group(1)).read()
    except OSError:
        pass
defs = []
for src_text, in_src in ((text, True),):
    for m in list(decl.finditer(text)) + [x for x in decl.finditer(hdr_text) if re.search(rf"\b{x.group(2)}\b", text)]:
        ty, name = m.groups()
        if name not in objs or any(d[0] == name for d in defs):
            continue
        size, ws = objs[name]
        width = {"u8": 1, "u16": 2, "u32": 4, "s32": 4}[ty]
        raw = b"".join(v.to_bytes({"4byte": 4, "2byte": 2, "byte": 1}[k], "big") for k, v in ws)
        vals = [int.from_bytes(raw[i:i + width], "big") for i in range(0, len(raw), width)]
        per = {1: 12, 2: 10, 4: 7}[width]
        body = ",\n\t".join(", ".join(f"0x{v:0{width * 2}X}" for v in vals[i:i + per]) for i in range(0, len(vals), per))
        defs.append((name, f"{ty} {name}[{len(vals)}] = {{\n\t{body},\n}};\n", m.group(0) if m in list(decl.finditer(text)) else None))
if not defs:
    sys.exit(f"{unit}: nothing to define")
for name, d, ext in defs:
    if ext:
        text = text.replace(ext, "", 1)
idx = [m.end() for m in re.finditer(r"^#include[^\n]*\n", text, re.M)][-1]
text = text[:idx] + "\n" + "\n".join(d for _, d, _ in defs) + text[idx:]
open(src, "w").write(text)
print(f"{unit}: defined {', '.join(n for n, _, _ in defs)}")
