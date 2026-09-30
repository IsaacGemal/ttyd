#!/usr/bin/env python3
"""Reject external function declarations in game/REL C files.

Shared interfaces belong in headers. A static forward declaration stays local, but only for a function the original
has local to that unit (`.fn NAME, local` in its asm), or one the file defines that the asm doesn't have at all
(inlined everywhere). Otherwise `static` would hide an unchecked prototype of another file's function, or make a
global function local.
A small exact allowlist records original ABI disagreements and symbol collisions;
changing an exception's signature, duplicating it, or leaving a stale entry fails.
An entry with path "*" names a function whose callers disagree in the original: any file may declare it locally,
with the signature that file's asm uses.

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
    code = re.sub(r"__declspec\s*\([^()]*\)", lambda m: " " * len(m[0]), code)
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
        if end < len(code) and code[end] == ",":
            # several declarators in one statement: `extern int a(void), b(int);`
            stop = code.find(";", end)
            if stop < 0:
                continue
            start = m.start("ret")
            line = code.count("\n", 0, start) + 1
            yield Prototype(m["name"], text[start:stop + 1], line, "static" in ret)
            for extra in re.finditer(r",\s*\**\s*([A-Za-z_]\w*)\s*\(", code[end:stop + 1]):
                yield Prototype(extra[1], text[start:stop + 1], line, "static" in ret)
            continue
        if end >= len(code) or code[end] != ";":
            continue  # a definition, expression, or function-pointer variable
        start = m.start("ret")
        yield Prototype(m["name"], text[start:end + 1],
                        code.count("\n", 0, start) + 1, "static" in ret)
    for m in re.finditer(
        r"(?:^|(?<=[;{}]))[ \t\r\n]*(?P<decl>(?:(?P<storage>static|extern)\s+)?"
        r"USER_FUNC\s*\(\s*(?P<name>\w+)\s*\)\s*;)", code, re.M
    ):
        start = m.start("decl")
        yield Prototype(m["name"], text[start:m.end()],
                        code.count("\n", 0, start) + 1, m["storage"] == "static")


def declaration_key(text):
    return re.sub(r"\s+", "", mask_noncode(text))


def asm_path(path, root=ROOT):
    """The original's asm for a source file: src/x/y.c -> build/G8MJ01/asm/x/y.s,
    rels/r/src/x/y.c -> build/G8MJ01/r/asm/x/y.s."""
    parts = Path(path).with_suffix(".s").parts
    if parts[0] == "src":
        return root / "build" / "G8MJ01" / "asm" / Path(*parts[1:])
    if parts[1] == "common":
        return shared_asm_path(Path(*parts[3:]).with_suffix(".c").as_posix(), root)
    return root / "build" / "G8MJ01" / parts[1] / "asm" / Path(*parts[3:])


def shared_asm_path(rel, root=ROOT):
    """A shared unit (rels/common/src/REL) has no asm of its own: use the first module configure.py builds it for."""
    for m in re.finditer(r'Object\(\w+, "([^"]+)", modules=\["(\w+)"\](?:, source="([^"]+)")?', (root / "configure.py").read_text()):
        if (m.group(3) or m.group(1)) == rel:
            return root / "build" / "G8MJ01" / m.group(2) / "asm" / Path(m.group(1)).with_suffix(".s")
    return root / "build" / "G8MJ01" / "common" / "asm" / Path(rel).with_suffix(".s")


def asm_scopes(path, root=ROOT):
    """{name: "local"/"global"/"weak"} for the functions in this unit's asm, or None if it isn't there (not built)."""
    a = asm_path(path, root)
    if not a.exists():
        return None
    return dict(re.findall(r"^\.fn (\w+), (\w+)", a.read_text(errors="ignore"), re.M))


def definitions(text):
    code = mask_noncode(text)
    names = set(re.findall(r"\b([A-Za-z_]\w*)\s*\([^;{}()]*(?:\([^;{}()]*\)[^;{}()]*)*\)\s*\{", code))
    return names | set(re.findall(r"\bUSER_FUNC\(\s*(\w+)\s*\)\s*\{", code))


def check_sources(sources, exceptions, scopes_of=asm_scopes):
    """sources maps repository-relative paths to C text (also usable in tests); scopes_of(path) gives the
    original's function scopes in that unit, or None if unknown."""
    problems, allowed, seen, anywhere = [], {}, Counter(), set()
    for entry in exceptions:
        if entry["path"] == "*":
            if not entry.get("reason", "").strip():
                problems.append(f"*: {entry['name']}: exception needs a reason")
            anywhere.add(entry["name"])
            continue
        key = (entry["path"], entry["name"], declaration_key(entry["declaration"]))
        if not entry.get("reason", "").strip():
            problems.append(f"{entry['path']}: {entry['name']}: exception needs a reason")
        if key in allowed:
            problems.append(f"{entry['path']}: {entry['name']}: duplicate exception")
        allowed[key] = entry
    for path, text in sorted(sources.items()):
        defined, scopes = definitions(text), scopes_of(path)
        for proto in prototypes(text):
            if proto.static:
                scope = None if scopes is None else scopes.get(proto.name)
                if scope == "local" or (scope is None and proto.name in defined):
                    continue
                if scopes is None:
                    problems.append(f"{path}:{proto.line}: {proto.name}: static declaration can't be checked "
                                    f"(no asm at {asm_path(path)}; build first)")
                elif scope is None:
                    problems.append(f"{path}:{proto.line}: {proto.name}: declared static, but this unit neither "
                                    f"defines it nor has it in the original; move the prototype to its owning header")
                else:
                    problems.append(f"{path}:{proto.line}: {proto.name}: declared static, but the original has it "
                                    f"{scope}; drop static and put the prototype in the unit's header")
                continue
            if proto.name in anywhere:
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
