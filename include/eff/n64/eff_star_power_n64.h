#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct EffStarPowerData {
    s16 field_0x0;  // 0x0
    s16 type;       // 0x2
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    s32 lifetime;   // 0x10
    s32 age;        // 0x14
    s32 color1R;    // 0x18
    s32 color1G;    // 0x1C
    s32 color1B;    // 0x20
    s32 color1A;    // 0x24
    s32 color2R;    // 0x28
    s32 color2G;    // 0x2C
    s32 color2B;    // 0x30
    s32 color2A;    // 0x34
    f32 scale;      // 0x38
    f32 angleX;     // 0x3C
    f32 angleY;     // 0x40
    f32 angleZ;     // 0x44
    u32 prevAngleX; // 0x48
    u32 prevAngleY; // 0x4C
    u32 prevAngleZ; // 0x50
    u32 prevScale;  // 0x54
} EffStarPowerData;
EffectEntry* effStarPowerN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effStarPowerMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effStarPowerDisp(CameraId camId, void* param);
