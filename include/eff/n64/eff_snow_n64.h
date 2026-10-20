#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effSnowData {
	f32 x;          // 0x00
	f32 y;          // 0x04
	f32 z;          // 0x08
	f32 velX;       // 0x0C
	f32 velY;       // 0x10
	f32 velZ;       // 0x14
	f32 rotX;       // 0x18 degrees
	f32 rotY;       // 0x1C degrees
	f32 rotXVel;    // 0x20 degrees per frame
	f32 rotYVel;    // 0x24 degrees per frame
	s32 type;       // 0x28
} effSnowData;
EffectEntry* effSnowN64Entry(s32 type, f32 x, f32 y, f32 z);
void effSnowMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effSnowDisp(CameraId camId, void* param);
