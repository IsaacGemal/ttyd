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

// NONMATCHING (99.93%): 8 register-allocation mismatches, no logical differences.
// The zero-prize-time store keeps the constant in r4/base in r3 (mine r3/r4); the four
// closing calendar calls keep info18's low word in r24 (mine only the first two).
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
		if (*(OSTime*)((u8*)info + 0x18) < start) {
			OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18), &ct[8]);
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
		u8* p = (u8*)info + arg1 * 8;

		if (*(OSTime*)(p + 0x30) < start) {
			OSTicksToCalendarTime(*(OSTime*)(p + 0x30), &ct[6]);
			OSTicksToCalendarTime(start, &ct[5]);
			if (ct[6].year == ct[5].year && ct[6].mon == ct[5].mon && ct[6].mday == ct[5].mday) {
				info->mFlags &= ~0x10;
			} else {
				if (!(info->mFlags & 8)) {
					info->mFlags |= 0x10;
				}
				*(OSTime*)(p + 0x30) = start;
			}
		} else {
			OSTicksToCalendarTime(*(OSTime*)(p + 0x30), &ct[4]);
			OSTicksToCalendarTime(start, &ct[3]);
			if (ct[4].year == ct[3].year && ct[4].mon == ct[3].mon && ct[4].mday == ct[3].mday) {
				info->mFlags &= ~0x10;
			} else {
				info->mFlags |= 0x10;
				*(OSTime*)(p + 0x30) = start;
			}
			info->mFlags |= 4;
		}
	} else {
#ifdef MUST_MATCH
		// Match only: splitting the 64-bit zero into two 32-bit stores keeps the
		// address base as info+arg1*8, as the original does.
		s32* q = (s32*)((u8*)info + arg1 * 8);

		q[13] = 0;
		q[12] = 0;
#else
		*(OSTime*)((u8*)info + arg1 * 8 + 0x30) = 0;
#endif
	}

	if (!(info->mFlags & 1)) {
		info->mFlags |= 1;
		*(OSTime*)((u8*)info + 0x8) = start;
		*(s16*)((u8*)info + 0x10) = rand() % 10000;
	} else if (*(OSTime*)((u8*)info + 0x8) < start) {
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x8), &ct[2]);
		OSTicksToCalendarTime(start, &ct[1]);
		if (ct[2].year != ct[1].year || ct[2].mon != ct[1].mon || ct[2].mday != ct[1].mday) {
			if (arg1 == 1) {
				*(OSTime*)((u8*)info + 0x8) = start;
			}
			if (!(info->mFlags & 2)) {
				*(s16*)((u8*)info + 0x10) = rand() % 10000;
			} else if (start >= *(OSTime*)((u8*)info + 0x18)) {
				s32 limit;
				s32 days;

				days = (s32)((start - *(OSTime*)((u8*)info + 0x18)) / OS_TIMER_CLOCK / 60 / 60 / 24);

				limit = *(s16*)((u8*)info + 0x22);
				if (days > limit) {
					while (days > limit) {
						limit += 0x18b - rand() % 60;
					}
					*(s16*)((u8*)info + 0x22) = limit - days;
				}

				limit = *(s16*)((u8*)info + 0x24);
				if (days > limit) {
					while (days > limit) {
						limit += 0x73 - rand() % 30;
					}
					*(s16*)((u8*)info + 0x24) = limit - days;
				}

				limit = *(s16*)((u8*)info + 0x26);
				if (days > limit) {
					while (days > limit) {
						limit += 0x23 - rand() % 10;
					}
					*(s16*)((u8*)info + 0x26) = limit - days;
				}

				limit = *(s16*)((u8*)info + 0x28);
				if (days > limit) {
					while (days > limit) {
						limit += 0xa - rand() % 6;
					}
					*(s16*)((u8*)info + 0x28) = limit - days;
				}

				if (days == *(s16*)((u8*)info + 0x22)) {
					*(s16*)((u8*)info + 0x10) = *(s16*)((u8*)info + 0x20);
					info->mFlags |= 0x1000;
				} else if (days == *(s16*)((u8*)info + 0x24)) {
					do {
						*(s16*)((u8*)info + 0x10) = (rand() % 10) * 1000 + *(s16*)((u8*)info + 0x20) % 1000;
					} while (*(s16*)((u8*)info + 0x10) == *(s16*)((u8*)info + 0x20));
					info->mFlags |= 0x2000;
				} else if (days == *(s16*)((u8*)info + 0x26)) {
					do {
						*(s16*)((u8*)info + 0x10) = (rand() % 100) * 100 + *(s16*)((u8*)info + 0x20) % 100;
					} while (*(s16*)((u8*)info + 0x10) == *(s16*)((u8*)info + 0x20)
						|| *(s16*)((u8*)info + 0x10) % 1000 == *(s16*)((u8*)info + 0x20) % 1000);
					info->mFlags |= 0x4000;
				} else if (days == *(s16*)((u8*)info + 0x28)) {
					do {
						*(s16*)((u8*)info + 0x10) = (rand() % 1000) * 10 + *(s16*)((u8*)info + 0x20) % 10;
					} while (*(s16*)((u8*)info + 0x10) == *(s16*)((u8*)info + 0x20)
						|| *(s16*)((u8*)info + 0x10) % 1000 == *(s16*)((u8*)info + 0x20) % 1000
						|| *(s16*)((u8*)info + 0x10) % 100 == *(s16*)((u8*)info + 0x20) % 100);
					info->mFlags |= 0x8000;
				} else {
					do {
						*(s16*)((u8*)info + 0x10) = rand() % 10000;
#ifdef MUST_MATCH
					// Match only: the original evaluates the mod clauses right-to-left.
					} while (*(s16*)((u8*)info + 0x10) == *(s16*)((u8*)info + 0x20)
						|| *(s16*)((u8*)info + 0x20) % 1000 == *(s16*)((u8*)info + 0x10) % 1000
						|| *(s16*)((u8*)info + 0x20) % 100 == *(s16*)((u8*)info + 0x10) % 100
						|| *(s16*)((u8*)info + 0x20) % 10 == *(s16*)((u8*)info + 0x10) % 10);
#else
					} while (*(s16*)((u8*)info + 0x10) == *(s16*)((u8*)info + 0x20)
						|| *(s16*)((u8*)info + 0x10) % 1000 == *(s16*)((u8*)info + 0x20) % 1000
						|| *(s16*)((u8*)info + 0x10) % 100 == *(s16*)((u8*)info + 0x20) % 100
						|| *(s16*)((u8*)info + 0x10) % 10 == *(s16*)((u8*)info + 0x20) % 10);
#endif
				}
			} else {
				info->mFlags |= 4;
				*(s16*)((u8*)info + 0x10) = -1;
			}
		}
	} else {
		info->mFlags |= 4;
		*(s16*)((u8*)info + 0x10) = -1;
		*(OSTime*)((u8*)info + 0x8) = start;
	}

	evtSetValue(event, args[1], info->mFlags);
	evtSetValue(event, args[2], *(s16*)((u8*)info + 0x10));
	OSTicksToCalendarTime(start, &ct[0]);
	if (info->mFlags & 2) {
#ifdef MUST_MATCH
		// Match only: re-reading info18 through a local variable makes the allocator
		// keep its low word in r24, as the original does.
		OSTime t;

		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18), &ct[0]);
		t = *(OSTime*)((u8*)info + 0x18);
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x22), &ct[0]);
		t = *(OSTime*)((u8*)info + 0x18);
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x24), &ct[0]);
		t = *(OSTime*)((u8*)info + 0x18);
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x26), &ct[0]);
		t = *(OSTime*)((u8*)info + 0x18);
		OSTicksToCalendarTime(t + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x28), &ct[0]);
