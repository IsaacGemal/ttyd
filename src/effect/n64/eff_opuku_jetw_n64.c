#include "eff/n64/eff_opuku_jetw_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_803A33D8[2] = { 0xBF800000, 0x3F800000 };

// .data
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A33E8[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_373[2] = { 1.0f, -1.0f };
__declspec(section ".data") f32 lbl_803A33F8[2] = { -1.0f, 1.0f };
__declspec(section ".data") static f32 negone_one_360[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A3408[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 one_negone_359[2] = { 1.0f, -1.0f };
// The two spiral tables hold 10-byte effOpukuJetwPoint entries.
u32 adata[150] = {
    0x0000001E, 0x008C0000, 0xFF000006, 0x001E0078,
    0x0000FA00, 0x010C001E, 0x006EFFFB, 0xF0000111,
    0x00320064, 0xFFECF000, 0x0216003C, 0x005AFFE2,
    0xF000021A, 0x0046005A, 0xFFD8F000, 0x031C0050,
    0x0055FFCE, 0xF0000420, 0x005A005A, 0xFFC4F000,
    0x05230064, 0x0064FFBA, 0xF0000726, 0x006E006E,
    0xFFB0F000, 0x08280078, 0x0078FFA6, 0xF0000B2A,
    0x00820082, 0xFF9CF000, 0x0E2B0096, 0x0096FF92,
    0xF000122C, 0x00BE00BE, 0xFF88F000, 0x152D00E6,
    0x00E6FF7E, 0xF000182E, 0x010E010E, 0xFF74F000,
    0x1B2F0136, 0x0136FF6A, 0xF0001E30, 0x015E015E,
    0xFF60C800, 0x2131017C, 0x017CFF56, 0x8C002432,
    0x01900190, 0xFF4C2800, 0x0000001E, 0x008C0000,
    0xFF000006, 0x001E0078, 0x0000F000, 0xFF0C001E,
    0x006E0005, 0xE600FF11, 0x00320064, 0x0014DC00,
    0xFE16003C, 0x005A001E, 0xD200FE1A, 0x0046005A,
    0x0028C800, 0xFD1C0050, 0x00550032, 0xBE00FC20,
    0x005A0050, 0x003CB400, 0xFB230064, 0x004D0046,
    0xAA00F926, 0x006E0041, 0x0050A000, 0xF8280078,
    0x0036005A, 0x9600F52A, 0x0082003C, 0x00648C00,
    0xF22B008C, 0x0046006E, 0x8200EE2C, 0x00960050,
    0x00787800, 0xEB2D00A0, 0x005A0082, 0x6E00E82E,
    0x00AA0064, 0x008C5A00, 0xE52F00B4, 0x006E0096,
    0x5000E230, 0x00BE0078, 0x00A04600, 0xDF3100C8,
    0x008200AA, 0x1E00DC32, 0x00D2008C, 0x00B40A00,
    0x0000001E, 0x008C0000, 0xFF000006, 0x001E0078,
    0x0000F000, 0x000C001E, 0x006E0005, 0xE6000011,
    0x00320064, 0x0014DC00, 0x0016003C, 0x005A001E,
    0xD200001A, 0x0046005A, 0x0028C800, 0x001C0050,
    0x00550032, 0xBE000020, 0x005A0050, 0x003CB400,
    0x00230064, 0x004D0046, 0xAA000026, 0x006E0041,
    0x0050A000, 0x00280078, 0x0036005A, 0x9600002A,
    0x0082003C, 0x00648C00, 0x002B008C, 0x0046006E,
    0x8200002C, 0x00960050, 0x00787800, 0x002D00A0,
    0x005A0082, 0x6E00002E, 0x00AA0064, 0x008C5A00,
    0x002F00B4, 0x006E0096, 0x50000030, 0x00BE0078,
    0x00A04600, 0x003100C8, 0x008200AA, 0x1E000032,
    0x00D2008C, 0x00B40A00,
};
u32 a2data[83] = {
    0x0000001E, 0x008C0000, 0xFF000006, 0x001E0078,
    0x0000F000, 0x010C001E, 0x006EFFFB, 0xE6000111,
    0x00320064, 0xFFECDC00, 0x0216003C, 0x005AFFE2,
    0xD200021A, 0x0046005A, 0xFFD8C800, 0x031C0050,
    0x0055FFCE, 0xBE000420, 0x005A0050, 0xFFC4B400,
    0x05230064, 0x004DFFBA, 0x78000726, 0x006E0041,
    0xFFB05000, 0x08280078, 0x0036FFA6, 0x28000000,
    0x001E008C, 0x0000FF00, 0x0006001E, 0x00780000,
    0xF000FF0C, 0x001E006E, 0x0005E600, 0xFF110032,
    0x00640014, 0xDC00FE16, 0x003C005A, 0x001ED200,
    0xFE1A0046, 0x005A0028, 0xC800FD1C, 0x00500055,
    0x0032BE00, 0xFC20005A, 0x0050003C, 0xB400FB23,
    0x0064004D, 0x00467800, 0xF926006E, 0x00410050,
    0x5000F828, 0x00780036, 0x005A2800, 0x0000001E,
    0x008C0000, 0xFF000006, 0x001E0078, 0x0000F000,
    0x000C001E, 0x006E0005, 0xE6000011, 0x00320064,
    0x0014DC00, 0x0016003C, 0x005A001E, 0xD200001A,
    0x0046005A, 0x0028C800, 0x001C0050, 0x00550032,
    0xBE000020, 0x005A0050, 0x003CB400, 0x00240064,
    0x004D0046, 0x78000027, 0x006E0041, 0x00505000,
    0x002A0078, 0x0036005A, 0x28000000,
};
static u32 size16x16_tex32x32_vtx[14] = {
    0xFFF8FFF8, 0x00000000, 0x0000FFFF, 0xFFFF0008,
    0xFFF80000, 0x04000000, 0xFFFFFFFF, 0x00080008,
    0x00000400, 0x0400FFFF, 0xFFFFFFF8, 0x00080000,
    0x00000400, 0xFFFFFFFF,
};
u32 size6x22_tex32x32_vtx[14] = {
    0xFFFD0000, 0x00000000, 0x0000FFFF, 0xFFFF0003,
    0x00000000, 0x04000000, 0xFFFFFFFF, 0x00030016,
    0x00000400, 0x0400FFFF, 0xFFFFFFFD, 0x00160000,
    0x00000400, 0xFFFFFFFF,
};

// .sdata
s32 dat_ry[8] = { 0, 0x28, 0x37, 0xBE, 0x14, 0x82, 0x10E, 0x12C };

// .sbss2 zero GXColor scratch objects and the 4-byte section-tail gap. Objects land in reverse
// declaration order, so the gap is declared before unk_804231d0.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_804231D4_sbss2[4];
__declspec(section ".sdata") GXColor unk_804231d0;
__declspec(section ".sdata") GXColor unk_804231cc;
__declspec(section ".sdata") GXColor unk_804231c8;
#pragma section ".sdata" ".sbss"

// Local copy of system.c's inline helper so it inlines here (as in eff_kemuri11.c).
inline Vec initializeVec(Vec* input) {
    Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
    temp.x = input->x;
    temp.y = input->y;
    temp.z = input->z;
    return temp;
}

EffectEntry* effOpukuJetwN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* effect = effEntry();
    effOpukuJetwData* data;

#ifdef MUST_MATCH
    // The original's pooled effect-name string is a 16-byte object: "OpukuJetwN64" followed by three NULs.
    effect->type = "OpukuJetwN64\0\0\0";
#else
    effect->type = "OpukuJetwN64";
#endif
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x44);
    effect->userdata = data;
    effect->callback = effOpukuJetwMain;
    effect->flags |= 2;

    data->type = type;
    data->counter = 0;
    if (duration <= 0) {
        data->lifetime = 1000;
    } else {
        data->lifetime = duration;
    }

    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->position.y = y;
    data->position.z = z;
    data->scale = scale;
    data->field_0x18 = 0x46;
    data->field_0x1C = 0xB4;
    data->field_0x20 = 0xFF;
    data->field_0x28 = 0xFF;
    data->field_0x2C = 0xFF;
    data->field_0x30 = 0xFF;
    data->field_0x38 = 0.0f;
    data->field_0x3C = 1.0f;
    data->field_0x40 = 1.0f;

    return effect;
}

void effOpukuJetwMain(EffectEntry* effect) {
    effOpukuJetwData* data = effect->userdata;
    Vec position = initializeVec(&data->position);

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->counter++;
    if (data->lifetime < 0) {
        effDelete(effect);
    } else {
        if (data->counter < 8) {
            data->field_0x24 = data->counter * 32 + 31;
        }
        dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effOpukuJetwDisp, effect);
    }
}

