#!/usr/bin/env python3
"""Check that the docs/CONTEXT.md split lost, changed and reordered nothing.

    python3 tools/check_context_split.py                 original = origin/main's docs/CONTEXT.md
    python3 tools/check_context_split.py ORIGINAL.md     original = this file
    python3 tools/check_context_split.py --rev REV       original = REV's docs/CONTEXT.md
    ... --allow-appended                                 allow chapters added to CONTEXT.md after the split

The original must be an unsplit CONTEXT.md. Sections are split on "## " headings outside code fences (a "## " line
inside ``` or ~~~ is not a heading); the preamble is the text before the first one. Checks:
  (a) every non-blank line of the original appears in the new pair at least as often as in the original;
  (b) every original section appears unchanged, as one contiguous section, in exactly one of the two files;
  (c) the sections in each file are in the original's order: CONTEXT-matching.md holds the chapters before the
      boundary, CONTEXT.md holds "Current state", "Parked ideas" and the chapters from the boundary on.
Also: the boundary is the chapter BOUNDARY, and every link in "Standing rules" resolves to a heading anchor.

Allowed differences (ALLOWED below, and nothing else):
  - CONTEXT-matching.md's header (its text before the first "## " heading);
  - the preamble lines in PREAMBLE_REPLACED (original -> new) and PREAMBLE_ADDED;
  - the "Current state" lines in CURRENT_REPLACED (original -> new);
  - the "Parked ideas" bullet starting with PARKED_MOVED, which must be gone from "Parked ideas" (the rest of that
    section unchanged) and appear verbatim in the split chapter;
  - the new sections in NEW_SECTIONS ("Standing rules" and the split chapter);
  - blank lines at the end of a section (the separators between sections);
  - with --allow-appended, chapters after the split chapter in CONTEXT.md.
"""
import argparse
import collections
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DOCS = ROOT / "docs"

PREAMBLE_REPLACED = {
    'made the call. The "Current state" section is the only part that gets rewritten in place; everything below it is':
        'made the call. The "Current state" and "Standing rules" sections are the only parts that get rewritten in place;',
    "append-only history (correct an old chapter by adding a new one that supersedes it, and say so).":
        "everything else is append-only history (correct an old chapter by adding a new one that supersedes it, and say so).",
}
PREAMBLE_ADDED = [
    "",
    "The matching-era chapters (2026-09-24 to 2026-10-18) are in `docs/CONTEXT-matching.md`, which is frozen; new",
    "chapters still go at the bottom of this file. Search both: `grep -n PATTERN docs/CONTEXT*.md`.",
]
CURRENT_REPLACED = {
    '- New orchestrator session? Read "The orchestrator\'s role: how to pick this up" (near the end) first, then the':
        '- New orchestrator session? Read "The orchestrator\'s role: how to pick this up" (in `docs/CONTEXT-matching.md`)'
        ' first, then the',
}
PARKED_MOVED = "- **Split this file at 100% matched** (parked 2026-10-20"
BOUNDARY = "Leftovers found while playing the matched build"
NEW_SECTIONS = ("Standing rules", "CONTEXT.md split at 100% matched (2026-10-21")
ALLOWED = {
    "matching header": "CONTEXT-matching.md text before its first '## ' heading",
    "preamble replaced": PREAMBLE_REPLACED,
    "preamble added": PREAMBLE_ADDED,
    "current state replaced": CURRENT_REPLACED,
    "parked idea moved to the split chapter": PARKED_MOVED,
    "new sections": NEW_SECTIONS,
    "separators": "blank lines at the end of a section",
}
CURRENT, PARKED = "Current state", "Parked ideas"

FENCE_RE = re.compile(r"^\s*(```|~~~)")
HEADING_RE = re.compile(r"^(#{1,6}) +(.*?)(?:\s+#+)?\s*$")
LINK_RE = re.compile(r"(?m)^\[(\d+)\]: (\S*?\.md)?#(\S+)$")
USE_RE = re.compile(r"\]\[(\d+)\]")


