#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
void effKemuri5Main(EffectEntry* effect);
void effKemuri5Disp(CameraId camId, void* param);
EffectEntry* effKemuri5N64Entry(f32 x, f32 y, f32 z, f32 angle);
