#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct effUranokoData {
    s32 type;       // 0x0
    Vec position;   // 0x4
    f32 scale;      // 0x10
    f32 baseScale;  // 0x14
    s32 field_0x18; // 0x18, cleared, then incremented each frame; never read
    s32 timer;      // 0x1C
    s32 animId;     // 0x20
    s32 alpha;      // 0x24
} effUranokoData;

EffectEntry* effUranokoEntry(s32 type, s32 count, f32 x, f32 y, f32 z);
void effUranokoMain(EffectEntry* effect);
void effUranokoDisp(CameraId camId, void* param);
