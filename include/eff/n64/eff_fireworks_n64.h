#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effFireworksData {
	s32 type;            // 0x00
	f32 x;               // 0x04
	f32 y;               // 0x08
	f32 z;               // 0x0C
	f32 field_0x10;      // 0x10
	f32 field_0x14;      // 0x14
	f32 field_0x18;      // 0x18
	s32 life;            // 0x1C
	s32 counter;         // 0x20
	u32 color0[4];       // 0x24
	u32 color1[4];       // 0x34
	f32 field_0x44;      // 0x44
	f32 field_0x48;      // 0x48
	s32 field_0x4C;      // 0x4C
	f32 field_0x50[4];   // 0x50
	f32 field_0x60[4];   // 0x60
	f32 field_0x70[4];   // 0x70
	f32 field_0x80[4];   // 0x80
	f32 field_0x90[4];   // 0x90
	f32 field_0xA0[4];   // 0xA0
} effFireworksData;
EffectEntry* effFireworksN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 a5, f32 a6, f32 a7, f32 a8);
void effFireworksMain(EffectEntry* effect);
void effFireworksDisp(CameraId camId, void* param);
