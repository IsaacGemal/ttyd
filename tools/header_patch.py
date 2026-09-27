#!/usr/bin/env python3
"""Check and apply an agent's header patch: it may only add declarations to headers under include/.

  python3 tools/header_patch.py PATCH          check it; print what's wrong, exit 1 if anything is
  python3 tools/header_patch.py --apply PATCH  check it, then append each added line to its header unless the
                                               header already has it; print "LINES<tab>HEADER" for each header changed

Applying never uses the patch's line numbers or context: several agents in a wave add to the same headers, so
their patches overlap, and all they may do is add lines. Appending (skipping lines already present) can't delete
or reorder anything, and a line two agents both added appears once.

Agents may add prototypes to the header of the file that defines a function (include/<path>.h), creating the
header if needed; they may not change or remove existing lines. Allowed added lines: blank lines, comments,
`#pragma once`, `#include`, function prototypes, `struct X;` forward declarations and `typedef struct X X;`.
Prints what's wrong and exits 1 if anything else is in the patch.
"""
import re
import sys

ALLOWED = [
    r"",
    r"//.*",
    r"#\s*pragma\s+once",
    r'#\s*include\s+(?:<(?!.*\.\.)[\w/.]+\.h>|"(?!.*\.\.)[\w/.]+\.h")',
    r"struct\s+\w+\s*;",
    r"typedef\s+struct\s+\w+\s+\w+\s*;",
    r"(?:extern\s+)?(?:const\s+|unsigned\s+|struct\s+)*[A-Za-z_]\w*[\s\*]+\w+\s*\([^;{}]*\)\s*;(\s*//.*)?",
    r"USER_FUNC\(\w+\)\s*;",
]


TARGET = re.compile(r"^\+\+\+ b/(include/(?:[\w.-]+/)*[\w.-]+\.h)(?:\t.*)?$")


def additions(patch):
    """{header path: [added lines]} in patch order."""
    out, cur = {}, None
    for line in patch.splitlines():
        if line.startswith(("diff ", "--- ")):
            cur = None  # a new file section: nothing belongs to the previous header any more
        elif line.startswith("+++ "):
            m = TARGET.match(line)
            cur = m.group(1) if m and ".." not in m.group(1) else None
            if cur:
                out.setdefault(cur, [])
        elif line.startswith("+") and cur:
            out[cur].append(line[1:])
    return out


def problems(patch):
    out, cur, sections = [], None, 0
    for n, line in enumerate(patch.splitlines(), 1):
        if line.startswith(("diff ", "--- ")):
            cur = None
            continue
        if line.startswith("+++ "):
            m = TARGET.match(line)
            if not m or ".." in m.group(1):
                out.append(f"line {n}: patches something other than a header under include/: {line[4:].split(chr(9))[0]}")
            cur = m.group(1) if m and ".." not in m.group(1) else None
            sections += cur is not None
            continue
        if line.startswith(("@@", "Only in", "\\ No newline")):
            continue
        if line.startswith("+") and cur is None:
            out.append(f"line {n}: an added line outside a header section: {line[1:].strip()}")
            continue
        if line.startswith("-"):
            out.append(f"line {n}: removes or changes an existing header line: {line[1:].strip()}")
        elif line.startswith("+"):
            body = line[1:].strip()
            if re.match(r"static\b", body):
                out.append(f"line {n}: static declarations belong in the .c file: {body}")
            elif body.endswith("\\"):
                out.append(f"line {n}: a line ending in a backslash continues into the next one: {body}")
            elif not any(re.fullmatch(rx, body) for rx in ALLOWED):
                out.append(f"line {n}: not a declaration: {body}")
    if patch.strip() and not sections and not out:
        out.append("no header sections (`+++ b/include/...h`) found: not a unified diff of include/")
    return out


def apply(patch, root, counts=None):
    """Append the patch's new lines; returns the headers changed (and fills counts with lines added per header)."""
    changed = []
    for path, lines in additions(patch).items():
        f = root / path
        if f.exists():
            text = f.read_text()
            have = {l.strip() for l in text.splitlines()}
            new = [l for l in lines if l.strip() and l.strip() not in have]
        else:
            text, new = "", lines
        if not new:
            continue
        f.parent.mkdir(parents=True, exist_ok=True)
        f.write_text(text + ("" if not text or text.endswith("\n") else "\n") + "\n".join(new) + "\n")
        changed.append(path)
        if counts is not None:
            counts[path] = len(new)
    return changed


def main():
    import argparse
    from pathlib import Path
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("patch")
    a = ap.parse_args()
    patch = open(a.patch).read()
    p = problems(patch)
    if p:
        print("\n".join(p))
        return 1
    if a.apply:
        counts = {}
        for path in apply(patch, Path(__file__).resolve().parent.parent, counts):
            print(f"{counts[path]}\t{path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
