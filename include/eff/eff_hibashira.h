#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effHibashiraEntry(s32 a, s32 b, s32 c, f32 x, f32 y, f32 z, f32 x2, f32 y2, f32 z2, f32 scale);
#include "driver/camdrv.h"
// EffectEntry->userdata points at an array of these. Element 0 is the header the entry
// fills in, elements 1..count-1 are the individual pillar segments.
typedef struct HibashiraData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	f32 scale;      // 0x10
	s32 animPose;   // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
} HibashiraData;
void effHibashiraDisp(CameraId camId, void* param);
void effHibashiraMain(EffectEntry* effect);
