#!/usr/bin/env python3
"""Split docs/CONTEXT.md at 100% matched into docs/CONTEXT-matching.md and a shorter docs/CONTEXT.md.

    python3 tools/split_context.py                  split docs/CONTEXT.md in place
    python3 tools/split_context.py --input PATH     split another copy (writes to --out-dir, default docs/)

Sections are split on "## " headings outside code fences. CONTEXT-matching.md gets a frozen header and every
matching-era chapter verbatim and in order: everything from the first chapter after "Current state" up to the
boundary chapter BOUNDARY, except "Current state" and "Parked ideas". CONTEXT.md keeps the preamble (with the
edits in PREAMBLE_REPLACE / PREAMBLE_INSERT), "Current state" (with the line edits in CURRENT_REPLACE), the Standing
rules from tools/context_standing_rules.md, "Parked ideas" (minus the PARKED_MOVED bullet, which moves verbatim into the
split chapter), the chapters from BOUNDARY on, and the split chapter (SPLIT_CHAPTER) at the end.

In the Standing rules source, a link written [label](@{Heading text prefix}) becomes a reference link to that
heading's GitHub anchor in whichever file holds it, with the definitions at the end of the section; the prefix must
match exactly one heading in the two output files.

Refuses an input that already has a "## Standing rules" section. Check the result with tools/check_context_split.py.
"""
import argparse
import re
import sys
import textwrap
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RULES = ROOT / "tools" / "context_standing_rules.md"

BOUNDARY = "Leftovers found while playing the matched build"  # first chapter after the 100% milestone (2026-10-18)
CURRENT = "Current state"
PARKED = "Parked ideas"
STANDING = "Standing rules"

PREAMBLE_REPLACE = {
    'made the call. The "Current state" section is the only part that gets rewritten in place; everything below it is':
        'made the call. The "Current state" and "Standing rules" sections are the only parts that get rewritten in place;',
    "append-only history (correct an old chapter by adding a new one that supersedes it, and say so).":
        "everything else is append-only history (correct an old chapter by adding a new one that supersedes it, and say so).",
}
PREAMBLE_INSERT_AFTER = "everything else is append-only history (correct an old chapter by adding a new one that supersedes it, and say so)."
PREAMBLE_INSERT = [
    "",
    "The matching-era chapters (2026-09-24 to 2026-10-18) are in `docs/CONTEXT-matching.md`, which is frozen; new",
    "chapters still go at the bottom of this file. Search both: `grep -n PATTERN docs/CONTEXT*.md`.",
]

CURRENT_REPLACE = {
    '- New orchestrator session? Read "The orchestrator\'s role: how to pick this up" (near the end) first, then the':
        '- New orchestrator session? Read "The orchestrator\'s role: how to pick this up" (in `docs/CONTEXT-matching.md`)'
        ' first, then the',
}
PARKED_MOVED = "- **Split this file at 100% matched** (parked 2026-10-20"  # done by this split: moves into SPLIT_CHAPTER

MATCHING_HEADER = """\
# TTYD decomp: the ledger, matching phase (frozen)

The historical log of the matching phase, 2026-09-24 to 2026-10-18: every chapter from the start of the fork to the
project reaching 100% matched and linked (#852), moved here verbatim from `docs/CONTEXT.md` on 2026-10-21. This file
is no longer appended to. New entries, including corrections to the chapters here, go at the bottom of
`docs/CONTEXT.md`, which also keeps "Current state", "Standing rules" and "Parked ideas". Search both files:
`grep -n PATTERN docs/CONTEXT*.md`.
"""

