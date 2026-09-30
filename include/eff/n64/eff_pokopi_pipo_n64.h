#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effPokopiPipoN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale);
void effPokopiPipoMain(EffectEntry* effect);
void effPokopiPipoDisp(CameraId camId, void* param);
