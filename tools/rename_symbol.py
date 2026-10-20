#!/usr/bin/env python3
"""Rename a dol symbol everywhere it is spelled: sources, headers, REL sources and config/G8MJ01/symbols.txt.

    python3 tools/rename_symbol.py OLD NEW [OLD NEW ...] [--dry-run]

Symbol names never reach the dol, but three things read them, so a rename has to be made in all of them at once:
  - the linker: every C file that names the symbol (dol units and REL units alike) must use the same name;
  - objdiff: the report pairs a function with the original by its config/G8MJ01/symbols.txt name (renamed only in the
    source, the function keeps its old name in report.json and the unit's .text section score drops);
  - the dol's FORCEACTIVE list, which dtk writes from symbols.txt for the dol symbols that RELs reference.
Data objects score by their bytes, so for them the symbols.txt edit only keeps the names in step.

Refuses when NEW is already spelled anywhere in those places or in a REL's symbols.txt (a clash would break the link
or pair the wrong symbol), when two pairs share a NEW, when OLD is also a REL symbol, or when OLD isn't found. Rebuild afterwards (`python3 configure.py && ninja`: the checksum must hold) and run tools/ci.py
with --allow-protected (symbols.txt); its ratchet reads a function renamed in place as a rename, not a regression.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = ROOT / "config" / "G8MJ01" / "symbols.txt"
EXTS = {".c", ".h", ".cpp", ".hpp", ".inc", ".s"}


def files():
    for top in ("src", "include", "rels", "libs"):
        for p in (ROOT / top).rglob("*"):
            if p.suffix in EXTS and p.is_file():
                yield p
    yield SYMBOLS


def word(name):
    """NAME as a whole symbol: `foo` doesn't match `foo_bar` or the static `foo$123`."""
    return re.compile(r"(?<![\w$])" + re.escape(name) + r"(?![\w$])")


def main():
    args = [a for a in sys.argv[1:] if a != "--dry-run"]
    dry = "--dry-run" in sys.argv
    if not args or len(args) % 2:
        sys.exit(__doc__)
    pairs = list(zip(args[::2], args[1::2]))
    texts = {p: p.read_text(errors="surrogateescape") for p in files()}
    rel_symbols = {p: p.read_text(errors="surrogateescape") for p in SYMBOLS.parent.glob("*/symbols.txt")}
    news = [new for _, new in pairs]
    if len(set(news)) != len(news):
        sys.exit("the same NEW name is given twice")
    for old, new in pairs:
        if not re.fullmatch(r"[A-Za-z_]\w*", new):
            sys.exit(f"{new}: not an identifier")
        clash = [p for p, t in (texts | rel_symbols).items() if word(new).search(t)]
        if clash:
            sys.exit(f"{new} is already used in {', '.join(str(p.relative_to(ROOT)) for p in clash[:5])}")
        rel = [p for p, t in rel_symbols.items() if word(old).search(t)]
        if rel:
            sys.exit(f"{old} is also a REL symbol ({rel[0].relative_to(ROOT)}): rename it by hand")
    changed = {}
    for old, new in pairs:
        rx = word(old)
        hits = 0
        for p, t in texts.items():
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
