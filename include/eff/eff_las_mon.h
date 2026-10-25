#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

typedef struct effLasMonData {
	s32 field_0x0; //0x0
	Vec position; //0x4
	f32 scale; //0x10
	s32 animId; //0x14
	s32 state; //0x18 anim step: 0 = playing A_1, 1 = looping S_1
	u32 alpha; //0x1c material evt colour alpha
	s32 life; //0x20 1000 = hold; set to 16 by flag 4, counts down, delete below 0
} effLasMonData;

EffectEntry* effLasMonEntry(s32 type, f32 x, f32 y, f32 z);
void effLasMonMain(EffectEntry* effect);
void effLasMonDisp(CameraId camId, void* param);
