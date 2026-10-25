#pragma once

#include <dolphin/types.h>
// Per-element work data of the BlowN64 effect (stride 0x68).
typedef struct effBlowData {
    u8 pad_0x00[0x8];
    f32 x;             // 0x08
    f32 y;             // 0x0C
    f32 z;             // 0x10
    u8 pad_0x14[0x30];
    f32 scaleX;        // 0x44
    f32 scaleY;        // 0x48
    f32 rotZ;          // 0x4C
    u8 pad_0x50[0x8];
    s32 alpha;         // 0x58, TEVREG1 alpha (effBlowParticle.alpha)
    u8 pad_0x5C[0xC];
} effBlowData;
#include "driver/camdrv.h"
typedef struct EffectEntry EffectEntry;
EffectEntry* effBlowN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 dirX, f32 dirY, f32 dirZ);
void effBlowMain(EffectEntry* effect);
void effBlowDisp(CameraId camId, void* param);
