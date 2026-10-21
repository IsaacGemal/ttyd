#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz);
EffectEntry* effRippleN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void effRippleMain(EffectEntry* effect);
void effRippleDisp(CameraId camId, void* param);
