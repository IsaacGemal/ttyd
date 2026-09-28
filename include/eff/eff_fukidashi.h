#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effFukidashiEntry(s32 a, s32 b, s32 c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effFukidashiData {
	s32 field_0x0;   // 0x0
	Vec position;    // 0x4
	Vec field_0x10;  // 0x10
	f32 field_0x1C;  // 0x1C
	f32 field_0x20;  // 0x20
	f32 field_0x24;  // 0x24
	f32 field_0x28;  // 0x28
	f32 field_0x2C;  // 0x2C
	s32 field_0x30;  // 0x30
	s32 field_0x34;  // 0x34
	s32 field_0x38;  // 0x38
	s32 field_0x3C;  // 0x3C
	s32 field_0x40;  // 0x40
	void* field_0x44;  // 0x44
	s32 field_0x48;  // 0x48
	s32 field_0x4C;  // 0x4C
	s32 field_0x50;  // 0x50
	s32 field_0x54;  // 0x54
} effFukidashiData;
void effFukidashiMain(EffectEntry* effect);
void effFukidashiDisp(CameraId camId, void* param);
void calc_pos(effFukidashiData* data, s32 index);
// Model / animation data defined by this translation unit.
