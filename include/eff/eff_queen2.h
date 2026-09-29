#pragma once

#include "driver/camdrv.h"
#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effQueen2Data {
	s32 field_0x0;           // 0x0
	Vec position;            // 0x4
	f32 field_0x10;          // 0x10
	f32 field_0x14;          // 0x14
	s32 field_0x18;          // 0x18
	s32 field_0x1C;          // 0x1C
	s32 field_0x20;          // 0x20
	s32 field_0x24;          // 0x24
	EffectEntry* field_0x28; // 0x28
} effQueen2Data;
EffectEntry* effQueen2Entry(s32 type, f32 x, f32 y, f32 z);
void effQueen2Main(EffectEntry* effect);
void effQueen2Disp(CameraId camId, void* param);
