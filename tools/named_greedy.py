#!/usr/bin/env python3
"""Greedy search over one function's float constants: literal <-> the unit's named .sdata2 object.

    python3 tools/named_greedy.py UNIT FUNC [--to-literal]     e.g. window/win_root winBookGX

MWCC treats a float literal and a named `const f32` differently when it looks for common subexpressions. With literals
it canonicalises `x + 16.0f` and `16.0f + x` to one expression and shares it, often in a saved FPR. A named object
(`x + float_16_8041d0e4`) is left as written. So:
  - default (literal -> named): where retail keeps expressions apart that ours shares (more fadds and saved FPRs in
    the original), use the named constant. win_root's winBookGX went 89.35 -> 90.68 this way (#586).
  - --to-literal (named -> literal): where retail folds or shares what ours keeps apart, use the literal (#522).

Each candidate site in FUNC is tried alone and kept only when FUNC's score goes up, no other function of the unit
scores lower, and tools/sdata2_diff.py reports no category growing (no new DUP_OURS, EXTRA or MISSING). Whether the
constant is read by name or pooled as a literal changes the unit's .sdata2, and that gate catches it. Accepted
changes stay in the source; rejected ones are undone. The names and values come from the original object's .sdata2
symbols (float_*), so the tool works for dol units and RELs alike. Run from the repo root after `ninja`; it rebuilds
through tools/agent_check.py."""
import json
import os
import re
import struct
import subprocess
import sys

LIT = re.compile(r"(?<![\w.])(\d+\.\d*|\d*\.\d+)f\b")


def func_span(text, func):
    """(start, end) of FUNC's definition: from its header to the closing `}` at column 0."""
    f = re.escape(func)
    m = re.search(rf"^(?:[^\n;]*\b{f}\([^;{{]*\)|[^\n;]*\bUSER_FUNC\({f}\))\s*{{", text, re.M)
    if not m:
        raise ValueError(f"{func} not found")
    return m.start(), text.index("\n}\n", m.start())


def literal_sites(text, func, names):
    """[(start, end, name)] for each float literal in FUNC whose value has a named object in `names` (value -> name)."""
    a, b = func_span(text, func)
    return [(m.start(), m.end(), names[float(m.group(1))]) for m in LIT.finditer(text, a, b) if float(m.group(1)) in names]


def named_sites(text, func, values):
    """[(start, end, literal)] for each use in FUNC of a named object in `values` (name -> value)."""
    a, b = func_span(text, func)
    if not values:
        return []
    pat = re.compile(r"\b(" + "|".join(map(re.escape, values)) + r")\b")
    return [(m.start(), m.end(), literal(values[m.group(1)])) for m in pat.finditer(text, a, b)]


def literal(v):
    """C float literal for v: 16 -> `16.0f`, 0.5 -> `0.5f`."""
    s = repr(float(v))
    return (s if "." in s or "e" in s else s + ".0") + "f"


def apply(text, func, site):
    """`text` with the site replaced; a named object gets an extern declaration before FUNC if it has none."""
    start, end, new = site
    out = text[:start] + new + text[end:]
    if new.endswith("f") or re.search(rf"\b{re.escape(new)}\b[^;]*;", out[:func_span(out, func)[0]]):
        return out
    a = func_span(out, func)[0]
    return out[:a] + f"extern const f32 {new};\n" + out[a:]


def sdata2_names(target):
    """{value: name} and {name: value} for the float_* objects in the original object's .sdata2, or .rodata where a
    REL keeps its floats."""
    from elftools.elf.elffile import ELFFile
    f = ELFFile(open(target, "rb"))
    by_value, by_name = {}, {}
    for secname in (".sdata2", ".rodata"):
        sec = f.get_section_by_name(secname)
        if sec is None:
            continue
        data, idx = sec.data(), f.get_section_index(secname)
        for s in f.get_section_by_name(".symtab").iter_symbols():
            if s["st_shndx"] == idx and s.name.startswith("float_") and s["st_size"] == 4:
                v = struct.unpack(">f", data[s["st_value"]:s["st_value"] + 4])[0]
                by_value.setdefault(v, s.name)
                by_name[s.name] = v
    return by_value, by_name


def main(argv):
    unit, func = argv[0], argv[1]
    to_literal = "--to-literal" in argv
    # a dol unit is named MarioSt/UNIT in objdiff.json, a REL unit MOD/UNIT as given
    entry = next(e for e in json.load(open("objdiff.json"))["units"] if e["name"] in ("MarioSt/" + unit, unit))
    src = entry["metadata"]["source_path"]
    by_value, by_name = sdata2_names(entry["target_path"])

    def scores():
        out = subprocess.run([sys.executable, "tools/agent_check.py", unit], capture_output=True, text=True).stdout
        return {m.group(2): float(m.group(1)) for m in re.finditer(r"^\s+([\d.]+)%\s+\d+ B\s+(\w+)", out, re.M)}

    def data():
        out = subprocess.run([sys.executable, "tools/sdata2_diff.py", unit], capture_output=True, text=True).stdout
        counts, sec = {}, ""
        for line in out.splitlines():
            if line.startswith("=="):
                sec = line.split()[1]
            m = re.match(r"\s+([A-Z_]+) \((\d+)\):", line)
            if m:
                counts[(sec, m.group(1))] = int(m.group(2))
        return counts

    base, gate = scores(), data()
    print(f"{func}: {base.get(func, 0):.2f}", flush=True)
    k = 0
    while True:
        text = open(src).read()
        sites = named_sites(text, func, by_name) if to_literal else literal_sites(text, func, by_value)
        if k >= len(sites):
            break
        site = sites[k]
        new = apply(text, func, site)
        open(src, "w").write(new)
        sc = scores()
        better = sc.get(func, 0) > base.get(func, 0) and all(sc.get(n, 0) >= v - 1e-9 for n, v in base.items() if n != func)
        d = data() if better else {}
        if better and all(d.get(c, 0) <= n for c, n in gate.items()) and not set(d) - set(gate):
            base, gate = sc, d
            print(f"  kept {text[site[0]:site[1]]} -> {site[2]} (line {text[:site[0]].count(chr(10)) + 1}): "
                  f"{sc[func]:.2f}", flush=True)
        else:
            open(src, "w").write(text)
            k += 1
    print(f"{func}: {base.get(func, 0):.2f} done")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    main(sys.argv[1:])
