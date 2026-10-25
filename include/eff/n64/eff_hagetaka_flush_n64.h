#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effHagetakaFlushData {
	s32 type;       //0x00
	f32 x;          //0x04
	f32 y;          //0x08
	f32 z;          //0x0C
	s32 life;       //0x10, frames left (1000+: never counts down; effect flag 4 sets 10); scale halves below 4, deleted below 0
	s32 frame;      //0x14, frames since spawn: drives the scale growth and the color_rotation_data index
	s32 field_0x18; //0x18, set to 70; never read
	s32 field_0x1C; //0x1C, set to 180; never read
	s32 field_0x20; //0x20, set to 120; never read
	s32 alpha;      //0x24, TEVREG0 alpha
	f32 baseScale;  //0x28, the Entry's scale; multiplies scale
	f32 scale;      //0x2C, grows as 0.2 * frame + 0.3
} effHagetakaFlushData;
EffectEntry* effHagetakaFlushN64Entry(s32 type, s32 unused, f32 x, f32 y, f32 z, f32 scale);
void effHagetakaFlushMain(EffectEntry* effect);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
void effHagetakaFlushDisp(CameraId camId, void* param);
