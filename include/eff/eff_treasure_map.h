#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// Work data behind EffectEntry->userdata for the treasure map effect.
typedef struct EffTreasureMapData {
	s32 type;     // 0x0
	f32 x;        // 0x4
	f32 y;        // 0x8
	f32 z;        // 0xC
	f32 scale;    // 0x10
	u32 alpha;    // 0x14
	s32 timer;    // 0x18
	s32 counter;  // 0x1C
	f32 angle;    // 0x20
	s32 animId;   // 0x24
	s32 cameraId; // 0x28
} EffTreasureMapData;
EffectEntry* effTreasureMapEntry(s32 type, f32 x, f32 y, f32 z);
void effTreasureMapMain(EffectEntry* effect);
void effTreasureMapDisp(CameraId camId, void* param);
