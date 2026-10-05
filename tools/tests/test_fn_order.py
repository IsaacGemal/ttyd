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

    def test_shared_macro_hoisted(self):
        src = ("#pragma dont_inline on\n\n// first\nvoid a(void) {\n}\n\n#ifdef MUST_MATCH\n#define RO(o) (base + (o))\n#else\n"
               "#define RO(o) (sym + (o))\n#endif\nvoid b(void) {\n    RO(1);\n}\nvoid c(void) {\n    RO(2);\n}\n"
               "#pragma dont_inline reset\n")
        out = fn_order.reorder(src, ["c", "b", "a"]).split("\n")
        # the macro block is used by two functions and never #undef'd: it moves ahead of all of them
        self.assertLess(out.index("#define RO(o) (base + (o))"), out.index("void c(void) {"))
        # the dont_inline region (with a blank line above the first function) now wraps each function
        for name in ("void a(void) {", "void b(void) {", "void c(void) {"):
            i = out.index(name)
            self.assertIn("#pragma dont_inline on", out[max(0, i - 3):i])

    def test_if_branches_and_comments(self):
        src = ("void a(void) {\n#ifdef MUST_MATCH\n    if (x) {\n#else\n    if (y) {\n#endif\n        q();\n    }\n}\n"
               "/*\nvoid old(void) {\n}\n*/\n"
               "s32 table[2]; // a declaration with a trailing comment\n"
               "void skipped(void) {\n    r();\n}\n"
               "void b(void) {\n    // uses NAME only in a comment\n}\n")
        names = [f[2] for f in fn_order.functions(src.split("\n"))]
        # the #else branch's `{` does not count, and the commented-out function is not one
        self.assertEqual(names, ["a", "skipped", "b"])
        out = fn_order.reorder(src, ["b", "a"]).split("\n")
        # `skipped` (not emitted by the original) travels whole with the function below it
        self.assertEqual(out[out.index("void skipped(void) {") + 1], "    r();")
        self.assertLess(out.index("s32 table[2]; // a declaration with a trailing comment"), out.index("void b(void) {"))
        self.assertEqual(sorted(l for l in out if l.strip()), sorted(l for l in src.split("\n") if l.strip()))

    def test_header_comment_and_one_line_body(self):
        src = "BOOL a(u32 r3) { // probably an id\n    return r3;\n}\nvoid b(void) {}\nvoid c(void) { /* 1:1 */\n}\n"
        self.assertEqual([f[2] for f in fn_order.functions(src.split("\n"))], ["a", "b", "c"])
        out = fn_order.reorder(src, ["c", "b", "a"]).split("\n")
        self.assertLess(out.index("void c(void) { /* 1:1 */"), out.index("void b(void) {}"))
        self.assertLess(out.index("void b(void) {}"), out.index("BOOL a(u32 r3) { // probably an id"))

    def test_block_comment_in_gap_and_inlined_forward_ref(self):
        src = ("void a(void) {\n    h();\n}\n/*\ntypedef struct T {\n    s32 x;\n} T;\n*/\n"
               "static void h(void) {\n}\nvoid b(void) {\n}\n")
        out = fn_order.reorder(src, ["b", "a"])
        L = out.split("\n")
        # the commented-out typedef stays inside its comment
        self.assertEqual(L[L.index("typedef struct T {") - 1], "/*")
        # h (not emitted by the original) now follows its caller a? then it needs a prototype
        refs = fn_order.forward_refs(out, [f[2] for f in fn_order.functions(L)])
        if L.index("static void h(void) {") > L.index("void a(void) {"):
            self.assertEqual(refs.get("h"), "static void h(void);")

    def test_pragma_stack(self):
        src = ("#pragma dont_inline on\n\n#pragma dont_inline on\nvoid a(void) {\n}\n#pragma dont_inline reset\n\n"
               "void b(void) {\n}\nvoid c(void) {\n}\n#pragma dont_inline reset\n")
        L = src.split("\n")
        # after a's own pair, the file-wide `on` is back in force (reset pops, it does not clear)
        self.assertEqual(fn_order.state_at(L, L.index("void b(void) {"), "pragma"), {"dont_inline": "on"})
        out = fn_order.reorder(src, ["c", "b", "a"])
        self.assertIn("void a(void) {", out)

    def test_data_after_a_pooling_function_stays_after_it(self):
        # a's compound literal is pooled at a's position; D was defined after a, so it must not be hoisted above it
        src = ("void a(void) {\n    g((Vec){ 0.0f, 1.0f, 0.0f });\n}\n"
               "static const Vec D = { 0.0f, -1.0f, 0.0f };\n"
               "void b(void) {\n    h(&D);\n}\n"
               "void c(void) {\n}\n")
        out = fn_order.reorder(src, ["c", "a", "b"])
        L = out.split("\n")
        self.assertLess(L.index("void a(void) {"), L.index("static const Vec D = { 0.0f, -1.0f, 0.0f };"))
        self.assertLess(L.index("static const Vec D = { 0.0f, -1.0f, 0.0f };"), L.index("void b(void) {"))

    def test_data_between_two_poolers_that_swap_is_refused(self):
        src = ("void a(void) {\n    g((Vec){ 0.0f, 1.0f, 0.0f });\n}\n"
               "static const Vec D = { 0.0f, -1.0f, 0.0f };\n"
               "void b(void) {\n    Vec v = { 1.0f, 0.0f, 0.0f };\n    h(&v);\n}\n")
        with self.assertRaises(ValueError):
            fn_order.reorder(src, ["b", "a"])

    def test_data_with_no_pooler_before_it_is_still_hoisted(self):
        src = ("void a(void) {\n}\nstatic const Vec D = { 0.0f, -1.0f, 0.0f };\nvoid b(void) {\n    h(&D);\n}\n")
        out = fn_order.reorder(src, ["b", "a"])
        L = out.split("\n")
        self.assertLess(L.index("static const Vec D = { 0.0f, -1.0f, 0.0f };"), L.index("void b(void) {"))


if __name__ == "__main__":
    unittest.main()
