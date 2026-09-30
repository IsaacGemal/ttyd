#!/usr/bin/env python3
"""Local CI: the single pass/fail gate for any change, human or agent.

    python3 tools/ci.py                 check the working tree against the accepted state
    python3 tools/ci.py --base REV      lint the diff against REV instead of HEAD
    python3 tools/ci.py --accept        on success, make this build the new accepted state
    python3 tools/ci.py --init          accept the current build without checks (first run only)

Steps, stopping at the first failure:
0. Tests: tools/tests (the checks' own unit tests).
1. Lint the diff: no new inline asm, no new #pragma, no edits to protected paths, and configure.py
   changes limited to Object(...) lines. Overrides: --allow-pragma, --allow-protected, --allow-configure.
   Also reject external local prototypes in src/ and rels/ except documented ABI exceptions.
2. Build: configure + ninja. Ninja fails if a linked module stops hashing to config/*/build.sha1.
3. Ratchet: compare build/<ver>/report.json with the accepted report (build/ci/accepted.json) using
   objdiff-cli. Any function, unit or total that got worse fails the run, whatever else improved.
   --allow-regress NAME excuses a named, not-yet-matched function's fuzzy score (and its unit's and the
   total's fuzzy score), or every measure of a named unit (e.g. MarioSt/event/evt_eff) and the total's same
   measure: for deliberate fixes or removals of wrong code that lower a score. Say why in the commit.
4. Summary: code fuzzy / matched / linked and data matched / linked, with deltas, and a note if any unit is
   fully matched but not yet linked.
"""
import argparse
import local_prototypes
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VERSION = "G8MJ01"
BUILD = ROOT / "build" / VERSION
CI_DIR = ROOT / "build" / "ci"
ACCEPTED = CI_DIR / "accepted.json"
OBJDIFF = ROOT / "build" / "tools" / "objdiff-cli"

PROTECTED = ("config/", "orig/", "tools/", ".github/", "libs/", "assets/")
UNPROTECTED = ("libs/metrowerks/src/",)  # the runtime libraries ported here are ours to decompile (their headers aren't)
ASM_RE = re.compile(r"(^|[^\w])(asm|__asm__|__asm)\b")
PRAGMA_RE = re.compile(r"^\s*#\s*pragma\b(?!\s+once\b)")
UNIT_KEYS = ["fuzzy_match_percent", "matched_code_percent", "matched_data_percent",
             "complete_code_percent", "complete_data_percent"]
FUNC_KEYS = ["fuzzy_match_percent"]
SUMMARY = [("code fuzzy", "fuzzy_match_percent"), ("code matched", "matched_code_percent"),
           ("code linked", "complete_code_percent"), ("data matched", "matched_data_percent"),
           ("data linked", "complete_data_percent")]


def run(cmd, **kw):
    return subprocess.run(cmd, cwd=ROOT, text=True, capture_output=True, **kw)


def fail(step, msg):
    print(f"FAIL [{step}] {msg}")
    sys.exit(1)


def lint(base, allow_pragma, allow_protected, allow_configure, allow_asm=False):
    names = run(["git", "diff", "--name-only", base]).stdout.split()
    names += run(["git", "ls-files", "--others", "--exclude-standard"]).stdout.split()
    problems = []
    for name in names:
        if name.startswith(PROTECTED) and not name.startswith(UNPROTECTED) and name != "tools/ci.py" and not allow_protected:
            problems.append(f"{name}: protected path (pass --allow-protected if intended)")
    scanned = ["src", "include", "rels", "libs/metrowerks/src"]
    diff = run(["git", "diff", "-U0", base, "--", *scanned, "configure.py"]).stdout
    for path in run(["git", "ls-files", "--others", "--exclude-standard", *scanned]).stdout.split():
        diff += f"+++ b/{path}\n" + "".join("+" + l for l in (ROOT / path).read_text(errors="replace").splitlines(True))
    current = None
    for line in diff.splitlines():
        if line.startswith("+++ "):
            current = line[6:] if line.startswith("+++ b/") else None
            continue
        if not line.startswith("+") or current is None:
            continue
        added = line[1:]
        if current == "configure.py":
            if added.strip() and "Object(" not in added and not allow_configure:
                problems.append(f"configure.py: only Object(...) lines may change (pass --allow-configure if intended): {added.strip()[:100]}")
            continue
        code = added.split("//", 1)[0]
        if ASM_RE.search(code) and not (allow_asm and current.startswith(UNPROTECTED)):
            problems.append(f"{current}: new inline asm: {added.strip()[:100]}"
                            + (" (--allow-asm: only for the ported libraries' hand-written assembly, checked against"
                               " the reference projects)" if current.startswith(UNPROTECTED) else ""))
        if PRAGMA_RE.search(code) and not allow_pragma:
            problems.append(f"{current}: new #pragma (pass --allow-pragma if intended): {added.strip()[:100]}")
    if problems:
        fail("lint", "\n  " + "\n  ".join(problems))
    print(f"ok   [lint] {len(names)} changed file(s) vs {base}")


