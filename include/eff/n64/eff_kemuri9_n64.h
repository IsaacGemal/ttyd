#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effKemuri9N64Entry(s32 a0, s32 a1, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7);
typedef struct EffectEntry EffectEntry;
void effKemuri9Main(EffectEntry* effect);
#include "driver/camdrv.h"
void effKemuri9Disp(CameraId camId, void* param);
