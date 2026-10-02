"""Tests for tools/data_check.py: which data objects a diff touches, and the byte comparison."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import data_check  # noqa: E402

SRC = '''#include "x.h"
const Vec vec3_a = { 0.0f, 4.0f, 0.0f };
extern const f32 float_b;
static s32 tbl[3] = {
    1, 2,
    3,
};
u8 Pos[] ATTRIBUTE_ALIGN(32) = {0x01, 0x00};
void f(void) {
    int x = 3;
    if (x == 2) { x = 4; }
}
const char* names[2] = { "a;b", "c{" };
typedef struct S { int a; } S;
static EVT_BEGIN(script)
    CALL(x)
EVT_END()
__declspec(section ".sdata2") const GXColor dat = { 0xFF, 0xFF, 0xFF, 0x00 };
#define MACRO(x) \\
    y = x;
'''


class Definitions(unittest.TestCase):
    def test_finds_initialised_data_only(self):
        names = [n for _, _, n in data_check.data_definitions(SRC)]
        self.assertEqual(sorted(names), ["Pos", "dat", "names", "script", "tbl", "vec3_a"])

    def test_spans_cover_multi_line_tables_and_scripts(self):
        spans = {n: (a, b) for a, b, n in data_check.data_definitions(SRC)}
        self.assertEqual(spans["tbl"], (4, 7))
        self.assertEqual(spans["script"], (15, 17))

    def test_touched_objects_follow_the_changed_lines(self):
        self.assertEqual(data_check.touched_objects(SRC, {5}), ["tbl"])
        self.assertEqual(data_check.touched_objects(SRC, {2, 16}), ["vec3_a", "script"])
        self.assertEqual(data_check.touched_objects(SRC, {10}), [])


def vec(*floats):
    return struct.pack(">3f", *floats)


class Compare(unittest.TestCase):
    def test_wrong_float_in_a_table_is_reported(self):
        ours = {"v": (".rodata", vec(4, 0, 0), {})}
        target = {"v": (".rodata", vec(0, 4, 0), {})}
        (problem,) = data_check.compare(ours, target, ["v"])
        self.assertIn("v [.rodata+0x0]", problem)
        self.assertIn("40800000", problem)

    def test_equal_objects_and_unknown_names_pass(self):
        obj = {"v": (".rodata", vec(0, 4, 0), {})}
        self.assertEqual(data_check.compare(obj, dict(obj), ["v", "missing"]), [])

    def test_relocated_words_are_masked_on_either_side(self):
        ours = {"p": (".data", b"\0\0\0\0\0\0\0\x05", {0: 1})}
        target = {"p": (".data", b"\x80\x12\x34\x56\0\0\0\x05", {0: 1})}
        self.assertEqual(data_check.compare(ours, target, ["p"]), [])
        raw = {"p": (".data", b"\x80\x12\x34\x56\0\0\0\x05", {})}  # the address written as a number
        self.assertEqual(data_check.compare(raw, target, ["p"]), [])
        wrong = {"p": (".data", b"\0\0\0\0\0\0\0\x06", {0: 1})}
        self.assertTrue(data_check.compare(wrong, target, ["p"]))

    def test_objects_of_different_size_are_compared_over_the_common_length(self):
        ours = {"s": (".sdata2", b"Spark\0", {})}
        target = {"s": (".sdata2", b"Spark\0\0\0", {})}
        self.assertEqual(data_check.compare(ours, target, ["s"]), [])
        merged = {"s": (".sdata2", b"Sparx\0\0\0", {})}
        self.assertTrue(data_check.compare(ours, merged, ["s"]))


if __name__ == "__main__":
    unittest.main()