def extend_data_ids():
    """extend_data(id) is an inline function forcedata<id>: two files linked into one module with the same id are
    merged by the linker and the second one's constants vanish. Ids must be unique among the sources each REL links
    (objdiff.json's source paths, shared rels/common units included), and among everything linked into the dol."""
    import json, re
    r = run([sys.executable, "configure.py"])
    if r.returncode:
        fail("extend_data", "configure.py failed:\n" + r.stderr[-2000:])
    rels = {d.name for d in (ROOT / "rels").iterdir() if d.is_dir() and d.name != "common"}
    sources = {}
    for u in json.load(open(ROOT / "objdiff.json"))["units"]:
        path = u.get("metadata", {}).get("source_path")
        if path:
            module = u["name"].split("/", 1)[0]
            sources.setdefault(module if module in rels else "dol", set()).add(path)
    problems = []
    for module, paths in sorted(sources.items()):
        seen = {}
        for path in sorted(paths):
            f = ROOT / path
            if not f.exists():
                continue
            for ident in set(re.findall(r"extend_data\s*\(\s*(\w+)\s*,", f.read_text(errors="replace"))):
                if ident in seen and seen[ident] != path:
                    problems.append(f"{module}: extend_data({ident}) in {path} and {seen[ident]}")
                seen[ident] = path
    if problems:
        fail("extend_data", "\n  " + "\n  ".join(problems))
    print(f"ok   [extend_data] ids unique within each of {len(sources)} modules' linked sources")


def build():
    r = run([sys.executable, "configure.py"])
    if r.returncode:
        fail("build", "configure.py failed:\n" + r.stderr[-2000:])
    ninja = shutil.which("ninja") or str(ROOT / ".venv" / "bin" / "ninja")
    r = run([ninja])
    if r.returncode:
        fail("build", "ninja failed (compile error, or a linked module no longer matches build.sha1):\n"
             + (r.stdout + r.stderr)[-3000:])
    print("ok   [build] ninja clean; linked modules match build.sha1")


