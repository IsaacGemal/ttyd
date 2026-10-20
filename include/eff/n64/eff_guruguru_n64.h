#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/effdrv.h"
typedef struct EffGuruguruData {
    s32 type;          // 0x0
    Vec position;      // 0x4
    f32 radius;        // 0x10 orbit radius, eased toward targetRadius
    f32 targetRadius;  // 0x14
    f32 angle;         // 0x18 quad spin in degrees
    f32 angleSpeed;    // 0x1C degrees added to angle per frame
    f32 pitch;         // 0x20 vertical angle in degrees
    s32 yaw;           // 0x24 horizontal angle in degrees
    s32 active;        // 0x28
    f32 drawScale;     // 0x2C matrix scale
} EffGuruguruData;
EffectEntry* effGuruguruN64Entry(s32 type, s32 numParticles, f32 x, f32 y, f32 z, f32 radius);
#include "driver/camdrv.h"
void effGuruguruDisp(CameraId camId, void* param);
void effGuruguruMain(EffectEntry* effect);
