#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effFukidashiEntry(s32 a, s32 b, s32 c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effFukidashiData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	Vec offset;     // 0x10, from the anchor (x, 3 + y, z)
	f32 angleBase;  // 0x1C, base angle in degrees, set from the entry's last argument
	f32 height;     // 0x20
	f32 spinY;      // 0x24, yaw in degrees; the type 6 phases swing it
	f32 tiltZ;      // 0x28, roll in degrees, from angleBase and the stack index
	f32 scale;      // 0x2C, 0.5 * height / 12 + 0.5
	s32 timer;      // 0x30, counts down from duration when duration > 0
	s32 animFrame;  // 0x34, index into the anim_data tables, two bytes per frame
	s32 animCell;   // 0x38, the current animation byte; the x offset of the sprite sheet
	s32 number;     // 0x3C, the number shown by case 3 (0..99)
	s32 duration;   // 0x40, 0 uses anim_data_table_2 and never times out
	void* target;   // 0x44, -1 Mario, -2 and -3 party members, otherwise a MarioWork pointer
	s32 alpha;      // 0x48
	s32 camId;      // 0x4C
	s32 field_0x50; // 0x50
	s32 phase;      // 0x54, type 6 phase counter
} effFukidashiData;
void effFukidashiMain(EffectEntry* effect);
void effFukidashiDisp(CameraId camId, void* param);
void calc_pos(effFukidashiData* data, s32 index);
// First object of this unit's .data (the pool the display function addresses its moja/chuchu tables from).
extern u8 lbl_8036BEA0[];
// Model / animation data defined by this translation unit.
