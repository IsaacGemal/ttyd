#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(sw_entry);
void sw_main(void);  // TODO: real signature
void sw_reset(void);  // TODO: real signature
void sw_cnt(void);  // TODO: real signature
void sw_cntup(void);  // TODO: real signature
EVT_DEF(evt_box_open);
EVT_DEF(evt_sw_complete);
EVT_DEF(eki_06_init_evt);
