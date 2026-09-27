#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct EffPuniBalloonData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	f32 velX;       // 0x10
	f32 velY;       // 0x14
	f32 velZ;       // 0x18
	f32 field_0x1c; // 0x1C
	f32 scale;      // 0x20
	f32 field_0x24; // 0x24
	s32 lifetime;   // 0x28
	s32 counter;    // 0x2C
} EffPuniBalloonData;

EffectEntry* effPuniBalloonEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void effPuniBalloonMain(EffectEntry* effect);
void effPuniBalloonDisp(CameraId camId, void* param);
