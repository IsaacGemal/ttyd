#!/usr/bin/env python3
"""Supervisor for decomp-permuter runs (run it on the tower, never on the Mac).

    python3 tools/perm_run.py QUEUE [--minutes 20] [--jobs 2] [--threads 4] [--log ~/permuter/runs.log]

QUEUE: one `UNIT FUNC` per line (e.g. `koopa_motion kpa_jump`; `#` comments). For each line it runs tools/perm_prepare.py, then
the permuter with `--stop-on-zero --best-only -j THREADS` under `nice -n 15`. JOBS runs go at once; JOBS x THREADS is capped
at 8 (the tower's budget while waves run). Nothing outlives this process:
  - every run is its own process group inside this supervisor's session; a run older than MINUTES (+60 s grace) is killed
    with its whole group (checked from `ps -o etimes`, on top of `timeout -k 30 MINUTESm`);
  - Ctrl-C, SIGTERM or any exit kills every group that is still running;
  - before each launch and every 10 s: stop everything and exit when the load average is above 20 or free disk is below 100 GB.
One line per finished run goes to the log, tab separated:
  ISO-time  unit  func  base_score  best_score  runtime_s  exit_reason
exit_reason is one of: zero (score 0 reached), finished (permuter ended by itself), timeout, killed-overdue, load, disk,
prepare-failed, permuter-error, interrupted. best_score is the lowest `output-SCORE-N` directory (none: `-`).
Wins are in PERM_ROOT/inputs/UNIT__FUNC/output-*/source.c; nothing is applied to the repo."""
import argparse
import atexit
import glob
import os
import re
import shutil
import signal
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
PERM_ROOT = os.environ.get("PERM_ROOT", os.path.expanduser("~/permuter"))
PERMUTER = os.path.join(PERM_ROOT, "decomp-permuter")
MAX_THREADS = 8
MAX_LOAD = 20.0
MIN_FREE = 100 * 10**9

ap = argparse.ArgumentParser()
ap.add_argument("queue")
ap.add_argument("--minutes", type=int, default=20)
ap.add_argument("--jobs", type=int, default=2)
ap.add_argument("--threads", type=int, default=4)
ap.add_argument("--log", default=os.path.join(PERM_ROOT, "runs.log"))
a = ap.parse_args()
if a.jobs * a.threads > MAX_THREADS:
    sys.exit(f"jobs x threads = {a.jobs * a.threads} exceeds the cap of {MAX_THREADS}")

items = []
for ln in open(a.queue):
    ln = ln.split("#")[0].strip()
    if ln:
        unit, func = ln.split()[:2]
        items.append((unit, func))

running = {}  # pid -> dict(unit, func, start, dir, base)
stop_reason = None


def kill_group(pid, sig=signal.SIGTERM):
    try:
        os.killpg(pid, sig)
    except ProcessLookupError:
        pass


def kill_all():
    for pid in list(running):
        kill_group(pid, signal.SIGTERM)
    time.sleep(2)
    for pid in list(running):
        kill_group(pid, signal.SIGKILL)


atexit.register(kill_all)


def on_signal(sig, frame):
    global stop_reason
    stop_reason = "interrupted"


signal.signal(signal.SIGINT, on_signal)
signal.signal(signal.SIGTERM, on_signal)


def healthy():
    if os.getloadavg()[0] > MAX_LOAD:
        return "load"
    if shutil.disk_usage(PERM_ROOT).free < MIN_FREE:
        return "disk"
    return None


def best_of(d):
    scores = [int(m.group(1)) for p in glob.glob(os.path.join(d, "output-*")) if (m := re.match(r"output-(\d+)-", os.path.basename(p)))]
    return min(scores) if scores else None


def log(r, reason):
    secs = int(time.time() - r["start"])
    best = best_of(r["dir"])
    os.makedirs(os.path.dirname(a.log), exist_ok=True)
    with open(a.log, "a") as f:
        f.write("\t".join([time.strftime("%Y-%m-%dT%H:%M:%S"), r["unit"], r["func"], str(r.get("base", "-")), "-" if best is None else str(best),
                           str(secs), reason]) + "\n")


def launch(unit, func):
    p = subprocess.run([sys.executable, os.path.join(ROOT, "tools/perm_prepare.py"), unit, func], capture_output=True, text=True, cwd=ROOT)
    d = os.path.join(PERM_ROOT, "inputs", unit.replace("/", "_") + "__" + func)
    r = {"unit": unit, "func": func, "start": time.time(), "dir": d}
    if p.returncode:
        log(r, "prepare-failed")
        return
    for old in glob.glob(os.path.join(d, "output-*")):
        shutil.rmtree(old, ignore_errors=True)
    out = open(os.path.join(d, "run.out"), "w")
    cmd = ["timeout", "-k", "30", f"{a.minutes}m", "nice", "-n", "15", os.path.join(PERMUTER, ".venv/bin/python"),
           os.path.join(PERMUTER, "permuter.py"), d, "-j", str(a.threads), "--stop-on-zero", "--best-only"]
    proc = subprocess.Popen(cmd, stdout=out, stderr=subprocess.STDOUT, start_new_session=True, cwd=PERMUTER)
    running[proc.pid] = dict(r, proc=proc, out=d + "/run.out")


def finish(pid, reason=None):
    r = running.pop(pid)
    rc = r["proc"].returncode
    txt = open(r["out"], errors="replace").read().replace("\r", "\n")
    m = re.search(r"base score = (\d+)", txt)
    r["base"] = m.group(1) if m else "-"
    best = best_of(r["dir"])
    if reason is None:
        if rc == 124 or rc == 137:
            reason = "timeout"
        elif best == 0:
            reason = "zero"
        elif rc == 0:
            reason = "finished"
        else:
            reason = "permuter-error" if m is None else "finished"
    log(r, reason)


last_check = 0.0
while (items or running) and not stop_reason:
    for pid in list(running):
        if running[pid]["proc"].poll() is not None:
            finish(pid)
    for pid, r in list(running.items()):
        el = subprocess.run(["ps", "-o", "etimes=", "-p", str(pid)], capture_output=True, text=True).stdout.strip()
        if el and int(el) > a.minutes * 60 + 60:
            kill_group(pid, signal.SIGKILL)
            r["proc"].wait()
            finish(pid, "killed-overdue")
    if time.time() - last_check > 10:
        last_check = time.time()
        bad = healthy()
        if bad:
            stop_reason = bad
            break
    while items and len(running) < a.jobs and not stop_reason:
        launch(*items.pop(0))
    time.sleep(2)

for pid in list(running):
    kill_group(pid, signal.SIGTERM)
    try:
        running[pid]["proc"].wait(timeout=10)
    except subprocess.TimeoutExpired:
        kill_group(pid, signal.SIGKILL)
    finish(pid, stop_reason or "interrupted")
if stop_reason:
    print("stopped:", stop_reason)
