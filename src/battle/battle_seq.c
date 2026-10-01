#include "battle/battle_seq.h"
#include "battle/battle.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_sub.h"
#include "driver/mapdrv.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "battle/battle_damage_split.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_nice.h"
#include "mario/mario_party.h"
#include "manager/fontmgr.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include <math.h>
#include <stdio.h>
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") f32 lbl_80360FE0[2] = { -1.0f, 1.0f };

extern BattleWork* _battleWorkPointer;

// Weapon stat table for ITEM_LUCKY_START; _set_effect_luck copies it and btlseqInit uses it as the
// .rodata base for its anim-name strings in the original (str_802EF5FC/604/610 at +0x3AC/3B4/3C0).
const u8 N_weapon_lucky_start[0xC0] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x40,0x20,0x10,0x00,0x00,0x00,0x00,0x00,0x03,0x02,
	0x00,0x00,0x00,0x00,0x00,0x02,0x01,0x08,0x00,0x00,0x07,0xff,0x80,0x00,0x20,0x04,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64,0x03,0x64,0x03,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x02,0x03,0x02,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

f32 float_0_8041c280 = 0.0f;
f32 float_1_8041c284 = 1.0f;
f32 float_110_8041c28c = 110.0f;
f32 float_90_8041c290 = 90.0f;
f32 float_100_8041c294 = 100.0f;
const char str_8041C27C[] = "A1";
const char str_8041C288[] = "A2";
f32 float_1000_8041c298 = 1000.0f;
f32 float_10_8041c2a0 = 10.0f;
f32 float_94_8041c2a4 = 94.0f;
f32 float_120_8041c2a8 = 120.0f;
f32 float_900_8041c2ac = 900.0f;
f32 float_45_8041c2b0 = 45.0f;
f32 float_60_8041c2b4 = 60.0f;
f32 float_0p75_8041c2b8 = 0.75f;
const char str_802EF61C[] = "SFX_BTL_DANGER1";
const char str_802EF62C[] = "SFX_BTL_PINCH1";

//local prototypes

// Needed: without dont_inline the compiler inlines this loop into its caller
// _set_haikei_entry_scale, which the original calls.
#pragma dont_inline on
void _mapobj_data_touch_scale(s32 kind) { //1:1
	BattleWork* wp = _battleWorkPointer;
	s32 i;

	for (i = 0; i < 32; i++) {
		BattleStageObject* obj = &wp->stageObjects[i];
		if (obj->id > 0 && *(u16*)((u8*)obj->data + 6) == kind) {
			obj->mFlags |= 2;
		}
	}
}
#pragma dont_inline reset

// NONMATCHING (79.18%): switch dispatch matches, but btlseqFirstAct/btlseqTurn are still stubs, so the
// compiler inlines them away and the case-3 compare/branch tree differs.
void BattleSequenceManager(void) { //1:1
	BattleWork* wp = _battleWorkPointer;
	switch (BattleGetSeq(wp, SEQ_UNKNOWN)) {
		case 0:
			btlseqInit(wp);
			BattleCheckAllPinchStatus(wp, TRUE);
			break;
		case 1:
			BattleCheckAllPinchStatus(wp, FALSE);
			btlseqFirstAct(wp);
			break;
		case 2:
			BattleCheckAllPinchStatus(wp, FALSE);
			btlseqTurn(wp);
			break;
		case 3:
			btlseqEnd(wp);
			break;
	}
}

