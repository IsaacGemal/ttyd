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
	Vec position;   // 0x04
	f32 rotZ;         // 0x10, tilt about Z (degrees)
	f32 targetRadius; // 0x14, radius eases towards this
	f32 radius;       // 0x18, ring radius before the wobble
	s32 life;         // 0x1C, 100 while held (effect flag 4 sets 99); below 100 counts down, deleted below 0; Disp picks texture and mesh from it
	s32 phase;        // 0x20, frame counter for the wobble sines; wraps from 324000 to 256
	BalloonSeg segs[21]; // 0x24, 20 ring points, then the tip offset
} effBalloonN64Data;
EffectEntry* effBalloonN64Entry(s32 type, f32 x, f32 y, f32 z, f32 targetRadius, f32 angleDeg);
void effBalloonMain(EffectEntry* effect);
void effBalloonDisp(CameraId camId, void* param);
