#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
void effKemuri12Main(EffectEntry* effect);
void effKemuri12Disp(CameraId camId, void* param);
EffectEntry* effKemuri12N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
