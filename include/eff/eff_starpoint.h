#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"

void effStarPointDisp(CameraId camId, void* param);
EffectEntry* effStarPointEntry(s32 type, s32 count, f32 x, f32 y, f32 z);
void effStarPointMain(EffectEntry* effect);