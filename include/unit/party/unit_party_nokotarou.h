#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
#include "manager/evtmgr.h"
extern const f32 float_2_8041e038;
extern const f32 float_0p006_8041e03c;
s32 _check_mario_move_count(EventEntry* event, BOOL isFirstCall);
s32 _color_lv_set(EventEntry* event, BOOL isFirstCall);
USER_FUNC(_tsuranuki_effect_control);
