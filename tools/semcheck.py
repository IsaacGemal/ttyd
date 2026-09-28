#!/usr/bin/env python3
"""Find likely WRONG CODE in a unit's partly matched functions: the differences from the original that register
allocation can't explain. objdiff's score can't tell a wrong constant from a register swap; these checks can.

  python3 tools/semcheck.py UNIT        e.g. effect/eff_fire or MSL_C.PPCEABI.bare.H.a/printf

For every function below 100% (and every data symbol the unit defines), after a build:
  CALL   a call the original makes that yours doesn't (or the reverse), or an integer argument (li r3-r10) that differs
  VALUE  the same instruction with a different constant: a wrong literal, mask, offset or argument
  MEM    loads/stores by (instruction, offset), ignoring which register holds the address: a missing, extra or
         wrongly offset field access
  OPS    instruction counts that differ (e.g. original has one more `bge`, yours one more `blt`): the C asks for a
         different operation somewhere, such as an inverted test or a missing step
  DATA   a data symbol whose bytes differ from the original's (tables, colours, strings)
Lines are your source's, where the compiler recorded one. Every flag is a lead, not a verdict: a reordered block
can show as two VALUE or MEM flags, so check each against build/G8MJ01/asm/<unit>.s before changing anything.
Exit 0 always.

The VALUE, OPS and MEM rules are SFA-Decomp's (CC0 1.0; tools/wrongvalue_scan.py, tools/a71_mnhist_scan.py and the
memory-access sweep in docs/aug05-A103-wrong-computation-sweep.md), run here over objdiff's per-function diff so
each flag can name a source line; tools/sfa/ has their project-wide versions. CALL follows a script from a wave 13
review.
"""
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
ARGS = [f"r{n}" for n in range(3, 11)]
LOCAL_BASES = ("r1", "r2", "r13", "r0")
MEM_DETAIL = 8  # more differing offsets than this is an addressing difference, summarised in one line  # stack frame and small-data bases: frame layout, not field accesses


def unit_objects(unit):
    for u in json.load(open(ROOT / "objdiff.json"))["units"]:
        if u["name"].split("/", 1)[1] == unit:
            return u.get("target_path"), u.get("base_path"), u.get("metadata", {}).get("source_path")
    sys.exit(f"semcheck.py: no unit {unit} in objdiff.json")


def instructions(symbol):
    out = []
    for x in symbol.get("instructions", []):
        i = x.get("instruction")
        if i and i.get("formatted"):
            out.append((i["formatted"], x.get("line_number") or i.get("line_number")))
    return out


def lines(entries):
    ls = sorted({ln for _, ln in entries if ln})
    return f" (line{'s' * (len(ls) > 1)} {', '.join(map(str, ls[:4]))}{'...' if len(ls) > 4 else ''})" if ls else ""


def is_branch(text):
    return re.match(r"b\w*\s", text) is not None


def calls(ins):
    out, regs = [], {}
    for text, ln in ins:
        m = re.match(r"li (r(?:[3-9]|10)), (-?0x[0-9a-f]+|-?\d+)$", text)
        if m:
            regs[m.group(1)] = int(m.group(2), 0)
            continue
        m = re.match(r"\w+\.?\s+(r(?:[3-9]|10)),", text)
        if m and not text.startswith(("st", "cmp", "b")):
            regs.pop(m.group(1), None)
        m = re.match(r"bl (\S+)", text)
        if m:
            out.append((m.group(1), dict(regs), ln))
            regs = {}
        elif is_branch(text):
            regs = {}
    return out


