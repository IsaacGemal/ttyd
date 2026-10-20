#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
#include "driver/camdrv.h"

typedef struct EffRadiationData {
	s32 type;         // 0x00
	Vec sortPos;      // 0x04, Main's dispCalcZ key (the entry never writes it)
	f32 x;            // 0x10
	f32 y;            // 0x14
	f32 z;            // 0x18
	f32 field_0x1C;   // 0x1C
	f32 field_0x20;   // 0x20
	s32 alpha;        // 0x24, vertex colour alpha (over 255)
	s32 life;         // 0x28, frames left (head: 100 or 10 when flagged)
	s32 age;          // 0x2C, frames since spawn
	u8 pad_0x30[0x4]; // 0x30
	f32 field_0x34;   // 0x34
	f32 field_0x38;   // 0x38
	f32 field_0x3C;   // 0x3C
	f32 field_0x40;   // 0x40
	u8 pad_0x44[0x4]; // 0x44
	f32 field_0x48;   // 0x48
	f32 field_0x4C;   // 0x4C
	f32 field_0x50;   // 0x50
	f32 field_0x54;   // 0x54
	f32 offset;       // 0x58, translation along the rotated X axis
	f32 rotX;         // 0x5C, degrees
	f32 rotY;         // 0x60, degrees
	f32 rotZ;         // 0x64, degrees
	f32 startOffset;  // 0x68
	f32 speed;        // 0x6C
	f32 startRotX;    // 0x70
	f32 startRotY;    // 0x74
	f32 startRotZ;    // 0x78
	s32 lifetime;     // 0x7C, value `life` is reset to
	f32 rotXVel;      // 0x80
	f32 rotYVel;      // 0x84
	f32 rotZVel;      // 0x88
	f32 offsetVel;    // 0x8C
	s32 holdTimer;    // 0x90
	s32 additive;     // 0x94, nonzero: blend and no-z-write draw state
	CameraId camId;   // 0x98
} EffRadiationData;
extern s32 r_data[];
EffectEntry* effRadiationN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5);
void effRadiationMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effRadiationDisp(CameraId camId, void* param);
