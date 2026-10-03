#!/usr/bin/env python3
"""Target vs built .rodata/.sdata2/.sdata/.data layout, side by side, `|` on the rows that differ.

    uv run --with pyelftools python3 tools/rodatalayout.py UNIT [SECTION]       SECTION defaults to .rodata and .sdata2

Left is the original object, right is ours (build/G8MJ01/src). One row per 4-byte word: offset, the word (with the
float reading when it is one), and the symbol that starts there on each side. A string that starts a row is printed as
text. After a first `|` the two sides usually shift together: look for the extra or missing object at the first
mismatch (a pooled constant that exists on one side only, a template in a different place, a string before the double).
Adapted from zcanann/bfbb's tools/rodatalayout.py (CC0); link_diag.py gives the same mismatch without symbols."""
import glob
import struct
import sys

from elftools.elf.elffile import ELFFile

args = [a for a in sys.argv[1:] if not a.startswith("--")]
unit = args[0]
sections = args[1:] or [".rodata", ".sdata2"]
tp = (glob.glob(f"build/G8MJ01/obj/{unit}.o") + glob.glob(f"build/G8MJ01/*/obj/{unit}.o"))
if not tp:
    sys.exit(f"no target object for {unit}")
bp = f"build/G8MJ01/src/{unit}.o"


def load(path, sec):
    f = ELFFile(open(path, "rb"))
    s = f.get_section_by_name(sec)
    data = s.data() if s is not None and s["sh_type"] != "SHT_NOBITS" else b""
    idx = f.get_section_index(sec) if s is not None else -1
    names = {}
    for sym in f.get_section_by_name(".symtab").iter_symbols():
        if sym["st_shndx"] == idx and sym.name and sym["st_info"]["type"] in ("STT_OBJECT", "STT_NOTYPE"):
            names.setdefault(sym["st_value"], sym.name)
    return data, names


def cell(data, names, off):
    if off >= len(data):
        return ""
    chunk = data[off:off + 4]
    w = chunk.hex()
    txt = ""
    if len(chunk) == 4:
        v = struct.unpack(">f", chunk)[0]
        if chunk[0] in (0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3) or chunk == b"\0\0\0\0":
            txt = f"{v:g}"
    if sum(chr(b).isalnum() or b in (95, 37, 46) for b in chunk) >= 3 and all(32 <= b < 127 or b == 0 for b in chunk):
        txt = repr(chunk.decode("latin1"))
    label = names.get(off, "")
    return f"{w} {txt:<12} {label}"


for sec in sections:
    a, an = load(tp[0], sec)
    b, bn = load(bp, sec)
    print(f"== {sec}: original {len(a)} bytes, built {len(b)} bytes")
    first = None
    for off in range(0, max(len(a), len(b)), 4):
        l, r = cell(a, an, off), cell(b, bn, off)
        same = a[off:off + 4] == b[off:off + 4]
        if not same and first is None:
            first = off
        print(f"{off:5x}  {l:<44.44} {'  ' if same else '| '}{r}")
    print("first mismatch:", "none" if first is None else f"+0x{first:x}")
