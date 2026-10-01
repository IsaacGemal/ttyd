#!/usr/bin/env python3
"""Move the .data boundary of every unit listed by tools/split_scan.py 8 bytes down (the float pair becomes the first object of
the next unit's .data), editing config/G8MJ01/splits.txt in place. Prints the unit pairs it changed.

    python3 tools/split_pair_fix.py [--dry-run]

Run tools/split_scan.py first from an up-to-date build (it reads build/G8MJ01/asm). A unit whose end is not the label address + 8
or whose successor does not start there is skipped."""
import re
import subprocess
import sys

dry = "--dry-run" in sys.argv
scan = subprocess.run([sys.executable, "tools/split_scan.py"], capture_output=True, text=True).stdout.splitlines()[1:]
text = open("config/G8MJ01/splits.txt").read()
changed = []
for ln in scan:
    m = re.match(r"(\S+)\s+(lbl_[0-9A-F]{8}) 0x([0-9A-F]+) \{.*?\}\s+next unit: (\S+)\s+USED AS BASE", ln)
    if not m:
        continue
    x, lbl, addr, y = m.group(1), m.group(2), int(m.group(3), 16), m.group(4)
    # the .data line of x ends at addr+8 and y's starts there
    px = re.compile(rf"(^{re.escape(x)}\.c:\n(?:\t.*\n)*?\t\.data\s+start:0x[0-9A-F]+ end:)0x{addr + 8:08X}", re.M)
    py = re.compile(rf"(^{re.escape(y)}\.c:\n(?:\t.*\n)*?\t\.data\s+start:)0x{addr + 8:08X}( end:)", re.M)
    if not px.search(text) or not py.search(text):
        print(f"skip {x} -> {y}: boundary not at 0x{addr + 8:08X}")
        continue
    text = px.sub(rf"\g<1>0x{addr:08X}", text, count=1)
    text = py.sub(rf"\g<1>0x{addr:08X}\g<2>", text, count=1)
    changed.append((x, y, lbl, addr))
    print(f"{x} -> {y}: boundary 0x{addr + 8:08X} -> 0x{addr:08X} ({lbl})")
if not dry:
    open("config/G8MJ01/splits.txt", "w").write(text)
open("/tmp/split_pair_plan.txt", "w").write("\n".join(f"{x} {y} {lbl} {a:08X}" for x, y, lbl, a in changed) + "\n")
