#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effItemGetEntry(s32 id, f32 x, f32 y, f32 z);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effItemGetData {
	s32 field_0x0;  //0x0
	Vec position;   //0x4
	f32 field_0x10; //0x10
	f32 field_0x14; //0x14
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	f32 field_0x24; //0x24
	f32 field_0x28; //0x28
	f32 field_0x2C; //0x2C
	f32 field_0x30; //0x30
	s32 field_0x34; //0x34
	s32 field_0x38; //0x38
} effItemGetData;
void effItemGetMain(EffectEntry* effect);
void effItemUseDisp(CameraId camId, void* param);
void effItemGetDisp(CameraId camId, void* param);
