import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import offset_to_field as otf  # noqa: E402  (libclang not needed for these helpers)


class Masks(unittest.TestCase):
    def test_must_match_both_arms_and_nesting(self):
        src = ("a;\n#ifdef MUST_MATCH\nb;\n#if X\nc;\n#endif\n#else\nd;\n#endif\ne;\n"
               "#if !defined(MUST_MATCH)\nf;\n#endif\n#if Y\ng;\n#elif MUST_MATCH\nh;\n#endif\n")
        m = otf.must_match_mask(src)
        lines = src.split("\n")
        hit = {lines[i - 1] for i in m}
        self.assertEqual(hit, {"b;", "c;", "d;", "f;", "h;"})

    def test_define_with_continuation(self):
        src = "x;\n#define F(p) \\\n    (*(f32*)((u8*)(p) + 4))\ny;\n"
        self.assertEqual(otf.define_mask(src), {2, 3})


class Render(unittest.TestCase):
    def test_simple_base_gets_arrow(self):
        self.assertEqual(otf.render("unit", [".moveSpeedY"]), "unit->moveSpeedY")
        self.assertEqual(otf.render("wp->party[i]", [".pos", ".x"]), "wp->party[i]->pos.x")

    def test_address_of_base_uses_dot(self):
        self.assertEqual(otf.render("&work->hit", [".pos"]), "work->hit.pos")

    def test_calls_and_casts(self):
        self.assertEqual(otf.render("marioGetPtr()", [".flags"]), "marioGetPtr()->flags")
        self.assertEqual(otf.render("(BattleUnit*)p", [".pos"]), "((BattleUnit*)p)->pos")
        self.assertEqual(otf.render("(p)", [".arr", "[2]"]), "p->arr[2]")

    def test_postfix(self):
        self.assertTrue(otf.postfix("f(x)[2].y"))
        self.assertTrue(otf.postfix("(a + b)[0]"))
        self.assertFalse(otf.postfix("(Foo*)x"))
        self.assertFalse(otf.postfix("a + b"))


class Literals(unittest.TestCase):
    def test_hex_and_decimal_only(self):
        self.assertTrue(otf.INT_LIT.fullmatch("0x174"))
        self.assertTrue(otf.INT_LIT.fullmatch("16u"))
        self.assertTrue(otf.INT_LIT.fullmatch("0"))
        self.assertIsNone(otf.INT_LIT.fullmatch("010"))


class ShortEnums(unittest.TestCase):
    def test_only_off_regions_are_packed(self):
        src = ("enum A { a0 };\n#pragma enumsalwaysint off\ntypedef enum B { b0 } B;\nenum { c0 };\n"
               "#pragma enumsalwaysint reset\nenum D { d0 };\n")
        out = otf.pack_short_enums(src)
        self.assertIn("enum A {", out)
        self.assertIn("typedef enum __attribute__((packed)) B {", out)
        self.assertIn("enum __attribute__((packed)) {", out)
        self.assertIn("enum D {", out)


class Dwarf(unittest.TestCase):
    def test_nested_anonymous_members(self):
        dump = ("struct Foo {\n"
                "    float x; // offset 0x0, size 0x4\n"
                "    union {\n"
                "        int a; // offset 0x0, size 0x4\n"
                "        short b[2]; // offset 0x0, size 0x4\n"
                "    } u; // offset 0x4, size 0x4\n"
                "    struct Vec pos; // offset 0x8, size 0xC\n"
                "};\n")
        lay = otf.parse_dwarf_dump(dump)["Foo"]
        self.assertEqual(lay["x"], (0, 4))
        self.assertEqual(lay["u"], (4, 4))
        self.assertEqual(lay["u.a"], (4, 4))
        self.assertEqual(lay["u.b"], (4, 4))
        self.assertEqual(lay["pos"], (8, 12))


if __name__ == "__main__":
    unittest.main()
