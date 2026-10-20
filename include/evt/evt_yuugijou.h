#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>

typedef struct PiantaParlorWork {
	u32 flags; //0x00 bit 0: count display on, bit 1: medals taken out, bit 2: yup flag, bit 3: window ended
	u32 keptTokens; //0x04
	s32 tokens; //0x08
	u32 dispTimer; //0x0C
	u32 dispTokens; //0x10
	u32 rankUsed[4][6]; //0x14 [mode][slot]
	u32 rankScore[4][6]; //0x74 [mode][slot]
	u32 rankMonte[4][6]; //0xD4 [mode][slot]
	u32 field_0x134; //0x134
} PiantaParlorWork;

void monteCountDisp(void);

extern PiantaParlorWork* yuwp;

void yuugijou_init(void);

struct EventEntry* evtYuuWindow(void);
BOOL evtYuuWindowEndChk(void);

USER_FUNC(yuugijou_add_montemedal);
USER_FUNC(yuugijou_add_montemedal_moto);
USER_FUNC(yuugijou_get_montemedal);
USER_FUNC(yuugijou_get_montemedal_keep);
USER_FUNC(yuugijou_get_takeoutflag);
USER_FUNC(yuugijou_montemedal_disp_onoff);
USER_FUNC(yuugijou_montemedal_keep);
USER_FUNC(yuugijou_montemedal_return);
USER_FUNC(yuugijou_montemedal_wait);
USER_FUNC(yuugijou_set_yupflag);
USER_FUNC(yuuminigame_end);
USER_FUNC(yuuminigame_window_end);
USER_FUNC(evt_yuu_get_window_evt);
void yuuSetWindowEvt(s32 value);
