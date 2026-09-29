#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effKemuri6N64Entry(s32 type, f32 x, f32 y, f32 z);
void effKemuri6Main(EffectEntry* effect);
void effKemuri6Disp(CameraId camId, void* param);
