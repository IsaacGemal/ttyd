#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/mtx.h>

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
