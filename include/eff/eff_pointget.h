#pragma once
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct PointGetWork {
    s32 type;   // 0x0
    f32 x;      // 0x4
    f32 y;      // 0x8
    f32 z;      // 0xC
    s32 value;  // 0x10
    f32 scale;  // 0x14
    s32 wait;   // 0x18
    s32 frame;  // 0x1C
    s32 state;  // 0x20
} PointGetWork;
EffectEntry* effPointGetEntry(s32 type, f32 x, f32 y, f32 z, s32 value, s32 wait);
void effPointGetMain(EffectEntry* effect);
void effPointGetDisp(CameraId camId, void* param);
