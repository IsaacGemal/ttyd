#!/usr/bin/env python3
"""Cleanup index: how much of the source tree a reader still has to decode.

    python3 tools/cleanup_index.py              the working tree (src/, rels/*/src, include/)
    python3 tools/cleanup_index.py --rev REV    a git revision instead (for the delta in tools/ci.py)
    python3 tools/cleanup_index.py --json       machine-readable output
    python3 tools/cleanup_index.py --top N      how many worst files to list (default 10)

Counts, per file and in total, the things a reader would want cleaned up. Each category has a weight; the index is
the weighted count per 1000 non-blank code lines, so growing the tree does not by itself make it worse. Lower is
better. Comments are ignored. Nothing here is a build or pass/fail check: tools/ci.py only reports the index and
its delta, with a warning when it gets worse.

Categories (definition changed 2026-10-20, n9-q-idxtool; the index on main (1e542f53) went 29.577 -> 23.362, so compare
numbers only within one definition: tools/ci.py measures HEAD and its base with the same script, and docs/CONTEXT.md
records old and new values for main and the reference points):
- address_name (1): a symbol named by its address: dat_, vec3_, str_, lbl_, L_, jumptable_, kuribo_init_800C23B4
  and so on. Before the change it also counted unk_ names (counted again by unk_name) and pool constants.
- pool_constant (0, new): a compiler pool constant named by its value: float_<value>_<addr>, double_<value>_<addr>,
  double_to_int_<addr>, zero_<addr>, one/negone/half/two_<addr>. Reported in the table, not in the index: many have
  to stay to keep a match.
- raw_offset (2): a `((u8*)p + ...)` cast, and, new, each call of a file-local offset macro instead of its
  #define. An offset macro is a function-like macro defined once in the file whose body has such a cast with a
  parameter in the offset or in an index applied to the result: WW_S32(w, off), MF32(off), NKF(off), A14L(i).
  Every call of the name in the file counts, with no #undef scope. A macro whose parameter is only the base
  (ANIM_FRAME_TIME(f) over `(u8*)(f) + 0x0`) is a named accessor: its #define is the one raw offset, as before.
  tools/offset_to_field.py --apply converts the subset of the shape `(*(T*)((u8*)(p) + (off)))`.
"""
import argparse
import json
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

# category: (weight, regex over comment-stripped text). Counted once per match.
PATTERNS = {
    "unk_name": (1, re.compile(r"\bunk_\w+")),
    "field_0x": (1, re.compile(r"\bfield_0x[0-9A-Fa-f]+\b")),
    "address_name": (1, re.compile(r"\b(?!(?:unk|float|double|zero|one|negone|half|two)_)\w+_8[0-9A-Fa-f]{7}\b")),
    "pool_constant": (0, re.compile(r"\b(?:float|double|zero|one|negone|half|two)_(?:\w*_)?8[0-9A-Fa-f]{7}\b")),
    "raw_offset": (2, re.compile(r"\(\s*\(\s*u8\s*\*\s*\)\s*\(?\s*[\w.\->\[\]]+\s*\)?\s*\+")),
    "goto": (2, re.compile(r"\bgoto\s+\w+\s*;")),
    "must_match": (1, re.compile(r"^[ \t]*#\s*(?:if|ifdef|ifndef|elif)\b.*\bMUST_MATCH\b", re.M)),
    "opt_pragma": (1, re.compile(r"^[ \t]*#\s*pragma\s+(?:opt_\w+|peephole|scheduling|optimization_level"
                                 r"|optimize_for_size|global_optimizer|inline_\w+|auto_inline|dont_inline"
                                 r"|defer_codegen|fp_contract)\b", re.M)),
    "fn_ptr_cast": (2, re.compile(r"\(\s*[\w\s]+\(\s*\*\s*\)\s*\(")),
    "inline_asm": (3, re.compile(r"\b(?:asm|__asm__|__asm)\b")),
}
WEIGHTS = {name: w for name, (w, _) in PATTERNS.items()}
SCOPES = ("src", "include")  # plus rels/*/src


def strip_comments(text):
    """Blank out comments, keeping newlines so line numbers survive."""
    def blank(m):
        return re.sub(r"[^\n]", " ", m.group())
    text = re.sub(r"/\*.*?\*/", blank, text, flags=re.S)
    return re.sub(r"//[^\n]*", blank, text)


def offset_operand(body, end):
    """The rest of the `((u8*)base + ...)` group that opens before END, plus any `)` and `[...]` straight after it:
    the offset, and an index applied to the result."""
    depth, i = 1, end
    while i < len(body) and depth:
        depth += {"(": 1, ")": -1}.get(body[i], 0)
        i += 1
    while i < len(body):
        if body[i] in ") \t":
            i += 1
        elif body[i] == "[":
            j = body.find("]", i)
            i = len(body) if j < 0 else j + 1
        else:
            break
    return body[end:i]


