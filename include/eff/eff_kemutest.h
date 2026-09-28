#pragma once
typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
EffectEntry* effKemuTestEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effKemuTestData {
	s32 type;       //0x0
	f32 posX;       //0x4
	f32 posY;       //0x8
	f32 posZ;       //0xC
	f32 velX;       //0x10
	f32 velY;       //0x14
	f32 velZ;       //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	s32 field_0x24; //0x24
	s32 field_0x28; //0x28
	s32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
	f32 field_0x34; //0x34
	f32 field_0x38; //0x38
	f32 field_0x3C; //0x3C
	s32 field_0x40; //0x40
	s32 field_0x44; //0x44
	f32 scale;      //0x48
	u8 field_0x4C;  //0x4C
	u8 field_0x4D;  //0x4D
	u8 field_0x4E;  //0x4E
	u8 pad_0x4F;    //0x4F
	f32 field_0x50; //0x50
	f32 field_0x54; //0x54
} effKemuTestData;
void effKemuTestMain(EffectEntry* effect);
void effKemuTestDisp(CameraId camId, void* param);
void effKemuTestSetRxRz(EffectEntry* effect, f32 rx, f32 rz);
void effKemuTestDrawCam(EffectEntry* effect, s32 camId);
