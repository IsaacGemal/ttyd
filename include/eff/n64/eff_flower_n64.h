#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effFlowerData {
    s32 type;        // 0x00
    Vec position;    // 0x04
    f32 velX;        // 0x10
    f32 velY;        // 0x14
    f32 velZ;        // 0x18
    f32 accelY;      // 0x1C
    f32 accelYDelta; // 0x20
    f32 angleX;      // 0x24
    f32 angleY;      // 0x28
    f32 angleXVel;   // 0x2C
    f32 angleYVel;   // 0x30
    s32 life;        // 0x34
    s32 age;         // 0x38
    s32 field_0x3C;  // 0x3C
    s32 field_0x40;  // 0x40
    s32 field_0x44;  // 0x44
} effFlowerData;
void effFlowerMain(EffectEntry* effect);
void effFlowerDisp(CameraId camId, void* param);
EffectEntry* effFlowerN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z);
