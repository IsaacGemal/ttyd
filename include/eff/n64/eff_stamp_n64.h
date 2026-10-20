#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effStampN64Entry(s32 type, f32 x, f32 y, f32 z);

typedef struct effStampData {
    u16 type;       // 0x0
    u8 pad_2[2];    // 0x2
    f32 x;          // 0x4
    f32 y;          // 0x8
    f32 z;          // 0xC
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    f32 field_0x18; // 0x18
    f32 scaleY;     // 0x1C
    f32 scaleX;     // 0x20
    s32 life;       // 0x24
    s32 counter;    // 0x28
    f32 angle;      // 0x2C
    f32 radius;     // 0x30: petal ring radius from the centre
    s32 alpha;      // 0x34
    u8 tevReg0R;    // 0x38 GX_TEVREG0 colour (its alpha is `alpha`); the TEV stage blends it
    u8 tevReg0G;    // 0x39 toward the GX_TEVREG1 colour by the vertex alpha
    u8 tevReg0B;    // 0x3A
    u8 tevReg1R;    // 0x3B GX_TEVREG1 colour
    u8 tevReg1G;    // 0x3C
    u8 tevReg1B;    // 0x3D
    u8 pad_3E[2];   // 0x3E
} effStampData;

#include "driver/camdrv.h"
void effStampMain(EffectEntry* effect);
void effStampDisp(CameraId camId, void* param);
