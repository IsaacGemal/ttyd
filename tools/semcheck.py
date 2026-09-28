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
import base64
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
TEST_OPS = re.compile(r"^(b(?!l\b|lr\b|ctr\b|ctrl\b)\w*|cmp\w*|cror|crnor|crand|creqv|crxor|crorc|crandc|crnand)$")


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
        if (m := re.match(r"bl (\S+)", text)):
            out.append((m.group(1), dict(regs), ln))
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


def check_calls(a, b):
    ca, cb = calls(a), calls(b)
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
                    if va[0:1] == ("const",) and vb[0:1] == ("const",) and va != vb:
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


def data_by_name(side):
    return {s["name"]: data_bytes(s) for s in side["symbols"] if s.get("kind") == "SYMBOL_OBJECT"}


def float_values(side):
    """name -> value of the side's 4- and 8-byte data symbols (float constants, pooled @NNN ones included)."""
    out = {}
    for s in side["symbols"]:
        if s.get("kind") == "SYMBOL_OBJECT":
            b = data_bytes(s)
            if len(b) == 4:
                out[s["name"]] = ("f", struct.unpack(">f", b)[0])
            elif len(b) == 8:
                out[s["name"]] = ("d", struct.unpack(">d", b)[0])
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
    # a value one side never loads is a different literal; the same values loaded a different number of times is
    # the compiler reusing a register
    more = Counter({k: v for k, v in ca.items() if k not in cb})
    fewer = Counter({k: v for k, v in cb.items() if k not in ca})
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
        if not pairs or Counter(x[0] for x, _ in pairs) == Counter(y[0] for _, y in pairs):
            continue
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
    """SFA's a71 mnemonic histogram, kept to compares and branches: a different count there is a different test in
    the source (moves, loads and adds shift with allocation)."""
    ha = Counter(t.split()[0] for t, _ in a if TEST_OPS.match(t.split()[0]))
    hb = Counter(t.split()[0] for t, _ in b if TEST_OPS.match(t.split()[0]))
    more, fewer = ha - hb, hb - ha
    if not more and not fewer:
        return []
    fmt = lambda c: ", ".join(f"{k} x{v}" for k, v in sorted(c.items()))
    return [f"OPS    original has more: {fmt(more) or '-'}; yours has more: {fmt(fewer) or '-'}"]


def data_bytes(symbol):
    return b"".join(base64.b64decode(c.get("data", "")) for c in symbol.get("data_diff", []) if c.get("data"))


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


def check_data(d):
    """Data symbols whose bytes differ; a string is compared up to its terminator (the rest is padding)."""
    right = {s["name"]: s for s in d["right"]["symbols"]}
    flags = []
    for s in d["left"]["symbols"]:
        if s.get("kind") != "SYMBOL_OBJECT" or s["name"] not in right:
            continue
        x, y = data_bytes(s), data_bytes(right[s["name"]])
        if not x or not y:
            continue
        x, y = value(s["name"], x), value(s["name"], y)
        if x == y:
            continue
        if len(x) == len(y) in (4, 8):
            flags.append(f"DATA   {s['name']}: original {show(x)}, yours {show(y)}")
        else:
            i = next((k for k in range(min(len(x), len(y))) if x[k] != y[k]), min(len(x), len(y)))
            flags.append(f"DATA   {s['name']}: differs from byte {i:#x} (original {x[i:i + 8].hex(' ') or '(end)'}, "
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
    fl, fr = float_values(d["left"]), float_values(d["right"])
    dl, dr = data_by_name(d["left"]), data_by_name(d["right"])
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
            print(f"{s['name']} ({pct:.2f}%): not written yet ({len(b)} of the original's {len(a)} instructions)")
            total += 1
            continue
        cf = check_calls(a, b)
        reported = {(int(m.group(1), 16), int(m.group(2), 16))
                    for f in cf for m in re.finditer(r"original (0x[0-9a-f]+), yours (0x[0-9a-f]+)", f)}
        values = check_values(a, b, reported)
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
    data = check_data(d)
    for f in data:
        print(f)
    total += len(data)
    print(f"semcheck: {total} flag(s) in {name}" + (" (leads: check each against the asm)" if total else ""))


if __name__ == "__main__":
    main()
