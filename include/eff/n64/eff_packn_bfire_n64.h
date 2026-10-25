#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effPacknBfireN64Entry(s32 type, s32 a1, s32 a2, f32 x, f32 y, f32 z, f32 targetX, f32 targetY, f32 targetZ, f32 scale);
void effPacknBfireMain(EffectEntry* effect);
void effPacknBfireDisp(CameraId camId, void* param);
