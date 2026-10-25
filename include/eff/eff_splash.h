#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
EffectEntry* effSplashEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct SplashData {
	s32 type;       // 0x0
	Vec position;   // 0x4
	f32 velX;       // 0x10, added to position.x each frame
	f32 velZ;       // 0x14, added to position.z each frame
	f32 scaleX;     // 0x18
	f32 scaleY;     // 0x1C
	f32 scaleZ;     // 0x20
	s32 counter;    // 0x24
	s32 delay;      // 0x28
	f32 velocity;   // 0x2C
	f32 decel;      // 0x30
	u8 pad_0x34[4]; // 0x34
	s32 gotColor;   // 0x38
	GXColor color;  // 0x3C
} SplashData;
void effSplashMain(EffectEntry* effect);
void effSplashDisp(enum CameraId camId, void* param);
void effSplashGetCol(enum CameraId camId, void* param);
