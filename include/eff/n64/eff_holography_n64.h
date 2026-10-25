#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct EffHolographyData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	s32 lifetime;   // 0x10: frames left (1000 = held, 16 once told to close); deleted below 0
	s32 frame;      // 0x14: counts up; picks Disp's vertex table and xlu_data entry
	s32 colorR;     // 0x18: TEVREG0 red
	s32 colorG;     // 0x1C: TEVREG0 green
	s32 colorB;     // 0x20: TEVREG0 blue
	s32 alpha;      // 0x24: fade-in over the first 16 frames, fade-out over the last 16; Disp's TEVREG0 alpha base
	f32 scale;      // 0x28: PSMTXScale (x mirrored)
} EffHolographyData;
void effHolographyMain(EffectEntry* effect);
void effHolographyDisp(CameraId camId, void* param);
EffectEntry* effHolographyN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
