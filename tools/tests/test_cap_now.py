"""tools/wave_launch.py: the nightly permuter window lowers the agent cap from 01:00 to 07:00."""
import os
import sys
import unittest
from unittest import mock

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import wave_launch  # noqa: E402


class CapNow(unittest.TestCase):
    def cap_at(self, hhmm):
        with mock.patch.object(wave_launch.time, "strftime", return_value=hhmm):
            return wave_launch.cap_now()

    def test_window_boundaries(self):
        self.assertEqual(self.cap_at("00:59"), wave_launch.CAP)
        for t in ("01:00", "03:30", "06:59"):
            self.assertEqual(self.cap_at(t), wave_launch.QUIET_CAP, t)
        self.assertEqual(self.cap_at("07:00"), wave_launch.CAP)
        self.assertEqual(self.cap_at("19:00"), wave_launch.CAP)

    def test_defaults(self):
        self.assertEqual((wave_launch.QUIET_FROM, wave_launch.QUIET_TO, wave_launch.QUIET_CAP), ("01:00", "07:00", 8))


if __name__ == "__main__":
    unittest.main()
