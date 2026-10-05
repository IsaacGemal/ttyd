import os
import re
import unittest

# finish_unit.py runs on import; take just the pure rewrite
_src = open(os.path.join(os.path.dirname(__file__), "..", "finish_unit.py")).read()
_ns = {"re": re}
exec(_src[_src.index("def to_literals"):_src.index("def literal_strings")], _ns)
to_literals = _ns["to_literals"]


class ToLiterals(unittest.TestCase):
    def test_decimal_and_hex_sizes(self):
        t = ('extern const char str_A[];\nconst char str_A[] = "a";\nconst char str_B[0x18] = "MOBJ_b";\n'
             'void f(void) {\n    g(str_A, str_B);\n}\n')
        self.assertEqual(to_literals(t, {}), 'void f(void) {\n    g("a", "MOBJ_b");\n}\n')

    def test_extern_with_declspec_definition_keeps_its_name(self):
        # the extern path must not rewrite the definition itself into `char "SFX"[0x18]`
        t = ('extern const char str_C[];\n__declspec(section ".rodata") char str_C[0x18] = "SFX";\n'
             'void f(void) {\n    g(str_C);\n}\n')
        self.assertEqual(to_literals(t, {"str_C": "SFX"}), t)

    def test_extern_only_string_from_the_asm(self):
        t = 'extern const char str_D[];\nvoid f(void) {\n    g(str_D);\n}\n'
        self.assertEqual(to_literals(t, {"str_D": "d"}), 'void f(void) {\n    g("d");\n}\n')


if __name__ == "__main__":
    unittest.main()
