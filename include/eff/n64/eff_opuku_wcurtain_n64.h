#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/effdrv.h"
typedef struct effOpukuWcurtainData {
	s32 type;             // 0x00
	Vec position;         // 0x04
	s32 duration;         // 0x10
	s32 counter;          // 0x14
	s32 colorR;           // 0x18, TEVREG0 red
	s32 colorG;           // 0x1C, TEVREG0 green
	s32 colorB;           // 0x20, TEVREG0 blue
	s32 alpha;            // 0x24, TEVREG0 alpha, fades in over the first 16 frames and out over the last 16 (the TEV stage takes alpha from TEXA * RASA)
	f32 scale;            // 0x28
	f32 bottomY[12];      // 0x2C, per column: bottom vertex y = 10 * (60 + bottomY) (top vertex at 600), top vertex t = (s32)bottomY << 5
	f32 bottomVelY[12];   // 0x5C, per column: added to bottomY each frame; itself drops by 0.05 per frame
} effOpukuWcurtainData;
// One N64 effect vertex: s16 position + s16 texture coords + RGBA8 color (14 bytes).
typedef struct OpukuWcurtainVtx {
	s16 x;  // 0x0
	s16 y;  // 0x2
	s16 z;  // 0x4
	s16 s;  // 0x6
	s16 t;  // 0x8
	u8 r;   // 0xA
	u8 g;   // 0xB
	u8 b;   // 0xC
	u8 a;   // 0xD
} OpukuWcurtainVtx;
EffectEntry* effOpukuWcurtainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effOpukuWcurtainMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effOpukuWcurtainDisp(CameraId camId, void* param);
