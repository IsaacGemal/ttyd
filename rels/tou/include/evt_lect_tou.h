#pragma once

#include "evt/evt_cmd.h"

// This unit's call site zero-extends iconId, so it sees iconDispGxAlpha with a u16 iconId (icondrv.h declares
// s16; documented exception).
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);
// This REL drops the icondrv.h include (it conflicts with the u16 iconDispGxAlpha above);
// these two are the only other icondrv.h prototypes the file needs, copied verbatim.
void iconFlagOn(const char* name, s16 flags);
void iconFlagOff(const char* name, s16 mask);

EVT_DEF(yoshi_party_lecture_init);
EVT_DEF(yoshi_party_lecture);
EVT_DEF(all_party_lecture_27_data_2DF64);
EVT_DEF(hammer_lv2_juyoitem_lecture_init);
EVT_DEF(hammer_lv2_juyoitem_lecture_sub01);
EVT_DEF(hammer_lv2_juyoitem_lecture_sub02);
EVT_DEF(hammer_lv2_juyoitem_lecture);
EVT_DEF(all_juyoitem_lecture_rel_27_data_2F48C);
