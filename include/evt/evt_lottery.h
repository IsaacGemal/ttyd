#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/os.h>
#include <dolphin/types.h>

typedef struct lottery_info {
	u16 mFlags; //0x0
	u8 field_0x2[0x20 - 0x2]; //0x2, offsets 0x8/0x10/0x18 are still read raw; winmgr.c and evt_win.c read 0x20 through here
	s16 mTicketNum; //0x20, the number the player bought
	s16 mTierDays[4]; //0x22, day offset of each match tier (0 all digits, 1 last three, 2 last two, 3 last one)
	u8 pad_0x2A[0x30 - 0x2A];
	OSTime mSlotTime[2]; //0x30, stamped per draw slot (evt_lottery's arg1 is 0 or 1)
} lottery_info;

lottery_info* lotteryGetPtr(void);
USER_FUNC(evt_lottery);

USER_FUNC(evt_lottery_buy);
USER_FUNC(evt_lottery_error_check);
USER_FUNC(evt_lottery_flag);
USER_FUNC(evt_lottery_todaynum);
