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
	s32 hopFrames;   // 0x14, hopping sheep (type 1): airtime of one hop; only Entry reads it, to set velY
	f32 velY;        // 0x18, hopping sheep: added to y each frame, negated on landing
	f32 gravity;     // 0x1C, hopping sheep: subtracted from velY each frame
	s32 poseId;      // 0x20
	s32 state;       // 0x24
	s32 timer;       // 0x28
	f32 velX;        // 0x2C, added to x each frame until x reaches 500
	s32 sfxId;       // 0x30
} EffSheepWork;
void effSheepDisp(CameraId camId, void* param);
void effSheepMain(EffectEntry* effect);
EffectEntry* effSheepEntry(s32 type, f32 x, f32 y, f32 z);
