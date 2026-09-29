#!/usr/bin/env python3
"""Try to link a REL unit: compare its object with the original, then mark it Matching and check the REL's checksum.

    python3 tools/rel_link.py MOD/UNIT     e.g. gor/gor_12 or aji/unit/unit_barriern
Prints LINKED (and leaves the unit Matching in configure.py) or NOT LINKED with what differs (and restores it)."""
import re, subprocess, sys

mod, unit = sys.argv[1].split("/", 1)
name = f'"{unit}.c"'


def set_state(state):
    s = open("configure.py").read()
    s2 = re.sub(rf"Object\((?:Matching|NonMatching), {re.escape(name)}\)", f"Object({state}, {name})", s)
    if s2 == s and f"Object({state}, {name})" not in s:
        sys.exit(f"{name} is not in configure.py")
    open("configure.py", "w").write(s2)
    subprocess.run(["python3", "configure.py"], capture_output=True, check=True)


set_state("NonMatching")
b = subprocess.run(["ninja", f"build/G8MJ01/src/{unit}.o"], capture_output=True, text=True)
if b.returncode:
    print("NOT LINKED: the file doesn't build\n" + "\n".join(l for l in (b.stdout + b.stderr).splitlines() if l.startswith("#"))[-2000:])
    sys.exit(1)
c = subprocess.run(["python3", "tools/rel_cmp.py", mod, unit], capture_output=True, text=True)
diffs = [l for l in c.stdout.strip().splitlines() if l != "OK"]
# a section a few bytes shorter than the original's is its trailing alignment padding, which the linker adds back
real = [l for l in diffs if not re.match(r"\.(data|rodata|bss) size 0x([0-9a-f]+) \(original\) vs 0x([0-9a-f]+)$", l)
        or not 0 < int(re.search(r"size 0x([0-9a-f]+)", l).group(1), 16) - int(l.rsplit("0x", 1)[1], 16) < 8]
if real:
    print("NOT LINKED: the object differs from the original:\n  " + "\n  ".join(real))
    sys.exit(1)
set_state("Matching")
out = subprocess.run(["ninja"], capture_output=True, text=True)
if f"build/G8MJ01/{mod}/{mod}.rel: FAILED" in out.stdout or out.returncode:
    set_state("NonMatching")
    subprocess.run(["ninja"], capture_output=True)
    print(f"NOT LINKED: the object compares equal here, but {mod}.rel's checksum fails with it linked "
          "(look for a difference the compare doesn't cover: function code, symbol order, alignment)")
    sys.exit(1)
print(f"LINKED: {mod}.rel matches its checksum with {unit}.c linked")
