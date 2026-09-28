#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
// One star element. The work array is laid out as an array of these starting at
// work+0x48 (element 0 aliases the header fields), so the header and the
// elements share the same 0x48-byte shape.
typedef struct DamageStarData {
    s32 type;       // 0x0
    Vec position;   // 0x4
    f32 vx;         // 0x10
    f32 vy;         // 0x14
    f32 vz;         // 0x18
    f32 rot;        // 0x1C
    u8 alpha;       // 0x20
    u8 pad_0x21[0x24 - 0x21];
    s32 field_0x24; // 0x24
    s32 frame;      // 0x28
    f32 scaleX;     // 0x2C
    f32 scaleY;     // 0x30
    f32 scaleZ;     // 0x34
    f32 angle;      // 0x38
    s32 field_0x3C; // 0x3C
    s32 field_0x40; // 0x40
    void* anim;     // 0x44
} DamageStarData;
EffectEntry* effDamageStarEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 speed, f32 angle);
void effDamageStarMain(EffectEntry* effect);
void effDamageStarDisp(CameraId camId, void* param);
