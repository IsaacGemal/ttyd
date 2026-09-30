#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

// The akari charge effect's work data, as eff_thunder_n64.c reads it; only those fields are modelled.
typedef struct EffAkariChargeData {
	u8 pad_0x0[0x18];
	s32 field_0x18; //0x18
	s32 field_0x1C; //0x1C
	s32 field_0x20; //0x20
	u8 pad_0x24[0x4];
	s32 field_0x28; //0x28
	s32 field_0x2C; //0x2C
	s32 field_0x30; //0x30
} EffAkariChargeData;

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effAkariChargeMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effAkariChargeDisp(CameraId camId, void* param);
