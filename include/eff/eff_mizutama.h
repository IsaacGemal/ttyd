#pragma once

#include "driver/effdrv.h"
#include <dolphin/types.h>

EffectEntry* effMizutamaEntry(s32 type, f32 x, f32 y, f32 z, f32 angle);
void effMizutamaMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effMizutamaDisp(CameraId camId, void* param);
