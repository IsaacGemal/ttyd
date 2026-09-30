#!/usr/bin/env python3
"""objdump stand-in for decomp-permuter on this project: there is no powerpc objdump here, so this prints `dtk elf disasm`
output in the layout the permuter's PowerPC scorer parses.

    PERM_FUNC=effKemuri2Main python3 tools/perm_objdump.py file.o

Only the function named by PERM_FUNC is printed (the target object holds the whole unit; the candidate holds just that
function). Branch targets become offsets from the function start, data symbols (`@ha`/`@l`/`@sda21` operands) become the
anonymous `@0` so a differently named pool entry does not count as a difference, call targets keep their names. Flags
other than the object file are ignored."""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
DTK = os.environ.get("PERM_DTK", os.path.join(ROOT, "build/tools/dtk"))
obj = sys.argv[-1]
want = os.environ.get("PERM_FUNC")

with tempfile.TemporaryDirectory() as d:
    s = os.path.join(d, "o.s")
    r = subprocess.run([DTK, "elf", "disasm", obj, s], capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit(1)
    lines = open(s, errors="replace").read().split("\n")

funcs = {}  # name -> [(offset, bytes, text)], labels by offset
cur = None
labels = {}
for ln in lines:
    m = re.match(r"\.fn (\S+),", ln)
    if m:
        cur = m.group(1)
        funcs[cur] = []
        continue
    if ln.startswith(".endfn"):
        cur = None
        continue
    if cur is None:
        continue
    m = re.match(r"/\* ([0-9A-F]{8}) [0-9A-F]{8}\s+((?:[0-9A-F]{2} ?){4}) \*/\t(.*)$", ln)
    if m:
        funcs[cur].append((int(m.group(1), 16), m.group(2).strip(), m.group(3)))

names = [want] if want else list(funcs)
if want and want not in funcs:
    sys.stderr.write(f"function {want} not in {obj}: {list(funcs)[:6]}\n")
    sys.exit(1)

print(f"{obj}:     file format elf32-powerpc")
for n in names:
    insns = funcs[n]
    base = insns[0][0] if insns else 0
    for off, raw, text in insns:
        text = re.sub(r"\.L_([0-9A-Fa-f]+)", lambda m: "0x%x" % (int(m.group(1), 16) - base), text)
        text = re.sub(r'"?@?[\w.$]+"?@(?:ha|h|l|sda21)', lambda m: "@0" + re.search(r"@(?:ha|h|l|sda21)$", m.group(0)).group(0), text)
        text = text.replace('"', "")
        mn, _, args = text.partition(" ")
        args = args.replace(", ", ",")
        print("%6x:\t%s\t%s\t%s" % (off - base, raw, mn, args))
