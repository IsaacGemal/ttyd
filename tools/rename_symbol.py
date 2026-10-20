#!/usr/bin/env python3
"""Rename a dol symbol everywhere it is spelled: sources, headers, REL sources and config/G8MJ01/symbols.txt.

    python3 tools/rename_symbol.py OLD NEW [OLD NEW ...] [--rel NAME] [--dry-run]

Symbol names never reach the dol, but three things read them, so a rename has to be made in all of them at once:
  - the linker: every C file that names the symbol (dol units and REL units alike) must use the same name;
  - objdiff: the report pairs a function with the original by its config/G8MJ01/symbols.txt name (renamed only in the
    source, the function keeps its old name in report.json and the unit's .text section score drops);
  - the dol's FORCEACTIVE list, which dtk writes from symbols.txt for the dol symbols that RELs reference.
Data objects score by their bytes, so for them the symbols.txt edit only keeps the names in step.

A REL's own symbol (defined in config/G8MJ01/<rel>/symbols.txt) is renamed in that map and in the files that can see
it: the REL's sources and headers under rels/<rel>/, plus the headers those reach through #include (searched in the
including file's directory, include/, rels/common/include and rels/<rel>/{include,src}). Such a rename refuses when a
file outside that set spells OLD. When OLD is defined in more than one REL map the rename is refused as ambiguous
unless `--rel NAME` picks the map. A dol symbol that any REL map also spells is still refused: the REL entry may be a
different object, so rename it by hand.

Refuses when NEW is already spelled anywhere in those places or in a REL's symbols.txt (a clash would break the link
or pair the wrong symbol), when two pairs share a NEW, when OLD is defined both in the dol map and a REL map, when
OLD is ambiguous or not found. Rebuild afterwards (`python3 configure.py && ninja`: the checksum must hold) and run
tools/ci.py with --allow-protected (symbols.txt); its ratchet reads a function renamed in place as a rename, not a
regression.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "G8MJ01" / "symbols.txt"
EXTS = {".c", ".h", ".cpp", ".hpp", ".inc", ".s"}
INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)


def files():
    for top in ("src", "include", "rels", "libs"):
        for p in (ROOT / top).rglob("*"):
            if p.suffix in EXTS and p.is_file():
                yield p
    yield SYMBOLS


def word(name):
    """NAME as a whole symbol: `foo` doesn't match `foo_bar` or the static `foo$123`."""
    return re.compile(r"(?<![\w$])" + re.escape(name) + r"(?![\w$])")


def defines(name, text):
    return re.search(r"^" + re.escape(name) + r"\s*=", text, re.M) is not None


def visible(rel, texts):
    """The REL's sources and the headers they reach through #include, as keys of texts."""
    base = ROOT / "rels" / rel
    search = [ROOT / "include", ROOT / "rels" / "common" / "include", base / "include", base / "src"]
    seen = set()
    todo = [p for p in texts if p.is_relative_to(base)]
    while todo:
        p = todo.pop()
        if p in seen:
            continue
        seen.add(p)
        for inc in INCLUDE.findall(texts[p]):
            for d in [p.parent, *search]:
                cand = d / inc
                if cand in texts and cand.is_file():
                    todo.append(cand)
                    break
    return seen


def main():
    argv = sys.argv[1:]
    dry = "--dry-run" in argv
    args, rel_pick = [], None
    i = 0
    while i < len(argv):
        a = argv[i]
        if a == "--dry-run":
            pass
        elif a == "--rel":
            if i + 1 >= len(argv):
                sys.exit("--rel needs a REL name")
            rel_pick = argv[i + 1]
            i += 1
        elif a.startswith("--"):
            sys.exit(f"unknown option {a}\n{__doc__}")
        else:
            args.append(a)
        i += 1
    if not args or len(args) % 2:
        sys.exit(__doc__)
    pairs = list(zip(args[::2], args[1::2]))
    texts = {p: p.read_text(errors="surrogateescape") for p in files()}
    rel_maps = {p: p.read_text(errors="surrogateescape") for p in SYMBOLS.parent.glob("*/symbols.txt")}
    texts.update(rel_maps)
    news = [new for _, new in pairs]
    if len(set(news)) != len(news):
        sys.exit("the same NEW name is given twice")
    plan = []
    for old, new in pairs:
        if not re.fullmatch(r"[A-Za-z_]\w*", new):
            sys.exit(f"{new}: not an identifier")
        clash = [p for p, t in texts.items() if word(new).search(t)]
        if clash:
            sys.exit(f"{new} is already used in {', '.join(str(p.relative_to(ROOT)) for p in clash[:5])}")
        in_dol = defines(old, texts[SYMBOLS])
        owners = [p for p in rel_maps if defines(old, rel_maps[p])]
        if in_dol and owners:
            sys.exit(f"{old} is defined in the dol map and in {owners[0].relative_to(ROOT)}: rename it by hand")
        if owners:
            if rel_pick is not None:
                owners = [p for p in owners if p.parent.name == rel_pick]
                if not owners:
                    sys.exit(f"{old} is not defined in config/G8MJ01/{rel_pick}/symbols.txt")
            if len(owners) > 1:
                names = ", ".join(p.parent.name for p in owners)
                sys.exit(f"{old} is ambiguous (REL maps: {names}): pick one with --rel NAME")
            owner = owners[0]
            scope = visible(owner.parent.name, texts) | {owner}
            outside = [p for p in texts if p not in scope and word(old).search(texts[p])]
            if outside:
                sys.exit(f"{old} is spelled outside rels/{owner.parent.name} "
                         f"({', '.join(str(p.relative_to(ROOT)) for p in outside[:5])}): rename it by hand")
            plan.append((old, new, scope))
            print(f"{old}: REL symbol of {owner.parent.name}")
            continue
        if in_dol:
            print(f"{old}: dol symbol")
        rel = [p for p in rel_maps if word(old).search(rel_maps[p])]
        if rel:
            sys.exit(f"{old} is also in a REL map ({rel[0].relative_to(ROOT)}): rename it by hand")
        plan.append((old, new, None))
    changed = {}
    for old, new, scope in plan:
        rx = word(old)
        hits = 0
        for p in (texts if scope is None else scope):
            t = texts[p]
            t2, n = rx.subn(new, t)
            if n:
                texts[p] = t2
                changed[p] = t2
                hits += n
                print(f"  {p.relative_to(ROOT)}: {n}")
        if not hits:
            sys.exit(f"{old}: not found")
        print(f"{old} -> {new}: {hits} occurrence(s)")
    if not dry:
        for p, t in changed.items():
            p.write_text(t, errors="surrogateescape")


if __name__ == "__main__":
    main()
