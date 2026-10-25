#pragma once
#include "driver/effdrv.h"
#include <dolphin/mtx.h>

typedef struct CoinFukidashiData {
    s32 type;         // 0x0
    Vec position;     // 0x4
    f32 offsetY;      // 0x10, rise above position (offset_tbl[offsetIdx] / 100); moves the coin and the number
    s32 coinNum;      // 0x14, the number printed beside the coin
    f32 scale;        // 0x18
    s32 alpha;        // 0x1C
    s32 life;         // 0x20, frames left (1000+: never counts down; effect flag 4 sets 16); fades out below 16, deleted below 0
    s32 offsetIdx;    // 0x24, index into offset_tbl, wraps at 61
} CoinFukidashiData;
EffectEntry* effCoinFukidashiEntry(s32 type, f32 x, f32 y, f32 z, s32 coinNum, s32 life);
void effCoinFukidashiMain(EffectEntry* effect);
#include "driver/camdrv.h"
void effCoinFukidashiDisp(CameraId cameraId, void* param);
