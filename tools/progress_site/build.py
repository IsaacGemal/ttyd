#!/usr/bin/env python3
"""Build a one-page progress site from the objdiff report, in the style of decomp.dev.

    python3 tools/progress_site/build.py                  write build/progress/index.html
    python3 tools/progress_site/build.py --record "Wave 17"   also append the current build to history.json
    python3 tools/progress_site/build.py --serve [PORT]   write, then serve build/progress/

Reads build/G8MJ01/report.json (run `ninja` or tools/ci.py first) and history.json next to this file.
The page is self-contained: the data is embedded in it. fragment.html is the same page without the
document skeleton, for hosts that add their own.
"""
import argparse
import datetime
import functools
import http.server
import json
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
REPORT = ROOT / "build" / "G8MJ01" / "report.json"
HISTORY = HERE / "history.json"
OUT = ROOT / "build" / "progress"


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, text=True, capture_output=True).stdout.strip()


def group_of(name, cats):
    parts = name.split("/")
    if parts[0] != "MarioSt":
        return "REL: " + parts[0]
    if len(parts) > 2:
        sub = parts[1]
        if sub.endswith(".a"):
            sub = sub.split(".")[0]
        return sub
    return "sdk (root)" if "sdk" in cats else "game (root)"


def measures(m):
    return {k: float(v) for k, v in m.items()}


def build_data(report):
    units = []
    for u in report["units"]:
        m = u["measures"]
        code = int(m.get("total_code", 0))
        if not code:
            continue
        meta = u.get("metadata", {})
        cats = meta.get("progress_categories", [])
        cat = "modules" if "modules" in cats else next((c for c in cats if c in ("game", "sdk", "musyx")), "dol")
        fns = [[f["name"], int(f.get("size", 0)), round(f.get("fuzzy_match_percent", 0.0), 1)]
               for f in u.get("functions", [])]
        units.append({
            "n": u["name"],
            "g": group_of(u["name"], cats),
            "c": cat,
            "src": meta.get("source_path", ""),
            "code": code,
            "fz": round(m.get("fuzzy_match_percent", 0.0), 2),
            "mc": round(m.get("matched_code_percent", 0.0), 2),
            "mf": m.get("matched_functions", 0),
            "tf": m.get("total_functions", 0),
            "done": bool(meta.get("complete")),
            "fn": fns,
        })
    return {
        "total": measures(report["measures"]),
        "categories": [{"id": c["id"], "name": c["name"], "m": measures(c["measures"])}
                       for c in report.get("categories", [])],
        "units": units,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--record", metavar="LABEL", help="append the current build to history.json under LABEL")
    ap.add_argument("--serve", nargs="?", const=8047, type=int, metavar="PORT")
    args = ap.parse_args()

    report = json.loads(REPORT.read_text())
    history = json.loads(HISTORY.read_text())
    total = report["measures"]
    commit = git("rev-parse", "--short", "HEAD")

    if args.record:
        history.append({
            "label": args.record,
            "date": datetime.datetime.now().strftime("%Y-%m-%dT%H:%M"),
            "functions": total["matched_functions"],
            "code": round(total["matched_code_percent"], 2),
            "commit": commit,
        })
        HISTORY.write_text(json.dumps(history, indent=2) + "\n")

    data = build_data(report)
    data["history"] = history
    data["commit"] = commit
    data["dirty"] = bool(git("status", "--porcelain", "--untracked-files=no"))
    data["built"] = datetime.datetime.fromtimestamp(REPORT.stat().st_mtime).strftime("%Y-%m-%d %H:%M")

    blob = json.dumps(data, separators=(",", ":")).replace("</", "<\\/")
    html = (HERE / "template.html").read_text().replace("/*DATA*/null", blob)
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / "fragment.html").write_text(html)
    head = '<!doctype html>\n<html lang="en"><head><meta charset="utf-8">\n' \
           '<meta name="viewport" content="width=device-width, initial-scale=1">\n'
    (OUT / "index.html").write_text(head + html.replace("<div class=\"wrap\">", "</head><body>\n<div class=\"wrap\">", 1)
                                     + "\n</body></html>\n")
    print(f"wrote {OUT / 'index.html'} ({len(html) // 1024} KB, {len(data['units'])} units)")

    if args.serve:
        handler = functools.partial(http.server.SimpleHTTPRequestHandler, directory=str(OUT))
        print(f"serving http://localhost:{args.serve}/")
        http.server.ThreadingHTTPServer(("127.0.0.1", args.serve), handler).serve_forever()


if __name__ == "__main__":
    main()
