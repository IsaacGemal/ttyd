#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effFreezeData {
	s32 type;              // 0x00
	f32 x;                 // 0x04
	f32 y;                 // 0x08
	f32 z;                 // 0x0C
	s32 life;              // 0x10
	s32 counter;           // 0x14
	u8 color0[4];          // 0x18
	u8 color1[4];          // 0x1C
	f32 scale;             // 0x20
	u8 field_0x24;         // 0x24
	u8 field_0x25;         // 0x25
	u8 pad_0x26[2];        // 0x26
	EffectEntry* snowDust; // 0x28
} effFreezeData;
EffectEntry* effFreezeN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effFreezeMain(EffectEntry* effect);
void effFreezeDisp(CameraId cameraId, void* param);
