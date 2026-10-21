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
	f32 speed; //0x10 horizontal drift, multiplies dirX/dirZ each tick
	u8 field_0x14[0x20 - 0x14]; //0x14
	f32 velY; //0x20 vertical velocity, damped by 0.9 per tick
	u8 field_0x24[0x30 - 0x24]; //0x24
	f32 dirX; //0x30 sin of the drift angle
	f32 dirZ; //0x34 cos of the drift angle
	u8 field_0x38[0x3C - 0x38]; //0x38
	s32 counter; //0x3C ticks since spawn; the puff is deleted after 30
	u8 field_0x40[0x64 - 0x40]; //0x40
	f32 spinVel; //0x64 spin rate, damped by 0.95 per tick
	f32 spin; //0x68 accumulated z rotation in degrees
	s32 alpha; //0x6C tint alpha, fades by 0.8 after counter passes 15
	f32 pulseScale; //0x70 sin(counter) pulse, used as the scale of the quads
	f32 scale; //0x74 base scale, starts at 1
	f32 heading; //0x78 camera-relative yaw in degrees; picks which quad faces the camera
	u8 field_0x7C[0x8C - 0x7C]; //0x7C
} effKemuri2Data;
typedef struct effKemuriData {
	s32 kind; //0x0 the int2 from effKemuriEntry: selects the scale table and texture
	f32 x; //0x4
	f32 y; //0x8
	f32 z; //0xC
	u8 field_0x10[0x40 - 0x10]; //0x10
	s32 frame; //0x40 advanced once per main tick; indexes the scale tables
	f32 scaleX; //0x44 puff size from touchdown_scale/heavy_scale
	f32 scaleY; //0x48
	f32 velX; //0x4C sideways speed, damped by 0.85 per tick (type 0)
	u8 field_0x50[0x58 - 0x50]; //0x50
	f32 offX; //0x58 x offset, accumulates velX
	f32 field_0x5C; //0x5C
	u8 field_0x60[0x74 - 0x60]; //0x60
	f32 scale; //0x74 uniform scale of the whole puff, starts at 1
	u8 field_0x78[0x80 - 0x78]; //0x78
	s32 sheetU0; //0x80 texture column of this frame (16 columns over lifetime)
	s32 sheetU1; //0x84 texture column of the next frame, clamped to 15
	s32 lifetime; //0x88 frames before effDelete (32 or 64)
} effKemuriData;
