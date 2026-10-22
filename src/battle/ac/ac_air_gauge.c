#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_table.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>
#include "battle/ac/ac_air_gauge.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803ABCE0[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };


// Match-only: dat_80420a70 must be non-const (but still in .sdata2) so MWCC loads it at its use
// instead of folding -1 into `li r5, -1` / hoisting a const load above the disp stores; the original
// emits `lwz r5, dat_80420a70` late in battleAcMain_AirGauge's case 0. The natural form is under #else.
#ifdef MUST_MATCH
__declspec(section ".sdata2") s32 dat_80420a70 = -1;
#else
const s32 dat_80420a70 = -1;
#endif


static void actionCommandDisp(f32, f32);

// The runtime work area of the gauge, at BattleWork+0x1F4C.
typedef struct AirGaugeDecWork {
	s32 frame;        // 0x0
	f32 gaugePercent; // 0x4
	u32 presses;      // 0x8, button presses; 0 at the end means the gauge was never pressed (no result)
	u32 heldFrames;   // 0xC, frames the button has been held since the last press
	s32 flag;         // 0x10
	s32 endTimer;     // 0x14
} AirGaugeDecWork;

// 100%. The `difficulty`/`dat_80420a70` shapes are match-only (comments at their uses);
// the natural forms are under #else.
s32 battleAcMain_AirGauge(BattleWork* work) {
	// Addressing the gauge work through the header's named field keeps `dec` in its own
	// base register (r31) instead of folding it into the BattleWork base. See ac_monosiri_target.c.
	BattleActions* ac = &work->actionCommands;
	AirGaugeDecWork* dec = (AirGaugeDecWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;

	switch (work->actionCommands.mAcState) {
	case 0x63:
		break;
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = -300.0f;
		disp->iconY = 0.0f;
		disp->counter = 0x14;
		disp->value = 0.0f;
		disp->field_0xC = dat_80420a70;
		dec->frame = 0;
		work->actionCommands.outputParams[0] = 0;
		work->actionCommands.outputParams[2] = 0;
		work->actionCommands.outputParams[1] = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		dec->gaugePercent = 0.0f;
		dec->presses = 0;
		dec->heldFrames = 0;
		dec->flag = 0;
		break;
	case 0x64:
		if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
			if (irand(100) >= 0) {
				work->actionCommands.field_0x8 = work->actionCommands.acParams[3] / 2;
				work->actionCommands.outputParams[0] = 0;
				work->actionCommands.outputParams[2] = 0;
			}
			else {
				s32 r = irand(work->actionCommands.acParams[2] - 1) + 1;
				work->actionCommands.outputParams[0] = r;
				switch (r) {
				case 1:
					work->actionCommands.field_0x8 = work->actionCommands.acParams[3] - 1;
					break;
				case 2:
					work->actionCommands.field_0x8 = work->actionCommands.acParams[4] - 1;
					break;
				case 3:
					work->actionCommands.field_0x8 = work->actionCommands.acParams[5] - 1;
					break;
				// The empty case 4 sharing the default body makes MWCC pivot the
				// decision tree on 3 and emit the original's extra `b default`.
				case 4:
				default:
					work->actionCommands.field_0x8 = work->actionCommands.acParams[5] + 1;
					break;
				}
				work->actionCommands.outputParams[2] = 100;
			}
			work->actionCommands.mAcState = 0x3E9;
			break;
		}
		BattleAcGaugeSeInit();
		work->actionCommands.mAcState = 0x3E8;
		// fallthrough
	case 0x3E8:
		dec->frame++;
		if (BattlePadCheckNow(0x100) != 0) {
			work->actionCommands.outputParams[1] = 1;
			if (BattlePadCheckTrigger(0x100)) {
				dec->presses++;
				dec->heldFrames = 0;
			}
			dec->heldFrames++;
		}
		else {
			work->actionCommands.outputParams[1] = 0;
			dec->heldFrames = 0;
		}
		dec->gaugePercent -= _GetGaugeDecValue();
		if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand) {
			dec->presses = 1;
			dec->gaugePercent = 100.0f;
		}
		if (dec->gaugePercent < 0.0f) {
			dec->gaugePercent = 0.0f;
		}
		if (dec->gaugePercent > 100.0f) {
			dec->gaugePercent = 100.0f;
		}
		BattleAcGaugeSeUpdate(dec->gaugePercent);
		if (dec->frame < work->actionCommands.acParams[1]) {
			break;
		}
		if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand) {
			dec->presses = 1;
			dec->gaugePercent = 100.0f;
		}
		if (dec->presses == 0) {
			work->actionCommands.outputParams[0] = 0;
			work->actionCommands.outputParams[2] = 0;
			work->actionCommands.result = 0;
			BattleAcGaugeSeDelete();
			return 0;
		}
		work->actionCommands.result = 2;
		work->actionCommands.mResultCount++;
		{
			// Assigning the difficulty inside the first `else if` (rather than a plain load)
			// makes MWCC keep it in r4 across the chain and load it lazily, as the original does.
			s32 difficulty;
			if (dec->gaugePercent < (f32)work->actionCommands.acParams[3]) {
				work->actionCommands.outputParams[0] = 1;
			}
			else if ((difficulty = work->actionCommands.acParams[2]) <= 2) {
				work->actionCommands.outputParams[0] = 2;
			}
			else if (dec->gaugePercent < (f32)work->actionCommands.acParams[4]) {
				work->actionCommands.outputParams[0] = 2;
			}
			else if (difficulty <= 3) {
				work->actionCommands.outputParams[0] = 3;
			}
			else if (dec->gaugePercent < (f32)work->actionCommands.acParams[5]) {
				work->actionCommands.outputParams[0] = 3;
			}
			else if (difficulty <= 4) {
				work->actionCommands.outputParams[0] = 4;
			}
			else if (dec->gaugePercent < (f32)work->actionCommands.acParams[6]) {
				work->actionCommands.outputParams[0] = 4;
			}
			else {
				work->actionCommands.outputParams[0] = 5;
			}
		}
		work->actionCommands.outputParams[2] = (s32)dec->gaugePercent;
		work->actionCommands.mAcState = 0x3EA;
		break;
	case 0x3E9:
		work->actionCommands.field_0x8 = work->actionCommands.field_0x8 - 1;
		if (work->actionCommands.field_0x8 < 0) {
			if (work->actionCommands.outputParams[0] > 0) {
				work->actionCommands.result = 2;
				work->actionCommands.mResultCount++;
			}
			else {
				work->actionCommands.result = 0;
			}
			return 0;
		}
		break;
	case 0x3EA:
		BattleAcGaugeSeDelete();
		dec->endTimer = 0x3C;
		work->actionCommands.mAcState = 0x3EB;
		// fallthrough
	case 0x3EB:
		dec->endTimer = dec->endTimer - 1;
		if (dec->endTimer <= 0) {
			work->actionCommands.mAcState = 0x3EC;
		}
		break;
	case 0x3EC:
		work->actionCommands.maincb = NULL;
		work->actionCommands.dispcb = NULL;
		work->actionCommands.resultcb = NULL;
		work->actionCommands.deletecb = NULL;
		return 0;
	}
	return 1;
}

