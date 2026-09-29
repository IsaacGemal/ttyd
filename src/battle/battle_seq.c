#include "battle/battle_seq.h"
#include "battle/battle.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
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

// Needed: -O4,p unrolls this loop 4x; the original is not unrolled.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Needed: -O4,p inlines btlseqInit into this function; the original calls it.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Needed: -O4,p drops stmw; the original saves r28-r31 with stmw.
#pragma optimize_for_size on
void BattleCheckAllPinchStatus(BattleWork* wp, BOOL a2) { //1:1, needs stmw forced
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
#pragma optimize_for_size off

// Needed: without dont_inline, -O4,p inlines this whole body into BattleSequenceManager.
#pragma dont_inline on
// Needed: -O4,p saves r30/r31 with two stw; the original uses stmw.
#pragma optimize_for_size on
void btlseqInit(BattleWork* wp) { //1:1, needs stmw
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
#pragma optimize_for_size off
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

// NONMATCHING (0.05%): empty placeholder, not decompiled yet.
void btlseqTurn(BattleWork* wp) {

}

// The three rule-message pointer tables are the 0x110-byte arrays the original addresses as
// lbl_80360FE0+0x40 (2bu), +0x150 (1bu) and +0x260 (after); one base register is shared.
// The pooled int->float double is a local @770 here instead of the original's shared
// double_to_int_802ef590 because this file's other int->float users are not written yet (MWCC then
// pools a local constant), and it lands at a different .rodata offset because the rule-message
// strings are also not in the file.
extern f32 lbl_80360FE0[];
extern GXColor dat_8041c26c;

// Needed: -O4,p saves r30/r31 with two individual stw; the original saves them with stmw.
#pragma optimize_for_size on
// The table loads use `add base,idx*4; lwz off(base)` where the original uses `addi base,off; lwzx`;
// the pooled int->float double is a local @770, not double_to_int_802ef590 (the file's other
// int->float users are not written yet); and MWCC swaps two temps (r6/r7). Control flow matches.
// NONMATCHING (92.77%): see the three differences above.
void _rule_disp(void) {
	u8* base = (u8*)lbl_80360FE0;
	BattleWork* wp = _battleWorkPointer;
	FieldBattleInfo* info = wp->fbatInfo;
	u16 lines;
	u16 width;
	f32 x;
	char buf[0x100];

	if (evtGetValue(NULL, 0xF5DE0180) < 0xAC) {
		if (evtGetValue(NULL, 0xF5DE03C4) < 0xB) {
			sprintf(buf, msgSearch(((const char**)(base + 0x150))[*(u8*)((u8*)info + 0x18)]),
				*(u8*)((u8*)info + 0x1a));
		} else {
			sprintf(buf, msgSearch(((const char**)(base + 0x40))[*(u8*)((u8*)info + 0x18)]),
				*(u8*)((u8*)info + 0x1a));
		}
	} else {
		sprintf(buf, msgSearch(((const char**)(base + 0x260))[*(u8*)((u8*)info + 0x18)]),
			*(u8*)((u8*)info + 0x1a));
	}
	width = FontGetMessageWidthLine(buf, &lines);
	lines++;
	x = float_0_8041c280 - (f32)(width >> 1);
	windowDispGX_Waku_col(0, dat_8041c26c, x - float_10_8041c2a0, float_94_8041c2a4,
		(f32)(width + 20), (f32)(lines * 29 + 3), float_10_8041c2a0);
	FontDrawStart();
	FontDrawMessage((s32)x, (s32)float_120_8041c2a8 - 29, buf);
}
#pragma optimize_for_size off

void btlseqEnd(BattleWork* wp) {

}

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

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

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

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

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

typedef struct PhaseEvtEntry {
	s32 unitIndex; //0x0
	s32 value;     //0x4
} PhaseEvtEntry;

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
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
#pragma optimize_for_size off

// Needed: -O4,p unrolls the constant-count loop 4x and drops stmw; the original does neither.
#pragma optimize_for_size on
// NONMATCHING (94.40%): logic matches; the original keeps wp in r31 and adds 0x120 to the loop offset
// (r0 = offset + 0x120; lwzx r0, wp, r0) while mine hoists order = wp + 0x120 into r31, and uses one
// fewer saved register (stmw r22 vs r23).
void battleSortPhaseMoveTable(BattleWork* wp, s32 mode) { //1:1
	s32* order = (s32*)((u8*)wp + 0x120);
	BattleUnit* a;
	BattleUnit* b;
	s32 count = mode;
	s32 i;
	s32 j;

	if (count <= 0) {
		count = 0;
		for (i = 0; i < 64; i++) {
			if (order[i] == -1) {
				break;
			}
			count++;
		}
	}

	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			f32 ax, ay, az;
			f32 bx, by, bz;
			a = BattleGetUnitPtr(wp, order[j]);
			b = BattleGetUnitPtr(wp, order[j + 1]);
			BtlUnit_GetHomePos(a, &ax, &ay, &az);
			BtlUnit_GetHomePos(b, &bx, &by, &bz);
			if (fabs(ax) > fabs(bx)) {
				s32 tmp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = tmp;
			}
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			a = BattleGetUnitPtr(wp, order[j]);
			b = BattleGetUnitPtr(wp, order[j + 1]);
			if (*(s32*)(a->field_0x27 + 1) < *(s32*)(b->field_0x27 + 1)) {
				s32 tmp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = tmp;
			}
		}
	}
}
#pragma optimize_for_size off

// NONMATCHING (94.43%): logic and field offsets match; callee-saved registers are rotated (wp r28 vs
// r27, ret r30 vs r31, offset r27 vs r30) so the whole body shifts by one register.
BOOL BattleCheckConcluded(BattleWork* wp) { //1:1
	BattleUnit* unit;
	s32 count;
	s32 i;
	s32 j;
	s32 offset;

	count = 0;
	offset = 0;
	for (i = 0; i < 3; i++, offset += sizeof(BattleAlliance)) {
		BattleAlliance* alliance = (BattleAlliance*)((u8*)wp->alliances + offset);
		alliance->clearConditionMet = 0;
		if ((u16)alliance->allianceId == 0) {
			count++;
			alliance->clearConditionMet = 1;
			continue;
		}
		if ((u16)alliance->allianceId & 1) {
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
		if ((u16)alliance->allianceId & 2) {
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
