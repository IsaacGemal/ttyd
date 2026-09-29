#pragma once
typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
EffectEntry* effTogeFlushEntry(s32 type, f32 x, f32 y, f32 z, s32 life);
void effTogeFlushMain(EffectEntry* effect);
void effTogeFlushDisp(CameraId cameraId, void* param);
typedef struct effTogeFlushData {
	s32 field_0x0;   // 0x0 type
	f32 x;           // 0x4
	f32 y;           // 0x8
	f32 z;           // 0xC
	f32 field_0x10;  // 0x10 rotation
	f32 field_0x14;  // 0x14 rotation speed
	f32 field_0x18;  // 0x18 scale
	f32 field_0x1C;  // 0x1C scale delta
	f32 field_0x20;  // 0x20 scale 2
	s32 field_0x24;  // 0x24 alpha
	s32 field_0x28;  // 0x28 color alpha
	s32 field_0x2C;  // 0x2C life
	s32 field_0x30;  // 0x30 frame
} effTogeFlushData;
