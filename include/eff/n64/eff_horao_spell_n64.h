#pragma once

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "driver/effdrv.h"
typedef struct effHoraoSpellData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 life;       // 0x10: frames left, counted down in Main (1000 = endless)
	s32 age;        // 0x14: frames elapsed
	s32 color1R;    // 0x18
	s32 color1G;    // 0x1C
	s32 color1B;    // 0x20
	s32 color1A;    // 0x24: TEVREG0 alpha, faded in and out by Main
	s32 color2R;    // 0x28
	s32 color2G;    // 0x2C
	s32 color2B;    // 0x30
	s32 color2A;    // 0x34: TEVREG1 alpha
	f32 scale;      // 0x38
} effHoraoSpellData;
typedef struct effHoraoSpellVtx {
	s16 pos[3]; // 0x0, XYZ
	s16 tex[2]; // 0x6, ST
	u8 clr[4];  // 0xA, RGBA8
} effHoraoSpellVtx;
EffectEntry* effHoraoSpellN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effHoraoSpellMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effHoraoSpellDisp(CameraId camId, void* param);
