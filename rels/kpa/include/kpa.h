#pragma once

#include "driver/camdrv.h"
#include "evt/evt_cmd.h"

USER_FUNC(kpa_get_cam_pt);
USER_FUNC(kpa_time_countdown);
USER_FUNC(kpa_stageclear_disp_entry);
void kpa_stageclear_disp(CameraId cameraId, void* param);
USER_FUNC(kpa_pole_hitoff);
USER_FUNC(kpa_set_time);
USER_FUNC(kpa_status_init);
USER_FUNC(kpa_status_main);
void kpa_status_disp(CameraId cameraId, void* param);
USER_FUNC(kpa_muteki_check);
USER_FUNC(kpa_get_msg);
USER_FUNC(kpa_set_scale);
USER_FUNC(unk_kpa_00000a08);
USER_FUNC(kpa_nohitenemy_ct_clear);
USER_FUNC(kpa_add_coin);
USER_FUNC(kpa_make_item_name);
USER_FUNC(kpa_make_mobj_name);
USER_FUNC(kpa_mobj_on);
USER_FUNC(kpa_status_get_stage);
void _unresolved(void);
void _epilog(void);
void _prolog(void);
EVT_DEF(kpa_init);
EVT_DEF(kpa_read_mobj);
EVT_DEF(kpa_coin_evt);
EVT_DEF(kpa_read_item);
EVT_DEF(kpa_dead);
EVT_DEF(kpa_fall_sub);
EVT_DEF(kpa_bgm_change);
EVT_DEF(kpa_stage_start);
EVT_DEF(kpa_status_main_evt);
EVT_DEF(kpa_time);
EVT_DEF(kpa_stage_clear);
EVT_DEF(kpa_00_init_evt);
EVT_DEF(kpa_01_init_evt);
EVT_DEF(kpa_02_init_evt);
EVT_DEF(kpa_03_init_evt);
EVT_DEF(kpa_04_init_evt);
EVT_DEF(kpa_05_init_evt);
EVT_DEF(kpa_06_init_evt);
EVT_DEF(kpa_07_init_evt);
