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
USER_FUNC(evt_cam_letter_box_camid);
USER_FUNC(evt_cam_letter_box_onoff);

USER_FUNC(evt_cam3d_event_from_road);
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_dolly);
#endif
USER_FUNC(evt_cam3d_evt_set_at);
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_set_at_rel);
#endif
USER_FUNC(evt_cam3d_evt_set_now);
USER_FUNC(evt_cam3d_evt_set_npc_rel);
#ifdef MUST_MATCH
USER_FUNC(evt_cam3d_evt_set_npc_rel_str);
#endif
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
USER_FUNC(evt_cam_road_reset2);
