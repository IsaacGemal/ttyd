#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

// The akari charge effect's work data, as eff_thunder_n64.c reads it; only those fields are modelled.
typedef struct EffAkariChargeData {
	u8 pad_0x0[0x18];
	s32 reg0R; //0x18
	s32 reg0G; //0x1C
	s32 reg0B; //0x20
	u8 pad_0x24[0x4];
	s32 reg1R; //0x28
	s32 reg1G; //0x2C
	s32 reg1B; //0x30
} EffAkariChargeData;

EffectEntry* effAkariChargeN64Entry(s32 type, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
void effAkariChargeMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effAkariChargeDisp(CameraId camId, void* param);
