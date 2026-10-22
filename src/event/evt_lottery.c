#include "evt/evt_lottery.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"
#include <dolphin/os.h>
#include <stdlib.h>
#include "manager/evtmgr_cmd.h"

extern GlobalWork* gp;

//.sbss
BOOL dbg_lotteryinfo;

//local prototypes
lottery_info* lotteryGetPtr(void) {
	return &gp->mLotteryInfo;
}

USER_FUNC(evt_lottery) {
	lottery_info* info;
	s32* args = event->args;
	s32 arg1 = evtGetValue(event, args[0]);
	OSTime start = OSGetTime();
	OSCalendarTime ct[9];

	dbg_lotteryinfo = TRUE;

	info = &gp->mLotteryInfo;

	info->mFlags &= ~0x1000;
	info->mFlags &= ~0x2000;
	info->mFlags &= ~0x4000;
	info->mFlags &= ~0x8000;

	if (info->mFlags & 8) {
		if (info->mBuyTime < start) {
			OSTicksToCalendarTime(info->mBuyTime, &ct[8]);
			OSTicksToCalendarTime(start, &ct[7]);
			if (ct[8].year != ct[7].year || ct[8].mon != ct[7].mon || ct[8].mday != ct[7].mday) {
				info->mFlags &= ~4;
				info->mFlags &= ~8;
			}
		} else {
			info->mFlags |= 4;
			info->mFlags &= ~8;
		}
	}

	if (info->mFlags & 2) {
		if (info->mSlotTime[arg1] < start) {
			OSTicksToCalendarTime(info->mSlotTime[arg1], &ct[6]);
			OSTicksToCalendarTime(start, &ct[5]);
			if (ct[6].year == ct[5].year && ct[6].mon == ct[5].mon && ct[6].mday == ct[5].mday) {
				info->mFlags &= ~0x10;
			} else {
				if (!(info->mFlags & 8)) {
					info->mFlags |= 0x10;
				}
				info->mSlotTime[arg1] = start;
			}
		} else {
			OSTicksToCalendarTime(info->mSlotTime[arg1], &ct[4]);
			OSTicksToCalendarTime(start, &ct[3]);
			if (ct[4].year == ct[3].year && ct[4].mon == ct[3].mon && ct[4].mday == ct[3].mday) {
				info->mFlags &= ~0x10;
			} else {
				info->mFlags |= 0x10;
				info->mSlotTime[arg1] = start;
			}
			info->mFlags |= 4;
		}
	} else {
		info->mSlotTime[arg1] = 0;
	}

	if (!(info->mFlags & 1)) {
		info->mFlags |= 1;
		info->mDrawTime = start;
		info->mTodayNum = rand() % 10000;
	} else if (info->mDrawTime < start) {
		OSTicksToCalendarTime(info->mDrawTime, &ct[2]);
		OSTicksToCalendarTime(start, &ct[1]);
		if (ct[2].year != ct[1].year || ct[2].mon != ct[1].mon || ct[2].mday != ct[1].mday) {
			if (arg1 == 1) {
				info->mDrawTime = start;
			}
			if (!(info->mFlags & 2)) {
				info->mTodayNum = rand() % 10000;
			} else if (start >= info->mBuyTime) {
				s32 limit;
				s32 days;

				days = (s32)((start - info->mBuyTime) / OS_TIMER_CLOCK / 60 / 60 / 24);

				limit = info->mTierDays[0];
				if (days > limit) {
					while (days > limit) {
						limit += 0x18b - rand() % 60;
					}
					info->mTierDays[0] = limit - days;
				}

				limit = info->mTierDays[1];
				if (days > limit) {
					while (days > limit) {
						limit += 0x73 - rand() % 30;
					}
					info->mTierDays[1] = limit - days;
				}

				limit = info->mTierDays[2];
				if (days > limit) {
					while (days > limit) {
						limit += 0x23 - rand() % 10;
					}
					info->mTierDays[2] = limit - days;
				}

				limit = info->mTierDays[3];
				if (days > limit) {
					while (days > limit) {
						limit += 0xa - rand() % 6;
					}
					info->mTierDays[3] = limit - days;
				}

				if (days == info->mTierDays[0]) {
					info->mTodayNum = info->mTicketNum;
					info->mFlags |= 0x1000;
				} else if (days == info->mTierDays[1]) {
					do {
						info->mTodayNum = (rand() % 10) * 1000 + info->mTicketNum % 1000;
					} while (info->mTodayNum == info->mTicketNum);
					info->mFlags |= 0x2000;
				} else if (days == info->mTierDays[2]) {
					do {
						info->mTodayNum = (rand() % 100) * 100 + info->mTicketNum % 100;
					} while (info->mTodayNum == info->mTicketNum
						|| info->mTodayNum % 1000 == info->mTicketNum % 1000);
					info->mFlags |= 0x4000;
				} else if (days == info->mTierDays[3]) {
					do {
						info->mTodayNum = (rand() % 1000) * 10 + info->mTicketNum % 10;
					} while (info->mTodayNum == info->mTicketNum
						|| info->mTodayNum % 1000 == info->mTicketNum % 1000
						|| info->mTodayNum % 100 == info->mTicketNum % 100);
					info->mFlags |= 0x8000;
				} else {
					do {
						info->mTodayNum = rand() % 10000;
					} while (info->mTodayNum == info->mTicketNum
						|| info->mTicketNum % 1000 == info->mTodayNum % 1000
						|| info->mTicketNum % 100 == info->mTodayNum % 100
						|| info->mTicketNum % 10 == info->mTodayNum % 10);
				}
			} else {
				info->mFlags |= 4;
				info->mTodayNum = -1;
			}
		}
	} else {
		info->mFlags |= 4;
		info->mTodayNum = -1;
		info->mDrawTime = start;
	}

	evtSetValue(event, args[1], info->mFlags);
	evtSetValue(event, args[2], info->mTodayNum);
	OSTicksToCalendarTime(start, &ct[0]);
	if (info->mFlags & 2) {
#ifdef MUST_MATCH
		// Match only: re-reading mBuyTime through a local variable makes the allocator
		// keep its low word in r24, as the original does.
		OSTime t;

		OSTicksToCalendarTime(info->mBuyTime, &ct[0]);
		t = info->mBuyTime;
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * info->mTierDays[0], &ct[0]);
		t = info->mBuyTime;
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * info->mTierDays[1], &ct[0]);
		t = info->mBuyTime;
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * info->mTierDays[2], &ct[0]);
		t = info->mBuyTime;
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * info->mTierDays[3], &ct[0]);
#else
		OSTicksToCalendarTime(info->mBuyTime, &ct[0]);
		OSTicksToCalendarTime(info->mBuyTime + OSSecondsToTicks((OSTime)86400) * info->mTierDays[0], &ct[0]);
		OSTicksToCalendarTime(info->mBuyTime + OSSecondsToTicks((OSTime)86400) * info->mTierDays[1], &ct[0]);
		OSTicksToCalendarTime(info->mBuyTime + OSSecondsToTicks((OSTime)86400) * info->mTierDays[2], &ct[0]);
		OSTicksToCalendarTime(info->mBuyTime + OSSecondsToTicks((OSTime)86400) * info->mTierDays[3], &ct[0]);
