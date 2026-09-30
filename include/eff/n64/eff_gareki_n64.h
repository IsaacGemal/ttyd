#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
void effGarekiMain(EffectEntry* effect);
void effGarekiDisp(CameraId camId, void* param);
EffectEntry* effGarekiN64Entry(s32 type, f32 x, f32 y, f32 z, f32 groundY);
