"""Tests for tools/cleanup_index.py: what each category counts, and comment stripping."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import cleanup_index  # noqa: E402

SRC = '''#include "x.h"
/* unk_12 in a block comment */
// field_0x4 in a line comment
void f(Mario* m) {
    *(f32*)((u8*)m + 0x8C) = 1.0f;
    s32 x = float_0_804198f4;
#ifdef MUST_MATCH
    goto done;
#endif
done:
    return;
}
#pragma opt_lifetimes off
#pragma once
'''


class CleanupIndex(unittest.TestCase):
    def test_counts(self):
        lines, counts = cleanup_index.count_text(SRC)
        self.assertEqual(counts["raw_offset"], 1)
        self.assertEqual(counts["address_name"], 1)
        self.assertEqual(counts["must_match"], 1)
        self.assertEqual(counts["goto"], 1)
        self.assertEqual(counts["opt_pragma"], 1)
        self.assertNotIn("unk_name", counts)
        self.assertNotIn("field_0x", counts)
        self.assertNotIn("inline_asm", counts)
        self.assertEqual(lines, sum(1 for l in SRC.splitlines() if l.strip()) - 2)

    def test_index_is_weighted_per_kloc(self):
        s = cleanup_index.summarise([("src/a.c", SRC)])
        expected = sum(cleanup_index.WEIGHTS[c] * n for c, n in s["files"]["src/a.c"]["counts"].items())
        self.assertEqual(s["weighted"], expected)
        self.assertAlmostEqual(s["index"], 1000 * expected / s["lines"], places=2)

    def test_scope(self):
        self.assertTrue(cleanup_index.in_scope("src/battle/a.c"))
        self.assertTrue(cleanup_index.in_scope("include/evt/a.h"))
        self.assertTrue(cleanup_index.in_scope("rels/mri/src/mri_puni.c"))
        self.assertFalse(cleanup_index.in_scope("rels/mri/include/a.h"))
        self.assertFalse(cleanup_index.in_scope("config/G8MJ01/splits.txt"))


if __name__ == "__main__":
    unittest.main()
