#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/os.h>
#include <dolphin/types.h>

typedef struct lottery_info {
	u16 mFlags; //0x0, see below
	u8 pad_0x2[0x8 - 0x2];
	OSTime mDrawTime; //0x8, day of the last daily draw; the number rerolls once the calendar day changes
	s16 mTodayNum; //0x10, today's winning number, -1 while the clock is suspect
	u8 pad_0x12[0x18 - 0x12];
	OSTime mBuyTime; //0x18, when the ticket was bought; mTierDays count days from here
	s16 mTicketNum; //0x20, the number the player bought
	s16 mTierDays[4]; //0x22, day offset of each match tier (0 all digits, 1 last three, 2 last two, 3 last one)
	u8 pad_0x2A[0x30 - 0x2A];
	OSTime mSlotTime[2]; //0x30, stamped per draw slot (evt_lottery's arg1 is 0 or 1)
} lottery_info;

// mFlags: 0x1 initialised, 0x2 holding a ticket, 0x4 clock went backwards, 0x8 bought again today
// (no draw until the next day), 0x10 a new day for this slot; 0x1000/0x2000/0x4000/0x8000 won
// tier 0..3 today (cleared at the start of every evt_lottery).

lottery_info* lotteryGetPtr(void);
USER_FUNC(evt_lottery);

USER_FUNC(evt_lottery_buy);
USER_FUNC(evt_lottery_error_check);
USER_FUNC(evt_lottery_flag);
USER_FUNC(evt_lottery_todaynum);
