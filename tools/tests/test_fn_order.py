"""tools/fn_order.py: the line-chunk reorder (reorder) and its refusals."""
import os
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
import fn_order  # noqa: E402

SRC = """#include "a.h"

// first
#pragma opt_lifetimes off
void a(void) {
    x();
}
#pragma opt_lifetimes reset

extern s32 table[];

#define HELPER(v) ((v) + 1)
static inline s32 b_help(s32 v) {
    return HELPER(v);
}
s32 b(s32 v)
{
    return b_help(v);
}
#undef HELPER

s32 data[2] = { 1,
    2 };

#pragma dont_inline on
void c(s32 x,
       s32 y) {
    a();
}
#pragma dont_inline reset
"""


class Reorder(unittest.TestCase):
    def test_functions(self):
        names = [f[2] for f in fn_order.functions(SRC.split("\n"))]
        self.assertEqual(names, ["a", "b_help", "b", "c"])

    def test_reverse(self):
        out = fn_order.reorder(SRC, ["c", "b", "a"])
        heads = [f[2] for f in fn_order.functions(out.split("\n")) if not f[3]]
        self.assertEqual(heads, ["c", "b", "a"])
        L = out.split("\n")
        # declarations and data are hoisted ahead of every function, in their order
        self.assertLess(L.index("extern s32 table[];"), L.index("s32 data[2] = { 1,"))
        self.assertLess(L.index("    2 };"), L.index("#pragma dont_inline on"))
        # pragma pairs and #define/#undef travel with their function, including the reset at the end of the file
        c_at = L.index("void c(s32 x,")
        self.assertEqual(L[c_at - 1], "#pragma dont_inline on")
        self.assertEqual(L[L.index("}", c_at) + 1], "#pragma dont_inline reset")
        self.assertLess(L.index("#define HELPER(v) ((v) + 1)"), L.index("s32 b(s32 v)"))
        self.assertGreater(L.index("#undef HELPER"), L.index("s32 b(s32 v)"))
        self.assertEqual(sorted(l for l in L if l.strip()), sorted(l for l in SRC.split("\n") if l.strip()))

    def test_spanning_pragma_split(self):
        src = "#pragma opt_lifetimes off\nvoid a(void) {\n}\nvoid b(void) {\n}\n#pragma opt_lifetimes reset\n"
        out = fn_order.reorder(src, ["b", "a"]).split("\n")
        for name in ("void a(void) {", "void b(void) {"):
            i = out.index(name)
            self.assertEqual(out[i - 1], "#pragma opt_lifetimes off")
            self.assertEqual(out[i + 2], "#pragma opt_lifetimes reset")

    def test_straddling_if_refused(self):
        src = "#ifdef X\nvoid a(void) {\n}\n#else\nvoid b(void) {\n}\n#endif\nvoid c(void) {\n}\n"
        with self.assertRaises(ValueError):
            fn_order.reorder(src, ["c", "b", "a"])

    def test_forward_refs(self):
        src = "void a(void) {\n    b(1);\n}\nstatic void b(s32 x) {\n}\n"
        self.assertEqual(fn_order.forward_refs(src, ["a", "b"]), {"b": "static void b(s32 x);"})


if __name__ == "__main__":
    unittest.main()
