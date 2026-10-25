#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effShockN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6);
// The shock effect's work data, as effShockMain/effShockDisp read it.
typedef struct EffShockData {
	s32 type;         // 0x0
	f32 x;            // 0x4
	f32 y;            // 0x8
	f32 z;            // 0xC
	s32 field_0x10;   // 0x10 0xFF at init, down 13 a frame after frame 6; nothing else reads it
	s32 type1Alpha;   // 0x14 type 1: alpha of its yellow TEVREG0 colour, x0.9 each frame
	s32 tev1A;        // 0x18 TEVREG0 alpha, x0.8 each frame in the last 5 frames
	s32 life;         // 0x1C frames left (30 at init, counts down below 100); deleted below 0
	s32 age;          // 0x20 frames elapsed
	f32 scale;        // 0x24 x/y scale of the shock mesh (times 0.25)
	f32 field_0x28;   // 0x28 0.5 * a5, written and never read
	f32 offsetY;      // 0x2C y translation of the shock mesh (0.5 * a6)
	s32 tev1R;   // 0x30
	s32 tev1G;   // 0x34
	s32 tev1B;   // 0x38
	s32 tev2R;   // 0x3C
	s32 tev2G;   // 0x40
	s32 tev2B;   // 0x44
} EffShockData;
void effShockMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effShockDisp(CameraId cameraId, void* param);
// .sbss2 scratch GXColors and .sdata2 seed this unit's display code builds TEV colours from.
// .data vertex array owned by this unit.
