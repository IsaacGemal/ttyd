#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>
#include "driver/effdrv.h"

typedef struct effQueenData {
	s32 field_0x0;  // 0x0
	Vec position;   // 0x4
	f32 widthScale; // 0x10
	f32 scale;      // 0x14
	s32 animId;     // 0x18
	s32 field_0x1C; // 0x1C
	s32 lifetime;   // 0x20
	s32 alpha;      // 0x24
	s32 alphaCap;   // 0x28
} effQueenData;

EffectEntry* effQueenEntry(s32 arg0, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effQueenMain(EffectEntry* effect);
void effQueenDisp(CameraId camId, void* param);
