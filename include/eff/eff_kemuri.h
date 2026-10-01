#pragma once

#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>

EffectEntry* effKemuriEntry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6);
EffectEntry* effKemuri12N64Entry(s32 int2, s32 int3, f32 float3, f32 float4, f32 float5, f32 float6);
void effKemuriMain(EffectEntry* effect);
void effKemuriDisp(CameraId camId, void* param);
// sdata2/sdata constants of the unit, used by effKemuriMain and effKemuriDisp.
extern const f32 float_16_8041c6f4;
extern const f32 float_0p0078125_8041c6f8;
extern const f32 float_1p5_8041c6fc;
extern const f32 float_0p85_8041c700;
extern const f32 float_0p8_8041c6f0;
extern const f32 float_0p9_8041c6e8;
extern const f32 float_0p95_8041c6ec;
extern const f32 float_2_8041c704;
extern const f32 float_1p2_8041c708;
extern const f32 float_0p0625_8041c6d8;
extern const f32 float_neg0p2_8041c6dc;
extern const f64 double_to_int_mask_802f3118;
extern const GXColor dat_8041c670;
extern const GXColor dat_8041c674;
extern const GXColor dat_8041c678;
extern const GXColor dat_8041c67c;
extern const u8 touchdown_scale[60];
extern const u8 heavy_scale[60];
EffectEntry* effKemuri2Entry(s32 int2, f32 float3, f32 float4, f32 float5, f32 float6, f32 float7);
typedef struct effKemuri2Data {
	u8 field_0x0[0x4]; //0x0
	f32 x; //0x4
	f32 y; //0x8
	f32 z; //0xC
	f32 field_0x10; //0x10
	u8 field_0x14[0x20 - 0x14]; //0x14
	f32 field_0x20; //0x20
	u8 field_0x24[0x30 - 0x24]; //0x24
	f32 field_0x30; //0x30
	f32 field_0x34; //0x34
	u8 field_0x38[0x3C - 0x38]; //0x38
	s32 counter; //0x3C
	u8 field_0x40[0x64 - 0x40]; //0x40
	f32 field_0x64; //0x64
	f32 field_0x68; //0x68
	s32 alpha; //0x6C
	f32 field_0x70; //0x70
	f32 field_0x74; //0x74
	f32 field_0x78; //0x78
	u8 field_0x7C[0x8C - 0x7C]; //0x7C
} effKemuri2Data;
