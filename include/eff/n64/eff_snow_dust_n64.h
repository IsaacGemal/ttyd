#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"

EffectEntry* effSnowDustN64Entry(s32 type, f32 x, f32 y, f32 z, s32 count, s32 life, f32 a6, f32 a7, f32 scale);
void effSnowDustMain(EffectEntry* effect);
void effSnowDustDisp(CameraId cameraId, void* param);
