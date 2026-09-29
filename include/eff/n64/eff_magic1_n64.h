#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effMagic1N64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
typedef struct effMagic1Data {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 posX;       // 0x10
    f32 posY;       // 0x14
    f32 posZ;       // 0x18
    f32 velX;       // 0x1C
    f32 velY;       // 0x20
    f32 velZ;       // 0x24
    f32 scale;      // 0x28
    s32 timer;      // 0x2C
    s32 life;       // 0x30
    s32 field_0x34; // 0x34
} effMagic1Data;
