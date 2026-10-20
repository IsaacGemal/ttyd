#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effSnowmanN64Data {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	s32 lifetime;    // 0x10
	s32 frame;       // 0x14, counts up every frame
	s32 field_0x18;  // 0x18
	s32 field_0x1C;  // 0x1C
	s32 field_0x20;  // 0x20
	s32 alpha;       // 0x24, fade-in over 16 frames and fade-out over the last 16 lifetime frames
	f32 scale;       // 0x28
	f32 rotY;        // 0x2C, yaw; state 3 reads jump2_ry_data
	f32 offX;        // 0x30, offset from x
	f32 offY;        // 0x34, height above the base; starts at 200
	f32 offZ;        // 0x38, offset from z
	f32 velX;        // 0x3C
	f32 velY;        // 0x40
	f32 velZ;        // 0x44
	f32 vScale;      // 0x48, 2 - hScale
	f32 hScale;      // 0x4C, from the hscale tables
	f32 gravity;     // 0x50, added to velY each frame in state 2
	s32 animIdx;     // 0x54, index into the hscale tables
	s32 state;       // 0x58
	s32 bounceCount; // 0x5C, landings so far; picks the bounce pattern
} effSnowmanN64Data;
EffectEntry* effSnowmanN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effSnowmanMain(EffectEntry* effect);
void effSnowmanDisp(CameraId cameraId, void* param);
