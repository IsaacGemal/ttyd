#include "eff/eff_spirit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// .rodata
const Vec vec3_80301320 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_80301330 = 4503601774854144.0;
const char str_80301338[] = "Spirit";

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

extern GXColor unk_80423350;

EffectEntry* effSpiritEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ,
                            f32 scale) {
    EffectEntry* entry;
    effSpiritData* data;

    entry = effEntry();
    entry->type = str_80301338;
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
    data->velX = 0.0f;
    data->velY = 2.0f;
    data->velZ = 0.0f;
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

// NONMATCHING (92.55%): the s32->f32 conversions use this TU's anonymous pool copy of double_to_int instead of
// the named symbol, and several intplGetValue argument loads are scheduled before the float constant loads.
void effSpiritMain(EffectEntry* effect) {
    effSpiritData* data = effect->userdata;
    Vec position;
    Vec disp;

    disp = vec3_80301320;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
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
            data->y += float_50_80422298 *
                       (f32)sin(float_3p1416_80422294 * ((f32)data->step / (f32)data->duration));
            data->scaleBase = intplGetValue(0xB, data->step, data->duration, float_1_8042229c, float_0_80422284);
        } else {
            effDelete(effect);
            return;
        }
        break;
    }

    switch (data->colorState) {
    case 0:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, float_255_804222a0, float_37_804222a4);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, float_242_804222a8, float_254_804222ac);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, float_0_80422284, float_246_804222b0);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 1;
            data->colorStep = 0;
        }
        break;
    case 1:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, float_37_804222a4, float_226_804222b4);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, float_254_804222ac, float_65_804222b8);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, float_246_804222b0, float_255_804222a0);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 2;
            data->colorStep = 0;
        }
        break;
    case 2:
        data->colorR = intplGetValue(0xB, data->colorStep, 0x20, float_226_804222b4, float_255_804222a0);
        data->colorG = intplGetValue(0xB, data->colorStep, 0x20, float_65_804222b8, float_242_804222a8);
        data->colorB = intplGetValue(0xB, data->colorStep, 0x20, float_255_804222a0, float_0_80422284);
        data->colorStep++;
        if (data->colorStep >= 0x20) {
            data->colorState = 0;
            data->colorStep = 0;
        }
        break;
    }

    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effSpiritDisp, effect);
}

// NONMATCHING (91.99%): the vertex-block constants are reloaded by the original for 0.0/2.0 but kept in
// registers here; the s32->f32 conversion also uses this TU's anonymous pool copy of double_to_int, not the
// named symbol; the color local and its outgoing-argument copy are at 0x8/0xc swapped.
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
    color = *(GXColor*)&unk_80423350;
    color.r = data->colorR;
    color.g = data->colorG;
    color.b = data->colorB;
    color.a = (u8)(float_0p8_80422274 * (f32)data->alpha);
    GXSetChanMatColor(4, color);
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
    GXPosition3f32(-float_8_80422278 * float_0p5_8042227c, float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_0_80422284, float_0_80422284);
    GXPosition3f32(float_4_80422280, float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_2_80422288, float_0_80422284);
    GXPosition3f32(float_4_80422280, -float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_2_80422288, float_2_80422288);
    GXPosition3f32(-float_4_80422280, -float_4_80422280, float_0_80422284);
    GXTexCoord2f32(float_0_80422284, float_2_80422288);
}
