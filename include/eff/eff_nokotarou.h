#pragma once

#include "driver/camdrv.h"
#include "driver/effdrv.h"
// The work block behind EffectEntry->userdata: one Nokotarou particle.
typedef struct NokotarouWork {
	s32 type;      // 0x00
	f32 x;         // 0x04
	f32 y;         // 0x08
	f32 z;         // 0x0C
	f32 x2;        // 0x10
	f32 y2;        // 0x14
	f32 z2;        // 0x18
	f32 scale;     // 0x1C
	f32 scaleAnim; // 0x20
	s32 alpha;     // 0x24
	s32 lifetime;  // 0x28
	u8 unk2C[0x4]; // 0x2C
	s32 animPose;  // 0x30
	s32 animPose2; // 0x34
} NokotarouWork;
EffectEntry* effNokotarouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effNokotarouMain(EffectEntry* effect);
void effNokotarouDisp(CameraId cameraId, void* param);
