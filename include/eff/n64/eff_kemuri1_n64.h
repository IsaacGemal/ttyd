#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

EffectEntry* effKemuri1N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5);
void effKemuri1Main(EffectEntry* effect);
void effKemuri1Disp(CameraId camId, void* param);
