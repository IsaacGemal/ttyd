#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effSnowData {
	f32 x;          // 0x00
	f32 y;          // 0x04
	f32 z;          // 0x08
	f32 field_0x0C; // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	s32 type;       // 0x28
} effSnowData;
EffectEntry* effSnowN64Entry(s32 type, f32 x, f32 y, f32 z);
void effSnowMain(EffectEntry* effect);
