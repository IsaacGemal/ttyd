#pragma once

#include <dolphin/mtx.h>

#include "evt/evt_cmd.h"

void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);

typedef struct LectureButton {
    u8 flags;               // 0x0
    u8 state;               // 0x1
    u8 pad_2[0x0C - 0x2];   // 0x2
    Vec position;           // 0xC
    u8 pad_18[0x40 - 0x18]; // 0x18
    s16 iconId;             // 0x40
    s16 alpha;              // 0x42
} LectureButton;

EVT_DEF(ship_majin_lecture_init);
EVT_DEF(ship_majin_lecture);
