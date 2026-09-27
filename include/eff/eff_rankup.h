#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct effRankupData {
	s32 field_0x0;    // 0x0
	Vec position;     // 0x4
	f32 scale;        // 0x10
	s32 animId;       // 0x14
	s32 field_0x18;   // 0x18
	s32 field_0x1C;   // 0x1C
	s32 field_0x20;   // 0x20
} effRankupData;

EffectEntry* effRankupEntry(s32 type, f32 x, f32 y, f32 z);
void effRankupMain(EffectEntry* effect);
void effRankupDisp(CameraId camId, void* param);