void BattleCheckAllPinchStatus(BattleWork* wp, BOOL a2) {
	BattleUnit* unit;
	BOOL v6;
	int i;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			BtlUnit_CheckPinchStatus(unit, a2);
		}
	}

	v6 = FALSE;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			if ((unit->mStatusFlags & 0x20000000) &&
				!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
				!BtlUnit_GetBelong(unit))
			{
				if (unit->currentKind == UNIT_MARIO ||
					(unit->currentKind >= TYPE_PARTNER_MIN &&
					 unit->currentKind < TYPE_PARTNER_MAX)) {
					if (unit->currentHp > 0) {
						v6 = TRUE;
					}
				}
				break;
			}
		}
	}

	if (v6) {
		wp->alertTick--;
		if (wp->alertTick <= 0) {
			wp->alertTick = 60;
			if (wp->commandMenu.field_4 & 1) {
				psndSFXOn(str_802EF61C);
			}
		}
	}
	else {
		v6 = FALSE;
		for (i = 0; i < 64; i++) {
			unit = BattleGetUnitPtr(wp, i);
			if (unit) {
				if ((unit->mStatusFlags & 0x10000000) &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!BtlUnit_GetBelong(unit))
				{
					if (unit->currentKind == UNIT_MARIO ||
						(unit->currentKind >= TYPE_PARTNER_MIN &&
						 unit->currentKind < TYPE_PARTNER_MAX)) {
						if (unit->currentHp > 0) {
							v6 = TRUE;
						}
					}
					break;
				}
			}
		}
		if (v6) {
			wp->alertTick--;
			if (wp->alertTick <= 0) {
				wp->alertTick = 60;
				if (wp->commandMenu.field_4 & 1) {
					psndSFXOn(str_802EF62C);
				}
			}
		}
		else {
			unit = BattleGetMarioPtr(wp);
			if (unit) {
				unit->work[8] = 0;
			}
			wp->alertTick = 0;
		}
	}
}

// Needed: without dont_inline this whole body is inlined into BattleSequenceManager; the original calls it.
#pragma dont_inline on
void btlseqInit(BattleWork* wp) {
	const char* base = (const char*)N_weapon_lucky_start;
	MarioWork* mario;
	PartyEntry* party;

	switch (BattleGetSeq(wp, SEQ_INIT)) {
		case 0x1000000: //continues through to 0x1000001
			BattleSetSeq(wp, SEQ_INIT, 0x1000001);

		case 0x1000001:
			mario = marioGetPtr();
			mario->dispFlags |= 1;
			party = partyGetPtr(marioGetPartyId());
			if (party) {
				party->flags2 |= 1;
			}
			Btl_UnitSetup(wp);
			BattleSetSeq(wp, SEQ_INIT, 0x1000002);
			break;

		case 0x1000002:
			if (BattleCheckEndUnitInitEvent(wp)) {
				BattleSetSeq(wp, SEQ_INIT, 0x1000003u);
			}
			break;

		case 0x1000003:
			if (animEffectAsync((char*)base + 0x3ac, 1) &&
				animEffectAsync((char*)base + 0x3b4, 1) &&
				animEffectAsync((char*)base + 0x3c0, 1) &&
				effNiceAsync(1))
			{
				BattleSetSeq(wp, SEQ_INIT, 0x1000004);
			}
			break;

		case 0x1000004:
			BattleSetSeq(wp, SEQ_INIT, 0x1000005);
			break;

		case 0x1000005:
			BattleStatusWindowEventOn();
			BattleStatusWindowSystemOn();
			BattleSetSeq(wp, SEQ_UNKNOWN, 1);
			BattleSetSeq(wp, SEQ_FIRST_ACT, 0x2000000);
			break;
	}
}
#pragma dont_inline reset

extern BattleWeapon marioDefaultWeapon_Jump;
extern BattleWeapon marioWeapon_KururinJump;
extern BattleWeapon marioDefaultWeapon_Hammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackKaitenHammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackUltraHammer;
extern BattleWeapon partyWeapon_NokotarouFirstAttack;
extern BattleWeapon partyWeapon_SandersFirstAttack;

BattleWeapon* _GetFirstAttackWeapon(s32 id) { //1:1
	switch (id) {
		case 1:
			return &marioDefaultWeapon_Jump;
		case 2:
			return &marioWeapon_KururinJump;
		case 3:
			return &marioDefaultWeapon_Jump;
		case 4:
			return &marioDefaultWeapon_Hammer;
		case 5:
			return &marioDefaultWeapon_FirstAttackKaitenHammer;
		case 6:
			return &marioDefaultWeapon_FirstAttackUltraHammer;
		case 7:
			return &partyWeapon_NokotarouFirstAttack;
		case 8:
			return &partyWeapon_SandersFirstAttack;
	}
	return NULL;
}

