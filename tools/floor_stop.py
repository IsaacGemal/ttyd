#!/usr/bin/env python3
"""Name the agent scope holding the most deleted-but-open file space, for the disk floor in tools/wave_launch.py.

    python3 tools/floor_stop.py [SLICE]        prints e.g. "ttyd-w71a_foo-run.scope 123456789", or nothing

Space a process keeps by holding a deleted file open never shows in `du` of the agents' dirs, only in `df`. When the
home filesystem falls under the floor, the launcher's watchdog stops just this scope (and pauses new launches); the
other agents keep running. SLICE defaults to ttyd-agents.slice under the user's systemd instance."""
import glob
import os
import sys


def main():
    slice_name = sys.argv[1] if len(sys.argv) > 1 else "ttyd-agents.slice"
    uid = os.getuid()
    roots = glob.glob(f"/sys/fs/cgroup/user.slice/user-{uid}.slice/user@{uid}.service/**/{slice_name}", recursive=True)
    best = (0, "")
    for root in roots:
        for scope in glob.glob(os.path.join(root, "*.scope")):
            held = 0
            try:
                pids = open(os.path.join(scope, "cgroup.procs")).read().split()
            except OSError:
                continue
            for pid in pids:
                for fd in glob.glob(f"/proc/{pid}/fd/*"):
                    try:
                        if os.readlink(fd).endswith(" (deleted)"):
                            held += os.stat(fd).st_size
                    except OSError:
                        pass
            best = max(best, (held, os.path.basename(scope)))
    if best[0] > 0:
        print(best[1], best[0])


if __name__ == "__main__":
    main()
