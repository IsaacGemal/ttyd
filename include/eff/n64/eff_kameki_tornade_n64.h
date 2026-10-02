#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 a4);
#include "driver/camdrv.h"
void effKamekiTornadeDisp(CameraId cameraId, void* param);
void effKamekiTornadeMain(EffectEntry* effect);

// 14-byte N64 vertex, matching effVtxArray's layout (without its 3-vertex colour arrays).
typedef struct KamekiTornadeVtx {
	s16 pos[3]; // 0x0, XYZ
	s16 tex[2]; // 0x6, ST
	u8 clr[4];  // 0xA, RGBA8
} KamekiTornadeVtx; // 0xE

// .sbss2 zero-initialised GXColor template and the .sdata2/.rodata constants this unit loads.
extern GXColor unk_80423160;
extern const f32 float_6p2832_8041f1dc;
extern const f32 float_360_8041f1e0;
extern const f32 float_100_8041f1e4;
extern const f32 float_deg2rad_8041f1e8;
extern const f32 float_0p02_8041f1f0;
extern const f32 float_0p015625_8041f1fc;
extern const f32 float_0p03125_8041f200;
extern const u32 dat_8041f1d8;
extern Vec __declspec(section ".rodata") vec3_802fa84c;
extern Vec __declspec(section ".rodata") vec3_802fa858;
