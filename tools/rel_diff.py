#!/usr/bin/env python3
"""Where does a built REL first differ from the original?  python3 tools/rel_diff.py MOD
Compares section sizes (dtk rel info) and, per section, the first differing byte and relocation."""
import subprocess, sys
m = sys.argv[1]
def info(p):
    return subprocess.run(["build/tools/dtk", "rel", "info", "-r", p], capture_output=True, text=True).stdout.splitlines()
o, b = info(f"orig/G8MJ01/files/rel/{m}.rel"), info(f"build/G8MJ01/{m}/{m}.rel")
print("sections:")
for x, y in zip(o[:12], b[:12]):
    if x != y:
        print("  orig", x.strip()); print("  ours", y.strip())
a = open(f"orig/G8MJ01/files/rel/{m}.rel", "rb").read(); c = open(f"build/G8MJ01/{m}/{m}.rel", "rb").read()
print("sizes", len(a), len(c))
d = [i for i in range(min(len(a), len(c))) if a[i] != c[i]]
print("first differing bytes (file offsets):", [hex(x) for x in d[:6]], "count", len(d))
n = [(x, y) for x, y in zip(o, b) if x != y]
print("first differing info lines:", n[:6])