// NONMATCHING (1.75%): 1 instruction differs, 501 missing, plus register differences.
void btlseqFirstAct(BattleWork* wp) {
	switch (BattleGetSeq(wp, SEQ_FIRST_ACT)) {

	}
}

extern const char str_802EF5C8[];

// _set_effect_luck is an EVT command (nanika_luck_event) returning 2. The four comma initializers are
// match-only: `(0, x)` computes the same value as `x`, but it makes MWCC assign the callee-saved
// registers r30=&weapon, r29=&z, r28=&y, r27=sfxName as the original does. Without them the string
// address is copied through r0 (one extra `mr`), so the natural forms are under #else.
s32 _set_effect_luck(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleWeapon weapon = *(BattleWeapon*)N_weapon_lucky_start;
	f32 z, y, x;
#ifdef MUST_MATCH
	BattleWeapon* weaponPtr = (0, &weapon);
	f32* pz = (0, &z);
	f32* py = (0, &y);
	const char* sfxName = (0, str_802EF5C8);
#else
	BattleWeapon* weaponPtr = &weapon;
	f32* pz = &z;
	f32* py = &y;
	const char* sfxName = str_802EF5C8;
#endif
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 i;
	EffectEntry* eff;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);

		if (unit == NULL) {
			continue;
		}
		if (unit->badgesEquipped.LuckyStart == 0) {
			continue;
		}
		if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
			continue;
		}
		switch (irand(4)) {
		case 0:
			weapon.dodgyChance = 0;
			weapon.dodgyTime = 0;
			weapon.hpRegenTime = 0;
			weapon.hpRegenStrength = 0;
			weapon.fpRegenTime = 0;
			weapon.fpRegenStrength = 0;
			break;
		case 1:
			weapon.electricChance = 0;
			weapon.electricTime = 0;
			weapon.hpRegenTime = 0;
			weapon.hpRegenStrength = 0;
			weapon.fpRegenTime = 0;
			weapon.fpRegenStrength = 0;
			break;
		case 2:
			weapon.electricChance = 0;
			weapon.electricTime = 0;
			weapon.dodgyChance = 0;
			weapon.dodgyTime = 0;
			weapon.fpRegenTime = 0;
			weapon.fpRegenStrength = 0;
			break;
		default:
			weapon.electricChance = 0;
			weapon.electricTime = 0;
			weapon.dodgyChance = 0;
			weapon.dodgyTime = 0;
			weapon.hpRegenTime = 0;
			weapon.hpRegenStrength = 0;
			break;
		}
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BattleSetStatusDamageFromWeapon(BattleGetSystemPtr(wp), unit, part, weaponPtr, 0x100);
		BtlUnit_GetPos(unit, &x, py, pz);
		psndSFXOn(sfxName);
		eff = effNiceEntry(6, x - float_45_8041c2b0, float_60_8041c2b4 + y, z);
		((NiceEffectWork*)eff->userdata)->field_0x1C = float_0p75_8041c2b8;
	}
	return 2;
}

// NONMATCHING (0.05%): empty placeholder, not decompiled yet.
void btlseqTurn(BattleWork* wp) {

}

// The three rule-message pointer tables are the 0x110-byte arrays the original addresses as
// lbl_80360FE0+0x40 (2bu), +0x150 (1bu) and +0x260 (after); one base register is shared.
// The pooled int->float double is a local @770 here instead of the original's shared
// double_to_int_802ef590 because this file's other int->float users are not written yet (MWCC then
// pools a local constant), and it lands at a different .rodata offset because the rule-message
// strings are also not in the file.
extern GXColor dat_8041c26c;
extern f32 lbl_80360FE0[];

// `#pragma opt_propagation off` is required: with propagation on, MWCC folds each branch's
// `tbl = (const char**)(base + 0x150); tbl[i]` into `add base,i*4; lwz 0x150(base)`, where the
// original keeps the table address in a register (`addi base,off; lwzx base,i*4`).
#pragma opt_propagation off