#endif
	}
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_lottery_buy) {
	s32* args = event->args;
	OSTime time = OSGetTime();
	lottery_info* info = &gp->mLotteryInfo;

	do {
		info->mTicketNum = rand() % 10000;
	} while (info->mTicketNum == info->mTodayNum
		|| info->mTicketNum % 1000 == info->mTodayNum % 1000
		|| info->mTicketNum % 100 == info->mTodayNum % 100
		|| info->mTicketNum % 10 == info->mTodayNum % 10);

	info->mBuyTime = time;
	info->mTierDays[0] = 0x18b - rand() % 60;
	info->mTierDays[1] = 0x73 - rand() % 30;
	info->mTierDays[2] = 0x23 - rand() % 10;
	info->mTierDays[3] = 0xa - rand() % 6;

	if (info->mFlags & 4) {
		info->mFlags |= 8;
		info->mDrawTime = time;
		info->mSlotTime[0] = time;
		info->mSlotTime[1] = time;
	}
	if (info->mFlags & 2) {
		info->mFlags |= 8;
		info->mDrawTime = time;
		info->mSlotTime[0] = time;
		info->mSlotTime[1] = time;
	}
	info->mFlags |= 2;
	evtSetValue(event, args[0], info->mTicketNum);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_lottery_flag) {
	s32* args = event->args;

	evtSetValue(event, args[0], gp->mLotteryInfo.mFlags);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_lottery_todaynum) {
	s32* args = event->args;

	evtSetValue(event, args[0], gp->mLotteryInfo.mTodayNum);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_lottery_error_check) {
	s32* args = event->args;
	lottery_info* info = &gp->mLotteryInfo;
	OSTime time = OSGetTime();
	u16 flags = info->mFlags;
	BOOL error = FALSE;

	if (flags & 4) {
		error = TRUE;
	}
	if ((flags & 2) && (time < info->mBuyTime)) {
		error = TRUE;
	}
	if (time < info->mSlotTime[0]) {
		error = TRUE;
	}
	if (time < info->mSlotTime[1]) {
		error = TRUE;
	}
	if (time < info->mDrawTime) {
		error = TRUE;
	}
	if (flags & 8) {
		if (time > info->mBuyTime) {
			OSCalendarTime ct1;
			OSCalendarTime ct2;

			OSTicksToCalendarTime(info->mBuyTime, &ct1);
			OSTicksToCalendarTime(time, &ct2);
			if (ct1.year != ct2.year || ct1.mon != ct2.mon || ct1.mday != ct2.mday) {
				error = FALSE;
			}
		}
	}
	evtSetValue(event, args[0], error);
	return EVT_RETURN_DONE;
}
