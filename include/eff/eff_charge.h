#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
// One charge element. The work array is 31 of these, with element 0 aliasing
// the header fields (they share the same 0x48-byte layout).
typedef struct ChargeEffectData {
    s32 field_0x00; // 0x00
    s32 type;       // 0x04
    f32 x;          // 0x08
    f32 y;          // 0x0C
    f32 z;          // 0x10
    s32 alpha;      // 0x14
    s32 timer;      // 0x18
    s32 counter;    // 0x1C
    f32 angle1;     // 0x20
    f32 angle2;     // 0x24
    f32 scale;      // 0x28
    f32 radius;     // 0x2C
    s32 colorR;     // 0x30
    s32 colorG;     // 0x34
    s32 colorB;     // 0x38
    s32 color2R;    // 0x3C TEVREG1; stage 1 lerps TEVREG0 -> TEVREG1 by texture colour
    s32 color2G;    // 0x40
    s32 color2B;    // 0x44
} ChargeEffectData;
EffectEntry* effChargeEntry(s32 type, s32 timer, f32 x, f32 y, f32 z, f32 scale);
void effChargeMain(EffectEntry* effect);
void effChargeDisp(CameraId cameraId, void* param);
