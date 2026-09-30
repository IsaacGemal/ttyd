#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"

USER_FUNC(_full_ap_recover);
void _nancy_disp(CameraId camera, void* param);
USER_FUNC(_nancy_mirage);
void _disp_item_icon(CameraId camera, void* param);
USER_FUNC(_throw_item_icon);
USER_FUNC(_get_throw_first_pos);
USER_FUNC(_naniga_okorukana_check);
USER_FUNC(_hpfp_change_declare_2);
USER_FUNC(_hpfp_change_declare_1);
USER_FUNC(_rank_up);
USER_FUNC(_cam_rot);
USER_FUNC(_sampling_random_target);
