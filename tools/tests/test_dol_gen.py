"""Tests for tools/dol_gen.py's --append source splitter (split_source): pragma pairs and preprocessor blocks must stay
with the function they wrap when the chunks are reordered.

  python3 -m unittest discover tools/tests     (from the repo root)
"""
import re
import unittest
from pathlib import Path

_src = (Path(__file__).resolve().parent.parent / "dol_gen.py").read_text()
_a = _src.index("PRAGMA_STATE = ")
_b = _src.index("if APPEND:\n    pre_text")
_ns = {"re": re}
exec(_src[_a:_b], _ns)
split_source = _ns["split_source"]

SOURCE = """#include "a.h"
s32 g;

#pragma opt_propagation off // needed: first
USER_FUNC(first) {
    return 1;
}
#pragma opt_propagation reset

USER_FUNC(second) {
    return 2;
}

#ifdef MUST_MATCH
static inline s32 helper(s32 x) {
    return x;
}
#endif

#pragma opt_propagation off // needed: third
#pragma dont_inline on
void third(void) {
#ifdef MUST_MATCH
    if (g) {
        g = 1;
    }
#else
    g = 2;
#endif
}
#pragma dont_inline reset
#pragma opt_propagation reset
"""


class SplitSource(unittest.TestCase):
    def setUp(self):
        self.pre, self.chunks, self.tail = split_source(SOURCE)
        self.by = dict(self.chunks)

    def test_finds_every_function_and_the_helper(self):
        self.assertEqual([n for n, _ in self.chunks], ["first", "second", "helper", "third"])

    def test_pragma_pair_wraps_only_its_function(self):
        self.assertEqual(self.by["first"].split("\n")[0], "#pragma opt_propagation off // needed: first")
        self.assertTrue(self.by["first"].rstrip().endswith("#pragma opt_propagation reset"))
        self.assertNotIn("#pragma", self.by["second"])
        self.assertNotIn("#pragma", self.by["helper"])

    def test_two_pragmas_are_both_wrapped_and_reset(self):
        t = self.by["third"]
        self.assertIn("#pragma opt_propagation off // needed: third", t)
        self.assertIn("#pragma dont_inline on", t)
        self.assertEqual(t.count("#pragma opt_propagation reset"), 1)
        self.assertEqual(t.count("#pragma dont_inline reset"), 1)

    def test_ifdef_blocks_survive_whole(self):
        h = self.by["helper"]
        self.assertTrue(h.startswith("#ifdef MUST_MATCH") and h.rstrip().endswith("#endif"))
        t = self.by["third"]
        for line in ("#ifdef MUST_MATCH", "#else", "#endif", "        g = 1;", "    g = 2;"):
            self.assertIn(line, t)

    def test_nothing_is_left_between_chunks(self):
        self.assertEqual(self.tail.strip(), "")
        self.assertEqual(self.pre.strip().split("\n")[:2], ['#include "a.h"', "s32 g;"])

    def test_reordering_keeps_the_pairs_balanced(self):
        text = "\n\n".join(c for _, c in reversed(self.chunks))
        for k in ("opt_propagation", "dont_inline"):
            ons = len(re.findall(rf"#pragma {k} (?:on|off)\b", text))
            self.assertEqual(ons, len(re.findall(rf"#pragma {k} reset", text)))
        self.assertEqual(len(re.findall(r"(?m)^#if", text)), len(re.findall(r"(?m)^#endif", text)))


if __name__ == "__main__":
    unittest.main()
