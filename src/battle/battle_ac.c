#include "battle/battle_ac.h"
#include "battle/battle.h"
#include "battle/battle_audience.h"
#include "battle/ac/ac_table.h"
#include "driver/dispdrv.h"
#include "pmario_sound.h"

extern ActionCommandEntry ActionCommandList[];
extern BattleWork* _battleWorkPointer;

//local prototypes
void psndSFX_pit(s32 id, s32 pitch);


void BattleActionCommandManagerInit(BattleWork* wp) {
	BtlPad_WorkInit(&wp->actionCommands.pad);
	wp->actionCommands.maincb = NULL;
	wp->actionCommands.resultcb = NULL;
	wp->actionCommands.dispcb = NULL;
	wp->actionCommands.deletecb = NULL;
	wp->actionCommands.field_0x288 = -1;
}

void BattleActionCommandManager(BattleWork* wp) {
	BattleACMainCallback main;
	DispCallback disp;

	BtlPad_WorkUpdate(&wp->actionCommands.pad, 0);
	main = wp->actionCommands.maincb;
	if (main) {
		if (!main(wp)) {
			wp->actionCommands.maincb = NULL;
		}

		disp = wp->actionCommands.dispcb;
		if (disp) {
			dispEntry(CAMERA_2D, 900.0f, 1, disp, wp);
		}
	}
}

s32 BattleActionCommandResult(BattleWork* wp) {
	BattleACResultCallback result;

	result = wp->actionCommands.resultcb;
	if (result) {
		return result(wp);
	}
	else {
		return wp->actionCommands.result & 2;
	}
}

void BattleActionCommandDeclareACResult(BattleWork* work, BattleWeapon* weapon, s32 result) {
	StarPowerInfo* info = &work->impendingBonuses;

	switch (result) {
	case -1:
		BattleAudience_Case_ActionCommandBad(weapon);
		break;
	default:
		info->mWeapon = weapon;
		info->mSpAcSuccessMultiplier = ((f32)result * 0.25f) + 0.5f;
		info->field_0x9 = weapon->field_0x19;
		info->mBingoSlotChance = weapon->mBingoSlotChance;
#ifdef MUST_MATCH
		// retail passes weapon in r3 even though the callee ignores it
		((void (*)(void*))BattleAudience_Case_ActionCommandGood)(weapon);
#else
		BattleAudience_Case_ActionCommandGood();
#endif
		break;
	}
}

// NONMATCHING (90.14%): 1 missing, 2 extra, plus register differences.
void BattleActionCommandSetup(BattleWork* work, s32 id, BattleUnit* unit, s32 a4, s32 a5) {
	ActionCommandEntry* entry; // r6

	work->actionCommands.mAcUnit = unit;
	work->actionCommands.field_0x4 = a4;
	work->actionCommands.mAcState = 0;
	work->actionCommands.mResultCount = 0;
	work->actionCommands.result = 1;
	work->actionCommands.maincb = NULL;
	work->actionCommands.dispcb = NULL;
	work->actionCommands.resultcb = NULL;
	work->actionCommands.deletecb = NULL;
	work->actionCommands.field_0x8 = a5;
	work->actionCommands.field_0x30 = 0;
	entry = ActionCommandList;
	while (entry++->id) {
		if (entry->id == id) {
			work->actionCommands.maincb = entry->main;
			if (!(work->actionCommands.mAcUnit->mTokenFlags & 0x10)){
				work->actionCommands.dispcb = entry->disp;
			}
			work->actionCommands.resultcb = entry->result;
			work->actionCommands.deletecb = entry->delete;
			return;
		}
	}
}

void BattleActionCommandStart(BattleWork* work) {
	work->actionCommands.mAcState = 100;
}

void BattleActionCommandStop(BattleWork* work) {
	if (work->actionCommands.deletecb) {
		work->actionCommands.deletecb(work);
	}
}

typedef struct GuardFrames {
	u8 a, b, c, d, e, f, g;
} GuardFrames;

