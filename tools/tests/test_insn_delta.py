"""tools/insn_delta.py: instruction-count gaps between the original and ours."""
import collections
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import insn_delta  # noqa: E402


def sym(*mns):
    return {"instructions": [{"instruction": {"parts": [{"opcode": {"mnemonic": m}}]}} for m in mns]
            + [{"row": "blank"}]}


class InsnDelta(unittest.TestCase):
    def test_opcodes(self):
        self.assertEqual(insn_delta.opcodes(sym("lfs", "fsubs", "lfs")), collections.Counter(lfs=2, fsubs=1))

    def test_delta(self):
        orig = insn_delta.opcodes(sym("lfs", "fsubs", "bl"))
        ours = insn_delta.opcodes(sym("lfs", "lfs", "fsubs", "fsubs", "fsubs", "bl"))
        self.assertEqual(insn_delta.delta(orig, ours), (3, "+2 fsubs, +1 lfs"))
        self.assertEqual(insn_delta.delta(ours, orig), (-3, "-2 fsubs, -1 lfs"))

    def test_same_count_different_ops(self):
        n, desc = insn_delta.delta(insn_delta.opcodes(sym("bne", "b")), insn_delta.opcodes(sym("beq", "nop")))
        self.assertEqual(n, 0)
        self.assertIn("-1 bne", desc)


if __name__ == "__main__":
    unittest.main()
