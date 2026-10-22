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
    void* y = lbl_8012ABCD;
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
    def test_pointer_retypes(self):
        text = """void f(Mario* m, void* p) {
    Npc* n = (Npc*)p;
    f32 a = *(f32*)&m->speed;
    u32 b = (u32)&m->pos;
    u8* c = (u8*)p;
    s32 d = *(s32*)((u8*)m + 0x10);
    Work* w = (Work*)0x80412345;
    g(sizeof(Npc*));
    evt((s32)"name", (s32)n);
}
void proto(Npc*);
"""
        counts = cleanup_index.count_text(text)[1]
        self.assertEqual(counts["struct_ptr_cast"], 1)
        self.assertEqual(counts["deref_cast"], 1)
        self.assertEqual(counts["ptr_to_int"], 1)
        self.assertEqual(counts["prim_ptr_cast"], 1)
        self.assertEqual(counts["raw_offset"], 1)
        self.assertEqual(counts["abs_address"], 1)

    def test_counts(self):
        lines, counts = cleanup_index.count_text(SRC)
        self.assertEqual(counts["raw_offset"], 1)
        self.assertEqual(counts["address_name"], 1)
        self.assertEqual(counts["pool_constant"], 1)
        self.assertEqual(counts["must_match"], 1)
        self.assertEqual(counts["goto"], 1)
        self.assertEqual(counts["opt_pragma"], 1)
        self.assertNotIn("unk_name", counts)
        self.assertNotIn("field_0x", counts)
        self.assertNotIn("inline_asm", counts)
        self.assertEqual(lines, sum(1 for l in SRC.splitlines() if l.strip()) - 2)

    def test_pool_constants_and_unk_names_are_not_address_names(self):
        text = "a = float_0p5_80123456 + double_to_int_80300468 + zero_80420520 + negone_8041A6B8 + unk_80123456;\n"
        lines, counts = cleanup_index.count_text(text)
        self.assertEqual(counts["pool_constant"], 4)
        self.assertEqual(counts["unk_name"], 1)
        self.assertNotIn("address_name", counts)
        s = cleanup_index.summarise([("src/a.c", text)])
        self.assertEqual(s["categories"]["pool_constant"]["weight"], 0)
        self.assertEqual(s["weighted"], 1)

    def test_offset_macro_calls_count_and_define_does_not(self):
        text = """#define WW_S32(p, off) (*(s32*)((u8*)(p) + (off)))
#define WW_F32(p, off) \\
    (*(f32*)((u8*)(p) + (off)))
#define NOT_OFFSET(p) ((u8*)(p) + 4)
s32 a = WW_S32(work, 0x214);
s32 b = WW_S32(work, 0x218) + WW_S32(work, 0x21C);
f32 c = WW_F32(work, 0x20);
u8* d = (u8*)NOT_OFFSET(work);
"""
        lines, counts = cleanup_index.count_text(text)
        self.assertEqual(counts["raw_offset"], 1 + 4)  # NOT_OFFSET's body, plus four calls of the two offset macros
        self.assertEqual(lines, sum(1 for l in text.splitlines() if l.strip()))

    def test_offset_from_a_parameter_makes_an_offset_macro(self):
        text = """#define NKF(off) (*(f32*)((u8*)party + (off)))
#define A14L(i) (((s32*)((u8*)yuwp + work->mode * 24))[5 + (i)])
#define ANIM_FRAME_TIME(f) (*(f32*)((u8*)(f) + 0x0))
#define DRAW(id_) \\
    { \\
        load(**(TEXPalette***)(*(u8**)((u8*)work + 0x28) + 0xA0), (id_)); \\
    }
f32 a = NKF(0x10) + NKF(0x14);
s32 b = A14L(1);
f32 c = ANIM_FRAME_TIME(f0) + ANIM_FRAME_TIME(f1);
DRAW(3); DRAW(4);
"""
        self.assertEqual(sorted(cleanup_index.offset_macros(text)), ["A14L", "NKF"])
        # three offset macro calls, plus the accessor's and DRAW's #define bodies once each
        self.assertEqual(cleanup_index.count_text(text)[1]["raw_offset"], 3 + 2)

    def test_other_offset_spellings(self):
        text = """u8* p = (u8*)puni;
s32 a = *(s32*)(p + 0x1E0);
f32 b = *(f32*)((char*)work + 0x10);
s8 c = *((s8*)src + 2);
u8 d = ((u8*)ac)[0x29D];
void* e = (u8*)_battleWorkPointer + 0x16F24;
Footmark* f = (Footmark*)(footmarkBuf + 0x500);
"""
        self.assertEqual(cleanup_index.count_text(text)[1]["raw_offset"], 6)

    def test_what_the_other_spellings_leave_alone(self):
        text = """const char* s = (char*)msg + 1;
BinaryFontBlock* b = (BinaryFontBlock*)(pfh + 1);
u8 r = ((u8*)&color)[0];
*(u8*)p += 1;
AnimData* a = (AnimData*)(off + (u32)data);
s16 v = (s16)(*(s16*)a + 0xF0);
"""
        self.assertNotIn("raw_offset", cleanup_index.count_text(text)[1])

    def test_offset_macro_in_another_spelling(self):
        text = """#define EM_VEC(off, sym) (*(Vec*)(base + (off)))
Vec v = EM_VEC(0x10, a);
Vec w = EM_VEC(0x1C, b);
"""
        self.assertEqual(cleanup_index.count_text(text)[1]["raw_offset"], 2)

    def test_macro_defined_twice_is_not_an_offset_macro(self):
        text = """#define WW_S32(p, off) (*(s32*)((u8*)(p) + (off)))
#define WW_S32(p, off) (*(s32*)((u8*)(p) + (off)))
s32 a = WW_S32(work, 0x214);
"""
        self.assertEqual(cleanup_index.offset_macros(text), {})
        self.assertEqual(cleanup_index.count_text(text)[1]["raw_offset"], 2)  # the two defines, not the call

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
