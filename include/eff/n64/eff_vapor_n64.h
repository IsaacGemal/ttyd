#pragma once
#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effVaporN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effVaporMain(EffectEntry* effect);

typedef struct effVaporN64Data {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xc
	s32 lifetime;   // 0x10
	s32 field_0x14; // 0x14
	u32 color1R;    // 0x18
	u32 color1G;    // 0x1c
	u32 color1B;    // 0x20
	u32 color1A;    // 0x24
	u32 color2R;    // 0x28
	u32 color2G;    // 0x2c
	u32 color2B;    // 0x30
	u32 color2A;    // 0x34
	f32 field_0x38; // 0x38
	f32 field_0x3c; // 0x3c
	f32 scale;      // 0x40
} effVaporN64Data;
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
void effVaporDisp(CameraId cameraId, void* param);
// Owned by this unit's .rodata/.sdata2; declared extern so the codegen keeps loading them
// from the named symbols instead of pooling anonymous copies.
extern const f32 float_0p5_804200dc;
extern const f32 float_0p02_80420100;
extern const f32 float_neg1p5_80420104;
extern const f32 float_1_80420108;
