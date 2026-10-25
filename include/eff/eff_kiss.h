#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

typedef struct KissEffectWork {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 velX;       // 0x10
	f32 velY;       // 0x14
	f32 angle;      // 0x18
	f32 rot;        // 0x1C
	f32 scaleX;     // 0x20
	f32 scaleY;     // 0x24
	s32 life;       // 0x28
	s32 frame;      // 0x2C
	s32 alpha;      // 0x30
	u8 pad_0x34[0x38 - 0x34]; // 0x34
} KissEffectWork;

EffectEntry* effKissEntry(s32 type, f32 x, f32 y, f32 z, f32 angle);
void effKissMain(EffectEntry* effect);
void effKissDisp(CameraId camId, void* param);
void effKissDisp2(CameraId camId, void* param);
