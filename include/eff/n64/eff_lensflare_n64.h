#pragma once

#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
// userdata is an array of six of these; the head (index 0) is also read as LensflareData.
typedef struct LensflareEntry {
    s32 field_0x0;  // 0x00
    s32 field_0x4;  // 0x04
    f32 x;          // 0x08, head only
    f32 y;          // 0x0C, head only
    f32 z;          // 0x10, head only
    s32 alpha;      // 0x14, the head's fade, each entry's cos-curve brightness
    s32 lifetime;   // 0x18, head only: frames left (100 never counts down)
    s32 frame;      // 0x1C, head only: frames since entry; fades in over 20, spins the flare
    f32 angle;      // 0x20, degrees, effLensflareDisp's z rotation
    f32 field_0x24; // 0x24
    f32 scale;      // 0x28, effLensflareDisp's PSMTXScale; 0 skips the entry
    f32 radius;     // 0x2C, distance out along angle, 60-80
    f32 radiusVel;  // 0x30, added to radius each frame
} LensflareEntry;   // 0x34
typedef struct LensflareData {
    s32 field_0x0;             // 0x00
    s32 field_0x4;             // 0x04
    f32 x;                     // 0x08
    f32 y;                     // 0x0C
    f32 z;                     // 0x10
    s32 alpha;                 // 0x14
    s32 lifetime;              // 0x18
    s32 frame;                 // 0x1C
    u8 pad_0x20[0x34 - 0x20];  // 0x20
    LensflareEntry entries[5]; // 0x34
} LensflareData;               // 0x138
EffectEntry* effLensflareN64Entry(s32 type, s32 a1, f32 x, f32 y, f32 z, f32 a2);
void effLensflareMain(EffectEntry* effect);
void effLensflareDisp(CameraId cameraId, void* param);
