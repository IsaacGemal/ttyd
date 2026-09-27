#!/usr/bin/env python3
"""Check an agent's header patch before merging it: it may only add declarations.

  python3 tools/header_patch.py PATCH

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
    r'#\s*include\s+[<"][\w/.]+[>"]',
    r"struct\s+\w+\s*;",
    r"typedef\s+struct\s+\w+\s+\w+\s*;",
    r"(?:extern\s+)?(?:const\s+|unsigned\s+|struct\s+)*[A-Za-z_]\w*[\s\*]+\w+\s*\([^;{}]*\)\s*;(\s*//.*)?",
    r"USER_FUNC\(\w+\)\s*;",
]


def problems(patch):
    out = []
    for n, line in enumerate(patch.splitlines(), 1):
        if line.startswith(("+++", "---", "diff ", "@@", "Only in", "\\ No newline")):
            continue
        if line.startswith("-"):
            out.append(f"line {n}: removes or changes an existing header line: {line[1:].strip()}")
        elif line.startswith("+"):
            body = line[1:].strip()
            if not any(re.fullmatch(rx, body) for rx in ALLOWED):
                out.append(f"line {n}: not a declaration: {body}")
            elif re.match(r"static\b", body):
                out.append(f"line {n}: static declarations belong in the .c file: {body}")
    return out


def main():
    p = problems(open(sys.argv[1]).read())
    if p:
        print("\n".join(p))
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
