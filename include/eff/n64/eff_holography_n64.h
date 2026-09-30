#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct EffHolographyData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	s32 field_0x10; // 0x10
	s32 field_0x14; // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
} EffHolographyData;
void effHolographyMain(EffectEntry* effect);
void effHolographyDisp(CameraId camId, void* param);
EffectEntry* effHolographyN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
