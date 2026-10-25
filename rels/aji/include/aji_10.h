#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "dolphin/mtx.h"

typedef struct MonitorWork {
    void* image;   // 0x0
    s32 texValid;  // 0x4, set once monitor_cap_capture has copied the frame; monitor_cap_tev draws it only then
    s32 kanbuFlag; // 0x8, monitor_kanbu_flag_onoff
} MonitorWork;

extern s32 evt_emergency[];
extern s32 evt_shuryolight_init_2_text_B870[];
extern s32 evt_shuryolight_run_evt_2_text_B538[];
extern s32 shuryolight_head_normal_2_data_37004[];
extern s32 shuryolight_pow_normal_2_data_36ED8[];
extern s32 shuryolight_tue_normal_2_data_37104[];

void monitor_cap_tev(void* param);
USER_FUNC(monitor_cap_evt);
void monitor_cap_capture(CameraId cameraId, void* param);
USER_FUNC(monitor_kanbu_flag_onoff);
USER_FUNC(monitor_cap_init);
EVT_DEF(monitor_on);
EVT_DEF(monitor_off);
EVT_DEF(monitor_init);
EVT_DEF(peach_move);
EVT_DEF(peach_evt_stg3_syuryo_talk);
EVT_DEF(eff_test);
EVT_DEF(kanbu_init);
EVT_DEF(aji_10_init_evt);
