#pragma once
#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"

/* One swirl particle. The first element is also the effect header (type,
   position, lifetime and counters share the element's leading fields). */
typedef struct effRotHammerSuccessData {
    s16 type;       // 0x00
    u8 pad_02[0x2]; // 0x02
    Vec position;   // 0x04
    Vec velocity;   // 0x10
    f32 angle;      // 0x1C
    f32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2c; // 0x2C
    f32 scale;      // 0x30
} effRotHammerSuccessData;
EffectEntry* effRotHammerSuccessN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, s32 a6);
void effRotHammerSuccessMain(EffectEntry* effect);
void effRotHammerSuccessDisp(CameraId camId, void* param);
