#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "dolphin/mtx.h"

typedef struct MonitorWork {
    void* image;   // 0x0
    s32 field_4;   // 0x4
    s32 field_8;   // 0x8
} MonitorWork;

// param block passed to the map TEV callback registered by monitor_init
typedef struct MonitorTevParam {
    void* mapObj;    // 0x0
    f32 (*mtx)[4];   // 0x4
    s32 field_8;     // 0x8
    s32 field_C;     // 0xC
    s32 field_10;    // 0x10
    s32 field_14;    // 0x14
    s32 field_18;    // 0x18
} MonitorTevParam;

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
