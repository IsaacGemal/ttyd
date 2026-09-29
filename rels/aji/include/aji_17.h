#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(peach_evt_stg4_read_quiz_data);
USER_FUNC(horogram_set_pose);
USER_FUNC(evt_fuki_btn_set_batu);
USER_FUNC(evt_fuki_btn);
USER_FUNC(peach_dance_after_fail);
USER_FUNC(set_npc_pos);
USER_FUNC(btn_check);
void wait_move_end(void);  // TODO: real signature
void wait_action_begin(void);  // TODO: real signature
void wait_action_end(void);  // TODO: real signature
USER_FUNC(compute_horogram_pos);
USER_FUNC(compute_peach_pos);
USER_FUNC(compute_move_pos);
USER_FUNC(move_center);
USER_FUNC(dance_reverse);
USER_FUNC(dance_flag_check);
USER_FUNC(dance_flag_onoff);
USER_FUNC(dance_data_init);
USER_FUNC(dance_get_step);
USER_FUNC(set_local_rotate);
USER_FUNC(get_local_rotate);
USER_FUNC(get_center);
USER_FUNC(horogram_flag_onoff);
USER_FUNC(horogram_init);
USER_FUNC(horogram_cap_evt);
void peach_capture(void);  // TODO: real signature
USER_FUNC(disp_horogram_evt);
void disp_horogram(void);  // TODO: real signature
USER_FUNC(noise);
void noiseDisp(void);  // TODO: real signature
EVT_DEF(evt_emergency);
EVT_DEF(evt_last);
EVT_DEF(evt_last2);
EVT_DEF(evt_camera);
EVT_DEF(mail_sousin_se);
EVT_DEF(peach_dance_move);
EVT_DEF(peach_dance_pose);
EVT_DEF(peach_dance_action_step);
EVT_DEF(dance_action_turn_sub);
EVT_DEF(peach_dance_action_turn);
EVT_DEF(peach_dance_action_reverse);
EVT_DEF(peach_dance_action_fail);
EVT_DEF(dance_btn_disp_sub);
EVT_DEF(dance_btn_disp);
EVT_DEF(dance_btn_batu);
EVT_DEF(dance_btn_clear);
EVT_DEF(dance_btn_clear_all);
EVT_DEF(wait_action_time);
EVT_DEF(peach_dance_dark);
EVT_DEF(peach_dance_light);
EVT_DEF(peach_evt_dance);
EVT_DEF(tec_talk);
EVT_DEF(aji_17_init_evt);
