#include "eff/n64/eff_lensflare_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>
#include <stdlib.h>

// .sdata2 constants live in this translation unit
// (see config/G8MJ01/symbols.txt).
extern const f32 float_60_8041f614;
extern const f32 float_0p1_8041f628;
extern const f32 float_6p2832_8041f62c;
extern const f32 float_90_8041f630;
extern const f32 float_70_8041f634;
extern const f32 float_10_8041f638;
extern const f32 float_360_8041f63c;
extern const f32 float_255_8041f640;
extern const f32 float_80_8041f644;
extern const f32 float_0p4_8041f648;
extern const f32 float_0p95_8041f5e8;
extern const f32 float_deg2rad_8041f5ec;
extern const f32 float_0p0625_8041f5f0;
extern const f32 float_0_8041f5f4;
extern const f32 float_0p03125_8041f5f8;
extern const f32 float_0p0078125_8041f5fc;
extern const f32 float_5_8041f600;
extern const f32 float_2p3_8041f604;
extern const f32 float_14_8041f608;
extern const f32 float_20_8041f60c;
extern const f32 float_0p05_8041f610;
extern const f32 float_1_8041f618;
extern const f32 float_2_8041f61c;
extern const f32 float_34_8041f620;
extern const f32 float_15_8041f624;

// .sdata2 GXColor tints
extern GXColor dat_8041f5d0;
extern GXColor dat_8041f5d4;
extern GXColor dat_8041f5d8;
extern GXColor dat_8041f5dc;
extern GXColor dat_8041f5e0;
extern GXColor dat_8041f5e4;

// .data vertex arrays, local to this file.
static u32 size48x48_tex32x32_vtx[0x38 / 4] = {
    0xFFE8FFE8, 0x00000000, 0x00000000, 0x00FF0018, 0xFFE80000, 0x04000000, 0x000000FF,
    0x00180018, 0x00000400, 0x04000000, 0x00FFFFE8, 0x00180000, 0x00000400, 0x000000FF,
};
static u32 size10x10_tex32x32_vtx[0xE0 / 4] = {
    0x0000FFFB, 0x00000000, 0x00000000, 0x00FF000A, 0xFFFB0000, 0x04000000, 0x000000FF, 0x000A0005,
    0x00000400, 0x04000000, 0x00FF0000, 0x00050000, 0x00000400, 0x000000FF,
    0x0000FFFB, 0x00000000, 0x04000000, 0x00FF000A, 0xFFFB0000, 0x04000400, 0x000000FF, 0x000A0005,
    0x00000400, 0x08000000, 0x00FF0000, 0x00050000, 0x00000800, 0x000000FF,
    0xFFFBFFFB, 0x00000000, 0x08000000, 0x00FF0005, 0xFFFB0000, 0x04000800, 0x000000FF, 0x00050005,
    0x00000400, 0x0C000000, 0x00FFFFFB, 0x00050000, 0x00000C00, 0x000000FF,
    0xFFFBFFFB, 0x00000000, 0x0C000000, 0x00FF0005, 0xFFFB0000, 0x04000C00, 0x000000FF, 0x00050005,
    0x00000400, 0x10000000, 0x00FFFFFB, 0x00050000, 0x00001000, 0x000000FF,
};

// .rodata
const Vec vec3_802faac8 = { 0.0f, 0.0f, 0.0f };
const char str_802FAAE0[16] = "LensflareN64";
EffectEntry* effLensflareN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2) {
    EffectEntry* effect;
    LensflareData* data;
    LensflareEntry* entry;
    s32 i;

    effect = effEntry();
    effect->type = str_802FAAE0;
    effect->count = 6;
    data = __memAlloc(HEAP_EFFECT, sizeof(LensflareData));
    effect->userdata = data;
    effect->callback = effLensflareMain;
    effect->flags |= 2;
    data->field_4 = type;
    data->field_1C = 0;
    if (a1 <= 0) {
        data->field_18 = 100;
    } else {
        data->field_18 = a1;
    }
    data->field_14 = 0;
    data->x = x;
    data->y = y;
    data->z = z;

    entry = data->entries;
    for (i = 1; i < 6; i++) {
        entry->field_20 = (f32)(rand() % 360);
        entry->field_2C = (f32)((i * 2) % 20 + 60);
        entry->field_28 = float_0p1_8041f628 * (f32)(rand() % 16) + float_0p4_8041f648;
        entry->field_30 = float_0p1_8041f628 * (f32)(rand() % 11) + float_0p1_8041f628;
        entry++;
    }
    return effect;
}

