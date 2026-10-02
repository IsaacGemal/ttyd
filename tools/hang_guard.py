#!/usr/bin/env python3
"""Refuse sources that make this mwcceppc loop forever.

    python3 tools/hang_guard.py [FILE ...]   (no FILE: every .c/.h under src/, rels/, libs/, include/)

Known trigger: `#pragma scheduling <cpu number>` (601, 801, ...). Waves 53 and 57 left compiles running for a day
on it. tools/agent_check.py and tools/ci.py both run this before building; exit 1 lists the offending lines.
"""
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
PATTERNS = [
    (re.compile(r"^\s*#\s*pragma\s+scheduling\s+\d", re.M),
     "`#pragma scheduling <cpu number>` makes the compiler hang forever; use `scheduling off` / `reset`"),
]


def problems(paths):
    out = []
    for p in paths:
        try:
            text = pathlib.Path(p).read_text(errors="replace")
        except OSError:
            continue
        for rx, why in PATTERNS:
            for m in rx.finditer(text):
                out.append(f"{p}:{text.count(chr(10), 0, m.start()) + 1}: {why}")
    return out


def all_headers():
    return [p for d in ("include", "rels", "libs", "src") for p in (ROOT / d).rglob("*.h")]


def all_sources():
    return [p for d in ("src", "rels", "libs", "include") for p in (ROOT / d).rglob("*") if p.suffix in (".c", ".h")]


if __name__ == "__main__":
    found = problems(sys.argv[1:] or all_sources())
    print("\n".join(found))
    sys.exit(1 if found else 0)
