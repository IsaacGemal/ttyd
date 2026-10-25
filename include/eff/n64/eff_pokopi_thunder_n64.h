#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
typedef struct EffPokopiThunderData {
	s32 type;        // 0x0
	f32 x;           // 0x4
	f32 y;           // 0x8
	f32 z;           // 0xC
	f32 startX;      // 0x10 bolt start; Disp spaces the 12 points from start to end
	f32 startY;      // 0x14
	f32 startZ;      // 0x18
	f32 endX;        // 0x1C bolt end
	f32 endY;        // 0x20
	f32 endZ;        // 0x24
	s32 lifetime;    // 0x28
	s32 timer;       // 0x2C
	s32 tev1R;  // 0x30
	s32 tev1G;  // 0x34
	s32 tev1B;  // 0x38
	s32 tev1A;  // 0x3C
	s32 tev2R;  // 0x40
	s32 tev2G;  // 0x44
	s32 tev2B;  // 0x48
	f32 scale;       // 0x4C multiplies the strip's half-width (with ptScale)
	f32 ptX[12];     // 0x50
	f32 ptY[12];     // 0x80
	f32 ptZ[12];     // 0xB0
	f32 ptScale[12]; // 0xE0
	s32 phase;       // 0x110 random 0..359; offsets the per-point wobble angle in Disp
	f32 field_0x114; // 0x114 0.005 * the start-end distance; never read
	s32 cameraId;    // 0x118
} EffPokopiThunderData;
void effPokopiThunderDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
EffectEntry* effPokopiThunderN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale);
void effPokopiThunderMain(EffectEntry* effect);
