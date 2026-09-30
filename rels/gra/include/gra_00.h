#pragma once

#include "evt/evt_cmd.h"
#include "memory.h"

void msg_callback(s32 state, smartEntry* smart);
USER_FUNC(camera_yure);
EVT_DEF(evt_yabure_on);
EVT_DEF(evt_yabure_off);
EVT_DEF(evt_nameent);
EVT_DEF(name_seikai);
EVT_DEF(evt_machibuse);
EVT_DEF(evt_majin_key);
EVT_DEF(gra_00_init_evt);
