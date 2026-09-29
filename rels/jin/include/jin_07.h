#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(senteresa_okure_move);
USER_FUNC(senteresa_okure_move_init);
void senteresa_get_position(void);  // TODO: real signature
void computePosHermite(void);  // TODO: real signature
EVT_DEF(jin_07_juyo_oumu_vivian);
EVT_DEF(evt_locked);
EVT_DEF(evt_teresa_box_open);
EVT_DEF(jin_07_init_evt);
