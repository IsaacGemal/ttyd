#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
struct EffectEntry;
EffectEntry* effStarStoneEntry(s32 idx, f32 x, f32 y, f32 z, f32 scale);
#include <dolphin/mtx.h>
#include "driver/dispdrv.h"
typedef struct effStarStoneData {
	s32 field_0x0;   // 0x0
	u16 field_0x4;   // 0x4
	u8 field_0x6[2]; // 0x6
	f32 field_0x8;   // 0x8
	f32 field_0xC;   // 0xC
	f32 field_0x10;  // 0x10
	f32 field_0x14;  // 0x14
	f32 field_0x18;  // 0x18
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	s32 field_0x2C;  // 0x2C
	u32 field_0x30;  // 0x30
	struct EffectEntry* field_0x34; // 0x34
	s32 field_0x38;  // 0x38
	s32 field_0x3C;  // 0x3C
	f32 field_0x40;  // 0x40
	f32 field_0x44;  // 0x44
	f32 spreadAX;    // 0x48 x offset of tip pair A, swung by camera yaw each tick
	f32 spreadAZ;    // 0x4C z offset of tip pair A
	f32 spreadBX;    // 0x50 x offset of tip pair B (opposite swing to A)
	f32 spreadBZ;    // 0x54 z offset of tip pair B
	f32 field_0x58;  // 0x58
	u8 tintR;        // 0x5C tint, applied to the TEV register colour
	u8 tintG;        // 0x5D
	u8 tintB;        // 0x5E
	u8 field_0x5F;   // 0x5F
	s32 sfxHandle;   // 0x60 handle from psndSFXOn_3D for the shine loop
} effStarStoneData;
void effStarStoneDraw(CameraId camId, EffectEntry* entry);
void effStarStoneDisp_1(CameraId camId, void* param);
void effStarStoneDisp_2(CameraId camId, void* param);
void effStarStoneMain(EffectEntry* effect);
