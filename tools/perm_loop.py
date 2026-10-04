#!/usr/bin/env python3
"""Keep a permuter queue running through load stops (tower only).

    python3 tools/perm_loop.py QUEUE [perm_run.py options]

Runs tools/perm_run.py on the entries of QUEUE that have no finished line in the log yet (a line counts only if it was logged for the unit's current source, by the sha1 prefix in its last column; a line whose exit reason is `load`,
`interrupted` or `disk` does not count as finished, and `prepare-failed` is retried until it has failed 3 times); when the supervisor stops on load it waits until the 1-minute load average is
below 12 and starts again with what is left. It ends when every entry is done, on Ctrl-C/SIGTERM (the running supervisor and
its runs are killed with it), or after 14 hours. All the limits of perm_run.py apply to every round."""
import os
import signal
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.realpath(__file__)))
import perm_unit  # noqa: E402

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


RETRY = ("load", "interrupted", "disk")
MAX_PREPARE_FAILS = 3


def finished_from(lines, hash_of=lambda unit: None):
    """{(unit, func)} with a finished log line for the CURRENT source. A line counts only if its eighth column (the sha1 prefix of the
    unit's source file when it ran, see perm_run.py) equals hash_of(unit): a run on source that has changed since, and every old
    line without the column, does not. A `load`/`interrupted`/`disk` exit is not finished; `prepare-failed` is retried until it has
    failed MAX_PREPARE_FAILS times. With the default hash_of (always None) the column is ignored."""
    done, fails = set(), {}
    for ln in lines:
        p = ln.rstrip("\n").split("\t")
        if len(p) < 7:
            continue
        want = hash_of(p[1])
        if want is not None and (len(p) < 8 or p[7] != want):
            continue
        key = (p[1], p[2])
        if p[6] == "prepare-failed":
            fails[key] = fails.get(key, 0) + 1
            if fails[key] >= MAX_PREPARE_FAILS:
                done.add(key)
        elif p[6] not in RETRY:
            done.add(key)
    return done


def finished():
    if not os.path.exists(LOG):
        return set()
    cache = {}

    def hash_of(unit):
        if unit not in cache:
            cache[unit] = perm_unit.source_hash(unit, ROOT)
        return cache[unit]
    return finished_from(open(LOG).readlines(), hash_of)


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
