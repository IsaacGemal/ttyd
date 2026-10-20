#include "eff/eff_mahorn.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "matching.h"
#include "memory.h"
#include <dolphin/gx.h>

// .sdata2 constants (defined by the matched data objects; named so loads match retail).
extern const f32 float_100_80422260;
extern const f32 float_20_80422264;
extern const f32 float_0p125_80422268;
extern const u32 dat_80422240;
extern const u32 dat_80422244;
extern const f32 float_deg2rad_80422248;
extern const f32 float_8_8042224c;
extern const f32 float_0p5_80422250;
extern const f32 float_4_80422254;
#ifdef MUST_MATCH
// MUST_MATCH: retail reloads float_0/float_1 for every FIFO write; a plain const lets MWCC
// pool them into registers, so declare them volatile to force the same reloads.
extern const volatile f32 float_0_80422258;
extern const volatile f32 float_1_8042225c;
#else
extern const f32 float_0_80422258;
extern const f32 float_1_8042225c;
#endif

// Scale curves for the two particle banks; each holds 21 entries (the rest is padding).
static u8 scale_dt[] = {
    0x00, 0x07, 0x0E, 0x16, 0x1E, 0x26, 0x2F, 0x37, 0x3F, 0x47, 0x4E,
    0x54, 0x5A, 0x5E, 0x62, 0x64, 0x64, 0x55, 0x33, 0x10, 0x00,
};
static u8 scale_dt2[] = {
    0x00, 0x13, 0x29, 0x3F, 0x52, 0x5F, 0x64, 0x63, 0x5F, 0x59, 0x51,
    0x47, 0x3D, 0x33, 0x28, 0x1E, 0x14, 0x0C, 0x06, 0x02, 0x00,
};

EffectEntry* effMahornEntry(BOOL unused, s32 count, f32 posX, f32 posY, f32 posZ, f32 scale, s32 timer) {
    EffectEntry* effect;
    effMahornData* data;
    s32 i;
    s32 newCount;

    effect = effEntry();
    effect->type = "Mahorn";
    newCount = count + 1;
    effect->count = newCount;
    data = __memAlloc(HEAP_EFFECT, newCount * sizeof(effMahornData));
    effect->userdata = data;
    effect->callback = effMahornMain;
    data->unused = unused;
    data->counter = 0;
    if (timer <= 0) {
        data->timer = 1000;
    }
    else {
        data->timer = timer;
    }
    data->position.x = posX;
    data->position.y = posY;
    data->position.z = posZ;
    data->scale = scale;
    data->alpha = 0xff;
    data++;
#ifdef MUST_MATCH
    // MUST_MATCH: the retail loop loads the pool's 0.0f through its name once (a literal would pool a second 0.0f, and
    // the volatile reads of the FIFO code would reload it for every store).
    {
        f32 zero = float_0_80422258;
        for (i = 1; i < effect->count; i++, data++) {
            data->position.x = zero;
            data->position.y = zero;
            data->position.z = zero;
            data->scale = zero;
            data->field_0x14 = zero;
            data->alpha = 0xff;
            data->field_0x24 = i * 4;
        }
        return effect;
    }
#endif
    for (i = 1; i < effect->count; i++, data++) {
        data->position.x = 0.0f;
        data->position.y = 0.0f;
        data->position.z = 0.0f;
        data->scale = 0.0f;
        data->field_0x14 = 0.0f;
        data->alpha = 0xff;
        data->field_0x24 = i * 4;
    }
    return effect;
}

void effMahornMain(EffectEntry* effect) {
    effMahornData* data = effect->userdata;
#ifdef MUST_MATCH
    Vec position;
#endif
    Vec disp = { 0 };
    f32 scale;
    s32 i;

    disp.x = data->position.x;
    disp.y = data->position.y;
    disp.z = data->position.z;
#ifdef MUST_MATCH
    position = disp;
#endif

    if (data->timer < 1000) {
        data->timer--;
    }
    data->counter++;
    if (data->timer < 16) {
        data->alpha = data->timer << 4;
    }
    if (data->timer < 0) {
        effDelete(effect);
        return;
    }

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x24 != 0) {
            data->field_0x24--;
        }
        else {
            data->counter++;
            if (i & 1) {
                scale = (f32)scale_dt[(u32)data->counter % 21] / float_100_80422260;
            }
            else {
                scale = (f32)scale_dt2[(u32)data->counter % 21] / float_100_80422260;
            }
            data->scale = scale;
#ifdef MUST_MATCH
            {
                f32 t = float_20_80422264 - data->field_0x14;
                data->field_0x14 = t * float_0p125_80422268 + data->field_0x14;
            }
#else
            data->field_0x14 = (float_20_80422264 - data->field_0x14) * float_0p125_80422268 + data->field_0x14;
#endif
            data->alpha += -data->alpha / 64;
        }
    }
