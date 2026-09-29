#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>
typedef struct MistWork {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 scaleX;     // 0x10
	f32 scaleY;     // 0x14
	f32 scaleZ;     // 0x18
	f32 velX;       // 0x1C
	f32 velY;       // 0x20
	f32 velZ;       // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	f32 alpha;      // 0x34
	s32 counter;    // 0x38
	s32 life;       // 0x3C
	f32 field_0x40; // 0x40
	u8 r;           // 0x44
	u8 g;           // 0x45
	u8 b;           // 0x46
	u8 pad_47;      // 0x47
} MistWork;
EffectEntry* effMistEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 w);
void effMistMain(EffectEntry* effect);
void effMistDisp(CameraId cameraId, void* param);
