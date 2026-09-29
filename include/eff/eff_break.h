#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effBreakEntry(s32 type, s32 a2, f32 x, f32 y, f32 z);
void effBreakMain(EffectEntry* effect);
void effBreakDisp(CameraId cameraId, void* param);
