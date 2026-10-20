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
    s32 color1R;    // 0x18
    s32 color1G;    // 0x1C
    s32 color1B;    // 0x20
    s32 alpha;      // 0x24
    s32 color2R;    // 0x28
    s32 color2G;    // 0x2C
    s32 color2B;    // 0x30
    f32 scale;      // 0x34
    f32 rot;        // 0x38
    f32 scaleX;     // 0x3C
    f32 scaleY;     // 0x40
} effOpukuJetwData;
// 10-byte entry of the spiral tables (adata / a2data).
typedef struct effOpukuJetwPoint {
    s8 posX;        // 0x0
    s8 posY;        // 0x1
    s16 scaleX;     // 0x2
    s16 scaleY;     // 0x4
    s16 rot;        // 0x6
    u8 alphaScale;  // 0x8
} effOpukuJetwPoint; // 0xA
void effOpukuJetwDisp(CameraId camId, void* param);
void effOpukuJetwMain(EffectEntry* effect);
