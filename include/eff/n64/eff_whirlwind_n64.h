#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effWhirlwindN64Entry(s32 a1, s32 a2, s32 a3, f32 a4);
#include "driver/camdrv.h"
void effWhirlwindMain(EffectEntry* effect);
void effWhirlwindDisp(CameraId camId, void* param);
