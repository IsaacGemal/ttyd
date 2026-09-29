#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effKemuri10N64Entry(s32 a1, f32 a2, f32 a3, f32 a4, f32 a5, f32 a6, f32 a7, f32 a8);
#include "driver/camdrv.h"
#include "driver/effdrv.h"
void effKemuri10Disp(CameraId camId, void* param);
void effKemuri10Main(EffectEntry* effect);