// The frame size (0x180), the Mtx slots (work/rot/scale/trans at
// 0x40/0x70/0xa0/0xd0), the switch layout, the loop's `rowOffset += columns, j++` bottom test and the
// three GXColor call slots (0x18/0x1c, 0x10/0x14, 0x8/0xc) all match, as does every other
// callee-saved value including the `.data` section base (r20). The one remaining difference is the
// colouring of `rowOffset` and `colPtr`: the target keeps `rowOffset` in r19 and gives the cam
// register r15, once cam is dead, to the shared col*10/`colPtr` web; this source does the reverse.
// Moving either declaration (individually or together), changing their types (u32/s16/void*/s32),
// splitting `colPtr`/`ptr`/`rowOffset` into per-block or inner-scope copies, reordering the loop
// statements and comparisons, and opt_lifetimes / opt_propagation / opt_common_subs /
// opt_dead_assignments / optimize_for_size / peephole / global_optimizer / pool_data pragmas were all
// tried; each gives the same swap or worse. All candidate compilers (GC/1.3..GC/2.7) generate
// identical code; GC/3.0 is worse. This is a permuter colouring tie.
// Building each TEV colour and calling GXSetTevColor inside one inline makes mwcc allocate the
// colour's build slot and its by-value argument copy together, matching the original's slots.
static inline void effOpukuJetwSetTev1(effOpukuJetwData* data, s32 field24) {
    GXColor color = unk_804231c8;
    color.r = data->field_0x28;
    color.g = data->field_0x2C;
    color.b = data->field_0x30;
    color.a = field24;
    GXSetTevColor(GX_TEVREG1, color);
}