SPLIT_CHAPTER = """\
## CONTEXT.md split at 100% matched (2026-10-21, Isaac; written up by Claude Opus 5.5)

The parked idea "Split this file at 100% matched", done. It read, in "Parked ideas" until this split:

{parked}

- `docs/CONTEXT-matching.md`: every chapter from "Why TTYD" (2026-09-24) through "animPaperPoseDispSub LINKS animdrv"
  (whose last section records mapdrv linking and the project reaching 100% on 2026-10-18, #852), verbatim and in
  their original order, under a frozen header. Nothing more is appended there.
- `docs/CONTEXT.md` (this file): the preamble, now pointing at both files; "Current state"; the new "Standing rules";
  "Parked ideas"; every chapter from "Leftovers found while playing the matched build" (2026-10-18) on, verbatim.
- "Standing rules" is the only new text: the rules still in force, each linked to the chapters that decided it, kept
  current in place like "Current state". `tools/split_context.py` inserted it from `tools/context_standing_rules.md`.
- Reproduce: `python3 tools/split_context.py` on an unsplit `docs/CONTEXT.md`. Verify: `python3
  tools/check_context_split.py [ORIGINAL]` takes the original from origin/main's `docs/CONTEXT.md` (or the path
  given) and checks that every line and every section is in exactly one of the two files, unchanged and in order;
  the only differences it allows are the frozen header, the preamble edit, the "Current state" pointer to the
  orchestrator's-role chapter, the parked idea above moving here, "Standing rules" and this chapter.
- Search both: `grep -n PATTERN docs/CONTEXT*.md`.
"""

FENCE_RE = re.compile(r"^\s*(```|~~~)")
HEADING_RE = re.compile(r"^(#{1,6}) +(.*?)(?:\s+#+)?\s*$")
LINK_RE = re.compile(r"\]\(@\{([^}\n]+)\}\)")


def split_sections(text):
    """[(title or None, [lines])]: the preamble, then one entry per "## " heading outside code fences."""
    sections = [(None, [])]
    in_fence = False
    for line in text.split("\n"):
        if FENCE_RE.match(line):
            in_fence = not in_fence
        elif not in_fence and line.startswith("## "):
            sections.append((line[3:], []))
        sections[-1][1].append(line)
    return sections


def slug(text):
    s = re.sub(r"[^\w\- ]", "", text.strip().lower())
    return s.replace(" ", "-")


def anchors(text):
    """[(heading text, anchor)] for every heading outside code fences, with GitHub's -1, -2 suffixes for repeats."""
    out, seen, in_fence = [], {}, False
    for line in text.split("\n"):
        if FENCE_RE.match(line):
            in_fence = not in_fence
            continue
        m = None if in_fence else HEADING_RE.match(line)
        if not m:
            continue
        base = slug(m.group(2))
        n = seen.get(base, 0)
        seen[base] = n + 1
        out.append((m.group(2), base if n == 0 else f"{base}-{n}"))
    return out


def block(lines):
    """A section's lines with trailing blank lines removed."""
    lines = list(lines)
    while lines and lines[-1] == "":
        lines.pop()
    return lines


def join(blocks):
    return "\n\n".join("\n".join(b) for b in blocks) + "\n"


def reflow(text, width=120):
    """Re-wrap each paragraph and "- " bullet to the width; headings, bold labels and blank lines stay as they are."""
    out, para = [], []

    def flush():
        if para:
            first = para[0].startswith("- ")
            words = " ".join(l.strip() for l in para)
            words = re.sub(r"\[[^\]]*\]\[\d+\]", lambda m: m.group(0).replace(" ", "\0"), words)  # keep link labels whole
            lines = textwrap.wrap(words, width, subsequent_indent="  " if first else "",
                                  break_long_words=False, break_on_hyphens=False)
            out.extend(l.replace("\0", " ") for l in lines)
            para.clear()

    for line in text.split("\n"):
        if not line.strip() or line.startswith("#") or line.startswith("**"):
            flush()
            out.append(line)
        elif line.startswith("- "):
            flush()
            para.append(line)
        else:
            para.append(line)
    flush()
    return "\n".join(out)


def bullet(lines, prefix):
    """(start, end) of the "- " bullet starting with prefix: its first line and the indented lines after it."""
    hits = [i for i, l in enumerate(lines) if l.startswith(prefix)]
    if len(hits) != 1:
        sys.exit(f"bullet {prefix!r} found {len(hits)} times, expected once")
    end = hits[0] + 1
    while end < len(lines) and lines[end].startswith("  "):
        end += 1
    return hits[0], end


