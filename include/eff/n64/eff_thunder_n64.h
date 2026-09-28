#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

typedef struct EffThunderData {
	s32 type;           //0x0
	f32 x;              //0x4
	f32 y;              //0x8
	f32 z;              //0xC
	s32 field_0x10;     //0x10
	s32 field_0x14;     //0x14
	s32 field_0x18;     //0x18
	f32 field_0x1C;     //0x1C
	f32 field_0x20;     //0x20
	f32 field_0x24;     //0x24
	f32 field_0x28;     //0x28
	f32 field_0x2C;     //0x2C
	f32 field_0x30;     //0x30
	f32 field_0x34;     //0x34
	f32 field_0x38;     //0x38
	s32 field_0x3C;     //0x3C
	s32 field_0x40;     //0x40
	EffectEntry* field_0x44; //0x44
	EffectEntry* field_0x48; //0x48
} EffThunderData;

EffectEntry* effThunderN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5, f32 a6);
void effThunderMain(EffectEntry* effect);
void effThunderDisp(CameraId camId, void* param);
