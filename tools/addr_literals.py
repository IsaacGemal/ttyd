#!/usr/bin/env python3
"""Turn a unit's address-named constants (`float_0p5_80420df8`, `str_80301540`, `vec3_802fb2e8`) back into literals
where that changes no byte.

    uv run --with pyelftools python3 tools/addr_literals.py SRC.c [SRC.c ...] [--apply] [-j N] [--kinds K,K]
    uv run --with pyelftools python3 tools/addr_literals.py --all [--apply] [-j N] [--kinds K,K]   src/ and rels/*/src

Kinds: `float` (f32/f64 objects), `str` (const char arrays), `agg` (const Vec/GXColor objects, as compound literals); all three
by default. Candidates are the objects a .c defines itself that no other source or header names, that the code
never takes the address or sizeof of, that aren't volatile and that no `#ifdef MUST_MATCH`/`#ifndef MUST_MATCH`
conditional (either branch) names: match-only code is left alone.

Named objects and literals don't pool in the same place (named ones first, in definition order; literals in first-use
order, in text order), and a named constant is sometimes loaded or scheduled differently from a literal, so each
change is checked by recompiling: the unit is compiled with its own ninja command into a temporary directory and
compared with the current object by tools/obj_equiv.py (same section bytes, same relocation targets by place). The
search tries every candidate at once, then one at a time from the highest address down, each on top of those kept.
String literals lose the alignment NULs the named arrays spelled out ("abc\\000") where the object stays the same.
A comment block that only headed removed declarations goes too. The reference objects are brought up to date with
ninja first, so run it on a tree that builds.

Without --apply it only reports. With --apply the sources get their kept literals; then rebuild (the linked checksum is
the final check) and run tools/ci.py. Prints, per unit, how many objects became literals and how the count of
address-named references changed.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import obj_equiv  # noqa: E402

ADDR = re.compile(r"\b\w+_8[0-9A-Fa-f]{7}\b")
SECTION = r"(?:__declspec\(section \"\.(?:sdata2|rodata)\"\) )?"
FLOAT_DEF = re.compile(r"^" + SECTION + r"(static )?(?:const )?(f32|f64|float|double) ((?:float|double)_\w*?_(8[0-9A-Fa-f]{7}))"
                       r" = ([^;\n]+);", re.M)
STR_LIT = r"\"(?:[^\"\\\n]|\\.)*\""
STR_DEF = re.compile(r"^" + SECTION + r"(static )?const char (str_\w*?_?(8[0-9A-Fa-f]{7}))\[[^\]\n]*\] = (" + STR_LIT + r");", re.M)
PAD = re.compile(r"(?:\\000|\\0)+\"$")
AGG_DEF = re.compile(r"^" + SECTION.replace("rodata", "rodata|sdata") + r"(static )?(?:const )?(Vec|GXColor) ((?:vec3|dat)_\w*?_?(8[0-9A-Fa-f]{7}))"
                     r" = (\{[^;{}\n]*\});", re.M)
NUM = re.compile(r"^-?(\d+\.?\d*|\.\d+)([eE][-+]?\d+)?[fF]?$")


def all_texts():
    out = {}
    for pat in ("src/**/*.[ch]", "include/**/*.h", "rels/*/src/**/*.[ch]", "rels/*/include/**/*.h", "libs/**/*.[ch]"):
        for p in ROOT.glob(pat):
            out[p.relative_to(ROOT).as_posix()] = p.read_text(errors="replace")
    return out


def float_literal(kind, init):
    init = init.strip()
    if not NUM.match(init):
        return None
    body = init.rstrip("fF")
    if "." not in body and "e" not in body.lower():
        body += ".0"
    return body + ("f" if kind in ("f32", "float") else "")


def must_match_regions(text):
    """The text of every `#if...MUST_MATCH` conditional (all its branches, nested ones included)."""
    out, depth, inside = [], [], 0
    for line in text.splitlines(keepends=True):
        s = line.strip()
        if re.match(r"#\s*if", s):
            depth.append("MUST_MATCH" in s)
            inside += depth[-1]
        if inside:
            out.append(line)
        if re.match(r"#\s*endif", s) and depth:
            inside -= depth.pop()
    return "".join(out)


def candidates(path, text, refs, kinds):
    """[(addr, name, literal)] in address order: the constants of KINDS ('float', 'str') this file alone names."""
    found = []
    if "float" in kinds:
        found += [(m.group(4), m.group(3), float_literal(m.group(2), m.group(5))) for m in FLOAT_DEF.finditer(text)]
    if "str" in kinds:
        found += [(m.group(3), m.group(2), m.group(4)) for m in STR_DEF.finditer(text)]
    if "agg" in kinds:
        found += [(m.group(4), m.group(3), f"({m.group(2)}){m.group(5)}") for m in AGG_DEF.finditer(text)]
    out = []
    match_only = must_match_regions(text)
    for addr, name, lit in found:
        if re.search(r"\b" + name + r"\b", match_only):
            continue  # match-only code is left as it is
        if lit is None or refs.get(name, {path}) != {path} or len(re.findall(r"\b" + name + r"\b", text)) < 2:
            continue
        if re.search(r"(&|sizeof)\s*\(?\s*" + name + r"\b", text) or re.search(r"volatile[^;\n]*\b" + name + r"\b", text):
            continue
        out.append((int(addr, 16), name, lit))
    return sorted(set(out))


GONE = "\x00gone\n"


def literalise(text, consts):
    """TEXT with each constant's declarations removed and its uses replaced by its literal. A comment block that only
    headed removed declarations goes with them."""
    for _, name, lit in consts:
        text = re.sub(r"^(?:extern )?" + SECTION + r"(?:static )?(?:const )?(?:f32|f64|float|double|char|Vec|GXColor) " + name
                      + r"(?:\[[^\]\n]*\])?(?: = (?:" + STR_LIT + r"|\{[^;{}\n]*\}|[^;\n]+))?;[ \t]*(?://[^\n]*)?\n", GONE, text, flags=re.M)
        current = text

        def repl(m, lit=lit, current=current):
            if lit.startswith("-") and current[:m.start()].rstrip().endswith(("-", "+")):
                return f"({lit})"
            return lit
        text = re.sub(r"\b" + name + r"\b", repl, text)
    lines = text.splitlines(keepends=True)
    drop = set()
    i = 0
    while i < len(lines):
        if lines[i] != GONE:
            i += 1
            continue
        j = i
        while j < len(lines) and lines[j] in (GONE, "\n"):
            j += 1
        after_blank = j == len(lines) or lines[j - 1] == "\n" or lines[j].strip().startswith(("//", "#"))
        k = i
        while k > 0 and lines[k - 1].lstrip().startswith("//") and not lines[k - 1].startswith((" ", "\t")):
            k -= 1
        if k < i and after_blank and (k == 0 or not lines[k - 1].strip()):
            drop.update(range(k, i))
        i = j
    out, removed = [], False
    for n, line in enumerate(lines):
        if n in drop or line == GONE:
            removed = True
        elif line == "\n" and removed and (not out or out[-1] == "\n"):
            continue  # a blank line the removal left doubled (or at the top)
        else:
            out.append(line)
            removed = removed and line == "\n"
    return "".join(out)


class Unit:
    @staticmethod
    def object_path(src):
        return ROOT / "build/G8MJ01/src" / (re.sub(r"^(src|rels/\w+/src)/", "", src)[:-2] + ".o")

    def __init__(self, src):
        self.src = src
        self.obj = self.object_path(src)
        cmds = subprocess.run(["ninja", "-t", "commands", str(self.obj.relative_to(ROOT))], cwd=ROOT,
                              capture_output=True, text=True).stdout.strip().splitlines()
        self.cmd = cmds[-1].split(" && ")[0] if cmds else None
        if self.cmd:
            self.cmd = self.cmd.replace(" -MMD", "")

    def compile(self, text, tmp):
        """Path of the object compiled from TEXT, or None if it doesn't compile."""
        p = ROOT / self.src
        variant = p.with_name(".qa_" + p.name)
        variant.write_text(text)
        try:
            cmd = self.cmd.replace(" -c " + self.src, " -c " + variant.relative_to(ROOT).as_posix())
            cmd = re.sub(r" -o \S+", " -o " + tmp, cmd)
            r = subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)
            out = Path(tmp) / (".qa_" + p.stem + ".o")
            return out if r.returncode == 0 and out.exists() else None
        finally:
            variant.unlink()


