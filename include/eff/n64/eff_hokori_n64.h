#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

EffectEntry* effHokoriN64Entry(s32 type, s32 a2, f32 x, f32 y, f32 z);
#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effHokoriN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z);
#include "driver/camdrv.h"
#include <dolphin/mtx.h>
typedef struct effHokoriData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	s32 field_0x18; // 0x18
	f32 field_0x1C; // 0x1C
	f32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	s32 life;       // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 field_0x40; // 0x40
	s32 field_0x44; // 0x44
} effHokoriData;
// Owned by this unit's .sdata2; declared extern so the float codegen keeps loading them
// from the named symbols instead of pooling anonymous copies.
extern const f32 float_0p9_8041f118;
extern const f32 float_0p005_8041f11c;
extern const f32 float_0p03125_8041f108;
extern const f32 float_0p010417_8041f10c;
extern const f32 float_deg2rad_8041f114;
// .data base the retail effHokoriDisp reaches its vertex arrays from; it is
// defined in another unit and only its address is used here.
extern u32 lbl_8039D5D0[];
void effHokoriMain(EffectEntry* effect);
void effHokoriDisp(CameraId cameraId, void* param);
