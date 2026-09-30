#pragma once

#include "driver/mobjdrv.h"
#include "evt/evt_cmd.h"

USER_FUNC(sw_entry);
s32 sw_main(MapObjectEntry* obj);
void sw_reset(void);
s32 sw_cnt(MapObjectEntry* obj);
void sw_cntup(MapObjectEntry* obj);
EVT_DEF(evt_box_open);
EVT_DEF(evt_sw_complete);
EVT_DEF(eki_06_init_evt);