def search(src, texts, refs, kinds):
    text = texts[src]
    cands = candidates(src, text, refs, kinds)
    if not cands:
        return src, text, 0, "no candidates"
    unit = Unit(src)
    if not unit.cmd or not unit.obj.exists():
        return src, text, 0, "no object/command"
    tmp = tempfile.mkdtemp(prefix="qa_")
    tries = 0

    def ok(subset):
        nonlocal tries
        tries += 1
        o = unit.compile(literalise(text, subset), tmp)
        return o is not None and not obj_equiv.compare(unit.obj, o)[0]

    try:
        if ok(cands):
            best = cands
        else:
            # one at a time from the highest address down, each on top of the ones already kept: named constants pool
            # ahead of literals, so the tail of the pool is the likeliest to go
            best = []
            for c in reversed(cands):
                if ok(best + [c]):
                    best.append(c)
        # strings defined with their alignment padding ("abc\\000"): literals are 4-aligned by the compiler, so the
        # padding is noise, except perhaps after the unit's last string, where it sets the section's size
        strings = sorted(c for c in best if c[2].startswith('"') and PAD.search(c[2]))
        if strings:
            def strip(cs):
                return [(a, n, PAD.sub('"', l)) if (a, n, l) in cs else (a, n, l) for a, n, l in best]
            for variant in (strip(strings), strip(strings[:-1])):
                if variant != best and ok(variant):
                    best = variant
                    break
        new = literalise(text, best) if best else text
        return src, new, len(best), f"{len(best)}/{len(cands)} literal ({tries} compiles)"
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("srcs", nargs="*")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--kinds", default="float,str,agg", help="comma list of float, str, agg (default all three)")
    ap.add_argument("-j", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    a = ap.parse_args()
    texts = all_texts()
    refs = {}
    for path, t in texts.items():
        for name in set(ADDR.findall(t)):
            refs.setdefault(name, set()).add(path)
    srcs = [s for s in texts if s.endswith(".c") and not s.startswith("libs/")] if a.all else a.srcs
    kinds = set(a.kinds.split(","))
    srcs = [s for s in srcs if candidates(s, texts.get(s, ""), refs, kinds)]
    objs = [str(Unit.object_path(s).relative_to(ROOT)) for s in srcs]
    if objs and subprocess.run(["ninja"] + objs, cwd=ROOT, capture_output=True).returncode:
        sys.exit("ninja failed to bring the reference objects up to date")
    total_before = total_after = 0
    with ThreadPoolExecutor(a.j) as ex:
        for src, new, n, msg in ex.map(lambda s: search(s, texts, refs, kinds), srcs):
            before, after = len(ADDR.findall(texts[src])), len(ADDR.findall(new))
            total_before += before
            total_after += after
            print(f"{src}: {msg}; address names {before} -> {after}", flush=True)
            if a.apply and n:
                (ROOT / src).write_text(new)
    print(f"total address names in these files: {total_before} -> {total_after}")


if __name__ == "__main__":
    main()
