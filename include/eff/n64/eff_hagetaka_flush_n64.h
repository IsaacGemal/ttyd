#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effHagetakaFlushData {
	s32 type;       //0x00
	f32 x;          //0x04
	f32 y;          //0x08
	f32 z;          //0x0C
	s32 field_0x10; //0x10
	s32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	f32 field_0x28; //0x28
	f32 field_0x2C; //0x2C
} effHagetakaFlushData;
EffectEntry* effHagetakaFlushN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5);
void effHagetakaFlushMain(EffectEntry* effect);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
void effHagetakaFlushDisp(CameraId camId, void* param);
