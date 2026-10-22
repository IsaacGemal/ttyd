#include "eff/eff_spirit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "matching.h"
// math_ext.h instead of <math.h>: the SDK math.h instantiates its sqrtf inline, whose static
// _half/_three doubles land at the front of .rodata and shift the vec3/double/string pool retail has.
#include "math_ext.h"

// .sdata2
extern const f32 float_deg2rad_80422270;
extern const f32 float_0p8_80422274;
extern const f32 float_8_80422278;
extern const f32 float_0p5_8042227c;
extern const f32 float_4_80422280;
extern const f32 float_0_80422284;
extern const f32 float_2_80422288;
extern const f32 float_0p95_8042228c;
extern const f32 float_0p1_80422290;
extern const f32 float_3p1416_80422294;

EffectEntry* effSpiritEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ,
                            f32 scale) {
    EffectEntry* entry;
    effSpiritData* data;

    entry = effEntry();
    entry->type = "Spirit";
    entry->count = 1;
    data = __memAlloc(HEAP_EFFECT, sizeof(effSpiritData));
    entry->userdata = data;
    entry->callback = effSpiritMain;
    entry->flags |= 2;
    data->type = type;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;
    data->scaleBase = 1.0f;
    data->tick = 0;
    data->lifetime = 1000;
    data->state = 0;
    data->velX = float_0_80422284;
    data->velY = float_2_80422288;
    data->velZ = float_0_80422284;
    data->startX = data->x;
    data->startY = data->y;
    data->startZ = data->z;
    data->targetX = targetX;
    data->targetY = targetY;
    data->targetZ = targetZ;
    data->colorR = 0xFF;
    data->colorG = 0xF2;
    data->colorB = 0;
    data->alpha = 0xCC;
    data->colorState = 0;
    data->colorStep = 0;
    data->step = 0;
    data->duration = duration;
    return entry;
}

// The zero-vector copy is built from an anonymous literal so the scheduler keeps the userdata load
// ahead of it.
static inline Vec makeVec(f32 x, f32 y, f32 z) {
    Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}


void effSpiritMain(EffectEntry* effect) {
    effSpiritData* data = effect->userdata;
    Vec position;
    Vec disp;

    disp = makeVec(data->x, data->y, data->z);
    position = disp;

    if (effect->flags & 4) {
        effect->flags &= ~4;
        data->lifetime = 0x10;
    }
    if (data->lifetime < 1000) {
        data->lifetime--;
    }
    data->tick++;
    if (data->lifetime < 0) {
        effDelete(effect);
        return;
    }
    if (data->lifetime < 16) {
        data->alpha = data->lifetime * 16;
    }

    switch (data->state) {
    case 0:
        data->x += (f32)sin((f32)data->tick * float_0p5_8042227c);
        data->y += data->velY;
        data->velY *= float_0p95_8042228c;
        if (data->velY < float_0p1_80422290) {
            data->startX = data->x;
            data->startY = data->y;
            data->startZ = data->z;
            data->state++;
        }
        break;
    case 1:
        if (data->step < data->duration) {
            data->step++;
            data->x = intplGetValue(0xB, data->step, data->duration, data->startX, data->targetX);
            data->y = intplGetValue(0xB, data->step, data->duration, data->startY, data->targetY);
            data->z = intplGetValue(0xB, data->step, data->duration, data->startZ, data->targetZ);
            data->y += 50.0f *
                       (f32)sin(float_3p1416_80422294 * ((f32)data->step / (f32)data->duration));
            data->scaleBase = intplGetValue(0xB, data->step, data->duration, 1.0f, float_0_80422284);
        } else {
            effDelete(effect);
            return;
        }
        break;
    }

    switch (data->colorState) {
    case 0:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, 255.0f, 37.0f);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, 242.0f, 254.0f);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, float_0_80422284, 246.0f);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 1;
            data->colorStep = 0;
        }
        break;
    case 1:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, 37.0f, 226.0f);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, 254.0f, 65.0f);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, 246.0f, 255.0f);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 2;
            data->colorStep = 0;
        }
        break;
    case 2:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, 226.0f, 255.0f);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, 65.0f, 242.0f);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, 255.0f, float_0_80422284);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 0;
            data->colorStep = 0;
        }
        break;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSpiritDisp, effect);
}

void effSpiritDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effSpiritData* data = effect->userdata;
    CameraEntry* cam;
    GXTexObj texObj;
    Mtx mtxTrans;
    Mtx mtxRot;
    Mtx mtxScale;
    f32 scale;

    cam = camGetPtr(camId);
    PSMTXTrans(mtxTrans, data->x, data->y, data->z);
    PSMTXRotRad(mtxRot, 'y', float_deg2rad_80422270 * -camGetPtr(camId)->viewYaw);
    scale = data->scale * data->scaleBase;
    PSMTXScale(mtxScale, scale, scale, scale);
    PSMTXConcat(mtxTrans, mtxRot, mtxRot);
    PSMTXConcat(mtxRot, mtxScale, mtxScale);
    PSMTXConcat(cam->view, mtxScale, mtxScale);
    GXLoadPosMtxImm(mtxScale, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    effGetTexObj(0x83, &texObj);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, (GXColor){ data->colorR, data->colorG, data->colorB, (u8)(float_0p8_80422274 * (f32)data->alpha) });
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
    // Retail's GXVert.h expands GXPosition/GXTexCoord as macros, so each FIFO store reloads its
    // constant; the inline forms in our header CSE the loads. MATCH_F32 forces the reloads the
    // original has for float_0/float_2; the -4 half-width and float_4 stay in registers as retail.
    // Splitting the negate from the scaling (half = -8; half *= 0.5) keeps -4 in f2 and 4 in f1,
    // where the single expression -float_8*float_0p5 puts them the other way.
    {
    f32 half = -float_8_80422278;
    half = half * float_0p5_8042227c;
    GXWGFifo.f32 = half;
    GXWGFifo.f32 = float_4_80422280;
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = float_4_80422280;
    GXWGFifo.f32 = float_4_80422280;
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_2_80422288);
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = float_4_80422280;
    GXWGFifo.f32 = half;
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_2_80422288);
    GXWGFifo.f32 = MATCH_F32(float_2_80422288);
    GXWGFifo.f32 = half;
    GXWGFifo.f32 = half;
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_0_80422284);
    GXWGFifo.f32 = MATCH_F32(float_2_80422288);
    }
#else
    GXPosition3f32(-float_8_80422278 * float_0p5_8042227c, float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_0_80422284, float_0_80422284);
    GXPosition3f32(float_4_80422280, float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_2_80422288, float_0_80422284);
    GXPosition3f32(float_4_80422280, -float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_2_80422288, float_2_80422288);
    GXPosition3f32(-float_4_80422280, -float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_0_80422284, float_2_80422288);
#endif
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const f32 float_deg2rad_80422270 = 0.017453292f;
const f32 float_0p8_80422274 = 0.8f;
const f32 float_8_80422278 = 8.0f;
const f32 float_0p5_8042227c = 0.5f;
const f32 float_4_80422280 = 4.0f;
const f32 float_0_80422284 = 0.0f;
const f32 float_2_80422288 = 2.0f;
const f32 float_0p95_8042228c = 0.95f;
const f32 float_0p1_80422290 = 0.1f;
const f32 float_3p1416_80422294 = 3.1415927f;
