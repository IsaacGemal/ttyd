"""Tests for tools/perm_check.py: first_error() and plausible(), the checks perm_prepare runs on a prepared base.c.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import perm_check  # noqa: E402


def lines(*mn):
    return [f"  {i * 4:x}:\t00 00 00 00\t{m}\tr3, r4" for i, m in enumerate(mn)]


class PlausibleTest(unittest.TestCase):
    def test_same_function_is_plausible(self):
        a = lines(*["lwz", "stw", "addi", "bl"] * 10)
        self.assertIsNone(perm_check.plausible(a, a))

    def test_missing_function(self):
        self.assertIn("missing", perm_check.plausible([], lines("blr")))

    def test_much_shorter_base_names_the_inline_helper(self):
        why = perm_check.plausible(lines(*["lwz"] * 311), lines(*["lwz"] * 2133))
        self.assertIn("311", why)
        self.assertIn("2133", why)

    def test_unlike_mnemonics(self):
        self.assertIn("mnemonic", perm_check.plausible(lines(*["lwz", "stw"] * 20), lines(*["fmuls", "bl"] * 20)))


class FirstErrorTest(unittest.TestCase):
    def test_finds_the_error_with_context(self):
        out = "### mwcceppc.exe Compiler:\n#    File: a.c\n# ------\n#      24: x = y;\n#   Error:   ^\n#   undefined identifier 'y'\n"
        msg = perm_check.first_error(out)
        self.assertIn("undefined identifier", msg)
        self.assertIn("x = y", msg)

    def test_no_error_line(self):
        self.assertEqual(perm_check.first_error("something odd"), "something odd")


if __name__ == "__main__":
    unittest.main()
