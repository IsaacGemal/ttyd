#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// userdata is an array of six of these; the head (index 0) is also read as LensflareData.
typedef struct LensflareEntry {
    s32 field_0;    // 0x00
    s32 field_4;    // 0x04
    f32 x;          // 0x08, head only
    f32 y;          // 0x0C, head only
    f32 z;          // 0x10, head only
    s32 field_14;   // 0x14, alpha: the head's fade, each entry's cos-curve brightness
    s32 field_18;   // 0x18, head only
    s32 field_1C;   // 0x1C, head only
    f32 field_20;   // 0x20
    f32 field_24;   // 0x24
    f32 field_28;   // 0x28
    f32 field_2C;   // 0x2C
    f32 field_30;   // 0x30
} LensflareEntry;   // 0x34
typedef struct LensflareData {
    s32 field_0;               // 0x00
    s32 field_4;               // 0x04
    f32 x;                     // 0x08
    f32 y;                     // 0x0C
    f32 z;                     // 0x10
    s32 field_14;              // 0x14
    s32 field_18;              // 0x18
    s32 field_1C;              // 0x1C
    u8 pad_20[0x34 - 0x20];    // 0x20
    LensflareEntry entries[5]; // 0x34
} LensflareData;               // 0x138
EffectEntry* effLensflareN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2);
void effLensflareMain(EffectEntry* effect);
void effLensflareDisp(CameraId cameraId, void* param);
