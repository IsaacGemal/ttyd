#!/usr/bin/env python3
"""Rewrite raw offset accesses as named fields where a header already declares the field.

  python3 tools/name_offsets.py FILE VAR STRUCT [VAR STRUCT ...]

For each `*(T*)((u8*)VAR + 0xNN)` (also `char*`) in FILE, where STRUCT (found in include/) has a field at 0xNN
of the same type (or the same width with different signedness), the access becomes `VAR->field`. A `Vec` field also covers its .x/.y/.z at +0/+4/+8. Anything
else is left alone and counted. Field offsets come from the
`// 0xNN` comments in the header, so those comments must be right. Check the result with tools/ci.py.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SCALARS = {"f32", "f64", "s8", "u8", "s16", "u16", "s32", "u32", "BOOL", "Vec"}


def struct_fields(name):
    """offset -> (type, field) for STRUCT, from the `// 0xNN` comments in its definition."""
    for h in (ROOT / "include").rglob("*.h"):
        text = h.read_text(errors="ignore")
        m = re.search(r"typedef struct \w*\s*\{(.*?)\}\s*" + re.escape(name) + r"\s*;", text, re.S)
        if not m:
            continue
        fields = {}
        for line in m.group(1).splitlines():
            f = re.match(r"\s*(?:struct\s+)?(\w+)(\s*\*+)?\s+(\w+)\s*;\s*//\s*(0x[0-9A-Fa-f]+)", line)
            if not f:
                continue
            typ = f.group(1) + ("*" if f.group(2) else "")
            fields[int(f.group(4), 16)] = (typ, f.group(3))
        return fields, h
    sys.exit(f"struct {name} not found in include/")


def same_size(a, b):
    """Exact type, or the same width with different signedness (the rebuild shows whether that matters)."""
    strip = lambda t: {"s": "", "u": ""}.get(t[:1], t[:1]) + t[1:] if re.fullmatch(r"[su](8|16|32)", t) else t
    return a == b or strip(a) == strip(b)


def rewrite(text, var, fields):
    done = skipped = 0

    def sub(m):
        nonlocal done, skipped
        typ, off = m.group(1).replace(" ", ""), int(m.group(2), 16)
        if off in fields and same_size(fields[off][0], typ) and typ.rstrip("*") in SCALARS | {"void"}:
            done += 1
            return f"{var}->{fields[off][1]}"
        for base, (ftyp, fname) in fields.items():
            if ftyp == "Vec" and typ == "f32" and off - base in (0, 4, 8):
                done += 1
                return f"{var}->{fname}.{'xyz'[(off - base) // 4]}"
        skipped += 1
        return m.group(0)

    pat = r"\*\(\s*(\w+\s*\**)\s*\*\s*\)\s*\(\s*\(\s*(?:u8|char)\s*\*\s*\)\s*" + re.escape(var) + r"\s*\+\s*(0x[0-9A-Fa-f]+)\s*\)"
    text = re.sub(pat, sub, text)
    return text, done, skipped


def main():
    if len(sys.argv) < 4 or len(sys.argv) % 2:
        sys.exit(__doc__)
    path = Path(sys.argv[1])
    text = path.read_text()
    for var, struct in zip(sys.argv[2::2], sys.argv[3::2]):
        fields, header = struct_fields(struct)
        text, done, skipped = rewrite(text, var, fields)
        print(f"{var} ({struct}, {header.relative_to(ROOT)}): {done} named, {skipped} left (no field of that type)")
    path.write_text(text)


if __name__ == "__main__":
    main()
