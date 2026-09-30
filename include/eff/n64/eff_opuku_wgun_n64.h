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
	s32 field_0x34;     // 0x34
	s32 field_0x38;     // 0x38
	s32 field_0x3C;     // 0x3C
	s32 field_0x40;     // 0x40
	s32 field_0x44;     // 0x44
	s32 field_0x48;     // 0x48
	s32 field_0x4C;     // 0x4C
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
void effOpukuWgunMain(EffectEntry* effect);
void effOpukuWgunDisp(CameraId camId, void* param);
EffectEntry* effOpukuWgunN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale);
