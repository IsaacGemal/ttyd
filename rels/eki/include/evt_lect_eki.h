#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>

// Button state used by lecture_button_* and lecture_button_disp.
typedef struct LectureButton {
    u8 flags;            // 0x0
    u8 seq;              // 0x1
    u8 pad_02[0xA];      // 0x2
    Vec position;        // 0xC
    u8 pad_18[0x28];     // 0x18
    s16 iconId;          // 0x40
    s16 alpha;           // 0x42
} LectureButton;

// The original compiler anchors the position copy four bytes before `position`; model that base.
typedef struct LectureButtonVec {
    f32 pad;
    Vec position;
} LectureButtonVec;

EVT_DEF(boots_lv3_juyoitem_lecture_init);
EVT_DEF(boots_lv3_juyoitem_lecture_sub01);
EVT_DEF(boots_lv3_juyoitem_lecture_sub02);
EVT_DEF(boots_lv3_juyoitem_lecture);
EVT_DEF(all_juyoitem_lecture_rel_7_data_150F8);