// The original walks the header fields and the entry array through ONE pointer: it loads userdata into
// `data`, uses it as a LensflareData for the header, then steps it once (`for (i = 1, data++; ...)`) into
// the entries. Keeping a separate `entry` variable makes MWCC spill userdata to a caller-saved register
// (r6) and materialise the loop pointer with `addi r31, r6, 0x34`, which flips the later register roles.
void effLensflareMain(EffectEntry* effect) {
    LensflareEntry* data = (LensflareEntry*)effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the relaxed Vec in a scratch local and copies it to a
       second local before dispCalcZ; a single local folds the copy away. */
    Vec position;
#endif
    Vec disp = vec3_802faac8;
    f32 angle;
    s32 i;
#ifdef MUST_MATCH
    /* MUST_MATCH: reading field_18 and field_1C once each keeps them in r0 and r3 as the original does;
       reading the fields directly in each test reloads them and swaps the two registers. */
    s32 fade;
    s32 frame;
#endif

    disp.x = ((LensflareData*)data)->x;
    disp.y = ((LensflareData*)data)->y;
    disp.z = ((LensflareData*)data)->z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        ((LensflareData*)data)->field_18 = 30;
    }
    if (((LensflareData*)data)->field_18 < 100) {
        ((LensflareData*)data)->field_18--;
    }
    ((LensflareData*)data)->field_1C++;
#ifdef MUST_MATCH
    fade = ((LensflareData*)data)->field_18;
    if (fade < 0) {
        effDelete(effect);
        return;
    }
    frame = ((LensflareData*)data)->field_1C;
    if (frame <= 20) {
        ((LensflareData*)data)->field_14 = frame * 12;
    } else if (fade < 20) {
        ((LensflareData*)data)->field_14 = fade * 12;
    }
#else
    if (((LensflareData*)data)->field_18 < 0) {
        effDelete(effect);
        return;
    }
    if (((LensflareData*)data)->field_1C <= 20) {
        ((LensflareData*)data)->field_14 = ((LensflareData*)data)->field_1C * 12;
    } else if (((LensflareData*)data)->field_18 < 20) {
        ((LensflareData*)data)->field_14 = ((LensflareData*)data)->field_18 * 12;
    }
#endif

    for (i = 1, data++; i < effect->count; i++, data++) {
        angle = float_6p2832_8041f62c * (float_90_8041f630 * (data->field_2C - float_70_8041f634) / float_10_8041f638) / float_360_8041f63c;
        // field_14 is inside LensflareEntry's padding; the overlapping LensflareData layout names it.
        ((LensflareData*)data)->field_14 = (s32)(float_255_8041f640 * (f32)cos(angle));
        data->field_2C += data->field_30;
        if (data->field_2C > float_80_8041f644) {
            data->field_2C = float_60_8041f614;
            data->field_28 = float_0p1_8041f628 * (f32)(rand() % 16) + float_0p4_8041f648;
#ifdef MUST_MATCH
            /* MUST_MATCH: the natural extern form below makes MWCC CSE 0.1f across the rand() call into
               a callee-saved f24, which grows the frame by 0x10. The literal rematerialises instead. */
            data->field_30 = 0.1f * (f32)(rand() % 11) + 0.1f;
#else
            data->field_30 = float_0p1_8041f628 * (f32)(rand() % 11) + float_0p1_8041f628;
#endif
            data->field_20 = (f32)(rand() % 360);
        }
    }
#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effLensflareDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effLensflareDisp, effect);
#endif
}

