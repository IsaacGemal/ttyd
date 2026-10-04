"""tools/wave_launch.py: the reaper, deadlines on every remote call, slot accounting, and the sandbox command."""
import json
import os
import signal
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import wave_launch as W  # noqa: E402
import agent_audit
import provenance

SPEC = {"unit": "battle/ac/ac_pendulum_crane_timing", "src": "src/battle/ac/ac_pendulum_crane_timing.c", "tag": "t",
        "mode": "near", "dol": True, "source_order": False, "todo": [], "open": [["_get_angle_rate", 100, 99.2]],
        "extra": [], "others": [], "group": 1}
KEY = "sk-test-0123456789abcdef"


def setup_globals(here):
    W.HERE, W.WAVE, W.MODEL, W.KEY_ENV, W.MODEL_LINE = Path(here), "wave_test", "m", "TEST_KEY", "Model: test"
    W.BASE, W.ARCHIVE, W.PFX, W.PFXRE = "$HOME/base", "archive", "wt_", "wt_"
    W.KEY_FILE, W.UID, W.KEY_HASH, W.NODE_BIN = "~/.config/ttyd-keys/wt", "1000", "abc123", "/opt/node/bin"


class Reaper(unittest.TestCase):
    def run_reap(self, ws, procs):
        with tempfile.TemporaryDirectory() as proc:
            for pid, (cwd, comm) in procs.items():
                os.makedirs(f"{proc}/{pid}")
                os.symlink(cwd, f"{proc}/{pid}/cwd")
                Path(f"{proc}/{pid}/comm").write_text(comm + "\n")
            script = 'kill() { echo KILL "$@"; }; ' + W.reap_script(ws, proc)
            r = subprocess.run(["bash", "-c", script], capture_output=True, text=True, cwd="/")
            return r.returncode, r.stdout.split("\n")

    def test_kills_only_named_programs_in_the_workspace(self):
        with tempfile.TemporaryDirectory() as ws:
            ws = os.path.realpath(ws)
            os.makedirs(f"{ws}/sub")
            rc, out = self.run_reap(ws, {101: (f"{ws}/sub", "python3"), 102: ("/", "python3"), 103: (ws, "zsh"),
                                         104: (ws + "x", "ninja"), 105: (ws, "ninja")})
            self.assertEqual(rc, 0)
            self.assertEqual(sorted(l for l in out if l), ["KILL -9 101", "KILL -9 105"])

    def test_missing_or_empty_workspace_kills_nothing(self):
        for ws in ("/nonexistent/ws", "", "/"):
            rc, out = self.run_reap(ws, {101: ("/", "python3"), 102: (os.path.expanduser("~"), "python3")})
            self.assertEqual((rc, [l for l in out if l]), (0, []), repr(ws))


