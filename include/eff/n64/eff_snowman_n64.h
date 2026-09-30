#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effSnowmanN64Data {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 lifetime;   // 0x10
	s32 field_0x14; // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
	f32 scale;      // 0x28
	f32 field_0x2C; // 0x2C
	f32 field_0x30; // 0x30
	f32 field_0x34; // 0x34
	f32 field_0x38; // 0x38
	f32 field_0x3C; // 0x3C
	f32 field_0x40; // 0x40
	f32 field_0x44; // 0x44
	f32 field_0x48; // 0x48
	f32 field_0x4C; // 0x4C
	f32 field_0x50; // 0x50
	s32 field_0x54; // 0x54
	s32 state;      // 0x58
	s32 field_0x5C; // 0x5C
} effSnowmanN64Data;
EffectEntry* effSnowmanN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effSnowmanMain(EffectEntry* effect);
void effSnowmanDisp(CameraId cameraId, void* param);
