#pragma once
#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effVaporN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effVaporMain(EffectEntry* effect);

typedef struct effVaporN64Data {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xc
	s32 lifetime;   // 0x10
	s32 field_0x14; // 0x14
	u32 field_0x18; // 0x18
	u32 field_0x1c; // 0x1c
	u32 field_0x20; // 0x20
	u32 field_0x24; // 0x24
	u32 field_0x28; // 0x28
	u32 field_0x2c; // 0x2c
	u32 field_0x30; // 0x30
	u32 field_0x34; // 0x34
	f32 field_0x38; // 0x38
	f32 field_0x3c; // 0x3c
	f32 scale;      // 0x40
} effVaporN64Data;
