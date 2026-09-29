#pragma once

typedef struct WinMgrEntry WinMgrEntry;

#include "evt/evt_cmd.h"
USER_FUNC(evt_win_coin_off);
USER_FUNC(evt_win_coin_on);
void coin_disp(WinMgrEntry* entry);
void oneMessageDisp(WinMgrEntry* entry);

USER_FUNC(evt_win_coin_wait);
USER_FUNC(evt_win_item_desttable);
USER_FUNC(evt_win_item_maketable);
USER_FUNC(evt_win_item_select);
USER_FUNC(evt_win_nameent_name);
USER_FUNC(evt_win_nameent_off);
USER_FUNC(evt_win_nameent_on);
USER_FUNC(evt_win_nameent_wait);
USER_FUNC(evt_win_other_select);
USER_FUNC(mainwindow_opendisable);
USER_FUNC(mainwindow_openenable);
