#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effTorchEntry(s32 id, f32 x, f32 y, f32 z, f32 scale);
#include "driver/camdrv.h"
#include <dolphin/mtx.h>
void effTorchMain(EffectEntry* effect);
void effTorchDisp(CameraId camId, void* param);
typedef struct effTorchData {
	s32 id;       // 0x0
	Vec position; // 0x4
	f32 scale;    // 0x10, overall scale (half the Entry's scale); also the Z scale
	f32 scaleX;   // 0x14, multiplies scale on X
	f32 scaleY;   // 0x18, multiplies scale on Y
	u8 field_0x1C; // 0x1C, only cleared
	u8 pad_0x1D[3]; // 0x1D
} effTorchData;
