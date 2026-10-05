"""Find function-pointer casts of named functions outside `#ifdef MUST_MATCH`.

    python3 tools/fncast_lint.py            list unguarded casts not in tools/fncast_allowlist.json (exit 1 if any)
    python3 tools/fncast_lint.py --stale    also list allowlist entries that no longer match a cast

A call through `((void (*)(Vec, Vec, GXColor))winIconSet)` reproduces the original's argument passing (by-value
caller copies, or a prototype that disagrees with the header), but calling a function through an incompatible type is
undefined behaviour. Match-only shapes belong under `#ifdef MUST_MATCH` with a plain C `#else`, so the natural build is
well-defined. tools/ci.py runs this as part of its lint step: only casts missing from the allowlist fail. The
allowlist (file, the stripped source line, reason) holds the pre-existing sites and doubles as the audit list; fixing
one makes its entry stale, and --stale shows those so they can be removed."""
import collections
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALLOWLIST = os.path.join(ROOT, "tools", "fncast_allowlist.json")
DIRS = ("src", "rels", "include")
CAST = re.compile(r"\(\(\w[\w\s\*]*\(\*\)\([^)]*\)\)\s*\w+\)")
COND = re.compile(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)")


def scan(text):
    """[(line number, stripped line)] for each line with a function-pointer cast of a name, outside MUST_MATCH."""
    out = []
    stack = []  # per open conditional: True while its active branch is MUST_MATCH-only
    for i, line in enumerate(text.split("\n"), 1):
        m = COND.match(line)
        if m:
            kind, rest = m.group(1), m.group(2)
            if kind in ("if", "ifdef", "ifndef"):
                mm = "MUST_MATCH" in rest
                negated = kind == "ifndef" or re.search(r"!\s*defined\s*\(?\s*MUST_MATCH", rest)
                stack.append([mm and not negated, mm and bool(negated)])
            elif kind == "elif" and stack:
                stack[-1][0] = False
            elif kind == "else" and stack:
                stack[-1][0] = stack[-1][1]
            elif kind == "endif" and stack:
                stack.pop()
            continue
        code = line.split("//", 1)[0]
        if CAST.search(code) and not any(s[0] for s in stack):
            out.append((i, line.strip()))
    return out


def sources():
    for d in DIRS:
        for dirpath, _, files in os.walk(os.path.join(ROOT, d)):
            for f in sorted(files):
                if f.endswith((".c", ".h", ".cpp")):
                    yield os.path.relpath(os.path.join(dirpath, f), ROOT)


def find_all():
    """{file: [(line, stripped line)]} over the game sources."""
    found = {}
    for path in sources():
        hits = scan(open(os.path.join(ROOT, path), errors="replace").read())
        if hits:
            found[path] = hits
    return found


def check(found, allow):
    """(new, stale): casts with no allowlist entry, and entries with no cast left to match."""
    want = collections.Counter((e["file"], e["context"]) for e in allow)
    have = collections.Counter((f, ctx) for f, hits in found.items() for _, ctx in hits)
    extra = have - want
    new = []
    for f, hits in sorted(found.items()):
        for line, ctx in hits:
            if extra[(f, ctx)]:
                extra[(f, ctx)] -= 1
                new.append((f, line, ctx))
    return new, sorted((want - have).elements())


def load_allowlist(path=ALLOWLIST):
    return json.load(open(path)) if os.path.exists(path) else []


def main(argv):
    new, stale = check(find_all(), load_allowlist())
    for f, line, ctx in new:
        print(f"{f}:{line}: unguarded function-pointer cast: {ctx[:110]}")
    if "--stale" in argv:
        for f, ctx in stale:
            print(f"{f}: stale allowlist entry: {ctx[:110]}")
    return 1 if new else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
