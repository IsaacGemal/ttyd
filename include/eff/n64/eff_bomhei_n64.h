#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effBomheiData {
	s16 type;       // 0x00
	u8 pad_02[2];   // 0x02
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 field_0x10; // 0x10
	s32 field_0x14; // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
} effBomheiData;
EffectEntry* effBomheiN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a5);
void effBomheiMain(EffectEntry* effect);
void effBomheiDisp(CameraId camId, void* param);
