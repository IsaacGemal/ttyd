#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(_get_swallow_param);
USER_FUNC(btl_yoshi_yoroyoro_jump_calc_param);
USER_FUNC(btl_yoshi_yoroyoro_jump_move);
USER_FUNC(yoshi_original_color_anim_set);

void _btlYoshiDisp(CameraId cameraId, void* param);
