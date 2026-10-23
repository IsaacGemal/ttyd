#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effRappaData {
	s32 type;        // 0x00
	Vec position;    // 0x04
	f32 velX;       // 0x10
	f32 velY;       // 0x14
	f32 velZ;       // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 scale;      // 0x24
	f32 speed;      // 0x28
	s32 alpha;       // 0x2C
	s32 life;        // 0x30
	s32 counter;     // 0x34
} effRappaData; // 0x38
// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p0625_8041fad8;
extern const f32 float_0p03125_8041fadc;
extern const f32 float_0_8041fae0;
extern const f32 float_deg2rad_8041fae4;
extern const f32 float_1_8041fae8;
extern const f32 float_0p8_8041faec;
extern const f32 float_0p9_8041faf0;
extern const f32 float_10_8041faf4;
void effRappaDisp(CameraId cameraId, void* param);
void effRappaMain(EffectEntry* effect);
EffectEntry* effRappaN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 yaw, f32 pitch);
extern const f32 float_6p2832_8041faf8;
extern const f32 float_5_8041fafc;
extern const f32 float_0p25_8041fb00;
