#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
typedef struct BushN64Data {
	s32 type;       // 0x0
	f32 x;          // 0x4
	f32 y;          // 0x8
	f32 z;          // 0xC
	f32 field_0x10; // 0x10
	f32 field_0x14; // 0x14
	f32 field_0x18; // 0x18
	f32 field_0x1c; // 0x1C
	f32 field_0x20; // 0x20
	s32 field_0x24; // 0x24
	s32 lifetime;   // 0x28
	s32 field_0x2c; // 0x2C
	f32 field_0x30; // 0x30
} BushN64Data;
EffectEntry* effBushN64Entry(s32 type, f32 x, f32 y, f32 z);
void effBushMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effBushDisp(CameraId camId, void* param);