def find(sections, prefix):
    hits = [i for i, (t, _) in enumerate(sections) if t is not None and t.startswith(prefix)]
    return hits


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--input", default=str(ROOT / "docs" / "CONTEXT.md"))
    ap.add_argument("--out-dir", default=str(ROOT / "docs"))
    args = ap.parse_args()

    text = Path(args.input).read_text()
    sections = split_sections(text)
    if find(sections, STANDING):
        sys.exit(f"{args.input} already has a '## {STANDING}' section: it is split already")
    cur, parked, bound = find(sections, CURRENT), find(sections, PARKED), find(sections, BOUNDARY)
    if len(cur) != 1 or len(parked) != 1 or not bound:
        sys.exit(f"expected one '{CURRENT}', one '{PARKED}' and a '{BOUNDARY}' heading; found {cur} {parked} {bound}")
    cur, parked, bound = cur[0], parked[0], bound[0]
    if cur != 1 or not parked < bound:
        sys.exit(f"unexpected layout: '{CURRENT}' at section {cur}, '{PARKED}' at {parked}, boundary at {bound}")

    preamble = block(sections[0][1])
    for old in PREAMBLE_REPLACE:
        if preamble.count(old) != 1:
            sys.exit(f"preamble line not found exactly once: {old!r}")
    preamble = [PREAMBLE_REPLACE.get(l, l) for l in preamble]
    i = preamble.index(PREAMBLE_INSERT_AFTER) + 1
    preamble[i:i] = PREAMBLE_INSERT

    current = block(sections[cur][1])
    for old in CURRENT_REPLACE:
        if current.count(old) != 1:
            sys.exit(f"Current state line not found exactly once: {old!r}")
    current = [CURRENT_REPLACE.get(l, l) for l in current]
    parked_lines = block(sections[parked][1])
    a, b = bullet(parked_lines, PARKED_MOVED)
    moved = parked_lines[a:b]
    del parked_lines[a:b]
    split_chapter = block(SPLIT_CHAPTER.replace("{parked}", "\n".join(moved)).split("\n"))

    matching = [block(MATCHING_HEADER.split("\n"))]
    matching += [block(sections[k][1]) for k in range(1, bound) if k not in (cur, parked)]
    rules = block(RULES.read_text().split("\n"))
    live = [preamble, current, rules, parked_lines]
    live += [block(sections[k][1]) for k in range(bound, len(sections))]
    live.append(split_chapter)

    matching_text, live_text = join(matching), join(live)
    heads = [("CONTEXT-matching.md", h, a) for h, a in anchors(matching_text)]
    heads += [("", h, a) for h, a in anchors(live_text)]

    refs = {}  # target -> reference number, in order of first use

    def resolve(m):
        hits = [(f, a) for f, h, a in heads if h.startswith(m.group(1))]
        if len(hits) != 1:
            sys.exit(f"Standing rules link @{{{m.group(1)}}} matches {len(hits)} headings, expected 1")
        f, a = hits[0]
        n = refs.setdefault(f"{f}#{a}", len(refs) + 1)
        return f"][{n}]"

    rules_text = "\n".join(rules)
    resolved = reflow(LINK_RE.sub(resolve, rules_text))
    resolved += "\n\n" + "\n".join(f"[{n}]: {t}" for t, n in refs.items())
    live_text = live_text.replace(rules_text, resolved, 1)

    out = Path(args.out_dir)
    (out / "CONTEXT-matching.md").write_text(matching_text)
    (out / "CONTEXT.md").write_text(live_text)
    n_match = len(matching) - 1
    n_live = len(sections) - bound
    print(f"split at section {bound} ('## {sections[bound][0][:70]}...')")
    print(f"CONTEXT-matching.md: {n_match} chapters, {matching_text.count(chr(10))} lines")
    print(f"CONTEXT.md: Current state, Standing rules, Parked ideas, {n_live} chapters + split chapter, "
          f"{live_text.count(chr(10))} lines")


if __name__ == "__main__":
    main()
