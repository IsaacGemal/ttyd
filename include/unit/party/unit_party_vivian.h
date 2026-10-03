#pragma once

#include "battle/battle_seq_end.h"

#include "evt/evt_cmd.h"

#include <dolphin/types.h>

USER_FUNC(battle_evt_majo_disp_off);
USER_FUNC(battle_evt_majo_disp_on);
EVT_DEF(vivian_shadow_tail_event);
USER_FUNC(_vivian_make_extra_work_area);
s32 _get_move_frame(EventEntry* event, BOOL isFirstCall);
USER_FUNC(unk_8017d03c);
USER_FUNC(_make_kagenuke_weapon);
USER_FUNC(_disp_heart_entry);
USER_FUNC(_disp_heart_entry_stop);
USER_FUNC(_disp_heart_entry_stop_check);
void _disp_heart(CameraId cameraId, void* param);
