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
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

# category: (weight, regex over comment-stripped text). Counted once per match.
PATTERNS = {
    "unk_name": (1, re.compile(r"\bunk_\w+")),
    "field_0x": (1, re.compile(r"\bfield_0x[0-9A-Fa-f]+\b")),
    "address_name": (1, re.compile(r"\b\w+_8[0-9A-Fa-f]{7}\b")),
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


def count_text(text):
    """(non-blank code lines, {category: count}) for one file's text."""
    code = strip_comments(text)
    lines = sum(1 for l in code.splitlines() if l.strip())
    counts = {}
    for name, (_, rx) in PATTERNS.items():
        n = len(rx.findall(code))
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
