#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
EffectEntry* effRotHammerFailN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5);
void effRotHammerFailMain(EffectEntry* effect);
void effRotHammerFailDisp(CameraId cameraId, void* param);
