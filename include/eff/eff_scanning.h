#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>
#include "driver/effdrv.h"
void effScanningMain(EffectEntry* effect);
void effScanningDisp(CameraId camId, void* param);
EffectEntry* effScanningEntry(s32 int2, f32 float3, f32 float4, f32 float5);
