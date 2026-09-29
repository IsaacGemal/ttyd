#pragma once
#include "driver/effdrv.h"
EffectEntry* effHitN64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f);
#include "driver/camdrv.h"
void effHitMain(EffectEntry* effect);
void effHitDisp(CameraId camId, void* param);
