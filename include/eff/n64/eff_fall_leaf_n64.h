#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
void effFallLeafMain(EffectEntry* effect);
void effFallLeafDisp(CameraId cameraId, void* param);
// The original reads r4 (stored at data->life): signature is
// (s32 type, s32 life, f32 x, f32 y, f32 z); evt_eff.s loads r4=r22 before the call.
EffectEntry* effFallLeafN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z);
