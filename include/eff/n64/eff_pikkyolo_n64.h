#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effPikkyoloN64Data {
	s32 field_0x0; // 0x0
	Vec position;  // 0x4
	f32 velocityX; // 0x10
	f32 velocityY; // 0x14
	f32 velocityZ; // 0x18
	s32 lifetime;  // 0x1C
	s32 timer;     // 0x20
	f32 scale;     // 0x24
	f32 angle;     // 0x28
	s16 r;         // 0x2C
	s16 g;         // 0x2E
	s16 b;         // 0x30
	s16 a;         // 0x32
	s16 r2;        // 0x34
	s16 g2;        // 0x36
	s16 b2;        // 0x38
	s16 a2;        // 0x3A
} effPikkyoloN64Data;
void effPikkyoloMain(EffectEntry* effect);
void effPikkyoloDisp(CameraId cameraId, void* param);