#ifdef MUST_MATCH
// Setting each TEV colour inside a void inline reproduces the original's
// per-call by-value colour slots (build/copy pairs, interleaved with the
// alpha computation). A natural inline block instead spills every colour to
// its own slot and drops Disp from 100% to 99.72%.
static inline void lensflareSetColor1(s32 a) {
    GXColor color = dat_8041f5d0;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void lensflareSetColor2(s32 a) {
    GXColor color = dat_8041f5d4;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void lensflareSetColor3(s32 a) {
    GXColor color = dat_8041f5d8;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void lensflareSetColor4(s32 a) {
    GXColor color = dat_8041f5dc;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void lensflareSetColor5(s32 a) {
    GXColor color = dat_8041f5e4;
    color.a = a;
    GXSetTevColor(1, color);
}

static inline void lensflareSetColor6(void) {
    GXSetTevColor(2, dat_8041f5e0);
}
#endif

// MWCC assigns the callee-saved GPRs partly by declaration order: `cam` must be
// declared before `alpha` so cam lands in r27 and alpha in r26 (the loop counter
// then reuses cam's r27).  The Mtx/GXTexObj relative order (trans, scale, rot,
// base, texObj) fixes the stack slots, so the GPR declarations are interleaved
// among them rather than grouped.
void effLensflareDisp(CameraId cameraId, void* param) {
    CameraEntry* cam;
    EffectEntry* effect;
    Mtx trans;
    Mtx scale;
    Mtx rot;
    LensflareEntry* data;
    s32 frame;
    Mtx base;
    GXTexObj texObj;
    f32 f30;
    s32 i;
    s32 alpha;

    effect = param;
    cam = camGetPtr(cameraId);
    data = effect->userdata;
    frame = ((LensflareData*)data)->field_1C;
    alpha = ((LensflareData*)data)->field_14;
    if (frame & 1) {
        alpha = (s32)(float_0p95_8041f5e8 * (f32)alpha);
    }
    PSMTXTrans(trans, ((LensflareData*)data)->x, ((LensflareData*)data)->y, ((LensflareData*)data)->z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041f5ec * -camGetPtr(CAMERA_3D)->viewYaw);
    PSMTXConcat(trans, rot, base);
    PSMTXConcat(cam->view, base, base);
#ifdef MUST_MATCH
    lensflareSetColor1(alpha);
#else
    {
        GXColor color = dat_8041f5d0;
        color.a = alpha;
        GXSetTevColor(1, color);
    }
#endif
    PSMTXRotRad(rot, 'z', float_deg2rad_8041f5ec * (f32)(frame + (frame & 1) * 2));
    PSMTXConcat(base, rot, rot);
    GXLoadPosMtxImm(rot, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xff);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 2, 12, 8, 15);
    GXSetTevAlphaIn(0, 7, 1, 4, 7);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
    PSMTXScale(scale, float_0p0625_8041f5f0, float_0p0625_8041f5f0, float_0_8041f5f4);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effGetTexObjN64(0x6f, &texObj);
    GXLoadTexObj(&texObj, 0);
#ifdef MUST_MATCH
    lensflareSetColor2(alpha);
#else
    {
        GXColor color = dat_8041f5d4;
        color.a = alpha;
        GXSetTevColor(1, color);
    }
#endif
    effSetVtxDescN64((effVtxArray*)size48x48_tex32x32_vtx);
    GXBegin(0x90, 0, 6);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    effGetTexObjN64(0x86, &texObj);
    GXLoadTexObj(&texObj, 0);
    PSMTXScale(scale, float_0p03125_8041f5f8, float_0p0078125_8041f5fc, float_0_8041f5f4);
    GXLoadTexMtxImm(scale, 0x1e, 1);
    effSetVtxDescN64((effVtxArray*)size10x10_tex32x32_vtx);

    for (i = 1, data++; i < effect->count; i++, data++) {
        f30 = data->field_28;
        if (f30 == float_0_8041f5f4) {
            continue;
        }
#ifdef MUST_MATCH
        lensflareSetColor3(((((LensflareData*)data)->field_14 * alpha) >> 8));
#else
        {
            GXColor color = dat_8041f5d8;
            color.a = (((LensflareData*)data)->field_14 * alpha) >> 8;
            GXSetTevColor(1, color);
        }
#endif
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xff);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 15, 15, 15, 2);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041f5ec * data->field_20);
        PSMTXTrans(trans, data->field_2C, float_0_8041f5f4, float_0_8041f5f4);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, f30, f30, f30);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(12, 13, 14, 0, 12, 14, 15, 0);

        PSMTXRotRad(rot, 'z', float_deg2rad_8041f5ec * data->field_20);
        PSMTXTrans(trans, float_5_8041f600 + data->field_2C, float_0_8041f5f4, float_0_8041f5f4);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, float_2p3_8041f604 - f30, float_2p3_8041f604 - f30, float_2p3_8041f604 - f30);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(8, 9, 10, 0, 8, 10, 11, 0);

