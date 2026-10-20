#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
typedef struct effConfettiN64Data {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	f32 velX;        // 0x10
	f32 velY;        // 0x14
	f32 velZ;        // 0x18
	f32 accelY;      // 0x1C
	s32 lifetime;    // 0x20
	s32 frame;       // 0x24 (effect head only)
	s32 alpha;       // 0x28 (effect head only)
	s32 count;       // 0x2C (effect head only)
	s32 delay;       // 0x30
	s32 camId;       // 0x34
} effConfettiN64Data; // 0x38
void effConfettiN64SetCamId(EffectEntry* entry, s32 camId);
void effConfettiMain(EffectEntry* effect);
void effConfettiDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
EffectEntry* effConfettiN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale);
