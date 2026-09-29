#!/usr/bin/env python3
"""Bring an agent's finished unit (scratch/waveNN/<mod>_<unit>/final.c) into the tree as a NonMatching Object.

    python3 tools/adopt_agent.py SCRATCH_DIR [--keep-pragma]

DIR names are <mod>_<unit path with / -> _>; the unit is found in the module's splits.txt. Copies final.c to
rels/<mod>/src/<unit>.c (and final.h next to it in rels/<mod>/include/), applies include.patch (paths relative to the
repo root), drops a trailing `#pragma force_active off` (with -inline deferred it strips unreferenced static functions),
adds `Object(NonMatching, "<unit>.c")` to the module's Rel list and reconfigures. Then run tools/rel_link.py MOD/UNIT."""
import os, re, subprocess, sys

src = sys.argv[1].rstrip("/")
keep = "--keep-pragma" in sys.argv
name = os.path.basename(src)
mod = name.split("_", 1)[0]
rest = name.split("_", 1)[1]
splits = [l.strip()[:-1] for l in open(f"config/G8MJ01/{mod}/splits.txt") if re.match(r"^\S+\.c:", l)]
unit = next((u[:-2] for u in splits if u[:-2].replace("/", "_") == rest), None)
if unit is None:
    sys.exit(f"no unit in {mod}'s splits for {rest}")
dst = f"rels/{mod}/src/{unit}.c"
os.makedirs(os.path.dirname(dst), exist_ok=True)
text = open(f"{src}/final.c").read()
if not keep:
    text = re.sub(r"\n#pragma force_active off\s*$", "\n", text)
open(dst, "w").write(text)
if os.path.exists(f"{src}/final.h"):
    hdr = f"rels/{mod}/include/{os.path.basename(unit)}.h"
    os.makedirs(os.path.dirname(hdr), exist_ok=True)
    open(hdr, "w").write(open(f"{src}/final.h").read())
patch = f"{src}/include.patch"
if os.path.exists(patch) and os.path.getsize(patch):
    subprocess.run(["patch", "-p0", "-N", "-s", "-i", os.path.abspath(patch)], check=False)
cfg = open("configure.py").read()
line = f'            Object(NonMatching, "{unit}.c"),\n'
if f'"{unit}.c"' not in cfg:
    m = re.search(rf'Rel\(\n        "{mod}",\n        \[\n(.*?)\n        \],\n', cfg, re.S)
    if not m:
        sys.exit(f"no Rel({mod!r}) list found")
    cfg = cfg[: m.end(1)] + "\n" + line.rstrip("\n") + cfg[m.end(1):]
    open("configure.py", "w").write(cfg)
subprocess.run(["python3", "configure.py"], capture_output=True)
print(f"adopted {mod}/{unit}")