def sections_of(text):
    """[(title or None, [lines])], fence-aware, trailing blank lines of each section dropped."""
    out, in_fence = [(None, [])], False
    for line in text.split("\n"):
        if FENCE_RE.match(line):
            in_fence = not in_fence
        elif not in_fence and line.startswith("## "):
            out.append((line[3:], []))
        out[-1][1].append(line)
    for _, lines in out:
        while lines and lines[-1] == "":
            lines.pop()
    return out


def anchors_of(text):
    seen, out, in_fence = {}, set(), False
    for line in text.split("\n"):
        if FENCE_RE.match(line):
            in_fence = not in_fence
            continue
        m = None if in_fence else HEADING_RE.match(line)
        if m:
            base = re.sub(r"[^\w\- ]", "", m.group(2).strip().lower()).replace(" ", "-")
            n = seen.get(base, 0)
            seen[base] = n + 1
            out.add(base if n == 0 else f"{base}-{n}")
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("original", nargs="?")
    ap.add_argument("--rev", default="origin/main")
    ap.add_argument("--docs", default=str(DOCS))
    ap.add_argument("--allow-appended", action="store_true")
    args = ap.parse_args()

    if args.original:
        orig_text, src = Path(args.original).read_text(), args.original
    else:
        orig_text = subprocess.run(["git", "-C", str(ROOT), "show", f"{args.rev}:docs/CONTEXT.md"],
                                   check=True, capture_output=True, text=True).stdout
        src = f"{args.rev}:docs/CONTEXT.md"
    docs = Path(args.docs)
    match_text = (docs / "CONTEXT-matching.md").read_text()
    live_text = (docs / "CONTEXT.md").read_text()

    orig = sections_of(orig_text)
    if any(t and t.startswith(NEW_SECTIONS[0]) for t, _ in orig):
        sys.exit(f"{src} already has '## {NEW_SECTIONS[0]}': pass the unsplit original (a path or --rev)")
    files = {"CONTEXT-matching.md": sections_of(match_text), "CONTEXT.md": sections_of(live_text)}
    failures = []

    def fail(msg):
        failures.append(msg)

    # Preamble: the new one minus the listed edits must be the original one.
    new_pre = list(files["CONTEXT.md"][0][1])
    n = len(PREAMBLE_ADDED)
    at = [i for i in range(len(new_pre) - n + 1) if new_pre[i:i + n] == PREAMBLE_ADDED]
    if len(at) == 1:
        del new_pre[at[0]:at[0] + n]
    else:
        fail(f"preamble: the added block appears {len(at)} times, expected once")
    back = {v: k for k, v in PREAMBLE_REPLACED.items()}
    new_pre = [back.get(l, l) for l in new_pre]
    if new_pre != orig[0][1]:
        fail("preamble: differs from the original beyond the listed edits")

    # Current state and Parked ideas: undo the listed edits, so the sections then match the original exactly.
    def bullet(lines):
        hits = [i for i, l in enumerate(lines) if l.startswith(PARKED_MOVED)]
        if len(hits) != 1:
            return None
        end = hits[0] + 1
        while end < len(lines) and lines[end].startswith("  "):
            end += 1
        return hits[0], end

    live_secs = files["CONTEXT.md"]
    orig_cur = next((l for t, l in orig if t and t.startswith(CURRENT)), None)
    orig_park = next((l for t, l in orig if t and t.startswith(PARKED)), None)
    split_lines = next((l for t, l in live_secs if t and t.startswith(NEW_SECTIONS[1])), [])
    for k, (title, lines) in enumerate(live_secs):
        if title and title.startswith(CURRENT) and orig_cur is not None:
            back_cur = {v: k2 for k2, v in CURRENT_REPLACED.items()}
            restored = [back_cur.get(l, l) for l in lines]
            if any(lines.count(v) != 1 for v in CURRENT_REPLACED.values()):
                fail("Current state: a listed replacement line is missing or repeated")
            elif restored == orig_cur:
                live_secs[k] = (title, restored)
        elif title and title.startswith(PARKED) and orig_park is not None:
            span = bullet(orig_park)
            if span is None:
                fail(f"Parked ideas: the original has no single bullet starting {PARKED_MOVED!r}")
                continue
            moved = orig_park[span[0]:span[1]]
            n = len(moved)
            if lines != orig_park[:span[0]] + orig_park[span[1]:]:
                fail("Parked ideas: differs from the original beyond the moved bullet")
            elif not any(split_lines[i:i + n] == moved for i in range(len(split_lines) - n + 1)):
                fail("Parked ideas: the moved bullet is not verbatim in the split chapter")
            else:
                live_secs[k] = (title, orig_park)

    # Map every new section (after the preamble) to an original section by its exact text.
    by_text = collections.defaultdict(collections.deque)
    for i, (_, lines) in enumerate(orig[1:], 1):
        by_text["\n".join(lines)].append(i)
    placed = {}  # original index -> (file, position)
    order = {}
    extras = {}
    for name, secs in files.items():
        order[name], extras[name] = [], []
        for pos, (title, lines) in enumerate(secs[1:], 1):
            q = by_text.get("\n".join(lines))
            if q:
                i = q.popleft()
                placed[i] = (name, pos)
                order[name].append(i)
            else:
                extras[name].append((pos, title))

    # (b) every original section in exactly one place.
    missing = [i for i in range(1, len(orig)) if i not in placed]
    for i in missing:
        fail(f"(b) section {i} '## {orig[i][0][:80]}' is not in either file unchanged")
    for name, secs in files.items():
        texts = collections.Counter("\n".join(l) for _, l in secs[1:])
        for t, n in texts.items():
            if n > 1 and t in {"\n".join(l) for _, l in orig[1:]}:
                want = sum(1 for _, l in orig[1:] if "\n".join(l) == t)
                if n > want:
                    fail(f"(b) {name}: section '{t.splitlines()[0][:80]}' appears {n} times, {want} in the original")
    both = set(order["CONTEXT-matching.md"]) & set(order["CONTEXT.md"])
    for i in sorted(both):
        fail(f"(b) section {i} is in both files")

    # Extras: only the allowed new sections.
    if files["CONTEXT-matching.md"][0][1] and not files["CONTEXT-matching.md"][0][1][0].startswith("# "):
        fail("CONTEXT-matching.md: header does not start with a '# ' title")
    for pos, title in extras["CONTEXT-matching.md"]:
        fail(f"CONTEXT-matching.md: unexpected section {pos} '## {title[:80]}'")
    live = files["CONTEXT.md"]
    split_pos = None
    for pos, title in extras["CONTEXT.md"]:
        if title.startswith(NEW_SECTIONS[0]) or title.startswith(NEW_SECTIONS[1]):
            if title.startswith(NEW_SECTIONS[1]):
                split_pos = pos
            continue
        if args.allow_appended and split_pos is not None and pos > split_pos:
            continue
        fail(f"CONTEXT.md: unexpected section {pos} '## {title[:80]}'")
    titles = [t for t, _ in live]
    if split_pos is None:
        fail(f"CONTEXT.md: no '## {NEW_SECTIONS[1]}...' chapter")
    elif any(placed.get(i, ("", 0))[0] == "CONTEXT.md" and placed[i][1] > split_pos for i in placed):
        fail("CONTEXT.md: original chapters after the split chapter")

    # (c) order within each file, and the layout around the boundary.
    for name, idx in order.items():
        if idx != sorted(idx):
            fail(f"(c) {name}: sections out of the original's order")
    cur = next((i for i, (t, _) in enumerate(orig) if t and t.startswith(CURRENT)), None)
    parked = next((i for i, (t, _) in enumerate(orig) if t and t.startswith(PARKED)), None)
    head = [t[:len(CURRENT)] if t else t for t in titles[1:4]]
    if not (titles[1] or "").startswith(CURRENT) or not (titles[2] or "").startswith(NEW_SECTIONS[0]) \
            or not (titles[3] or "").startswith(PARKED):
        fail(f"CONTEXT.md: expected Current state, Standing rules, Parked ideas first; found {head}")
    live_chapters = [i for i in order["CONTEXT.md"] if i not in (cur, parked)]
    boundary = min(live_chapters) if live_chapters else len(orig)
    if order["CONTEXT-matching.md"] and max(order["CONTEXT-matching.md"]) >= boundary:
        fail("(c) CONTEXT-matching.md holds a chapter from after the boundary")
    if cur in order["CONTEXT-matching.md"] or parked in order["CONTEXT-matching.md"]:
        fail("(c) Current state or Parked ideas is in CONTEXT-matching.md")
    if boundary < len(orig) and not (orig[boundary][0] or "").startswith(BOUNDARY):
        fail(f"(c) the first chapter in CONTEXT.md is not '## {BOUNDARY}...'")
    expect_matching = [i for i in range(1, boundary) if i not in (cur, parked)]
    if order["CONTEXT-matching.md"] != expect_matching:
        fail("(c) CONTEXT-matching.md is not exactly the chapters before the boundary")

    # (a) every non-blank original line present.
    have = collections.Counter(match_text.split("\n")) + collections.Counter(live_text.split("\n"))
    replaced = set(PREAMBLE_REPLACED) | set(CURRENT_REPLACED)
    need = collections.Counter(l for l in orig_text.split("\n") if l.strip() and l not in replaced)
    short = [(l, n, have[l]) for l, n in need.items() if have[l] < n]
    for l, n, h in short[:10]:
        fail(f"(a) line appears {h} times, {n} in the original: {l[:100]!r}")
    if len(short) > 10:
        fail(f"(a) ... and {len(short) - 10} more lines")

    # Standing rules links resolve.
    targets = {"CONTEXT-matching.md": anchors_of(match_text), "": anchors_of(live_text), "CONTEXT.md": anchors_of(live_text)}
    rules = "\n".join(live[2][1]) if len(live) > 2 else ""
    links = LINK_RE.findall(rules)
    for n, f, a in links:
        if a not in targets.get(f or "", set()):
            fail(f"Standing rules: link [{n}] {f}#{a} does not resolve")
    for n in sorted(set(USE_RE.findall(rules)) - {n for n, _, _ in links}, key=int):
        fail(f"Standing rules: reference [{n}] has no definition")

    n_orig = orig_text.count("\n")
    n_m, n_l = match_text.count("\n"), live_text.count("\n")
    print(f"original: {src}, {n_orig} lines, {len(orig) - 1} sections after the preamble")
    print(f"CONTEXT-matching.md: {n_m} lines, {len(order['CONTEXT-matching.md'])} original sections")
    print(f"CONTEXT.md: {n_l} lines, {len(order['CONTEXT.md'])} original sections, "
          f"{len(extras['CONTEXT.md'])} new ({', '.join(t[:40] for _, t in extras['CONTEXT.md'])})")
    print(f"boundary: original section {boundary} '## {orig[boundary][0][:90] if boundary < len(orig) else '-'}'")
    print(f"Standing rules: {len(rules.splitlines())} lines, {len(links)} links")
    print("allowed differences: " + "; ".join(ALLOWED))
    for k in ("(a) every original line present", "(b) every section once, unchanged",
              "(c) order and boundary", "Standing rules links"):
        tag = k.split()[0]
        bad = [m for m in failures if m.startswith(tag) or (tag == "Standing" and m.startswith("Standing"))]
        print(f"{'FAIL' if bad else 'PASS'} {k}")
    other = [m for m in failures if not re.match(r"\((a|b|c)\)|Standing", m)]
    print(f"{'FAIL' if other else 'PASS'} preamble, header and new sections")
    for m in failures:
        print("  " + m)
    print("RESULT: " + ("FAIL" if failures else "PASS"))
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
