#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effMagicHoraoData {
	s32 type;       // 0x00
	f32 x;          // 0x04
	f32 y;          // 0x08
	f32 z;          // 0x0C
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	f32 field_0x24; // 0x24
	f32 field_0x28; // 0x28
	s32 state;      // 0x2C
	s32 field_0x30; // 0x30
	f32 alpha;      // 0x34
	s32 field_0x38; // 0x38
	s32 field_0x3C; // 0x3C
	s32 life;       // 0x40
	s32 field_0x44; // 0x44
} effMagicHoraoData;
// .sdata2, defined by the object's data section once the unit is complete
extern const f32 float_1_8041f780;
extern const f32 float_0_8041f770;
EffectEntry* effMagicHoraoN64Entry(s32 type, s32 a1, s32 a2, s32 life, f32 x, f32 y, f32 z, f32 a4, f32 a5, f32 a6);
void effMagicHoraoMain(EffectEntry* effect);
void effMagicHoraoDisp(CameraId cameraId, void* param);
