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
    s32 alpha;      // 0x20
    s32 timer;      // 0x24 header: countdown, deletes the effect below 0; 100+ never counts down
    s32 frame;      // 0x28
    f32 scaleX;     // 0x2C
    f32 scaleY;     // 0x30
    f32 scaleZ;     // 0x34
    f32 angle;      // 0x38
    s32 damage;     // 0x3C header: number drawn by effNumberGX
    u32 prevRetraceCount; // 0x40 header: gp->retraceCount at the last Main
    void* anim;     // 0x44
} DamageStarData;
EffectEntry* effDamageStarEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 speed, f32 angle);
void effDamageStarMain(EffectEntry* effect);
void effDamageStarDisp(CameraId camId, void* param);
