#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
// this unit's original .sdata2 constants (defined by the unit's data pass)
extern const f32 float_deg2rad_8042015c;
extern const f32 float_0p0625_80420160;
extern const f32 float_0_80420164;
extern u32 dat_80420158;
EffectEntry* effWaterDamageN64Entry(s32 int1, s32 int2, f32 float3, f32 float4, f32 float5, f32 float6);
#include "driver/camdrv.h"
void effWaterDamageMain(EffectEntry* effect);
void effWaterDamageDisp(CameraId camId, void* param);
void effWaterDamageDisp2(CameraId camId, void* param);
