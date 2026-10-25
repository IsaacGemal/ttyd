#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"

typedef struct effSnowDustData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 spreadX;    // 0x10: width of the random spawn range across the view
	f32 spreadY;    // 0x14: height of the random spawn range above y
	f32 velX;       // 0x18: eases toward targetVelX each frame
	f32 velY;       // 0x1C: eases toward targetVelY each frame
	u8 pad_0x20[4]; // 0x20
	f32 targetVelX; // 0x24
	f32 targetVelY; // 0x28
	u8 pad_0x2C[4]; // 0x2C
	s32 frame;      // 0x30: particle frame, 0..29; negative until its spawn delay runs out (hidden)
	s32 life;       // 0x34: entry: frames left (1000 = no limit, 0x10 after a restart)
	s32 angle;      // 0x38: particle: spin in degrees (type 6); entry: frames since spawn
	s32 colorR;     // 0x3C: particle colour, sampled at spawn
	s32 colorG;     // 0x40
	s32 colorB;     // 0x44
	s32 alpha;      // 0x48: 0..255; a_data[frame] per frame, life << 5 near the end of the entry
	s32 topR;       // 0x4C: entry colour (TEVREG1 of the top layer)
	s32 topG;       // 0x50
	s32 topB;       // 0x54
	s32 topA;       // 0x58
	f32 scale;      // 0x5C
	f32 texScrollX; // 0x60: tex-matrix offset, grows by 0.2 each frame
	f32 texScrollY; // 0x64: grows by 0.6 each frame
	s32 camId;      // 0x68: camera (4 = CAMERA_3D)
} effSnowDustData;

EffectEntry* effSnowDustN64Entry(s32 type, f32 x, f32 y, f32 z, s32 count, s32 life, f32 spreadX, f32 spreadY, f32 scale);
void effSnowDustMain(EffectEntry* effect);
void effSnowDustDisp(CameraId cameraId, void* param);
