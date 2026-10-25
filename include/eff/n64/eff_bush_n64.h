#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
typedef struct BushN64Data {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 velX;       // 0x10
	f32 velY;       // 0x14
	f32 rotZ;       // 0x18: degrees
	f32 swayPhase;  // 0x1C: degrees, random-walked each frame; drives rotZ and rotY
	f32 rotY;       // 0x20: degrees
	s32 alpha;      // 0x24: head only, TEV colour alpha
	s32 lifetime;   // 0x28
	s32 age;        // 0x2C: head only, frames elapsed
	u8 pad_0x30[0x4]; // 0x30
} BushN64Data;
EffectEntry* effBushN64Entry(s32 type, f32 x, f32 y, f32 z);
void effBushMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effBushDisp(CameraId camId, void* param);
