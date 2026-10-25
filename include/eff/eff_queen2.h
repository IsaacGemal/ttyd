#pragma once

#include "driver/camdrv.h"
#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effQueen2Data {
	s32 field_0x0;            // 0x0
	Vec position;             // 0x4
	f32 scale;                // 0x10
	f32 baseScale;            // 0x14
	s32 alpha;                // 0x18
	u8 pad_0x1C[4];           // 0x1C
	s32 phaseTimer;           // 0x20
	s32 state;                // 0x24
	EffectEntry* chargeEntry; // 0x28
} effQueen2Data;
EffectEntry* effQueen2Entry(s32 type, f32 x, f32 y, f32 z);
void effQueen2Main(EffectEntry* effect);
void effQueen2Disp(CameraId camId, void* param);
