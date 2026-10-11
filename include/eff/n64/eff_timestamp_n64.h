#pragma once

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "driver/effdrv.h"

typedef struct effTimestampVtx {
	/* 0x0 */ s16 x;
	/* 0x2 */ s16 y;
	/* 0x4 */ s16 z;
	/* 0x6 */ s16 s;
	/* 0x8 */ s16 t;
	/* 0xA */ u8 r;
	/* 0xB */ u8 g;
	/* 0xC */ u8 b;
	/* 0xD */ u8 a;
} effTimestampVtx;

// .rodata/.sdata2 constants shared with the display code, named as in the original unit.
extern const f32 float_0_804200b8;
extern const f32 float_deg2rad_804200b0;
extern const f32 float_0p015625_804200b4;
extern const f32 float_6p2832_804200bc;
extern const f32 float_360_804200c0;
extern const f32 float_500_804200c4;
extern const f32 float_200_804200c8;
extern const f32 float_0p01_804200cc;
extern const f32 float_0p05_804200d0;
extern const f32 float_1_804200d4;
extern GXColor unk_804232b0;
extern GXColor unk_804232b4;

EffectEntry* effTimestampN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale);
void effTimestampMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effTimestampDisp(CameraId camId, void* param);
