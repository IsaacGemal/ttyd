#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effCloudData {
	s32 type;             // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	s32 lifetime;         // 0x10
	s32 field_0x14;       // 0x14
	s32 field_0x18;       // 0x18
	s32 field_0x1C;       // 0x1C
	s32 field_0x20;       // 0x20
	s32 field_0x24;       // 0x24
	s32 field_0x28;       // 0x28
	s32 field_0x2C;       // 0x2C
	s32 field_0x30;       // 0x30
	f32 scale;            // 0x34
	f32 field_0x38;       // 0x38
	f32 field_0x3C;       // 0x3C
	f32 field_0x40;       // 0x40
	f32 field_0x44;       // 0x44
	f32 field_0x48;       // 0x48
	f32 field_0x4C;       // 0x4C
	f32 field_0x50[30];   // 0x50
	f32 field_0xC8[30];   // 0xC8
	f32 field_0x140[30];  // 0x140
	s32 field_0x1B8[30];  // 0x1B8
	s32 field_0x230[30];  // 0x230
	s32 field_0x2A8;      // 0x2A8
	f32 field_0x2AC[30];  // 0x2AC
} effCloudData;

// One GX vertex in this unit's interleaved format (see effVtxArray): 14 bytes of
// position/texcoord/colour; effCloudDisp writes two per live ring entry.
typedef struct cloudVtx {
	s16 x;
	s16 y;
	s16 z;
	s16 u;
	s16 v;
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} cloudVtx;

EffectEntry* effCloudN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effCloudMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCloudDisp(CameraId cameraId, void* param);
