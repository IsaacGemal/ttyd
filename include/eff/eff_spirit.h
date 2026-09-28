#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>
typedef struct effSpiritData {
    s32 type;       // 0x00
    s32 state;      // 0x04
    s32 lifetime;   // 0x08
    s32 tick;       // 0x0C
    f32 x;          // 0x10
    f32 y;          // 0x14
    f32 z;          // 0x18
    f32 startX;     // 0x1C
    f32 startY;     // 0x20
    f32 startZ;     // 0x24
    f32 targetX;    // 0x28
    f32 targetY;    // 0x2C
    f32 targetZ;    // 0x30
    f32 velX;       // 0x34
    f32 velY;       // 0x38
    f32 velZ;       // 0x3C
    s32 step;       // 0x40
    s32 duration;   // 0x44
    s32 colorR;     // 0x48
    s32 colorG;     // 0x4C
    s32 colorB;     // 0x50
    s32 alpha;      // 0x54
    s32 colorState; // 0x58
    s32 colorStep;  // 0x5C
    f32 scale;      // 0x60
    f32 scaleBase;  // 0x64
} effSpiritData;
EffectEntry* effSpiritEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ, f32 scale);
void effSpiritMain(EffectEntry* effect);
void effSpiritDisp(CameraId camId, void* param);
