"""tools/fncast_lint.py: function-pointer casts outside MUST_MATCH."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import fncast_lint  # noqa: E402

CAST = "((void (*)(Vec, Vec, GXColor))winIconSet)(id, pos, scale, color);"


class FncastLint(unittest.TestCase):
    def test_unguarded(self):
        self.assertEqual(fncast_lint.scan("void f(void) {\n    " + CAST + "\n}"), [(2, CAST)])

    def test_guarded(self):
        text = "#ifdef MUST_MATCH\n" + CAST + "\n#else\nwinIconSet(id, &pos, &scale, &color);\n#endif\n"
        self.assertEqual(fncast_lint.scan(text), [])
        self.assertEqual(fncast_lint.scan("#if defined(MUST_MATCH)\n" + CAST + "\n#endif\n"), [])

    def test_else_of_guard(self):
        self.assertEqual(fncast_lint.scan("#ifdef MUST_MATCH\nx();\n#else\n" + CAST + "\n#endif\n"), [(4, CAST)])
        self.assertEqual(fncast_lint.scan("#ifndef MUST_MATCH\nx();\n#else\n" + CAST + "\n#endif\n"), [])
        self.assertEqual(fncast_lint.scan("#ifndef MUST_MATCH\n" + CAST + "\n#endif\n"), [(2, CAST)])

    def test_nested_and_other_conditionals(self):
        text = "#ifdef MUST_MATCH\n#if VERSION > 0\n" + CAST + "\n#endif\n#endif\n#ifdef DEBUG\n" + CAST + "\n#endif\n"
        self.assertEqual(fncast_lint.scan(text), [(7, CAST)])

    def test_not_a_cast(self):
        self.assertEqual(fncast_lint.scan("s32 (*fn)(s32) = f;\nx = (s32)(f);\n// " + CAST + "\n"), [])

    def test_allowlist(self):
        found = {"src/a.c": [(3, CAST), (9, CAST)], "src/b.c": [(1, CAST)]}
        allow = [{"file": "src/a.c", "context": CAST}, {"file": "src/c.c", "context": CAST}]
        new, stale = fncast_lint.check(found, allow)
        self.assertEqual(new, [("src/a.c", 3, CAST), ("src/b.c", 1, CAST)])
        self.assertEqual(stale, [("src/c.c", CAST)])


if __name__ == "__main__":
    unittest.main()
