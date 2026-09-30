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
