#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct effMiniGameWork {
	s32 type;       // 0x00
	Vec pos;        // 0x04
	f32 scaleX;     // 0x10: 1.0 when shown; the letter's X scale in the draw
	f32 scaleY;     // 0x14: Y scale, same use
	f32 rotZ;       // 0x18: letter's tilt (degrees)
	f32 swing;      // 0x1C: angle (degrees) about a pivot 20 units below the letter; grows by 30 in state 34
	f32 vel;        // 0x20: velocity added to posX (states 11, 14, 24) or posY (31, 51, 54)
	f32 accel;      // 0x24: subtracted from vel each frame
	s32 timer;      // 0x28: frames left in the current state
	s32 field_0x2C; // 0x2C: only cleared
	s32 state;      // 0x30: animation state index (see the switch in effMiniGameMain)
	s32 alpha;      // 0x34: colour alpha, 0..255
} effMiniGameWork;

EffectEntry* effMiniGameEntry(s32 type, f32 x, f32 y, f32 z);
void effMiniGameMain(EffectEntry* effect);
void effMiniGameDisp(CameraId camId, void* param);
