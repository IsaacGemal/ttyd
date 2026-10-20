#pragma once
#include "driver/effdrv.h"
EffectEntry* effBreathFireN64Entry(s32 kind, f32 x, f32 y, s32 children, f32 z, s32 seed, f32 endX, s32 duration, f32 endY, f32 endZ);
void effBreathFireMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effBreathFireDisp(CameraId camId, void* param);
