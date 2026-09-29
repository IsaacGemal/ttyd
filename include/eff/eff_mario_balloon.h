#pragma once

typedef struct EffectEntry EffectEntry;
typedef struct NpcEntry NpcEntry;

#include <dolphin/types.h>
#include "driver/camdrv.h"
EffectEntry* effMarioBalloonEntry(s32 type, NpcEntry* npc);
// Work data behind EffectEntry->userdata for the Mario balloon effect.
typedef struct EffMarioBalloonData {
	s32 type;        // 0x0
	f32 x;           // 0x4
	f32 y;           // 0x8
	f32 z;           // 0xC
	f32 angle;       // 0x10
	f32 scale;       // 0x14
	NpcEntry* npc;   // 0x18
	s32 cameraId;    // 0x1C
} EffMarioBalloonData;
void effMarioBalloonMain(EffectEntry* effect);
void effMarioBalloonDisp(CameraId camId, void* param);
