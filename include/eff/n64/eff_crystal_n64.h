#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effCrystalN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale);
void effCrystalMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCrystalDisp(CameraId cameraId, void* param);