#ifdef MUST_MATCH
// The tables are const in retail, but the compiler only keeps retail's direct-symbol `lwzu` reads
// when the globals are declared non-const; an unqualified `const` turns them into `@sda21` accesses.
// `volatile` on the first table stops CSE from reusing the base register for the loaded word.
extern volatile GuardFrames N_normal_guard_frames[];
extern GuardFrames N_superguard_frames[];
#else
extern const GuardFrames N_normal_guard_frames[];
extern const GuardFrames N_superguard_frames[];
#endif

// NONMATCHING (32.95%): ours inlines BattleACPadCheckRecordTrigger, which the original calls with `bl`. Before
// that: the first guard-frame copy is `lis; addi; lwz` here vs retail's `lwzu`
// (the loaded word is parked in the base register, blocking the fold), and that extra live register
// rotates the rest of the callee-saved allocation. The second `lwzu` and all stores match.
s32 BattleActionCommandCheckDefence(BattleUnit* unit, BattleWeapon* weapon) {
	BattleWork* work = _battleWorkPointer;
	BattleWorkPad* pad = &work->actionCommands.pad;
	GuardFrames normal;
	GuardFrames super;
	s16* guardFrames = (s16*)&work->actionCommands.field_78[0x10];
	s32 index;
	s32 normalFrames;
	s32 superFrames;
	s32 i;
	s32 count;
	s32 trigger;
	s32 defended = 0;

	normal = N_normal_guard_frames[0];
	super = N_superguard_frames[0];
	work->actionCommands.mDefenseResult = 0;

	index = unit->badgesEquipped.Unsimplifier + 3 - unit->badgesEquipped.Simplifier;
	if (index < 0) {
		index = 0;
	}
	if (index > 6) {
		index = 6;
	}

	superFrames = ((u8*)&super)[index];
	normalFrames = ((u8*)&normal)[index];

	if (weapon->superguardType != 0 && unit->currentHp > 0) {
		if (*guardFrames > 0) {
			(*guardFrames)--;
			defended = 1;
			work->actionCommands.mDefenseResult = 5;
		}
		else if (unit->badgesEquipped.AutoCommand != 0) {
			defended = 1;
			work->actionCommands.mDefenseResult = 5;
		}
		else {
			for (i = 0; i < superFrames; i++) {
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
				if (trigger) {
					break;
				}
			}
			if (i < superFrames) {
				count = 0;
				for (i = 0; i < 15; i++) {
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
					if (trigger) {
						count++;
					}
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
					if (trigger) {
						count++;
					}
				}
				if (count < 2) {
					defended = 1;
					work->actionCommands.mDefenseResult = 5;
				}
			}
		}
	}

	if (!defended) {
		if (*guardFrames > 0) {
			(*guardFrames)--;
			work->actionCommands.mDefenseResult = 4;
		}
		else {
			count = 0;
			for (i = 0; i < 15; i++) {
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
				if (trigger) {
					count++;
				}
				trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x200);
				if (trigger) {
					count++;
				}
			}
			if (count >= 2) {
				work->actionCommands.mDefenseResult = 1;
			}
			else {
				for (i = 0; i < normalFrames; i++) {
					trigger = (s32)BattleACPadCheckRecordTrigger(i, 0x100);
					if (trigger) {
						break;
					}
				}
				if (i >= normalFrames) {
					if (count > 0) {
						work->actionCommands.mDefenseResult = 1;
					}
					else {
						work->actionCommands.mDefenseResult = 2;
					}
				}
				else {
					work->actionCommands.mDefenseResult = 4;
				}
			}
		}
	}

	BtlPad_WorkInit(pad);
	return work->actionCommands.mDefenseResult;
}

u32 BattleACPadCheckRecordTrigger(s32 chan, s32 mask) {
	return _battleWorkPointer->actionCommands.pad.mButtonsPressedHistory[chan] & mask;
}

s32 BattleActionCommandGetDefenceResult(void) {
	return _battleWorkPointer->actionCommands.mDefenseResult;
}

