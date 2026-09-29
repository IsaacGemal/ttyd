#pragma once

#include "evt/evt_window.h"

#include "evt/evt_cmd.h"
USER_FUNC(evt_sub_get_coin);
USER_FUNC(evt_sub_get_mapname);
USER_FUNC(evt_key_get_buttontrg);
USER_FUNC(evt_sub_random);
USER_FUNC(evt_sub_intpl_msec_get_value);
USER_FUNC(evt_sub_intpl_msec_init);
USER_FUNC(evt_sub_random);
USER_FUNC(evt_sub_rumble_onoff);

// evt_sub_tree_access_entry's table, ended by leafType 7
typedef struct TreeAccessEntry {
    s32 leafType;         // 0x00 fall_leaf_n64's type, by case
    const char* hitName;  // 0x04
    const char* trunk;    // 0x08
    const char* leaves1;  // 0x0C
    const char* leaves2;  // 0x10
    s32* event;           // 0x14
} TreeAccessEntry;

// evt_sub_kusa_access_entry's table, ended by field_0x0 == 4
typedef struct KusaAccessEntry {
    s32 field_0x0;        // 0x00
    const char* hitName;  // 0x04
    const char* mapObj;   // 0x08
    s32* event;           // 0x0C
} KusaAccessEntry;

EVT_DEF(evt_sub_tree_access_entry);
EVT_DEF(evt_sub_kusa_access_entry);
EVT_DEF(evt_sub_mail_vibration);

USER_FUNC(evt_key_get_button);
USER_FUNC(evt_key_get_dir);
USER_FUNC(evt_sub_animgroup_async);
USER_FUNC(evt_sub_area_check);
USER_FUNC(evt_sub_check_intersect);
USER_FUNC(evt_sub_countdown_end);
USER_FUNC(evt_sub_countdown_flag_onoff);
USER_FUNC(evt_sub_countdown_get_status);
USER_FUNC(evt_sub_countdown_set_restart_time);
USER_FUNC(evt_sub_countdown_start);
USER_FUNC(evt_sub_get_dir);
USER_FUNC(evt_sub_get_dist);
USER_FUNC(evt_sub_get_language);
USER_FUNC(evt_sub_get_sincos);
USER_FUNC(evt_sub_get_stopwatch);
USER_FUNC(evt_sub_get_system_flag);
USER_FUNC(evt_sub_intpl_get_float);
USER_FUNC(evt_sub_intpl_get_value);
USER_FUNC(evt_sub_intpl_get_value_para);
USER_FUNC(evt_sub_intpl_init);
USER_FUNC(evt_sub_intpl_init_float);
USER_FUNC(evt_sub_intpl_msec_get_value_para);
USER_FUNC(evt_sub_load_progresstime);
EVT_DEF(evt_sub_mail_event);
USER_FUNC(evt_sub_save_playtime);
USER_FUNC(evt_sub_spline_free);
USER_FUNC(evt_sub_spline_get_value);
USER_FUNC(evt_sub_spline_get_value_manual);
USER_FUNC(evt_sub_spline_init);
EVT_DEF(evt_sub_starstone);
USER_FUNC(evt_sub_status_gauge_check_update);
USER_FUNC(evt_sub_status_gauge_force_close);
USER_FUNC(evt_sub_status_gauge_force_open);
EVT_DEF(evt_sub_stopwatch);
USER_FUNC(evt_sub_system_flag_onoff);
void coingetDisp(WinMgrEntry* entry);
USER_FUNC(stone_ry);
USER_FUNC(stone_bg_open);
USER_FUNC(stageclear_wait);
USER_FUNC(set_stop);
USER_FUNC(get_stop);
USER_FUNC(irai_mail_check);
USER_FUNC(mail_evt_gor_04_keyoff_check);
USER_FUNC(evt_sub_get_areaname);
