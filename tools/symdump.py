#!/usr/bin/env python3
"""Read a unit's data straight from the original object instead of guessing.

    uv run --with pyelftools python3 tools/symdump.py UNIT                       list data symbols, largest first
    uv run --with pyelftools python3 tools/symdump.py UNIT SYMBOL [--as u32|f32|raw|str]   dump one symbol

UNIT is the split's name without .c (battle/battle_seq_command, unit/unit_boss_black_peach), the same name link_diag.py
takes. A word that carries a relocation is shown as the symbol it points to (`-> name+off`), so a pointer table, a
script's PTR()/function references and a string table read directly. Default view is u32 with an f32 reading beside
words that look like floats. Adapted from zcanann/bfbb's tools/symdump.py (CC0)."""
import glob
import struct
import sys

from elftools.elf.elffile import ELFFile

args = [a for a in sys.argv[1:] if not a.startswith("--")]
unit = args[0]
sym = args[1] if len(args) > 1 else None
fmt = sys.argv[sys.argv.index("--as") + 1] if "--as" in sys.argv else "u32"
path = (glob.glob(f"build/G8MJ01/obj/{unit}.o") + glob.glob(f"build/G8MJ01/*/obj/{unit}.o"))
if not path:
    sys.exit(f"no target object for {unit}: build first")
f = ELFFile(open(path[0], "rb"))
st = f.get_section_by_name(".symtab")
DATA = (".rodata", ".data", ".sdata", ".sdata2", ".bss", ".sbss", ".sbss2")
syms = []
for s in st.iter_symbols():
    if not isinstance(s["st_shndx"], int):
        continue
    sec = f.get_section(s["st_shndx"]).name
    if sec in DATA and s["st_info"]["type"] in ("STT_OBJECT", "STT_NOTYPE") and s.name:
        syms.append((s["st_size"], sec, s["st_value"], s.name))
if sym is None:
    for size, sec, val, name in sorted(syms, reverse=True):
        print(f"{size:7d}  {sec:8s} +0x{val:05x}  {name}")
    sys.exit()
match = [x for x in syms if x[3] == sym] or [x for x in syms if x[3].startswith(sym)] or [x for x in syms if sym in x[3]]
if not match:
    sys.exit(f"{sym} is not a data symbol of {unit}")
size, sec, off, name = match[0]
section = f.get_section_by_name(sec)
data = section.data() if section["sh_type"] != "SHT_NOBITS" else bytes(size)
data = data[off:off + size]
relocs = {}
rel = f.get_section_by_name(".rela" + sec)
if rel is not None:
    for r in rel.iter_relocations():
        if off <= r["r_offset"] < off + size:
            t = st.get_symbol(r["r_info_sym"])
            tn = t.name or f.get_section(t["st_shndx"]).name if isinstance(t["st_shndx"], int) else t.name
            relocs[r["r_offset"] - off] = f"{tn}{'+0x%x' % (t['st_value'] + r['r_addend']) if not t.name else ('+0x%x' % r['r_addend'] if r['r_addend'] else '')}"
print(f"{name}: {sec} +0x{off:x}, {size} bytes ({size // 4} words)")
if fmt == "raw":
    for i in range(0, len(data), 16):
        print(f"  {i:04x}  {data[i:i + 16].hex(' ')}")
elif fmt == "str":
    print("  " + repr(data.split(b"\0")[0].decode("shift_jis", "replace")))
else:
    for i in range(0, len(data) - len(data) % 4, 4):
        w = struct.unpack(">I", data[i:i + 4])[0]
        if i in relocs:
            note = f"-> {relocs[i]}"
        elif fmt == "f32" or (0x00800000 <= w < 0x7F800000 or 0x80800000 <= w < 0xFF800000):
            note = f"{struct.unpack('>f', data[i:i + 4])[0]:.9g}"
        else:
            note = ""
        print(f"  {i:04x}  0x{w:08x}  {note}")
