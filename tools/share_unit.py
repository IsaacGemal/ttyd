#!/usr/bin/env python3
"""Move a rel_gen'd unit source into the shared `common` REL and register one Object per REL.

    python3 tools/share_unit.py unit/unit_x MOD [MOD ...] [-- MOD ...]     (groups after `--` are variants: v2, v3)

The source must be at rels/<first MOD>/src/<unit>.c. Its per-REL data table name becomes UNIT_DATA_SYM (each Object
passes -DUNIT_DATA_SYM=<that REL's name>), and its extend_data ids become <unit>_<n>: extend_data(id) is an inline function forcedata<id>
that the linker merges by name within a REL, so ids must be unique per REL.
New Objects are NonMatching; flip them with tools/rel_link.py MOD/unit/unit_x."""
import re, sys, os, shutil, glob

args = sys.argv[1:]
unit = args.pop(0)
groups, cur = [], []
for a in args:
    if a == "--":
        groups.append(cur)
        cur = []
    else:
        cur.append(a)
groups.append(cur)
name = os.path.basename(unit)
cfg = open("configure.py").read()
head, tail = cfg.split('        "common",', 1)
end = tail.index("        ],\n")  # end of the common object list
# Everything is read, resolved and checked first; files change only once the whole migration is known to work.
plan = []  # (src, dst, text, header move, configure lines)
for gi, mods in enumerate(groups):
    src = f"rels/{mods[0]}/src/{unit}.c"
    dst_rel = unit if len(groups) == 1 else f"v{gi + 1}/{unit}"
    dst = f"rels/common/src/{dst_rel}.c"
    if not os.path.exists(src):
        sys.exit(f"{src} doesn't exist")
    if os.path.exists(dst):
        sys.exit(f"{dst} already exists")
    s = open(src).read()
    m = re.search(rf"\b({name}_\d+_data_[0-9A-F]+)\b", s)
    if m:
        s = s.replace(m.group(1), "UNIT_DATA_SYM")
    else:
        s2 = re.sub(rf"static u32 {name}\[49\]", "u32 UNIT_DATA_SYM[49]", s)
        if s2 == s:
            sys.exit(f"{src}: no `{name}_<id>_data_<offset>` table name and no `static u32 {name}[49]` table to make global")
        s = re.sub(rf"&{name}\b", "&UNIT_DATA_SYM", s2)
    refs = sorted(set(re.findall(rf"\b(unit_(?!{name}_)\w+?)_\d+_data_[0-9A-F]+\b", s)))
    for r in refs:
        s = re.sub(rf"\b{r}_\d+_data_[0-9A-F]+\b", f"UNIT_REF_{r[5:].upper()}", s)
    k = 0

    def bump(_):
        global k
        k += 1
        return f"extend_data({name}_{k - 1},"

    s = re.sub(r"extend_data\(\w+,", bump, s)
    hsrc = f"rels/{mods[0]}/include/{name}.h"
    hdst = f"rels/common/include/{name}.h"
    hmove = None
    if os.path.exists(hsrc):
        # the common header, as it exists or as an earlier group of this run will write it, must say the same
        planned = next((open(h[0]).read() for _, _, _, h, _ in plan if h and h[1] == hdst), None)
        have = planned if planned is not None else (open(hdst).read() if os.path.exists(hdst) else None)
        if have is not None and have != open(hsrc).read():
            sys.exit(f"{hsrc} differs from the common {hdst} (or another group's header): merge them by hand first")
        hmove = (hsrc, hdst)
    lines = []
    for mod in mods:
        apath = f"build/G8MJ01/{mod}/asm/{unit}.s"
        if not os.path.exists(apath):
            sys.exit(f"{apath} doesn't exist (build {mod} first)")
        asm = open(apath).read()
        g = re.search(rf"\.obj ({name}_\d+_data_[0-9A-F]+), global", asm)
        sym = g.group(1) if g else f"{name}_local_data"
        srcopt = f', source="{dst_rel}.c"' if dst_rel != unit else ""
        flags = [f"-DUNIT_DATA_SYM={sym}"]
        for r in refs:
            rm = re.search(rf"\b{r}_\d+_data_[0-9A-F]+\b", asm)
            if not rm:
                sys.exit(f"{apath}: no {r}_<id>_data_<offset> symbol for UNIT_REF_{r[5:].upper()}")
            flags.append(f"-DUNIT_REF_{r[5:].upper()}=" + rm.group(0))
        flag_text = ", ".join(f'"{f}"' for f in flags)
        lines.append(f'            Object(NonMatching, "{unit}.c", modules=["{mod}"]{srcopt}, extra_cflags=[{flag_text}]),\n')
    plan.append((src, dst, s, hmove, lines))
for src, dst, s, hmove, lines in plan:
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w").write(s)
    tail = tail[:end] + "".join(lines) + tail[end:]
    end += sum(len(l) for l in lines)
open("configure.py", "w").write(head + '        "common",' + tail)
for src, dst, s, hmove, lines in plan:  # the originals go last, once the new files and configure.py are written
    os.remove(src)
    if hmove:
        if os.path.exists(hmove[1]):
            os.remove(hmove[0])
        else:
            shutil.move(*hmove)
print("shared", unit, groups)
