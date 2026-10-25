#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effHibashiraEntry(s32 type, s32 segmentCount, s32 lifetime, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale);
#include "driver/camdrv.h"
// EffectEntry->userdata points at an array of these. Element 0 is the header the entry
// fills in, elements 1..count-1 are the individual pillar segments.
typedef struct HibashiraData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 scale;      // 0x10
	s32 animPose;   // 0x14
	s32 alpha;      // 0x18: material alpha; segments fade by 10 a frame after age 5, and the header's alpha scales them all
	s32 lifetime;   // 0x1C: header only: frames left (1000 = held); fades the header alpha below 16, deleted below 0
	s32 age;        // 0x20: segments: frames since the segment appeared, capped at 1000
	s32 delay;      // 0x24: segments: frames before the segment appears
} HibashiraData;
void effHibashiraDisp(CameraId camId, void* param);
void effHibashiraMain(EffectEntry* effect);
