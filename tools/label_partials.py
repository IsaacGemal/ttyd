#!/usr/bin/env python3
"""Keep `// NONMATCHING (NN%)` labels honest in a source file, from the current build.

  python3 tools/label_partials.py UNIT [--reset FUNC ...] [--note FUNC "text" ...]

UNIT is the source path without src/ and .c. For every function in the file below 100%:
- with no label: adds `// NONMATCHING (NN.NN%): <summary>` directly above it;
- with a label: rewrites only its percentage;
- listed after --reset: replaces the whole label with a fresh summary (for descriptions known to be false);
- listed after --note: appends "Known issue: <text>" to its label.
A function at 100% that still carries a label has the label removed. The summary comes from the instruction
diff: "only register numbers differ", or how many instructions are changed, missing or extra.
Run after a build; check the file still builds afterwards.
"""
import json
import re
import subprocess
import sys

DEF = r"^(?:USER_FUNC\(\s*{name}\s*\)|[A-Za-z_][\w \*]*?\b{name}\s*\([^;{{)]*\))\s*\{{"


def diff_summary(unit, func):
    subprocess.run(["build/tools/objdiff-cli", "diff", "-p", ".", "-u", "MarioSt/" + unit, func, "-o",
                    "build/_od.json", "--format", "json"], capture_output=True)
    try:
        d = json.load(open("build/_od.json"))
    except Exception:
        return "differs from the original"
    kinds = {}
    for side in ("left", "right"):
        for sym in d.get(side, {}).get("symbols", []):
            if sym.get("name") != func:
                continue
            for ins in sym.get("instructions", []):
                k = ins.get("diff_kind")
                if k and side == "left":
                    kinds[k] = kinds.get(k, 0) + 1
    if not kinds:
        return "differs from the original"
    if set(kinds) <= {"DIFF_ARG_MISMATCH"}:
        return "same instructions; only register numbers or operands differ"
    parts = []
    changed = kinds.get("DIFF_REPLACE", 0) + kinds.get("DIFF_OP_MISMATCH", 0)
    if changed:
        parts.append(f"{changed} instruction{'s' if changed != 1 else ''} differ{'' if changed != 1 else 's'}")
    if kinds.get("DIFF_DELETE"):
        parts.append(f"{kinds['DIFF_DELETE']} missing")
    if kinds.get("DIFF_INSERT"):
        parts.append(f"{kinds['DIFF_INSERT']} extra")
    if kinds.get("DIFF_ARG_MISMATCH"):
        parts.append("plus register differences")
    return ", ".join(parts)


def main():
    unit = sys.argv[1]
    args = sys.argv[2:]
    reset, notes = set(), {}
    while args:
        flag = args.pop(0)
        if flag == "--reset":
            while args and not args[0].startswith("--"):
                reset.add(args.pop(0))
        elif flag == "--note":
            func = args.pop(0)
            notes[func] = args.pop(0)
    path = f"src/{unit}.c"
    report = json.load(open("build/G8MJ01/report.json"))
    u = next(x for x in report["units"] if x["name"] == "MarioSt/" + unit)
    pct = {f["name"]: f.get("fuzzy_match_percent", 0) for f in u.get("functions", [])}
    lines = open(path).read().split("\n")
    changed = 0
    for name, p in sorted(pct.items()):
        text = "\n".join(lines)
        m = re.search(DEF.format(name=re.escape(name)), text, re.M)
        if not m:
            continue
        at = text.count("\n", 0, m.start())
        # an existing label: NONMATCHING comment lines directly above (possibly with following // lines)
        top = at
        while top > 0 and lines[top - 1].lstrip().startswith(("//", "#pragma")):
            top -= 1
        label = next((i for i in range(top, at) if "NONMATCHING" in lines[i]), None)
        if p >= 100:
            if label is not None:
                end = label + 1
                while end < at and lines[end].lstrip().startswith("//") and "NONMATCHING" not in lines[end] \
                        and not lines[end].lstrip().startswith("// Keep") and "pragma" not in lines[end]:
                    end += 1
                del lines[label:end]
                changed += 1
            continue
        note = f" Known issue: {notes[name]}" if name in notes else ""
        if note and label is not None and name not in reset and "Known issue:" in lines[label]:
            note = ""
        if label is None or name in reset:
            if label is not None:
                end = label + 1
                while end < at and lines[end].lstrip().startswith("//") and not lines[end].lstrip().startswith("// Keep"):
                    end += 1
                del lines[label:end]
                at -= end - label
                top = label
            new = f"// NONMATCHING ({p:.2f}%): {diff_summary(unit, name)}.{note}"
            lines.insert(top if label is not None else at, new)
        else:
            lines[label] = re.sub(r"NONMATCHING \([^)]*\)", f"NONMATCHING ({p:.2f}%)", lines[label]) + note
        changed += 1
    open(path, "w").write("\n".join(lines))
    print(f"{path}: {changed} labels written")


if __name__ == "__main__":
    main()
