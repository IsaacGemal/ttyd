#pragma once

typedef struct EffectEntry EffectEntry;
#include <dolphin/types.h>
EffectEntry* effOpukuJetwN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
#include <dolphin/mtx.h>
#include "driver/camdrv.h"
typedef struct effOpukuJetwData {
    s32 type;       // 0x0
    Vec position;   // 0x4
    s32 lifetime;   // 0x10
    s32 counter;    // 0x14
    s32 field_0x18; // 0x18
    s32 field_0x1C; // 0x1C
    s32 field_0x20; // 0x20
    s32 field_0x24; // 0x24
    s32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    f32 scale;      // 0x34
    f32 field_0x38; // 0x38
    f32 field_0x3C; // 0x3C
    f32 field_0x40; // 0x40
} effOpukuJetwData;
// 10-byte entry of the spiral tables (adata / a2data).
typedef struct effOpukuJetwPoint {
    s8 field_0x0;  // 0x0
    s8 field_0x1;  // 0x1
    s16 field_0x2; // 0x2
    s16 field_0x4; // 0x4
    s16 field_0x6; // 0x6
    u8 field_0x8;  // 0x8
} effOpukuJetwPoint; // 0xA
void effOpukuJetwDisp(CameraId camId, void* param);
void effOpukuJetwMain(EffectEntry* effect);
extern f32 lbl_803A33D8[];
