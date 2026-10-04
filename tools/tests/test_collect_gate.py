"""tools/collect_gate.py: what may be merged from an agent's results folder."""
import hashlib
import os
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import collect_gate  # noqa: E402


def write_collect(d, files, audit="0 flagged, 0 outside reads\n"):
    files = dict(files, **{"audit.txt": audit})
    for name, text in files.items():
        open(os.path.join(d, name), "w").write(text)
    with open(os.path.join(d, "COLLECT-OK"), "w") as f:
        for name, text in files.items():
            f.write(f"{hashlib.sha256(text.encode()).hexdigest()} {len(text.encode())} {name}\n")


class Gate(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.d = os.path.join(self.tmp.name, "battle_x")
        os.mkdir(self.d)

    def tearDown(self):
        self.tmp.cleanup()

    def test_clean_collect_passes(self):
        write_collect(self.d, {"final.c": "int x;\n", "other_changes.txt": ""})
        for f in ("PROMPT.md", "LAUNCHED", "lint.txt", "proto_asm.txt"):  # written on this side
            open(os.path.join(self.d, f), "w").write("local")
        self.assertIsNone(collect_gate.check(self.d))

    def test_changed_missing_or_extra_files_fail(self):
        write_collect(self.d, {"final.c": "int x;\n"})
        open(os.path.join(self.d, "final.c"), "w").write("int y;\n")
        self.assertIn("differs", collect_gate.check(self.d))
        write_collect(self.d, {"final.c": "int x;\n", "stderr.log": "big"})
        os.remove(os.path.join(self.d, "stderr.log"))  # e.g. skipped by rsync --max-size
        self.assertIn("missing", collect_gate.check(self.d))
        write_collect(self.d, {"final.c": "int x;\n"})
        open(os.path.join(self.d, "include.patch"), "w").write("+x")
        self.assertIn("not in COLLECT-OK", collect_gate.check(self.d))

    def test_host_pass_boundaries_use_the_existing_manifest_format(self):
        write_collect(self.d, {"final.c": "int x;\n", "passes.txt": "100 124\n200 0\n"})
        self.assertIsNone(collect_gate.check(self.d))
        with open(os.path.join(self.d, "passes.txt"), "w") as f:
            f.write("100 0\n200 0\n")
        self.assertIn("passes.txt: differs", collect_gate.check(self.d))

    def test_limits_flags_and_failed_collects_fail(self):
        write_collect(self.d, {"final.c": "x"})
        open(os.path.join(self.d, "limits.txt"), "w").write("run: events.jsonl reached its cap")
        self.assertIn("limit", collect_gate.check(self.d))
        os.remove(os.path.join(self.d, "limits.txt"))
        write_collect(self.d, {"final.c": "x"}, audit="2 flagged, 0 outside reads\n")
        self.assertIn("flagged", collect_gate.check(self.d))
        os.remove(os.path.join(self.d, "COLLECT-OK"))
        self.assertIn("no COLLECT-OK", collect_gate.check(self.d))

    def test_allow_unaudited_is_per_folder_and_only_for_pre_audit_results(self):
        open(os.path.join(self.d, "final.c"), "w").write("x")
        self.assertIsNotNone(collect_gate.check(self.d))
        for v in ("1", "0", "other_tag"):
            with mock.patch.dict(os.environ, {"ALLOW_UNAUDITED": v}):
                self.assertIsNotNone(collect_gate.check(self.d), v)
        with mock.patch.dict(os.environ, {"ALLOW_UNAUDITED": "battle_x"}):
            self.assertIsNotNone(collect_gate.check(self.d))  # the wave must be named too
        wave_tag = os.path.basename(self.tmp.name) + "/battle_x"
        with mock.patch.dict(os.environ, {"ALLOW_UNAUDITED": wave_tag}):
            self.assertIsNone(collect_gate.check(self.d))
            open(os.path.join(self.d, "LAUNCHED"), "w").write("run")
            self.assertIn("never collected", collect_gate.check(self.d))  # a killed collect isn't pre-audit
            os.remove(os.path.join(self.d, "LAUNCHED"))
            open(os.path.join(self.d, "collect.txt"), "w").write("diff failed")
            self.assertIsNotNone(collect_gate.check(self.d))  # a failed collect is never waved through


if __name__ == "__main__":
    unittest.main()
