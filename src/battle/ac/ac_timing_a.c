#include "battle/ac/ac_timing_a.h"

// TimingA's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcTimingADecWork {
	s32 field_0x00; // 0x0
	u32 flags;      // 0x4
	s32 field_0x08; // 0x8
} AcTimingADecWork;

// A timing window: the frame threshold and the success frame it maps to.
typedef struct AcTimingAEntry {
	s32 threshold; // 0x0
	s32 value;     // 0x4
} AcTimingAEntry;

// One window table per difficulty level (indexed by BattleActionCommandGetDifficulty).
extern AcTimingAEntry* _ac_timing_a_timing_tbl[7];

// NONMATCHING (1.54%): 64 missing.
s32 battleAcMain_TimingA(BattleWork* work) {
	switch (work->actionCommands.mAcState) { //decimal values
		case 0:
			break;
	}
}

// optimize_for_size: ac_timing_a is built with -O4,s (its other functions use stmw/lmw prologues);
// without it this loop is rotated and the original's is not.
#pragma optimize_for_size on
// NONMATCHING (88.39%): the header's s8 return makes MWCC emit an extra `extsb r3, r3` before the table index (the original is plain `slwi`; the definition is u8 `lbz; blr`, so this caller used full width), which also reorders the next two loads.
s32 battleAcTimingA_GetSuccessFrame(BattleWork* work) {
	AcTimingADecWork* dec = (AcTimingADecWork*)((u8*)work + 0x1F4C);
	s32 difficulty = BattleActionCommandGetDifficulty(work);
	AcTimingAEntry* entry = _ac_timing_a_timing_tbl[difficulty];
	s32 frame = work->actionCommands.mResultCount;
	s32 result = entry->value;

	if (dec->flags & 2) {
		if (frame >= 100000) {
			result = 0;
		} else {
#ifdef MUST_MATCH
			// The original's -O4,s loop is not rotated; a plain `while` rotates it here.
			for (;;) {
				if (frame <= entry->threshold) {
					break;
				}
				entry++;
			}
#else
			while (frame > entry->threshold) {
				entry++;
			}
#endif
			result = entry->value;
		}
	}
	return result;
}
#pragma optimize_for_size reset

void battleAcDelete_TimingA(BattleWork* work) {
	work->actionCommands.mAcState = 1000;
}

