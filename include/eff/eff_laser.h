#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
typedef struct effLaserData {
	s32 type;  // 0x0
	Vec position;   // 0x4
	f32 originX; // 0x10
	f32 originY; // 0x14
	f32 originZ; // 0x18
	f32 scale;      // 0x1c
	s32 animId;     // 0x20
	s32 field_0x24; // 0x24
	s32 timer;      // 0x28
	s32 state;      // 0x2c
	s32 alpha;      // 0x30
} effLaserData;
EffectEntry* effLaserEntry(s32 type, f32 x, f32 y, f32 z);
void effLaserMain(EffectEntry* effect);
void effLaserDisp(CameraId camId, void* param);
