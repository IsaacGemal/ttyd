#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
typedef struct effMagic1Data {
    s32 type;         // 0x00
    f32 x;            // 0x04
    f32 y;            // 0x08
    f32 z;            // 0x0C
    f32 field_0x10;   // 0x10
    f32 field_0x14;   // 0x14
    f32 field_0x18;   // 0x18
    f32 field_0x1c;   // 0x1C
    f32 field_0x20;   // 0x20
    f32 field_0x24;   // 0x24
    f32 field_0x28;   // 0x28
    s32 field_0x2c;   // 0x2C
    s32 field_0x30;   // 0x30
    s32 field_0x34;   // 0x34
} effMagic1Data;
EffectEntry* effMagic1N64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
void effMagic1Main(EffectEntry* effect);
