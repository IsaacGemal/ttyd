#pragma once

#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/types.h>

typedef struct FallWork {
	s32 type;   // 0x00
	f32 x;      // 0x04
	f32 y;      // 0x08
	f32 z;      // 0x0C
	f32 unk10;  // 0x10
	f32 unk14;  // 0x14
	f32 unk18;  // 0x18
	s32 unk1C;  // 0x1C
	s32 unk20;  // 0x20
	s32 unk24;  // 0x24
	s32 unk28;  // 0x28
	f32 unk2C;  // 0x2C
	f32 unk30;  // 0x30
	f32 unk34;  // 0x34
	s32 unk38;  // 0x38
	s32 unk3C;  // 0x3C
	f32 unk40;  // 0x40
	f32 unk44;  // 0x44
	f32 unk48;  // 0x48
	f32 unk4C;  // 0x4C
	f32 unk50;  // 0x50
	f32 unk54;  // 0x54
	f32 unk58;  // 0x58
	s32 unk5C;  // 0x5C
	s32 unk60;  // 0x60
	s32 unk64;  // 0x64
} FallWork;

s32 effFallCheckHit(EffectEntry* effect);
void effFallDispWater(CameraId cameraId, void* param);
void effFallDispInseki(CameraId cameraId, void* param);
void effFallDisp(CameraId cameraId, void* param);
void effFallMain(EffectEntry* effect);
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 a, f32 z, s32 b, f32 c, f32 d, f32 e);
