#!/usr/bin/env python3
"""Check and apply an agent's header patch: it may only add declarations to headers under include/.

  python3 tools/header_patch.py PATCH          check it; print what's wrong, exit 1 if anything is
  python3 tools/header_patch.py --apply PATCH  check it, then append each added line to its header unless the
                                               header already has it; print the headers changed

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
    r'#\s*include\s+[<"](?!.*\.\.)[\w/.]+\.h[>"]',
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
        if line.startswith("+++ "):
            m = TARGET.match(line)
            cur = m.group(1) if m and ".." not in m.group(1) else None
            if cur:
                out.setdefault(cur, [])
        elif line.startswith("+") and cur:
            out[cur].append(line[1:])
    return out


def problems(patch):
    out = []
    for n, line in enumerate(patch.splitlines(), 1):
        if line.startswith("+++ "):
            m = TARGET.match(line)
            if not m or ".." in m.group(1):
                out.append(f"line {n}: patches something other than a header under include/: {line[4:].split(chr(9))[0]}")
            continue
        if line.startswith(("--- a/", "--- /", "diff ", "@@", "Only in", "\\ No newline")):
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


def apply(patch, root):
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
    return changed


def main():
    args = sys.argv[1:]
    do_apply = args[:1] == ["--apply"]
    patch = open(args[-1]).read()
    p = problems(patch)
    if p:
        print("\n".join(p))
        return 1
    if do_apply:
        from pathlib import Path
        print("\n".join(apply(patch, Path(__file__).resolve().parent.parent)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