#else
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18), &ct[0]);
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18) + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x22), &ct[0]);
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18) + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x24), &ct[0]);
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18) + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x26), &ct[0]);
		OSTicksToCalendarTime(*(OSTime*)((u8*)info + 0x18) + OSSecondsToTicks((OSTime)86400) * *(s16*)((u8*)info + 0x28), &ct[0]);
#endif
	}
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_lottery_buy) {
	s32* args = event->args;
	OSTime time = OSGetTime();
	lottery_info* info = &gp->mLotteryInfo;

	do {
		*(s16*)((u8*)info + 0x20) = rand() % 10000;
	} while (*(s16*)((u8*)info + 0x20) == *(s16*)((u8*)info + 0x10)
		|| *(s16*)((u8*)info + 0x20) % 1000 == *(s16*)((u8*)info + 0x10) % 1000
		|| *(s16*)((u8*)info + 0x20) % 100 == *(s16*)((u8*)info + 0x10) % 100
		|| *(s16*)((u8*)info + 0x20) % 10 == *(s16*)((u8*)info + 0x10) % 10);

	*(OSTime*)((u8*)info + 0x18) = time;
	*(s16*)((u8*)info + 0x22) = 0x18b - rand() % 60;
	*(s16*)((u8*)info + 0x24) = 0x73 - rand() % 30;
	*(s16*)((u8*)info + 0x26) = 0x23 - rand() % 10;
	*(s16*)((u8*)info + 0x28) = 0xa - rand() % 6;

	if (info->mFlags & 4) {
		info->mFlags |= 8;
		*(OSTime*)((u8*)info + 0x08) = time;
		*(OSTime*)((u8*)info + 0x30) = time;
		*(OSTime*)((u8*)info + 0x38) = time;
	}
	if (info->mFlags & 2) {
		info->mFlags |= 8;
		*(OSTime*)((u8*)info + 0x08) = time;
		*(OSTime*)((u8*)info + 0x30) = time;
		*(OSTime*)((u8*)info + 0x38) = time;
	}
	info->mFlags |= 2;
	evtSetValue(event, args[0], *(s16*)((u8*)info + 0x20));
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
