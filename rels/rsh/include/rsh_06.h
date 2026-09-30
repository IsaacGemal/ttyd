#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/mtx.h>

USER_FUNC(npc_anim_wait);
void moamoa_joint_position(void* entry, Vec* out);
void evalOn(void);  // TODO: real signature
EVT_DEF(evt_resha_init);
EVT_DEF(evt_great_moamoa);
EVT_DEF(moamoa_debug);
EVT_DEF(rsh_06_init_evt);
EVT_DEF(rsh_06_a_init_evt);
EVT_DEF(rsh_06_init_evt_nidome);
EVT_DEF(rsh_06_a_init_evt_nidome);
