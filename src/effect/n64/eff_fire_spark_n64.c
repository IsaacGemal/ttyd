#include "eff/n64/eff_fire_spark_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423104;
__declspec(section ".sdata") GXColor unk_80423100;


#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// Local to this unit in the original (`.obj scale_data, local`).
static f32 scale_data[30] = {
    0.01f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.65f, 0.8f, 0.95f, 1.0f,
    1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  1.0f, 1.0f,  1.0f,
    1.0f,  0.95f, 0.8f, 0.65f, 0.5f, 0.35f, 0.2f, 0.1f, 0.05f, 0.01f,
};

u32 size4x4_tex32x32_vtx[14] = {
    0xFFFEFFFE, 0x00000000, 0x00000000, 0x00FF0002,
    0xFFFE0000, 0x04000000, 0x000000FF, 0x00020002,
    0x00000400, 0x04000000, 0x00FFFFFE, 0x00020000,
    0x00000400, 0x000000FF,
};

typedef struct effFireSparkData {
    s32 type;             // 0x00
    f32 posX;             // 0x04, head origin or particle position
    f32 posY;             // 0x08
    f32 posZ;             // 0x0C
    f32 spreadX;          // 0x10, random spawn range on X (rand() % spreadX)
    f32 spreadY;          // 0x14, random spawn range on Y
    s32 life;             // 0x18
    s32 frame;            // 0x1C, head frame counter (wraps at 324000)
    s32 color1R;          // 0x20, TEVREG0 colour
    s32 color1G;          // 0x24
    s32 color1B;          // 0x28
    s32 alpha;            // 0x2C, TEVREG0 alpha, from life and frame
    s32 color2R;          // 0x30, TEVREG1 colour
    s32 color2G;          // 0x34
    s32 color2B;          // 0x38
    s32 color2A;          // 0x3C
    f32 scale;            // 0x40
    f32 velX;             // 0x44, eases toward velXTarget each frame
    f32 velY;             // 0x48, eases toward velYTarget each frame
    f32 velZ;             // 0x4C, eases toward velZTarget each frame
    f32 velXTarget;       // 0x50
    f32 velYTarget;       // 0x54
    f32 velZTarget;       // 0x58
    f32 texScrollU;       // 0x5C, TEXMTX1 translation
    f32 texScrollV;       // 0x60
    s32 cycleFrame;       // 0x64, 0..29 (scale_data index), negative until the particle starts
    f32 velYScale;        // 0x68, scales the random initial velY
    f32 velYTargetScale;  // 0x6C, scales the random velYTarget
} effFireSparkData;

extern const GXColor dat_8041ee98;

