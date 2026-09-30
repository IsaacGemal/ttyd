#pragma once

#include "driver/camdrv.h"
void effMissStarDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
EffectEntry* effMissStarEntry(s32 type, f32 x, f32 y, f32 z, s32 lifetime, s32 angle);
void effMissStarMain(EffectEntry* effect);
