#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
typedef struct effConfettiN64Data {
	s32 type;        // 0x00
	f32 x;           // 0x04
	f32 y;           // 0x08
	f32 z;           // 0x0C
	u8 unk_0x10[0x24];
	s32 camId;       // 0x34
} effConfettiN64Data; // 0x38
void effConfettiN64SetCamId(EffectEntry* entry, s32 camId);
#include "driver/effdrv.h"
EffectEntry* effConfettiN64Entry(s32 a, s32 b, f32 x, f32 y, f32 z, f32 scale);
