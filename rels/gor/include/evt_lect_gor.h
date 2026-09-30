#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/mtx.h>

// The gor call site zero-extends iconId (`clrlwi r5, r5, 16`), so this unit sees iconDispGxAlpha with a u16
// iconId (icondrv.h declares s16; documented exception). The other two are icondrv.h's, which this unit can't include.
void iconDispGxAlpha(Vec* position, s16 flags, u16 iconId, u8 alpha, f32 scale);
void iconFlagOn(const char* name, s16 flags);
void iconFlagOff(const char* name, s16 mask);

EVT_DEF(christine_party_lecture_init);
EVT_DEF(christine_party_lecture);
EVT_DEF(chuchulina_party_lecture_init);
EVT_DEF(chuchulina_party_lecture);
EVT_DEF(all_party_lecture_10_data_4B0DC);
EVT_DEF(hammer_lv3_juyoitem_lecture_init);
EVT_DEF(hammer_lv3_juyoitem_lecture_sub01);
EVT_DEF(hammer_lv3_juyoitem_lecture_sub02);
EVT_DEF(hammer_lv3_juyoitem_lecture);
EVT_DEF(all_juyoitem_lecture_rel_10_data_4C6C0);
