#!/usr/bin/env python3
"""tools/lint_patch.py UNIT [BASE]: review checks on an agent's change to src/UNIT.c, after a build. UNIT may
also be a source path (rels/<rel>/src/... for REL modules).

BASE is a git ref (default HEAD) or, where there is no git (an agent's workspace), a path to the file to compare
against (`/dev/null` for a new file).

The checks reviewers kept flagging, run mechanically so reviews can look for new problems instead:
  STUB       a function the patch left empty or near-empty, below 100%, that wasn't already empty (rejects the file)
  UNLABELLED a function the patch touched that isn't at 100% and has no `// NONMATCHING` comment above it
  OFFSET     a raw offset cast like *(T*)((u8*)p + 0x..) added by the patch
  STRUCT     a struct/typedef defined in the .c file by the patch (it may duplicate a header)
  EXTERN     an extern declaration added in the .c file (it may belong in a header)
  PRAGMA     an added `dont_inline on` / `opt_propagation off` / `optimize_for_size on` with no reason comment
Exit 1 if any STUB, else 0. Everything else is printed for the merge and the PR description.
"""
import json
import os
import re
import subprocess
import sys


def definitions(text):
    """Function definitions: name -> (start line, body text)."""
    funcs = {}
    for m in re.finditer(r"^(?:(?:static\s+)?USER_FUNC\(\s*(\w+)\s*\)|[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{)]*\))\s*\{", text,
                         re.M):
        name, start, depth, i = m.group(1) or m.group(2), m.end(), 1, m.end()
        while depth and i < len(text):
            depth += {"{": 1, "}": -1}.get(text[i], 0)
            i += 1
        if name not in ("if", "while", "for", "switch"):
            funcs[name] = (text.count("\n", 0, m.start()), text[start:i - 1])
    return funcs


def is_stub(body):
    code = re.sub(r"//[^\n]*|/\*.*?\*/", "", body, flags=re.S).strip()
    return code in ("", "return;", "return 0;", "return NULL;", "return FALSE;")




def source_path(unit):
    """(source path, report unit name) for UNIT: src/UNIT.c in MarioSt, or a path to a .c file, which may be a
    REL module's source (rels/<rel>/src/UNIT.c, reported as <rel>/UNIT)."""
    if unit.endswith(".c"):
        m = re.fullmatch(r"(?:rels/([^/]+)/)?src/(.+)\.c", unit)
        if not m:
            sys.exit(f"{unit}: expected src/... or rels/<rel>/src/...")
        return unit, f"{m.group(1) or 'MarioSt'}/{m.group(2)}"
    return f"src/{unit}.c", f"MarioSt/{unit}"


def base_file(base):
    """BASE is a git ref when git knows it (refs such as origin/main contain a slash too), otherwise a file
    (/dev/null for a new file)."""
    is_ref = subprocess.run(["git", "rev-parse", "--verify", "--quiet", base + "^{commit}"],
                            capture_output=True).returncode == 0
    if is_ref:
        return None
    if not os.path.exists(base):
        sys.exit(f"{base}: neither a git ref nor a file")
    return base


def main():
    unit = sys.argv[1]
    base = sys.argv[2:] or []
    path, report_name = source_path(unit)
    BASE_FILE = base_file(base[0]) if base else None
    if BASE_FILE:
        diff = subprocess.run(["diff", "-U0", BASE_FILE, path], capture_output=True, text=True).stdout
    else:
        diff = subprocess.run(["git", "diff", "-U0", *base, "--", path], capture_output=True, text=True).stdout
    added = [l[1:] for l in diff.splitlines() if l.startswith("+") and not l.startswith("+++")]
    text = open(path).read()
    lines = text.splitlines()

    funcs = definitions(text)
    head = (open(BASE_FILE).read() if BASE_FILE else
            subprocess.run(["git", "show", f"{(base or ['HEAD'])[0]}:{path}"], capture_output=True, text=True).stdout)
    head_funcs = {n: body for n, (_, body) in definitions(head).items()}

    # Which functions the patch touched: a hunk's new-file line range intersecting the function.
    touched_lines = set()
    for m in re.finditer(r"^@@ -\S+ \+(\d+)(?:,(\d+))? @@", diff, re.M):
        s, n = int(m.group(1)), int(m.group(2) or 1)
        touched_lines.update(range(s - 1, s - 1 + max(n, 1)))
    touched = {n for n, (s, body) in funcs.items() if touched_lines & set(range(s, s + body.count("\n") + 2))}

    report = json.load(open("build/G8MJ01/report.json"))
    u = next(x for x in report["units"] if x["name"] == report_name)
    pct = {f["name"]: f.get("fuzzy_match_percent", 0) for f in u.get("functions", [])}

    stubs, out = [], []
    for n in sorted(touched):
        s, body = funcs[n]
        if is_stub(body) and pct.get(n, 100) < 100 and not is_stub(head_funcs.get(n, "x")):
            stubs.append(n)
        elif pct.get(n, 100) < 100 and not any("NONMATCHING" in l for l in lines[max(0, s - 3):s + 1]):
            out.append(f"UNLABELLED {n} ({pct[n]:.1f}%)")
    for l in added:
        if re.search(r"\*\s*\(\s*\w+\s*\*\s*\)\s*\(\s*\(\s*(u8|s8|char)\s*\*\s*\)", l):
            out.append("OFFSET     " + l.strip())
        if re.match(r"\s*(typedef\s+)?struct\s+\w*\s*\{|\s*typedef\s+(struct|union|enum)\b", l):
            out.append("STRUCT     " + l.strip())
        if re.match(r"\s*extern\b", l):
            out.append("EXTERN     " + l.strip())
    for i, l in enumerate(lines):
        if i in touched_lines and re.match(r"\s*#\s*pragma\s+(dont_inline\s+on|opt_propagation\s+off|opt_common_subs\s+off|optimize_for_size\s+on)", l):
            if not any("//" in c or "/*" in c for c in lines[max(0, i - 2):i + 1]):
                out.append(f"PRAGMA     line {i + 1}: {l.strip()}")
    for n in stubs:
        print(f"STUB       {n}")
    print("\n".join(out))
    sys.exit(1 if stubs else 0)



if __name__ == "__main__":
    main()
