#pragma once

#include "evt/evt_cmd.h"

extern s32 move_dai[];

USER_FUNC(get_hosi_name);
EVT_DEF(hosi_sw_check);
EVT_DEF(hosi_onoff);
EVT_DEF(hosi_02_sw_evt);
EVT_DEF(hosi_03_sw_evt);
EVT_DEF(hosi_04_sw_evt);
EVT_DEF(hosi_05_sw_evt);
EVT_DEF(hosi_06_sw_evt);
EVT_DEF(hosi_07_sw_evt);
EVT_DEF(hosi_08_sw_evt);
EVT_DEF(hosi_init);
EVT_DEF(las_20_init_evt);
