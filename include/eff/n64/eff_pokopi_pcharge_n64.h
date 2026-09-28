#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effPokopiPchargeN64Entry(f32 x, f32 y, f32 z, f32 scale, s32 a, s32 b);
#include "driver/camdrv.h"
typedef struct effPokopiPchargeData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	s32 lifetime;   // 0x10
	s32 timer;      // 0x14
	u32 r;          // 0x18
	u32 g;          // 0x1C
	u32 b;          // 0x20
	s32 a;          // 0x24
	u32 r2;         // 0x28
	u32 g2;         // 0x2C
	u32 b2;         // 0x30
	f32 scale;      // 0x34
	f32 field_0x38; // 0x38
	f32 field_0x3C; // 0x3C
	f32 field_0x40; // 0x40
	f32 field_0x44; // 0x44
} effPokopiPchargeData;
void effPokopiPchargeMain(EffectEntry* effect);
void effPokopiPchargeDisp(CameraId camId, void* param);