class Remote(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        setup_globals(self.tmp.name)
        self.calls = []

    def tearDown(self):
        self.tmp.cleanup()

    def fake(self, out="", rc=0, exc=None):
        def run(cmd, **kw):
            self.calls.append((cmd, kw))
            if exc:
                raise exc
            return subprocess.CompletedProcess(cmd, rc, out, "")
        return mock.patch.object(W.subprocess, "run", side_effect=run)

    def test_every_remote_call_has_a_deadline(self):
        with self.fake(out="rc=0 audit=0 TAKEN REAPED 100.00%"), mock.patch.object(W.time, "sleep"):
            W.install_key(KEY)
            W.reserve("t")
            W.free("t")
            W.reap("t")
            W.host_state()
            W.prepare("t", SPEC)
            W.run("t", SPEC["unit"], SPEC["src"], SPEC, lambda m: None)
            W.collect("t", SPEC)
        self.assertTrue(self.calls)
        for cmd, kw in self.calls:
            self.assertIsNotNone(kw.get("timeout"), cmd)

    def test_the_key_is_on_no_command_line(self):
        with self.fake(out="rc=0"), mock.patch.object(W.time, "sleep"):
            W.install_key(KEY)
            W.prepare("t", SPEC)
            W.run("t", SPEC["unit"], SPEC["src"], SPEC, lambda m: None)
        for cmd, kw in self.calls:
            self.assertNotIn(KEY, " ".join(map(str, cmd)))
        self.assertEqual(self.calls[0][1]["input"], KEY)  # install_key: over stdin

    def test_no_answer_is_no_capacity(self):
        with self.fake(rc=255):
            self.assertEqual(W.reserve("t"), "FULL")
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 200)):
            self.assertEqual(W.reserve("t"), "FULL")
        with self.fake(out="FULL"):
            self.assertEqual(W.reserve("t"), "FULL")
        with self.fake(out="TAKEN"):
            self.assertEqual(W.reserve("t"), "TAKEN")
        with self.fake(out="PAUSED"):
            self.assertEqual(W.reserve("t"), "PAUSED")
        with self.fake(out="PAUSED", rc=3):
            self.assertEqual(W.reserve("t"), "FULL")

    def test_an_audit_that_did_not_run_counts_as_flagged(self):
        with self.fake(out="something failed"):
            self.assertNotEqual(W.collect("t", SPEC), 0)
        with self.fake(out="audit=2"):
            self.assertEqual(W.collect("t", SPEC), 2)
        with self.fake(out="audit=0"):
            self.assertEqual(W.collect("t", SPEC), 0)

    def test_reap_reports_whether_it_worked(self):
        with self.fake(out="REAPED"):
            self.assertTrue(W.reap("t"))
        with self.fake(out=""):
            self.assertFalse(W.reap("t"))
        with self.fake(out="REAPED", rc=1):
            self.assertFalse(W.reap("t"))
        with self.fake():
            W.reap("t")
        self.assertIn("inactive|failed) ;; *) exit 1", self.calls[-1][0][-1])
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 180)):
            self.assertFalse(W.reap("t"))

    def test_one_slot_pool_for_every_key(self):
        self.assertEqual(W.slot_file("t"), "$HOME/ttyd-agents-slots/all/abc123_wt_t.slot")
        with self.fake(out="TAKEN"):
            W.reserve("t")
        cmd = self.calls[0][0][-1]
        self.assertIn("'ttyd-*-run.scope'", cmd)  # counted by scopes, which agents can't create
        self.assertNotIn("pgrep", cmd)
        self.assertLess(cmd.index("flock"), cmd.index("PAUSED"))  # checked inside the lock
        with self.fake(out="RUNNING"):
            self.assertEqual(W.paused(), "")
        with self.fake(out="floor tripped\nPAUSED"):
            self.assertIn("floor tripped", W.paused())
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 60)):
            self.assertTrue(W.paused())

    def test_collect_runs_outside_the_agents_slice(self):
        self.assertIn("--slice=ttyd-collect.slice", W.sandbox("t", "collect", "collect"))
        self.assertIn("--slice=ttyd-agents.slice", W.sandbox("t", "run", "agent"))

    def test_launched_marker(self):
        with self.fake(out="rc=0"):
            W.prepare("t", SPEC)
        self.assertTrue((Path(self.tmp.name) / "t" / "LAUNCHED").exists())

    def test_earlier_results_keep_the_newest_two(self):
        here = Path(self.tmp.name)
        bin_ = here / "trashbin"
        bin_.mkdir()

        def trash(cmd, **kw):  # really move it away, as `trash` does
            Path(cmd[1]).rename(bin_ / Path(cmd[1]).name)
            return subprocess.CompletedProcess(cmd, 0, "", "")
        for k in range(5):
            (here / "t").mkdir()
            (here / "t" / "final.c").write_text(str(k))
            with mock.patch.object(W.subprocess, "run", side_effect=trash):
                W.set_aside("t")
        kept = sorted(here.glob("t.prev*"), key=lambda p: p.name)
        self.assertEqual([p.name for p in kept], ["t.prev4", "t.prev5"])
        self.assertEqual([(p / "final.c").read_text() for p in kept], ["3", "4"])  # the newest two runs

    def test_caps_markers_and_guards(self):
        c = W.agent_cmd("t", 100, "x", "run")
        self.assertIn("limits.txt", c)
        self.assertIn(str(W.EVENTS_MAX), c)
        self.assertIn("passes.txt", c)
        self.assertNotIn("launcher_pass_end", c)
        self.assertNotIn(">(", c)  # no process substitution: the shell can't wait for one reliably
        for cmd in (c, W.box_cmd("t", "check0", "true")):
            self.assertTrue(cmd.split("cd $HOME")[0].endswith(W.step_guard("t")))
            self.assertIn("exit 7", cmd)
            self.assertIn("ttyd-agents-slots/PAUSED ] && exit 8", cmd)

    def test_reap_stops_exact_scopes(self):
        with self.fake():
            W.reap("t")
        cmd = self.calls[0][0][-1]
        self.assertIn("ttyd-wt_t-run.scope", cmd)
        self.assertNotIn("*", cmd.split("; for p in")[0].split("2>/dev/null")[0])  # no glob that would hit ttyd-wt_t-2

    def test_an_earlier_runs_results_are_kept_aside(self):
        d = Path(self.tmp.name) / "t"
        d.mkdir()
        (d / "final.c").write_text("old")
        with self.fake(out="rc=0"):
            W.prepare("t", SPEC)
        self.assertFalse((d / "final.c").exists())
        self.assertEqual((Path(self.tmp.name) / "t.prev1" / "final.c").read_text(), "old")

    def test_agent_steps_have_disk_and_log_caps(self):
        c = W.agent_cmd("t", 100, "x", "run")
        self.assertIn("du -smx", c)
        self.assertIn("--inodes", c)
        self.assertIn("df -Pk", c)
        self.assertIn("! -readable", c)
        self.assertNotIn("m=over", c)  # du's exit status (files vanish mid-scan in any build) is not a failure
        self.assertIn("is-active ttyd-wt_t-run.scope", c)
        self.assertIn("floor_stop.py", c)
        self.assertNotIn("stop ttyd-agents.slice", c)
        self.assertIn("sleep 10", c)
        self.assertIn(f"er=$(( {W.EVENTS_MAX} - ", c)  # caps over the whole run, not per pass
        self.assertIn(f"sr=$(( {W.STDERR_MAX} - ", c)
        self.assertIn("capped_log events.jsonl", c)
        self.assertIn('head -c "$remaining"', c)
        self.assertIn("kt $wd", c)
        self.assertIn("du -smx", W.box_cmd("t", "check0", "true"))

    def test_hung_ssh_raises_instead_of_waiting(self):
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 1)):
            with self.assertRaises(subprocess.TimeoutExpired):
                W.prepare("t", SPEC)
            W.reap("t")  # swallows its own deadline


