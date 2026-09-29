#pragma once

#include "evt/evt_cmd.h"

// Entry used by the namiyoke entry manager (a slot with a position).
typedef struct NamiyokeEntry {
    s32 flag; // 0x0
    s32 id;   // 0x4
    f32 x;    // 0x8
    f32 y;    // 0xC
    f32 z;    // 0x10
} NamiyokeEntry;

// Entry manager allocated on the map heap by evt_sub_entrymgr_init.
typedef struct NamiyokeEntryMgr {
    s32 count;              // 0x0
    NamiyokeEntry* entries; // 0x4
} NamiyokeEntryMgr;

USER_FUNC(evt_dou_normal_puka);
USER_FUNC(namiyoke_se_check);
USER_FUNC(namiyoke_fail_stop);
USER_FUNC(dou_namiatari_check);
USER_FUNC(nami_running_func);
USER_FUNC(nami_running_init_func);
USER_FUNC(namiyoke_set_mapobj);
USER_FUNC(evt_sub_entrymgr_getreg);
USER_FUNC(evt_sub_entrymgr_setreg);
USER_FUNC(evt_sub_entrymgr_check);
USER_FUNC(evt_sub_entrymgr_delete);
USER_FUNC(evt_sub_entrymgr_entry);
USER_FUNC(evt_sub_entrymgr_init);
EVT_DEF(nami_running);
EVT_DEF(namiyoke_fail_kaiten);
EVT_DEF(namiyoke_fail_shizumu);
EVT_DEF(namiyoke_fail_recover);
EVT_DEF(namiyoke_fail);
EVT_DEF(namiyoke_atari_check);
EVT_DEF(namiyoke_main);
EVT_DEF(namiyoke_init);
EVT_DEF(namiyoke_se);
EVT_DEF(namiyoke_check);
EVT_DEF(dou_10_init_evt);
