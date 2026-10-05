"""tools/store_sweep.py: stores and constants only one side has."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import store_sweep  # noqa: E402


def ins(mn, *args):
    parts = [{"opcode": {"mnemonic": mn}}]
    for a in args:
        if isinstance(a, int):
            parts.append({"arg": {"signed": a}})
        elif a == "RELOC":
            parts.append({"arg": {"reloc": {}}})
        else:
            parts.append({"arg": {"opaque": a}})
    return {"parts": parts}


class StoreSweep(unittest.TestCase):
    def test_keys(self):
        self.assertEqual(store_sweep.key(ins("stw", "r0", 0x110, "r28")), ("store", "stw", 0x110))
        self.assertEqual(store_sweep.key(ins("stwu", "r1", -0x20, "r3")), ("store", "stw", -0x20))
        self.assertIsNone(store_sweep.key(ins("stw", "r0", 0x8, "r1")))  # stack slot
        self.assertEqual(store_sweep.key(ins("li", "r4", -1)), ("li", -1))
        self.assertIsNone(store_sweep.key(ins("li", "r3", "RELOC")))  # sda21 symbol address
        self.assertIsNone(store_sweep.key(ins("lwz", "r3", 0x4, "r5")))

    def test_missing_store_is_reported(self):
        orig = [("store", "stw", 0x108), ("store", "stw", 0x110), ("li", -1)]
        ours = [("store", "stw", 0x108)]
        oa, ob, shift = store_sweep.compare(orig, ours)
        self.assertEqual(oa, {("store", "stw", 0x110): 1, ("li", -1): 1})
        self.assertEqual(ob, {})
        self.assertIsNone(shift)

    def test_width_change_is_reported(self):
        oa, ob, _ = store_sweep.compare([("store", "stw", 0x1C)], [("store", "stfs", 0x1C)])
        self.assertEqual((oa, ob), ({("store", "stw", 0x1C): 1}, {("store", "stfs", 0x1C): 1}))

    def test_uniform_offset_delta_is_a_shift(self):
        orig = [("store", "sth", 0x1E0), ("store", "sth", 0x1E2)]
        ours = [("store", "sth", 0x2), ("store", "sth", 0x4)]
        self.assertEqual(store_sweep.compare(orig, ours)[2], -0x1DE)

    def test_lone_zero_ignored_unless_asked(self):
        self.assertEqual(store_sweep.compare([], [("li", 0)])[:2], ({}, {}))
        self.assertEqual(store_sweep.compare([], [("li", 0)], zeros=True)[1], {("li", 0): 1})


if __name__ == "__main__":
    unittest.main()
