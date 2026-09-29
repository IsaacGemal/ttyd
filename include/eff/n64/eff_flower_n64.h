#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effFlowerData {
    s32 type;       // 0x0
    Vec position;   // 0x4
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    f32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    f32 field_0x2C; // 0x2C
    f32 field_0x30; // 0x30
    s32 field_0x34; // 0x34
    s32 field_0x38; // 0x38
    s32 field_0x3C; // 0x3C
    s32 field_0x40; // 0x40
    s32 field_0x44; // 0x44
} effFlowerData;
EffectEntry* effFlowerN64Entry(s32 type, f32 x, f32 y, f32 z);
void effFlowerMain(EffectEntry* effect);
void effFlowerDisp(CameraId camId, void* param);
