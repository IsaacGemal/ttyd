import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import named_greedy as ng  # noqa: E402

SRC = ("extern const f32 float_8_8041d0a0;\n"
       "void other(f32 x) {\n    g(x + 16.0f);\n}\n"
       "void f(f32 x) {\n    g(x + 16.0f, 16.0f + x, x + float_8_8041d0a0, 0.5f, 3.25f);\n}\n")
NAMES = {16.0: "float_16_8041d0e4", 8.0: "float_8_8041d0a0", 0.5: "float_0p5_8041d06c"}


class Sites(unittest.TestCase):
    def test_literal_sites_only_in_the_function_and_only_named_values(self):
        sites = ng.literal_sites(SRC, "f", NAMES)
        self.assertEqual([s[2] for s in sites], ["float_16_8041d0e4", "float_16_8041d0e4", "float_0p5_8041d06c"])
        a, _ = ng.func_span(SRC, "f")
        self.assertTrue(all(s[0] > a for s in sites))

    def test_named_sites(self):
        sites = ng.named_sites(SRC, "f", {"float_8_8041d0a0": 8.0})
        self.assertEqual([s[2] for s in sites], ["8.0f"])

    def test_user_func_header(self):
        src = "static USER_FUNC(npc_jump);\nstatic USER_FUNC(npc_jump) {\n    g(16.0f);\n}\n"
        a, b = ng.func_span(src, "npc_jump")
        self.assertTrue(src[a:].startswith("static USER_FUNC(npc_jump) {"))
        self.assertEqual([s[2] for s in ng.literal_sites(src, "npc_jump", NAMES)], ["float_16_8041d0e4"])

    def test_literal_formatting(self):
        self.assertEqual([ng.literal(v) for v in (16.0, 0.5, -4.0)], ["16.0f", "0.5f", "-4.0f"])


class Apply(unittest.TestCase):
    def test_to_named_adds_an_extern_once(self):
        out = ng.apply(SRC, "f", ng.literal_sites(SRC, "f", NAMES)[0])
        self.assertIn("extern const f32 float_16_8041d0e4;\nvoid f(f32 x) {", out)
        self.assertIn("g(x + float_16_8041d0e4, 16.0f + x", out)
        out2 = ng.apply(out, "f", ng.literal_sites(out, "f", NAMES)[0])
        self.assertEqual(out2.count("extern const f32 float_16_8041d0e4;"), 1)

    def test_known_extern_is_not_redeclared(self):
        lit = ng.named_sites(SRC, "f", {"float_8_8041d0a0": 8.0})[0]
        out = ng.apply(SRC, "f", lit)
        self.assertIn("x + 8.0f", out)
        self.assertEqual(out.count("extern const f32 float_8_8041d0a0;"), 1)

    def test_other_functions_are_untouched(self):
        out = ng.apply(SRC, "f", ng.literal_sites(SRC, "f", NAMES)[0])
        self.assertIn("void other(f32 x) {\n    g(x + 16.0f);", out)


if __name__ == "__main__":
    unittest.main()
