#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/types.h>

USER_FUNC(evt_cam3d_evt_set);
USER_FUNC(evt_cam3d_evt_off);
USER_FUNC(evt_cam3d_evt_off);
USER_FUNC(evt_cam3d_evt_set);
USER_FUNC(evt_cam_shake);
USER_FUNC(evt_cam_shake);
USER_FUNC(evt_cam3d_evt_set);
USER_FUNC(evt_cam_letter_box_disable);
inline Vec makeVecByValue(f32 x, f32 y, f32 z);
inline Vec makeVecByValue2(f32 x, f32 y, f32 z);
USER_FUNC(evt_cam_letter_box_camid);
USER_FUNC(evt_cam_letter_box_onoff);

USER_FUNC(evt_cam3d_event_from_road);
USER_FUNC(evt_cam3d_evt_set_at);
USER_FUNC(evt_cam3d_evt_set_now);
USER_FUNC(evt_cam3d_evt_set_npc_rel);
USER_FUNC(evt_cam3d_evt_set_rel);
USER_FUNC(evt_cam3d_evt_set_rel_dir);
USER_FUNC(evt_cam3d_evt_set_xyz);
USER_FUNC(evt_cam3d_evt_xyz_off);
USER_FUNC(evt_cam3d_evt_zoom_in);
USER_FUNC(evt_cam3d_get_shift);
USER_FUNC(evt_cam3d_road_shift_onoff);
USER_FUNC(evt_cam_ctrl_onoff);
USER_FUNC(evt_cam_get_at);
USER_FUNC(evt_cam_get_pos);
USER_FUNC(evt_cam_letter_box_wait);
USER_FUNC(evt_cam_road_reset);
USER_FUNC(evt_cam_shift_reset);
USER_FUNC(evt_cam_type_change);
