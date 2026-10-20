#pragma once

#include "driver/effdrv.h"
#include <dolphin/types.h>

EffectEntry* effTeresaEntry(s32 type, f32 x, f32 y, f32 z);
#pragma once

#include "driver/effdrv.h"
typedef struct EffTeresaData {
	s32 type;            // 0x0
	f32 x;               // 0x4
	f32 y;               // 0x8
	f32 z;               // 0xC
	u8 pad_0x10[0x18];   // 0x10
	f32 alpha;           // 0x28
	s32 field_0x2C;      // 0x2C
	s32 field_0x30;      // 0x30
	f32 scale;           // 0x34
	f32 angle;           // 0x38
	s32 state;           // 0x3C
} EffTeresaData;
void effTeresaMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effTeresaDisp(CameraId camId, void* param);
