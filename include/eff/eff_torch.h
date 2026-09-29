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
} effTorchData;
