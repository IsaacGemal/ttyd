#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"

EffectEntry* effIndirectEntry(s32 type, s32 timer, f32 x, f32 y, f32 z);
void effIndirectMain(EffectEntry* effect);
void effIndirectDisp(CameraId camId, void* param);
