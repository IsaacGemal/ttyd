#pragma once
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct effStoneSmokeN64Data {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 life;       // 0x10
	s32 age;        // 0x14: elapsed frames, indexes the scale keyframes
	s32 color1R;    // 0x18: TEVREG0 colour
	s32 color1G;    // 0x1C
	s32 color1B;    // 0x20
	s32 alpha;      // 0x24
	s32 color2R;    // 0x28: TEVREG1 colour
	s32 color2G;    // 0x2C
	s32 color2B;    // 0x30
	f32 scale;      // 0x34: size from the spawn call
	f32 scaleKey;   // 0x38: keyframe multiplier, 0.01 * the scale table
} effStoneSmokeN64Data;
EffectEntry* effStoneSmokeN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effStoneSmokeMain(EffectEntry* effect);
void effStoneSmokeDisp(CameraId camId, void* param);
