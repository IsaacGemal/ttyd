#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effSparkEntry(s32 a1, s32 a2, f32 a3, f32 a4, f32 a5, f32 a6);
#include <dolphin/mtx.h>
// Work data behind EffectEntry->userdata. It is an array of 0x38-byte elements:
// element 0 is the effect header, elements 1..count-1 are the individual sparks.
typedef struct effSparkData {
    s32 type;     // 0x00
    Vec position; // 0x04
    f32 vx;       // 0x10
    f32 vy;       // 0x14
    f32 targetVx; // 0x18
    f32 targetVy; // 0x1C
    s32 seq;      // 0x20
    f32 angle;    // 0x24
    s32 life;     // 0x28
    s32 counter;  // 0x2C
    s32 alpha;    // 0x30
    f32 scale;    // 0x34
} effSparkData;
