#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>

typedef struct EffCrystalData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	s32 lifetime;   // 0x10
	s32 timer;      // 0x14, counts up every frame
	s32 field_0x18; // 0x18
	s32 variant;    // 0x1C, picks the colour and brightness case in effCrystalMain
	s32 tev0R;      // 0x20, GX_TEVREG0 colour (alpha below)
	s32 tev0G;      // 0x24
	s32 tev0B;      // 0x28
	s32 alpha;      // 0x2C, GX_TEVREG0 alpha: per-variant level scaled by the fade-in and fade-out
	f32 scale;      // 0x30
	f32 scaleStep;  // 0x34, added to scale each frame in the default variant
	s32 camId;      // 0x38, camera the display entry is drawn with (CAMERA_3D)
} EffCrystalData;

EffectEntry* effCrystalN64Entry(s32 type, s32 time, f32 x, f32 y, f32 z, f32 scale);
void effCrystalMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCrystalDisp(CameraId cameraId, void* param);
