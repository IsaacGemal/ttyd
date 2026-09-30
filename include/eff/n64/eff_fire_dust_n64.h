#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effFireDustN64Entry(s32 type, s32 a2, s32 a3, f32 x, f32 y, f32 z, f32 a7, f32 a8);
void effFireDustMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effFireDustDisp(CameraId camId, void* param);
