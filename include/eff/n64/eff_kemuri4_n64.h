#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effKemuri4Data {
	s32 field_0x00;  // 0x00
	u16 type;        // 0x04
	s16 field_0x06;  // 0x06
	u8 field_0x08;   // 0x08
	u8 field_0x09;   // 0x09
	u8 pad_0A[2];    // 0x0A
	Vec position;    // 0x0C
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	Mtx field_0x30;  // 0x30
	f32 field_0x60;  // 0x60
	f32 field_0x64;  // 0x64
	f32 field_0x68;  // 0x68
	f32 field_0x6C;  // 0x6C
	f32 field_0x70;  // 0x70
	f32 field_0x74;  // 0x74
	f32 field_0x78;  // 0x78
	f32 field_0x7C;  // 0x7C
	f32 field_0x80;  // 0x80
	f32 field_0x84;  // 0x84
} effKemuri4Data;    // 0x88
EffectEntry* effKemuri4N64Entry(s32 type, f32 x, f32 y, f32 z, f32 a4, f32 a5);
void effKemuri4Main(EffectEntry* effect);
void effKemuri4Disp(CameraId camId, void* param);
