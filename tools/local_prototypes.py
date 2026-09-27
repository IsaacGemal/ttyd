#!/usr/bin/env python3
"""Reject external function declarations in game/REL C files.

Shared interfaces belong in headers. Static forward declarations remain local.
A small exact allowlist records original ABI disagreements and symbol collisions;
changing an exception's signature, duplicating it, or leaving a stale entry fails.

This is a lexical check for this project's C declarators and USER_FUNC macro, not
an ABI verifier. The build and objdiff ratchet still validate header changes.
"""
import argparse
from collections import Counter
from dataclasses import dataclass
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
EXCEPTIONS = ROOT / "tools" / "local_prototype_exceptions.json"


def mask_noncode(text):
    """Blank comments, literals and preprocessor lines, preserving offsets."""
    pattern = (r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|'
               r"'(?:\\.|[^'\\])*'|^[ \t]*\#(?:[^\n\\]|\\[^\n]|\\\n)*")
    return re.sub(pattern, lambda m: re.sub(r"[^\n]", " ", m[0]), text, flags=re.M)


@dataclass(frozen=True)
class Prototype:
    name: str
    declaration: str
    line: int
    static: bool


def prototypes(text):
    code = mask_noncode(text)
    # Statement boundaries also catch declarations after another statement on
    # the same line, and prototypes nested inside a function body.
    pattern = re.compile(
        r"(?:^|(?<=[;{}]))[ \t\r\n]*"
        r"(?P<ret>[A-Za-z_][\w\s*]*?)(?<=[\s*])"
        r"(?P<name>[A-Za-z_]\w*)\s*\(", re.M)
    for m in pattern.finditer(code):
        if m["name"] == "USER_FUNC":
            continue  # handled below, including extern/static forms
        ret = m["ret"].split()
        if set(ret) & {"return", "else", "typedef", "if", "while", "switch", "case", "goto", "RETURN", "END"}:
            continue
        depth, end = 1, m.end()
        while end < len(code) and depth:
            depth += (code[end] == "(") - (code[end] == ")")
            end += 1
        if depth:
            continue
        while end < len(code) and code[end].isspace():
            end += 1
        if end >= len(code) or code[end] != ";":
            continue  # a definition, expression, or function-pointer variable
        start = m.start("ret")
        yield Prototype(m["name"], text[start:end + 1],
                        code.count("\n", 0, start) + 1, "static" in ret)
    for m in re.finditer(
        r"(?:^|(?<=[;{}]))[ \t\r\n]*(?P<decl>(?:(?P<storage>static|extern)\s+)?"
        r"USER_FUNC\(\s*(?P<name>\w+)\s*\)\s*;)", code, re.M
    ):
        start = m.start("decl")
        yield Prototype(m["name"], text[start:m.end()],
                        code.count("\n", 0, start) + 1, m["storage"] == "static")


def declaration_key(text):
    return re.sub(r"\s+", "", mask_noncode(text))


def check_sources(sources, exceptions):
    """sources maps repository-relative paths to C text (also usable in tests)."""
    problems, allowed, seen = [], {}, Counter()
    for entry in exceptions:
        key = (entry["path"], entry["name"], declaration_key(entry["declaration"]))
        if not entry.get("reason", "").strip():
            problems.append(f"{entry['path']}: {entry['name']}: exception needs a reason")
        if key in allowed:
            problems.append(f"{entry['path']}: {entry['name']}: duplicate exception")
        allowed[key] = entry
    for path, text in sorted(sources.items()):
        for proto in prototypes(text):
            if proto.static:
                continue
            key = (path, proto.name, declaration_key(proto.declaration))
            if key not in allowed:
                problems.append(f"{path}:{proto.line}: {proto.name}: move external prototype to its owning header")
            else:
                seen[key] += 1
    for key in allowed:
        if seen[key] != 1:
            problems.append(f"{key[0]}: {key[1]}: exception must match exactly once (found {seen[key]})")
    return problems


def check_tree(root=ROOT):
    sources = {str(p.relative_to(root)): p.read_text()
               for folder in ("src", "rels") for p in (root / folder).rglob("*.c")}
    exceptions = json.loads((root / "tools" / EXCEPTIONS.name).read_text())
    return check_sources(sources, exceptions), len(exceptions)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args()
    problems, count = check_tree(args.root)
    if problems:
        print("\n".join(problems))
        return 1
    print(f"ok   [prototypes] no unapproved external local declarations; {count} documented exceptions")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
