#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
EffectEntry* effRippleEntry(s32 int1, f32 float2, f32 float3, f32 float4);
// Per-effect data for the ripple effects (also used by eff_ripple_n64.c).
typedef struct EffRippleData {
    s32 type;       // 0x00
    f32 x;          // 0x04
    f32 y;          // 0x08
    f32 z;          // 0x0C
    f32 scale;      // 0x10, grows every frame and is multiplied into the drawn XZ scale
    f32 field_0x14; // 0x14
    s32 alpha;      // 0x18, GX_TEVREG0 alpha
    s32 timer;      // 0x1C, counts down; the effect is deleted below zero
    s32 age;        // 0x20, counts up every frame
    s32 scrollV;    // 0x24, texture V offset, wraps at 32
    f32 field_0x28; // 0x28
    f32 rx;         // 0x2C
    f32 rz;         // 0x30
    s32 camId;      // 0x34
} EffRippleData;
void effRippleSetCamId(EffectEntry* effect, s32 camId);
void effRippleSetPosition(EffectEntry* effect, f32 x, f32 y, f32 z);
void effRippleSetRxRz(EffectEntry* effect, f32 rx, f32 rz);
