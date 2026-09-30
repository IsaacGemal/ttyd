#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effExplosionN64Data {
	u16 type;      // 0x00
	u8 pad_2[2];   // 0x02
	f32 x;         // 0x04
	f32 y;         // 0x08
	f32 z;         // 0x0C
	f32 scale;     // 0x10
	s32 life;      // 0x14
	s32 counter;   // 0x18
	s32 duration;  // 0x1C
	f32 ratio;     // 0x20
	f32 velocity;  // 0x24
	f32 accel;     // 0x28
} effExplosionN64Data;
EffectEntry* effExplosionN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effExplosionMain(EffectEntry* effect);
void effExplosionDisp(CameraId camId, void* param);
extern GXColor unk_804230e8;
extern GXColor unk_804230ec;
extern GXColor unk_804230f0;
extern const u32 dat_8041ed60;
extern const u32 dat_8041ed64;
extern u32 size32x32_tex32x32_vtx[];
