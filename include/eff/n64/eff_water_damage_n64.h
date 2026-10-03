#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effWaterDamageN64Entry(s32 int1, s32 int2, f32 float3, f32 float4, f32 float5, f32 float6);
#include "driver/camdrv.h"
void effWaterDamageMain(EffectEntry* effect);
void effWaterDamageDisp(CameraId camId, void* param);
void effWaterDamageDisp2(CameraId camId, void* param);
