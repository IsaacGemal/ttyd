#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
EffectEntry* effRecoverHeartN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z);
void effRecoverHeartMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effRecoverHeartDisp(CameraId camId, void* param);
