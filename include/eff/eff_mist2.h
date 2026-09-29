#pragma once
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
typedef struct EffMist2Data {
	s32 type;    // 0x00
	f32 x;       // 0x04
	f32 y;       // 0x08
	f32 z;       // 0x0C
	f32 unk10;   // 0x10
	f32 unk14;   // 0x14
	f32 unk18;   // 0x18
	f32 unk1C;   // 0x1C
	f32 unk20;   // 0x20
	f32 unk24;   // 0x24
	f32 unk28;   // 0x28
	f32 unk2C;   // 0x2C
	s32 unk30;   // 0x30
	s32 unk34;   // 0x34
	u8 unk38;    // 0x38
	u8 unk39;    // 0x39
	u8 unk3A;    // 0x3A
	u8 pad3B;    // 0x3B
	s32 unk3C;   // 0x3C
	f32 unk40;   // 0x40
	f32 unk44;   // 0x44
} EffMist2Data;
void effMist2Disp(CameraId cameraId, void* param);
void effMist2Main(EffectEntry* effect);
EffectEntry* effMist2Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 range, f32 yOffset);
