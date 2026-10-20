#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effFireworksData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 velX;            // 0x10
	f32 velY;            // 0x14
	f32 velZ;            // 0x18
	s32 life;            // 0x1C
	s32 counter;         // 0x20
	u32 color0[4];       // 0x24
	u32 color1[4];       // 0x34
	f32 radius;          // 0x44
	f32 targetRadius;    // 0x48
	s32 mode;            // 0x4C
	f32 trailX[4];       // 0x50
	f32 trailY[4];       // 0x60
	f32 trailZ[4];       // 0x70
	f32 trailVelX[4];    // 0x80
	f32 trailVelY[4];    // 0x90
	f32 trailVelZ[4];    // 0xA0
} effFireworksData;
EffectEntry* effFireworksN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 velX, f32 velY, f32 velZ, f32 targetRadius);
void effFireworksMain(EffectEntry* effect);
void effFireworksDisp(CameraId camId, void* param);
// .sbss2 scratch GXColors the original builds its TEV colours from.
extern GXColor unk_80423108;
extern GXColor unk_8042310c;
