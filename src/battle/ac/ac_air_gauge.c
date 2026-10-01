#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/ac/ac_table.h"
#include "battle/battle_pad.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>
#include "battle/ac/ac_air_gauge.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803ABCE0[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };


const Vec vec3_802fdc68 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802fdc74 = {0.0f, 0.0f, 0.0f};
const f64 double_to_int_802fdc80 = 4503601774854144.0;

const s32 dat_80420a70 = -1;
const f32 float_100_80420a74 = 100.0f;
const f32 float_0p01_80420a78 = 0.01f;
const f32 float_neg200_80420a7c = -200.0f;
const f32 float_58_80420a80 = 58.0f;
const f32 float_1_80420a84 = 1.0f;
const f32 float_neg300_80420a88 = -300.0f;
const f32 float_30_80420a8c = 30.0f;
const f32 float_0p3_80420a90 = 0.3f;
const f32 float_0p33_80420a94 = 0.33f;
const f32 float_0p36_80420a98 = 0.36f;
const f32 float_0p4_80420a9c = 0.4f;
const f32 float_0p44_80420aa0 = 0.44f;
const f32 float_0p47_80420aa4 = 0.47f;
const f32 float_0p5_80420aa8 = 0.5f;
const f32 float_0_80420aac = 0.0f;


static void actionCommandDisp(f32, f32);

// The runtime work area of the gauge, at BattleWork+0x1F4C.
typedef struct AirGaugeDecWork {
	s32 frame;        // 0x0
	f32 gaugePercent; // 0x4
	u32 field_0x8;    // 0x8
	u32 value;        // 0xC
	s32 flag;         // 0x10
	s32 field_0x14;   // 0x14
} AirGaugeDecWork;

// don't_inline: the original keeps actionCommandDisp as its own local function
#pragma dont_inline on
// NONMATCHING (89.16%): 6 instructions differ, 4 missing, 2 extra, plus register differences.
static void actionCommandDisp(f32 f1, f32 f2) {
	BattleWork* work = _battleWorkPointer;
	BattleACDispParams* disp = &work->actionCommands.dispParams;
	AirGaugeDecWork* dec = (AirGaugeDecWork*)((u8*)work + 0x1f4c);
	f32 target;
	f32 value;

	camGetPtr(CAMERA_2D);

	target = dec->gaugePercent / float_100_80420a74;
	value = disp->value;
	if (value <= target) {
		disp->value = value + float_0p01_80420a78;
		if (disp->value > target) {
			disp->value = target;
		}
	}
	else {
		disp->value = value - float_0p01_80420a78;
		if (disp->value < target) {
			disp->value = target;
		}
	}

	BattleAcDrawGauge((s32)f1, (s32)f2, 0xb2, *(s32*)((u8*)work + 0x1cd0), *(s32*)((u8*)work + 0x1cd4),
	                  *(s32*)((u8*)work + 0x1cd8), *(s32*)((u8*)work + 0x1cdc), 0, disp->value);

	if (dec->flag != 0) {
		Vec pos;
		pos = vec3_802fdc68;
		pos.x = float_neg200_80420a7c + f1;
		pos.y = float_58_80420a80 + f2;
		iconDispGx(&pos, 0x10, 0x65, float_1_80420a84);
	}
	else {
		Vec pos;
		pos = vec3_802fdc74;
		pos.x = float_neg200_80420a7c + f1;
		pos.y = float_58_80420a80 + f2;
		iconDispGx(&pos, 0x10, 0x64, float_1_80420a84);
	}
}
#pragma dont_inline reset

void battleAcDelete_AirGauge(BattleWork* work) {
	work->actionCommands.mAcState = 0x3EA;
}

// Deriving both the state and disp parameters from one BattleActions* makes MWCC materialise the
// disp base: `addi r31, r4, 0x1f20`, then 0x14(r31).
void battleAcDisp_AirGauge(CameraId camId, void* param) {
	BattleActions* ac = &((BattleWork*)param)->actionCommands;
	s32 state = ac->mAcState;
	BattleACDispParams* ag = &ac->dispParams;

	switch (state) {
	case 0x63:
		ag->iconX = intplGetValue(4, 0x14 - ag->counter, 0x14, float_neg300_80420a88, float_30_80420a8c);
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
			ag->iconX = intplGetValue(4, ag->counter - 0x28, 0x14, float_30_80420a8c, float_neg300_80420a88);
		}
		else {
			ag->iconX = float_30_80420a8c;
		}
		actionCommandDisp(ag->iconX, ag->iconY);
		if (ag->counter < 0x3C) {
			ag->counter++;
		}
		break;
	}
}

