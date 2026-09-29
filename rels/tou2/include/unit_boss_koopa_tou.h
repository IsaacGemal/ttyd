#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

typedef struct BossKoopaIconData {
    Vec position;  // 0x00
    Vec curPos;    // 0x0C
    Vec targetPos; // 0x18
    s32 count;     // 0x24
    f32 speed;     // 0x28
    f32 gravity;   // 0x2C
    f32 velocity;  // 0x30
} BossKoopaIconData;
