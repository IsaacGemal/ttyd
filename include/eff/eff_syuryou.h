#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/effdrv.h"
// The syuryou effect's work data, as las_28.c reads it; only those fields are modelled.
typedef struct EffSyuryouData {
    u8 pad_0x0[0x4];
    Vec position; // 0x4
    f32 scale;    // 0x10
} EffSyuryouData;
EffectEntry* effSyuryouEntry(s32 type, s32 duration, f32 x, f32 y, f32 z);
