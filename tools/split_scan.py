#!/usr/bin/env python3
"""List dol units whose .data split looks 8 bytes late: the unit's last .data object is an 8-byte unnamed `lbl_XXXXXXXX`
holding a float pair ({-1.0f, 1.0f} or {1.0f, -1.0f}) that some other unit's code uses as a base register.

    python3 tools/split_scan.py

Those pairs are the compiler's per-file constant pair (`negone_one`, `one_negone`); when the original code addresses its own
.data through it (`lis/addi lbl_XXXXXXXX@ha/@l`, then a large offset), the pair really is the FIRST object of the next unit's
.data and the boundary in config/G8MJ01/splits.txt should move 8 bytes down. Prints unit, label, address, pair, the units that
reference the label, and the unit that starts at the label's end."""
import glob, os, re, sys

PAIRS = {("0xBF800000", "0x3F800000"): "{-1.0f, 1.0f}", ("0x3F800000", "0xBF800000"): "{1.0f, -1.0f}"}
root = "build/G8MJ01/asm"
units = {}
for path in glob.glob(f"{root}/**/*.s", recursive=True):
    text = open(path, errors="replace").read()
    units[os.path.relpath(path, root)[:-2]] = text
refs = {}
for u, t in units.items():
    for m in set(re.findall(r"\b(lbl_[0-9A-F]{8})@ha", t)):
        refs.setdefault(m, set()).add(u)
# unit order by .data start address, from the splits
splits = open("config/G8MJ01/splits.txt").read()
starts = {}
for m in re.finditer(r"^(\S+\.c):\n((?:\t.*\n)+)", splits, re.M):
    d = re.search(r"\.data\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)", m.group(2))
    if d:
        starts[m.group(1)[:-2]] = (int(d.group(1), 16), int(d.group(2), 16))
by_start = {s: u for u, (s, e) in starts.items()}
found = []
for u, t in units.items():
    sec = t.split("\n.section .data", 1)
    data = re.split(r"\n\.section ", t)
    body = next((b for b in data if b.startswith(".data") or b.startswith("\n.data")), None)
    if body is None:
        m = re.search(r"\n\.data\n(.*?)(?=\n\.section |\Z)", t, re.S)
        body = m.group(1) if m else None
    if body is None:
        continue
    objs = re.findall(r"# \.data:0x[0-9A-F]+ \| 0x([0-9A-F]+) \| size: 0x([0-9A-F]+)\n\.obj (\w+), \w+\n(.*?)\.endobj", body, re.S)
    if not objs:
        continue
    addr, size, name, content = objs[-1]
    if size != "8" or not name.startswith("lbl_"):
        continue
    words = tuple(re.findall(r"\.4byte (0x[0-9A-Fa-f]+)", content))
    words = tuple("0x" + w[2:].upper().rjust(8, "0") for w in words)
    if words not in PAIRS:
        continue
    users = sorted(refs.get(name, ()))
    nxt = by_start.get(int(addr, 16) + 8, "?")
    found.append((u, name, addr, PAIRS[words], users, nxt))
print(f"{len(found)} units end their .data with an 8-byte float-pair label")
for u, name, addr, pair, users, nxt in sorted(found):
    flag = "USED AS BASE" if any(x == nxt for x in users) else ("used elsewhere" if users else "unreferenced")
    print(f"{u:40s} {name} 0x{addr} {pair:14s} next unit: {nxt:34s} {flag}: {', '.join(users) or '-'}")
