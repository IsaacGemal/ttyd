#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
extern u32 dat_8041eec8;
extern u32 dat_8041eecc;
typedef struct effFireFlowerData {
	s32 type;       // 0x00
	s32 state;      // 0x04
	f32 x;          // 0x08
	f32 y;          // 0x0C
	f32 z;          // 0x10
	f32 velX;       // 0x14
	f32 velY;       // 0x18
	f32 velZ;       // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 scale;      // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 lifetime;   // 0x40
	s32 timer;      // 0x44
	s32 field_0x48; // 0x48
	s32 field_0x4C; // 0x4C
	s32 mode;       // 0x50
} effFireFlowerData;
EffectEntry* effFireFlowerN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z);
void effFireFlowerMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effFireFlowerDisp(CameraId camId, void* param);
