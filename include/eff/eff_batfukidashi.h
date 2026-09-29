#pragma once
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct CoinFukidashiData {
    s32 type;         // 0x0
    Vec position;     // 0x4
    f32 field_0x10;   // 0x10
    s32 field_0x14;   // 0x14
    f32 scale;        // 0x18
    s32 alpha;        // 0x1C
    s32 field_0x20;   // 0x20
    s32 field_0x24;   // 0x24
} CoinFukidashiData;
EffectEntry* effCoinFukidashiEntry(s32 type, s32 coinNum, s32 count, f32 x, f32 y, f32 z);
void effCoinFukidashiMain(EffectEntry* effect);
