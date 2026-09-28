#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct effMiniGameWork {
	s32 type;       // 0x00
	f32 field_0x04; // 0x04
	f32 field_0x08; // 0x08
	f32 field_0x0C; // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
} effMiniGameWork;

EffectEntry* effMiniGameEntry(s32 type, f32 x, f32 y, f32 z);
void effMiniGameMain(EffectEntry* effect);
void effMiniGameDisp(CameraId camId, void* param);
