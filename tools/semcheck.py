#!/usr/bin/env python3
"""Find likely WRONG CODE in a unit's partly matched functions: the differences from the original that register
allocation can't explain. objdiff's score can't tell a wrong constant from a register swap; these checks can.

  python3 tools/semcheck.py UNIT        e.g. effect/eff_fire, MSL_C.PPCEABI.bare.H.a/printf, or mri/<unit> for a REL
  python3 tools/semcheck.py UNIT --object PATH    check a trial object (e.g. a copy compiled elsewhere) instead

For every function below 100% and every data symbol the unit defines, after a build:
  CALL    a call the original makes that yours doesn't (or the reverse); an argument that differs: an integer
          constant, a constant where yours passes a variable (or the reverse), or which arguments point at the same
          stack buffer (e.g. PSMTXConcat(a, b, b) vs (a, b, a))
  SYMBOL  the same instruction referring to a different symbol whose value differs: a wrong string or global
  FLOAT   the float constants a function loads, by value: a wrong float literal
  VALUE   the same instruction with a different constant: a wrong literal, mask, offset or argument, including two
          arguments swapped
  MEM     loads/stores by (instruction, offset), ignoring which register holds the address: a missing, extra or
          wrongly offset field access
  OPS     compare and branch counts that differ (e.g. original has one more `bge`, yours one more `blt`): an inverted
          or changed test, or a missing step
  DATA    a data symbol whose bytes differ from the original's (tables, colours; strings up to their terminator)
A function below 20%, or with under half the original's instructions, gets one line: it isn't written yet.
Line numbers are the compiler's: inside a loop it records the loop's last line for the whole body, so read "near".
Every flag is a lead, not a verdict: read the asm at that spot (build/G8MJ01/asm/<unit>.s) before changing
anything, and leave code the asm confirms. Exit 0 after checking; 1 if the unit can't be checked.

The VALUE, OPS and MEM rules are SFA-Decomp's (CC0 1.0; tools/wrongvalue_scan.py, tools/a71_mnhist_scan.py and the
memory-access sweep in docs/aug05-A103-wrong-computation-sweep.md), run here over objdiff's per-function diff so
each flag can name a source line; tools/sfa/ has their project-wide versions. CALL follows a script from a wave 13
review; SYMBOL uses objdiff's data_value relocation comparison.
"""
import struct
import difflib
import json
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
OBJDIFF = ROOT / "build/tools/objdiff-cli"
REG = re.compile(r"\b[rf]\d+\b")
NUM = re.compile(r"(?:^|[ ,(])(-?(?:0x[0-9a-fA-F]+|\d+))\b")
MEM = re.compile(r"^(l[bhwf]\w*|st[bhwf]\w*|psq_\w+)\s+[rf]\d+,\s*(-?(?:0x[0-9a-f]+|\d+))\((r\d+)\)")
ARGS = [f"r{n}" for n in range(3, 11)] + [f"f{n}" for n in range(1, 9)]
LOCAL_BASES = ("r1", "r2", "r13", "r0")  # stack frame and small-data bases: frame layout, not field accesses
MEM_DETAIL = 8  # more differing offsets than this is an addressing difference, summarised in one line
ARITH = re.compile(r"^(f(?:add|sub|mul|div|madd|msub|nmadd|nmsub)s?|fneg|fabs|subf\w*|mullw|mulhw\w*|divw\w*|neg|"
                   r"and|andc|or|orc|xor|nand|nor|slw|srw|sraw\w*|rlwinm|mulli|subfic)$")
TEST_OPS = re.compile(r"^(b(?!l\b|lr\b|ctr\b|ctrl\b)\w*|cmp\w*|cror|crnor|crand|creqv|crxor|crorc|crandc|crnand)$")


