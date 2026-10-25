#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
typedef struct effMagic3Data {
	s32 type;  // 0x00
	Vec position; // 0x04
	f32 vx;    // 0x10
	f32 vy;    // 0x14
	f32 vz;    // 0x18
	f32 rot;   // 0x1C
	f32 scale; // 0x20
	s32 alpha; // 0x24
	s32 life;  // 0x28
	s32 timer; // 0x2C
} effMagic3Data;
void effMagic3Disp(CameraId camId, void* param);
// .sdata2 float constants specific to effMagic3Main (named after their original addresses).
extern const f32 float_6p2832_8041f718;
extern const f32 float_0p8_8041f71c;
extern const f32 float_0p4_8041f720;
extern const f32 float_360_8041f724;
extern const f32 float_3_8041f728;
extern const f32 float_0p6_8041f72c;
typedef struct EffectEntry EffectEntry;
void effMagic3Main(EffectEntry* effect);
EffectEntry* effMagic3N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z);
