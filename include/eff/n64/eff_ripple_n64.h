#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
#include "driver/camdrv.h"
extern Vec __declspec(section ".rodata") vec3_802fb520;
typedef struct effRippleN64Data {
	u8 pad_0x00[0x2C];
	f32 field_0x2C; //0x2C
	f32 field_0x30; //0x30
} effRippleN64Data;
void effRippleN64SetRxRz(EffectEntry* effect, f32 rx, f32 rz);
EffectEntry* effRippleN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale);
void effRippleMain(EffectEntry* effect);
void effRippleDisp(CameraId camId, void* param);
