#pragma once

#include <dolphin/mtx.h>
#include "driver/effdrv.h"
typedef struct effMahornData {
    BOOL unused;    // 0x0
    Vec position;   // 0x4
    f32 scale;      // 0x10
    f32 field_0x14; // 0x14
    s32 alpha;      // 0x18
    s32 timer;      // 0x1c
    s32 counter;    // 0x20
    s32 field_0x24; // 0x24
} effMahornData;
EffectEntry* effMahornEntry(BOOL unused, s32 count, f32 posX, f32 posY, f32 posZ, f32 scale, s32 unk);
void effMahornMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effMahornDisp(CameraId camId, void* param);
