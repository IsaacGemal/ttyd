#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

typedef struct EffThunderflareData {
	s32 field_0x00; // 0x00
	s32 type;       // 0x04
	f32 x;          // 0x08
	f32 y;          // 0x0C
	f32 z;          // 0x10
	s32 alpha;      // 0x14
	s32 timer;      // 0x18
	s32 counter;    // 0x1C
	f32 angle1;     // 0x20
	f32 angle2;     // 0x24
	f32 scale;      // 0x28
	f32 radius;     // 0x2C
	s32 tevReg0R;   // 0x30, GX_TEVREG0: stage 1's colour where the texture is dark (alpha from `alpha`)
	s32 tevReg0G;   // 0x34
	s32 tevReg0B;   // 0x38
	s32 tevReg1R;   // 0x3C, GX_TEVREG1: stage 1's colour where the texture is bright
	s32 tevReg1G;   // 0x40
	s32 tevReg1B;   // 0x44
} EffThunderflareData;

// .sdata2 float constants owned by this unit.
extern const f32 float_0p5_8042008c;
extern const f32 float_6p2832_80420090;
extern const f32 float_360_80420094;
extern const f32 float_2_80420098;
extern const f32 float_255_8042009c;
extern const f32 float_100_804200a0;

EffectEntry* effThunderflareN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effThunderflareMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effThunderflareDisp(CameraId camId, void* param);