// 100%: the local @NNN int->float double is a relocation name only (the .rodata value and its
// offset are the original's double_to_int_802ef590; cf. battle_attack_audience.c's
// _get_item_announce_disp). Using _battleWorkPointer->fbatInfo directly, rather than a wp temp,
// keeps it and lbl_80360FE0 in the original's r5/r6 prologue registers.
void _rule_disp(void) {
	u8* base = (u8*)lbl_80360FE0;
	const char** tbl;
	FieldBattleInfo* info = _battleWorkPointer->fbatInfo;
	GXColor color;
	u16 lines;
	u16 width;
	f32 x;
	char buf[0x100];

	if (evtGetValue(NULL, 0xF5DE0180) < 0xAC) {
		if (evtGetValue(NULL, 0xF5DE03C4) < 0xB) {
			tbl = (const char**)(base + 0x150);
			sprintf(buf, msgSearch(tbl[info->field_0x14[4]]), info->field_0x14[6]);
		} else {
			tbl = (const char**)(base + 0x40);
			sprintf(buf, msgSearch(tbl[info->field_0x14[4]]), info->field_0x14[6]);
		}
	} else {
		tbl = (const char**)(base + 0x260);
		sprintf(buf, msgSearch(tbl[info->field_0x14[4]]), info->field_0x14[6]);
	}
	width = FontGetMessageWidthLine(buf, &lines);
	lines++;
	color = dat_8041c26c;
	x = float_0_8041c280 - (f32)(width >> 1);
#ifdef MUST_MATCH
	((void (*)(s32, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &color, x - float_10_8041c2a0,
		float_94_8041c2a4, (f32)(width + 20), (f32)(lines * 29 + 3), float_10_8041c2a0);
#else
	windowDispGX_Waku_col(0, color, x - float_10_8041c2a0, float_94_8041c2a4,
		(f32)(width + 20), (f32)(lines * 29 + 3), float_10_8041c2a0);
#endif
	FontDrawStart();
	FontDrawMessage((s32)x, (s32)float_120_8041c2a8 - 29, buf);
}
#pragma opt_propagation reset

void btlseqEnd(BattleWork* wp) {

}

BOOL BattlePhaseEndCheck(void) { //1:1
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit;
	s32 i;

	for (i = 0; i < 64; i++) {
		s32 id = *(s32*)((u8*)wp + 0x120 + i * 4);
		if (id != -1) {
			unit = BattleGetUnitPtr(wp, id);
			if (unit != NULL && unit->moveState != 10 && unit->movesRemaining > 0 &&
				!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL))
			{
				break;
			}
		}
	}
	if (i >= 64) {
		return TRUE;
	}
	return FALSE;
}

void btlseqPhase(BattleWork* wp) { //1:1
	switch (BattleGetSeq(wp, SEQ_PHASE)) {
		case 0x4000000:
			BattleIncSeq(wp, SEQ_PHASE);
			BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
			break;
		case 0x4000001:
		case 0x4000002:
		case 0x4000003:
		case 0x4000004:
		case 0x4000005:
			btlseqMove(wp);
			break;
		case 0x4000006:
			BattleIncSeq(wp, 3);
			BattleSetSeq(wp, SEQ_PHASE, 0x4000000);
			break;
	}
}

void btlseqTurnFirstProcess(BattleWork* wp) { //1:1
	BattleUnitKind* kind;
	s32 i;
	BattleUnit* unit;

	BtlActRec_JudgeTurnRuleKeep();
	wp->turnCount++;
	if (wp->turnCount >= 2) {
		BattleMajinaiCheck();
	}
	*(s32*)((u8*)wp + 0x18FDC) = 0;
	*(s32*)((u8*)wp + 0x18FD8) = 0;
	BtlActRec_AddCount((u8*)wp + 0x16F3B);

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			unit->activeTurns++;
			BtlUnit_ResetMoveStatus(unit);
			kind = unit->mKindParams;
			*(s32*)((u8*)unit + 0x28) = *(s8*)((u8*)kind + 0x88)
				+ irand(*(s8*)((u8*)kind + 0x89))
				- *(s8*)((u8*)kind + 0x89) / 2;
			if (BtlUnit_CheckStatusFlag(unit, 0x01000000)) {
				BtlUnit_OffStatusFlag(unit, 0x01000000);
			}
		}
	}
	BattleFogEndCheck();
}

