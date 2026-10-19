#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
void effAkariPoseDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
void effAkariPoseMain(EffectEntry* effect);
EffectEntry* effAkariPoseN64Entry(s32 type, s32 count, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
