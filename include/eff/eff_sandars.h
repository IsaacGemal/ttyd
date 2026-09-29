#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effSandarsEntry(s32 type, f32 x, f32 y, f32 z, f32 a);
#include "driver/camdrv.h"
// One cone particle. The work array is an array of these (stride 0x44).
typedef struct effSandarsData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 scale;      // 0x10
	s32 state;      // 0x14
	s32 timer;      // 0x18
	s32 poseId;     // 0x1C
	s32 alpha;      // 0x20
	s32 bombType;   // 0x24
	f32 animScale;  // 0x28
	f32 scaleRate;  // 0x2C
	f32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	f32 rot;        // 0x38
	f32 rotSpeed;   // 0x3C
	s32 camId;      // 0x40
} effSandarsData;
void effSandarsMain(EffectEntry* effect);
void effSandarsDisp(CameraId camId, void* param);
void effSandarsDisp2(CameraId camId, void* param);
