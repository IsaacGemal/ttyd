#!/usr/bin/env python3
"""Keep agent workspace directories under a size cap by deleting the oldest first (FIFO).

Usage: python3 tools/prune_agents.py [--limit-gb 50] [--dry-run] DIR [DIR ...]

Each DIR holds one subdirectory per agent (e.g. ~/ttyd-agents/<name>). When their combined size is over the
limit, whole subdirectories are deleted, oldest last-modified first, until it fits. Anything modified in the
last --active-min minutes is never touched, so a running agent keeps its workspace. Runs from cron on the
tower every 30 minutes over ~/ttyd-agents with a 50 GB cap.
"""
import argparse
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path


def size_bytes(path):
    out = subprocess.run(["du", "-sb", str(path)], capture_output=True, text=True).stdout.split()
    return int(out[0]) if out else 0


def last_modified(path):
    newest = path.stat().st_mtime
    for entry in os.scandir(path):
        try:
            newest = max(newest, entry.stat(follow_symlinks=False).st_mtime)
        except OSError:
            pass
    return newest


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dirs", nargs="+")
    ap.add_argument("--limit-gb", type=float, default=50)
    ap.add_argument("--active-min", type=float, default=60)
    ap.add_argument("--dry-run", action="store_true")
    a = ap.parse_args()
    limit = a.limit_gb * 1024 ** 3
    now = time.time()
    items = []
    for d in a.dirs:
        root = Path(d).expanduser().resolve()
        if not root.is_dir():
            continue
        for child in root.iterdir():
            if child.is_dir() and not child.is_symlink():
                items.append((last_modified(child), size_bytes(child), child))
    total = sum(s for _, s, _ in items)
    stamp = time.strftime("%Y-%m-%d %H:%M:%S")
    if total <= limit:
        print(f"{stamp} {total / 1024 ** 3:.1f} GB in {len(items)} workspaces, under {a.limit_gb:g} GB")
        return
    for mtime, size, child in sorted(items):
        if total <= limit:
            break
        if now - mtime < a.active_min * 60:
            continue
        print(f"{stamp} {'would delete' if a.dry_run else 'deleting'} {child} "
              f"({size / 1024 ** 3:.1f} GB, last modified {time.strftime('%Y-%m-%d %H:%M', time.localtime(mtime))})")
        if not a.dry_run:
            shutil.rmtree(child)
        total -= size
    print(f"{stamp} now {total / 1024 ** 3:.1f} GB" + (" (still over: remaining workspaces are active)" if total > limit else ""))


if __name__ == "__main__":
    sys.exit(main())