class Watchdog(unittest.TestCase):
    def run_watchdog(self, *, states=(), du_error="", error_scan="both", hidden="", end_during="", floor=False):
        """Run the generated host loop locally with deterministic scans and scope queries; no systemd or ssh."""
        with tempfile.TemporaryDirectory() as home:
            setup_globals(home)
            for d in ("ttyd-agents/wt_t", "ttyd-agents-out/wt_t", "ttyd-agents-priv/wt_t", "ttyd-agents-slots"):
                Path(home, d).mkdir(parents=True)
            Path(home, "query-count").write_text("0\n")
            Path(home, "query-states").write_text("\n".join(states) + "\n")
            stubs = r'''
ticks=0
sleep() { ticks=$((ticks + 1)); [ "$ticks" -le 5 ]; }
timeout() { shift 3; "$@"; }
systemctl() {
  case "$*" in
    *is-active*)
      n=$(cat "$HOME/query-count"); n=$((n + 1)); echo "$n" > "$HOME/query-count"
      state=$(sed -n "${n}p" "$HOME/query-states")
      [ ! -e "$HOME/ended" ] || state=inactive
      state=${state:-active}
      case $state in
        active|activating|reloading) echo "$state"; return 0;; deactivating) echo "$state"; return 3;; inactive|failed) echo "$state"; return 3;;
        error) return 1;; bad-active) echo active; return 1;; hung) return 124;;
      esac;;
    *) echo "$*" >> "$HOME/actions"; touch "$HOME/ended";;
  esac
}
du() {
  echo '1 directory'
  [ "$END_DURING" != du ] || touch "$HOME/ended"
  case $ERROR_SCAN:$* in
    both:*|blocks:-smx*|inodes:*--inodes*)
      if [ -n "$DU_ERROR" ]; then echo "du: $DU_ERROR" >&2; return 1; fi;;
  esac
}
find() { echo "$*" > "$HOME/find-args"; [ -z "$HIDDEN" ] || echo "$HIDDEN"; }
df() {
  [ "$END_DURING" != df ] || touch "$HOME/ended"
  echo 'Filesystem size used available percent mount'
  echo "home 999999999 1 $FREE 1% /home"
}
python3() { echo "floor $*" >> "$HOME/actions"; }
'''
            env = dict(os.environ, HOME=home, DU_ERROR=du_error, ERROR_SCAN=error_scan, HIDDEN=hidden, END_DURING=end_during,
                       FREE="1" if floor else "999999999")
            result = subprocess.run(["bash", "-c", stubs + W.watchdog("t", "run") + 'wait "$wd"'],
                                    env=env, capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            def read(name):
                return Path(home, name).read_text() if Path(home, name).exists() else ""
            return {"limits": read("ttyd-agents-out/wt_t/limits.txt"), "actions": read("actions"),
                    "paused": Path(home, "ttyd-agents-slots/PAUSED").exists(), "find": read("find-args"),
                    "queries": int(read("query-count"))}

    def test_readable_but_unsearchable_directory_trips(self):
        result = self.run_watchdog(hidden="directory-without-execute")
        self.assertIn("( ! -readable -o ! -executable )", result["find"])
        self.assertIn("inaccessible directory-without-execute", result["limits"])
        self.assertIn("stop ttyd-wt_t-run.scope", result["actions"])

    def test_du_permission_denied_trips_but_vanished_files_do_not(self):
        for scan in ("blocks", "inodes"):
            result = self.run_watchdog(du_error="cannot access file: Permission denied", error_scan=scan, hidden="blocked")
            self.assertIn("du: Permission denied", result["limits"])
            self.assertIn("stop ttyd-wt_t-run.scope", result["actions"])
        result = self.run_watchdog(du_error="cannot access file: No such file or directory")
        self.assertEqual((result["limits"], result["actions"], result["paused"]), ("", "", False))

    def test_du_denials_must_be_confirmed_by_find(self):
        for error in ("cannot access file: Permission denied", "Permission denied: No such file or directory"):
            result = self.run_watchdog(du_error=error)
            self.assertEqual((result["limits"], result["actions"]), ("", ""))

    def test_transitional_owner_states_are_valid_and_reset_errors(self):
        result = self.run_watchdog(states=("error", "error", "activating", "deactivating", "reloading", "inactive"))
        self.assertEqual((result["limits"], result["actions"]), ("", ""))
        self.assertGreaterEqual(result["queries"], 6)
        result = self.run_watchdog(states=("deactivating",), hidden="blocked")
        self.assertIn("inaccessible blocked", result["limits"])

    def test_owner_ending_during_du_prevents_limit_and_floor_actions(self):
        result = self.run_watchdog(du_error="Permission denied", end_during="du", floor=True)
        self.assertEqual((result["limits"], result["actions"], result["paused"]), ("", "", False))

    def test_owner_ending_during_df_prevents_floor_actions(self):
        result = self.run_watchdog(end_during="df", floor=True)
        self.assertEqual((result["limits"], result["actions"], result["paused"]), ("", "", False))

    def test_three_consecutive_query_failures_trip(self):
        for states in (("error",) * 3, ("active", "error", "hung", "bad-active")):
            result = self.run_watchdog(states=states)
            self.assertIn("query failed 3 consecutive times", result["limits"])
            self.assertIn("stop ttyd-wt_t-run.scope", result["actions"])
            self.assertEqual(result["queries"], len(states) + 1)  # stop verification

    def test_successful_query_resets_failure_counter(self):
        result = self.run_watchdog(states=("error", "error", "active", "error", "active", "active",
                                           "error", "error", "inactive", "failed"))
        self.assertEqual((result["limits"], result["actions"], result["paused"]), ("", "", False))

    def test_active_owner_still_takes_floor_action(self):
        result = self.run_watchdog(floor=True)
        self.assertTrue(result["paused"])
        self.assertIn("/base/tools/floor_stop.py --act ttyd-agents.slice", result["actions"])
        self.assertGreater(result["queries"], 3)  # floor action must not disable later per-agent scans

    def test_teardown_finishes_and_logs_in_progress_floor_action(self):
        with tempfile.TemporaryDirectory() as home:
            setup_globals(home)
            for directory in ("ttyd-agents/wt_t", "ttyd-agents-out/wt_t", "ttyd-agents-priv/wt_t", "ttyd-agents-slots"):
                Path(home, directory).mkdir(parents=True)
            stubs = r'''
timeout() { shift 3; "$@"; }
systemctl() { echo active; }
du() { echo '1 directory'; }
find() { :; }
df() { echo header; echo 'home 100 99 1 99% /home'; }
python3() { touch "$HOME/floor-started"; sleep 0.2; echo 'floor action finished'; }
'''
            watchdog = W.watchdog("t", "run").replace('sleep 10', 'sleep 0.01')
            cmd = (stubs + W.KILL_TREE + watchdog +
                   'until [ -e "$HOME/floor-started" ]; do sleep 0.01; done; ' + W.watchdog_teardown("t", "run"))
            result = subprocess.run(["bash", "-c", cmd], env=dict(os.environ, HOME=home),
                                    capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("floor action finished", Path(home, "ttyd-agents-slots/PAUSED/why.txt").read_text())
            self.assertFalse(Path(home, "ttyd-agents-out/wt_t/limits.txt").exists())

    def test_teardown_wait_is_bounded_and_failure_is_logged(self):
        with tempfile.TemporaryDirectory() as home:
            setup_globals(home)
            out = Path(home, "ttyd-agents-out/wt_t")
            out.mkdir(parents=True)
            # Shorten polling intervals, retaining the real stop, process-state check and tree teardown.
            cmd = (W.KILL_TREE +
                   '( touch "$HOME/ttyd-agents-out/wt_t/watchdog-run.busy"; '
                   'while :; do sleep 1; done ) </dev/null >/dev/null 2>&1 & wd=$!; '
                   'until [ -e "$HOME/ttyd-agents-out/wt_t/watchdog-run.busy" ]; do sleep 0.01; done; '
                   'sleep() { command sleep 0.001; }; ' + W.watchdog_teardown("t", "run"))
            result = subprocess.run(["bash", "-c", cmd], env=dict(os.environ, HOME=home),
                                    capture_output=True, text=True, timeout=5)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("watchdog action did not finish within 90 seconds", (out / "limits.txt").read_text())
            self.assertFalse((out / "watchdog-run.busy").exists())
            self.assertFalse((out / "watchdog-run.done").exists())

    def test_finishing_steps_wait_for_in_progress_stop_escalation(self):
        for mode in ("agent", "box"):
            with self.subTest(mode=mode), tempfile.TemporaryDirectory() as home:
                setup_globals(home)
                for directory in ("ttyd-agents/wt_t", "ttyd-agents-out/wt_t", "ttyd-agents-priv/wt_t/tmp",
                                  "ttyd-agents-priv/wt_t/pi-agent", "bin"):
                    Path(home, directory).mkdir(parents=True)
                out = Path(home, "ttyd-agents-out/wt_t")
                Path(home, "key").write_text("test")
                W.KEY_FILE = "$HOME/key"
                # The first stop lets the foreground step exit immediately, then delays and fails. Teardown
                # must let this watchdog retry, KILL, stop again, and verify inactive before removing it.
                systemctl = Path(home, "bin/systemctl")
                systemctl.write_text('''#!/bin/bash
echo "$*" >> "$HOME/actions"
case "$*" in
  *is-active*)
    if [ -e "$HOME/scope-inactive" ]; then echo inactive; exit 3; fi
    echo active; exit 0;;
  *stop*)
    touch "$HOME/step-ended"
    sleep 0.2
    exit 1;;
  *--signal=KILL*) touch "$HOME/scope-inactive";;
esac
''')
                systemctl.chmod(0o755)
                foreground = 'while [ ! -e "$HOME/step-ended" ]; do sleep 0.01; done; exit 23'
                cli = Path(home, "bin/pi")
                cli.write_text('#!/bin/bash\n' + foreground + '\n')
                cli.chmod(0o755)
                stubs = r'''
stat() { python3 -c 'import os,sys; print(os.stat(sys.argv[1]).st_size)' "${@: -1}"; }
du() { echo '1 directory'; }
find() { :; }
'''
                real_watchdog = W.watchdog

                def fast_watchdog(tag, step):
                    return real_watchdog(tag, step).replace('sleep 10', 'sleep 0.01')
                with mock.patch.object(W, "sandbox", return_value=""), \
                     mock.patch.object(W, "watchdog", side_effect=fast_watchdog), \
                     mock.patch.object(W, "DISK_MB", 0):
                    cmd = (W.agent_cmd("t", 5, "", "run") if mode == "agent"
                           else W.box_cmd("t", "run", foreground, secs=5))
                result = subprocess.run(["bash", "-c", stubs + cmd],
                                        env=dict(os.environ, HOME=home, PATH=f"{home}/bin:" + os.environ["PATH"]),
                                        capture_output=True, text=True, timeout=8)
                self.assertEqual(result.returncode, 0 if mode == "agent" else 23, result.stderr)
                actions = Path(home, "actions").read_text().splitlines()
                self.assertEqual(sum('--user stop' in action for action in actions), 3)
                self.assertEqual(sum('--signal=KILL' in action for action in actions), 1)
                self.assertIn('is-active', actions[-1])
                self.assertTrue(Path(home, "scope-inactive").exists())
                limits = (out / "limits.txt").read_text()
                self.assertIn("scope still owned after stop retries: escalating to KILL", limits)
                self.assertIn("scope stop after KILL failed", limits)
                self.assertNotIn("watchdog action did not finish", limits)
                self.assertFalse((out / "watchdog-run.busy").exists())
                if mode == "agent":
                    self.assertEqual((out / "passes.txt").read_text(), "0 23 0\n")


class ScopeStop(unittest.TestCase):
    def test_stop_retries_kills_and_verifies_even_after_errors(self):
        for mode in ("retry", "kill", "stuck", "query-error"):
            with self.subTest(mode=mode), tempfile.TemporaryDirectory() as home:
                setup_globals(home)
                out = Path(home, "ttyd-agents-out/wt_t")
                out.mkdir(parents=True)
                stubs = r"""
sleep() { :; }
timeout() { shift 3; "$@"; }
systemctl() {
  echo "$*" >> "$HOME/actions"
  case "$*" in
    *is-active*)
      [ "$MODE" != query-error ] || return 1
      if [ -e "$HOME/ended" ]; then echo inactive; return 3; fi
      echo deactivating; return 3;;
    *stop*)
      if [ "$MODE" = retry ] && [ -e "$HOME/stopped-once" ]; then touch "$HOME/ended"; return 0; fi
      touch "$HOME/stopped-once"; return 1;;
    *--signal=KILL*)
      [ "$MODE" != stuck ] || return 1
      touch "$HOME/ended";;
  esac
}
"""
                result = subprocess.run(["bash", "-c", stubs + W.scope_stop("t", "run") + "stop_scope"],
                                        env=dict(os.environ, HOME=home, MODE=mode), capture_output=True, text=True, timeout=5)
                actions = Path(home, "actions").read_text()
                limits = (out / "limits.txt").read_text()
                self.assertIn("scope stop failed", limits)
                self.assertGreaterEqual(actions.count("--user stop"), 2)
                self.assertIn("--user is-active", actions)
                self.assertEqual(result.returncode, int(mode in ("stuck", "query-error")))
                if mode == "retry":
                    self.assertNotIn("--signal=KILL", actions)
                else:
                    self.assertIn("kill --kill-who=all --signal=KILL ttyd-wt_t-run.scope", actions)
                if mode in ("stuck", "query-error"):
                    self.assertIn("could not verify scope inactive", limits)
                if mode == "stuck":
                    self.assertIn("scope KILL failed", limits)


class PassPipes(unittest.TestCase):
    def prepare_home(self, home):
        setup_globals(home)
        for d in ("ttyd-agents/wt_t", "ttyd-agents-out/wt_t", "ttyd-agents-priv/wt_t/tmp",
                  "ttyd-agents-priv/wt_t/pi-agent", "bin"):
            Path(home, d).mkdir(parents=True)
        Path(home, "key").write_text("test")
        W.KEY_FILE = "$HOME/key"
        cli = Path(home, "bin/pi")
        cli.write_text(f"#!{sys.executable}\n" + '''
import os, signal, time
from pathlib import Path
home = Path(os.environ['HOME'])
(home / 'producer-pid').write_text(str(os.getpid()))
signal.signal(signal.SIGTERM, signal.SIG_IGN)
os.write(int(os.environ.get('CHANNEL', '1')), os.environ['PAYLOAD'].encode())
os.write(2, os.environ.get('STDERR_PAYLOAD', '').encode())
if os.environ.get('TIMED_IDLE') == 'yes':
    signal.signal(signal.SIGTERM, signal.SIG_DFL)
    time.sleep(30)
if os.environ.get('IDLE') == 'yes':
    time.sleep(1)
    (home / 'continued-unlogged').write_text('ran after cap')
    time.sleep(30)
raise SystemExit(int(os.environ.get('PASS_RC', '0')))
''')
        cli.chmod(0o755)
        systemctl = Path(home, "bin/systemctl")
        systemctl.write_text('''#!/bin/bash
echo "$*" >> "$HOME/actions"
case "$*" in
  *--signal=KILL*) kill -KILL "$(cat "$HOME/producer-pid")";;
  *is-active*) echo inactive; exit 3;;
esac
''')
        systemctl.chmod(0o755)
        return dict(os.environ, HOME=home, PATH=f"{home}/bin:" + os.environ["PATH"])

    def run_pass(self, env, secs=20, event_cap=64):
        # Use the real head/timeout and pipes. Only platform-specific stat and systemd are replaced. The fake scope
        # contains one producer, which ignores SIGTERM and idles after its output, exercising immediate KILL.
        stubs = r'''
stat() { python3 -c 'import os,sys; print(os.stat(sys.argv[1]).st_size)' "${@: -1}"; }
'''
        with mock.patch.object(W, "sandbox", return_value=""), \
             mock.patch.object(W, "watchdog", return_value='sleep 30 </dev/null >/dev/null 2>&1 & wd=$!; '), \
             mock.patch.object(W, "KILL_TREE", 'kt() { kill "$1" 2>/dev/null; wait "$1" 2>/dev/null; }; '), \
             mock.patch.object(W, "EVENTS_MAX", event_cap), mock.patch.object(W, "STDERR_MAX", 48):
            cmd = W.agent_cmd("t", secs, "", "run")
        try:
            return subprocess.run(["bash", "-c", stubs + cmd], env=env, capture_output=True, text=True, timeout=5)
        except subprocess.TimeoutExpired:
            # A regression must not leave the deliberately idle producer alive after the test fails.
            pid_file = Path(env["HOME"], "producer-pid")
            if pid_file.exists():
                try:
                    os.kill(int(pid_file.read_text()), signal.SIGKILL)
                except ProcessLookupError:
                    pass
            raise

    def test_either_exact_cap_stops_an_idle_producer_before_unlogged_work(self):
        for channel, name, cap in (("1", "events.jsonl", 64), ("2", "stderr.log", 48)):
            for initial in (0, cap - 7):
                with self.subTest(channel=channel, initial=initial), tempfile.TemporaryDirectory() as home:
                    env = self.prepare_home(home)
                    out = Path(home, "ttyd-agents-out/wt_t")
                    (out / name).write_bytes(b"x" * initial)
                    env.update(CHANNEL=channel, PAYLOAD="y" * (cap - initial - int(channel == "1" and initial > 0)), IDLE="yes")
                    result = self.run_pass(env)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertFalse(Path(home, "continued-unlogged").exists())
                    self.assertEqual((out / name).stat().st_size, cap)
                    self.assertIn(f"{name} reached its {cap}-byte cap", (out / "limits.txt").read_text())
                    actions = Path(home, "actions").read_text()
                    self.assertIn("kill --kill-who=all --signal=KILL ttyd-wt_t-run.scope", actions)
                    self.assertIn("stop ttyd-wt_t-run.scope", actions)
                    self.assertTrue((out / "passes.txt").exists())

    def test_pass_end_offsets_preserve_raw_events_and_exit_codes(self):
        with tempfile.TemporaryDirectory() as home:
            env = self.prepare_home(home)
            pieces = ('{"type":"msg","text":"é', '{"type":"done"}\n')
            for piece, rc in zip(pieces, (124, 0)):
                env.update(PAYLOAD=piece, PASS_RC=str(rc))
                result = self.run_pass(env)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn(f"rc={rc}", result.stdout)
            out = Path(home, "ttyd-agents-out/wt_t")
            self.assertEqual((out / "events.jsonl").read_bytes(), "\n".join(pieces).encode())
            self.assertEqual((out / "passes.txt").read_text(),
                             f"{len(pieces[0].encode())} 124 0\n{len(''.join(pieces).encode()) + 1} 0 0\n")
            self.assertFalse((out / "limits.txt").exists())

    def test_early_timeout_exit_codes_and_stderr_cannot_forge_host_timeout(self):
        for rc in (124, 137):
            with self.subTest(rc=rc), tempfile.TemporaryDirectory() as home:
                env = self.prepare_home(home)
                partial = '{"type":"message'
                fake_notice = "timeout: sending signal TERM to command 'pi'\n"
                env.update(PAYLOAD=partial, PASS_RC=str(rc), STDERR_PAYLOAD=fake_notice)
                result = self.run_pass(env)
                self.assertEqual(result.returncode, 0, result.stderr)
                out = Path(home, "ttyd-agents-out/wt_t")
                self.assertEqual((out / "passes.txt").read_text(), f"{len(partial)} {rc} 0\n")
                self.assertEqual((out / "stderr.log").read_text(), fake_notice)
                self.assertFalse((out / "limits.txt").exists())

    def test_separator_never_exceeds_the_event_cap(self):
        for initial in (63, 64):
            with self.subTest(initial=initial), tempfile.TemporaryDirectory() as home:
                env = self.prepare_home(home)
                out = Path(home, "ttyd-agents-out/wt_t")
                (out / "events.jsonl").write_bytes(b"x" * initial)
                env.update(PAYLOAD="unused")
                result = self.run_pass(env)
                self.assertEqual(result.returncode, 7, result.stderr)
                self.assertEqual((out / "events.jsonl").stat().st_size, initial)
                self.assertTrue((out / "limits.txt").exists())
                self.assertFalse(Path(home, "producer-pid").exists())

    def test_only_the_host_timeout_sets_the_flag_and_provenance_survives(self):
        with tempfile.TemporaryDirectory() as home:
            env = self.prepare_home(home)
            out = Path(home, "ttyd-agents-out/wt_t")
            partial = '{"type":"message","n":0.'
            env.update(PAYLOAD=partial, TIMED_IDLE="yes")
            result = self.run_pass(env, secs=1)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn("rc=124", result.stdout)
            self.assertEqual((out / "passes.txt").read_text(), f"{len(partial)} 124 1\n")
            event = json.dumps({"type": "tool_execution_start", "toolName": "read", "args": {"path": "/opt/reference"}}) + "\n"
            env.update(PAYLOAD=event, TIMED_IDLE="no")
            result = self.run_pass(env, event_cap=1000)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(list(provenance.events(str(out / "events.jsonl"))), [("read", {"path": "/opt/reference"})])
            boundaries = [tuple(map(int, line.split())) for line in (out / "passes.txt").read_text().splitlines()]
            self.assertEqual(boundaries[-1], ((out / "events.jsonl").stat().st_size, 0, 0))
            with (out / "events.jsonl").open("rb") as events:
                self.assertNotIn("EVENTS-CORRUPT", {kind for kind, _, _ in agent_audit.audit(events, home, boundaries)})
            self.assertFalse((out / "limits.txt").exists())

class Modes(unittest.TestCase):
    """The one-short arm (chained into a link attempt) and the data-prep arm."""
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        setup_globals(self.tmp.name)
        self.calls = []

    def tearDown(self):
        self.tmp.cleanup()

    def fake(self, respond):
        def run(cmd, **kw):
            self.calls.append(cmd)
            return subprocess.CompletedProcess(cmd, 0, respond(" ".join(map(str, cmd))), "")
        return mock.patch.object(W.subprocess, "run", side_effect=run)

    def test_oneshort_prompt_carries_hypothesis_prerequisites_and_link_steps(self):
        spec = dict(SPEC, mode="oneshort", hyp="the conversion double is read through the base register",
                    secs=[[".sdata2", 41.2]], files={"DOSSIER.md": "scratch/x.md"})
        with self.fake(lambda c: ""):
            W.prepare("t", spec)
        prompt = (Path(self.tmp.name) / "t" / "PROMPT.md").read_text()
        for part in ("ONE FUNCTION AWAY FROM LINKING", "base register", ".sdata2 41.2%", "LINKING:",
                     'Object(Matching, "battle/ac/ac_pendulum_crane_timing.c")', "DOSSIER.md"):
            self.assertIn(part, prompt)
        self.assertTrue(any("DOSSIER.md" in " ".join(map(str, c)) for c in self.calls))

    def test_dataprep_prompt_is_about_data_only(self):
        spec = dict(SPEC, mode="dataprep", secs=[[".data", 80.0], [".rodata", 60.5]],
                    open=[["a", 100, 90.0], ["b", 200, 95.0]])
        with self.fake(lambda c: ""):
            W.prepare("t", spec)
        prompt = (Path(self.tmp.name) / "t" / "PROMPT.md").read_text()
        self.assertIn("PORT THE DATA", prompt)
        self.assertIn(".rodata 60.5%", prompt)
        self.assertIn("don't try to match them", prompt)
        self.assertNotIn("FINISH THE OPEN FUNCTIONS", prompt)
        self.assertNotIn("{", prompt.replace("{0}", ""))  # every placeholder filled

    def test_budgets(self):
        self.assertEqual(W.limit(dict(SPEC, mode="oneshort")), 10800)
        self.assertEqual(W.limit(dict(SPEC, mode="dataprep")), 7200)

    def test_oneshort_goes_on_to_link_once_its_function_matches(self):
        def respond(c):
            if "check.py battle" in c and "--final" not in c:
                return "MarioSt/battle/ac/ac_pendulum_crane_timing: 7/7\n  100.00%    100 B  _get_angle_rate\n"
            return "rc=0" if "pi --mode json" in c else ""
        with self.fake(respond), mock.patch.object(W.time, "sleep"):
            W.run("t", SPEC["unit"], SPEC["src"], dict(SPEC, mode="oneshort", hyp="h"), lambda m: None)
        links = [c for c in self.calls if "LINKING part of PROMPT.md" in " ".join(map(str, c))]
        self.assertEqual(len(links), 3)  # unlinked after each pass: up to the three continuation passes

    def test_dataprep_gets_one_pass(self):
        with self.fake(lambda c: "rc=0" if "pi --mode json" in c else ""), mock.patch.object(W.time, "sleep"):
            W.run("t", SPEC["unit"], SPEC["src"], dict(SPEC, mode="dataprep"), lambda m: None)
        passes = [c for c in self.calls if "pi --mode json" in " ".join(map(str, c))]
        self.assertLessEqual(len(passes), 2)  # the pass, and at most the finisher
        self.assertFalse(any("Keep going" in " ".join(map(str, c)) for c in passes))


class Sandbox(unittest.TestCase):
    def setUp(self):
        setup_globals(tempfile.gettempdir())

    def test_agent_sandbox(self):
        s = W.sandbox("t", "run", "agent")
        for part in ["MemorySwapMax=", "MemoryHigh=", "/opt/node/bin:", '--tmpfs "$HOME"', "--tmpfs /run/user/1000", "/dev/null /run/docker.sock",
                     "/dev/null /run/dbus/system_bus_socket", "--unshare-pid", "--unshare-ipc", "--die-with-parent",
                     "--clearenv", "--ro-bind / /", "--slice=ttyd-agents.slice", "MemoryMax=", "TasksMax=",
                     "--bind $HOME/ttyd-agents/wt_t $HOME/ttyd-agents/wt_t", "--bind $HOME/ttyd-agents-priv/wt_t/tmp /tmp"]:
            self.assertIn(part, s)
        # nothing the agent can rename is a writable bind source except the workspace itself
        self.assertNotIn("wt_t/tmp /tmp", s.replace("ttyd-agents-priv/wt_t/tmp /tmp", ""))
        self.assertNotIn("ttyd-agents-out", s)

    def test_collect_sandbox_sees_the_workspace_read_only(self):
        s = W.sandbox("t", "collect", "collect")
        self.assertIn("--ro-bind $HOME/ttyd-agents/wt_t $HOME/ttyd-agents/wt_t", s)
        self.assertIn("--bind $HOME/ttyd-agents-out/wt_t $HOME/ttyd-agents-out/wt_t", s)
        self.assertNotIn("--bind $HOME/ttyd-agents/wt_t ", s)

    def test_bind_sources_are_checked_for_links(self):
        with tempfile.TemporaryDirectory() as home:
            for d in ("ttyd-agents/wt_t", "ttyd-agents-priv/wt_t/tmp", "ttyd-agents-priv/wt_t/pi-agent"):
                os.makedirs(f"{home}/{d}")
            check = W.bind_check("t", "agent")
            env = dict(os.environ, HOME=home)
            self.assertEqual(subprocess.run(["bash", "-c", check], env=env, capture_output=True).returncode, 0)
            os.rmdir(f"{home}/ttyd-agents-priv/wt_t/tmp")
            os.symlink(home, f"{home}/ttyd-agents-priv/wt_t/tmp")
            self.assertEqual(subprocess.run(["bash", "-c", check], env=env, capture_output=True).returncode, 4)

    def test_archive_path(self):
        self.assertEqual(W.archive_rel("~/ttyd-archive/x"), "ttyd-archive/x")
        self.assertEqual(W.archive_rel("$HOME/ttyd-archive/x"), "ttyd-archive/x")
        with self.assertRaises(SystemExit):
            W.archive_rel("/etc/x")

    @unittest.skipUnless(subprocess.run(["which", "timeout"], capture_output=True).returncode == 0, "needs timeout(1)")
    def test_step_shell_gates_on_binds_and_leaves_nothing_behind(self):
        with tempfile.TemporaryDirectory() as home:
            for d in ("ttyd-agents/wt_t", "ttyd-agents-out/wt_t", "ttyd-agents-priv/wt_t/tmp", "ttyd-agents-priv/wt_t/pi-agent"):
                os.makedirs(f"{home}/{d}")
            env = dict(os.environ, HOME=home)
            with mock.patch.object(W, "sandbox", return_value=""):
                cmd = W.box_cmd("t", "check0", "echo ran")
            t0 = time.time()
            r = subprocess.run(["bash", "-c", cmd], env=env, capture_output=True, text=True, timeout=20)
            # capture_output waits for every holder of stdout: a watchdog that kept it would hang here
            self.assertLess(time.time() - t0, 5)
            self.assertEqual((r.returncode, r.stdout.strip()), (0, "ran"))
            os.rmdir(f"{home}/ttyd-agents-priv/wt_t/tmp")
            os.symlink(home, f"{home}/ttyd-agents-priv/wt_t/tmp")
            r = subprocess.run(["bash", "-c", cmd], env=env, capture_output=True, text=True, timeout=20)
            self.assertEqual((r.returncode, r.stdout.strip()), (4, ""))

    def test_deadline(self):
        now = time.mktime((2026, 10, 4, 23, 0, 0, 0, 0, -1))
        self.assertEqual(W.parse_deadline("01:30", now) - now, 2.5 * 3600)
        self.assertEqual(W.parse_deadline("23:30", now) - now, 1800)
        self.assertEqual(W.parse_deadline("2026-10-05 12:00", now) - now, 13 * 3600)
        self.assertIsNone(W.parse_deadline(""))


if __name__ == "__main__":
    unittest.main()