def offset_macros(code):
    """{name: (first, last) line indices of its #define} for the function-like macros of one file that are defined
    once and whose body is a raw offset where a parameter supplies the offset or the index (WW_S32(w, off), NKF(off),
    A14L(i)), so every call is its own raw offset. A macro whose parameter is only the base, such as
    ANIM_FRAME_TIME(f) over `(u8*)(f) + 0x0`, is a named accessor: its one #define is the raw offset."""
    raw = PATTERNS["raw_offset"][1]
    lines = code.split("\n")
    seen, found = Counter(), {}
    i = 0
    while i < len(lines):
        m = re.match(r"\s*#\s*define\s+(\w+)\(([^)]*)\)(.*)$", lines[i])
        if not m:
            i += 1
            continue
        first, name, body = i, m.group(1), m.group(3)
        params = [p.strip() for p in m.group(2).split(",") if p.strip()]
        while body.rstrip().endswith("\\") and i + 1 < len(lines):
            i += 1
            body = body.rstrip()[:-1] + " " + lines[i]
        seen[name] += 1
        r = raw.search(body)
        if r and params and re.search(r"\b(?:" + "|".join(map(re.escape, params)) + r")\b", offset_operand(body, r.end())):
            found[name] = (first, i)
        i += 1
    return {n: span for n, span in found.items() if seen[n] == 1}


def blank_offset_macros(code):
    """(code with the #define of each offset macro blanked, number of calls of those macros in the code)."""
    spans = offset_macros(code)
    if not spans:
        return code, 0
    lines = code.split("\n")
    for first, last in spans.values():
        for k in range(first, last + 1):
            lines[k] = ""
    code = "\n".join(lines)
    names = sorted(spans, key=len, reverse=True)
    return code, len(re.findall(r"\b(?:" + "|".join(map(re.escape, names)) + r")\s*\(", code))


def count_text(text):
    """(non-blank code lines, {category: count}) for one file's text."""
    code = strip_comments(text)
    lines = sum(1 for l in code.splitlines() if l.strip())
    code, macro_calls = blank_offset_macros(code)
    counts = {}
    for name, (_, rx) in PATTERNS.items():
        n = len(rx.findall(code)) + (macro_calls if name == "raw_offset" else 0)
        if n:
            counts[name] = n
    return lines, counts


def in_scope(path):
    parts = path.split("/")
    if not path.endswith((".c", ".h")):
        return False
    if parts[0] in SCOPES:
        return True
    return len(parts) > 3 and parts[0] == "rels" and parts[2] == "src"


def worktree_sources():
    for path in sorted(ROOT.glob("src/**/*")) + sorted(ROOT.glob("include/**/*")) + sorted(ROOT.glob("rels/*/src/**/*")):
        rel = path.relative_to(ROOT).as_posix()
        if path.is_file() and in_scope(rel):
            yield rel, path.read_text(errors="replace")


def rev_sources(rev):
    """The same files as they are in REV, read with one git cat-file batch (no checkout)."""
    names = subprocess.run(["git", "ls-tree", "-r", "-z", "--name-only", rev, "--", "src", "include", "rels"],
                           cwd=ROOT, capture_output=True, check=True).stdout.decode().split("\0")
    names = [n for n in names if in_scope(n)]
    out = subprocess.run(["git", "cat-file", "--batch"], cwd=ROOT, check=True, capture_output=True,
                         input="".join(f"{rev}:{n}\n" for n in names).encode()).stdout
    pos = 0
    for name in names:
        eol = out.index(b"\n", pos)
        header = out[pos:eol].split()
        size = int(header[2])
        body = out[eol + 1:eol + 1 + size]
        pos = eol + 1 + size + 1
        yield name, body.decode(errors="replace")


def summarise(sources):
    """{'index', 'lines', 'weighted', 'categories', 'files'} for an iterable of (path, text)."""
    files, total_lines, total = {}, 0, {}
    for path, text in sources:
        lines, counts = count_text(text)
        weighted = sum(WEIGHTS[c] * n for c, n in counts.items())
        files[path] = {"lines": lines, "weighted": weighted, "counts": counts}
        total_lines += lines
        for c, n in counts.items():
            total[c] = total.get(c, 0) + n
    weighted = sum(f["weighted"] for f in files.values())
    for f in files.values():
        f["index"] = round(1000 * f["weighted"] / f["lines"], 3) if f["lines"] else 0.0
    return {
        "index": round(1000 * weighted / total_lines, 3) if total_lines else 0.0,
        "lines": total_lines,
        "weighted": weighted,
        "categories": {c: {"count": n, "weight": WEIGHTS[c], "weighted": WEIGHTS[c] * n}
                       for c, n in sorted(total.items(), key=lambda kv: -WEIGHTS[kv[0]] * kv[1])},
        "files": files,
    }


def index(rev=None):
    """The summary for the working tree, or for REV when given."""
    return summarise(rev_sources(rev) if rev else worktree_sources())


def report(s, top):
    print(f"cleanup index {s['index']:.3f} per kLOC ({s['weighted']} weighted over {s['lines']} code lines; lower is better)")
    print(f"  {'category':14} {'count':>6} {'weight':>6} {'weighted':>8}")
    for c, v in s["categories"].items():
        print(f"  {c:14} {v['count']:6} {v['weight']:6} {v['weighted']:8}")
    worst = sorted(((p, f) for p, f in s["files"].items() if f["weighted"]), key=lambda kv: (-kv[1]["weighted"], kv[0]))
    if worst and top:
        print(f"  worst {min(top, len(worst))} files by weighted count:")
        for p, f in worst[:top]:
            print(f"    {f['weighted']:5}  {f['index']:8.3f}/kLOC  {p}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rev", help="measure this git revision instead of the working tree")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--top", type=int, default=10)
    a = ap.parse_args()
    s = index(a.rev)
    if a.json:
        json.dump(s, sys.stdout, indent=2, sort_keys=True)
        print()
    else:
        report(s, a.top)


if __name__ == "__main__":
    main()
