#!/usr/bin/env python3
"""Create a decomp-permuter input directory for one function of a dol or REL unit.

    python3 tools/perm_prepare.py UNIT FUNC [OUTDIR] [--keep-all] [--no-check]       e.g. effect/eff_hibashira effHibashiraMain; REL: yuu/event/evt_yuucounter FUNC (see tools/perm_unit.py)

Writes OUTDIR (default $PERM_ROOT/inputs/UNIT__FUNC, PERM_ROOT default ~/permuter) with
  base.c         the unit's source, preprocessed with the unit's flags, every other function reduced to a prototype
  target.o       the original unit object (build/G8MJ01/obj/UNIT.o, or build/G8MJ01/MOD/obj/PATH.o for a REL); the scorer only reads FUNC from it
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

sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
import perm_unit  # noqa: E402

KEEP_ALL = "--keep-all" in sys.argv
args = [a for a in sys.argv[1:] if not a.startswith("--")]
unit, func = args[0], args[1]
out = args[2] if len(args) > 2 else os.path.join(os.environ.get("PERM_ROOT", os.path.expanduser("~/permuter")), "inputs", unit.replace("/", "_") + "__" + func)
try:
    res = perm_unit.resolve(unit, ROOT)
    cflags = perm_unit.cflags(open(os.path.join(ROOT, "build.ninja")).read(), res["name"])
except LookupError as e:
    sys.exit(str(e))
os.makedirs(out, exist_ok=True)
toks = [t for t in shlex.split(cflags) if t != "-MMD"]
pre = subprocess.run(["build/tools/wibo", "build/tools/sjiswrap.exe", "build/compilers/GC/2.6/mwcceppc.exe", *toks, "-E",
                      res["src"]], capture_output=True, cwd=ROOT)
pre.stdout = pre.stdout.decode("utf-8", errors="replace")  # Shift-JIS string literals: the code is what is scored
pre.stderr = pre.stderr.decode("utf-8", errors="replace")
if pre.returncode:
    sys.exit("mwcc -E failed:\n" + (pre.stdout + pre.stderr)[-2000:])
text = re.sub(r"__declspec\s*\((?:[^()]|\([^()]*\))*\)", "", pre.stdout)  # the parser cannot read it; it only places data
text = text.replace("__typeof__(sizeof(0))", "unsigned long")  # the permuter prints it back as `typeof`, which MWCC rejects
text = re.sub(r"\bsizeof\s*\((\w+)\)", r"(sizeof(\1))", text)  # `sizeof(T) + x` parses as sizeof((T)(+x)) when T is a typedef


def drop_extern_c(s):
    """math.h wraps some declarations in `#pragma cplusplus on` / `extern "C" { ... }`; the permuter's C parser rejects it."""
    while True:
        m = re.search(r'extern "C"\s*\{', s)
        if not m:
            break
        depth, i = 1, m.end()
        while depth:
            depth += {"{": 1, "}": -1}.get(s[i], 0)
            i += 1
        s = s[:m.start()] + s[m.end():i - 1] + s[i:]
    return re.sub(r"^#pragma cplusplus (?:on|reset)\s*$", "", s, flags=re.M)


text = drop_extern_c(text)
src = text if KEEP_ALL else strip_other_fns(text, func)  # --keep-all: constants pooled by the other functions change how this one compiles
open(os.path.join(out, "base.c"), "w").write(src)
target = os.path.join(ROOT, res["target"])
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
if "--no-check" not in sys.argv:
    import perm_check
    base_c = os.path.join(out, "base.c")
    args_ = (os.path.join(out, "compile.sh"), base_c, os.path.join(out, "target.o"), func, os.path.join(ROOT, "tools/perm_objdump.py"))
    why = perm_check.check(*args_)
    if why and not KEEP_ALL:
        # a stripped `static inline` helper is the usual cause: keep every function and look again
        open(base_c, "w").write(text)
        why = perm_check.check(*args_)
        if not why:
            print("note: base.c kept every function (the stripped form did not compile like the target)", file=sys.stderr)
    if why:
        sys.exit(f"{unit} {func}: {why}")
print(out)
