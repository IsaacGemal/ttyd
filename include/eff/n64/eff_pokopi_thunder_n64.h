#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
// Only the fields used by effPokopiThunderMain are named; the reserved runs cover the
// per-particle arrays the other functions of this file use.
typedef struct EffPokopiThunderData {
	s32 type;        // 0x0
	f32 x;           // 0x4
	f32 y;           // 0x8
	f32 z;           // 0xC
	u8 pad_0x10[0x18];
	s32 lifetime;    // 0x28
	s32 timer;       // 0x2C
	s32 field_0x30;  // 0x30
	s32 field_0x34;  // 0x34
	s32 field_0x38;  // 0x38
	s32 field_0x3C;  // 0x3C
	s32 field_0x40;  // 0x40
	s32 field_0x44;  // 0x44
	s32 field_0x48;  // 0x48
	u8 pad_0x4C[0xCC];
	s32 cameraId;    // 0x118
} EffPokopiThunderData;
void effPokopiThunderDisp(CameraId camId, void* param);
#include "driver/effdrv.h"
EffectEntry* effPokopiThunderN64Entry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 endX, f32 endY, f32 endZ, f32 scale);
void effPokopiThunderMain(EffectEntry* effect);
