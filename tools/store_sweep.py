"""Find stores and constants the original has and ours lacks (or the reverse) in near-miss functions.

    python3 tools/store_sweep.py [--min 50] [--all] [UNIT-FRAGMENT ...]

For each non-matching function at or above --min percent, this compares the multiset of struct stores (stw/sth/stb/
stfs/stfd and their u/x forms, keyed by mnemonic and offset, off any base but r1) and `li` constants in the original
against ours, regardless of alignment. A store only one side makes is a field the C never writes, or writes with
the wrong width or offset; a `li` only one side has is a wrong literal or a constant the C computes differently.
This found npcEntry's missing `field_110 = -1` and __psndSFXOn's float-wise Vec copy (stfs where retail copies
words). Stack stores (r1) are skipped because frame layout moves them; relocated `li`s (sda21 symbols) are skipped
because tools/reloc_value_check.py covers them. A function whose findings pair up one-for-one at a constant offset
delta (every original offset X matched by X+d in ours) is a systematic layout shift, reported once as SHIFT.
A lone `li 0` on either side is usually the allocator materialising a zero twice, so `li 0` is ignored unless --all.
Loads are reported only when the same offset is read with a different width or signedness on each side (`lha` vs
`lhz` is an s16/u16 field or cast, `lwz` vs `lfs` an int/float type), since load counts move with scheduling; loads at
offset 0 are skipped (mostly a pool constant read through a materialised address)."""
import collections
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "G8MJ01")
OBJDIFF = os.path.join(BUILD, "..", "tools", "objdiff-cli")
STORE = re.compile(r"^(stw|sth|stb|stfs|stfd)u?$")
LOAD = re.compile(r"^(lwz|lhz|lha|lbz|lfs|lfd)u?$")


def objects(name):
    mod, _, path = name.partition("/")
    t = os.path.join(BUILD, "obj", path + ".o") if mod == "MarioSt" else os.path.join(BUILD, mod, "obj", path + ".o")
    return t, os.path.join(BUILD, "src", path + ".o")


def key(ins):
    """('store', mnemonic, offset) or ('li', value) for one objdiff instruction, else None."""
    parts = ins.get("parts", [])
    mn = next((p["opcode"]["mnemonic"] for p in parts if "opcode" in p), "")
    regs, imms, reloc = [], [], False
    for p in parts:
        a = p.get("arg")
        if not a:
            continue
        if "opaque" in a:
            regs.append(a["opaque"])
        elif "signed" in a or "unsigned" in a:
            imms.append(int(a.get("signed", a.get("unsigned"))))
        elif "reloc" in a or "branch_dest" in a:
            reloc = True
    if reloc:
        return None
    if STORE.match(mn) and len(regs) >= 2 and imms and regs[-1] != "r1":
        return ("store", mn.rstrip("u"), imms[0])
    if mn == "li" and imms:
        return ("li", imms[0])
    if LOAD.match(mn) and len(regs) >= 2 and imms and imms[0] and regs[-1] != "r1":
        return ("load", mn.rstrip("u"), imms[0])
    return None


def widths(oa, ob):
    """Offsets loaded with a different width or signedness on each side (`lha` vs `lhz`, `lwz` vs `lfs`)."""
    a = {k[2]: k[1] for k in oa if k[0] == "load"}
    b = {k[2]: k[1] for k in ob if k[0] == "load"}
    return sorted((off, a[off], b[off]) for off in a if off in b and a[off] != b[off])


def compare(orig, ours, zeros=False):
    """(only_in_original, only_in_ours, shift) from two lists of key() results."""
    keep = lambda k: k and (zeros or k != ("li", 0))
    a = collections.Counter(k for k in orig if keep(k) and k[0] != "load")
    b = collections.Counter(k for k in ours if keep(k) and k[0] != "load")
    oa, ob = a - b, b - a
    shift = None
    sa = sorted(k for k in oa.elements() if k[0] == "store")
    sb = sorted(k for k in ob.elements() if k[0] == "store")
    if sa and len(sa) == len(sb) and all(x[1] == y[1] for x, y in zip(sa, sb)):
        deltas = {y[2] - x[2] for x, y in zip(sa, sb)}
        if len(deltas) == 1:
            shift = deltas.pop()
    return dict(oa), dict(ob), shift


def scan(name, fns, zeros=False):
    t, o = objects(name)
    if not (os.path.exists(t) and os.path.exists(o)):
        return []
    r = subprocess.run([OBJDIFF, "diff", "-1", t, "-2", o, "-o", "-"], capture_output=True, text=True)
    if r.returncode:
        return []
    d = json.loads(r.stdout)
    right = {s.get("name"): s for s in d["right"]["symbols"]}
    out = []
    for s in d["left"]["symbols"]:
        if s.get("name") not in fns or s["name"] not in right:
            continue
        orig = [key(i["instruction"]) for i in s.get("instructions", []) if "instruction" in i]
        ours = [key(i["instruction"]) for i in right[s["name"]].get("instructions", []) if "instruction" in i]
        oa, ob, shift = compare(orig, ours, zeros)
        la = collections.Counter(k for k in orig if k and k[0] == "load")
        lb = collections.Counter(k for k in ours if k and k[0] == "load")
        w = widths(la - lb, lb - la)
        if oa or ob or w:
            out.append((s["name"], oa, ob, shift, w))
    return out


def fmt(c):
    return ", ".join(f"{k[1]} {k[2]:#x}" if k[0] == "store" else f"li {k[1]:#x}" for k in sorted(c)) or "-"


def main(argv):
    lo = float(argv[argv.index("--min") + 1]) if "--min" in argv else 50.0
    frags = [x for i, x in enumerate(argv) if not x.startswith("--") and (i == 0 or argv[i - 1] != "--min")]
    report = json.load(open(os.path.join(BUILD, "report.json")))
    total = 0
    for u in report["units"]:
        if frags and not any(f in u["name"] for f in frags):
            continue
        fns = {f["name"] for f in u.get("functions", []) if lo <= (f.get("fuzzy_match_percent") or 0) < 100}
        if not fns:
            continue
        hits = scan(u["name"], fns, "--all" in argv)
        if hits:
            print(u["name"])
            for fn, oa, ob, shift, w in hits:
                for off, x, y in w:
                    print(f"  {fn}: load +{off:#x} is `{x}` in the original, `{y}` in ours")
                if not (oa or ob):
                    continue
                if shift is not None and not any(k[0] == "li" for k in list(oa) + list(ob)):
                    print(f"  {fn}: SHIFT {shift:+#x} ({len(oa)} store(s))")
                    continue
                print(f"  {fn}: original only [{fmt(oa)}]  ours only [{fmt(ob)}]")
            total += len(hits)
    print(f"{total} function(s) with store/constant differences")


if __name__ == "__main__":
    main(sys.argv[1:])
