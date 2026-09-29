#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
typedef struct EffGuruguruData {
    s32 field_0x00; // 0x0
    f32 field_0x04; // 0x4
    f32 field_0x08; // 0x8
    f32 field_0x0C; // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 field_0x1C; // 0x1C
    f32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    f32 field_0x2C; // 0x2C
} EffGuruguruData;
EffectEntry* effGuruguruN64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f);