// NONMATCHING (80.03%): control-flow layout of the four event-id ifs differs (the original checks the condition
// after the body) and wp/i land in r28/r30 instead of r30/r28; logic is verified against the asm.
s32 BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp) { //1:1
	s32 i;
	BOOL result = TRUE;
	BattleUnit* unit;

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			if (unit->phaseEventId) {
				if (evtCheckID(unit->phaseEventId)) {
					result = FALSE;
				}
				else {
					unit->phaseEventId = 0;
				}
			}
			if (unit->attackEventId) {
				if (evtCheckID(unit->attackEventId)) {
					result = FALSE;
				}
				else {
					unit->attackEventId = 0;
				}
			}
			if (unit->damageEventId) {
				if (evtCheckID(unit->damageEventId)) {
					result = FALSE;
				}
				else {
					unit->damageEventId = 0;
				}
			}
			if (*(s32*)(unit->field_0x2B4 + 4)) {
				if (evtCheckID(*(s32*)(unit->field_0x2B4 + 4))) {
					result = FALSE;
				}
				else {
					*(s32*)(unit->field_0x2B4 + 4) = 0;
				}
			}
		}
	}
	if (!result) {
		return result;
	}
	if (BattleAfterReactionMain()) {
		result = FALSE;
	}
	return result;
}

BOOL BattleWaitAllActiveEvtEnd(BattleWork* wp) { //1:1
	u8* p = (u8*)wp + 0x20000;
	s32 ret = BattleWaitAllActiveEvtEnd_NoBgSetEndWait(wp);

	if (!ret) {
		return ret;
	}
	if (*(s32*)(p - 0x7d68)) {
		if (evtCheckID(*(s32*)(p - 0x7d68))) {
			return FALSE;
		}
		*(s32*)(p - 0x7d68) = 0;
	}
	if (*(s32*)(p - 0x7d58)) {
		if (evtCheckID(*(s32*)(p - 0x7d58))) {
			return FALSE;
		}
		*(s32*)(p - 0x7d58) = 0;
	}
	if (*(s32*)(p - 0x7f20)) {
		if (evtCheckID(*(s32*)(p - 0x7f20))) {
			return FALSE;
		}
		*(s32*)(p - 0x7f20) = 0;
	}
	if (*(s32*)(p - 0x7d54)) {
		if (evtCheckID(*(s32*)(p - 0x7d54))) {
			return FALSE;
		}
		*(s32*)(p - 0x7d54) = 0;
	}
	if (*(s32*)(p - 0x7d5c)) {
		if (evtCheckID(*(s32*)(p - 0x7d5c))) {
			return FALSE;
		}
		*(s32*)(p - 0x7d5c) = 0;
	}
	return TRUE;
}

void btlseqPhaseFirstProcess(BattleWork* wp) { //1:1
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 count;
	s32 i;

	*(s32*)((u8*)wp + 0x1C70) = 0;
	*(u8*)((u8*)wp + 0x1C74) = 0;
	for (i = 0; i < 64; i++) {
		*(s32*)((u8*)wp + 0x120 + i * 4) = -1;
	}
	count = 0;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			unit->mMoveColorLv = 0xFF;
			if (unit->moveState == 10) {
				if (!BtlUnit_CheckStatus(unit, STATUS_SLOW)) {
					continue;
				}
			}
			if (*(s32*)((u8*)unit + 0x2C) != BattleGetSeq(wp, SEQ_PHASE)) {
				continue;
			}
			if (unit->mAttributeFlags & 0x80000) {
				continue;
			}
			*(s32*)((u8*)wp + 0x120 + count * 4) = unit->unitId;
			unit->protectId = -1;
			part = unit->mParts;
			while (part) {
				if (*(s32*)((u8*)part + 0x1C0) != -1) {
					*(u8*)((u8*)part + 0x4E4) = 1;
				}
				part = part->mNextPart;
			}
			count++;
		}
	}
	battleSortPhaseMoveTable(wp, count);
	battleMakePhaseEvtTable(wp);
}

