"""Find functions that load the WRONG constant: a blind spot of objdiff's fuzzy score.

    python3 tools/reloc_value_check.py [UNIT-FRAGMENT ...] [--all] [--partial] [--json OUT]

objdiff does not compare relocation targets by value, so a function can score 100% while it loads `40.0f` where the
original loads `60.0f` (eff_fukidashi's 16.0f/1.0f, fixed in #381, was this class). For every function present in both
the original object and ours whose instruction encodings are identical once each relocated field is zeroed, this
resolves both sides' relocation targets to the bytes the instruction actually reads (width from the opcode: lbz 1,
lhz 2, lwz/lfs 4, lfd 8) and reports any pair whose bytes differ. Address-forming relocations (lis/addi/ori, bl) are
compared by the target symbol's own bytes over the shorter of the two sizes, or by name when either side is undefined.

--partial also checks functions that are NOT instruction-identical (the near-misses): it compares the SET of constant
values each side's relocated loads (lfs/lfd/lwz/lhz/lbz...) read, and reports values only one side loads. Order-free,
so scheduling and pool order add no noise. Only small two-sided exchanges are reported (at most 3 values missing AND at
most 3 extra): one-sided gaps are base-register addressing or computed constants. Read each hit against the source. Idea from Claude #4 (bom_01's
disp_sight draft loaded 1.0 where the original loads 0.6, invisible to the fuzzy score).

Linked (Matching) units are skipped unless --all: the checksum already covers them, and running them is a control
(it should report nothing). Idea and method from zcanann/SFA-Decomp tools/sda_reloc_check.py and reloc_width_check.py
(CC0); written for this repo's layout."""
import json
import os
import re
import sys

from elftools.elf.elffile import ELFFile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "G8MJ01")
WIDTH = {34: 1, 35: 1, 38: 1, 39: 1, 40: 2, 41: 2, 42: 2, 43: 2, 44: 2, 45: 2, 32: 4, 33: 4, 36: 4, 37: 4,
         48: 4, 49: 4, 52: 4, 53: 4, 50: 8, 51: 8, 54: 8, 55: 8}
R_PPC_REL24, R_PPC_REL14 = 10, 11


class Obj:
    def __init__(self, path):
        self.elf = ELFFile(open(path, "rb"))
        self.secs = list(self.elf.iter_sections())
        self.data = {i: (s.data() if s.header.sh_type != "SHT_NOBITS" else b"\0" * s.header.sh_size)
                     for i, s in enumerate(self.secs)}
        self.syms = list(self.elf.get_section_by_name(".symtab").iter_symbols())
        self.relocs = {}
        for s in self.secs:
            if s.header.sh_type == "SHT_RELA":
                self.relocs.setdefault(s.header.sh_info, []).extend(
                    (r["r_offset"], r["r_info_type"], r["r_info_sym"], r["r_addend"]) for r in s.iter_relocations())
        self.funcs = {}
        for sym in self.syms:
            if sym["st_info"]["type"] == "STT_FUNC" and isinstance(sym["st_shndx"], int) and sym["st_size"]:
                self.funcs[sym.name] = (sym["st_shndx"], sym["st_value"], sym["st_size"])

    def func(self, name):
        shndx, off, size = self.funcs[name]
        code = self.data[shndx][off:off + size]
        rel = {o - off: (t, si, a) for o, t, si, a in self.relocs.get(shndx, []) if off <= o < off + size}
        return code, rel

    def target(self, symidx, addend, width):
        """(label, bytes or None) for symbol+addend; bytes None when the symbol is undefined here."""
        sym = self.syms[symidx]
        shndx = sym["st_shndx"]
        label = sym.name or (self.secs[shndx].name if isinstance(shndx, int) else "?")
        if not isinstance(shndx, int) or shndx == 0:
            return f"{label}+{addend:#x}", None
        base = (sym["st_value"] if sym["st_info"]["type"] != "STT_SECTION" else 0) + addend
        if width is None:  # address-forming: compare the referenced object's bytes (a function: by name only)
            if sym["st_info"]["type"] == "STT_FUNC":
                return f"{label}+{addend:#x}", None
            size = sym["st_size"] or 4
            return f"{label}+{addend:#x}", self.data[shndx][base:base + size]
        return f"{label}+{addend:#x}", self.data[shndx][base:base + width]


def normalise(code, rel):
    out = bytearray(code)
    for off, (typ, _, _) in rel.items():
        w = off - off % 4
        word = int.from_bytes(out[w:w + 4], "big")
        if typ == R_PPC_REL24:
            word &= 0xFC000003
        elif typ == R_PPC_REL14:
            word &= 0xFFFF0003
        else:
            word &= 0xFFE00000 if typ == 109 else 0xFFFF0000  # SDA21 also carries the base register
        out[w:w + 4] = word.to_bytes(4, "big")
    return bytes(out)


def loaded_values(obj, code, rel):
    """{(width, bytes): label} for every relocated load (lfs/lfd/lwz/lhz/lbz...) in one function."""
    out = {}
    for off, (typ, sym, add) in rel.items():
        if typ in (R_PPC_REL24, R_PPC_REL14):
            continue
        w = off - off % 4
        width = WIDTH.get(int.from_bytes(code[w:w + 4], "big") >> 26)
        if width is None:
            continue
        label, data = obj.target(sym, add, width)
        if data is not None:
            out.setdefault((width, data), label)
    return out