s32 battleAcResult_AirGauge(BattleWork* work) {
	return work->actionCommands.result;
}

// Table record used by _GetGaugeDecValue; laid out in the original as 5 records of 0xC bytes.
typedef struct GaugeTableEntry {
	s32 threshold; // 0x0
	f32 value;     // 0x4
	s32 unk8;      // 0x8
} GaugeTableEntry;


// NONMATCHING (92.72%): register allocation only; the work-area pointer is folded into the BattleWork base
// and the base/table registers differ.
f32 _GetGaugeDecValue(void) {
	BattleWork* work = _battleWorkPointer;
	AirGaugeDecWork* dec = (AirGaugeDecWork*)((u8*)work + 0x1f4c);
	GaugeTableEntry* table;
	f32 max;
	u32 value;
	u8* base = lbl_803ABCE0;

	switch (*(s32*)((u8*)work + 0x1ce4)) {
	case 0:
		max = float_0p3_80420a90;
		table = (GaugeTableEntry*)(base + 0x40);
		break;
	case 1:
		max = float_0p33_80420a94;
		table = (GaugeTableEntry*)(base + 0x7C);
		break;
	case 2:
		max = float_0p36_80420a98;
		table = (GaugeTableEntry*)(base + 0xB8);
		break;
	case 3:
		max = float_0p4_80420a9c;
		table = (GaugeTableEntry*)(base + 0xF4);
		break;
	case 4:
		max = float_0p44_80420aa0;
		table = (GaugeTableEntry*)(base + 0x130);
		break;
	case 5:
		max = float_0p47_80420aa4;
		table = (GaugeTableEntry*)(base + 0x16C);
		break;
	case 6:
	default:
		max = float_0p5_80420aa8;
		table = (GaugeTableEntry*)(base + 0x1A8);
		break;
	}

	value = dec->value;
	while (value > (u32)table->threshold) {
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
	max = max - intplGetValue(0, value - (table - 1)->threshold, table->threshold - (table - 1)->threshold,
	                          (table - 1)->value, table->value);
	return max;
}


// 100%. The `opt_common_subs`/`difficulty`/`dat_80420a70` shapes are match-only (comments at their uses);
// the natural forms are under #else.
#ifdef MUST_MATCH
// Match-only: with common-subexpression elimination on, MWCC colours the gauge clamp's field/constant
// the other way round (field in f0) than the original (field in f1).
#pragma opt_common_subs off
#endif
#pragma optimize_for_size on
s32 battleAcMain_AirGauge(BattleWork* work) {
#ifdef MUST_MATCH
	// Match-only: addressing the gauge work through the header's named field keeps `dec` in its own
	// base register (r31) instead of folding it into the BattleWork base. See ac_monosiri_target.c.
	BattleActions* ac = &work->actionCommands;
	AirGaugeDecWork* dec = (AirGaugeDecWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;
#else
	AirGaugeDecWork* dec = (AirGaugeDecWork*)((u8*)work + 0x1f4c);
	BattleACDispParams* disp = &work->actionCommands.dispParams;
#endif

	switch (work->actionCommands.mAcState) {
	case 0x63:
		break;
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = float_neg300_80420a88;
		disp->iconY = float_0_80420aac;
		disp->counter = 0x14;
		disp->value = float_0_80420aac;
		disp->field_0xC = dat_80420a70;
		dec->frame = 0;
		work->actionCommands.outputParams[0] = 0;
		work->actionCommands.outputParams[2] = 0;
		work->actionCommands.outputParams[1] = 0;
		work->actionCommands.mAcState = 0x63;
		work->actionCommands.result = 1;
		dec->gaugePercent = float_0_80420aac;
		dec->field_0x8 = 0;
		dec->value = 0;
		dec->flag = 0;
		break;
	case 0x64:
		if ((work->actionCommands.field_0x4 & 1) && (work->actionCommands.mAcUnit->mTokenFlags & 0x10)) {
			if ((s32)irand(100) >= 0) {
				work->actionCommands.field_0x8 = *(s32*)((u8*)work + 0x1cd4) / 2;
				work->actionCommands.outputParams[0] = 0;
				work->actionCommands.outputParams[2] = 0;
			}
			else {
				s32 r = (s32)irand(*(s32*)((u8*)work + 0x1cd0) - 1) + 1;
				work->actionCommands.outputParams[0] = r;
				switch (r) {
				case 1:
					work->actionCommands.field_0x8 = *(s32*)((u8*)work + 0x1cd4) - 1;
					break;
				case 2:
					work->actionCommands.field_0x8 = *(s32*)((u8*)work + 0x1cd8) - 1;
					break;
				case 3:
					work->actionCommands.field_0x8 = *(s32*)((u8*)work + 0x1cdc) - 1;
					break;
#ifdef MUST_MATCH
				// Match-only empty case 4: sharing the default body makes MWCC pivot the
				// decision tree on 3 and emit the original's extra `b default`. The natural
				// source has only `default`.
				case 4:
#endif
				default:
					work->actionCommands.field_0x8 = *(s32*)((u8*)work + 0x1cdc) + 1;
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
		if ((s32)BattlePadCheckNow(0x100) != 0) {
			work->actionCommands.outputParams[1] = 1;
			if (BattlePadCheckTrigger(0x100)) {
				dec->field_0x8++;
				dec->value = 0;
			}
			dec->value++;
		}
		else {
			work->actionCommands.outputParams[1] = 0;
			dec->value = 0;
		}
		dec->gaugePercent -= _GetGaugeDecValue();
		if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand) {
			dec->field_0x8 = 1;
			dec->gaugePercent = float_100_80420a74;
		}
		if (dec->gaugePercent < float_0_80420aac) {
			dec->gaugePercent = float_0_80420aac;
		}
		if (dec->gaugePercent > float_100_80420a74) {
			dec->gaugePercent = float_100_80420a74;
		}
		BattleAcGaugeSeUpdate(dec->gaugePercent);
		if (dec->frame < *(s32*)((u8*)work + 0x1ccc)) {
			break;
		}
		if (work->actionCommands.mAcUnit->badgesEquipped.AutoCommand) {
			dec->field_0x8 = 1;
			dec->gaugePercent = float_100_80420a74;
		}
		if (dec->field_0x8 == 0) {
			work->actionCommands.outputParams[0] = 0;
			work->actionCommands.outputParams[2] = 0;
			work->actionCommands.result = 0;
			BattleAcGaugeSeDelete();
			return 0;
		}
		work->actionCommands.result = 2;
		work->actionCommands.mResultCount++;
		{
#ifdef MUST_MATCH
			// Match-only: assigning the difficulty inside the first `else if` (rather than a plain load)
			// makes MWCC keep it in r4 across the chain and load it lazily, as the original does.
			s32 difficulty;
#endif
			if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cd4)) {
				work->actionCommands.outputParams[0] = 1;
			}
#ifdef MUST_MATCH
			else if ((difficulty = *(s32*)((u8*)work + 0x1cd0)) <= 2) {
#else
			else if (*(s32*)((u8*)work + 0x1cd0) <= 2) {
#endif
				work->actionCommands.outputParams[0] = 2;
			}
			else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cd8)) {
				work->actionCommands.outputParams[0] = 2;
			}
#ifdef MUST_MATCH
			else if (difficulty <= 3) {
#else
			else if (*(s32*)((u8*)work + 0x1cd0) <= 3) {
#endif
				work->actionCommands.outputParams[0] = 3;
			}
			else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1cdc)) {
				work->actionCommands.outputParams[0] = 3;
			}
#ifdef MUST_MATCH
			else if (difficulty <= 4) {
#else
			else if (*(s32*)((u8*)work + 0x1cd0) <= 4) {
#endif
				work->actionCommands.outputParams[0] = 4;
			}
			else if (dec->gaugePercent < (f32)*(s32*)((u8*)work + 0x1ce0)) {
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
			if ((s32)work->actionCommands.outputParams[0] > 0) {
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
		dec->field_0x14 = 0x3C;
		work->actionCommands.mAcState = 0x3EB;
		// fallthrough
	case 0x3EB:
		dec->field_0x14 = dec->field_0x14 - 1;
		if (dec->field_0x14 <= 0) {
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
#pragma optimize_for_size reset

// .data: float pairs and gauge tables. The gauge tables are reached from lbl_803ABCE0 (defined in
// nameent.c) + 0x40, so this block's layout (7 pairs, then 7 tables of 5 records) is load-bearing.
// -sdata 48 would move these 8-byte objects to .sdata; the original has them in .data.
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