typedef struct PhaseEvtEntry {
	s32 unitIndex; //0x0
	s32 value;     //0x4
} PhaseEvtEntry;

void battleMakePhaseEvtTable(BattleWork* wp) { //1:1
	PhaseEvtEntry* table = (PhaseEvtEntry*)((u8*)wp + 0x220);
	BattleUnit* unit;
	s32 count;
	s32 i;
	s32 j;

	for (i = 0; i < 64; i++) {
		table[i].unitIndex = -1;
		table[i].value = 0;
	}
	count = 0;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (!unit) {
			continue;
		}
		if (unit->mAttributeFlags & 0x80000) {
			continue;
		}
		table[count].unitIndex = i;
		{
			f32 x, y, z;
			BtlUnit_GetHomePos(unit, &x, &y, &z);
			if (x >= float_0_8041c280) {
				table[count].value = (s32)(float_1000_8041c298 + x);
			}
			else {
				table[count].value = (s32)fabs(x);
			}
		}
		count++;
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			if (table[j].value > table[j + 1].value) {
				PhaseEvtEntry tmp = table[j];
				table[j] = table[j + 1];
				table[j + 1] = tmp;
			}
		}
	}
}

// NONMATCHING (99.19%): all instructions match except the count/wp callee-saved pair is swapped
// (target count r30/wp r31, compiler picks count r31/wp r30). Declaration order, raw vs header-field
// offsets, 128 pragma combos, compilers 1.3-2.7, inline helpers, f32/Vec/array temps and loop forms tried.
void battleSortPhaseMoveTable(BattleWork* wp, s32 mode) { //1:1
	s32 count = mode;
	s32 i;
	s32 j;
	BattleUnit* a;
	BattleUnit* b;

	if (count <= 0) {
		count = 0;
		for (i = 0; i < 64; i++) {
			if (*(s32*)(wp->field_120 + i * 4) == -1) {
				break;
			}
			count++;
		}
	}

	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			f32 ax, ay, az;
			f32 bx, by, bz;
			a = BattleGetUnitPtr(wp, *(s32*)(wp->field_120 + j * 4));
			b = BattleGetUnitPtr(wp, *(s32*)(wp->field_120 + j * 4 + 4));
			BtlUnit_GetHomePos(a, &ax, &ay, &az);
			BtlUnit_GetHomePos(b, &bx, &by, &bz);
			if (fabs(ax) > fabs(bx)) {
				s32 tmp = *(s32*)(wp->field_120 + j * 4);
				*(s32*)(wp->field_120 + j * 4) = *(s32*)(wp->field_120 + j * 4 + 4);
				*(s32*)(wp->field_120 + j * 4 + 4) = tmp;
			}
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			a = BattleGetUnitPtr(wp, *(s32*)(wp->field_120 + j * 4));
			b = BattleGetUnitPtr(wp, *(s32*)(wp->field_120 + j * 4 + 4));
			if (*(s32*)(a->field_0x27 + 1) < *(s32*)(b->field_0x27 + 1)) {
				s32 tmp = *(s32*)(wp->field_120 + j * 4);
				*(s32*)(wp->field_120 + j * 4) = *(s32*)(wp->field_120 + j * 4 + 4);
				*(s32*)(wp->field_120 + j * 4 + 4) = tmp;
			}
		}
	}
}

