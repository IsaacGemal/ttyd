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
	s32 colorR;                  // 0x18: shell tint red (TEVREG0 in the shell pass)
	s32 colorG;                  // 0x1C: shell tint green, a sine pulse set every frame
	s32 colorB;                  // 0x20: shell tint blue
	s32 alpha;                   // 0x24: fade-in/out alpha: the bubbles' alpha, and 0.3 times this is the shell's where its texture is opaque
	s32 baseAlpha;               // 0x28: the shell's alpha where its texture alpha is 0 (TEVREG1 alpha); Main keeps it equal to alpha
	f32 scale;                   // 0x2C
	f32 field_0x30;              // 0x30
	f32 field_0x34;              // 0x34
	f32 bubbleX[4];              // 0x38: rising bubble position X
	f32 bubbleY[4];              // 0x48: bubble position Y (spawns at 240, respawns past 480)
	f32 bubbleVelX[4];           // 0x58: bubble X velocity, damped by 0.97 per frame
	f32 bubbleVelY[4];           // 0x68: bubble Y velocity (upward), eased toward 10
	f32 bubbleScale[4];          // 0x78: bubble scale
	s32 bubbleTimer[4];          // 0x88: negative countdown to the bubble spawning; re-armed when bubbleY passes 480
} effOpukuWprotectData;          // 0x98
EffectEntry* effOpukuWprotectN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWprotectMain(EffectEntry* effect);
void effOpukuWprotectDisp(CameraId camId, void* param);
