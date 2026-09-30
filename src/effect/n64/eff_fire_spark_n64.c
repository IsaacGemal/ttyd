#include "eff/n64/eff_fire_spark_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

typedef struct effFireSparkData {
    s32 type;             // 0x00
    f32 field_0x04;       // 0x04
    f32 field_0x08;       // 0x08
    f32 field_0x0C;       // 0x0C
    f32 field_0x10;       // 0x10
    f32 field_0x14;       // 0x14
    s32 life;             // 0x18
    s32 field_0x1C;       // 0x1C
    s32 field_0x20;       // 0x20
    s32 field_0x24;       // 0x24
    s32 field_0x28;       // 0x28
    s32 field_0x2C;       // 0x2C
    s32 field_0x30;       // 0x30
    s32 field_0x34;       // 0x34
    s32 field_0x38;       // 0x38
    s32 field_0x3C;       // 0x3C
    f32 field_0x40;       // 0x40
    f32 field_0x44;       // 0x44
    f32 field_0x48;       // 0x48
    f32 field_0x4C;       // 0x4C
    f32 field_0x50;       // 0x50
    f32 field_0x54;       // 0x54
    f32 field_0x58;       // 0x58
    f32 field_0x5C;       // 0x5C
    f32 field_0x60;       // 0x60
    s32 field_0x64;       // 0x64
    f32 field_0x68;       // 0x68
    f32 field_0x6C;       // 0x6C
} effFireSparkData;

// .sdata2 and .rodata constants named after their original addresses (defined by the split map).
extern const f32 float_30_8041eec0;
extern const f32 float_0p5_8041eeac;
extern const char str_802FA5C8[];
extern const f32 float_deg2rad_8041ee9c;
extern const f32 float_0p03125_8041eea0;
extern const f32 float_0p0078125_8041eea4;
extern const f32 float_0_8041eea8;
extern const f32 float_0p05_8041eeb0;
extern const f32 float_0p1_8041eeb4;
extern const f32 float_0p3_8041eeb8;
extern const f32 float_0p04_8041eebc;
extern const Vec vec3_802fa5b0;
const f64 double_to_int_802fa5c0 = 4503601774854144.0;

// Local to this unit in the original (`.obj scale_data, local`).
static f32 scale_data[30] = {
    0.01f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.65f, 0.8f, 0.95f, 1.0f,
    1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
    1.0f,  0.95f, 0.8f, 0.65f, 0.5f, 0.35f, 0.2f, 0.1f, 0.05f, 0.01f,
};
extern const GXColor dat_8041ee98;
extern GXColor unk_80423100;
extern GXColor unk_80423104;
extern u32 size4x4_tex32x32_vtx[];

EffectEntry* effFireSparkN64Entry(s32 type, s32 count, s32 life, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8, f32 a9, f32 a10, f32 a11) {
    EffectEntry* effect;
    effFireSparkData* data;
    effFireSparkData* ptr;
    f32 ratio;
    s32 i;

    effect = effEntry();
    effect->type = str_802FA5C8;
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, sizeof(effFireSparkData) * count);
    effect->userdata = data;
    effect->callback = effFireSparkMain;
    effect->flags |= 2;

    data->type = type;
    data->field_0x1C = 0;
    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    data->field_0x2C = 0;
    data->field_0x04 = a4;
    data->field_0x08 = a5;
    data->field_0x0C = a6;
    data->field_0x40 = a9;
    data->field_0x10 = a7;
    data->field_0x14 = a8;
    data->field_0x68 = a10;
    data->field_0x6C = a11;
    data->field_0x20 = 0x7f;
    data->field_0x24 = 0;
    data->field_0x28 = 0;
    data->field_0x30 = 0x7f;
    data->field_0x34 = 0x7f;
    data->field_0x38 = 0;
    data->field_0x3C = 0xff;

    ratio = float_30_8041eec0 / (f32)(count - 1);
    ratio *= float_0p5_8041eeac;
    ptr = data + 1;
    for (i = 1; i < count; i++, ptr++) {
        ptr->field_0x64 = (s32)(-(f32)i * ratio) - 1;
    }

    return effect;
}

void effFireSparkMain(EffectEntry* effect) {
    s32 rangeX;
    s32 rangeY;
    effFireSparkData* data = effect->userdata;
#ifdef MUST_MATCH
    /* MUST_MATCH: retail builds the Vec in a scratch slot and copies it into a second
       local before dispCalcZ; a single local folds the copy away (same idiom as effBushMain). */
    Vec position;
#endif
    Vec disp = vec3_802fa5b0;
    s32 i;
    s32 life;
    s32 counter;
    f32 half;
    f32 v68;
    f32 v6C;

    disp.x = data->field_0x04;
    disp.y = data->field_0x08;
    disp.z = data->field_0x0C;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->life = 0x10;
    }

    if (data->life < 1000) {
        data->life--;
    }

    data->field_0x1C++;
    if (data->field_0x1C > 324000) {
        data->field_0x1C = 0x100;
    }

    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    life = data->life;
    counter = data->field_0x1C;
    if (life < 0x10) {
        data->field_0x2C = life * 16;
    }
    if (counter < 0x10) {
        data->field_0x2C = counter * 16 + 15;
    }

    rangeX = (s32)data->field_0x10;
    half = float_0p5_8041eeac * data->field_0x10;
    rangeY = (s32)data->field_0x14;
    v68 = data->field_0x68;
    v6C = data->field_0x6C;

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        data->field_0x64++;
        if (data->field_0x64 >= 0x1E) {
            data->field_0x64 = 0;
        }
        if (data->field_0x64 < 0) {
            continue;
        }
        if (data->field_0x64 == 0) {
            data->field_0x04 = (f32)(rand() % rangeX) - half;
            data->field_0x08 = (f32)(rand() % rangeY);
            data->field_0x0C = float_0_8041eea8;
            data->field_0x44 = float_0p05_8041eeb0 * (f32)(rand() % 20 - 10);
            data->field_0x48 = float_0p05_8041eeb0 * (v68 * (f32)(rand() % 40 - 1));
            data->field_0x4C = float_0_8041eea8;
            data->field_0x50 = float_0p05_8041eeb0 * (f32)(rand() % 20 - 10);
            data->field_0x54 = float_0p05_8041eeb0 * (v6C * (f32)(rand() % 30 - 1));
            data->field_0x58 = float_0_8041eea8;
            data->field_0x5C = (f32)(rand() % 16);
            data->field_0x60 = (f32)(rand() % 16);
        }
        data->field_0x40 = scale_data[data->field_0x64];
        data->field_0x04 += data->field_0x44;
        data->field_0x08 += data->field_0x48;
        data->field_0x0C += data->field_0x4C;
        data->field_0x5C += float_0p1_8041eeb4;
        data->field_0x60 += float_0p3_8041eeb8;
        data->field_0x44 += float_0p04_8041eebc * (data->field_0x50 - data->field_0x44);
        data->field_0x48 += float_0p04_8041eebc * (data->field_0x54 - data->field_0x48);
        data->field_0x4C += float_0p04_8041eebc * (data->field_0x58 - data->field_0x4C);
    }

