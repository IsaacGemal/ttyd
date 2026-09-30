#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_gauge_timing2.h"
#include "system.h"

extern const f32 float_neg300_80421b38;
extern const f32 float_30_80421b3c;

static void actionCommandDisp(f32 x, f32 y);

s32 battleAcResult_GaugeTiming2(BattleWork* work) {
	return work->actionCommands.result;
}

// NONMATCHING (98.86%): under this file's current -O4,p, MWCC folds the disp pointer into work
// (`mr r31, r4`, then 0x1f34(r31)..0x1f40(r31)); the original materialises it (`addi r31, r4, 0x1f20`).
// Building this file with -O2,p instead makes the function 100% (verified); the source below is the natural one.
void battleAcDisp_GaugeTiming2(CameraId camId, void* param) {
	BattleACDispParams* disp = &((BattleWork*)param)->actionCommands.dispParams;

	switch (((BattleWork*)param)->actionCommands.mAcState) {
	case 0x63:
		disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_80421b38, float_30_80421b3c);
		// fallthrough
	case 0x64:
	case 0x3E8:
	case 0x3EA:
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter > 0) {
			disp->counter--;
		}
		break;
	case 0x3EB:
	case 0x3EC:
		if (disp->counter >= 0x28) {
			disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_80421b3c, float_neg300_80421b38);
		}
		else {
			disp->iconX = float_30_80421b3c;
		}
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter < 0x3C) {
			disp->counter++;
		}
		break;
	}
}

void battleAcDelete_GaugeTiming2(BattleWork* work) {
	if ((work->actionCommands.field_0x30 & 1) == 0) {
		work->actionCommands.mAcState = 0x3EB;
	}
}
