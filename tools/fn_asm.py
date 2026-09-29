#!/usr/bin/env python3
"""Print a unit's functions from its asm without the address columns.  python3 tools/fn_asm.py MOD UNIT [FUNC]"""
import re, sys
mod, unit = sys.argv[1:3]
s = open(f"build/G8MJ01/{mod}/asm/{unit}.s", errors="replace").read()
for m in re.finditer(r"^\.fn (\S+), (\w+)\n(.*?)^\.endfn", s, re.M | re.S):
    if len(sys.argv) > 3 and m.group(1) != sys.argv[3]:
        continue
    print(f"== {m.group(1)} ({m.group(2)})")
    print(re.sub(r"^/\* [0-9A-F ]+ \*/\t", "\t", m.group(3), flags=re.M))
