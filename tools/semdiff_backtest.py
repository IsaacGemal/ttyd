#!/usr/bin/env python3
"""Back-test tools/semdiff.py on bugs that reviews found in past waves.

  python3 tools/semdiff_backtest.py [-v] [--keep]

--keep leaves the scratch worktree in place for the next run (it is removed by default).

Each case names a function, the commit before its fix (bug present) and the fix commit. For both commits the
file is compiled as it was then (a scratch git worktree under build/semdiff_bt, with that commit's configure.py
flags) and compared with the original object. A case passes when semdiff flags the expected token before the
fix and no longer flags it after (or, for a function that was removed instead of fixed, when it's flagged
before). The token is one symptom chosen by hand: a pass shows semdiff sees the bug, not that the fixed function
has no other differences. Cases marked `limit` are bugs semdiff isn't expected to catch (control flow,
stack arguments, matched functions); they're run and reported so a change to semdiff that starts catching them
shows up. Exit 1 if any expected detection fails.
"""
import re
import shutil
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import semdiff  # noqa: E402

ROOT = semdiff.ROOT
WT = ROOT / "build" / "semdiff_bt"

# (unit, function, commit with the bug, fix commit, token expected before the fix and gone after, note)
# The token is matched as a substring of semdiff's "original only: ..." / "ours only: ..." lines.
CASES = [
    # wave 4, fixed in b2a5087 and 16f68b2
    ("event/evt_mobj", "evt_mobj_switch_red", "b2a5087^", "b2a5087", "original only: logic or 0xa10000",
     "flag ORed 8 bits too far"),
    ("event/evt_mobj", "evt_mobj_jumpstand_blue", "b2a5087^", "b2a5087", "original only: logic or 0x200000", ""),
    ("event/evt_mario", "evt_koopa_hip_attack", "b2a5087^", "b2a5087", "original only: mask 0xfff0ffff",
     "clears bits 12-15 instead of 16-19"),
    ("koopa", "kpaFireMain", "b2a5087^", "b2a5087", "limit", "PSMTXConcat destination is a stack address"),
    ("window/win_root", "sort_5_3_func", "16f68b2^", "16f68b2", "original only: imm 0x92490000", "/10 vs /28"),
    ("event/evt_nannpc", "evt_nannpc_move_position2", "254051f^", "254051f", "original only: call __div2u",
     "signed vs unsigned 64-bit division"),
    ("party/party_motion", "party_motion_jump2", "d14958f^", "d14958f", "limit",
     "0.0f argument left in f1 by luck: same constant used elsewhere in the function"),
    # wave 5, fixed in 828803d, ccc4c8b and eea503a
    ("battle/sac/sac_deka", "yuka_capture", "828803d^", "828803d", "ours only: logic xor 0x80000000",
     "tan() with no prototype: result read from r3 as an int"),
    ("battle/battle_audience_kinopio", "BattleAudienceCtrlProcessKinopioIntrude", "828803d^", "828803d",
     "ours only: logic xor 0x80000000", "fabs() with no prototype: int result converted to double"),
    ("battle/battle_audience_kinopio", "BattleAudienceCtrlProcessKinopioIntrude", "eea503a^", "eea503a",
     "original only: op fabs", "fabs called as a function; the original uses the instruction"),
    ("mario/mario_motion", "marioLandOn", "ccc4c8b^", "ccc4c8b", "original only: mask 0x00100000",
     "HAMMER_HIT_WALL tested instead of PAPER_MODE"),
    ("party/party_yoshi", "L_yoshiFlyMove", "ccc4c8b^", "ccc4c8b", "original only: cmp cror",
     "clamp inverted"),
    ("party/party_yoshi", "marioYoshiForceCancel", "ccc4c8b^", "ccc4c8b", "limit", "else on the wrong if"),
    ("battle/sac/sac_scissor", "wait_star_standby", "ccc4c8b^", "ccc4c8b", "original only: imm -5",
     "== 5 read as < 6"),
    ("battle/sac/sac_genki", "wait_game_end", "ccc4c8b^", "ccc4c8b", "original only: imm -3", "== 3 read as < 4"),
    ("battle/sac/sac_suki", "wait_star_stone_up_end", "ccc4c8b^", "ccc4c8b", "original only: imm -4", ""),
    ("battle/sac/sac_suki", "aud_set_draw", "ccc4c8b^", "ccc4c8b", "original only: mask 0xfffdffff",
     "clears 0x1C000 instead of 0x20000"),
    ("battle/battle_audience_kinopio", "BattleAudienceCtrlProcessKinopioTransEvt", "ccc4c8b^", "ccc4c8b",
     "original only: imm 0xf1190000", "0x114d80 instead of -250000000"),
    ("battle/sac/sac_scissor", "scissor_intersection3", "ccc4c8b^", "ccc4c8b", "ours only: cmp cror eq,lt,eq",
     "ay <= by instead of >="),
    ("battle/sac/sac_genki", "main_object", "ccc4c8b^", "ccc4c8b", "limit", "missing case fall-through"),
    ("battle/battle_audience_kinopio", "BattleAudienceCtrlProcessKinopioIntrude", "eea503a^", "eea503a",
     "original only: load f64:450.0", "thresholds read from a zeroed stand-in array"),
    # Held out while semdiff was first built (tokens chosen afterwards, from the wave 5 review's descriptions);
    # from now on they're regression cases like the rest. Removed in ccc4c8b rather than fixed, so no "after".
    ("battle/sac/sac_suki", "star_stone_attack", "ccc4c8b^", None, "original only: imm 2", "returns 2; ours void"),
    ("battle/sac/sac_suki", "set_tev", "ccc4c8b^", None, "0xff", "alpha 0xFF in the wrong byte"),
    ("battle/sac/sac_suki", "main_base", "ccc4c8b^", None, 'str:"SFX_BTL_SAC_SUKKIRI2"', "wrong SFX strings"),
    ("battle/sac/sac_suki", "main_base", "ccc4c8b^", None, "original only: mask 0x00000f00", "mask 0xF00"),
    ("battle/sac/sac_genki", "main_star1", "ccc4c8b^", None, "ours only: store", "writes the wrong fields"),
    ("battle/sac/sac_genki", "main_weapon", "ccc4c8b^", None, "original only: call effHitEntry",
     "hit resolution missing"),
    ("party/party_yoshi", "L_getStick", "ccc4c8b^", None, "op extsb", "negate then truncate to s8 (not fixed)"),
]


