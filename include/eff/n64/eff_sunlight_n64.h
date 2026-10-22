#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"
typedef struct effSunlightData {
	/* 0x00 */ s32 type;
	/* 0x04 */ f32 x;
	/* 0x08 */ f32 y;
	/* 0x0C */ f32 z;
	/* 0x10 */ s32 life;
	/* 0x14 */ s32 counter;
	/* 0x18 */ u8 r0;
	/* 0x19 */ u8 g0;
	/* 0x1A */ u8 b0;
	/* 0x1B */ u8 r1;
	/* 0x1C */ u8 g1;
	/* 0x1D */ u8 b1;
	/* 0x1E */ u8 a1;
	/* 0x1F */ u8 pad0;
	/* 0x20 */ f32 offset[5];
	/* 0x34 */ f32 scale;
	/* 0x38 */ s32 alpha;
	/* 0x3C */ s32 alphaTarget;
} effSunlightData;
EffectEntry* effSunlightN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effSunlightMain(EffectEntry* effect);
void effSunlightDisp(CameraId cameraId, void* param);
// .sdata2 float constants shared with the display code, named as in the original unit.
extern const f32 float_256_8041ffa4;
// .rodata / .data objects used by the display code.
extern const f64 double_to_int_802fb7f8;
extern u32 testsrc__s_v[];
