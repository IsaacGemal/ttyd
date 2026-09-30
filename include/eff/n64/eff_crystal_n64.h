#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effCrystalN64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f);
void effCrystalMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCrystalDisp(CameraId cameraId, void* param);
