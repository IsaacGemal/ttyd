#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
typedef struct effMagic1Data {
    s32 type;         // 0x00
    f32 x;            // 0x04
    f32 y;            // 0x08
    f32 z;            // 0x0C
    f32 offsetX;      // 0x10, accumulated offset from the base position
    f32 offsetY;      // 0x14
    f32 offsetZ;      // 0x18
    f32 velX;         // 0x1C, per-frame offset delta
    f32 velY;         // 0x20
    f32 velZ;         // 0x24
    f32 scale;        // 0x28
    s32 age;          // 0x2C, frames since spawn
    s32 timer;        // 0x30, remaining frames / segment count
    s32 alpha;        // 0x34
} effMagic1Data;
#include "driver/camdrv.h"
void effMagic1Disp(CameraId camId, EffectEntry* effect);
extern const f32 float_0p83_8041f69c;
extern const f32 float_0p9_8041f6a0;
EffectEntry* effMagic1N64Entry(s32 a, s32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
void effMagic1Main(EffectEntry* effect);
