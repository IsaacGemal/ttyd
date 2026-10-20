#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effOpukuWprotectData {
	s32 type;                    // 0x00
	f32 x;                       // 0x04
	f32 y;                       // 0x08
	f32 z;                       // 0x0C
	s32 lifetime;                // 0x10: frames left (1000 when no duration is given); deleted below 0
	s32 timer;                   // 0x14: frame counter, wraps to 0x100 past 0x4F1A0
	s32 colorR;                  // 0x18: red of the TEVREG0 colour
	s32 colorG;                  // 0x1C: green of the TEVREG0 colour, a sine pulse set every frame
	s32 colorB;                  // 0x20: blue of the TEVREG0 colour
	s32 alpha;                   // 0x24: alpha of the TEVREG0 colour; the last pass uses 0.3 times this
	s32 alpha2;                  // 0x28: alpha of the TEVREG1 colour
	f32 scale;                   // 0x2C
	f32 field_0x30;              // 0x30
	f32 field_0x34;              // 0x34
	f32 dropX[4];                // 0x38: drop position X
	f32 dropY[4];                // 0x48: drop position Y (starts at 240)
	f32 dropVelX[4];             // 0x58: drop X velocity, damped by 0.97 per frame
	f32 dropVelY[4];             // 0x68: drop Y velocity, eased toward 10
	f32 dropScale[4];            // 0x78: drop scale
	s32 dropTimer[4];            // 0x88: countdown to the drop spawning; re-armed with a new negative value when dropY passes 480
} effOpukuWprotectData;          // 0x98
EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWprotectMain(EffectEntry* effect);
void effOpukuWprotectDisp(CameraId camId, void* param);
