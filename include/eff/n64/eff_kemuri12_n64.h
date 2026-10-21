#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
typedef struct effKemuri12Data {
    u16 type;        // 0x0
    u8 pad_2[2];     // 0x2
    Vec position;    // 0x4
    f32 scale;  // 0x10
    s32 life;  // 0x14
    s32 elapsed;  // 0x18
    s32 maxLife;  // 0x1c
    f32 animProgress;  // 0x20
    u8 color[4];     // 0x24
} effKemuri12Data;

void effKemuri12Main(EffectEntry* effect);
void effKemuri12Disp(CameraId camId, void* param);
EffectEntry* effKemuri12N64Entry(s32 type, s32 life, f32 x, f32 y, f32 z, f32 scale);
