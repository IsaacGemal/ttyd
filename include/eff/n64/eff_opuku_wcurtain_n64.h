#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effOpukuWcurtainData {
	s32 type;             // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	s32 duration;         // 0x10
	s32 counter;          // 0x14
	s32 field_0x18;       // 0x18
	s32 field_0x1C;       // 0x1C
	s32 field_0x20;       // 0x20
	s32 field_0x24;       // 0x24
	f32 scale;            // 0x28
	f32 field_0x2C[12];   // 0x2C
	f32 field_0x5C[12];   // 0x5C
} effOpukuWcurtainData;
// One N64 effect vertex: s16 position + s16 texture coords + RGBA8 color (14 bytes).
typedef struct OpukuWcurtainVtx {
	s16 x;  // 0x0
	s16 y;  // 0x2
	s16 z;  // 0x4
	s16 s;  // 0x6
	s16 t;  // 0x8
	u8 r;   // 0xA
	u8 g;   // 0xB
	u8 b;   // 0xC
	u8 a;   // 0xD
} OpukuWcurtainVtx;
EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWcurtainMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effOpukuWcurtainDisp(CameraId camId, void* param);
