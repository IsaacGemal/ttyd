#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effBombData {
	s32 type;      // 0x0
	Vec position;  // 0x4
	f32 scaleX;    // 0x10
	f32 scaleY;    // 0x14
	f32 scale;     // 0x18
	f32 angle;     // 0x1C
	s32 counter;   // 0x20
	s32 texOffset; // 0x24
} effBombData;
EffectEntry* effBombEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
