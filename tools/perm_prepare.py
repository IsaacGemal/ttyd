#!/usr/bin/env python3
"""Create a decomp-permuter input directory for one function of a dol unit.

    python3 tools/perm_prepare.py UNIT FUNC [OUTDIR]       e.g. effect/eff_hibashira effHibashiraMain

Writes OUTDIR (default $PERM_ROOT/inputs/UNIT__FUNC, PERM_ROOT default ~/permuter) with
  base.c         the unit's source, preprocessed with the unit's flags, every other function reduced to a prototype
  target.o       the original unit object (build/G8MJ01/obj/UNIT.o); the scorer only reads FUNC from it
  compile.sh     the project's MWCC command line (wibo + sjiswrap), run from the repo root
  settings.toml  func_name, compiler_type = "mwcc", and tools/perm_objdump.py as the disassembler (PERM_FUNC selects FUNC)
Needs a build first (build.ninja, build/G8MJ01/obj). Run it with tools/perm_run.py, which adds the time, CPU and disk limits."""
import os
import re
import shlex
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
PERMUTER = os.environ.get("PERM_HOME", os.path.expanduser("~/permuter/decomp-permuter"))
sys.path.insert(0, PERMUTER)
from strip_other_fns import strip_other_fns  # noqa: E402

unit, func = sys.argv[1], sys.argv[2]
out = sys.argv[3] if len(sys.argv) > 3 else os.path.join(os.environ.get("PERM_ROOT", os.path.expanduser("~/permuter")), "inputs", unit.replace("/", "_") + "__" + func)
os.makedirs(out, exist_ok=True)
ninja = open(os.path.join(ROOT, "build.ninja")).read()
m = re.search(rf"^build build/G8MJ01/src/{re.escape(unit)}\.o: mwcc\w* .*?\n  cflags = (.*?)\n  basedir", ninja, re.M | re.S)
if not m:
    sys.exit(f"{unit}: no compile rule in build.ninja")
cflags = re.sub(r"\s+", " ", re.sub(r"\$\n\s*", "", m.group(1))).strip()
toks = [t for t in shlex.split(cflags) if t != "-MMD"]
pre = subprocess.run(["build/tools/wibo", "build/tools/sjiswrap.exe", "build/compilers/GC/2.6/mwcceppc.exe", *toks, "-E",
                      os.path.join("src", unit + ".c")], capture_output=True, text=True, cwd=ROOT)
if pre.returncode:
    sys.exit("mwcc -E failed:\n" + (pre.stdout + pre.stderr)[-2000:])
text = re.sub(r"__declspec\s*\((?:[^()]|\([^()]*\))*\)", "", pre.stdout)  # the parser cannot read it; it only places data
text = text.replace("__typeof__(sizeof(0))", "unsigned long")  # the permuter prints it back as `typeof`, which MWCC rejects
text = re.sub(r"\bsizeof\s*\((\w+)\)", r"(sizeof(\1))", text)  # `sizeof(T) + x` parses as sizeof((T)(+x)) when T is a typedef
src = strip_other_fns(text, func)
open(os.path.join(out, "base.c"), "w").write(src)
target = os.path.join(ROOT, "build/G8MJ01/obj", unit + ".o")
if not os.path.exists(target):
    sys.exit(f"{target} missing: build first")
subprocess.run(["cp", target, os.path.join(out, "target.o")], check=True)
flags = " ".join(shlex.quote(t) for t in toks if not t.startswith("-i") and not t.startswith("-MMD"))
# drop the include paths and -MMD: base.c is already preprocessed
clean, skip = [], False
for t in toks:
    if skip:
        skip = False
        continue
    if t == "-i":
        skip = True
        continue
    if t in ("-MMD",):
        continue
    clean.append(t)
compile_sh = f"""#!/bin/bash
# usage: compile.sh input.c -o output.o
cd {shlex.quote(ROOT)}
{ROOT}/build/tools/wibo {ROOT}/build/tools/sjiswrap.exe {ROOT}/build/compilers/GC/2.6/mwcceppc.exe {' '.join(shlex.quote(t) for t in clean)} -c "$1" -o "$3"
"""
open(os.path.join(out, "compile.sh"), "w").write(compile_sh)
os.chmod(os.path.join(out, "compile.sh"), 0o755)
open(os.path.join(out, "settings.toml"), "w").write(
    f'func_name = "{func}"\ncompiler_type = "mwcc"\nobjdump_command = "env PERM_FUNC={func} {sys.executable} {ROOT}/tools/perm_objdump.py"\n')
print(out)
