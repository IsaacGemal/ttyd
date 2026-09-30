#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct BalloonHeihoData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	s32 life;       // 0x1C
	s32 counter;    // 0x20
} BalloonHeihoData;
void effBalloonHeihoMain(EffectEntry* effect);
void effBalloonHeihoDisp(CameraId camId, void* param);
EffectEntry* effBalloonHeihoN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 a4, f32 a5);
