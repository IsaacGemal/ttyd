#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/mtx.h>

typedef struct effLasMonData {
	s32 field_0x0; //0x0
	Vec position; //0x4
	f32 scale; //0x10
	s32 animId; //0x14
	s32 field_0x18; //0x18
	u32 field_0x1c; //0x1c
	s32 field_0x20; //0x20
} effLasMonData;

EffectEntry* effLasMonEntry(s32 type, f32 x, f32 y, f32 z);
void effLasMonMain(EffectEntry* effect);
void effLasMonDisp(CameraId camId, void* param);
