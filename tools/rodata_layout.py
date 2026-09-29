#!/usr/bin/env python3
"""Show a unit's .rodata (or another section) symbol layout: original vs ours.
    python3 tools/rodata_layout.py MOD UNIT [SECTION]"""
import sys, re
from elftools.elf.elffile import ELFFile
mod, unit = sys.argv[1:3]
sec = sys.argv[3] if len(sys.argv) > 3 else ".rodata"
def load(p):
    f = ELFFile(open(p, "rb"))
    s = f.get_section_by_name(sec)
    d = s.data() if s else b""
    idx = f.get_section_index(sec) if s else -1
    syms = sorted((y["st_value"], y["st_size"], y.name) for y in f.get_section_by_name(".symtab").iter_symbols() if y["st_shndx"] == idx)
    return d, syms
import os
ours = f"build/G8MJ01/src/rel/{mod}/{unit}.o"
if not os.path.exists(ours): ours = f"build/G8MJ01/src/{unit}.o"
a, sa = load(f"build/G8MJ01/{mod}/obj/{unit}.o")
b, sb = load(ours)
def show(d, syms):
    for v, sz, n in syms:
        print(f"  {v:#06x} {sz:#05x} {n:32s} {d[v:v+min(sz,16)].hex()}")
print("original", len(a)); show(a, sa)
print("ours", len(b)); show(b, sb)
