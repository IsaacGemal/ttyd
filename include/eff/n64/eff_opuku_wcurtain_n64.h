#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effOpukuWcurtainData {
	s32 type;             // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	s32 duration;         // 0x10
	s32 counter;          // 0x14
	s32 field_0x18;       // 0x18
	s32 field_0x1C;       // 0x1C
	s32 field_0x20;       // 0x20
	s32 field_0x24;       // 0x24
	f32 scale;            // 0x28
	f32 field_0x2C[12];   // 0x2C
	f32 field_0x5C[12];   // 0x5C
} effOpukuWcurtainData;
EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWcurtainMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effOpukuWcurtainDisp(CameraId camId, void* param);
