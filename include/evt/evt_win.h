#pragma once

#include "evt/evt_cmd.h"
#include "manager/winmgr.h"

USER_FUNC(evt_unitwin_main_func);
USER_FUNC(unk_80174e98);
void evt_unitwin_disp_func(CameraId cameraId, void* param);
void unk_80175710(WinMgrEntry* entry);
void unk_801758fc(WinMgrEntry* entry);
void unk_8017597c(WinMgrEntry* entry);
void unk_8017599c(WinMgrEntry* entry);

USER_FUNC(evt_unitwin_alloc);
USER_FUNC(evt_unitwin_free);
USER_FUNC(evt_unitwin_selltable_setup);
USER_FUNC(set_new_goods_list);
USER_FUNC(unitwin_get_work_ptr);
USER_FUNC(unitwin_getpartylank);
USER_FUNC(unitwin_setmariolevel);
USER_FUNC(unitwin_setpartylank);
USER_FUNC(unitwincoin_disp_onoff);
USER_FUNC(unitwincoin_wait);
#ifdef MUST_MATCH
USER_FUNC(unitwin_icon_delete);
#endif
USER_FUNC(unk_801751f8);
USER_FUNC(unk_8017524c);
