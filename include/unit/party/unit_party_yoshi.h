#pragma once

#include "evt/evt_cmd.h"
#include "battle/battle_status_effect.h"

USER_FUNC(_get_swallow_param);
USER_FUNC(btl_yoshi_yoroyoro_jump_calc_param);
USER_FUNC(btl_yoshi_yoroyoro_jump_move);
USER_FUNC(yoshi_original_color_anim_set);

void _btlYoshiDisp(CameraId cameraId, void* param);
struct PoseSoundEntry;
s32 _yoshi_slide_move_sound(BattleUnit* unit, struct PoseSoundEntry* entry);
USER_FUNC(_get_nomikomi_hit_position);
USER_FUNC(_check_swallow_attribute);
USER_FUNC(_wait_yoshig_run);
USER_FUNC(_wait_yoshig_complete);
USER_FUNC(_gundan_yoshi_run_effect);
