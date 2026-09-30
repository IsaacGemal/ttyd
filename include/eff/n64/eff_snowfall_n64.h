#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
EffectEntry* effSnowfallN64Entry(s32 a, s32 b);
#include "driver/camdrv.h"
void effSnowfallDisp(CameraId cameraId, void* param);
void effSnowfallMain(EffectEntry* effect);
void cam_clip_chk(void* p, s32 a, s32 b, s32 c);