EffectEntry* effFireSparkN64Entry(s32 type, s32 count, s32 life, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8, f32 a9, f32 a10, f32 a11) {
    EffectEntry* effect;
    effFireSparkData* data;
    effFireSparkData* ptr;
    f32 ratio;
    s32 i;

    effect = effEntry();
    effect->type = "FireSparkN64";
    effect->count = count;
    data = __memAlloc(HEAP_EFFECT, sizeof(effFireSparkData) * count);
    effect->userdata = data;
    effect->callback = effFireSparkMain;
    effect->flags |= 2;

    data->type = type;
    data->frame = 0;
    if (life <= 0) {
        data->life = 1000;
    } else {
        data->life = life;
    }
    data->alpha = 0;
    data->posX = a4;
    data->posY = a5;
    data->posZ = a6;
    data->scale = a9;
    data->spreadX = a7;
    data->spreadY = a8;
    data->velYScale = a10;
    data->velYTargetScale = a11;
    data->color1R = 0x7f;
    data->color1G = 0;
    data->color1B = 0;
    data->color2R = 0x7f;
    data->color2G = 0x7f;
    data->color2B = 0;
    data->color2A = 0xff;

    ratio = 30.f / (f32)(count - 1);
    ratio *= 0.5f;
    ptr = data + 1;
    for (i = 1; i < count; i++, ptr++) {
        ptr->cycleFrame = (s32)(-(f32)i * ratio) - 1;
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
    Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 life;
    s32 counter;
    f32 half;
    f32 v68;
    f32 v6C;

    disp.x = data->posX;
    disp.y = data->posY;
    disp.z = data->posZ;
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

    data->frame++;
    if (data->frame > 324000) {
        data->frame = 0x100;
    }

    if (data->life < 0) {
        effDelete(effect);
        return;
    }

    life = data->life;
    counter = data->frame;
    if (life < 0x10) {
        data->alpha = life * 16;
    }
    if (counter < 0x10) {
        data->alpha = counter * 16 + 15;
    }

    rangeX = (s32)data->spreadX;
    half = 0.5f * data->spreadX;
    rangeY = (s32)data->spreadY;
    v68 = data->velYScale;
    v6C = data->velYTargetScale;

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        data->cycleFrame++;
        if (data->cycleFrame >= 0x1E) {
            data->cycleFrame = 0;
        }
        if (data->cycleFrame < 0) {
            continue;
        }
        if (data->cycleFrame == 0) {
            data->posX = (f32)(rand() % rangeX) - half;
            data->posY = (f32)(rand() % rangeY);
            data->posZ = 0.0f;
            data->velX = 0.05f * (f32)(rand() % 20 - 10);
            data->velY = 0.05f * (v68 * (f32)(rand() % 40 - 1));
            data->velZ = 0.0f;
            data->velXTarget = 0.05f * (f32)(rand() % 20 - 10);
            data->velYTarget = 0.05f * (v6C * (f32)(rand() % 30 - 1));
            data->velZTarget = 0.0f;
            data->texScrollU = (f32)(rand() % 16);
            data->texScrollV = (f32)(rand() % 16);
        }
        data->scale = scale_data[data->cycleFrame];
        data->posX += data->velX;
        data->posY += data->velY;
        data->posZ += data->velZ;
        data->texScrollU += 0.1f;
        data->texScrollV += 0.3f;
        data->velX += 0.04f * (data->velXTarget - data->velX);
        data->velY += 0.04f * (data->velYTarget - data->velY);
        data->velZ += 0.04f * (data->velZTarget - data->velZ);
    }

#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effFireSparkDisp, effect);
#else
    dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effFireSparkDisp, effect);
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
    color.r = data->color1R;
    color.g = data->color1G;
    color.b = data->color1B;
    color.a = alpha;
    GXSetTevColor(GX_TEVREG0, color);
}
static inline void setFireSparkColor2(effFireSparkData* data) {
    GXColor color = unk_80423104;
    color.r = data->color2R;
    color.g = data->color2G;
    color.b = data->color2B;
    color.a = data->color2A;
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
    alpha = data->alpha;

    PSMTXTrans(m0, data->posX, data->posY, data->posZ);
    PSMTXRotRad(m1, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
    PSMTXScale(m2, data->scale, data->scale, data->scale);
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
        color1.r = data->color1R;
        color1.g = data->color1G;
        color1.b = data->color1B;
        color1.a = alpha;
        GXSetTevColor(GX_TEVREG0, color1);
        color2 = unk_80423104;
        color2.r = data->color2R;
        color2.g = data->color2G;
        color2.b = data->color2B;
        color2.a = data->color2A;
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
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
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
        if (data->cycleFrame < 0) {
            continue;
        }
        PSMTXTrans(m0, data->posX, data->posY, data->posZ);
        PSMTXConcat(viewMtx, m0, m0);
        GXLoadPosMtxImm(m0, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        PSMTXScale(m2, 0.03125f, 0.0078125f, 0.0f);
#ifdef MUST_MATCH
        // Match-only: literal 0.0f instead of the named 0.0f makes the signed i%4*32 conversion
        // schedule its constant loads as the original does. It adds an anonymous 0.0 to .sdata2.
        PSMTXTrans(m0, 0.0f, (f32)(i % 4 * 32), 0.0f);
#else
        PSMTXTrans(m0, 0.0f, (f32)(i % 4 * 32), 0.0f);
#endif
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX0, GX_MTX2x4);
        PSMTXScale(m2, 0.03125f, 0.03125f, 0.0f);
        PSMTXTrans(m0, data->texScrollU, data->texScrollV, 0.0f);
        PSMTXConcat(m2, m0, m2);
        GXLoadTexMtxImm(m2, GX_TEXMTX1, GX_MTX2x4);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const GXColor dat_8041ee98 = { 0x7F, 0x7F, 0x7F, 0x00 };