#ifdef MUST_MATCH
    dispEntry(4, dispCalcZ(&position), 2, effMahornDisp, effect);
#else
    dispEntry(4, dispCalcZ(&disp), 2, effMahornDisp, effect);
#endif
}

#ifdef MUST_MATCH
// MUST_MATCH: retail builds the GXColor in a temp then copies it to the outgoing slot; inlining the whole
// build+call reproduces that. The natural form (one build, one call) is under #else.
static inline void mahornColorCall(const u32* base, s32 alpha) {
    GXColor c = *(GXColor*)base;
    c.a = alpha;
    GXSetChanMatColor(GX_COLOR0A0, c);
}
#endif

void effMahornDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effMahornData* data = effect->userdata;
    GXTexObj texObj;
    Mtx mtxBase;
    Mtx mtxRot;
    Mtx mtxScale;
    Mtx mtxFinal;
    GXColor color;
    s32 headerAlpha;
#ifdef MUST_MATCH
    f32 negEight;
#endif
    s32 i;

    headerAlpha = data->alpha;
    PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
    PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
    PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80422248);
    PSMTXConcat(mtxBase, mtxRot, mtxFinal);
    PSMTXConcat(mtxFinal, mtxScale, mtxFinal);

    effGetTexObj(0x82, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

#ifdef MUST_MATCH
    negEight = -float_8_8042224c;
#endif

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x24 != 0) {
            continue;
        }

#ifdef MUST_MATCH
        if (i & 1) {
            mahornColorCall(&dat_80422240, (data->alpha * headerAlpha) / 255);
        } else {
            mahornColorCall(&dat_80422244, (data->alpha * headerAlpha) / 255);
        }
#else
        color = *(GXColor*)((i & 1) ? &dat_80422240 : &dat_80422244);
        color.a = (data->alpha * headerAlpha) / 255;
        GXSetChanMatColor(GX_COLOR0A0, color);
#endif

        PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
        if (i & 1) {
            PSMTXScale(mtxScale, data->scale * data->field_0x14, data->scale * data->field_0x14,
                       data->scale * data->field_0x14);
        } else {
            PSMTXScale(mtxScale, data->scale * data->field_0x14, (-data->scale) * data->field_0x14,
                       data->scale * data->field_0x14);
        }
        PSMTXConcat(mtxBase, mtxScale, mtxBase);
        PSMTXConcat(mtxFinal, mtxBase, mtxBase);
        PSMTXConcat(camGetPtr(camId)->view, mtxBase, mtxBase);
        GXLoadPosMtxImm(mtxBase, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
        GXPosition3f32(negEight * float_0p5_80422250, float_4_80422254, float_0_80422258);
#else
        GXPosition3f32(-float_8_8042224c * float_0p5_80422250, float_4_80422254, float_0_80422258);
#endif
        GXTexCoord2f32(float_0_80422258, float_0_80422258);
        GXPosition3f32(float_4_80422254, float_4_80422254, float_0_80422258);
        GXTexCoord2f32(float_1_8042225c, float_0_80422258);
#ifdef MUST_MATCH
        GXPosition3f32(float_4_80422254, negEight * float_0p5_80422250, float_0_80422258);
#else
        GXPosition3f32(float_4_80422254, -float_8_8042224c * float_0p5_80422250, float_0_80422258);
#endif
        GXTexCoord2f32(float_1_8042225c, float_1_8042225c);
#ifdef MUST_MATCH
        GXPosition3f32(negEight * float_0p5_80422250, negEight * float_0p5_80422250,
                      float_0_80422258);
#else
        GXPosition3f32(-float_8_8042224c * float_0p5_80422250, -float_8_8042224c * float_0p5_80422250,
                      float_0_80422258);
#endif
        GXTexCoord2f32(float_0_80422258, float_1_8042225c);
    }
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const u32 dat_80422240 = 0xFDE83C00;
const u32 dat_80422244 = 0xFFFCE700;
const f32 float_deg2rad_80422248 = 0.017453292f;
const f32 float_8_8042224c = 8.0f;
const f32 float_0p5_80422250 = 0.5f;
const f32 float_4_80422254 = 4.0f;
#ifdef MUST_MATCH
const volatile f32 float_0_80422258 = 0.0f;
const volatile f32 float_1_8042225c = 1.0f;
#else
const f32 float_0_80422258 = 0.0f;
const f32 float_1_8042225c = 1.0f;
#endif
const f32 float_100_80422260 = 100.0f;
const f32 float_20_80422264 = 20.0f;
const f32 float_0p125_80422268 = 0.125f;
