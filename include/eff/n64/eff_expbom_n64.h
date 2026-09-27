#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effExpBomN64Entry(f32 x, f32 y, f32 z);
void effExpBomMain(EffectEntry* effect);
void effExpBomDisp(CameraId camId, void* param);
