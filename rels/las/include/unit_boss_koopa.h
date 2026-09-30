#pragma once

#include <dolphin/mtx.h>

#include "evt/evt_cmd.h"

typedef struct IconJump {
    Vec start;     // 0x0
    Vec pos;       // 0xC
    Vec target;    // 0x18
    s32 frames;    // 0x24
    f32 speed;     // 0x28
    f32 fallAccel; // 0x2C
    f32 vy;        // 0x30
    u8 pad_0x34[0x7C - 0x34];
} IconJump;

EVT_DEF(first_mario_damage_check);
EVT_DEF(first_mario_damage_event);
