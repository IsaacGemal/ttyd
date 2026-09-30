#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effTimestampN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale);
void effTimestampMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effTimestampDisp(CameraId camId, void* param);
