#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
void effAkariPoseDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
extern const volatile Vec vec3_802fa1b8;
void effAkariPoseMain(EffectEntry* effect);
EffectEntry* effAkariPoseN64Entry(s32 type, s32 count, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
