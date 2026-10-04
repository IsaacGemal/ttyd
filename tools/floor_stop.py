#!/usr/bin/env python3
"""The disk floor's action, for the watchdog in tools/wave_launch.py (run while the home filesystem is under it).

    python3 tools/floor_stop.py [--act] [--min-gb N] [SLICE]

Finds the agent scope (in SLICE, default ttyd-agents.slice) holding the most deleted-but-open file space on the home
filesystem: space no `du` of the agents' dirs can see. Counted by allocated blocks, each file once (by inode), only
on the home filesystem. Prints one line saying what it found. With --act, and only if that scope holds more than
--min-gb (default 1) GB, it writes limits.txt into that agent's output dir (so its results are never collected as
clean) and stops the scope; otherwise it stops nothing and the launches simply stay paused."""
import glob
import os
import re
import subprocess
import sys
import time


def held_by_scope(slice_name, home_dev):
    uid = os.getuid()
    roots = glob.glob(f"/sys/fs/cgroup/user.slice/user-{uid}.slice/user@{uid}.service/**/{slice_name}", recursive=True)
    held = {}
    for root in roots:
        for scope in glob.glob(os.path.join(root, "*.scope")):
            seen, total = set(), 0
            try:
                pids = open(os.path.join(scope, "cgroup.procs")).read().split()
            except OSError:
                continue
            for pid in pids:
                for fd in glob.glob(f"/proc/{pid}/fd/*"):
                    try:
                        if not os.readlink(fd).endswith(" (deleted)"):
                            continue
                        st = os.stat(fd)
                    except OSError:
                        continue
                    if st.st_dev != home_dev or (st.st_dev, st.st_ino) in seen:
                        continue
                    seen.add((st.st_dev, st.st_ino))
                    total += st.st_blocks * 512
            held[os.path.basename(scope)] = total
    return held


def main():
    args = sys.argv[1:]
    act = "--act" in args
    min_gb = float(args[args.index("--min-gb") + 1]) if "--min-gb" in args else 1.0
    rest = [a for i, a in enumerate(args) if not a.startswith("--") and (i == 0 or args[i - 1] != "--min-gb")]
    slice_name = rest[0] if rest else "ttyd-agents.slice"
    home = os.path.expanduser("~")
    held = held_by_scope(slice_name, os.stat(home).st_dev)
    stamp = time.strftime("%F %T")
    if not held or max(held.values()) == 0:
        print(f"{stamp} no agent scope holds deleted files on the home filesystem: nothing stopped")
        return
    scope, size = max(held.items(), key=lambda kv: kv[1])
    if size < min_gb * 2**30 or not act:
        print(f"{stamp} largest holder {scope}: {size // 2**20} MB deleted-but-open (stop threshold {min_gb:g} GB): nothing stopped")
        return
    m = re.fullmatch(r"ttyd-(.+)-[a-z]+\d?\.scope", scope)
    if m:
        out = os.path.join(home, "ttyd-agents-out", m.group(1))
        if os.path.isdir(out) and not os.path.islink(out):
            with open(os.path.join(out, "limits.txt"), "a") as f:
                f.write(f"{stamp} disk floor: this agent held {size // 2**20} MB of deleted-but-open files: stopped\n")
    subprocess.run(["systemctl", "--user", "stop", scope], capture_output=True, timeout=60)
    print(f"{stamp} stopped {scope}: {size // 2**20} MB deleted-but-open")


if __name__ == "__main__":
    main()
