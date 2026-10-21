#pragma once
#include "driver/effdrv.h"

typedef struct effBreathFireData {
	s32 kind;          // 0x00: 0 = plain flame (alpha fades near the end), 1 = rising (rise accumulates), 2 = eases scale to endScale
	s32 childrenLeft;  // 0x04: child flames still to spawn (the child gets childrenLeft - 1)
	s32 seed;          // 0x08: random-table seed for the curve bend; a child spawns on the frame after it
	f32 posX;          // 0x0C
	f32 posY;          // 0x10
	f32 posZ;          // 0x14
	f32 startX;        // 0x18: position at spawn; the curve runs from here to endX/Y/Z
	f32 startY;        // 0x1C
	f32 startZ;        // 0x20
	f32 endX;          // 0x24
	f32 endY;          // 0x28
	f32 endZ;          // 0x2C
	f32 baseScale;     // 0x30
	f32 endScale;      // 0x34: kind 2 eases scale toward this
	f32 scale;         // 0x38: draw scale (kind 0 eases toward 2.5, kind 1 toward 0.3, kind 2 toward endScale)
	f32 scaleRate;     // 0x3C: ease rate for kind 2
	u32 alpha;         // 0x40: TEVREG0 alpha
	s32 duration;      // 0x44: total frames
	s32 life;          // 0x48: frames left; deleted below 0
	s32 age;           // 0x4C: frames elapsed
	f32 bendX;         // 0x50: random per-axis bend added to the curve (scaled by the start-to-end gap)
	f32 bendY;         // 0x54
	f32 bendZ;         // 0x58
	f32 texPos;        // 0x5C: 0.4 * age; its integer part is the texture scroll frame
	f32 rise;          // 0x60: kind 1 vertical offset, += 0.01 * age each frame
	u32 color0R;       // 0x64: TEVREG0 colour
	u32 color0G;       // 0x68
	u32 color0B;       // 0x6C
	u32 color1R;       // 0x70: TEVREG1 colour
	u32 color1G;       // 0x74
	u32 color1B;       // 0x78
} effBreathFireData;

EffectEntry* effBreathFireN64Entry(s32 kind, f32 x, f32 y, s32 children, f32 z, s32 seed, f32 endX, s32 duration, f32 endY, f32 endZ);
void effBreathFireMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effBreathFireDisp(CameraId camId, void* param);
