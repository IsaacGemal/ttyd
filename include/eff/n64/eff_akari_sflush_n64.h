#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effAkariSflushN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 scale);
void effAkariSflushMain(EffectEntry* effect);
typedef struct EffAkariSflushData {
	s32 type;       //0x0
	f32 x;          //0x4
	f32 y;          //0x8
	f32 z;          //0xC
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	f32 scale;      //0x28
	f32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
} EffAkariSflushData;
