"""Tests for tools/rasim.py (replay and search on a saved mwcc-debugger dump) and tools/mwccdbg.py's summary/regmap helpers.

The fixture is the register-allocator dump of eff_timestamp_n64's effTimestampDisp (GC/2.6, 2026-10-04): the replay must
reproduce every register of the compiler's colouring.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parent.parent
FIX = Path(__file__).resolve().parent / "fixtures" / "mwccdbg_timestamp"
sys.path.insert(0, str(TOOLS))
import mwccdbg  # noqa: E402


def rasim(*a):
    return subprocess.run([sys.executable, str(TOOLS / "rasim.py"), str(FIX), *a], capture_output=True, text=True).stdout


class ReplayTest(unittest.TestCase):
    def test_gpr_replay_reproduces_the_compilers_colours(self):
        self.assertTrue(rasim("replay").startswith("pass 1 replay: 0 of 81 registers differ from the dump; priority order the same"))

    def test_fpr_replay_reproduces_the_compilers_colours(self):
        self.assertTrue(rasim("replay", "--fpr").startswith("pass 1 replay: 0 of 37 registers differ"))

    def test_search_reports_the_current_order(self):
        out = rasim("search", "39", "48", "--summary", "--iter", "200")
        self.assertIn("current order: 4 of 47 targets wrong", out)


class LockTest(unittest.TestCase):
    def test_second_holder_waits_for_the_first(self):
        import socket
        import threading
        import time
        free = socket.socket()
        free.bind(("127.0.0.1", 0))
        port = free.getsockname()[1]
        free.close()
        old = mwccdbg.LOCK_PORT
        mwccdbg.LOCK_PORT = port
        try:
            events = []

            def second():
                with mwccdbg.net_lock():
                    events.append(("second", time.time()))

            with mwccdbg.net_lock():
                t = threading.Thread(target=second)
                t.start()
                time.sleep(2.5)
                events.append(("first-done", time.time()))
            t.join(timeout=30)
        finally:
            mwccdbg.LOCK_PORT = old
        self.assertEqual([e[0] for e in events], ["first-done", "second"])


class SummaryTest(unittest.TestCase):
    def test_summary_flags_differing_registers(self):
        with tempfile.TemporaryDirectory() as d:
            for f in FIX.glob("regalloc-gpr-pass-1-assigned.txt"):
                Path(d, f.name).write_text(f.read_text())
            lines = mwccdbg.summary(Path(d), {"r30": "r29", "r28": "r26"})
        self.assertTrue(lines[0].startswith("== regalloc-gpr-pass-1-assigned.txt"))
        self.assertTrue(any("!ORIG r29" in ln for ln in lines))
        self.assertTrue(any("!ORIG r26" in ln for ln in lines))

    def test_summary_without_a_mapping_flags_nothing(self):
        with tempfile.TemporaryDirectory() as d:
            Path(d, "regalloc-gpr-pass-1-assigned.txt").write_text((FIX / "regalloc-gpr-pass-1-assigned.txt").read_text())
            self.assertFalse(any("!ORIG" in ln for ln in mwccdbg.summary(Path(d), {})))


if __name__ == "__main__":
    unittest.main()
