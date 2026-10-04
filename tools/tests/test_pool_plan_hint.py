"""Tests for tools/pool_plan.py's i2f_swapped(): the I2F HELPER hint fires only when a string follows the conversion double
in the target and our build pools that string before the double.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import unittest
from pathlib import Path

_src = (Path(__file__).resolve().parent.parent / "pool_plan.py").read_text()
_ns = {}
exec(_src[_src.index("def blob_key"):_src.index("def solve_named")] + _src[_src.index("def i2f_swapped"):_src.index("def plan")], _ns)
i2f_swapped = _ns["i2f_swapped"]

D = b"\x43\x30\x00\x00\x00\x00\x00\x00"
S = b"FireSmoke\0\0\0"


def tgt(*pairs):
    return [dict(kind=k, data=d) for k, d in pairs]


def ours(*blobs):
    return [(i * 8, "n", b) for i, b in enumerate(blobs)]


class HintTest(unittest.TestCase):
    def test_swapped(self):
        self.assertTrue(i2f_swapped(tgt(("magic", D), ("str", S)), ours(S, D)))

    def test_already_in_order(self):
        self.assertFalse(i2f_swapped(tgt(("magic", D), ("str", S)), ours(D, S)))

    def test_string_before_double_in_target(self):
        self.assertFalse(i2f_swapped(tgt(("str", S), ("magic", D)), ours(S, D)))

    def test_missing_in_ours(self):
        self.assertFalse(i2f_swapped(tgt(("magic", D), ("str", S)), ours(S)))


if __name__ == "__main__":
    unittest.main()
