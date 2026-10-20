#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effMagicHoraoData {
	s32 type;          // 0x00
	f32 x;             // 0x04
	f32 y;             // 0x08
	f32 z;             // 0x0C
	f32 velY;          // 0x10, vertical velocity in the fall state
	f32 field_0x14;    // 0x14, unused
	f32 endScale;      // 0x18, scale eases toward this
	s32 field_0x1C;    // 0x1C, unused
	s32 endColorAlpha; // 0x20, color alpha eases toward this
	f32 scale;         // 0x24
	f32 scaleRate;     // 0x28, per-frame ease rate
	s32 state;         // 0x2C
	s32 timer;         // 0x30, counts down through the fade states
	f32 alpha;         // 0x34, fades out over the first state
	s32 angleBase;     // 0x38, random start angle in degrees
	s32 colorAlpha;    // 0x3C, TEV color alpha
	s32 life;          // 0x40
	s32 counter;       // 0x44, frames elapsed
} effMagicHoraoData;
// .sdata2, defined by the object's data section once the unit is complete
extern const f32 float_1_8041f780;
extern const f32 float_0_8041f770;
EffectEntry* effMagicHoraoN64Entry(s32 type, s32 colorAlpha, s32 endColorAlpha, s32 life, f32 x, f32 y, f32 z, f32 scale, f32 endScale, f32 scaleRate);
void effMagicHoraoMain(EffectEntry* effect);
void effMagicHoraoDisp(CameraId cameraId, void* param);
