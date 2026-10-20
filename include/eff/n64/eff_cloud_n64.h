#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct effCloudData {
	s32 type;             // 0x00
	f32 x;                // 0x04
	f32 y;                // 0x08
	f32 z;                // 0x0C
	s32 lifetime;         // 0x10
	s32 frame;            // 0x14
	s32 color0R;          // 0x18 GX_TEVREG0
	s32 color0G;          // 0x1C
	s32 color0B;          // 0x20
	s32 alpha;            // 0x24 GX_TEVREG0 alpha
	s32 color1R;          // 0x28 GX_TEVREG1
	s32 color1G;          // 0x2C
	s32 color1B;          // 0x30
	f32 scale;            // 0x34
	f32 headX;            // 0x38
	f32 headY;            // 0x3C
	f32 headZ;            // 0x40
	f32 velX;             // 0x44
	f32 velY;             // 0x48
	f32 velZ;             // 0x4C
	// Trail ring: a new entry is pushed at trailHead whenever the head moves.
	f32 trailX[30];       // 0x50
	f32 trailY[30];       // 0xC8
	f32 trailZ[30];       // 0x140
	s32 trailFrame[30];   // 0x1B8 frame the entry was pushed (drives width and fade)
	s32 trailActive[30];  // 0x230
	s32 trailHead;        // 0x2A8
	f32 trailLength[30];  // 0x2AC cumulative path length (drives the texture U)
} effCloudData;

// One GX vertex in this unit's interleaved format (see effVtxArray): 14 bytes of
// position/texcoord/colour; effCloudDisp writes two per live ring entry.
typedef struct cloudVtx {
	s16 x;
	s16 y;
	s16 z;
	s16 u;
	s16 v;
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} cloudVtx;

EffectEntry* effCloudN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale);
void effCloudMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCloudDisp(CameraId cameraId, void* param);
