#pragma once
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include <dolphin/mtx.h>
// The work array behind EffectEntry->userdata. Entry 0 is the header; entries
// 1..count-1 are the individual batten elements and share the same 0x5C shape.
typedef struct BattenData {
    s32 type;       // 0x00
    Vec position;   // 0x04
    f32 scale;      // 0x10
    u8 alpha;       // 0x14
    u8 pad_0x15[3]; // 0x15
    s32 state;      // 0x18
    s32 timer;      // 0x1C
    f32 field_0x20; // 0x20
    s32 animPoseId; // 0x24
    Mtx matrix;     // 0x28
    f32 field_0x58; // 0x58
} BattenData;
EffectEntry* effBattenEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale);
void effBattenMain(EffectEntry* effect);
void effBattenDisp(CameraId camId, void* param);
void effBattenDisp2(CameraId camId, void* param);
