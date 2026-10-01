#!/usr/bin/env python3
"""Keep a permuter queue running through load stops (tower only).

    python3 tools/perm_loop.py QUEUE [perm_run.py options]

Runs tools/perm_run.py on the entries of QUEUE that have no finished line in the log yet (a line whose exit reason is `load`
or `interrupted` does not count as finished); when the supervisor stops on load it waits until the 1-minute load average is
below 12 and starts again with what is left. It ends when every entry is done, on Ctrl-C/SIGTERM (the running supervisor and
its runs are killed with it), or after 14 hours. All the limits of perm_run.py apply to every round."""
import os
import signal
import subprocess
import sys
import tempfile
import time

ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
PERM_ROOT = os.environ.get("PERM_ROOT", os.path.expanduser("~/permuter"))
LOG = os.path.join(PERM_ROOT, "runs.log")
queue = sys.argv[1]
opts = sys.argv[2:]
deadline = time.time() + 14 * 3600
child = None


def stop(*_):
    if child and child.poll() is None:
        child.terminate()
        try:
            child.wait(timeout=60)
        except subprocess.TimeoutExpired:
            child.kill()
    sys.exit(0)


signal.signal(signal.SIGINT, stop)
signal.signal(signal.SIGTERM, stop)


def finished():
    done = set()
    if os.path.exists(LOG):
        for ln in open(LOG):
            p = ln.rstrip("\n").split("\t")
            if len(p) >= 7 and p[6] not in ("load", "interrupted", "disk"):
                done.add((p[1], p[2]))
    return done


while time.time() < deadline:
    done = finished()
    todo = []
    for ln in open(queue):
        t = ln.split("#")[0].strip()
        if t and (t.split()[0], t.split()[1]) not in done:
            todo.append(ln)
    if not todo:
        print("queue done")
        break
    while os.getloadavg()[0] > 12:
        time.sleep(30)
    with tempfile.NamedTemporaryFile("w", suffix=".txt", delete=False) as f:
        f.writelines(todo)
        qf = f.name
    child = subprocess.Popen([sys.executable, os.path.join(ROOT, "tools/perm_run.py"), qf] + opts, start_new_session=True)
    child.wait()
    os.unlink(qf)
    time.sleep(20)
