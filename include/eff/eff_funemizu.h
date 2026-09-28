#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effFunemizuEntry(s32 type, f32 a, f32 b, f32 c, f32 d, f32 e);
u32 unk_80224298(void);
#include "driver/camdrv.h"
void effFunemizuMain(EffectEntry* effect);
void effFunemizuShibukiDisp(CameraId camId, void* param);
void effFunemizuDisp(CameraId camId, void* param);
typedef struct EffFunemizuData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	f32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	f32 field_0x34; // 0x34
	f32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	f32 field_0x44; // 0x44
	s32 field_0x48; // 0x48
} EffFunemizuData;