// The field order in this declaration list is deliberate: it drives MWCC's callee-saved register
// allocation to the original's (wp r28, offset r27, i r29, count r30, alliance r31). The loop base
// `i * sizeof(BattleAlliance)` lets the compiler strength-reduce the byte offset into its own
// induction register.
BOOL BattleCheckConcluded(BattleWork* wp) { //1:1
	BattleUnit* unit;
	s32 j;
	BattleAlliance* alliance;
	s32 count;
	s32 i;

	count = 0;
	for (i = 0; i < 3; i++) {
		alliance = (BattleAlliance*)((u8*)wp->alliances + i * sizeof(BattleAlliance));
		alliance->clearConditionMet = 0;
#ifdef MUST_MATCH
		// The original reads allianceId as an unsigned halfword (`lhz`/`cmplwi`); the header
		// declares the field s16, whose sign-extending load compiles to `lha`. Natural form in #else.
		if (*(u16*)&alliance->allianceId == 0) {
#else
		if (alliance->allianceId == 0) {
#endif
			count++;
			alliance->clearConditionMet = 1;
			continue;
		}
#ifdef MUST_MATCH
		if (*(u16*)&alliance->allianceId & 1) {
#else
		if (alliance->allianceId & 1) {
#endif
			for (j = 0; j < 64; j++) {
				unit = BattleGetUnitPtr(wp, j);
				if (unit != NULL && unit->mAlliance == i &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!(unit->mAttributeFlags & kInactive) &&
					!(unit->mAttributeFlags & 0x10000000))
				{
					break;
				}
			}
			if (j >= 64) {
				count++;
				alliance->clearConditionMet = 1;
				continue;
			}
		}
#ifdef MUST_MATCH
		if (*(u16*)&alliance->allianceId & 2) {
#else
		if (alliance->allianceId & 2) {
#endif
			for (j = 0; j < 64; j++) {
				unit = BattleGetUnitPtr(wp, j);
				if (unit != NULL && unit->mAlliance == i &&
					(unit->mAttributeFlags & kUnknown_0x40000) &&
					!BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					!(unit->mAttributeFlags & 0x10000000))
				{
					break;
				}
			}
			if (j >= 64) {
				count++;
				alliance->clearConditionMet = 1;
			}
		}
	}
	if (count >= 2) {
		return TRUE;
	}
	return (wp->flags & 0x30) != 0;
}

// EVT command (default_haikei_set_entry_event): 60-frame delay, then animates the A1/A2
// background objects in over 15/20 frames. Returns 2 once both have finished (0 while animating).
s32 _set_haikei_entry_scale(EventEntry* event, BOOL isFirstCall) {
	BattleWork* wp = _battleWorkPointer;
	s32 ret = 0;
#ifdef MUST_MATCH
	// Declaration order and the separate obj2 reproduce the original's FPR/GPR allocation; the
	// natural form is under #else.
	f32 scale2, scale1, scaleDiv;
	BattleStageObject* obj;
	BattleStageObject* obj2;
#else
	f32 scale1, scale2, scaleDiv;
	BattleStageObject* obj;
#endif
	s32 i;

	if (isFirstCall) {
		event->userdata[0] = 60;
		event->userdata[1] = 0;
		if (mapGetMapObj(str_8041C27C) != NULL) {
			mapObjScale(str_8041C27C, float_0_8041c280, float_0_8041c280, float_1_8041c284);
		}
		if (mapGetMapObj(str_8041C288) != NULL) {
			mapObjScale(str_8041C288, float_0_8041c280, float_0_8041c280, float_1_8041c284);
		}
	}
	if (event->userdata[0] > 0) {
		event->userdata[0]--;
		if (event->userdata[0] > 0) {
			if (mapGetMapObj(str_8041C27C) != NULL) {
				mapObjScale(str_8041C27C, float_0_8041c280, float_0_8041c280, float_1_8041c284);
				_mapobj_data_touch_scale(0);
			}
			if (mapGetMapObj(str_8041C288) != NULL) {
				mapObjScale(str_8041C288, float_0_8041c280, float_0_8041c280, float_1_8041c284);
				_mapobj_data_touch_scale(1);
			}
			return 0;
		}
	}
	event->userdata[1]++;
	if (mapGetMapObj(str_8041C27C) != NULL) {
		if (event->userdata[1] >= 40) {
			mapObjScale(str_8041C27C, float_1_8041c284, float_1_8041c284, float_1_8041c284);
			_mapobj_data_touch_scale(0);
		} else {
			ret = 1;
			if (event->userdata[1] < 15) {
				mapObjScale(str_8041C27C, float_0_8041c280, float_0_8041c280, float_1_8041c284);
				_mapobj_data_touch_scale(0);
			} else {
				mapObjScale(str_8041C27C, float_1_8041c284, float_1_8041c284, float_1_8041c284);
				_mapobj_data_touch_scale(0);
				if (event->userdata[1] < 25) {
					scale1 = intpl_sub(5, event->userdata[1] - 15, 10, float_0_8041c280, float_110_8041c28c);
				} else if (event->userdata[1] < 35) {
					scale1 = intpl_sub(11, event->userdata[1] - 25, 10, float_110_8041c28c, float_90_8041c290);
				} else if (event->userdata[1] < 40) {
					scale1 = intpl_sub(11, event->userdata[1] - 35, 5, float_90_8041c290, float_100_8041c294);
				}
				if (event->userdata[1] < 25) {
					scale2 = intpl_sub(5, event->userdata[1] - 15, 10, float_100_8041c294, float_90_8041c290);
				} else if (event->userdata[1] < 35) {
					scale2 = intpl_sub(11, event->userdata[1] - 25, 10, float_90_8041c290, float_110_8041c28c);
				} else if (event->userdata[1] < 40) {
					scale2 = intpl_sub(11, event->userdata[1] - 35, 5, float_110_8041c28c, float_100_8041c294);
				}
				scaleDiv = float_100_8041c294;
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 0) {
						mapObjScale(obj->data->name, scale2 / scaleDiv, scale1 / scaleDiv, float_1_8041c284);
						obj->mFlags |= 2;
					}
				}
			}
		}
	}
	if (mapGetMapObj(str_8041C288) != NULL) {
		if (event->userdata[1] >= 45) {
			mapObjScale(str_8041C288, float_1_8041c284, float_1_8041c284, float_1_8041c284);
			_mapobj_data_touch_scale(1);
		} else {
			ret = 1;
			if (event->userdata[1] < 20) {
				mapObjScale(str_8041C288, float_0_8041c280, float_0_8041c280, float_1_8041c284);
				_mapobj_data_touch_scale(1);
			} else {
				mapObjScale(str_8041C288, float_1_8041c284, float_1_8041c284, float_1_8041c284);
				_mapobj_data_touch_scale(1);
				if (event->userdata[1] < 30) {
					scale1 = intpl_sub(5, event->userdata[1] - 20, 10, float_0_8041c280, float_110_8041c28c);
				} else if (event->userdata[1] < 40) {
					scale1 = intpl_sub(11, event->userdata[1] - 30, 10, float_110_8041c28c, float_90_8041c290);
				} else if (event->userdata[1] < 45) {
					scale1 = intpl_sub(11, event->userdata[1] - 40, 5, float_90_8041c290, float_100_8041c294);
				}
				if (event->userdata[1] < 30) {
					scale2 = intpl_sub(5, event->userdata[1] - 20, 10, float_100_8041c294, float_90_8041c290);
				} else if (event->userdata[1] < 40) {
					scale2 = intpl_sub(11, event->userdata[1] - 30, 10, float_90_8041c290, float_110_8041c28c);
				} else if (event->userdata[1] < 45) {
					scale2 = intpl_sub(11, event->userdata[1] - 40, 5, float_110_8041c28c, float_100_8041c294);
				}
				scaleDiv = float_100_8041c294;
				for (i = 0; i < 32; i++) {
#ifdef MUST_MATCH
					obj2 = &wp->stageObjects[i];
					if (obj2->id > 0 && obj2->data->mLayer == 1) {
						mapObjScale(obj2->data->name, scale2 / scaleDiv, scale1 / scaleDiv, float_1_8041c284);
						obj2->mFlags |= 2;
					}
#else
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 1) {
						mapObjScale(obj->data->name, scale2 / scaleDiv, scale1 / scaleDiv, float_1_8041c284);
						obj->mFlags |= 2;
					}
#endif
				}
			}
		}
	}
	return ret ? 0 : 2;
}

