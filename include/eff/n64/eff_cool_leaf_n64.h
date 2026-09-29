#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
EffectEntry* effCoolLeafN64Entry(s32 type, f32 x, f32 y, f32 z);
void effCoolLeafMain(EffectEntry* effect);
void effCoolLeafDisp(CameraId cameraId, void* param);
