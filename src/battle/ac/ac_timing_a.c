#include "battle/ac/ac_timing_a.h"

// NONMATCHING (1.54%): 64 missing.
s32 battleAcMain_TimingA(BattleWork* work) {
	switch (work->actionCommands.mAcState) { //decimal values
		case 0:
			break;
	}
}

void battleAcDelete_TimingA(BattleWork* work) {
	work->actionCommands.mAcState = 1000;
}