static inline void effOpukuJetwSetTev0a(s32 field18, s32 field1c, s32 field20, s32 field24) {
    GXColor color = unk_804231cc;
    color.r = field18;
    color.g = field1c;
    color.b = field20;
    color.a = field24;
    GXSetTevColor(GX_TEVREG0, color);
}

static inline void effOpukuJetwSetTev0b(s32 field18, s32 field1c, s32 field20, s32 alpha) {
    GXColor color = unk_804231d0;
    color.r = field18;
    color.g = field1c;
    color.b = field20;
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
}

#pragma opt_strength_reduction off // without it MWCC strength-reduces the rowPtr induction and hoists columns*10
#pragma opt_loop_invariants off // without it the hoisted col*10/rowPtr live across calls and spill half/rowOffset
// The original's loop shapes: a running counterCurrent (the target computes counter + i*2 once and steps
// it by 2), rowPtr = &table[rowOffset] built before the two `continue` tests, ptr = &rowPtr[col] sharing
// one col*10 with colPtr = &table[col], and the second test as `columns - 1 - col > lifetime` off the
// cached local. With plain -O4 MWCC strength-reduces the rowPtr induction and hoists columns*10; these two
// pragmas stop that.
// The Mtx declarations keep `scale` before `rot` so the two temporaries land in the original's 0x70/0xa0
// slots (the allocator places Mtx locals in reverse declaration order), the field assignments load
// field_0x24 before the other colours as the original does, `dat_ry[(j + i) & 7]` gives the original's
// operand order in the run-angle sum, and `for (j = 0, rowOffset = 0; ...)` gives its loop-init order.
// The declaration order below holds the callee-saved GPR map for every other value (counter r14,
// counterCurrent r18, threshold r17, cam r15, ptr r16, col r21, table r22, rows r23, columns r24,
// field20 r25, field1c r26, field18 r27, field24 r28, i r29, j r30, lifetime r31).
// NONMATCHING (99.84%): all instructions, operands and stack slots match; `rowOffset` and `colPtr` are
// swapped between r15 and r19 (target: rowOffset r19, colPtr r15).
void effOpukuJetwDisp(CameraId camId, void* param) {
    s32 counter;
    s32 counterCurrent;
    s32 threshold;
    CameraEntry* cam;
    effOpukuJetwPoint* ptr;
    EffectEntry* effect;
    s32 rowOffset;
    effOpukuJetwPoint* colPtr;
    s32 lifetime;
    Mtx trans;
    s32 j;
    Mtx scale;
    s32 i;
    s32 field24;
    s32 field18;
    Mtx rot;
    s32 field1c;
    s32 field20;
    s32 columns;
    s32 rows;
    effOpukuJetwData* data;
    effOpukuJetwPoint* table;
    s32 half;
    s32 col;
    Mtx work;
    GXTexObj texObj;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    counter = data->counter;
    lifetime = data->lifetime;
    field24 = data->field_0x24;
    field18 = data->field_0x18;
    field1c = data->field_0x1C;
    field20 = data->field_0x20;
    switch (data->type) {
        case 0:
            table = (effOpukuJetwPoint*)adata;
            columns = 20;
            rows = 3;
            break;
        default:
            table = (effOpukuJetwPoint*)a2data;
            columns = 11;
            rows = 3;
            break;
    }

    PSMTXTrans(trans, data->position.x, data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXScale(scale, data->scale, data->scale, data->scale);
    PSMTXConcat(trans, rot, trans);
    PSMTXConcat(trans, scale, work);
    PSMTXConcat(cam->view, work, work);

    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    effGetTexObjN64(0x34, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
    GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
    GXSetCullMode(GX_CULL_NONE);

    PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x38);
    PSMTXScale(scale, data->field_0x3C, data->field_0x40, 1.0f);
    PSMTXConcat(rot, scale, scale);
    PSMTXConcat(work, scale, work);

    effOpukuJetwSetTev1(data, field24);

    if (columns - 1 <= lifetime) {
        effOpukuJetwSetTev0a(field18, field1c, field20, field24);
        GXLoadPosMtxImm(work, 0);
        GXSetCurrentMtx(0);
        effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }

    threshold = counter * 5;
    half = columns / 2;
    counterCurrent = counter;
    for (i = 0; i < half; i++) {
        col = counterCurrent % columns;
        for (j = 0, rowOffset = 0; j < rows; rowOffset += columns, j++) {
            effOpukuJetwPoint* rowPtr = &table[rowOffset];
            if (col > threshold) {
                continue;
            }
            if (columns - 1 - col > lifetime) {
                continue;
            }
            ptr = &rowPtr[col];
            PSMTXTrans(trans, (f32)ptr->field_0x0, 10.f + (f32)ptr->field_0x1, 0.0f);
            colPtr = &table[col];
            PSMTXScale(scale, (f32)colPtr->field_0x2 * 0.01f, (f32)colPtr->field_0x4 * 0.01f, 1.0f);
            PSMTXConcat(trans, scale, trans);
            PSMTXRotRad(rot, 'z', 0.017453292f * ((f32)ptr->field_0x6 + (f32)dat_ry[(j + i) & 7]));
            PSMTXConcat(trans, rot, trans);
            PSMTXConcat(work, trans, trans);
            GXLoadPosMtxImm(trans, 0);
            GXSetCurrentMtx(0);
            effOpukuJetwSetTev0b(field18, field1c, field20, (field24 * colPtr->field_0x8) >> 8);
            effSetVtxDescN64((effVtxArray*)size16x16_tex32x32_vtx);
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
            tri2(0, 1, 2, 0, 0, 2, 3, 0);
        }
        counterCurrent += 2;
    }
}
#pragma opt_strength_reduction reset
#pragma opt_loop_invariants reset
