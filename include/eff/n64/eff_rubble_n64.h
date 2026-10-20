#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
EffectEntry* effRubbleN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 groundY, f32 vy, f32 gravity, f32 scale);
void effRubbleMain(EffectEntry* effect);
void effRubbleDisp(CameraId cameraId, void* param);
