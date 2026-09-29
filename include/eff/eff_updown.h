#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>
typedef struct EffUpdownWork {
	s32 type;     // 0x00
	f32 x;        // 0x04
	f32 y;        // 0x08
	f32 z;        // 0x0C
	f32 scaleX;   // 0x10
	f32 scaleY;   // 0x14
	s32 field18;  // 0x18
	s32 lifetime; // 0x1C
	u32 frame;    // 0x20
	u8 alpha;     // 0x24
} EffUpdownWork;
void effUpdownDisp(CameraId cameraId, void* param);
void effUpdownMain(EffectEntry* effect);
EffectEntry* effUpdownEntry(s32 type, f32 x, f32 y, f32 z, s32 field18, s32 lifetime);
