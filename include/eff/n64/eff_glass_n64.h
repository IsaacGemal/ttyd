#pragma once

#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct effGlassData {
	s32 type;       // 0x00
	Vec position;   // 0x04
	s32 life;       // 0x10
	s32 counter;    // 0x14
	s32 color1R;    // 0x18
	s32 color1G;    // 0x1C
	s32 color1B;    // 0x20
	s32 alpha;      // 0x24
	s32 color2R;    // 0x28
	s32 color2G;    // 0x2C
	s32 color2B;    // 0x30
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
extern const f32 float_deg2rad_8041f080;
extern const f32 float_4_8041f084;
extern const f32 float_256_8041f088;
extern const f32 float_0p015625_8041f08c;
extern const f32 float_0p0078125_8041f090;
extern GXColor unk_80423120;
extern GXColor unk_80423124;
extern GXColor unk_80423128;
void effGlassMain(EffectEntry* effect);
void effGlassDisp(CameraId camId, void* param);
extern const f32 float_neg0p1_8041f0a4;
EffectEntry* effGlassN64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
