#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"

void kpa_stg1_008_cb(s32 type, smartEntry* smart);
void kpa_stg1_014_2_cb(s32 type, smartEntry* smart);
void kpa_stg1_036_cb(s32 type, smartEntry* smart);
void kpa_stg1_034_cb(s32 type, smartEntry* smart);
USER_FUNC(get_togenoko_pos);
EVT_DEF(kpa_01_door_open);
EVT_DEF(kpa_01_door_close);
EVT_DEF(kpa_01_touzyo_evt);
EVT_DEF(kpa_01_syuppatu_evt);
EVT_DEF(kpa_01_pos_check);
EVT_DEF(douzou_1_touch_evt);
EVT_DEF(douzou_2_touch_evt);
EVT_DEF(douzou_3_touch_evt);
EVT_DEF(togenoko_A_talk_evt);
EVT_DEF(togenoko_B_talk_evt);
EVT_DEF(togenoko_C_init_evt);
EVT_DEF(hammer_AB_init_evt);
EVT_DEF(hammer_AB_talk_evt);
EVT_DEF(hammer_C_talk_evt);
EVT_DEF(hammer_D_talk_evt);
EVT_DEF(hammer_E_init_evt);
EVT_DEF(hammer_E_talk_evt);
EVT_DEF(baba_init_evt);
EVT_DEF(kamek_init_evt);
EVT_DEF(hkoopa_init_evt);
EVT_DEF(kpa_01_init_evt);
