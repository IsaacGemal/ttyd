#pragma once

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

// evt_sub_kusa_access_entry's table, ended by kind 4
typedef struct KusaAccessEntry {
    s32 kind;             // 0x00
    const char* hitName;  // 0x04
    const char* mapObj;   // 0x08
    s32* event;           // 0x0C
} KusaAccessEntry;

EVT_DEF(evt_sub_tree_access_entry);
EVT_DEF(evt_sub_kusa_access_entry);
EVT_DEF(evt_sub_mail_vibration);
