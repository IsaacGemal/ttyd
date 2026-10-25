#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effBomheiData {
	s16 type;       // 0x00
	u8 pad_02[2];   // 0x02
	Vec position;   // 0x04
	f32 scale;      // 0x10 x/y scale of the quad
	s32 life;       // 0x14 ticks left; the effect is deleted when it drops below 0
	s32 age;        // 0x18 ticks since spawn
	s32 lifetime;   // 0x1C total ticks (the Entry count); divides age into animFrame
	f32 animFrame;  // 0x20 0..9 over the lifetime: integer part picks the texture row, fraction blends to the next
	f32 rotZ;       // 0x24 z rotation in degrees
	f32 rotZVel;    // 0x28 added to rotZ each tick (0 for type 0, else 40)
} effBomheiData;
EffectEntry* effBomheiN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effBomheiMain(EffectEntry* effect);
void effBomheiDisp(CameraId camId, void* param);
