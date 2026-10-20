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
	s32 frame;      // 0x14: animation frame, counts up each update
	u32 color1R;    // 0x18
	u32 color1G;    // 0x1c
	u32 color1B;    // 0x20
	u32 color1A;    // 0x24
	u32 color2R;    // 0x28
	u32 color2G;    // 0x2c
	u32 color2B;    // 0x30
	u32 color2A;    // 0x34
	f32 timer;      // 0x38: float frame timer, incremented but never read
	f32 velY;       // 0x3c: vertical velocity, added to y each update
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

// effVaporDisp's .sdata2/.rodata/.sbss2 data.
extern const f32 float_deg2rad_804200d8;
extern const f32 float_0p01_804200e0;
extern const f32 float_0_804200e4;
extern const f32 float_5_804200e8;
extern const f32 float_0p0039062_804200ec;
extern const f32 float_256_804200f0;
extern const f32 float_0p032258_804200f4;
extern const f32 float_0p007874_804200f8;
extern const f32 float_32_804200fc;
extern const f64 double_to_int_802fb8f8;
extern const f64 double_to_int_mask_802fb900;
extern GXColor unk_804232b8;
extern GXColor unk_804232bc;
