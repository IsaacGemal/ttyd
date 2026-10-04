"""For each non-matching function, the most similar already-matched functions anywhere in the game.

    python3 tools/sibling_fns.py [--min 0] [--top 3] [--sim 0.6] [UNIT-FRAGMENT ...] [--json OUT]

Similarity is computed on the ORIGINAL objects (build/G8MJ01/obj, .../MOD/obj), so it measures what the code is, not
how far our C is from it: each function becomes the set of 4-grams of its instruction shapes (primary opcode plus
extended opcode, registers and immediates dropped), and two functions score the Jaccard index of those sets. A matched
sibling at 0.8 is a worked example: its C shows the shapes, idioms and helper calls the stuck function probably uses.
Idea from dougchansan/pkmn-colosseum tools/decomp_work/simindex/families.py (MIT): "solve one, sweep N"."""
import json
import os
import sys
from collections import defaultdict

from elftools.elf.elffile import ELFFile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, "build", "G8MJ01")
XO = {19, 31, 59, 63, 4}  # primary opcodes whose extended opcode tells instructions apart


def shapes(code):
    out = []
    for i in range(0, len(code) - 3, 4):
        w = int.from_bytes(code[i:i + 4], "big")
        op = w >> 26
        out.append((op, (w >> 1) & 0x3FF) if op in XO else (op,))
    return out


def grams(seq, n=4):
    return {tuple(seq[i:i + n]) for i in range(len(seq) - n + 1)} if len(seq) >= n else {tuple(seq)}


def functions(path):
    elf = ELFFile(open(path, "rb"))
    secs = list(elf.iter_sections())
    out = {}
    for sym in elf.get_section_by_name(".symtab").iter_symbols():
        idx = sym["st_shndx"]
        if sym["st_info"]["type"] == "STT_FUNC" and isinstance(idx, int) and sym["st_size"] >= 16:
            out[sym.name] = secs[idx].data()[sym["st_value"]:sym["st_value"] + sym["st_size"]]
    return out


def target(name):
    mod, _, path = name.partition("/")
    return os.path.join(BUILD, "obj", path + ".o") if mod == "MarioSt" else os.path.join(BUILD, mod, "obj", path + ".o")


def main(argv):
    def opt(flag, default):
        return type(default)(argv[argv.index(flag) + 1]) if flag in argv else default
    lo, top, sim, out = opt("--min", 0.0), opt("--top", 3), opt("--sim", 0.6), opt("--json", "")
    skip = {argv[i + 1] for i, a in enumerate(argv) if a in ("--min", "--top", "--sim", "--json")}
    frags = [a for a in argv if not a.startswith("--") and a not in skip]
    report = json.load(open(os.path.join(BUILD, "report.json")))
    matched, stuck = [], []
    for u in report["units"]:
        path = target(u["name"])
        if not os.path.exists(path):
            continue
        scores = {f["name"]: f.get("fuzzy_match_percent") for f in u.get("functions", [])}
        try:
            fns = functions(path)
        except Exception:
            continue
        for fn, code in fns.items():
            g = grams(shapes(code))
            p = scores.get(fn)
            if p == 100.0:
                matched.append((u["name"], fn, g, len(code)))
            elif p is not None and p >= lo and (not frags or any(f in u["name"] for f in frags)):
                stuck.append((u["name"], fn, g, len(code), p))
    # inverted index over grams keeps this from being all-pairs
    index = defaultdict(list)
    for k, (_, _, g, _) in enumerate(matched):
        for x in g:
            index[x].append(k)
    rows = []
    for unit, fn, g, size, p in sorted(stuck, key=lambda s: -s[3]):
        hits = defaultdict(int)
        for x in g:
            for k in index.get(x, ()):
                hits[k] += 1
        best = []
        for k, inter in hits.items():
            mu, mf, mg, msize = matched[k]
            j = inter / (len(g) + len(mg) - inter)
            if j >= sim and not (mu == unit and mf == fn):
                best.append((j, mu, mf, msize))
        best.sort(reverse=True)
        if best:
            rows.append(dict(unit=unit, fn=fn, size=size, score=p,
                             siblings=[dict(sim=round(j, 3), unit=mu, fn=mf, size=ms) for j, mu, mf, ms in best[:top]]))
    for r in rows:
        print(f"{r['unit']} {r['fn']} ({r['size']} B, {r['score']:.2f}%)")
        for s in r["siblings"]:
            print(f"    {s['sim']:.2f}  {s['unit']} {s['fn']} ({s['size']} B)")
    print(f"{len(rows)} of {len(stuck)} non-matching functions have a matched sibling at >= {sim}")
    if out:
        json.dump(rows, open(out, "w"), indent=1)


if __name__ == "__main__":
    main(sys.argv[1:])
