#!/usr/bin/env python3
"""Compare the bytes of the data objects a change defines against the original object's bytes.

objdiff scores a data relocation by the symbol it names, not by the bytes of the object it points at, so a table
written `{4, 0, 0}` where retail has `{0, 4, 0}` scores the same as the right one. This reads the unit's original
object (objdiff.json's target_path) and our object (base_path) and compares, by symbol name, every data object
that the diff defines or touches: `.data`, `.sdata`, `.rodata`, `.sdata2`. Words that carry a relocation are masked
(their symbol names differ legitimately).

    python3 tools/data_check.py [--base REF] [NAME ...]     NAME: an object to excuse

Used by tools/ci.py (after the build, before the ratchet): `--allow-data-mismatch NAME` excuses an object.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA_SECTIONS = (".data", ".sdata", ".rodata", ".sdata2")


def _strip(text):
    """Blank out comments and string/char literals (keeping the newlines) so brace matching is safe."""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(re.sub(r"[^\n]", " ", text[i:j]))
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            out.append(c + re.sub(r"[^\n]", " ", text[i + 1:j - 1]) + c)
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


def data_definitions(text):
    """Top-level data definitions with an initialiser: [(first_line, last_line, name)], lines 1-based.

    Statements end at a `;` at depth 0, or at the `}` of a function body. Typedefs, `extern` declarations and
    function definitions are skipped; the name is the declarator before the first top-level `=`."""
    s = _strip(text)
    defs = []
    # event scripts: EVT_BEGIN(name) ... EVT_END() defines the s32 array `name`
    for m in re.finditer(r"(?:\b(?:static|const)\s+)*\bEVT_BEGIN\s*\(\s*(\w+)\s*\)(.*?)\bEVT_END\s*\(\s*\)", s, re.S):
        first = s.count("\n", 0, m.start()) + 1
        defs.append((first, first + s.count("\n", m.start(), m.end()), m.group(1)))
        s = s[:m.start()] + re.sub(r"[^\n]", " ", s[m.start():m.end()]) + s[m.end():]
    depth, line, i, n = 0, 1, 0, len(s)
    first = None  # (index, line) of the current statement's first token
    while i < n:
        c = s[i]
        if c == "\n":
            line += 1
        elif c == "#" and first is None and (i == 0 or s[i - 1] == "\n"):
            while i < n and (s[i] != "\n" or s[i - 1] == "\\"):
                line += s[i] == "\n"
                i += 1
            continue
        elif first is None and not c.isspace():
            first = (i, line)
        if c in "{([":
            depth += 1
        elif c in "})]":
            depth -= 1
        if first is not None and depth == 0 and (c == ";" or (c == "}" and _is_function_head(s[first[0]:i]))):
            if c == ";":
                _add_definition(defs, s[first[0]:i], first[1], line)
            first = None
        i += 1
    return sorted(defs)


def _is_function_head(head):
    """True if `head` (a statement up to a closing brace) is a function definition: `... ) {` before any `=`."""
    brace = head.find("{")
    return brace >= 0 and "=" not in head[:brace] and head[:brace].rstrip().endswith(")")


def _add_definition(defs, stmt, first, last):
    if re.match(r"\s*(typedef|extern)\b", stmt):
        return
    m = re.search(r"(?<![=!<>+\-*/&|^])=(?!=)", stmt)
    if not m:
        return
    head = re.sub(r"__declspec\s*\([^)]*\)", "", stmt[:m.start()])
    if "(" in re.sub(r"ATTRIBUTE_ALIGN\s*\(\s*\d+\s*\)|\(\s*\*\s*\w+\s*\)", "", head):
        return  # a function-like declarator, not data
    head = re.sub(r"ATTRIBUTE_ALIGN\s*\(\s*\d+\s*\)", "", head)
    head = re.sub(r"(\[[^\]]*\]\s*)+$", "", head.strip())
    name = re.search(r"([A-Za-z_]\w*)\s*$", head)
    if name:
        defs.append((first, last, name.group(1)))


def changed_lines(base, path):
    """New-file line numbers touched by the diff of `path` against base (all lines for a file git doesn't track)."""
    r = subprocess.run(["git", "diff", "-U0", base, "--", path], cwd=ROOT, text=True, capture_output=True).stdout
    if not r.strip():
        tracked = subprocess.run(["git", "ls-files", "--error-unmatch", path], cwd=ROOT, capture_output=True).returncode == 0
        if tracked:
            return set()
        return set(range(1, len((ROOT / path).read_text(errors="replace").splitlines()) + 2))
    lines = set()
    for m in re.finditer(r"^@@ -\S+ \+(\d+)(?:,(\d+))? @@", r, re.M):
        a, b = int(m.group(1)), int(m.group(2) if m.group(2) is not None else 1)
        lines.update(range(a, a + max(b, 1)))
    return lines


def touched_objects(text, lines):
    """Names of the data objects whose definition overlaps the changed lines."""
    return [name for first, last, name in data_definitions(text) if any(first <= x <= last for x in lines)]


def read_objects(path):
    """{name: (section, bytes, masked offsets, {reloc offset: type})} for the named data objects of an ELF object."""
    from elftools.elf.elffile import ELFFile
    elf = ELFFile(open(path, "rb"))
    sections = list(elf.iter_sections())
    relocs = {}
    for sec in sections:
        if sec.header.sh_type == "SHT_RELA":
            target = sections[sec.header.sh_info].name
            for r in sec.iter_relocations():
                relocs.setdefault(target, {})[r["r_offset"]] = r["r_info_type"]
    out = {}
    symtab = elf.get_section_by_name(".symtab")
    for sym in symtab.iter_symbols():
        idx = sym["st_shndx"]
        if not isinstance(idx, int) or idx >= len(sections):
            continue
        sec = sections[idx]
        if sec.name not in DATA_SECTIONS or sym["st_size"] == 0 or not sym.name or sym.name.startswith("@"):
            continue
        off, size = sym["st_value"], sym["st_size"]
        data = sec.data()[off:off + size]
        rel = {o - off: t for o, t in relocs.get(sec.name, {}).items() if off <= o < off + size}
        out[sym.name] = (sec.name, data, rel)
    return out


def compare(ours, target, names):
    """Mismatch lines for the named objects present in both: the first differing byte outside a relocated word.

    A word that carries a relocation on either side is skipped (a pointer the original links to a symbol is often
    written here as the address it resolves to, and the checksum covers that once the unit is linked). Objects of
    different sizes are compared over their common length: a merged or split object is a naming choice."""
    problems = []
    for name in names:
        if name not in ours or name not in target:
            continue
        osec, odata, orel = ours[name]
        tsec, tdata, trel = target[name]
        short = min(len(odata), len(tdata))
        masked = {o + k for o in set(orel) | set(trel) for k in range(4)}
        for i in range(short):
            if odata[i] != tdata[i] and i not in masked:
                lo = i - i % 4
                problems.append(f"{name} [{tsec}+{i:#x}]: original {tdata[lo:lo + 16].hex(' ', 4)}, ours "
                                f"{odata[lo:lo + 16].hex(' ', 4)}")
                break
    return problems


def check(base, allowed=(), root=ROOT):
    """Problems for every changed .c file that maps to a unit with an original object. [] when all match."""
    units = {}
    for u in json.load(open(root / "objdiff.json"))["units"]:
        src = u.get("metadata", {}).get("source_path")
        if src and u.get("target_path") and u.get("base_path"):
            units[src] = u
    names = subprocess.run(["git", "diff", "--name-only", base], cwd=root, text=True, capture_output=True).stdout.split()
    names += subprocess.run(["git", "ls-files", "--others", "--exclude-standard"], cwd=root, text=True,
                            capture_output=True).stdout.split()
    problems, checked = [], 0
    for path in sorted(set(names)):
        unit = units.get(path)
        if not unit or not (root / path).exists():
            continue
        target, ours = root / unit["target_path"], root / unit["base_path"]
        if not target.exists() or not ours.exists():
            continue
        wanted = [n for n in touched_objects((root / path).read_text(errors="replace"), changed_lines(base, path))
                  if n not in allowed]
        if not wanted:
            continue
        found = compare(read_objects(ours), read_objects(target), wanted)
        checked += len(wanted)
        problems += [f"{path}: {p}" for p in found]
    return problems, checked


if __name__ == "__main__":
    args = sys.argv[1:]
    base = "HEAD"
    if args[:1] == ["--base"]:
        base, args = args[1], args[2:]
    probs, count = check(base, set(args))
    for p in probs:
        print(p)
    print(f"{count} data object(s) checked, {len(probs)} mismatch(es)")
    sys.exit(1 if probs else 0)
