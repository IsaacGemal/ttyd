#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>

typedef struct lottery_info {
	u16 mFlags; //0x0
	u8 field_0x2[0x40 - 0x2]; //0x2
} lottery_info;

lottery_info* lotteryGetPtr(void);
USER_FUNC(evt_lottery);

USER_FUNC(evt_lottery_buy);
USER_FUNC(evt_lottery_error_check);
USER_FUNC(evt_lottery_flag);
USER_FUNC(evt_lottery_todaynum);
