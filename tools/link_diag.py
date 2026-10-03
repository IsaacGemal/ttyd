#!/usr/bin/env python3
"""Why a unit flipped to Matching fails build.sha1.

    uv run --with pyelftools python3 tools/link_diag.py UNIT      e.g. effect/eff_hit_n64

Run after flipping the unit in configure.py and building (a failed link is fine; compile errors are not). For every
data section it compares the built object against the original's word by word and prints the differing words, so a
pool-order swap, an extra pooled constant (the built section is longer) or a missing object shows up directly. If
build/G8MJ01/MarioSt.dol exists it also lists where it differs from the original dol, which pins down code that
loads a different .sdata2/.rodata address.
"""
import struct
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parent.parent
SECTIONS = [".text", ".rodata", ".data", ".sdata", ".sdata2", ".sbss", ".sbss2", ".bss"]


def section(path, name):
    sec = ELFFile(open(path, "rb")).get_section_by_name(name)
    return sec.data() if sec is not None and sec["sh_type"] != "SHT_NOBITS" else b""


def words(data):
    return [data[i:i + 4].hex() for i in range(0, len(data), 4)]


def main():
    unit = sys.argv[1].removesuffix(".c")
    target = ROOT / f"build/G8MJ01/obj/{unit}.o"
    built = ROOT / f"build/G8MJ01/src/{unit}.o"
    for name in SECTIONS[1:5]:
        a, b = words(section(target, name)), words(section(built, name))
        if a == b:
            print(f"{name}: identical ({len(a) * 4} bytes)")
            continue
        print(f"{name}: original {len(a) * 4} bytes, built {len(b) * 4} bytes")
        shown = 0
        for i in range(max(len(a), len(b))):
            x = a[i] if i < len(a) else "--------"
            y = b[i] if i < len(b) else "--------"
            if x != y and shown < 24:
                print(f"  +0x{i * 4:x}  original {x}  built {y}")
                shown += 1
    dol, orig = ROOT / "build/G8MJ01/MarioSt.dol", ROOT / "orig/G8MJ01/sys/main.dol"
    if dol.exists() and orig.exists():
        a, b = dol.read_bytes(), orig.read_bytes()
        off = struct.unpack(">18I", a[0:72])
        addr = struct.unpack(">18I", a[0x48:0x90])
        size = struct.unpack(">18I", a[0x90:0xD8])
        total = 0
        for i in range(18):
            if not size[i]:
                continue
            diff = [j for j in range(size[i]) if a[off[i] + j] != b[off[i] + j]]
            total += len(diff)
            if diff:
                print(f"dol section {i}: {len(diff)} bytes differ, first at 0x{addr[i] + diff[0]:08x}")
        print(f"dol: {total} bytes differ")


if __name__ == "__main__":
    main()
