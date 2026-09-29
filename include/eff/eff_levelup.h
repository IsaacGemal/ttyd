#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
typedef struct effLevelupData {
	s32 type;      // 0x00
	Vec position;  // 0x04, effect position (element 0) / per-glyph offset (elements 1+)
	f32 xOffset;   // 0x10
	s32 timer;     // 0x14
	s32 state;     // 0x18
	f32 scaleX;    // 0x1C
	f32 scaleY;    // 0x20
} effLevelupData;
EffectEntry* effLevelupEntry(s32 type, s32 count, f32 x, f32 y, f32 z);
void effLevelupMain(EffectEntry* effect);
void effLevelupDisp(CameraId camId, void* param);
