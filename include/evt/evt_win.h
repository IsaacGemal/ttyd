#pragma once

#include "evt/evt_cmd.h"
#include "manager/winmgr.h"

// Work structure for the unit window (evt_win.c; unitwin_get_work_ptr hands it to scripts).
typedef struct UnitWinObj {
	const char* name;      // 0x0
	const char* hitName;   // 0x4, compared with hitGetName()
	void* evtScript;       // 0x8, event script run when the object is used
} UnitWinObj;             // size 0xC

typedef struct UnitWinWork {
	u16 flags;                  // 0x0
	u16 pad_02;                 // 0x2
	UnitWinObj* objList;        // 0x4
	s32* items;                 // 0x8, item id per object (0 = none)
	u16 taken[6];               // 0xC, per object: bit 0 set once the object is done
	s32 count;                  // 0x18
	EventEntry* mainEvt;        // 0x1C
	EventEntry* useEvt;         // 0x20
	s32 index;                  // 0x24
	s32 hideTimer;              // 0x28, frames until the box closes
	s32 dispCoin;               // 0x2C, coin count shown (counts toward the pouch value)
	s32 dispNum;                // 0x30, shop points / shine sprites shown
	f32 cursorRow;              // 0x34, row under the cursor (compared with the list index)
	u8 pad_0x38[0x4];           // 0x38
	f32 cursorX;                // 0x3C
	f32 cursorY;                // 0x40
	f32 listY;                  // 0x44
	u8 pad_0x48[0x4];           // 0x48
	s32 excludeId;              // 0x4C, entry left out of the list (partner id, or stat row)
	s32 mode;                   // 0x50, which list the window shows
	u16 freeState;              // 0x54, bit 0 requests free, bit 1 is set once the event has run
	u8 pad_0x56[2];             // 0x56
	s32 rowCount;               // 0x58
	s32 coinTick;               // 0x5C, animation counter for the coin count (mod 1200)
	s32 winId[3];               // 0x60, winMgr entries
	u8 pad_0x6C[0x74];          // 0x6C
	s32 selObj;                 // 0xE0, selected object index (-1 = none)
	s32 useTaken;               // 0xE4, set by the script when the use was accepted
} UnitWinWork;

USER_FUNC(evt_unitwin_main_func);
USER_FUNC(evt_unitwin_select_func);
void evt_unitwin_disp_func(CameraId cameraId, void* param);
void unitwin_point_disp(WinMgrEntry* entry);
void unitwin_help_disp(WinMgrEntry* entry);
void unitwin_help_main(WinMgrEntry* entry);
void unitwin_selltable_disp(WinMgrEntry* entry);

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
USER_FUNC(unitwin_set_taken);
USER_FUNC(unitwin_lottery_disp_onoff);
