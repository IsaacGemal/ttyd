#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effWangFlushN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z);
void effWangFlushMain(EffectEntry* effect);
void effWangFlushDisp(CameraId camId, void* param);
