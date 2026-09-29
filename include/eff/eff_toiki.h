#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/types.h>
typedef struct ToikiData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 velX;       // 0x10
	f32 velY;       // 0x14
	f32 velZ;       // 0x18
	f32 scale;      // 0x1C
	f32 rotation;   // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 life;       // 0x30
} ToikiData;
EffectEntry* effToikiEntry(s32 type, s32 life, f32 x, f32 y, f32 z);
void effToikiMain(EffectEntry* effect);
void effToikiDisp(CameraId cameraId, void* param);
