#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct effGonbabaBreathData {
	s32 type;         // 0x00
	Vec position;     // 0x04
	f32 velX;         // 0x10
	f32 velY;         // 0x14
	s32 field_0x18;   // 0x18
	s32 lifetime;     // 0x1C
	s32 counter;      // 0x20
	s32 colorR;       // 0x24
	s32 colorG;       // 0x28
	s32 colorB;       // 0x2C
	s32 targetR;      // 0x30
	s32 targetG;      // 0x34
	s32 targetB;      // 0x38
	s32 alpha;        // 0x3C
	f32 scale;        // 0x40
	s32 delay;        // 0x44, particles: frames before it starts; while nonzero Main counts it down and Disp skips it
	f32 rotation;     // 0x48
	EffectEntry* child; // 0x4C
} effGonbabaBreathData;

EffectEntry* effGonbabaBreathEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effGonbabaBreathMain(EffectEntry* effect);
void effGonbabaBreathDisp(CameraId camId, void* param);
void init_breath(effGonbabaBreathData* data, s32 index, s32 type);