#ifdef MUST_MATCH
        lensflareSetColor4(((((LensflareData*)data)->field_14 * alpha) >> 9));
        lensflareSetColor6();
#else
        {
            GXColor color = dat_8041f5dc;
            color.a = (((LensflareData*)data)->field_14 * alpha) >> 9;
            GXSetTevColor(1, color);
        }
        GXSetTevColor(2, dat_8041f5e0);
#endif
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 0xff);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 2, 4, 8, 15);
        GXSetTevAlphaIn(0, 7, 1, 4, 7);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041f5ec * (float_14_8041f608 + data->field_20));
        PSMTXRotRad(trans, 'y', float_deg2rad_8041f5ec * (f32)(frame + (i * 10) % 45));
        PSMTXConcat(rot, trans, rot);
        PSMTXTrans(trans, float_20_8041f60c, float_0_8041f5f4, float_0_8041f5f4);
        PSMTXConcat(rot, trans, trans);
        {
            f32 t = float_0p05_8041f610 * (data->field_2C - float_60_8041f614);
            PSMTXScale(scale, float_1_8041f618 + t, float_2_8041f61c - t, float_1_8041f618);
        }
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(4, 5, 6, 0, 4, 6, 7, 0);

#ifdef MUST_MATCH
        lensflareSetColor5(((((LensflareData*)data)->field_14 * alpha) >> 8));
#else
        {
            GXColor color = dat_8041f5e4;
            color.a = (((LensflareData*)data)->field_14 * alpha) >> 8;
            GXSetTevColor(1, color);
        }
#endif
        {
            f32 a = float_34_8041f620 + data->field_20;
            PSMTXRotRad(rot, 'z', float_deg2rad_8041f5ec * (a + (f32)frame));
        }
        PSMTXRotRad(trans, 'y', float_deg2rad_8041f5ec * (f32)(i * 35));
        PSMTXConcat(rot, trans, rot);
        PSMTXTrans(trans, float_15_8041f624, float_0_8041f5f4, float_0_8041f5f4);
        PSMTXConcat(rot, trans, trans);
        PSMTXScale(scale, float_0p1_8041f628 * (data->field_2C - float_60_8041f614) + float_2_8041f61c,
                   float_1_8041f618, float_1_8041f618);
        PSMTXConcat(trans, scale, trans);
        PSMTXConcat(base, trans, trans);
        GXLoadPosMtxImm(trans, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x90, 0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_0p95_8041f5e8 = 0.95f;
const f32 float_deg2rad_8041f5ec = 0.017453292f;
const f32 float_0p0625_8041f5f0 = 0.0625f;
const f32 float_0_8041f5f4 = 0.0f;
const f32 float_0p03125_8041f5f8 = 0.03125f;
const f32 float_0p0078125_8041f5fc = 0.0078125f;
const f32 float_5_8041f600 = 5.0f;
const f32 float_2p3_8041f604 = 2.3f;
const f32 float_14_8041f608 = 14.0f;
const f32 float_20_8041f60c = 20.0f;
const f32 float_0p05_8041f610 = 0.05f;
const f32 float_60_8041f614 = 60.0f;
const f32 float_1_8041f618 = 1.0f;
const f32 float_2_8041f61c = 2.0f;
const f32 float_34_8041f620 = 34.0f;
const f32 float_15_8041f624 = 15.0f;
const f32 float_0p1_8041f628 = 0.1f;
const f32 float_6p2832_8041f62c = 6.2831855f;
const f32 float_90_8041f630 = 90.0f;
const f32 float_70_8041f634 = 70.0f;
const f32 float_10_8041f638 = 10.0f;
const f32 float_360_8041f63c = 360.0f;
const f32 float_255_8041f640 = 255.0f;
const f32 float_80_8041f644 = 80.0f;
const f32 float_0p4_8041f648 = 0.4f;
