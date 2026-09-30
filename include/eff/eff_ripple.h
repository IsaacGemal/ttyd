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
    f32 field_0x10; // 0x10
    f32 field_0x14; // 0x14
    s32 field_0x18; // 0x18
    s32 field_0x1C; // 0x1C
    s32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    f32 rx;         // 0x2C
    f32 rz;         // 0x30
    s32 camId;      // 0x34
} EffRippleData;
void effRippleSetCamId(EffectEntry* effect, s32 camId);
void effRippleSetPosition(EffectEntry* effect, f32 x, f32 y, f32 z);
void effRippleSetRxRz(EffectEntry* effect, f32 rx, f32 rz);
