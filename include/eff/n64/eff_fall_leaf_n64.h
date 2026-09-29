#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
EffectEntry* effFallLeafN64Entry(s32 type, f32 x, f32 y, f32 z);
void effFallLeafMain(EffectEntry* effect);
void effFallLeafDisp(CameraId cameraId, void* param);
