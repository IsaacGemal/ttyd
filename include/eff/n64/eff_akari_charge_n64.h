#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

typedef struct EffAkariChargeParticle {
	s32 type;        //0x0, head only
	f32 x;           //0x4
	f32 y;           //0x8
	f32 z;           //0xC
	s32 life;        //0x10
	s32 counter;     //0x14
	u8 pad_0x18[0xC]; //0x18
	s32 alpha;       //0x24, fade times the head alpha
	u8 pad_0x28[0xC]; //0x28
	f32 scale;       //0x34
	f32 field_0x38;  //0x38
	f32 angle;       //0x3C, degrees
	s32 index;       //0x40, texture column (0-15)
} EffAkariChargeParticle; //0x44

typedef struct EffAkariChargeWork {
	s32 type;        //0x0
	f32 x;           //0x4
	f32 y;           //0x8
	f32 z;           //0xC
	s32 life;        //0x10, 1000 when the caller passes a2 <= 0
	s32 counter;     //0x14, frames since the start; also the texture scroll in disp
	s32 reg0R;       //0x18, TEVREG0 colour of the particles (set by the caller)
	s32 reg0G;      //0x1C
	s32 reg0B;      //0x20
	s32 alpha;       //0x24, head fade in and out
	s32 reg1R;       //0x28, TEVREG1 colour (GXSetTevColor(GX_TEVREG1))
	s32 reg1G;       //0x2C
	s32 reg1B;       //0x30
	f32 field_0x34; //0x34
	f32 scale;       //0x38, effect scale (k in disp)
	s32 field_0x3C;  //0x3C
	s32 field_0x40;  //0x40
	// Flexible array (sizeof(work) == 0x44): the disp function reuses the work pointer as the particle
	// cursor, so `data++` must step one 0x44-byte particle, not the whole 0x5D8 allocation.
	EffAkariChargeParticle particles[]; //0x44
} EffAkariChargeWork;

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effAkariChargeMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effAkariChargeDisp(CameraId camId, void* param);
