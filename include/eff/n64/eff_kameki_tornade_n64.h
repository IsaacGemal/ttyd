#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 a4);
#include "driver/camdrv.h"
void effKamekiTornadeDisp(CameraId cameraId, void* param);
