#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct PeachDustParticle {
	s32 field_0x00; // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 vx;         // 0x10
	f32 vy;         // 0x14
	f32 gravity;    // 0x18
	s32 life;       // 0x1C
	s32 counter;    // 0x20
	f32 scale;      // 0x24
	s32 frame;      // 0x28, texture frame 0-7 (picks the size8x8_tex22x22_vtx slice), -1 = not drawn
} PeachDustParticle;
EffectEntry* effPeachDustN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5);
void effPeachDustMain(EffectEntry* effect);
void effPeachDustDisp(CameraId camId, void* param);
