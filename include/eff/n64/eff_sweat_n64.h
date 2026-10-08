#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effSweatData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 angle;      // 0x10
	f32 field_0x14; // 0x14
	Vec velocity;   // 0x18
	s32 life;       // 0x24
	s32 age;        // 0x28
	s32 timer;      // 0x2C
	s32 alpha;      // 0x30
} effSweatData; // 0x34
void effSweatMain(EffectEntry* effect);
void effSweatDisp(CameraId camId, void* param);
EffectEntry* effSweatN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale, f32 angle, s32 life);
