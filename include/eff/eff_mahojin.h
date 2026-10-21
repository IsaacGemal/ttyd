#pragma once
#include <dolphin/mtx.h>
#include <dolphin/types.h>
#include "driver/camdrv.h"
#include "driver/effdrv.h"
typedef struct MahojinWork {
    s32 type;        // 0x0
    Vec position;    // 0x4
    s32 timer;       // 0x10
    s32 field_0x14;  // 0x14
    s32 animId[2];   // 0x18
    s32 alpha;       // 0x20
    s32 state;       // 0x24
    void* drawArg;                               // 0x28, argument for drawFunc
    void (*drawFunc)(CameraId camId, void* arg); // 0x2C
} MahojinWork;
void effMahojinDisp(CameraId camId, void* param);
void effMahojinMain(EffectEntry* effect);
EffectEntry* effMahojinEntry(s32 type, f32 x, f32 y, f32 z);
