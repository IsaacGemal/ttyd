#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effThunderflareN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effThunderflareMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effThunderflareDisp(CameraId camId, void* param);
