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
	/* 0x20 */ f32 field_0x20;
	/* 0x24 */ f32 field_0x24;
	/* 0x28 */ f32 field_0x28;
	/* 0x2C */ f32 field_0x2C;
	/* 0x30 */ f32 field_0x30;
	/* 0x34 */ f32 scale;
	/* 0x38 */ s32 field_0x38;
	/* 0x3C */ s32 field_0x3C;
} effSunlightData;
EffectEntry* effSunlightN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
void effSunlightMain(EffectEntry* effect);
void effSunlightDisp(CameraId cameraId, void* param);
// .sdata2 float constants shared with the display code, named as in the original unit.
extern const f32 float_0_8041ff80;
extern const f32 float_0p25_8041ff8c;
extern const f32 float_6p2832_8041ff90;
extern const f32 float_360_8041ff94;
extern const f32 float_4_8041ff98;
extern const f32 float_0p05_8041ff9c;
extern const f32 float_0p01_8041ffa0;
extern const f32 float_1200_8041ff60;
extern const f32 float_neg1200_8041ff64;
extern const f32 float_neg1600_8041ff68;
extern const f32 float_1600_8041ff6c;
extern const f32 float_1_8041ff70;
extern const f32 float_32768_8041ff74;
extern const f32 float_0p0625_8041ff78;
extern const f32 float_0p015625_8041ff7c;
extern const f32 float_0p0039062_8041ff84;
extern const f32 float_0p125_8041ff88;
extern const f32 float_256_8041ffa4;
// .rodata / .data / .sbss2 objects used by the display code.
extern const Vec vec3_802fb7c8[4];
extern const f64 double_to_int_802fb7f8;
extern u32 testsrc__s_v[];
extern GXColor unk_80423290;
extern GXColor unk_80423294;
