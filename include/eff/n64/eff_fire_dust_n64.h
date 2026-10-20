#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effFireDustN64Entry(s32 type, s32 count, s32 life, f32 x, f32 y, f32 z, f32 radius, f32 height);
void effFireDustMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effFireDustDisp(CameraId camId, void* param);
