#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"

// fbatHitCheckAll is defined in src/driver/npcdrv.c (currently as void); the original leaves
// fbatHitCheck's result in r3 and toron_check reads it, so it returns a pointer here.
void* fbatHitCheckAll(void);
USER_FUNC(unk_rsh_00002384);
USER_FUNC(toron_check);
void yuurei_callback(s32 state, smartEntry* smart);
void powan_callback(s32 state, smartEntry* smart);
USER_FUNC(bgm_wait_stop);
EVT_DEF(evt_powan_msg);
EVT_DEF(evt_nabe);
EVT_DEF(init_yuurei);
EVT_DEF(evt_shake2);
EVT_DEF(talk_yuurei);
EVT_DEF(evt_1day_end);
EVT_DEF(evt_2day_end);
EVT_DEF(evt_normal_yado);
EVT_DEF(evt_bed);
EVT_DEF(evt_toron_catch);
EVT_DEF(evt_toron_catch2);
EVT_DEF(evt_toron_appear);
EVT_DEF(rsh_02_init_evt);
EVT_DEF(rsh_02_a_init_evt);
EVT_DEF(rsh_02_b_init_evt);
EVT_DEF(rsh_02_c_init_evt);
EVT_DEF(rsh_02_init_evt_nidome);
EVT_DEF(rsh_02_a_init_evt_nidome);
EVT_DEF(rsh_02_b_init_evt_nidome);
EVT_DEF(rsh_02_c_init_evt_nidome);
EVT_DEF(evt_nidome_bed);
