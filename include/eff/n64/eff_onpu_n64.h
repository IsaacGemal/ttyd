#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effOnpuN64Entry(s32 type, f32 x, f32 y, f32 z);
#include "driver/camdrv.h"
void effOnpuMain(EffectEntry* effect);
void effOnpuDisp(CameraId camId, void* param);