def ratchet(allowed=()):
    current = BUILD / "report.json"
    changes = CI_DIR / "changes.json"
    r = run([str(OBJDIFF), "report", "changes", "--format", "json", str(ACCEPTED), str(current), "-o", str(changes)])
    if r.returncode:
        fail("ratchet", "objdiff-cli report changes failed:\n" + r.stderr[-2000:])
    data = json.loads(changes.read_text())
    worse, better = [], []

    def cmp(name, obj, keys):
        for k in keys:
            a, b = obj.get("from", {}).get(k, 0.0), obj.get("to", {}).get(k, 0.0)
            (worse if b < a else better if b > a else []).append((name, k, a, b))

    cmp("TOTAL", data, UNIT_KEYS)
    for unit in data.get("units", []):
        cmp(unit["name"], unit, UNIT_KEYS)
        for fn in unit.get("functions", []):
            cmp(f"{unit['name']}:{fn['name']}", fn, FUNC_KEYS)
    if allowed:
        # A name is a function (its fuzzy drop, while below 100%, excuses its unit's and the total's fuzzy drop)
        # or a unit path such as MarioSt/event/evt_eff (any of its measures, and the total's same measure).
        named = [w for w in worse if (w[0].split(":")[-1] in allowed and w[2] < 100) or w[0] in allowed]
        units = {n.split(":", 1)[0] for n, _, _, _ in named}
        keys = {k for n, k, _, _ in named if ":" not in n}
        excused = named + [w for w in worse if w not in named and (
            (w[1] == "fuzzy_match_percent" and (w[0] == "TOTAL" or w[0] in units))
            or (w[0] == "TOTAL" and w[1] in keys))]
        for n, k, a, b in excused:
            print(f"     allowed regression: {n} {k.removesuffix('_percent')}: {a:.2f} -> {b:.2f}")
        worse = [w for w in worse if w not in excused]
    if worse:
        lines = [f"{n} {k.removesuffix('_percent')}: {a:.2f} -> {b:.2f}" for n, k, a, b in worse[:40]]
        fail("ratchet", f"{len(worse)} regression(s) vs the accepted build:\n  " + "\n  ".join(lines))
    print(f"ok   [ratchet] no regressions; {len(better)} measure(s) improved")


def tests():
    r = run([sys.executable, "-m", "unittest", "discover", "-s", str(ROOT / "tools" / "tests"), "-q"])
    if r.returncode:
        fail("tests", "tools/tests failed:\n" + (r.stderr or r.stdout)[-2000:])
    print("ok   [tests] tools/tests pass")


def unlinked():
    """Fully matched units still marked NonMatching: tools/autolink.py would try to link them."""
    r = run([sys.executable, str(ROOT / "tools" / "autolink.py"), "--list"])
    names = [l for l in r.stdout.splitlines() if l.strip()]
    if names:
        print(f"note {len(names)} unit(s) fully matched but not linked (python3 tools/autolink.py --list)")


def summary():
    now = json.loads((BUILD / "report.json").read_text())["measures"]
    before = json.loads(ACCEPTED.read_text())["measures"]
    for label, k in SUMMARY:
        a, b = float(before.get(k, 0)), float(now.get(k, 0))
        print(f"     {label:13} {b:6.2f}%  ({b - a:+.2f})")
    print(f"     functions     {now.get('matched_functions', 0)} / {now.get('total_functions', 0)} matched")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--base", default="HEAD")
    ap.add_argument("--accept", action="store_true")
    ap.add_argument("--init", action="store_true")
    ap.add_argument("--allow-pragma", action="store_true")
    ap.add_argument("--allow-protected", action="store_true")
    ap.add_argument("--allow-asm", action="store_true")
    ap.add_argument("--allow-configure", action="store_true")
    ap.add_argument("--allow-regress", action="append", default=[], metavar="NAME",
                    help="a not-yet-matched function, or a unit, whose score may drop when wrong code is fixed or "
                         "removed; repeatable")
    a = ap.parse_args()
    CI_DIR.mkdir(parents=True, exist_ok=True)
    if a.init:
        build()
        shutil.copy(BUILD / "report.json", ACCEPTED)
        print(f"accepted current build as the baseline ({ACCEPTED.relative_to(ROOT)})")
        return
    if not ACCEPTED.exists():
        fail("ratchet", "no accepted baseline yet; run `python3 tools/ci.py --init` on a known-good tree")
    tests()
    lint(a.base, a.allow_pragma, a.allow_protected, a.allow_configure, a.allow_asm)
    problems, count = local_prototypes.check_tree(ROOT)
    if problems:
        fail("prototypes", "\n  " + "\n  ".join(problems))
    print(f"ok   [prototypes] {count} documented exceptions; no other external local prototypes")
    extend_data_ids()
    build()
    ratchet(set(a.allow_regress))
    summary()
    unlinked()
    if a.accept:
        shutil.copy(BUILD / "report.json", ACCEPTED)
        print("     accepted: this build is the new baseline")
    print("PASS")


if __name__ == "__main__":
    main()
