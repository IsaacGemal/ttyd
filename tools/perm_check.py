"""Is a prepared permuter input sane? Compile base.c once and compare the function with the target before a run is spent on it.

A wrong base wastes the run: win_root's winMailGX ran 14 minutes on a base that compiled to 311 instructions against the target's 2133
(strip_other_fns had reduced the `static inline` helper it expands to a prototype), a score of 200630 for a 99.8% function.

    check(compile_sh, base_c, target_o, func, objdump)  ->  None when plausible, else a one-line reason (the first compile error, a missing
    function, or an instruction count / mnemonic sequence too unlike the target's)."""
import difflib
import os
import subprocess
import sys
import tempfile

MIN_SIZE_RATIO = 0.7
MIN_SIMILARITY = 0.6


def first_error(text):
    """The first compiler error with its location line, from mwcc's output."""
    lines = [ln.rstrip() for ln in text.splitlines()]
    for i, ln in enumerate(lines):
        if "Error:" in ln or "Error :" in ln:
            ctx = [x.strip("# ").strip() for x in lines[max(0, i - 3):i + 2] if x.strip("# -")]
            return " | ".join(ctx)[:300]
    return (lines[-1] if lines else "compiler failed")[:300]


def plausible(ours, target):
    """None when `ours` (a list of instruction lines) looks like a build of the function `target` holds, else the reason."""
    if not ours:
        return "the function is missing from the compiled base.c"
    ratio = min(len(ours), len(target)) / max(len(ours), len(target))
    if ratio < MIN_SIZE_RATIO:
        return f"base.c compiles to {len(ours)} instructions, the target has {len(target)} (an inline helper stripped from base.c?)"
    sm = difflib.SequenceMatcher(None, [x.split("\t")[2] if x.count("\t") >= 2 else x for x in ours],
                                 [x.split("\t")[2] if x.count("\t") >= 2 else x for x in target], autojunk=False)
    if sm.ratio() < MIN_SIMILARITY:
        return f"base.c's mnemonic sequence matches the target's only {sm.ratio():.0%}"
    return None


def disasm(objdump, func, path):
    r = subprocess.run([sys.executable, objdump, path], capture_output=True, text=True, env=dict(os.environ, PERM_FUNC=func))
    return [ln for ln in r.stdout.splitlines() if ln.count("\t") >= 3]


def check(compile_sh, base_c, target_o, func, objdump):
    with tempfile.TemporaryDirectory() as d:
        out = os.path.join(d, "base.o")
        r = subprocess.run(["bash", compile_sh, base_c, "-o", out], capture_output=True, text=True)
        if r.returncode or not os.path.exists(out):
            return "base.c does not compile: " + first_error(r.stdout + r.stderr)
        return plausible(disasm(objdump, func, out), disasm(objdump, func, target_o))
