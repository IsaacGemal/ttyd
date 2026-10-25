#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct EffOpukuWgunData {
	s32 type;           // 0x00
	Vec position;       // 0x04
	Vec start;          // 0x10
	Vec end;            // 0x1C
	s32 duration;       // 0x28
	s32 timer;          // 0x2C
	s32 frame;          // 0x30
	s32 color1R;        // 0x34, GX_TEVREG0
	s32 color1G;        // 0x38
	s32 color1B;        // 0x3C
	s32 color1A;        // 0x40
	s32 color2R;        // 0x44, GX_TEVREG1 (alpha fixed at 0xC8)
	s32 color2G;        // 0x48
	s32 color2B;        // 0x4C
	f32 scale;          // 0x50
	s32 field_0x54;     // 0x54
	f32 field_0x58[12]; // 0x58
	f32 field_0x88[12]; // 0x88
	f32 field_0xB8[12]; // 0xB8
	f32 x[12];          // 0xE8
	f32 y[12];          // 0x118
	f32 z[12];          // 0x148
	s32 counter[12];    // 0x178
	s32 counter2[12];   // 0x1A8
	u8 alpha[12];       // 0x1D8
} EffOpukuWgunData;     // 0x1E4
// One N64 effect vertex: s16 position + s16 texture coords + RGBA8 color (14 bytes).
typedef struct OpukuWgunVtx {
	s16 x; // 0x0
	s16 y; // 0x2
	s16 z; // 0x4
	s16 s; // 0x6
	s16 t; // 0x8
	u8 r;  // 0xA
	u8 g;  // 0xB
	u8 b;  // 0xC
	u8 a;  // 0xD
} OpukuWgunVtx;        // 0xE
void effOpukuWgunMain(EffectEntry* effect);
void effOpukuWgunDisp(CameraId camId, void* param);
EffectEntry* effOpukuWgunN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale);
