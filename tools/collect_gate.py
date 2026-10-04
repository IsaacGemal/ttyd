#!/usr/bin/env python3
"""Check that an agent's results folder is a clean, complete collect before anything merges from it.

    python3 tools/collect_gate.py RESULTS_DIR        exit 0 and print OK, or exit 1 and print why

Clean means: COLLECT-OK (written last by tools/agent_collect.sh) is present; every file it lists is here with the same
size and sha256; every file here is listed, except the ones the launcher adds locally (PROMPT.md, host_changes.txt,
events.jsonl.zst); there is no limits.txt; and audit.txt ends with "0 flagged". A file the copy back skipped (over
its size limit) or changed afterwards fails the check.

Results from before the launcher wrote audits have none of audit.txt, collect.txt, limits.txt or COLLECT-OK. They
pass only when ALLOW_UNAUDITED is set to that folder's wave/tag (e.g. wave62/battle_x: one folder at a time, on
purpose). A folder with any of those files, or the LAUNCHED marker the current launcher writes, never passes that way:
a flagged, failed or killed collect can't be waved through."""
import hashlib
import os
import sys

# written next to the results on this side: by the launcher, and by tools/merge_agent.sh on an earlier merge attempt
LOCAL = {"PROMPT.md", "host_changes.txt", "events.jsonl.zst", "LAUNCHED", "lint.txt", "proto_asm.txt"}
MARKERS = ("audit.txt", "collect.txt", "limits.txt", "COLLECT-OK")


def text(p):
    with open(p, errors="replace") as f:
        return f.read()


def digest(p):
    with open(p, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest()


def check(d):
    full = os.path.abspath(d)
    tag = os.path.join(os.path.basename(os.path.dirname(full)), os.path.basename(full))  # wave/tag
    have = {f for f in os.listdir(d) if os.path.isfile(os.path.join(d, f))}
    if not any(m in have for m in MARKERS):
        if "LAUNCHED" not in have and os.environ.get("ALLOW_UNAUDITED", "") == tag:
            return None
        return ("launched by the sandboxed launcher but never collected (killed or failed before the collect)"
                if "LAUNCHED" in have else "no audit, collect or COLLECT-OK file: not gathered by the sandboxed collect")
    if "limits.txt" in have:
        return "a limit tripped (limits.txt): " + text(os.path.join(d, "limits.txt")).strip()[:300]
    if "COLLECT-OK" not in have:
        why = text(os.path.join(d, "collect.txt")).strip()[:300] if "collect.txt" in have else ""
        return "no COLLECT-OK (the collect failed or flagged)" + (f": {why}" if why else "")
    listed = {}
    for line in text(os.path.join(d, "COLLECT-OK")).splitlines(True):
        parts = line.rstrip("\n").split(" ", 2)
        if len(parts) != 3 or not parts[1].isdigit():
            return f"COLLECT-OK: malformed line {line.strip()[:80]!r}"
        listed[parts[2]] = (parts[0], int(parts[1]))
    for name, (want, size) in listed.items():
        p = os.path.join(d, name)
        if name not in have or os.path.islink(p):
            return f"{name}: listed in COLLECT-OK but missing (skipped by the copy back?)"
        if os.path.getsize(p) != size or digest(p) != want:
            return f"{name}: differs from COLLECT-OK"
    extra = sorted(have - set(listed) - LOCAL - {"COLLECT-OK"})
    if extra:
        return f"files not in COLLECT-OK: {', '.join(extra)}"
    if "audit.txt" not in listed:
        return "audit.txt not in COLLECT-OK"
    lines = text(os.path.join(d, "audit.txt")).strip().splitlines()
    if not lines or not lines[-1].startswith("0 flagged"):
        return "audit flagged: " + (lines[-1] if lines else "empty audit.txt")
    return None


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    why = check(sys.argv[1])
    if why:
        print(why)
        sys.exit(1)
    print("OK")


if __name__ == "__main__":
    main()
