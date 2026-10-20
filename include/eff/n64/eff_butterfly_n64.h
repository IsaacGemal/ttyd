#pragma once
#include "driver/effdrv.h"
EffectEntry* effButterflyN64Entry(s32 type, f32 x, f32 y, f32 z);
void effButterflyMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effButterflyDisp(CameraId camId, void* param);
