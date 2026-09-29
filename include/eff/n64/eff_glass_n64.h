#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effGlassData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	s32 life;       // 0x10
	s32 counter;    // 0x14
	s32 field_0x18; // 0x18
	s32 field_0x1C; // 0x1C
	s32 field_0x20; // 0x20
	s32 alpha;      // 0x24
	s32 field_0x28; // 0x28
	s32 field_0x2C; // 0x2C
	s32 field_0x30; // 0x30
	s32 field_0x34; // 0x34
	f32 scale;      // 0x38
	f32 rot;        // 0x3C
	f32 rotVel;     // 0x40
	f32 angle;      // 0x44
	f32 angleVel;   // 0x48
	f32 velX;       // 0x4C
	f32 velY;       // 0x50
	f32 velZ;       // 0x54
	f32 gravity;    // 0x58
} effGlassData;     // 0x5C
// .sdata2 constants shared with the display code, named as in the original unit.
extern const f32 float_0_8041f094;
extern const f32 float_1_8041f098;
EffectEntry* effGlassN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void effGlassMain(EffectEntry* effect);
void effGlassDisp(CameraId camId, void* param);
