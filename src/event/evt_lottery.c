#include "evt/evt_lottery.h"
#include "evt/evt_cmd.h"
#include "mario/mariost.h"
#include <dolphin/os.h>
#include "manager/evtmgr_cmd.h"

extern GlobalWork* gp;

//.sbss
BOOL dbg_lotteryinfo;

//local prototypes

lottery_info* lotteryGetPtr(void) {
	return &gp->mLotteryInfo;
}

// NONMATCHING (3.63%): 3 instructions differ, 643 missing, plus register differences.
USER_FUNC(evt_lottery) {
	lottery_info* LotteryInfo;

	s32* args = event->args;
	s32 arg1 = evtGetValue(event, args[0]); //r25
	OSTime start = OSGetTime();
	dbg_lotteryinfo = TRUE;

	LotteryInfo = &gp->mLotteryInfo;

	LotteryInfo->mFlags &= ~0x1000;
	LotteryInfo->mFlags &= ~0x2000;
	LotteryInfo->mFlags &= ~0x4000;
	LotteryInfo->mFlags &= ~0x8000;

	if (LotteryInfo->mFlags & 8) {

	}
















	return EVT_RETURN_DONE;
}

USER_FUNC(evt_lottery_flag) {
	s32* args = event->args;

	evtSetValue(event, args[0], gp->mLotteryInfo.mFlags);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_lottery_todaynum) {
	s32* args = event->args;

	// offset 0x10 into lottery_info is an s16; the header only has a padding array there
	evtSetValue(event, args[0], *(s16*)((u8*)&gp->mLotteryInfo + 0x10));
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
	if ((flags & 2) && (time < *(OSTime*)((u8*)info + 0x18))) {
		error = TRUE;
	}
	if (time < *(OSTime*)((u8*)info + 0x30)) {
		error = TRUE;
	}
	if (time < *(OSTime*)((u8*)info + 0x38)) {
		error = TRUE;
	}
	if (time < *(OSTime*)((u8*)info + 0x08)) {
		error = TRUE;
	}
	if (flags & 8) {
		if (time > *(OSTime*)((u8*)info + 0x18)) {
			OSCalendarTime ct1;
			OSCalendarTime ct2;

			OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18), &ct1);
			OSTicksToCalendarTime(time, &ct2);
			if (ct1.year != ct2.year || ct1.mon != ct2.mon || ct1.mday != ct2.mday) {
				error = FALSE;
			}
		}
	}
	evtSetValue(event, args[0], error);
	return EVT_RETURN_DONE;
}