def check_calls(a, b):
    ca, cb = calls(a), calls(b)
    flags = []
    sm = difflib.SequenceMatcher(None, [c[0] for c in ca], [c[0] for c in cb], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for x, y in zip(ca[i1:i2], cb[j1:j2]):
                d = [f"{r}: original {x[1][r]:#x}, yours {y[1][r]:#x}" for r in ARGS
                     if r in x[1] and r in y[1] and x[1][r] != y[1][r]]
                if d:
                    flags.append(f"CALL   {x[0]}{' (line %s)' % y[2] if y[2] else ''}: {'; '.join(d)}")
        else:
            miss, extra = [c[0] for c in ca[i1:i2]], [c[0] for c in cb[j1:j2]]
            if Counter(miss) == Counter(extra):
                continue
            near = next((c[2] for c in cb[max(j1 - 1, 0):j2 + 1] if c[2]), None)
            flags.append(f"CALL   original calls {', '.join(miss) or 'nothing'} where yours calls "
                         f"{', '.join(extra) or 'nothing'}{' (near line %s)' % near if near else ''}")
    return flags


def shape(text):
    return NUM.sub(lambda m: m.group(0)[:-len(m.group(1))] + "N", REG.sub("R", text))


def check_values(a, b):
    """SFA's wrongvalue rule: same masked instruction, different constant. Regions whose constants are the same
    multiset on both sides are reorders, not wrong values, and are skipped."""
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
        va = Counter(v for x, _ in pairs for v in NUM.findall(REG.sub("R", x[0])))
        vb = Counter(v for _, y in pairs for v in NUM.findall(REG.sub("R", y[0])))
        if va == vb:
            continue  # the same constants in a different order
        for x, y in pairs:
            nx, ny = NUM.findall(REG.sub("R", x[0])), NUM.findall(REG.sub("R", y[0]))
            if set(nx) != set(ny):
                flags.append(f"VALUE  original `{REG.sub('R', x[0])}`, yours `{REG.sub('R', y[0])}`"
                             f"{' (line %s)' % y[1] if y[1] else ''}")
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
    flags = []
    diff = [k for k in sorted(set(ka) | set(kb)) if ka[k] != kb[k]]
    if len(diff) > MEM_DETAIL:
        return [f"MEM    {len(diff)} load/store offsets differ: the fields are probably reached from a different base"
                " (one struct pointer vs another, or a different address split); compare the asm's addressing"]
    for k in diff:
        ls = sorted(l for l in wb.get(k, ()) if l)
        flags.append(f"MEM    `{k[0]} ..., {k[1]}(base)`: original {ka[k]}, yours {kb[k]}"
                     + (f" (line{'s' * (len(ls) > 1)} {', '.join(map(str, ls[:4]))})" if ls else ""))
    return flags


def check_ops(a, b):
    """SFA's a71 mnemonic histogram: allocation leaves opcode counts alone, so a difference is a different
    operation in the source."""
    ha = Counter(t.split()[0] for t, _ in a)
    hb = Counter(t.split()[0] for t, _ in b)
    more, fewer = ha - hb, hb - ha
    if not more and not fewer:
        return []
    fmt = lambda c: ", ".join(f"{k} x{v}" for k, v in sorted(c.items()))
    return [f"OPS    original has more: {fmt(more) or '-'}; yours has more: {fmt(fewer) or '-'}"]


def check_data(target, base):
    """Data symbols whose bytes differ (relocated words, which read as zero in an object, are ignored)."""
    flags = []
    d = json.loads(subprocess.run([str(OBJDIFF), "diff", "-1", str(target), "-2", str(base), "-c",
                                   "functionRelocDiffs=none", "-o", "-"], capture_output=True, text=True).stdout)
    right = {s["name"]: s for s in d["right"]["symbols"]}
    for s in d["left"]["symbols"]:
        if s.get("kind") != "SYMBOL_OBJECT" or s["name"] not in right or s["name"].startswith(("str_", "@stringBase")):
            continue  # strings differ only in trailing padding
        pct = right[s["name"]].get("match_percent")
        if pct is not None and pct < 100:
            flags.append(f"DATA   {s['name']}: {pct:.1f}% of its bytes match the original's")
    return flags


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    unit = sys.argv[1].removeprefix("src/").removesuffix(".c")
    target, base, source = unit_objects(unit)
    if not base or not (ROOT / base).exists():
        sys.exit(f"semcheck.py: {unit} has no built object yet; build first")
    d = json.loads(subprocess.run([str(OBJDIFF), "diff", "-1", str(ROOT / target), "-2", str(ROOT / base), "-c",
                                   "functionRelocDiffs=none", "-o", "-"], capture_output=True, text=True).stdout)
    left = {s["name"]: s for s in d["left"]["symbols"] if "instructions" in s}
    total = 0
    for s in d["right"]["symbols"]:
        if "instructions" not in s or s["name"] not in left:
            continue
        pct = s.get("match_percent", 0)
        if pct >= 100:
            continue
        a, b = instructions(left[s["name"]]), instructions(s)
        cf = check_calls(a, b)
        called = {m.group(1) for f in cf for m in [re.search(r"\(line (\d+)\)", f)] if m}
        values = [f for f in check_values(a, b) if not re.search(r"`li R, .*\(line (%s)\)$" % "|".join(called), f)
                  or not called]
        cmps = [f + "  [may be switch case order]" for f in values if f.startswith("VALUE  original `cmp")]
        values = [f for f in values if not f.startswith("VALUE  original `cmp")]
        flags = cf + values + check_mem(a, b) + check_ops(a, b) + cmps
        if flags:
            total += len(flags)
            print(f"{s['name']} ({pct:.2f}%){lines(b[:1])}")
            for f in flags:
                print("  " + f)
    data = check_data(ROOT / target, ROOT / base)
    for f in data:
        print(f)
    total += len(data)
    print(f"semcheck: {total} flag(s) in {unit}" + (" (check each against the asm)" if total else ""))


if __name__ == "__main__":
    main()
