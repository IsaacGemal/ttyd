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
`#pragma once`, `#include`, function prototypes, `struct X;` forward declarations, `typedef struct X X;`, and type
definitions, as the codebase's own headers hold them (e.g. an effect's work struct): struct/union/enum bodies,
their fields and enum members. Function bodies, data, macros and other directives are refused everywhere.
A type definition is applied as one block, never line by line: appended whole, or skipped when the header already
has exactly that block. If the header already defines the name differently, applying fails (two agents' layouts for
one struct must be reconciled by hand), since skipping already-present lines would mangle a body.
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


# Type definitions (struct/union/enum bodies, their fields and enum members), as the codebase's own headers define
# an effect's work struct.
C = r"(\s*//.*)?"
OPEN = re.compile(r"(typedef\s+)?(struct|union|enum)(?:\s+\w+)?\s*\{" + C)
FIELD = re.compile(r"(?:(?:const|unsigned|signed|volatile|struct|union|enum)\s+)*\w+[\s\*]+\w+"
                   r"(?:\s*\[[\w\s+\-*/()]+\])*(?:\s*:\s*\d+)?\s*;" + C)
FNPTR = re.compile(r"(?:(?:const|unsigned|struct)\s+)*\w+[\s\*]*\(\s*\*\s*\w+\s*\)\s*\([^;{}]*\)\s*;" + C)
MEMBER = re.compile(r"\w+(?:\s*=\s*[-+\w\s()<>|&~]+)?\s*,?" + C)
CLOSE = re.compile(r"\}\s*(\w*)\s*;" + C)


def type_line(body, stack):
    """True if body is part of a type definition, given the open definitions (stack of (typedef?, kind)); updates
    the stack. Fields only inside a struct/union, members only inside an enum, and `} name;` only closing a typedef
    or a nested member, so a header can't define global data."""
    body = re.sub(r"^/\*[^*]*\*/\s*", "", body)  # a leading offset comment: /* 0x00 */ char text[8];
    m = OPEN.fullmatch(body)
    if m and (not stack or stack[-1][1] != "enum"):
        stack.append((bool(m.group(1)), m.group(2)))
        return True
    if not stack:
        return False
    if stack[-1][1] == "enum":
        ok = bool(MEMBER.fullmatch(body))
    else:
        ok = bool(FIELD.fullmatch(body) or FNPTR.fullmatch(body))
    m = CLOSE.fullmatch(body)
    if m:
        typedef, _ = stack.pop()
        return not m.group(1) or typedef or bool(stack)
    return ok


TARGET = re.compile(r"^\+\+\+ b/(include/(?:[\w.-]+/)*[\w.-]+\.h)(?:\t.*)?$")


def additions(patch, created=None):
    """{header path: [added lines]} in patch order; created (a set) collects the headers the patch creates."""
    out, cur = {}, None
    for line in patch.splitlines():
        if line.startswith(("diff ", "--- ")):
            cur = None  # a new file section: nothing belongs to the previous header any more
        elif line.startswith("+++ "):
            m = TARGET.match(line)
            cur = m.group(1) if m and ".." not in m.group(1) else None
            if cur:
                out.setdefault(cur, [])
        elif line.startswith("@@ -0,0 ") and cur and created is not None:
            created.add(cur)
        elif line.startswith("+") and cur:
            out[cur].append(line[1:])
    return out


def declaration(body):
    return any(re.fullmatch(rx, body) for rx in ALLOWED)


def problems(patch):
    out, cur, sections, new, stack = [], None, 0, False, []
    for n, line in enumerate(patch.splitlines(), 1):
        if line.startswith(("diff ", "--- ")):
            cur = None
            continue
        if line.startswith(("+++ ", "diff ")) and stack:
            out.append(f"line {n}: a type definition is never closed before the next file")
            stack = []
        if line.startswith("+++ "):
            m = TARGET.match(line)
            if not m or ".." in m.group(1):
                out.append(f"line {n}: patches something other than a header under include/: {line[4:].split(chr(9))[0]}")
            cur = m.group(1) if m and ".." not in m.group(1) else None
            sections += cur is not None
            new, stack = False, []
            continue
        if line.startswith("@@"):
            new = new or line.startswith("@@ -0,0 ")
            continue
        if line.startswith(("Only in", "\\ No newline")):
            continue
        if line.startswith("+") and cur is None:
            out.append(f"line {n}: an added line outside a header section: {line[1:].strip()}")
            continue
        if stack and not line.startswith("+"):
            out.append(f"line {n}: an existing line inside a new type definition (only whole new definitions may be added)")
            stack = []
        if line.startswith("-"):
            out.append(f"line {n}: removes or changes an existing header line: {line[1:].strip()}")
        elif line.startswith("+"):
            body = line[1:].strip()
            if re.match(r"static\b", body):
                out.append(f"line {n}: static declarations belong in the .c file: {body}")
            elif body.endswith("\\"):
                out.append(f"line {n}: a line ending in a backslash continues into the next one: {body}")
            elif not type_line(body, stack) and (stack or not declaration(body)):
                out.append(f"line {n}: not a declaration or type definition: {body}")
    if stack:
        out.append("a type definition is never closed at the end of the patch")
    if patch.strip() and not sections and not out:
        out.append("no header sections (`+++ b/include/...h`) found: not a unified diff of include/")
    return out


def blocks(lines):
    """The added lines as ("line", text) and ("type", [lines of one whole struct/union/enum definition])."""
    stack, cur = [], None
    for l in lines:
        if cur is None and not type_line(l.strip(), stack):
            yield "line", l
            continue
        if cur is None:
            cur = []
        else:
            type_line(l.strip(), stack)
        cur.append(l)
        if not stack:
            yield "type", cur
            cur = None
    if cur:
        yield "type", cur


def type_names(block):
    """The block's struct/union/enum tag and its typedef name (either may be missing)."""
    m = CLOSE.fullmatch(block[-1].strip())
    tag = re.match(r"\s*(?:typedef\s+)?(?:struct|union|enum)\s+(\w+)", block[0])
    return [n for n in ((tag and tag.group(1)), (m and m.group(1))) if n]


def code(text):
    """text without comments, so a commented-out definition doesn't count as one."""
    return re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)


def defines(text, name):
    return bool(re.search(rf"(?:\b(?:struct|union|enum)\s+{name}\s*\{{|\}}\s*{name}\s*;)", code(text)))


def squash(lines):
    return "\n".join(re.sub(r"\s+", " ", l.strip()) for l in lines if l.strip())


def apply(patch, root, counts=None):
    """Append the patch's new lines; returns the headers changed (and fills counts with lines added per header)."""
    changed = []
    for path, lines in additions(patch).items():
        f = root / path
        text = f.read_text() if f.exists() else ""
        have = {l.strip() for l in text.splitlines()}
        new = []
        for kind, item in blocks(lines):
            if kind == "line":
                if item.strip() and item.strip() not in have:
                    new.append(item)
                    have.add(item.strip())
            elif squash(code("\n".join(item)).splitlines()) in squash(code(text + "\n" + "\n".join(new)).splitlines()):
                continue  # exactly this definition is already there (another agent's identical patch)
            elif any(defines(text + "\n".join(new), n) for n in type_names(item)):
                raise SystemExit(f"header_patch.py: {path} already defines {' or '.join(type_names(item))} "
                                 "differently; reconcile the two definitions by hand")
            else:
                new += item
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