#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effFireSparkDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effFireSparkDisp, effect);
#endif
}

// MUST_MATCH: the original builds each TEV colour through a small helper.  Passing the colour
// through a void helper (same shape as bushSetColor) makes the by-value argument copy land in
// the stack slot directly above the helper's local, which is what the original does; the plain
// form is under #else.  The KColor helper is needed too, so its argument temp is allocated
// alongside the other two instead of last.
#ifdef MUST_MATCH
static inline void setFireSparkColor1(effFireSparkData* data, s32 alpha) {
    GXColor color = unk_80423100;
    color.r = data->field_0x20;
    color.g = data->field_0x24;
    color.b = data->field_0x28;
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
}
static inline void setFireSparkColor2(effFireSparkData* data) {
    GXColor color = unk_80423104;
    color.r = data->field_0x30;
    color.g = data->field_0x34;
    color.b = data->field_0x38;
    color.a = data->field_0x3C;
    GXSetTevColor(GX_TEVREG1, color);
}
static inline void setFireSparkKColor(GXColor color) {
    GXSetTevKColor(GX_KCOLOR0, color);
}
#endif

void effFireSparkDisp(CameraId camId, void* param) {
    EffectEntry* effect;
    effFireSparkData* data;
    CameraEntry* cam;
    GXTexObj texobj;
    Mtx m0;
    Mtx m2;
    Mtx m1;
    Mtx viewMtx;
    s32 i;
    s32 alpha;

    effect = param;
    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x2C;

    PSMTXTrans(m0, data->field_0x04, data->field_0x08, data->field_0x0C);
    PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * float_deg2rad_8041ee9c);
    PSMTXScale(m2, data->field_0x40, data->field_0x40, data->field_0x40);
    PSMTXConcat(m0, m1, m0);
    PSMTXConcat(m0, m2, m0);
    PSMTXConcat(cam->view, m0, viewMtx);

#ifdef MUST_MATCH
    setFireSparkColor1(data, alpha);
    setFireSparkColor2(data);
#else
    {
        GXColor color2;
        GXColor color1;

        color1 = unk_80423100;
        color1.r = data->field_0x20;
        color1.g = data->field_0x24;
        color1.b = data->field_0x28;
        color1.a = alpha;
        GXSetTevColor(GX_TEVREG0, color1);
        color2 = unk_80423104;
        color2.r = data->field_0x30;
        color2.g = data->field_0x34;
        color2.b = data->field_0x38;
        color2.a = data->field_0x3C;
        GXSetTevColor(GX_TEVREG1, color2);
    }
#endif

    GXSetNumChans(0);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_C0);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_KONST, GX_CC_C1, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
    GXSetNumTexGens(2);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 125);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, 125);
#ifdef MUST_MATCH
    setFireSparkKColor(dat_8041ee98);
#else
    GXSetTevKColor(GX_KCOLOR0, dat_8041ee98);
#endif
    effGetTexObjN64(0x8B, &texobj);
    GXLoadTexObj(&texobj, GX_TEXMAP0);
    effGetTexObjN64(0x65, &texobj);
    GXLoadTexObj(&texobj, GX_TEXMAP1);
    GXSetCullMode(GX_CULL_NONE);
    effSetVtxDescN64((effVtxArray*)size4x4_tex32x32_vtx);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x64 < 0) {
            continue;
        }
        PSMTXTrans(m0, data->field_0x04, data->field_0x08, data->field_0x0C);
        PSMTXConcat(viewMtx, m0, m0);
        GXLoadPosMtxImm(m0, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        PSMTXScale(m2, float_0p03125_8041eea0, float_0p0078125_8041eea4, float_0_8041eea8);
#ifdef MUST_MATCH
        // Match-only: literal 0.0f instead of the named float_0_8041eea8 makes the signed i%4*32 conversion
        // schedule its constant loads as the original does. It adds an anonymous 0.0 to .sdata2.
        PSMTXTrans(m0, 0.0f, (f32)(i % 4 * 32), 0.0f);
#else
        PSMTXTrans(m0, float_0_8041eea8, (f32)(i % 4 * 32), float_0_8041eea8);
#endif
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);
        PSMTXScale(m2, float_0p03125_8041eea0, float_0p03125_8041eea0, float_0_8041eea8);
        PSMTXTrans(m0, data->field_0x5C, data->field_0x60, float_0_8041eea8);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