def structural(key):
    """A loaded value that is not a source literal: int->float conversion doubles (0x4330...), colour/mask words (a
    4-byte value whose float exponent is all zeros or all ones, i.e. not a normal float)."""
    width, data = key
    if width == 8:
        return data[:4] in (b"\x43\x30\x00\x00", b"\x43\x30\x00\x00")
    if width == 4:
        exp = (int.from_bytes(data, "big") >> 23) & 0xFF
        return data != b"\0\0\0\0" and exp in (0, 0xFF)
    return True


def check_partial(t, o, name, tc, tr, oc, orr):
    """Value hits for one non-identical function: constants one side loads and the other never does. Order-free, so
    scheduling and pool-order differences in a near-miss add no noise; a wrong literal shows up as one value on each side."""
    tv, ov = loaded_values(t, tc, tr), loaded_values(o, oc, orr)
    only_t = {k for k in set(tv) - set(ov) if not structural(k)}
    only_o = {k for k in set(ov) - set(tv) if not structural(k)}
    # one-sided differences are how base-register addressing (lis base; lfs off(base)) and computed constants look;
    # a wrong literal is a small exchange: values missing on one side AND extra on the other
    if not only_t or not only_o or len(only_t) > 3 or len(only_o) > 3:
        return []
    hits = []
    for key in sorted(only_t):
        hits.append(dict(fn=name, off=0, kind=f"partial{key[0]}", target=tv[key], ours="(never loaded)",
                         tbytes=key[1].hex(), obytes=""))
    for key in sorted(only_o):
        hits.append(dict(fn=name, off=0, kind=f"partial{key[0]}", target="(never loaded)", ours=ov[key],
                         tbytes="", obytes=key[1].hex()))
    return hits


def check_unit(tpath, opath, partial=False):
    t, o = Obj(tpath), Obj(opath)
    findings = []
    for name in sorted(set(t.funcs) & set(o.funcs)):
        tc, tr = t.func(name)
        oc, orr = o.func(name)
        if len(tc) != len(oc) or set(tr) != set(orr) or normalise(tc, tr) != normalise(oc, orr):
            if partial:
                findings.extend(check_partial(t, o, name, tc, tr, oc, orr))
            continue
        for off in sorted(tr):
            w = off - off % 4
            op = int.from_bytes(tc[w:w + 4], "big") >> 26
            ttyp, tsym, tadd = tr[off]
            otyp, osym, oadd = orr[off]
            if ttyp in (R_PPC_REL24, R_PPC_REL14):
                tl, _ = t.target(tsym, tadd, 4)
                ol, _ = o.target(osym, oadd, 4)
                if tl.split("+")[0] != ol.split("+")[0] and not tl.startswith(("@", "lbl_", "fn_", ".")) \
                        and not ol.startswith(("@", "lbl_", "fn_", ".")):
                    findings.append(dict(fn=name, off=off, kind="call", target=tl, ours=ol))
                continue
            width = WIDTH.get(op)
            tl, tb = t.target(tsym, tadd, width)
            ol, ob = o.target(osym, oadd, width)
            if tb is None or ob is None:
                tn, on = tl.split("+")[0], ol.split("+")[0]
                if tb is None and ob is None and tn != on and not (tn.startswith(("lbl_", "fn_")) or on.startswith(("lbl_", "fn_"))):
                    findings.append(dict(fn=name, off=off, kind="name", target=tl, ours=ol))
                continue
            n = min(len(tb), len(ob))
            if tb[:n] != ob[:n]:
                findings.append(dict(fn=name, off=off, kind=f"value{width or 'addr'}", target=tl, ours=ol,
                                     tbytes=tb[:16].hex(), obytes=ob[:16].hex()))
    return findings


def units(report, frags, everything):
    matching = set(re.findall(r'Object\(Matching,\s*"([^"]+)"', open(os.path.join(ROOT, "configure.py")).read()))
    for u in json.load(open(report))["units"]:
        mod, _, path = u["name"].partition("/")
        if frags and not any(f in u["name"] for f in frags):
            continue
        linked = any(m.rsplit(".", 1)[0] == path for m in matching)
        if linked and not everything:
            continue
        tpath = os.path.join(BUILD, "obj", path + ".o") if mod == "MarioSt" else os.path.join(BUILD, mod, "obj", path + ".o")
        opath = os.path.join(BUILD, "src", path + ".o")
        if os.path.exists(tpath) and os.path.exists(opath):
            yield u["name"], tpath, opath, linked


def main(argv):
    everything = "--all" in argv
    partial = "--partial" in argv
    out = argv[argv.index("--json") + 1] if "--json" in argv else None
    frags = [a for a in argv if not a.startswith("--") and a != out]
    results = {}
    for name, tpath, opath, linked in units(os.path.join(BUILD, "report.json"), frags, everything):
        try:
            f = check_unit(tpath, opath, partial)
        except Exception as e:  # a malformed or missing object is a note, not a finding
            print(f"skip {name}: {e}", file=sys.stderr)
            continue
        if f:
            results[name] = f
            print(f"{name}{' (linked)' if linked else ''}")
            for x in f:
                extra = f"  {x['tbytes']} vs {x['obytes']}" if "tbytes" in x else ""
                print(f"  {x['fn']} +{x['off']:#x} {x['kind']}: original {x['target']} ours {x['ours']}{extra}")
    if out:
        json.dump(results, open(out, "w"), indent=1)
    print(f"{sum(len(v) for v in results.values())} finding(s) in {len(results)} unit(s)")
    return 1 if results else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
