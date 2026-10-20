#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effPokopiPchargeN64Entry(f32 x, f32 y, f32 z, f32 scale, s32 a, s32 b);
#include "driver/camdrv.h"
typedef struct effPokopiPchargeData {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	s32 lifetime;   // 0x10
	s32 timer;      // 0x14
	u32 r;          // 0x18
	u32 g;          // 0x1C
	u32 b;          // 0x20
	s32 a;          // 0x24
	u32 r2;         // 0x28
	u32 g2;         // 0x2C
	u32 b2;         // 0x30
	f32 scale;      // 0x34
	f32 velX;       // 0x38: per-frame x step of a particle; on the root, the radial offset (vx_data[type]) added in Disp
	f32 rot;        // 0x3C: degrees, the particle's rotation about z
	f32 rotSpeed;   // 0x40: degrees added to rot each frame
	f32 scaleMul;   // 0x44: root only, multiplies scale (1.0)
} effPokopiPchargeData;
void effPokopiPchargeMain(EffectEntry* effect);
void effPokopiPchargeDisp(CameraId camId, void* param);
