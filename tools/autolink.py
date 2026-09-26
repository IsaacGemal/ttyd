#!/usr/bin/env python3
"""Link every file that is fully matched but still marked NonMatching in configure.py.

For each candidate (100% code in build/G8MJ01/report.json), flip its Object from NonMatching to Matching,
rebuild, and keep the flip only if the build's checksums still pass. Candidates that fail are listed with the
reason: the report can show 100% while something it doesn't measure still differs. Data isn't required to be
100%: item_data and npc_data linked with byte-identical output while the report scored their data at 41% and
66%, so the checksums decide.

  python3 tools/autolink.py            try every candidate
  python3 tools/autolink.py SRC...     try only these (paths as in configure.py, e.g. driver/swdrv.c)
  python3 tools/autolink.py --list     list candidates without building

Run on a clean tree after a passing build. Leaves configure.py with the kept flips; commit them yourself.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CONFIGURE = ROOT / "configure.py"
REPORT = ROOT / "build/G8MJ01/report.json"


def objects():
    """(module, source path, Matching?) for every Object in configure.py. Objects inside a Rel("name", [...])
    block belong to that REL module; all others to MarioSt, as in the report's unit names."""
    text = CONFIGURE.read_text()
    rels = [(m.start(), m.end(), m.group(1)) for m in
            re.finditer(r'Rel\(\s*"(\w+)",\s*\[.*?\]\s*,?\s*\)', text, re.S)]
    out = []
    for m in re.finditer(r'Object\((NonMatching|Matching), "([^"]+)"', text):
        module = next((name for a, b, name in rels if a <= m.start() < b), "MarioSt")
        out.append((module, m.group(2), m.group(1) == "Matching"))
    return out


def candidates():
    units = {u["name"]: u for u in json.loads(REPORT.read_text())["units"]}
    out = []
    for module, src, matching in objects():
        u = units.get(f"{module}/{src.rsplit('.', 1)[0]}")
        if not matching and u and u.get("measures", {}).get("matched_code_percent") == 100:
            out.append(src)
    return out


def build():
    subprocess.run([sys.executable, "configure.py"], cwd=ROOT, capture_output=True, check=True)
    r = subprocess.run(["ninja"], cwd=ROOT, capture_output=True, text=True)
    if r.returncode == 0:
        return None
    out = r.stdout + r.stderr
    bad = [l.split(":")[0].split("/")[-1] for l in out.splitlines() if l.endswith(": FAILED")]
    if bad:
        return f"checksum mismatch in {', '.join(bad[:4])}{' ...' if len(bad) > 4 else ''}"
    err = next((l.strip() for l in out.splitlines() if "rror" in l), "build failed")
    return err[:120]


def set_matching(src, on):
    text = CONFIGURE.read_text()
    a, b = f'Object(NonMatching, "{src}"', f'Object(Matching, "{src}"'
    if text.count(f'"{src}"') != 1:
        sys.exit(f"{src} is listed {text.count(chr(34) + src + chr(34))} times in configure.py; flip it by hand")
    CONFIGURE.write_text(text.replace(a, b, 1) if on else text.replace(b, a, 1))


def main():
    args = sys.argv[1:]
    todo = candidates() if not args or args == ["--list"] else args
    if args == ["--list"]:
        print("\n".join(todo))
        return
    linked = []
    for src in todo:
        set_matching(src, True)
        err = build()
        if err:
            set_matching(src, False)
            print(f"FAIL   {src}: {err}")
        else:
            linked.append(src)
            print(f"LINKED {src}")
    if build():
        sys.exit("final build failed; check configure.py")
    print(f"{len(linked)} linked, {len(todo) - len(linked)} failed")


if __name__ == "__main__":
    main()
