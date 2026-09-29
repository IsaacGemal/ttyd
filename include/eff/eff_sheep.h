#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>
typedef struct EffSheepWork {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 scale;       // 0x10
	s32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	s32 poseId;      // 0x20
	s32 state;       // 0x24
	s32 timer;       // 0x28
	f32 field_0x2C;  // 0x2C
	s32 sfxId;       // 0x30
} EffSheepWork;
void effSheepDisp(CameraId camId, void* param);
void effSheepMain(EffectEntry* effect);
EffectEntry* effSheepEntry(s32 type, f32 x, f32 y, f32 z);
