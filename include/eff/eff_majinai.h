#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
#include "driver/camdrv.h"
EffectEntry* effMajinaiEntry(s32 type, f32 x, f32 y, f32 z);
typedef struct MajinaiWork {
    s32 type;       // 0x0
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    f32 scale;      // 0x10
    s32 alpha;      // 0x14
    s32 state;      // 0x18
    s32 animPose;   // 0x1C
} MajinaiWork;
void effMajinaiMain(EffectEntry* effect);
void effMajinaiDisp(CameraId cameraId, void* param);
