"""tools/wave_launch.py: the reaper, deadlines on every remote call, slot accounting, and the sandbox command."""
import os
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import wave_launch as W  # noqa: E402

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
            self.assertFalse(W.reserve("t"))
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 200)):
            self.assertFalse(W.reserve("t"))
        with self.fake(out="FULL"):
            self.assertFalse(W.reserve("t"))
        with self.fake(out="TAKEN"):
            self.assertTrue(W.reserve("t"))

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
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 180)):
            self.assertFalse(W.reap("t"))

    def test_one_slot_pool_for_every_key(self):
        self.assertEqual(W.slot_file("t"), "$HOME/ttyd-agents-slots/all/abc123_wt_t.slot")

    def test_earlier_results_keep_the_newest_two(self):
        here = Path(self.tmp.name)
        for k in range(4):
            (here / "t").mkdir()
            (here / "t" / "final.c").write_text(str(k))
            with mock.patch.object(W.subprocess, "run") as trash:
                W.set_aside("t")
        self.assertEqual(sorted(p.name for p in here.glob("t.prev*")), ["t.prev1", "t.prev2", "t.prev3", "t.prev4"])
        trashed = [c.args[0][1] for c in trash.call_args_list]
        self.assertEqual([os.path.basename(t) for t in trashed], ["t.prev1", "t.prev2"])

    def test_cap_hits_are_recorded(self):
        c = W.caps_check("t", "run")
        self.assertIn("limits.txt", c)
        self.assertIn(str(W.EVENTS_MAX), c)

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
        self.assertIn("|| m=over", c)
        self.assertIn("sleep 10", c)
        self.assertIn(f"head -c {W.EVENTS_MAX}", c)
        self.assertIn(f"head -c {W.STDERR_MAX}", c)
        self.assertIn("kt $wd", c)
        self.assertIn("du -smx", W.box_cmd("t", "check0", "true"))

    def test_hung_ssh_raises_instead_of_waiting(self):
        with self.fake(exc=subprocess.TimeoutExpired("ssh", 1)):
            with self.assertRaises(subprocess.TimeoutExpired):
                W.prepare("t", SPEC)
            W.reap("t")  # swallows its own deadline


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
