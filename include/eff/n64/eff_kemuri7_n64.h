#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effKemuri7N64Entry(f32 float1, f32 float2, f32 float3, f32 float4, f32 float5);
#include "driver/camdrv.h"
void effKemuri7Main(EffectEntry* effect);
void effKemuri7Disp(CameraId camId, void* param);
