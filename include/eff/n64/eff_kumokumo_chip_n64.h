#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effKumokumoChipData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 xVel;       // 0x10
	f32 yVel;       // 0x14
	f32 zVel;       // 0x18
	s32 lifetime;   // 0x1C
	s32 timer;      // 0x20
	s32 r;          // 0x24
	s32 g;          // 0x28
	s32 b;          // 0x2C
	s32 a;          // 0x30
	s32 r2;         // 0x34
	s32 g2;         // 0x38
	s32 b2;         // 0x3C
	s32 a2;         // 0x40
	f32 scale;      // 0x44
} effKumokumoChipData;
void effKumokumoChipMain(EffectEntry* effect);
void effKumokumoChipDisp(CameraId camId, void* param);
EffectEntry* effKumokumoChipN64Entry(s32 type, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 spacing, f32 scale);
