#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>
#include <dolphin/types.h>

// One lecture-button display entry. Only the fields this file touches are named.
typedef struct LectureButton {
    u8 flags;          // 0x0
    u8 seq;            // 0x1
    u8 pad_2[0xA];     // 0x2
    Vec position;      // 0xC
    u8 pad_18[0x28];   // 0x18
    s16 iconId;        // 0x40
    s16 alpha;         // 0x42
} LectureButton;

EVT_DEF(plane_majin_lecture_init);
EVT_DEF(plane_majin_lecture);
