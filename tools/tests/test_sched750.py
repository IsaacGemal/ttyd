"""Tests for tools/sched750.py (MWCC 2.6's list scheduler on the 750 model, from tw2004, CC0) on a saved mwcc-debugger dump
(badgeShop_set, GC/2.6): `validate` must reproduce the compiler's real schedule for nearly every integer-only block, and a
block that mixes floating-point ops is reported as not modelled rather than mis-scheduled.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import subprocess
import sys
import unittest
from pathlib import Path

TOOLS = Path(__file__).resolve().parent.parent
FIX = Path(__file__).resolve().parent / "fixtures" / "mwccdbg_sched"


def run(*a):
    return subprocess.run([sys.executable, str(TOOLS / "sched750.py"), *a], capture_output=True, text=True)


class SchedTest(unittest.TestCase):
    def test_validate_reproduces_the_compilers_schedules(self):
        out = run("validate", str(FIX)).stdout.strip()
        ok, _, total = out.split()[0], out.split()[1], out.split()[2]
        self.assertGreaterEqual(int(total), 15)
        self.assertGreaterEqual(int(ok) / int(total), 0.9)

    def test_validate_reproduces_floating_point_blocks(self):
        out = run("validate", str(Path(FIX).parent / "mwccdbg_sched_fp")).stdout
        self.assertIn("of which 2 of 2 blocks with floating-point ops", out)

    def test_validate_skips_a_dump_directory_without_scheduling_files(self):
        import tempfile
        with tempfile.TemporaryDirectory() as d:
            self.assertEqual(run("validate", d).stdout.strip(), "0 of 0 blocks reproduced")


if __name__ == "__main__":
    unittest.main()
