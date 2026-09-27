#pragma once

typedef struct WinMgrEntry WinMgrEntry;

#include "evt/evt_cmd.h"
USER_FUNC(evt_win_coin_off);
USER_FUNC(evt_win_coin_on);
void coin_disp(WinMgrEntry* entry);
void oneMessageDisp(WinMgrEntry* entry);
