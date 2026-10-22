#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct BalloonSeg {
	f32 x; // 0x0
	f32 y; // 0x4
} BalloonSeg;
typedef struct effBalloonN64Data {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	BalloonSeg field_0x24[21]; // 0x24, 20 ring points, then the tip offset
} effBalloonN64Data;
EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5);
void effBalloonMain(EffectEntry* effect);
void effBalloonDisp(CameraId camId, void* param);