def sh(cmd, cwd=ROOT, check=True):
    r = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, shell=isinstance(cmd, str))
    if check and r.returncode:
        raise RuntimeError(f"{cmd}: {r.stdout[-800:]}{r.stderr[-800:]}")
    return r


def setup_worktree():
    if not (WT / ".git").exists():
        sh(["git", "worktree", "add", "--detach", str(WT), "HEAD"])
    for rel in ("orig", "build/tools", "build/compilers", "build/G8MJ01/include", "build/G8MJ01/config.json"):
        link = WT / rel
        if not link.exists():
            link.parent.mkdir(parents=True, exist_ok=True)
            link.symlink_to(ROOT / rel)


def build_at(commit, units):
    """Compile `units` as they were at `commit`; returns {unit: object path}."""
    sh(["git", "checkout", "-q", "--detach", commit], cwd=WT)
    sh([sys.executable, "configure.py"], cwd=WT)
    ninja = shutil.which("ninja") or str(ROOT / ".venv" / "bin" / "ninja")
    out = {}
    for unit in units:
        obj = f"build/G8MJ01/src/{unit}.o"
        cmd = sh([ninja, "-t", "commands", obj], cwd=WT).stdout.strip().splitlines()[-1]
        (WT / obj).parent.mkdir(parents=True, exist_ok=True)
        dest = WT / "build" / "bt" / commit.replace("^", "_parent") / f"{unit}.o"
        dest.parent.mkdir(parents=True, exist_ok=True)
        r = sh(cmd, cwd=WT, check=False)
        if r.returncode:
            raise RuntimeError(f"compile {unit} at {commit} failed:\n{r.stdout[-1500:]}{r.stderr[-1500:]}")
        shutil.copy(WT / obj, dest)
        out[unit] = dest
    return out


def flags(unit, obj, func):
    orig = ROOT / "build" / "G8MJ01" / "obj" / f"{unit}.o"
    diff = semdiff.run_objdiff(["-1", str(orig), "-2", str(obj)])
    res = semdiff.compare(orig, obj, diff, [func], include_matched=True)
    if not res:
        return None, []
    r = res[0]
    return r["percent"], [f"original only: {t}" for t in r["missing"]] + [f"ours only: {t}" for t in r["extra"]]


def main():
    verbose = "-v" in sys.argv
    setup_worktree()
    commits = {}
    for unit, _, bad, good, _, _ in CASES:
        commits.setdefault(bad, set()).add(unit)
        if good:
            commits.setdefault(good, set()).add(unit)
    # The worktree borrows the current build's split (config.json), generated headers and original objects, which
    # is only valid if the split configuration hasn't changed since each commit.
    for commit in commits:
        if sh(["git", "diff", "--quiet", commit, "HEAD", "--", "config/"], check=False).returncode:
            sys.exit(f"config/ changed between {commit} and HEAD: the borrowed split doesn't apply to that commit")
    objs = {}
    for commit, units in commits.items():
        for unit, path in build_at(commit, sorted(units)).items():
            objs[(commit, unit)] = path
    passed = failed = 0
    for unit, func, bad, good, expect, note in CASES:
        pb, fb = flags(unit, objs[(bad, unit)], func)
        pg, fg = flags(unit, objs[(good, unit)], func) if good else (None, [])
        if good is None:
            ok = any(expect in f for f in fb)
            status = "PASS" if ok else "FAIL"
            passed += ok
            failed += not ok
            detail = f"before fix: {'flagged' if ok else 'NOT flagged'} (no fix commit: function removed)"
        elif expect == "limit":
            status = "limit"
            detail = "(not expected to be caught)"
        else:
            before = any(expect in f for f in fb)
            after = any(expect in f for f in fg)
            ok = before and not after
            status = "PASS" if ok else "FAIL"
            passed += ok
            failed += not ok
            detail = f"before fix: {'flagged' if before else 'NOT flagged'}; after fix: {'still flagged' if after else 'gone'}"
        print(f"{status:5} {unit} {func} ({pb}% -> {pg}%): {note} {detail}")
        if verbose or status != "PASS":
            for f in fb:
                print(f"        before: {f}")
            for f in fg:
                print(f"        after:  {f}")
    print(f"\n{passed} passed, {failed} failed, {sum(c[4] == 'limit' for c in CASES)} known limits")
    if "--keep" not in sys.argv:
        sh(["git", "worktree", "remove", "--force", str(WT)], check=False)
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