// Table record used by _GetGaugeDecValue; laid out in the original as 5 records of 0xC bytes.
typedef struct GaugeTableEntry {
	s32 threshold; // 0x0
	f32 value;     // 0x4
	s32 unk8;      // 0x8
} GaugeTableEntry;

extern GaugeTableEntry _table_0[5];
extern GaugeTableEntry _table_1[5];
extern GaugeTableEntry _table_2[5];
extern GaugeTableEntry _table_3[5];
extern GaugeTableEntry _table_4[5];
extern GaugeTableEntry _table_5[5];
extern GaugeTableEntry _table_6[5];


// The work area is reached through BattleActions.field_0x2BC so MWCC materialises it in its own register.
f32 _GetGaugeDecValue(void) {
	// `ac` + field_0x2BC keeps the work area in its own base register (r6) instead of
	// folding it into the BattleWork base (see ac_monosiri_target.c).
	BattleActions* ac = &_battleWorkPointer->actionCommands;
	AirGaugeDecWork* dec = (AirGaugeDecWork*)&ac->field_0x2BC[0];
	s32 type;
	GaugeTableEntry* table;
	f32 max;

	type = ac->acParams[7]; // BattleWork+0x1ce4
	switch (type) {
	case 0:
		max = 0.3f;
		table = _table_0;
		break;
	case 1:
		max = 0.33f;
		table = _table_1;
		break;
	case 2:
		max = 0.36f;
		table = _table_2;
		break;
	case 3:
		max = 0.4f;
		table = _table_3;
		break;
	case 4:
		max = 0.44f;
		table = _table_4;
		break;
	case 5:
		max = 0.47f;
		table = _table_5;
		break;
	case 6:
	default:
		// table before max: with max first MWCC emits `lfs` before `addi` here, unlike every other case.
		table = _table_6;
		max = 0.5f;
		break;
	}

	while (dec->heldFrames > (u32)table->threshold) {
		table++;
		if (table->threshold < 0) {
			break;
		}
	}

	if (table->threshold <= 0) {
		dec->flag = 1;
		return max;
	}

	dec->flag = table->unk8;
	max = max - intplGetValue(0, dec->heldFrames - (table - 1)->threshold, table->threshold - (table - 1)->threshold,
	                          (table - 1)->value, table->value);
	return max;
}

