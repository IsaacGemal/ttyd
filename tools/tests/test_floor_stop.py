"""tools/floor_stop.py: which scope the disk floor stops, and what it leaves behind."""
import io
import os
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from unittest import mock

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import floor_stop  # noqa: E402


class FloorStop(unittest.TestCase):
    def run_main(self, held, args):
        with tempfile.TemporaryDirectory() as home:
            out = os.path.join(home, "ttyd-agents-out", "w71a_foo")
            os.makedirs(out)
            with mock.patch.object(floor_stop, "held_by_scope", return_value=held), \
                    mock.patch.object(floor_stop.subprocess, "run") as run, \
                    mock.patch.object(floor_stop.os.path, "expanduser", return_value=home), \
                    mock.patch.object(sys, "argv", ["floor_stop.py"] + args), redirect_stdout(io.StringIO()) as said:
                floor_stop.main()
            limits = os.path.join(out, "limits.txt")
            return said.getvalue(), run.call_args_list, open(limits).read() if os.path.exists(limits) else None

    def test_stops_the_largest_holder_above_the_threshold(self):
        said, calls, limits = self.run_main({"ttyd-w71a_foo-run.scope": 3 * 2**30, "ttyd-w71a_bar-cont0.scope": 2**20}, ["--act"])
        self.assertEqual([c.args[0][-1] for c in calls], ["ttyd-w71a_foo-run.scope"])
        self.assertIn("disk floor", limits)  # its results can't be collected as clean
        self.assertIn("stopped ttyd-w71a_foo-run.scope", said)

    def test_below_the_threshold_or_without_act_nothing_is_stopped(self):
        for held, args in (({"ttyd-w71a_foo-run.scope": 500 * 2**20}, ["--act"]),
                           ({"ttyd-w71a_foo-run.scope": 3 * 2**30}, []),
                           ({}, ["--act"])):
            said, calls, limits = self.run_main(held, args)
            self.assertEqual((calls, limits), ([], None), held)
            self.assertIn("nothing stopped", said)


if __name__ == "__main__":
    unittest.main()
