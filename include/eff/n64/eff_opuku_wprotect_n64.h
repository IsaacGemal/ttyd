#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effOpukuWprotectData {
	s32 type;                    // 0x00
	f32 x;                       // 0x04
	f32 y;                       // 0x08
	f32 z;                       // 0x0C
	s32 field_0x10;              // 0x10
	s32 field_0x14;              // 0x14
	s32 field_0x18;              // 0x18
	s32 field_0x1C;              // 0x1C
	s32 field_0x20;              // 0x20
	s32 field_0x24;              // 0x24
	s32 field_0x28;              // 0x28
	f32 scale;                   // 0x2C
	f32 field_0x30;              // 0x30
	f32 field_0x34;              // 0x34
	u8 field_0x38[0x88 - 0x38];  // 0x38
	s32 field_0x88;              // 0x88
	s32 field_0x8C;              // 0x8C
	s32 field_0x90;              // 0x90
	s32 field_0x94;              // 0x94
} effOpukuWprotectData;          // 0x98
EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWprotectMain(EffectEntry* effect);
void effOpukuWprotectDisp(CameraId camId, void* param);
