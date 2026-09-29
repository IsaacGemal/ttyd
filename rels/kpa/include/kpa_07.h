#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"

void kpa_stg6_014_cb(s32 state, smartEntry* smart);
void kpa_stg6_012_1_cb(s32 state, smartEntry* smart);
USER_FUNC(set_starstone_ry);
EVT_DEF(kpa_07_hipattack_eff);
EVT_DEF(kpa_07_evt);
EVT_DEF(kpa_07_init_evt);