class Elf:
    """Just enough of a big-endian ELF32 relocatable object: symbols, their bytes, and the relocations inside them."""
    def __init__(self, path):
        d = self.d = Path(path).read_bytes()
        shoff, = struct.unpack_from(">I", d, 0x20)
        shentsize, shnum, shstrndx = struct.unpack_from(">HHH", d, 0x2E)
        self.sh = [struct.unpack_from(">IIIIIIIIII", d, shoff + i * shentsize) for i in range(shnum)]
        names = self.sh[shstrndx]
        self.secname = [self._str(names[4], h[0]) for h in self.sh]
        self.syms, self.symlist = {}, []
        for i, h in enumerate(self.sh):
            if h[1] == 2:  # SHT_SYMTAB
                strtab = self.sh[h[6]][4]
                for k in range(h[5] // 16):
                    nm, val, size, info, other, shndx = struct.unpack_from(">IIIBBH", d, h[4] + k * 16)
                    name = self._str(strtab, nm)
                    self.symlist.append((name, shndx, val))
                    if name and 0 < shndx < len(self.sh):
                        self.syms[name] = (shndx, val, size)
        self.relocs = {}  # section index -> {offset: (symbol, addend)}
        for h in self.sh:
            if h[1] == 4:  # SHT_RELA
                target = self.relocs.setdefault(h[7], {})
                for k in range(h[5] // 12):
                    off, info, add = struct.unpack_from(">IIi", d, h[4] + k * 12)
                    name, shndx, val = self.symlist[info >> 8]
                    if not name and shndx < len(self.secname):  # a section-relative reference
                        name = f"{self.secname[shndx]}+{val + add:#x}"
                        add = 0
                    target[off] = (name, add)

    def _str(self, off, i):
        return self.d[off + i:self.d.index(b"\0", off + i)].decode(errors="replace")

    def data(self, name):
        shndx, val, size = self.syms[name]
        h = self.sh[shndx]
        return b"\0" * size if h[1] == 8 else self.d[h[4] + val:h[4] + val + size]  # SHT_NOBITS: .bss

    def pointers(self, name):
        shndx, val, size = self.syms[name]
        return {o - val: r for o, r in self.relocs.get(shndx, {}).items() if val <= o < val + size}


def unit_objects(unit):
    units = json.load(open(ROOT / "objdiff.json"))["units"]
    exact = [u for u in units if u["name"] == unit or u["name"] == "MarioSt/" + unit]
    found = exact or [u for u in units if u["name"].split("/", 1)[1] == unit]
    if not found:
        sys.exit(f"semcheck.py: no unit {unit} in objdiff.json")
    if len(found) > 1:
        sys.exit(f"semcheck.py: {unit} is in several modules; name one: " + ", ".join(u["name"] for u in found))
    u = found[0]
    return u["name"], u.get("target_path"), u.get("base_path")


def rows(symbol):
    """objdiff's aligned rows: (text, line, diff_kind); text is None where this side has no instruction."""
    out = []
    for x in symbol.get("instructions", []):
        i = x.get("instruction") or {}
        out.append((i.get("formatted"), x.get("line_number") or i.get("line_number"), x.get("diff_kind")))
    return out


def near(line):
    return f" (near line {line})" if line else ""


def is_branch(text):
    return re.match(r"b\w*\s", text) is not None


def calls(ins):
    """Each call with what its argument registers hold: ('const', n), ('stack', offset), ('sym', name) or 'var'."""
    out, regs = [], {}
    for text, ln in ins:
        op = text.split()[0]
        dest = re.match(r"\w+\.?\s+([rf]\d+),", text)
        m = re.match(r"li (r\d+), (-?0x[0-9a-f]+|-?\d+)$", text)
        if m:
            regs[m.group(1)] = ("const", int(m.group(2), 0))
        elif (m := re.match(r"addi (r\d+), r1, (-?0x[0-9a-f]+|-?\d+)$", text)):
            regs[m.group(1)] = ("stack", int(m.group(2), 0))
        elif (m := re.match(r"(?:lfs|lfd|lwz) ([rf]\d+), ([A-Za-z_@][\w@.]*)@", text)):
            regs[m.group(1)] = ("sym", m.group(2))
        elif dest and not op.startswith(("st", "cmp", "b", "psq_st")):
            regs[dest.group(1)] = "var"
        if (m := re.match(r"bl (\S+)", text)) or text.startswith("bctrl"):
            out.append((m.group(1) if m else "(indirect call)", dict(regs), ln))
            regs = {}
        elif is_branch(text):
            regs = {}
    return out


def stack_groups(args):
    by = {}
    for r, v in args.items():
        if isinstance(v, tuple) and v[0] == "stack":
            by.setdefault(v[1], []).append(r)
    return sorted(tuple(sorted(g)) for g in by.values() if len(g) > 1)


def check_calls(a, b, fa=None, fb=None):
    ca, cb = calls(a), calls(b)
    fa, fb = fa or {}, fb or {}
    flags = []
    sm = difflib.SequenceMatcher(None, [c[0] for c in ca], [c[0] for c in cb], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for x, y in zip(ca[i1:i2], cb[j1:j2]):
                d = []
                for r in ARGS:
                    va, vb = x[1].get(r), y[1].get(r)
                    if va is None or vb is None:
                        continue
                    if va[0:1] == ("sym",) and vb[0:1] == ("sym",) and va[1] in fa and vb[1] in fb \
                            and round(fa[va[1]][1], 6) != round(fb[vb[1]][1], 6):
                        d.append(f"{r}: original {fa[va[1]][1]:g}, yours {fb[vb[1]][1]:g}")
                    elif va[0:1] == ("const",) and vb[0:1] == ("const",) and va != vb:
                        d.append(f"{r}: original {va[1]:#x}, yours {vb[1]:#x}")
                    elif (va[0:1] == ("const",)) != (vb[0:1] == ("const",)) and "stack" not in (va[0], vb[0]):
                        d.append(f"{r}: original passes {'%#x' % va[1] if va[0] == 'const' else 'a variable'}, "
                                 f"yours {'%#x' % vb[1] if vb[0] == 'const' else 'a variable'}")
                ga, gb = stack_groups(x[1]), stack_groups(y[1])
                if ga != gb:
                    fmt = lambda g: "; ".join("=".join(t) for t in g) or "none"
                    d.append(f"arguments pointing at the same stack buffer: original {fmt(ga)}, yours {fmt(gb)}")
                if d:
                    flags.append(f"CALL   {x[0]}{near(y[2])}: {'; '.join(d)}")
        else:
            miss, extra = [c[0] for c in ca[i1:i2]], [c[0] for c in cb[j1:j2]]
            if Counter(miss) == Counter(extra):
                continue
            ln = next((c[2] for c in cb[max(j1 - 1, 0):j2 + 1] if c[2]), None)
            flags.append(f"CALL   original calls {', '.join(miss) or 'nothing'} where yours calls "
                         f"{', '.join(extra) or 'nothing'}{near(ln)}")
    return flags


SYM = re.compile(r"([A-Za-z_@][\w@.$]*)@(?:sda21|l|ha)")


def check_symbols(r_left, r_right, dl, dr):
    """objdiff (data_value) marks an argument mismatch where the referenced symbols' values differ. Report rows that
    are otherwise the same instruction, registers included (a row objdiff paired across a shifted block is not)."""
    flags = []
    for (ta, _, _), (tb, ln, kind) in zip(r_left, r_right):
        if not ta or not tb or kind != "DIFF_ARG_MISMATCH" or ta.startswith(("lfs ", "lfd ", "bl ")):
            continue  # float loads: check_floats; calls: check_calls
        na, nb = SYM.search(ta), SYM.search(tb)
        if not na or not nb or SYM.sub("S", ta) != SYM.sub("S", tb) or na.group(1) == nb.group(1):
            continue  # a shifted row, or the same symbol (its bytes are the DATA check's)
        xa, xb = dl.get(na.group(1)), dr.get(nb.group(1))
        if xa is not None and xb is not None and value(na.group(1), xa) == value(nb.group(1), xb):
            continue
        flags.append(f"SYMBOL original `{ta}`, yours `{tb}`: refers to data with a different value{near(ln)}")
    return flags


def float_values(elf):
    """name -> value of the object's 4- and 8-byte data symbols (float constants, pooled @NNN ones included)."""
    out = {}
    for name, (shndx, val, size) in elf.syms.items():
        if size in (4, 8) and not elf.secname[shndx].startswith(".text"):
            b = elf.data(name)
            out[name] = ("f", struct.unpack(">f", b)[0]) if size == 4 else ("d", struct.unpack(">d", b)[0])
    return out


def check_floats(a, b, va, vb):
    """The float constants each side loads, by value and ignoring order: a wrong literal changes the multiset."""
    def loads(ins, vals):
        c, where = Counter(), {}
        for t, ln in ins:
            m = re.match(r"(lfs|lfd) f\d+, ([A-Za-z_@][\w@.$]*)@", t)
            if m and m.group(2) in vals:
                k = round(vals[m.group(2)][1], 6)
                c[k] += 1
                where.setdefault(k, set()).add(ln)
        return c, where
    ca, _ = loads(a, va)
    cb, wb = loads(b, vb)
    # a value only one side loads is a different literal, and so is a substitution (one value loaded less, another
    # more); the same values loaded a different number of times on one side only is the compiler reusing a register
    more, fewer = ca - cb, cb - ca
    if not (set(more) - set(cb) or set(fewer) - set(ca) or (more and fewer)):
        return []
    if not more and not fewer:
        return []
    fmt = lambda c, w=None: ", ".join(f"{k:g} x{v}" + (f" (near line {min(l for l in w[k] if l)})" if w and any(w[k]) else "")
                                      for k, v in sorted(c.items()))
    return [f"FLOAT  original loads more: {fmt(more) or '-'}; yours loads more: {fmt(fewer, wb) or '-'}"]


def shape(text):
    return NUM.sub(lambda m: m.group(0)[:-len(m.group(1))] + "N", REG.sub("R", text))


def check_values(a, b, reported):
    """SFA's wrongvalue rule: same masked instruction, different constant. A block whose instructions are the same
    on both sides (registers included) in a different order is a reorder and is skipped; the same constants moved
    between registers is an argument swap and is kept."""
    flags = []
    sm = difflib.SequenceMatcher(None, [shape(t) for t, _ in a], [shape(t) for t, _ in b], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != "equal":
            continue
        pairs = [(x, y) for x, y in zip(a[i1:i2], b[j1:j2])
                 if REG.sub("R", x[0]) != REG.sub("R", y[0]) and not is_branch(x[0])
                 and not re.search(r"\((?:r1|r2|r13)\)|\br1\b", x[0])]
        if not pairs:
            continue
        if Counter(x[0] for x, _ in pairs) == Counter(y[0] for _, y in pairs):
            fifo = [x for x, _ in pairs if MEM.match(x[0]) and x[0].startswith("st")]
            if len({REG.sub("R", x[0].split(",", 1)[1]) for x in fifo}) < len(fifo):
                flags.append(f"VALUE  stores to the same address in a different order than the original (a GX vertex "
                             f"written out of order?){near(pairs[0][1][1])}")
            continue
        ca_ = Counter(REG.sub("R", x[0]) for x, _ in pairs if x[0].startswith("cmp"))
        cb_ = Counter(REG.sub("R", y[0]) for _, y in pairs if y[0].startswith("cmp"))
        if ca_ and ca_ == cb_:  # the same compares in another order (an || chain); a changed test shows in OPS
            pairs = [(x, y) for x, y in pairs if not x[0].startswith("cmp")]
        deltas = {int(NUM.findall(REG.sub("R", y[0]))[-1], 0) - int(NUM.findall(REG.sub("R", x[0]))[-1], 0)
                  for x, y in pairs if MEM.match(x[0]) and MEM.match(y[0])}
        walking = len(pairs) > 2 and len(deltas) == 1 and all(MEM.match(x[0]) for x, _ in pairs)
        for x, y in pairs:
            nx, ny = NUM.findall(REG.sub("R", x[0])), NUM.findall(REG.sub("R", y[0]))
            if set(nx) == set(ny) and REG.findall(x[0]) == REG.findall(y[0]):
                continue
            if walking or (x[0].startswith("li ") and (int(nx[-1], 0), int(ny[-1], 0)) in reported):
                continue  # one constant shift over a run: pointer walking; or a call argument CALL already reported
            flags.append(f"VALUE  original `{REG.sub('R', x[0])}`, yours `{REG.sub('R', y[0])}`{near(y[1])}")
    return flags


def stored_constants(ins):
    """For each store, the constant it writes when its source register was last set by `li` (constants hoisted out
    of a loop included); None otherwise."""
    regs, out = {}, []
    for text, _ in ins:
        m = re.match(r"li (r\d+), (-?0x[0-9a-f]+|-?\d+)$", text)
        st = re.match(r"(st[bhw]\w*) (r\d+), ", text)
        out.append(regs.get(st.group(2)) if st else None)
        if m:
            regs[m.group(1)] = int(m.group(2), 0)
        elif (dest := re.match(r"\w+\.?\s+(r\d+),", text)) and not text.startswith(("st", "cmp", "b")):
            regs.pop(dest.group(1), None)
    return out


def check_stores(a, b):
    """Stores of a known constant where the original stores a different one (e.g. a vertex written as 12, not -12):
    register masking hides these, since only the source register differs."""
    ca, cb = stored_constants(a), stored_constants(b)
    flags = []
    sm = difflib.SequenceMatcher(None, [shape(t) for t, _ in a], [shape(t) for t, _ in b], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for i, j in zip(range(i1, i2), range(j1, j2)):
                if ca[i] is not None and cb[j] is not None and ca[i] != cb[j]:
                    flags.append(f"VALUE  `{REG.sub('R', a[i][0])}` stores {ca[i]:#x} in the original, {cb[j]:#x} in "
                                 f"yours{near(b[j][1])}")
    return flags


def check_mem(a, b):
    """SFA's A103 pass 2: (mnemonic, displacement) multisets with the base register masked, frame and small-data
    traffic excluded."""
    def key(ins):
        c, where = Counter(), {}
        for t, ln in ins:
            m = MEM.match(t)
            if m and m.group(3) not in LOCAL_BASES:
                k = (m.group(1), m.group(2))
                c[k] += 1
                where.setdefault(k, set()).add(ln)
        return c, where
    ka, _ = key(a)
    kb, wb = key(b)
    diff = [k for k in sorted(set(ka) | set(kb)) if ka[k] != kb[k]]
    if len(diff) > MEM_DETAIL:
        return [f"MEM    {len(diff)} load/store offsets differ: the fields are probably reached from a different base"
                " (one struct pointer vs another, or a different address split); compare the asm's addressing"]
    flags = []
    for k in diff:
        ls = sorted(l for l in wb.get(k, ()) if l)
        where = f" (yours at line{'s' * (len(ls) > 1)} {', '.join(map(str, ls[:4]))})" if ls and kb[k] else ""
        flags.append(f"MEM    `{k[0]} ..., {k[1]}(base)`: original {ka[k]}, yours {kb[k]}{where}")
    return flags


def check_ops(a, b):
    """SFA's a71 mnemonic histogram, kept to compares and branches (a different count there is a different test in
    the source), plus arithmetic substitutions: one operation fewer and another more (fsubs where the original has
    fadds). Moves, loads and plain adds shift with allocation and aren't counted."""
    flags = []
    ha = Counter(t.split()[0] for t, _ in a if TEST_OPS.match(t.split()[0]))
    hb = Counter(t.split()[0] for t, _ in b if TEST_OPS.match(t.split()[0]))
    more, fewer = ha - hb, hb - ha
    fmt = lambda c: ", ".join(f"{k} x{v}" for k, v in sorted(c.items()))
    if more or fewer:
        flags.append(f"OPS    original has more: {fmt(more) or '-'}; yours has more: {fmt(fewer) or '-'}")
    ma = Counter(t.split()[0].rstrip(".") for t, _ in a if ARITH.match(t.split()[0].rstrip(".")))
    mb = Counter(t.split()[0].rstrip(".") for t, _ in b if ARITH.match(t.split()[0].rstrip(".")))
    more, fewer = ma - mb, mb - ma
    if more and fewer:
        flags.append(f"OPS    arithmetic: original has more {fmt(more)}; yours has more {fmt(fewer)}")
    return flags


def value(name, b):
    """A data symbol's meaningful bytes: a string up to its terminator, anything else whole."""
    if name.startswith(("str_", "@stringBase")) or (b"\0" in b and all(32 <= c < 127 for c in b.split(b"\0")[0])):
        return b.split(b"\0")[0]
    return b


def show(b):
    if len(b) == 4:
        return f"{struct.unpack('>f', b)[0]!r}"
    if len(b) == 8:
        return f"{struct.unpack('>d', b)[0]!r}"
    return b[:8].hex(" ") or "(end)"


def check_data(ea, eb):
    """Data symbols whose bytes or pointers differ; a string is compared up to its terminator (the rest is padding).
    Pointers are compared by target name (a table entry pointing at col_poison instead of col_normal)."""
    flags = []
    for name, (shndx, val, size) in sorted(ea.syms.items()):
        if name not in eb.syms or ea.secname[shndx].startswith(".text") or size == 0 or name.startswith("@"):
            continue
        if eb.syms[name][2] == 0 or eb.secname[eb.syms[name][0]].startswith(".text"):
            continue
        x, y = value(name, ea.data(name)), value(name, eb.data(name))
        pa, pb = ea.pointers(name), eb.pointers(name)
        named = lambda p: {o: r for o, r in p.items() if not r[0].startswith((".", "@"))}
        wrong = sorted(o for o in set(named(pa)) | set(named(pb)) if named(pa).get(o) != named(pb).get(o)
                       and not (o in pa and o in pb and (pa[o][0].startswith((".", "@")) or pb[o][0].startswith((".", "@")))))
        if wrong:
            o = wrong[0]
            flags.append(f"DATA   {name}: +{o:#x} points at {pa.get(o, ('nothing',))[0]} in the original, "
                         f"{pb.get(o, ('nothing',))[0]} in yours" + (f" (+{len(wrong) - 1} more)" if len(wrong) > 1 else ""))
            continue
        relocated = set(pa) | set(pb)
        if x == y or all(any(r <= i < r + 4 for r in relocated) for i in range(min(len(x), len(y))) if x[i] != y[i]) \
                and len(x) == len(y):
            continue
        if len(x) == len(y) in (4, 8):
            flags.append(f"DATA   {name}: original {show(x)}, yours {show(y)}")
        else:
            i = next((k for k in range(min(len(x), len(y))) if x[k] != y[k]), min(len(x), len(y)))
            flags.append(f"DATA   {name}: differs from byte {i:#x} (original {x[i:i + 8].hex(' ') or '(end)'}, "
                         f"yours {y[i:i + 8].hex(' ') or '(end)'})")
    return flags


def main():
    args = sys.argv[1:]
    obj = None
    if len(args) == 3 and args[1] == "--object":
        obj = args[2]
    elif len(args) != 1:
        sys.exit(__doc__)
    unit = args[0].removeprefix("src/").removesuffix(".c")
    name, target, base = unit_objects(unit)
    base = obj or base
    if not base or not (ROOT / base).exists():
        sys.exit(f"semcheck.py: {name} has no built object yet; build first")
    r = subprocess.run([str(OBJDIFF), "diff", "-1", str(ROOT / target), "-2", str(ROOT / base), "-c",
                        "functionRelocDiffs=data_value", "-o", "-"], capture_output=True, text=True)
    if r.returncode or not r.stdout.strip():
        sys.exit(f"semcheck.py: objdiff-cli failed on {name}: {r.stderr.strip()[:300]}")
    d = json.loads(r.stdout)
    left = {s["name"]: s for s in d["left"]["symbols"] if "instructions" in s}
    ea, eb = Elf(ROOT / target), Elf(ROOT / base)
    fl = float_values(ea)
    fr = {**fl, **float_values(eb)}  # an extern constant in yours resolves to the original's symbol of that name
    dl = {n: ea.data(n) for n in ea.syms}
    dr = {**dl, **{n: eb.data(n) for n in eb.syms}}
    total = 0
    for s in d["right"]["symbols"]:
        if "instructions" not in s or s["name"] not in left:
            continue
        pct = s.get("match_percent", 0)
        if pct >= 100:
            continue
        rl, rr = rows(left[s["name"]]), rows(s)
        a = [(t, ln) for t, ln, _ in rl if t]
        b = [(t, ln) for t, ln, _ in rr if t]
        if pct < 20 or len(b) < len(a) / 2:
            # unfinished: only the checks that stay precise on partial code (arguments of calls both make, stored
            # constants); the note isn't counted as a flag
            print(f"{s['name']} ({pct:.2f}%): not written yet ({len(b)} of the original's {len(a)} instructions)")
            early = ([f for f in check_calls(a, b, fl, fr) if not f.startswith("CALL   original calls")] + check_stores(a, b)
                     + [f for f in check_values(a, b, set()) if f.startswith("VALUE  original `cmp")])
            for f in early:
                print("  " + f)
            total += len(early)
            continue
        cf = check_calls(a, b, fl, fr)
        reported = {(int(m.group(1), 16), int(m.group(2), 16))
                    for f in cf for m in re.finditer(r"original (0x[0-9a-f]+), yours (0x[0-9a-f]+)", f)}
        values = check_values(a, b, reported)
        if Counter(REG.sub("R", t) for t, _ in a if t.startswith("cmp")) == \
                Counter(REG.sub("R", t) for t, _ in b if t.startswith("cmp")):
            values = [f for f in values if not f.startswith("VALUE  original `cmp")]  # same compares, other order
        has_switch = any(t.startswith("bctr") for t, _ in a)
        cmps = [f + ("  [may be switch case order]" if has_switch else "") for f in values if f.startswith("VALUE  original `cmp")]
        values = [f for f in values if not f.startswith("VALUE  original `cmp")]
        flags = (cf + check_symbols(rl, rr, dl, dr) + check_floats(a, b, fl, fr) + values + check_stores(a, b) + cmps
                 + check_mem(a, b) + check_ops(a, b))
        if flags:
            total += len(flags)
            print(f"{s['name']} ({pct:.2f}%)")
            for f in flags:
                print("  " + f)
    data = check_data(ea, eb)
    for f in data:
        print(f)
    total += len(data)
    print(f"semcheck: {total} flag(s) in {name}" + (" (leads: check each against the asm)" if total else ""))


if __name__ == "__main__":
    main()
