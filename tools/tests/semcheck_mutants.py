#!/usr/bin/env python3
"""Regression test for tools/semcheck.py: plant one bug at a time in a fully matched file, compile it with the unit's
own flags, and check that semcheck flags it (and that the unchanged file gets no flags). Run after a build, from the
repo root: python3 tools/tests/semcheck_mutants.py. Exit 1 if a planted bug is missed.

The mutants are a wave 13 review's (PR #18, review 1), on src/effect/eff_puniballoon.c. One of them
(`operand_swap`: `i < n` written `n > i`) is the same code, so it must NOT be flagged.
"""
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
UNIT = "effect/eff_puniballoon"
SAME = {"operand_swap"}
MUTANTS = {  # name: (line, old text, new text)
    "call_missing": (39, "GXSetCullMode(GX_CULL_NONE);", ""),
    "call_arg": (29, "GX_FALSE, 0x7d", "GX_FALSE, 0x3c"),
    "gx_enum": (34, "GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC", "GX_CC_ZERO, GX_CC_ONE, GX_CC_ZERO, GX_CC_TEXC"),
    "call_arg_swap": (43, "GX_POS_XYZ, GX_S16", "GX_S16, GX_POS_XYZ"),
    "value_const": (153, "0x10", "0x11"),
    "value_mask": (151, "& 4", "& 8"),
    "mem_field": (175, "data->velX", "data->velZ"),
    "mem_field2": (170, "data->scale = scale", "data->field_0x1c = scale"),
    "ops_invert": (156, "< 1000", ">= 1000"),
    "ops_le": (156, "< 1000", "<= 1000"),
    "operand_swap": (174, "i < effect->count", "effect->count > i"),
    "operand_swap2": (47, "i < effect->count", "i > effect->count"),
    "float_const": (179, "0.98f", "0.97f"),
    "float_const2": (170, "10.0f", "20.0f"),
    "quad_swap": (57, "GXPosition3s16(-12, -12, 0)", "GXPosition3s16(12, 12, 0)"),
    "tex_swap": (58, "GXTexCoord2s16(0, 1)", "GXTexCoord2s16(1, 0)"),
    "string": (206, '"PuniBalloon"', '"PuniBalloom"'),
    "case_val": (201, "case 1:", "case 2:"),
    "rand_mod": (229, "% 11 - 5", "% 11 - 6"),
    "wrong_var_arg": (163, "data->position.x, data->position.y", "data->position.y, data->position.x"),
    "wrong_idx_param": (185, "2, effPuniBalloonDisp", "3, effPuniBalloonDisp"),
    "float_lone": (182, "0.9f", "0.8f"),
    "float_lone2": (178, "-0.1f", "-0.2f"),
    "float_zero": (217, "data->field_0x24 = 0.0f", "data->field_0x24 = 1.0f"),
    "var_for_const": (185, "2, effPuniBalloonDisp", "type, effPuniBalloonDisp"),
    "cmp_const": (156, "< 1000", "< 100"),
    "sin_arg": (171, "20.0f * (f32)sin", "20.0f * (f32)cos"),
}


def main():
    obj = f"build/G8MJ01/src/{UNIT}.o"
    cmd = subprocess.run(["ninja", "-t", "commands", obj], cwd=ROOT, capture_output=True, text=True).stdout.splitlines()[-1]
    cmd = cmd.split(" && ")[0]  # the compile, without the dependency-file step
    src = (ROOT / f"src/{UNIT}.c").read_text().split("\n")
    missed, noisy = [], []
    with tempfile.TemporaryDirectory(dir=ROOT / "build") as tmp:
        for name, (ln, old, new) in [("unchanged", (0, "", ""))] + list(MUTANTS.items()):
            lines = list(src)
            if ln:
                assert old in lines[ln - 1], name
                lines[ln - 1] = lines[ln - 1].replace(old, new)
            c = Path(tmp) / f"{name}.c"
            c.write_text("\n".join(lines))
            out = Path(tmp) / name
            out.mkdir()
            run = shlex.split(cmd)
            run[run.index("-c") + 1] = str(c)
            run[run.index("-o") + 1] = str(out)
            subprocess.run(run, cwd=ROOT, capture_output=True, check=True)
            res = subprocess.run(["python3", "tools/semcheck.py", UNIT, "--object", str(out / f"{name}.o")],
                                 cwd=ROOT, capture_output=True, text=True).stdout
            flagged = " 0 flag(s)" not in res
            if name in ("unchanged",) or name in SAME:
                if flagged:
                    noisy.append(name)
            elif not flagged:
                missed.append(name)
            print(f"{name:16} {'flagged' if flagged else 'clean'}")
    print(f"{len(MUTANTS) - len(SAME) - len(missed)}/{len(MUTANTS) - len(SAME)} planted bugs flagged;"
          f" missed: {missed or 'none'}; flagged without a bug: {noisy or 'none'}")
    sys.exit(1 if missed or noisy else 0)


if __name__ == "__main__":
    main()
