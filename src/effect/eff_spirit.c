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
extern const f32 float_50_80422298;
extern const f32 float_1_8042229c;
extern const f32 float_255_804222a0;
extern const f32 float_37_804222a4;
extern const f32 float_242_804222a8;
extern const f32 float_254_804222ac;
extern const f32 float_246_804222b0;
extern const f32 float_226_804222b4;
extern const f32 float_65_804222b8;

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423350;

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
    data->scaleBase = float_1_8042229c;
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

#ifdef MUST_MATCH
// The original translation unit called intplGetValue through a prototype that lists the two
// float bounds before the step/duration words. The ABI is identical (ints->r3-r5, floats->f1-f2);
// only the source argument order differs, and MWCC then schedules the float loads first as retail does.
#define intplGetValueFM(mode, start, end, currStep, steps) \
    ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(mode, start, end, currStep, steps)
#else
#define intplGetValueFM(mode, start, end, currStep, steps) intplGetValue(mode, currStep, steps, start, end)
#endif

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
            data->x = intplGetValueFM(0xB, data->startX, data->targetX, data->step, data->duration);
            data->y = intplGetValueFM(0xB, data->startY, data->targetY, data->step, data->duration);
            data->z = intplGetValueFM(0xB, data->startZ, data->targetZ, data->step, data->duration);
            data->y += float_50_80422298 *
                       (f32)sin(float_3p1416_80422294 * ((f32)data->step / (f32)data->duration));
            data->scaleBase = intplGetValueFM(0xB, float_1_8042229c, float_0_80422284, data->step, data->duration);
        } else {
            effDelete(effect);
            return;
        }
        break;
    }

    switch (data->colorState) {
    case 0:
        data->colorR = intplGetValueFM(0xB, float_255_804222a0, float_37_804222a4, data->colorStep, 0x20);
        data->colorG = intplGetValueFM(0xB, float_242_804222a8, float_254_804222ac, data->colorStep, 0x20);
        data->colorB = intplGetValueFM(0xB, float_0_80422284, float_246_804222b0, data->colorStep, 0x20);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 1;
            data->colorStep = 0;
        }
        break;
    case 1:
        data->colorR = intplGetValueFM(0xB, float_37_804222a4, float_226_804222b4, data->colorStep, 0x20);
        data->colorG = intplGetValueFM(0xB, float_254_804222ac, float_65_804222b8, data->colorStep, 0x20);
        data->colorB = intplGetValueFM(0xB, float_246_804222b0, float_255_804222a0, data->colorStep, 0x20);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 2;
            data->colorStep = 0;
        }
        break;
    case 2:
        data->colorR = intplGetValueFM(0xB, float_226_804222b4, float_255_804222a0, data->colorStep, 0x20);
        data->colorG = intplGetValueFM(0xB, float_65_804222b8, float_242_804222a8, data->colorStep, 0x20);
        data->colorB = intplGetValueFM(0xB, float_255_804222a0, float_0_80422284, data->colorStep, 0x20);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 0;
            data->colorStep = 0;
        }
        break;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSpiritDisp, effect);
}

#ifdef MUST_MATCH
// The original builds the material colour in a helper that returns it; the caller's local and its
// outgoing-argument copy then land at 0x8/0xc as retail. Building the colour directly in the caller
// (or in a void helper that calls GXSetChanMatColor) puts the local at 0xc and the copy at 0x8.
static inline GXColor effSpiritMakeColor(effSpiritData* data) {
    GXColor color = unk_80423350;
    color.r = data->colorR;
    color.g = data->colorG;
    color.b = data->colorB;
    color.a = (u8)(float_0p8_80422274 * (f32)data->alpha);
    return color;
}
#endif

void effSpiritDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    effSpiritData* data = effect->userdata;
    CameraEntry* cam;
    GXColor color;
    GXTexObj texObj;
    Mtx mtx90;
    Mtx mtx60;
    Mtx mtx30;
    f32 scale;

    cam = camGetPtr(camId);
    PSMTXTrans(mtx90, data->x, data->y, data->z);
    PSMTXRotRad(mtx60, 'y', float_deg2rad_80422270 * -camGetPtr(camId)->viewYaw);
    scale = data->scale * data->scaleBase;
    PSMTXScale(mtx30, scale, scale, scale);
    PSMTXConcat(mtx90, mtx60, mtx60);
    PSMTXConcat(mtx60, mtx30, mtx30);
    PSMTXConcat(cam->view, mtx30, mtx30);
    GXLoadPosMtxImm(mtx30, 0);
    GXSetCurrentMtx(0);
    effGetTexObj(0x83, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
#ifdef MUST_MATCH
    color = effSpiritMakeColor(data);
    GXSetChanMatColor(4, color);
#else
    color = *(GXColor*)&unk_80423350;
    color.r = data->colorR;
    color.g = data->colorG;
    color.b = data->colorB;
    color.a = (u8)(float_0p8_80422274 * (f32)data->alpha);
    GXSetChanMatColor(4, color);
#endif
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXBegin(0x80, 0, 4);
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
const f32 float_50_80422298 = 50.0f;
const f32 float_1_8042229c = 1.0f;
const f32 float_255_804222a0 = 255.0f;
const f32 float_37_804222a4 = 37.0f;
const f32 float_242_804222a8 = 242.0f;
const f32 float_254_804222ac = 254.0f;
const f32 float_246_804222b0 = 246.0f;
const f32 float_226_804222b4 = 226.0f;
const f32 float_65_804222b8 = 65.0f;