// NONMATCHING (91.67%): 1 extra.
void BattleActionCommandResetDefenceResult(void) {
	_battleWorkPointer->actionCommands.mDefenseResult = 0;
	BtlPad_WorkInit(&_battleWorkPointer->actionCommands.pad);
}

s32 BattleActionCommandGetDifficulty(BattleWork* work) {
	return work->actionCommands.mAcDifficulty;
}

void BattleActionCommandSetDifficulty(BattleWork* work, BattleUnit* unit, s32 difficulty) {
	s32 adjusted;

	work->actionCommands.mBaseAcDifficulty = difficulty;
	adjusted = difficulty;
	adjusted -= unit->badgesEquipped.Simplifier;
	adjusted += unit->badgesEquipped.Unsimplifier;
	if (adjusted < 0) {
		adjusted = 0;
	}
	if (adjusted > 6) {
		adjusted = 6;
	}
	work->actionCommands.mAcDifficulty = adjusted;
}

s32 BattleActionCommandGetPrizeLv(BattleWork* wp, BattleUnit* unit, s32 index) {
	s32 lv;

	if (unit != NULL && (u8)unit->damageCode == 0x1E) {
		return -1;
	}

	lv = index + (u8)wp->actionCommands.mAcDifficulty;
	lv--;
	if (lv < 0) {
		lv = 0;
	}
	if (lv >= 7) {
		lv = 6;
	}
	return lv;
}

s32 BattleACGetButtonIcon(s32 button, s32 status) {
	s32 icon = 0;

	switch (button) {
	case 0x100:
		if (status) {
			icon = 0x65;
		}
		else {
			icon = 0x64;
		}
		break;
	case 0x200:
		if (status) {
			icon = 0x67;
		}
		else {
			icon = 0x66;
		}
		break;
	case 0x400:
		if (status) {
			icon = 0x69;
		}
		else {
			icon = 0x68;
		}
		break;
	case 0x800:
		if (status) {
			icon = 0x6B;
		}
		else {
			icon = 0x6A;
		}
		break;
	case 0x20:
		if (status) {
			icon = 0x81;
		}
		else {
			icon = 0x80;
		}
		break;
	case 0x40:
		if (status) {
			icon = 0x7F;
		}
		else {
			icon = 0x7E;
		}
		break;
	case 0x10:
		if (status) {
			icon = 0x83;
		}
		else {
			icon = 0x82;
		}
		break;
	case 0x10000:
		if (status) {
			icon = 0x7C;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x20000:
		if (status) {
			icon = 0x76;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x40000:
		if (status) {
			icon = 0x78;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x80000:
		if (status) {
			icon = 0x7A;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x90000:
		if (status) {
			icon = 0x7D;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0xA0000:
		if (status) {
			icon = 0x77;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x50000:
		if (status) {
			icon = 0x7B;
		}
		else {
			icon = 0x79;
		}
		break;
	case 0x60000:
		if (status) {
			icon = 0x75;
		}
		else {
			icon = 0x79;
		}
		break;
	}

	return icon;
}

void BattleAcGaugeSeInit(void) {
	BattleWork* work = _battleWorkPointer;

	work->actionCommands.field_0x288 = psndSFXOn("SFX_AC_POWER_GAUGE1");
	*(f32*)&work->actionCommands.field_0x28C[0] = 0.0f;
}

void BattleAcGaugeSeUpdate(f32 value) {
	BattleWork* work = _battleWorkPointer;

	if (value < 0.0f) {
		value = 0.0f;
	}
	if (value > 100.0f) {
		value = 100.0f;
	}
	*(f32*)&work->actionCommands.field_0x28C[0] = value;
	if ((u32)work->actionCommands.field_0x288 != -1) {
		psndSFX_pit(work->actionCommands.field_0x288, (s32)(4092.0f * *(f32*)&work->actionCommands.field_0x28C[0] * 0.01f));
	}
}

void BattleAcGaugeSeDelete(void) {
	BattleWork* work = _battleWorkPointer;

	if ((u32)work->actionCommands.field_0x288 != -1) {
		psndSFXOff(work->actionCommands.field_0x288);
		work->actionCommands.field_0x288 = -1;
	}
}
