#!/usr/bin/env python3
"""Are two builds of a unit's object interchangeable at link time, whatever their symbol names?

    uv run --with pyelftools python3 tools/obj_equiv.py OLD.o NEW.o [-v]
    uv run --with pyelftools python3 tools/obj_equiv.py --tree OLD_DIR NEW_DIR [-v]   every .o under both

Compares every allocated section's size, alignment and bytes, and every relocation with its target resolved to a
place: a symbol the object defines becomes (section, offset), so `float_0_8041fcac` and the compiler's `@123` pool entry
at the same offset are the same target; an undefined symbol stays a name. Symbol names are otherwise ignored, so a
renamed static, a renamed object or a named constant turned into a literal that pools to the same slot all pass.
A global both objects define must sit at the same place with the same size (other units reach it by name).

Also listed (not a failure on their own): global symbols the new object no longer defines or defines anew. A removed
global that another unit references breaks the link, and a renamed function or object no longer lines up with
config/G8MJ01/symbols.txt, so check those before keeping the change. The linked checksum stays the final word.
Exit status 0 when every compared object is equivalent.
"""
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

SKIP = (".comment", ".mwcats", ".debug", ".line")


def load(path):
    elf = ELFFile(open(path, "rb"))
    secs = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    syms = list(symtab.iter_symbols()) if symtab else []
    names, count = {}, {}
    for i, s in enumerate(secs):
        if s.header.sh_flags & 2 and not s.name.startswith(SKIP):  # SHF_ALLOC
            n = count[s.name] = count.get(s.name, 0) + 1
            names[i] = s.name + (f"#{n}" if n > 1 else "")

    def target(sym, addend):
        idx = sym["st_shndx"]
        if isinstance(idx, int) and idx in names:
            return (names[idx], sym["st_value"] + addend)
        if isinstance(idx, int) and idx != 0:
            return ("?" + secs[idx].name, sym["st_value"] + addend)
        return (sym.name, addend)

    out = {}
    for i, key in names.items():
        s = secs[i]
        data = b"" if s.header.sh_type == "SHT_NOBITS" else s.data()
        out[key] = {"size": s.header.sh_size, "align": s.header.sh_addralign, "data": data, "relocs": []}
    for s in secs:
        if s.header.sh_type in ("SHT_RELA", "SHT_REL") and s.header.sh_info in names:
            rel = out[names[s.header.sh_info]]["relocs"]
            for r in s.iter_relocations():
                sym = syms[r["r_info_sym"]]
                rel.append((r["r_offset"], r["r_info_type"], target(sym, r["r_addend"] if r.is_RELA() else 0)))
    glob = {s.name: (target(s, 0), s["st_size"]) for s in syms if s["st_info"]["bind"] in ("STB_GLOBAL", "STB_WEAK")
            and isinstance(s["st_shndx"], int) and s["st_shndx"] != 0}
    return out, glob


def compare(a_path, b_path):
    """([differences], removed globals, added globals)."""
    a, ga = load(a_path)
    b, gb = load(b_path)
    diffs = []
    for key in sorted(a.keys() | b.keys()):
        if key not in a or key not in b:
            diffs.append(f"section {key} only in {'old' if key in a else 'new'}")
            continue
        x, y = a[key], b[key]
        if x["size"] != y["size"] or x["align"] != y["align"]:
            diffs.append(f"{key}: size/align {x['size']:#x}/{x['align']} -> {y['size']:#x}/{y['align']}")
            continue
        if x["data"] != y["data"]:
            off = next(i for i in range(len(x["data"])) if x["data"][i] != y["data"][i])
            diffs.append(f"{key}+{off:#x}: bytes differ")
        rx, ry = sorted(x["relocs"]), sorted(y["relocs"])
        if rx != ry:
            first = next((p for p in zip(rx, ry) if p[0] != p[1]), None)
            diffs.append(f"{key}: relocations differ ({len(rx)} vs {len(ry)}){': %s vs %s' % first if first else ''}")
    for name in sorted(ga.keys() & gb.keys()):
        if ga[name] != gb[name]:
            (sa, oa), za = ga[name]
            (sb, ob), zb = gb[name]
            diffs.append(f"global {name}: {sa}+{oa:#x} size {za:#x} -> {sb}+{ob:#x} size {zb:#x}")
    return diffs, sorted(ga.keys() - gb.keys()), sorted(gb.keys() - ga.keys())


def main():
    args = [a for a in sys.argv[1:] if a != "-v"]
    verbose = "-v" in sys.argv
    if args[:1] == ["--tree"]:
        old, new = Path(args[1]), Path(args[2])
        pairs = [(p, new / p.relative_to(old)) for p in sorted(old.rglob("*.o"))]
    else:
        pairs = [(Path(args[0]), Path(args[1]))]
    bad = 0
    for a, b in pairs:
        if not b.exists():
            print(f"MISSING {b}")
            bad += 1
            continue
        if a.read_bytes() == b.read_bytes():
            continue
        diffs, removed, added = compare(a, b)
        bad += bool(diffs)
        if diffs or verbose or removed or added:
            print(f"{'DIFF' if diffs else 'same'} {b}")
            for d in diffs[:10]:
                print(f"    {d}")
            if removed:
                print(f"    globals removed: {' '.join(removed)}")
            if added:
                print(f"    globals added:   {' '.join(added)}")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    main()
