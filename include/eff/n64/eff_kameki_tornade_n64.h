#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>

typedef struct effKamekiVtx {
	s16 pos[3];  // 0x0, XYZ
	s16 tex[2];  // 0x6, ST
	u8 color[4]; // 0xA, RGBA8
} effKamekiVtx; // 0xE

EffectEntry* effKamekiTornadeN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 a4);
#include "driver/camdrv.h"
void effKamekiTornadeDisp(CameraId cameraId, void* param);
void effKamekiTornadeMain(EffectEntry* effect);
