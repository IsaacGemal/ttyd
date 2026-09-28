#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"

// Local view of the akari charge effect's work data, used by eff_thunder_n64.c. Only the fields this
// file reads are modelled; the effect's own source does not have a header in this tree yet.
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
