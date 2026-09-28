#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effFlameVtx {
	s16 pos[3]; //0x0, XYZ
	s16 tex[2]; //0x6, ST
	u8 clr[4];  //0xA, RGBA8
} effFlameVtx; //0xE
typedef struct effFlameData {
	s32 type;           //0x0
	Vec position;       //0x4
	f32 field_0x10;     //0x10
	f32 field_0x14;     //0x14
	f32 field_0x18;     //0x18
	f32 field_0x1C;     //0x1C
	f32 field_0x20;     //0x20
	f32 field_0x24;     //0x24
	s32 alpha;          //0x28
	s32 life;           //0x2C
	s32 counter;        //0x30
	f32 field_0x34;     //0x34
	f32 field_0x38;     //0x38
	f32 field_0x3C;     //0x3C
	f32 field_0x40;     //0x40
	f32 field_0x44;     //0x44
	f32 field_0x48;     //0x48
	f32 field_0x4C;     //0x4C
	f32 field_0x50;     //0x50
	f32 field_0x54;     //0x54
	f32 field_0x58;     //0x58
	f32 field_0x5C;     //0x5C
	f32 field_0x60;     //0x60
	f32 rot;            //0x64
	u8 color0[3];       //0x68
	u8 color1[4];       //0x6B
	u8 pad_0x6F;        //0x6F
	effFlameVtx vtx[8]; //0x70
} effFlameData;
// .data tables in this unit are addressed through the shared base symbol defined in eff_fireworks.
EffectEntry* effFlameN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5);
void effFlameMain(EffectEntry* effect);
void effFlameDisp(CameraId camId, void* param);
