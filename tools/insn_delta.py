"""List near-miss functions whose instruction count differs from the original's, with the opcodes that differ.

    python3 tools/insn_delta.py [--min 80] [--top N] [UNIT-FRAGMENT ...]

For each non-matching function at or above --min percent, this compares the original's and ours instruction
counts and, where they differ, the opcode multisets: `+28 fsubs` means ours has 28 more `fsubs` than the original.
A function stuck on register colouring has equal counts, so a count gap is a source-shape lead, sorted largest
first. Patterns seen so far:
  - extra fsubs/fadds/lfs: the original CSEs an expression across calls that ours recomputes (a literal where
    ours reads a named constant: winKirinukiGX), or the reverse.
  - extra frsp, fsub vs fsubs, lfd vs lfs: a double literal or `__fabs` where the original uses f32.
  - extra psq_st/stfd: ours keeps a hoisted constant in a saved FPR (mario_hit's gndCenti/gndAngle).
  - paired lwz/stw: a struct copy only one side makes (a temp the original builds and copies, or the reverse).
  - lwzx vs lwz+add, or `b`/`bne` vs `beq`: indexing form or branch layout; often allocator-driven, check first.
"""
import collections
import json
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import store_sweep  # noqa: E402


def opcodes(sym):
    """Counter of mnemonics for one objdiff symbol."""
    c = collections.Counter()
    for i in sym.get("instructions", []):
        ins = i.get("instruction")
        if ins:
            c[next((p["opcode"]["mnemonic"] for p in ins.get("parts", []) if "opcode" in p), "")] += 1
    return c


def delta(orig, ours, top=4):
    """(ours count - original count, 'opcode deltas') for two opcode Counters; the deltas largest first."""
    diff = (orig - ours) + (ours - orig)
    parts = [f"{'+' if ours[k] > orig[k] else '-'}{abs(ours[k] - orig[k])} {k}" for k, _ in diff.most_common(top)]
    return sum(ours.values()) - sum(orig.values()), ", ".join(parts)


def scan(name, fns):
    t, o = store_sweep.objects(name)
    if not (os.path.exists(t) and os.path.exists(o)):
        return []
    r = subprocess.run([store_sweep.OBJDIFF, "diff", "-1", t, "-2", o, "-o", "-"], capture_output=True, text=True)
    if r.returncode:
        return []
    d = json.loads(r.stdout)
    right = {s.get("name"): s for s in d["right"]["symbols"]}
    out = []
    for s in d["left"]["symbols"]:
        if s.get("name") in fns and s["name"] in right:
            a, b = opcodes(s), opcodes(right[s["name"]])
            n, desc = delta(a, b)
            if n:
                out.append((n, s["name"], sum(a.values()), sum(b.values()), desc))
    return out


def main(argv):
    lo = float(argv[argv.index("--min") + 1]) if "--min" in argv else 80.0
    top = int(argv[argv.index("--top") + 1]) if "--top" in argv else 0
    skip = {i + 1 for i, x in enumerate(argv) if x in ("--min", "--top")}
    frags = [x for i, x in enumerate(argv) if not x.startswith("--") and i not in skip]
    report = json.load(open(os.path.join(store_sweep.BUILD, "report.json")))
    rows = []
    for u in report["units"]:
        if frags and not any(f in u["name"] for f in frags):
            continue
        fns = {f["name"]: f.get("fuzzy_match_percent") or 0 for f in u.get("functions", [])
               if lo <= (f.get("fuzzy_match_percent") or 0) < 100}
        if fns:
            rows += [(n, u["name"], fn, a, b, fns[fn], desc) for n, fn, a, b, desc in scan(u["name"], fns)]
    rows.sort(key=lambda r: (-abs(r[0]), r[1], r[2]))
    for n, unit, fn, a, b, pct, desc in rows[:top] if top else rows:
        print(f"{n:+5d}  {unit:42s} {fn:36s} orig {a:5d} ours {b:5d}  {pct:6.2f}%  {desc}")
    print(f"{len(rows)} function(s) with a different instruction count")


if __name__ == "__main__":
    main(sys.argv[1:])