s32 battleAcResult_AirGauge(BattleWork* work) {
	return work->actionCommands.result;
}

// Deriving both the state and disp parameters from one BattleActions* makes MWCC materialise the
// disp base: `addi r31, r4, 0x1f20`, then 0x14(r31).
void battleAcDisp_AirGauge(CameraId camId, void* param) {
	BattleActions* ac = &((BattleWork*)param)->actionCommands;
	s32 state = ac->mAcState;
	BattleACDispParams* ag = &ac->dispParams;

	switch (state) {
	case 0x63:
		ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, -300.0f, 30.0f);
		// fallthrough
	case 0x64:
	case 0x3E8:
		actionCommandDisp(ag->iconX, ag->iconY);
		if (ag->counter > 0) {
			ag->counter--;
		}
		break;
	case 0x3EA:
	case 0x3EB:
		if (ag->counter >= 0x28) {
			ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, 30.0f, -300.0f);
		}
		else {
			ag->iconX = 30.0f;
		}
		actionCommandDisp(ag->iconX, ag->iconY);
		if (ag->counter < 0x3C) {
			ag->counter++;
		}
		break;
	}
}

void battleAcDelete_AirGauge(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EA;
}

static void actionCommandDisp(f32 f1, f32 f2) {
	BattleWork* work = _battleWorkPointer;
	// Deriving disp before camGetPtr and dec after makes MWCC materialise them in their
	// own registers (work=r31, disp=r30, dec=r29) instead of folding them into the BattleWork base.
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	AirGaugeDecWork* dec;
	f32 value;
	f32 target;

	camGetPtr(CAMERA_2D);
	dec = (AirGaugeDecWork*)&ac->field_0x2BC[0];

	target = dec->gaugePercent;
	target = target / 100.0f;
	value = disp->value;
	if (value <= target) {
		// A named step keeps `value + step` as f1 + f0; the plain literal lets MWCC fold
		// the operands the other way (`fadds f0, f0, f1`).
		f32 step = 0.01f;
		disp->value = value + step;
		if (disp->value > target) {
			disp->value = target;
		}
	}
	else {
		disp->value = value - 0.01f;
		if (disp->value < target) {
			disp->value = target;
		}
	}

	BattleAcDrawGauge((s32)f1, (s32)f2, disp->value, 0xb2, work->actionCommands.acParams[2],
	                  work->actionCommands.acParams[3], work->actionCommands.acParams[4],
	                  work->actionCommands.acParams[5], 0);
	if (dec->flag != 0) {
		Vec pos;
		pos = (Vec){0.0f, 0.0f, 0.0f};
		pos.x = -200.0f + f1;
		pos.y = 58.0f + f2;
		iconDispGx(1.0f, &pos, 0x10, 0x65);
	}
	else {
		Vec pos;
		pos = (Vec){0.0f, 0.0f, 0.0f};
		pos.x = -200.0f + f1;
		pos.y = 58.0f + f2;
		iconDispGx(1.0f, &pos, 0x10, 0x64);
	}
}

// .data: float pairs and gauge tables. _GetGaugeDecValue addresses the gauge tables off this section's base
// (lbl_803ABCE0 + 0x40...), so this block's layout (7 pairs, then 7 tables of 5 records) is load-bearing.
// -sdata 48 would move these 8-byte objects to .sdata; the original has them in .data.
// force_active: nothing reads the float pairs, so the linker would drop them.
#pragma force_active on
__declspec(section ".data") f32 negone_one_374[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803ABCF0[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 one_negone_373[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 lbl_803ABD00[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 negone_one_360[2] = {-1.0f, 1.0f};
__declspec(section ".data") f32 lbl_803ABD10[2] = {1.0f, -1.0f};
__declspec(section ".data") f32 one_negone_359[2] = {1.0f, -1.0f};

GaugeTableEntry _table_0[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_1[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_2[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_3[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_4[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_5[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
GaugeTableEntry _table_6[5] = {
	{0, 0.0f, 1}, {0x14, 1.0f, 1}, {0x28, 1.0f, 1}, {0x50, 0.0f, 0}, {-1, -1.0f, 0},
};
#pragma force_active reset
