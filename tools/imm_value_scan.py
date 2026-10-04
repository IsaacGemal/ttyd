"""Find wrong numbers in near-miss functions: aligned instructions that agree on everything but an immediate.

    python3 tools/imm_value_scan.py [--min 90] [UNIT-FRAGMENT ...]

For each non-matching function at or above --min percent, objdiff aligns our instructions with the original's. A row
whose mnemonic and registers agree but whose immediate differs (an `li`, `cmpwi`, `addi`, a load/store offset off a
non-stack base, a shift or mask) is usually a wrong literal, argument or struct offset in the C, and fixing it changes
bytes. Rows based on r1 (stack slots move with frame layout) and branch targets are skipped, as are relocated operands
(tools/reloc_value_check.py covers those). Idea from zcanann/SFA-Decomp tools/wrongvalue_scan.py (CC0)."""
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "G8MJ01")
OBJDIFF = os.path.join(BUILD, "..", "tools", "objdiff-cli")
BRANCH = re.compile(r"^b")


def objects(name):
    mod, _, path = name.partition("/")
    t = os.path.join(BUILD, "obj", path + ".o") if mod == "MarioSt" else os.path.join(BUILD, mod, "obj", path + ".o")
    return t, os.path.join(BUILD, "src", path + ".o")


def shape(ins):
    """(mnemonic, registers, immediates, has_reloc, r1_based) of one objdiff instruction."""
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
    return mn, regs, imms, reloc, "r1" in regs[1:]


def scan(name, fns):
    t, o = objects(name)
    if not (os.path.exists(t) and os.path.exists(o)):
        return []
    r = subprocess.run([OBJDIFF, "diff", "-1", t, "-2", o, "-o", "-"], capture_output=True, text=True)
    if r.returncode:
        return []
    d = json.loads(r.stdout)
    right = {s.get("name"): s for s in d["right"]["symbols"]}
    hits = []
    for s in d["left"]["symbols"]:
        if s.get("name") not in fns or s["name"] not in right:
            continue
        based = set()  # registers holding an address built from a relocation (a pool, table or global's base)
        for li, ri in zip(s.get("instructions", []), right[s["name"]].get("instructions", [])):
            if "instruction" not in li or "instruction" not in ri:
                continue
            a, b = shape(li["instruction"]), shape(ri["instruction"])
            dest = a[1][0] if a[1] else None
            if a[3] and dest:
                based.add(dest)
            elif dest and a[0] in ("addi", "mr") and len(a[1]) > 1 and a[1][1] in based:
                based.add(dest)
            elif dest in based and a[0] not in ("stw", "stb", "sth", "stfs", "stfd", "cmpwi", "cmplwi"):
                based.discard(dest)
            if li.get("diff_kind") != "DIFF_ARG_MISMATCH":
                continue
            if a[0] != b[0] or BRANCH.match(a[0]) or a[3] or b[3] or a[4] or b[4]:
                continue
            if len(a[1]) > 1 and a[1][-1] in based:
                continue
            if a[1] == b[1] and a[2] != b[2] and a[0] not in ("lfd", "stfd"):
                hits.append((s["name"], tuple(a[2]), tuple(b[2]), li["instruction"]["formatted"], ri["instruction"]["formatted"]))
    # a pair of values that recurs within a function is a systematic shift (pool or struct layout), not a typo
    seen = {}
    for h in hits:
        seen[(h[0], h[1], h[2])] = seen.get((h[0], h[1], h[2]), 0) + 1
    return [(fn, a, b) for fn, x, y, a, b in hits if seen[(fn, x, y)] < 3]


def main(argv):
    lo = float(argv[argv.index("--min") + 1]) if "--min" in argv else 90.0
    frags = [x for i, x in enumerate(argv) if not x.startswith("--") and (i == 0 or argv[i - 1] != "--min")]
    report = json.load(open(os.path.join(BUILD, "report.json")))
    total = 0
    for u in report["units"]:
        if frags and not any(f in u["name"] for f in frags):
            continue
        fns = {f["name"] for f in u.get("functions", []) if lo <= (f.get("fuzzy_match_percent") or 0) < 100}
        if not fns:
            continue
        hits = scan(u["name"], fns)
        if hits:
            print(u["name"])
            for fn, a, b in hits:
                print(f"  {fn}: original `{a}`  ours `{b}`")
            total += len(hits)
    print(f"{total} immediate mismatch(es)")


if __name__ == "__main__":
    main(sys.argv[1:])
