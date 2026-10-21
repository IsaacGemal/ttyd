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
   Data objects: every data object defined or changed in the diff (tools/data_check.py) must hold the bytes
   of the original object; --allow-data-mismatch NAME excuses one.
4. Summary: code fuzzy / matched / linked and data matched / linked, with deltas, and a note if any unit is
   fully matched but not yet linked.
5. Cleanup index (tools/cleanup_index.py): reported with its delta against the base. Soft: it warns when the index
   got worse and never fails the run.
"""
import argparse
import os
import local_prototypes
import data_check
import cleanup_index
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
ASM_EXCEPTIONS = ROOT / "tools" / "asm_exceptions.json"  # the only game-code asm functions (docs/CONTEXT-matching.md)


FN_HEAD_RE = re.compile(r"^(?![\s#}/])(?!(?:typedef|struct|union|enum|return|if|for|while|switch)\b)[^;=]*?\b([A-Za-z_]\w*)\s*\([^;]*$")


def enclosing_function(lines, idx):
    """The function whose body holds lines[idx] (0-based): the nearest column-0 function header at or above it, or None if
    a column-0 `}` (the end of an earlier function) comes first, i.e. the line is at file scope."""
    for i in range(idx, -1, -1):
        line = lines[i]
        if line.startswith("}"):
            return None
        m = FN_HEAD_RE.match(line)
        if m:
            return m.group(1)
    return None


def asm_exception(path, code, exceptions=None, func=None):
    """True when a new line mentioning asm is allowed by tools/asm_exceptions.json for that file: it declares a listed asm
    function (`asm static s32 f(`), or it sits inside a listed function (`func`, e.g. an `asm { ... }` block in an
    ordinary C function, the original-asm style of TP/TWW)."""
    import json
    if exceptions is None:
        exceptions = json.loads(ASM_EXCEPTIONS.read_text()) if ASM_EXCEPTIONS.exists() else []
    m = re.search(r"\basm\b[^(]*?\b(\w+)\s*\(", code)
    names = {n for n in (m.group(1) if m else None, func) if n}
    return any(e["file"] == path and e["function"] in names for e in exceptions)


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
    post, lineno = [], 0  # the changed file as it is now, and the post-image line number of the next added line
    for line in diff.splitlines():
        if line.startswith("+++ "):
            current = line[6:] if line.startswith("+++ b/") else None
            post = (ROOT / current).read_text(errors="replace").splitlines() if current and (ROOT / current).is_file() else []
            lineno = 1
            continue
        if line.startswith("@@"):
            hm = re.search(r"\+(\d+)", line)
            lineno = int(hm.group(1)) if hm else lineno
            continue
        if not line.startswith("+") or current is None:
            continue
        added = line[1:]
        lineno += 1
        if current == "configure.py":
            if added.strip() and "Object(" not in added and not allow_configure:
                problems.append(f"configure.py: only Object(...) lines may change (pass --allow-configure if intended): {added.strip()[:100]}")
            continue
        code = added.split("//", 1)[0]
        if ASM_RE.search(code) and not (allow_asm and current.startswith(UNPROTECTED)) \
                and not asm_exception(current, code, func=enclosing_function(post, lineno - 2) if post else None):
            problems.append(f"{current}: new inline asm: {added.strip()[:100]}"
                            + (" (--allow-asm: only for the ported libraries' hand-written assembly, checked against"
                               " the reference projects)" if current.startswith(UNPROTECTED) else
                               " (game code: only functions listed in tools/asm_exceptions.json, with Isaac's approval)"))
        if PRAGMA_RE.search(code) and not allow_pragma:
            problems.append(f"{current}: new #pragma (pass --allow-pragma if intended): {added.strip()[:100]}")
    sys.path.insert(0, str(ROOT / "tools"))
    import fncast_lint
    new, _ = fncast_lint.check(fncast_lint.find_all(), fncast_lint.load_allowlist())
    problems += [f"{f}:{n}: function-pointer cast outside #ifdef MUST_MATCH (undefined behaviour in the natural build;"
                 f" guard it with a plain C #else, see tools/fncast_lint.py): {ctx[:100]}" for f, n, ctx in new]
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


def data_objects(base, allowed):
    """The data objects the diff defines must hold the original object's bytes (objdiff scores a data relocation by
    its symbol, so a wrong table value is invisible to the ratchet)."""
    problems, count = data_check.check(base, set(allowed))
    if problems:
        fail("data", "\n  " + "\n  ".join(problems) + "\n  (--allow-data-mismatch NAME excuses an object)")
    print(f"ok   [data] {count} changed data object(s) hold the original's bytes")


def hang_guard_check():
    """Sources that make the compiler hang (tools/hang_guard.py) fail here, whatever --allow-pragma says."""
    sys.path.insert(0, str(ROOT / "tools"))
    import hang_guard
    bad = hang_guard.problems(hang_guard.all_sources())
    if bad:
        fail("hang_guard", "\n  " + "\n  ".join(bad))
    print("ok   [hang_guard] no source the compiler is known to hang on")


def build():
    r = run([sys.executable, "configure.py"])
    if r.returncode:
        fail("build", "configure.py failed:\n" + r.stderr[-2000:])
    ninja = shutil.which("ninja") or str(ROOT / ".venv" / "bin" / "ninja")
    # Several sessions share one Mac: cap parallel compiles (CI_JOBS, default 4) and run them at low priority.
    jobs = os.environ.get("CI_JOBS", "4")
    r = run(["nice", "-n", "10", ninja, "-j", jobs])
    if r.returncode:
        fail("build", "ninja failed (compile error, or a linked module no longer matches build.sha1):\n"
             + (r.stdout + r.stderr)[-3000:])
    print("ok   [build] ninja clean; linked modules match build.sha1")


def natural(base):
    """Changed C files must also compile without MUST_MATCH (rule 7: the #else holds the natural code)."""
    import tempfile
    names = [n for n in run(["git", "diff", "--name-only", base]).stdout.split() if n.endswith(".c") and (ROOT / n).exists()]
    ninja = shutil.which("ninja") or str(ROOT / ".venv" / "bin" / "ninja")
    targets = {}
    for line in run([ninja, "-t", "targets", "all"]).stdout.splitlines():
        t = line.split(":")[0]
        if t.endswith(".o") and "/src/" in t:
            targets.setdefault(t.split("/src/", 1)[1].rsplit(".", 1)[0], t)
    problems = []
    with tempfile.TemporaryDirectory() as tmp:
        for name in names:
            stem = name.split("/src/", 1)[1] if "/src/" in name else name.removeprefix("src/")
            target = targets.get(stem.rsplit(".", 1)[0])
            if not target:
                continue
            cmd = run([ninja, "-t", "commands", target]).stdout.strip().splitlines()[-1]
            cmd = cmd.split(" && ")[0].replace(" -DMUST_MATCH", "")
            cmd = cmd.replace(" -o " + target.rsplit("/", 1)[0], " -o " + tmp).replace(" -MMD", "")
            r = subprocess.run(cmd, shell=True, cwd=ROOT, text=True, capture_output=True)
            if r.returncode:
                problems.append(f"{name}: doesn't compile without MUST_MATCH:\n" + (r.stdout + r.stderr)[-800:])
    if problems:
        fail("natural", "\n  " + "\n  ".join(problems))
    print(f"ok   [natural] {len(names)} changed C file(s) compile without MUST_MATCH")


def moved_functions(worse, report):
    """The `worse` entries that are a function moved between units: it scores 0 under its old unit (gone from it) and
    another unit of the current report has a function of the same name scoring at least the old score. Returns
    [(entry, new_unit, new_score)]."""
    scores = {}
    for u in report.get("units") or []:
        for f in u.get("functions") or []:
            scores.setdefault(f["name"], []).append((u["name"], f.get("fuzzy_match_percent") or 0.0))
    out = []
    for w in worse:
        name, key, a, b = w
        if key != "fuzzy_match_percent" or ":" not in name or b != 0:
            continue
        unit, fn = name.split(":", 1)
        best = max(((u, sc) for u, sc in scores.get(fn, []) if u != unit and sc >= a), key=lambda x: x[1], default=None)
        if best:
            out.append((w, best[0], best[1]))
    return out


def renamed_functions(worse, changes):
    """The `worse` entries that are a function renamed in place (config/G8MJ01/symbols.txt and the source together):
    gone from its unit, and the same unit gained a function at the same address with at least the old score.
    Returns [(entry, new_name, new_score)]."""
    gained = {}
    for u in changes.get("units") or []:
        for f in u.get("functions") or []:
            if "to" in f and "from" not in f:
                addr = (f.get("metadata") or {}).get("virtual_address")
                if addr is not None:
                    gained[(u["name"], addr)] = (f["name"], f["to"].get("fuzzy_match_percent") or 0.0)
    lost = {}
    for u in changes.get("units") or []:
        for f in u.get("functions") or []:
            if "from" in f and "to" not in f:
                lost[f"{u['name']}:{f['name']}"] = (u["name"], (f.get("metadata") or {}).get("virtual_address"))
    out = []
    for w in worse:
        name, key, a, b = w
        if key != "fuzzy_match_percent" or name not in lost or b != 0 or lost[name][1] is None:
            continue
        new = gained.get(lost[name])
        if new and new[1] >= a:
            del gained[lost[name]]  # one successor excuses one lost function
            out.append((w, new[0], new[1]))
    return out


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
    # Data sections of units that aren't linked yet: `changes` leaves them out, and a function can reach 100% while
    # the unit's .rodata/.sdata2 gets worse (e.g. a named constant left behind next to the literal that replaced it).
    def sections(report):
        scores, units = {}, set()
        for u in json.loads(report.read_text()).get("units") or []:
            if (u.get("metadata") or {}).get("complete"):
                continue  # linked: the checksum is the check, and some linked units have imperfect data scores
            units.add(u["name"])
            seen = {}
            for s in u.get("sections") or []:
                n = seen[s["name"]] = seen.get(s["name"], 0) + 1
                if s["name"] != ".text":  # objdiff omits the score when it's 0
                    scores[(u["name"], s["name"] + (f"#{n}" if n > 1 else ""))] = s.get("fuzzy_match_percent") or 0.0
        return scores, units

    (before, units_before), (after, units_now) = sections(ACCEPTED), sections(current)
    for key in before.keys() | after.keys():
        if key[0] not in units_now or key[0] not in units_before:
            continue  # unit linked, removed or new: the checksum and the unit measures cover it
        a, b = before.get(key, 0.0), after.get(key, 0.0)
        (worse if b < a else better if b > a else []).append((f"{key[0]}[{key[1]}]", "fuzzy_match_percent", a, b))
    for w, new, score in renamed_functions(worse, data):
        print(f"     renamed: {w[0]} -> {new} ({w[2]:.2f} -> {score:.2f})")
        worse.remove(w)
    for w, unit, score in moved_functions(worse, json.loads(current.read_text())):
        print(f"     moved: {w[0].split(':', 1)[1]} {w[0].split(':', 1)[0]} -> {unit} ({w[2]:.2f} -> {score:.2f})")
        worse.remove(w)
    if allowed:
        # A name is a function (its fuzzy drop, while below 100%, excuses its unit's and the total's fuzzy drop)
        # or a unit path such as MarioSt/event/evt_eff (any of its measures, and the total's same measure).
        named = [w for w in worse if (w[0].split(":")[-1] in allowed and w[2] < 100) or w[0] in allowed
                 or w[0].split("[")[0] in allowed]
        units = {n.split(":", 1)[0] for n, _, _, _ in named if "[" not in n}
        keys = {k for n, k, _, _ in named if ":" not in n and "[" not in n}
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


def cleanup(base):
    """Soft ratchet: the cleanup index and its delta against base. Warns when it got worse; never fails."""
    try:
        now, before = cleanup_index.index(), cleanup_index.index(base)
    except subprocess.CalledProcessError as e:
        print(f"warn [cleanup] could not measure against {base}: {(e.stderr or b'').decode(errors='replace')[-200:]}")
        return
    delta = now["index"] - before["index"]
    print(f"ok   [cleanup] index {now['index']:.3f} per kLOC ({delta:+.3f} vs {base}; lower is better)")
    if delta > 0.0005:
        print(f"warn [cleanup] the cleanup index got worse vs {base}; worst files: python3 tools/cleanup_index.py --top 10")


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
    ap.add_argument("--allow-data-mismatch", action="append", default=[], metavar="NAME",
                    help="a data object whose bytes may differ from the original's; repeatable")
    ap.add_argument("--allow-regress", action="append", default=[], metavar="NAME",
                    help="a not-yet-matched function, or a unit, whose score may drop when wrong code is fixed or "
                         "removed; repeatable")
    a = ap.parse_args()
    CI_DIR.mkdir(parents=True, exist_ok=True)
    if a.init:
        hang_guard_check()
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
    hang_guard_check()
    build()
    natural(a.base)
    data_objects(a.base, a.allow_data_mismatch)
    ratchet(set(a.allow_regress))
    summary()
    cleanup(a.base)
    unlinked()
    if a.accept:
        shutil.copy(BUILD / "report.json", ACCEPTED)
        print("     accepted: this build is the new baseline")
    print("PASS")


if __name__ == "__main__":
    main()
