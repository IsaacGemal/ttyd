// dont_inline: retail calls the file's small helpers (BattleWaitAllActiveEvtEnd and others) with `bl` from
// btlseqMove/BattleSequenceManager; without it MWCC inlines them.
#pragma dont_inline on
#include "battle/battle_seq.h"
#include "battle/battle.h"
#include "battle/battle_actrecord.h"
#include "battle/battle_sub.h"
#include "driver/mapdrv.h"
#include "battle/battle_unit.h"
#include "battle/battle_unit_event.h"
#include "battle/battle_damage_split.h"
#include "battle/battle_detect.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_mario.h"
#include "battle/battle_status_effect.h"
#include "driver/animdrv.h"
#include "driver/fadedrv.h"
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
#include "battle/battle_event_subset.h"
#include "battle/battle_event_default.h"
#include "battle/battle_break_slot.h"
#include "driver/dispdrv.h"
#include "eff/eff_recovery.h"
#include "eff/n64/eff_freeze_n64.h"

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
f32 float_2_8041c29c = 2.0f;
f32 float_10_8041c2a0 = 10.0f;
f32 float_94_8041c2a4 = 94.0f;
f32 float_120_8041c2a8 = 120.0f;
f32 float_900_8041c2ac = 900.0f;
f32 float_45_8041c2b0 = 45.0f;
f32 float_60_8041c2b4 = 60.0f;
f32 float_0p75_8041c2b8 = 0.75f;
const char str_802EF61C[] = "SFX_BTL_DANGER1";
const char str_802EF62C[] = "SFX_BTL_PINCH1";

extern BattleWeapon marioDefaultWeapon_Jump;
extern BattleWeapon marioWeapon_KururinJump;
extern BattleWeapon marioDefaultWeapon_Hammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackKaitenHammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackUltraHammer;
extern BattleWeapon partyWeapon_NokotarouFirstAttack;
extern BattleWeapon partyWeapon_SandersFirstAttack;
extern GXColor dat_8041c268;
extern const char str_802EF5DC[];
extern __declspec(section ".sdata") u32 nanika_luck_event[];
extern __declspec(section ".sdata") u32 _default_haikei_set_entry_event[];
extern const char str_802EF5C8[];
extern GXColor dat_8041c26c;
extern f32 lbl_80360FE0[];
typedef struct PhaseEvtEntry {
	s32 unitIndex; //0x0
	s32 value;     //0x4
} PhaseEvtEntry;

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
		case 4: // empty: the original's switch has a 5th case, producing its extra bound compare
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

void btlseqFirstAct(BattleWork* wp) {
	FieldBattleInfo* info = wp->fbatInfo;
	s32 party = info->partyId;
	NpcBattleInfo* setup;
	s32 i;
	BattleUnit* unit;
	BattleUnit* unit1;
	BattleWeapon* weapon;
	BattleUnit* unit2;
	EventEntry* evt;


	switch (BattleGetSeq(wp, SEQ_FIRST_ACT)) {
	case 0x2000000:
		switch (party) {
		case 0:
			*(s32*)wp->field_EFC = 0;
			evtEntry(_default_haikei_set_entry_event, 10, 0);
			setup = wp->fbatInfo->setup;
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit == NULL) {
					continue;
				}
				if (*(void**)unit->field_0x2B4 == NULL) {
					continue;
				}
				if (setup->field_4 & 0x00100000) {
					if (unit->currentKind == 0xDE) {
						continue;
					}
					if (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
						continue;
					}
				}
				if (marioGetPtr()->currMotionId == 0x1A && unit->currentKind == 0xDE) {
					evt = evtEntry(BtlUnit_GetData(unit, 0x41), 10, 0);
					*(s32*)(unit->field_0x2B4 + 4) = evt->eventId;
					evt->unitId = unit->unitId;
				} else {
					evt = evtEntry(*(void**)unit->field_0x2B4, 10, 0);
					*(s32*)(unit->field_0x2B4 + 4) = evt->eventId;
					evt->unitId = unit->unitId;
				}
			}
			BattleSetSeq(wp, 2, 0x2000001);
			break;
		default:
		switch (party) {
		case 1:
		case 2:
		case 3:
			*(s32*)wp->field_EFC = 10;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			*(s32*)wp->field_EFC = 0;
			break;
		default:
			*(s32*)wp->field_EFC = 0;
			break;
		}
		wp->flags |= 2;
		BattleGetFirstAttackUnit(wp, &unit1, &unit2);
		wp->activeUnitId = unit1->unitId;
		unit1->mTokenFlags |= 0x1000;
		evt = evtEntry(unit2->damageEventCode, 10, 0x20);
		if (evt != NULL) {
			evt->unitId = unit2->unitId;
			unit2->damageCode = 0x39;
			unit2->damageEventId = evt->eventId;
		}
		unit1->flags |= 0x04000000;
		if (party < 9) {
			weapon = _GetFirstAttackWeapon(party);
			*(void**)unit1->field_0x29C = weapon->mAttackEvtScript;
			*(s32*)unit1->field_0x2A8 = 0;
			BattleStatusWindowEventOff();
		}
		if (BtlUnit_GetBelong(unit1) == 0) {
			BattleAudience_Case_FirstAttack_Good();
		} else {
			BattleAudience_Case_FirstAttack_Bad();
		}
		evt = evtEntry(*(void**)unit1->field_0x29C, 10, 0);
		if (evt != NULL) {
			evt->unitId = unit1->unitId;
			unit1->attackEventId = evt->eventId;
		}
		switch (party) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			unit = BattleGetPartyPtr(wp);
			if (unit != NULL) {
				evt = evtEntry(*(void**)unit->field_0x2B4, 10, 0);
				*(s32*)(unit->field_0x2B4 + 4) = evt->eventId;
				evt->unitId = unit->unitId;
			}
			break;
		case 7:
		case 8:
			unit = BattleGetMarioPtr(wp);
			if (unit != NULL) {
				evt = evtEntry(*(void**)unit->field_0x2B4, 10, 0);
				*(s32*)(unit->field_0x2B4 + 4) = evt->eventId;
				evt->unitId = unit->unitId;
			}
			break;
		}
		BattleSetSeq(wp, 2, 0x2000001);
		break;
		}
		break;

	case 0x2000001:
		if (--*(s32*)wp->field_EFC > 0) {
			break;
		}
		fadeEntry(0x17, 0xC8, dat_8041c268);
		wp->field_19060 = psndSFXOn(str_802EF5DC);
		BattleSetSeq(wp, 2, 0x2000002);
		/* fallthrough */
	case 0x2000002:
		if (fadeIsFinish()) {
			if ((u32)wp->field_19060 != 0xFFFFFFFF) {
				psndSFXOff(wp->field_19060);
				wp->field_19060 = -1;
			}
		}
		if (BattleWaitAllActiveEvtEnd(wp)) {
			BattleStatusWindowEventOn();
		}
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		if (!fadeIsFinish()) {
			break;
		}
		BattleSetSeq(wp, 2, 0x2000003);
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		if (unit == NULL) {
			break;
		}
		unit->flags &= ~0x04000000;
		break;

	case 0x2000003:
		if (BattleCheckConcluded(wp)) {
			BattleAudience_PerAct();
			BattleSetSeq(wp, 0, 3);
			BattleSetSeq(wp, 7, 0x7000000);
			break;
		}
		BattleSetSeq(wp, 2, 0x2000004);
		BattleStatusChangeMsgAdjust(wp);
		/* fallthrough */
	case 0x2000004:
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
			break;
		}
		BattleIncSeq(wp, 2);
		/* fallthrough */
	case 0x2000005:
		if ((wp->flags & 0x30) == 0) {
			BattleStatusWindowSystemOn();
		}
		BattleConsumeReserveItem();
		wp->flags &= ~0x4;
		BattleAudience_PerAct();
		BattleIncSeq(wp, 2);
		/* fallthrough */
	case 0x2000006:
		if ((u8)BattleAudience_CheckReaction()) {
			break;
		}
		if (!BattleCheckConcluded(wp)) {
			if ((u8)BattleBreakSlot_CheckReaction()) {
				break;
			}
		}
		BattleIncSeq(wp, 2);
		/* fallthrough */
	case 0x2000007:
		if (BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) {
			break;
		}
		BattleIncSeq(wp, 2);
		/* fallthrough */
	case 0x2000008:
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		if (wp->fbatInfo->setup->field_4 & 0x10) {
			BattleSetSeq(wp, 2, 0x2000009);
			BattleStage_WallCloseCheck();
			break;
		}
		BattleSetSeq(wp, 2, 0x200000A);
		break;

	case 0x2000009: {
		s32* p = (s32*)((u8*)wp + 0x182A4);
		if (*p) {
			if (!evtCheckID(*p)) {
				*p = 0;
			}
		}
		if (*p != 0) {
			break;
		}
		BattleSetSeq(wp, 2, 0x200000A);
		break;
	}

	case 0x200000A: {
		BattleUnit* u;
		BOOL found;
		int j;
		*(s32*)(wp->field_18FF0 + 0x5C) = 0;
		found = FALSE;
		for (j = 0; j < 64; j++) {
			u = BattleGetUnitPtr(wp, j);
			if (u == NULL) {
				continue;
			}
			if (u->badgesEquipped.LuckyStart == 0) {
				continue;
			}
			if (BtlUnit_CheckStatus(u, 0x1B)) {
				continue;
			}
			found = TRUE;
		}
		if (found) {
			evt = evtEntry(nanika_luck_event, 10, 0);
			evt->unitId = -5;
			*(s32*)(wp->field_18FF0 + 0x5C) = evt->eventId;
			*(u64*)(wp->field_18C70 + 0x368) |= 0xFFFFFFFFFFFFFFFFULL;
			BattleStatusChangeMsgAdjust(wp);
			BattleSetSeq(wp, 2, 0x200000B);
			*(u16*)(wp->field_18FE1 + 1) = 0;
			break;
		}
		BattleSetSeq(wp, 2, 0x200000C);
		break;
	}

	case 0x200000B: {
		BOOL result;
		result = TRUE;
		if (*(u16*)(wp->field_18FE1 + 1) < 0x12C) {
			(*(u16*)(wp->field_18FE1 + 1))++;
		}
		if (*(s32*)(wp->field_18FF0 + 0x5C)) {
			if (evtCheckID(*(s32*)(wp->field_18FF0 + 0x5C))) {
				result = FALSE;
			} else {
				*(s32*)(wp->field_18FF0 + 0x5C) = 0;
			}
		}
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			result = FALSE;
		}
		if (BattleStatusChangeAnnouceMain(wp)) {
			result = FALSE;
		}
		if (BattleStatusChangeMsgMain(wp)) {
			result = FALSE;
		}
		if (result) {
			BattleSetSeq(wp, 2, 0x200000C);
		}
		break;
	}

	case 0x200000C:
		if (wp->flags & 2) {
			BattleSetSeq(wp, 2, 0x200000E);
			break;
		}
		BattleStage_NozzleWorkCheck(0);
		BattleSetSeq(wp, 2, 0x200000D);
		/* fallthrough */
	case 0x200000D:
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		BattleSetSeq(wp, 2, 0x200000E);
		/* fallthrough */
	case 0x200000E:
		wp->flags &= ~0x2;
		BattleSetSeq(wp, 0, 2);
		BattleSetSeq(wp, 3, 0x3000000);
		break;

	default:
		break;
	}
}

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

void btlseqTurn(BattleWork* wp) {
	BattleUnit* unit;
	s32 i;
	s8 turns;
	s8 strength;
	BOOL handled = FALSE;
	s32 recovered = 0;

	switch (BattleGetSeq(wp, SEQ_TURN)) {		case 0x3000000:
			BattleIncSeq(wp, SEQ_TURN);
			break;

		case 0x3000001:
			btlseqTurnFirstProcess(wp);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000003:
			if (wp->turnCount != 1) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else if (wp->fbatInfo->field_0x14[4] == 0) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				*(s32*)wp->field_EFC = 0x78;
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000004:
			if (*(s32*)wp->field_EFC != 0) {
				(*(s32*)wp->field_EFC)--;
			}
			if (*(s32*)wp->field_EFC <= 0 || BattlePadCheckTrigger(0x300)) {
				BattleIncSeq(wp, SEQ_TURN);
			} else {
				dispEntry(CAMERA_2D, float_900_8041c2ac, 1, (DispCallback)_rule_disp, NULL);
			}
			break;

		case 0x3000002:
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000005:
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000007:
		case 0x300000B:
		case 0x300000D:
		case 0x300000F:
		case 0x3000011:
		case 0x3000013:
		case 0x3000015:
		case 0x3000017:
		case 0x3000019:
		case 0x300001B:
		case 0x300001D:
		case 0x300001F:
		case 0x3000021:
		case 0x3000023:
		case 0x3000025:
		case 0x3000027:
		case 0x3000029:
		case 0x300002B:
		case 0x300002D:
		case 0x300002F:
		case 0x3000031:
		case 0x3000033:
		case 0x3000035:
		case 0x3000037:
		case 0x3000039:
		case 0x300003B:
		case 0x300003D:
		case 0x300003F:
		case 0x3000041:
		case 0x3000043:
		case 0x3000045:
		case 0x3000047:
		case 0x3000049:
			if (BattleWaitAllActiveEvtEnd(wp)) {
				if (BattleCheckConcluded(wp)) {
					BattleSetSeq(wp, SEQ_UNKNOWN, 3);
					BattleSetSeq(wp, SEQ_END, 0x7000000);
				} else {
					BattleIncSeq(wp, SEQ_TURN);
				}
			}
			break;

		case 0x3000012:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_SLEEP);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
			}
			break;

		case 0x3000014:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_STOP);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000016:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_DIZZY);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000018:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_CONFUSE);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300001A:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_ELECTRIC);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300001C:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_DODGY);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300001E:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_BURN);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000020: {
#ifdef MUST_MATCH
			// MUST_MATCH: three locals declared z, y, x take the original's Vec slots (sp+0x3c..0x44) and the
			// comma initializers keep pz/py/sfx in r27/r28/r29 (and the first recovery loop's counter in r25).
			// The natural form under #else uses a real Vec.
			f32 z;
			f32 y;
			f32 x;
			f32* pz = (0, &z);
			f32* py = (0, &y);
			const char* sfx = (0, str_802EF5AC);
#define BT_X x
#define BT_Y y
#define BT_Z z
#define BT_POS ((Vec*)&x)
#else
			Vec pos;
			f32* pz = &pos.z;
			f32* py = &pos.y;
			const char* sfx = str_802EF5AC;
#define BT_X pos.x
#define BT_Y pos.y
#define BT_Z pos.z
#define BT_POS (&pos)
#endif
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (BtlUnit_CheckRecoveryStatus(unit, STATUS_FREEZE)) {
					if (!BtlUnit_CheckShadowGuard(unit)) {
						BattleDamageDirect(-5, unit, 0, 1, 0, 0x11a, 0, 1);
					}
					BtlUnit_GetPos(unit, &BT_X, py, pz);
					BT_Y += (f32)BtlUnit_GetHeight(unit);
					effFreezeN64Entry(0, 1, BT_X, BT_Y, BT_Z, float_1_8041c284);
					psndSFXOn_3D(sfx, BT_POS);
					handled = TRUE;
				}
			}
#undef BT_X
#undef BT_Y
#undef BT_Z
#undef BT_POS
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;
		}

		case 0x3000022:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_HUGE);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000024:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_TINY);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000026:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_POISON);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000028:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_ATTACK_UP);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300002A:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_ATTACK_DOWN);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300002C:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_DEFENSE_UP);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300002E:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_DEFENSE_DOWN);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000030:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				result = BtlUnit_CheckRecoveryStatus(unit, STATUS_ALLERGIC);
				recovered |= result;
				if (result) {
					EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
					if (event) {
						unit->attackEventId = event->eventId;
						event->unitId = unit->unitId;
					}
					handled = TRUE;
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000032:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_HP_REGEN);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000034:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_FP_REGEN);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000036:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_INVISIBLE);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000038:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_FAST);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300003A:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_SLOW);
					recovered |= result;
					if (result) {
						EventEntry* event;
						BtlUnit_ResetMoveStatus(unit);
						event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300003C:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_PAYBACK);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300003E:
			for (i = 0; i < 64; i++) {
				s32 result;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					result = BtlUnit_CheckRecoveryStatus(unit, STATUS_HOLD_FAST);
					recovered |= result;
					if (result) {
						EventEntry* event = evtEntry(subsetevt_status_recovery, 0xa, 0);
						if (event) {
							unit->attackEventId = event->eventId;
							event->unitId = unit->unitId;
						}
						handled = TRUE;
					}
				}
			}
			if (recovered) {
				psndSFXOn(str_802EF598);
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000006:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, STATUS_POISON, &turns, &strength);
					if (strength > 0 && !BtlUnit_CheckShadowGuard(unit)) {
						handled = TRUE;
						BattleDamageDirect(-5, unit, 0, strength, 0, 0x117, 0, 1);
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000008:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, STATUS_BURN, &turns, &strength);
					if (strength > 0) {
						handled = TRUE;
						break;
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				*(u16*)wp->field_2 = 0;
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000009:
			switch (*(u16*)wp->field_2) {
				case 0:
					(*(u16*)wp->field_2)++;
					for (i = 0; i < 64; i++) {
						unit = BattleGetUnitPtr(wp, i);
						if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
							unit->currentHp > 0) {
							BtlUnit_GetStatus(unit, STATUS_BURN, &turns, &strength);
							if (strength > 0) {
								BSE_TurnFirstProcessEffectEntry(unit, 1);
							}
						}
					}
					// falls through
				case 1: {
					BOOL done = TRUE;
					for (i = 0; i < 64; i++) {
						unit = BattleGetUnitPtr(wp, i);
						if (unit != NULL && BSE_TurnFirstProcessEffectMain(unit)) {
							done = FALSE;
						}
					}
					if (done) {
						(*(u16*)wp->field_2)++;
					}
					break;
				}
				default:
					if (BattleWaitAllActiveEvtEnd(wp)) {
						if (BattleCheckConcluded(wp)) {
							BattleSetSeq(wp, SEQ_UNKNOWN, 3);
							BattleSetSeq(wp, SEQ_END, 0x7000000);
						} else {
							BattleIncSeq(wp, SEQ_TURN);
						}
					}
					break;
			}
			break;

		case 0x300000A:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, STATUS_HP_REGEN, &turns, &strength);
					if (strength > 0) {
						if (!(unit->mAttributeFlags & 0x40000000)) {
							unit->hpGauge.field_0xC = unit->currentHp;
							BtlUnit_RecoverHp(unit, strength);
							unit->hpGauge.field_0x10 = unit->currentHp;
							unit->hpGauge.field_0x4 = unit->hpGauge.field_0x6;
							unit->hpGauge.field_0x8 = unit->hpGauge.field_0xA;
							unit->hpGauge.field_0x2 |= 1;
						} else {
							BtlUnit_RecoverHp(unit, strength);
						}
						{
							f32 x, y, z;
							BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
							BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
							y += unit->mSizeMultiplier * (f32)(BtlUnit_GetHeight(unit) + 5);
							effRecoveryEntry(0, strength, x, y, z);
						}
						{
							EventEntry* event = evtEntry(subsetevt_regeneration, 0xa, 0);
							if (event) {
								unit->attackEventId = event->eventId;
								event->unitId = unit->unitId;
							}
						}
						handled = TRUE;
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300000C:
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) &&
					unit->currentHp > 0) {
					BtlUnit_GetStatus(unit, STATUS_FP_REGEN, &turns, &strength);
					if (strength > 0) {
						BtlUnit_RecoverFp(unit, strength);
						{
							f32 x, y, z;
							BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
							BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
							y += unit->mSizeMultiplier * (f32)(BtlUnit_GetHeight(unit) + 5);
							effRecoveryEntry(1, strength, x, y, z);
						}
						{
							EventEntry* event = evtEntry(subsetevt_regeneration, 0xa, 0);
							if (event) {
								unit->attackEventId = event->eventId;
								event->unitId = unit->unitId;
							}
						}
						handled = TRUE;
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x300000E:
			if (wp->turnCount <= 1) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
				break;
			}
			for (i = 0; i < 64; i++) {
				s32 count;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) || unit->currentHp <= 0) {
						continue;
					}
					count = 0;
					{
						s32 j = unit->badgesEquipped.HappyHeart;
						s32 chance;
						while (j > 0) {
							chance = 33 - (unit->currentHp / unit->maxHp) * 3;
							if (irand(100) < chance) {
								count++;
							}
							j--;
						}
					}
					if (count > 0) {
						unit->currentHp += count;
						if (unit->currentHp > unit->maxHp) {
							unit->currentHp = unit->maxHp;
						}
						{
							f32 x, y, z;
							BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
							BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
							y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit);
							effRecoveryEntry(0, count, x, y, z);
						}
						{
							EventEntry* event = evtEntry(subsetevt_happy_heart, 0xa, 0);
							if (event) {
								unit->attackEventId = event->eventId;
								event->unitId = unit->unitId;
							}
						}
						handled = TRUE;
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
			}
			break;

		case 0x3000010:
			if (wp->turnCount <= 1) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
				break;
			}
			for (i = 0; i < 64; i++) {
				s32 j;
				s32 count;
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) || unit->currentHp <= 0) {
						continue;
					}
					j = unit->badgesEquipped.HappyFlower;
					count = 0;
					{
						s32 chance;
						while (j > 0) {
							chance = 33 - (unit->currentFp / unit->maxFp) * 3;
							if (irand(100) < chance) {
								count++;
							}
							j--;
						}
					}
					if (count > 0) {
						BtlUnit_RecoverFp(unit, count);
						{
							f32 x, y, z;
							BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
							BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
							y += unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit);
							effRecoveryEntry(1, count, x, y, z);
						}
						{
							EventEntry* event = evtEntry(subsetevt_happy_flower, 0xa, 0);
							if (event) {
								unit->attackEventId = event->eventId;
								event->unitId = unit->unitId;
							}
						}
						handled = TRUE;
					}
				}
			}
			if (!handled) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000040:
			if (*(s32*)&wp->field_18FF0[0x48] != 0) {
				*(s32*)&wp->field_18FF0[0x48] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000042:
			if (*(s32*)&wp->field_18FF0[0x4C] != 0) {
				*(s32*)&wp->field_18FF0[0x4C] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000044:
			if (*(s32*)&wp->field_18FF0[0x50] != 0) {
				*(s32*)&wp->field_18FF0[0x50] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000046:
			if (*(s32*)&wp->field_18FF0[0x54] != 0) {
				*(s32*)&wp->field_18FF0[0x54] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000048:
			if (*(s32*)&wp->field_18FF0[0x58] != 0) {
				*(s32*)&wp->field_18FF0[0x58] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x300004A:
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x300004B:
			BattleIncSeq(wp, SEQ_TURN);
			BattleSetSeq(wp, SEQ_PHASE, 0x4000000);
			break;

		case 0x300004C:
			btlseqPhase(wp);
			break;

		case 0x300004D:
			BattleAudience_Case_TurnEnd();
			BattleSetSeq(wp, SEQ_TURN, 0x3000001);
			wp->flags &= ~0x2000;
			break;
	}
}

// The three rule-message pointer tables are the 0x110-byte arrays the original addresses as
// lbl_80360FE0+0x40 (2bu), +0x150 (1bu) and +0x260 (after); one base register is shared.
// The pooled int->float double is a local @770 here instead of the original's shared
// double_to_int_802ef590 because this file's other int->float users are not written yet (MWCC then
// pools a local constant), and it lands at a different .rodata offset because the rule-message
// strings are also not in the file.

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

// NONMATCHING (99.93%): only the case 0x5000006 loop index differs (r27 here, r25 in the original; it
// wants a separate colour from the shared case-4/case-8 counter `i`). The other two diffs are the
// jump-table relocation, which needs btlseqAct/btlseqTurn written before the table lands at its offset.
void btlseqMove(BattleWork* wp) {
	BattleUnit* unit;
	s32 j;
	s32 i;
	BOOL changed;

	do {
		changed = FALSE;
		switch (BattleGetSeq(wp, SEQ_MOVE)) {
		case 0x5000000:
			btlseqPhaseFirstProcess(wp);
			BattleIncSeq(wp, SEQ_MOVE);
			changed = TRUE;
			break;
		case 0x5000001:
			BattleIncSeq(wp, SEQ_MOVE);
			changed = TRUE;
			break;
		case 0x5000002: {
			PouchData* pouch = pouchGetPtr();
			BattleUnit* mario = BattleGetMarioPtr(wp);
			if (pouch->mMerleeNextCurseType != 2 ||
				BattleGetSeq(wp, SEQ_PHASE) != 0x4000004)
			{
				BattleIncSeq(wp, SEQ_MOVE);
				BattleIncSeq(wp, SEQ_MOVE);
				changed = TRUE;
				break;
			}
			wp->field_18FE1[0] = 2;
			BattleMajinaiDone();
			BattleMajinaiEndCheck();
			{
				EventEntry* event = evtEntry(marioAttackEvent_MajinaiDefenceUp, 0xa, 0);
				event->unitId = mario->unitId;
				mario->attackEventId = event->eventId;
			}
			BattleIncSeq(wp, SEQ_MOVE);
			// falls through
		}
		case 0x5000003:
			if (BattleWaitAllActiveEvtEnd(wp)) {
				BattleIncSeq(wp, SEQ_MOVE);
				changed = TRUE;
			}
			break;
		case 0x5000004: {
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL && unit->unisonPhaseEventCode != NULL) {
					BattleRunPhaseEvent(unit, TRUE);
				}
			}
			BattleIncSeq(wp, SEQ_MOVE);
			changed = TRUE;
			break;
		}
		case 0x5000005:
			if (BattleWaitAllActiveEvtEnd(wp)) {
				BattleIncSeq(wp, SEQ_MOVE);
				changed = TRUE;
			}
			break;
		case 0x5000006: {
			s32 id;
			for (j = 0; j < 64; j++) {
				id = *(s32*)(wp->field_120 + 0x100 + j * 8);
				if (id == -1) {
					continue;
				}
				unit = BattleGetUnitPtr(wp, id);
				if (unit != NULL && unit->phaseEventCode != NULL) {
					break;
				}
			}
			if (j < 64) {
				BattleRunPhaseEvent(unit, FALSE);
				*(s32*)(wp->field_120 + 0x100 + j * 8) = -1;
				BattleIncSeq(wp, SEQ_MOVE);
			}
			else {
				BattleSetSeq(wp, SEQ_MOVE, 0x500000A);
				changed = TRUE;
			}
			break;
		}
		case 0x5000007:
			if (BattleWaitAllActiveEvtEnd(wp)) {
				if (BattleCheckConcluded(wp)) {
					BattleSetSeq(wp, SEQ_UNKNOWN, 3);
					BattleSetSeq(wp, SEQ_END, 0x7000000);
				}
				else {
					BattleSetSeq(wp, SEQ_MOVE, 0x5000006);
					changed = TRUE;
				}
			}
			break;
		case 0x5000008: {
			s32 id;
			PouchData* pouch;
			memset(&wp->impendingBonuses, 0, sizeof(StarPowerInfo));
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					unit->mTokenFlags &= 0x80002000;
					unit->totalHpDamageTaken = 0;
					unit->hpDamageTaken = 0;
					unit->hpDamageCount = 0;
					((s8*)unit->field_0x26E)[1] = 0;
					*(s32*)unit->field_0x264 = 0;
					unit->fpDamageTaken = 0;
					*(s32*)(unit->field_0x264 + 4) = 0;
					((s8*)unit->field_0x26E)[0] = 0;
					*(s32*)unit->field_0x27C = 0;
				}
			}
			for (i = 0; i < 64; i++) {
				id = *(s32*)(wp->field_120 + i * 4);
				if (id == -1) {
					continue;
				}
				unit = BattleGetUnitPtr(wp, id);
				if (unit == NULL) {
					continue;
				}
				if (unit->movesRemaining <= 0) {
					continue;
				}
				{
					s32 moveState = unit->moveState;
					if (moveState == 10) {
						continue;
					}
					if (BtlUnit_CheckStatus(unit, 0x1B) && !(unit->mAttributeFlags & 0x10000)) {
						continue;
					}
					if (moveState != 1) {
						break;
					}
				}
				unit->moveState = 0;
			}
			wp->activeUnitId = id;
			unit = BattleGetUnitPtr(wp, id);
			unit->mTokenFlags |= 0x1000;
			BattleIncSeq(wp, SEQ_MOVE);
			BattleSetSeq(wp, SEQ_ACT, 0x6000000);
			for (i = 0; i < 64; i++) {
				id = *(s32*)(wp->field_120 + i * 4);
				if (id == -1) {
					continue;
				}
				unit = BattleGetUnitPtr(wp, id);
				if (unit == NULL) {
					continue;
				}
				if (unit->moveState != 1) {
					continue;
				}
				unit->moveState = 0;
			}
			unit = BattleGetUnitPtr(wp, wp->activeUnitId);
			if (unit->currentKind != 0xDE) {
				break;
			}
			pouch = pouchGetPtr();
			wp->flags |= 0x600;
			for (i = 0; i < MAX_BADGES; i++) {
				if (pouch->equippedBadges[i] >= 0xF0 && pouch->equippedBadges[i] < 0xF7) {
					wp->flags &= ~0x200;
					break;
				}
			}
			if (pouch->jumpLevel >= 2) {
				wp->flags &= ~0x200;
			}
			for (i = 0; i < MAX_BADGES; i++) {
				if (pouch->equippedBadges[i] >= 0xF7 && pouch->equippedBadges[i] < 0xFE) {
					wp->flags &= ~0x400;
					break;
				}
			}
			if (pouch->hammerLevel >= 2) {
				wp->flags &= ~0x400;
			}
			break;
		}
		case 0x5000009:
			btlseqAct(wp);
			break;
		case 0x500000A: {
			if (BattleCheckConcluded(wp)) {
				BattleSetSeq(wp, SEQ_UNKNOWN, 3);
				BattleSetSeq(wp, SEQ_END, 0x7000000);
				break;
			}
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					BtlUnit_SetTotalHitDamage(unit, 0);
					unit->mTokenFlags &= ~0x1000;
				}
			}
			{
			BOOL end = BattlePhaseEndCheck();
			if (wp->flags & 0x2000) {
				end = TRUE;
			}
			if (end) {
				if (wp->field_18FE1[0] == 2) {
					wp->field_18FE1[0] = 0;
				}
				BattleAudience_PerPhase(BattleGetSeq(wp, SEQ_PHASE));
				BattleIncSeq(wp, SEQ_MOVE);
			}
			else {
				BattleSetSeq(wp, SEQ_MOVE, 0x5000008);
			}
			}
			break;
		}
		case 0x500000B:
			if ((u8)BattleAudience_CheckReactionPerPhase() == 0 && BattleWaitAllActiveEvtEnd(wp)) {
				BattleIncSeq(wp, SEQ_MOVE);
			}
			break;
		case 0x500000C: {
			if ((BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) == 0) {
				BattleIncSeq(wp, SEQ_PHASE);
				BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
			}
			break;
		}
		case 0x500000D:
			BattleIncSeq(wp, SEQ_MOVE);
			break;
		case 0x500000E:
			BattleIncSeq(wp, SEQ_PHASE);
			BattleSetSeq(wp, SEQ_MOVE, 0x5000000);
			break;
		}
	} while (changed);
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

// Raw offsets (fields that battle_unit.h leaves in padding, so no named member exists):
// 0x258, 0x280, 0x29C, 0x2A0, 0x2A8, 0x2C, 0x2C4, 0x2C8, 0x2CC.
// Case 0 shadows `unit` so MWCC keeps it in r29 (the outer `unit` is r31 elsewhere); the original splits
// them the same way. The moveState switch lists case 8 before the shared case 7/case 9 so the bodies come
// out in the target's order, and the currentKind check is a switch so the then-block lands before the else.
void btlseqAct(BattleWork* wp) {
	BOOL v;
	BattleUnit* unit;
	EventEntry* event;
	f32 speed;

	switch (BattleGetSeq(wp, SEQ_ACT)) {
	case 0x6000000: {
		BattleUnit* unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		effNiceAsync(1);
		*(s32*)((u8*)unit + 0x280) = 0;
		*(s32*)((u8*)unit + 0x2CC) = 0;
		BattleAudience_ActInit();
		v = TRUE;
		switch (unit->currentKind) {
		case 0xDE:
			if (!BtlUnit_CheckStatus(unit, 1) && !BtlUnit_CheckStatus(unit, 2) &&
				!BtlUnit_CheckStatus(unit, 9) && !BtlUnit_CheckStatus(unit, 0x1B)) {
				v = FALSE;
			}
			break;
		default:
			if (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
				if (!BtlUnit_CheckStatus(unit, 1) && !BtlUnit_CheckStatus(unit, 2) &&
					!BtlUnit_CheckStatus(unit, 9) && !BtlUnit_CheckStatus(unit, 0x1B)) {
					v = FALSE;
				}
			}
			break;
		}
		if (v) {
			BattleSetSeq(wp, SEQ_ACT, 0x6000003);
			break;
		}
		if (unit->mAttributeFlags & 0x10) {
			BattleSetSeq(wp, SEQ_ACT, 0x6000004);
			break;
		}
		if (unit->currentKind == 0xE4) {
			if (BattleGetMarioPtr(wp)->mAttributeFlags & 0x10) {
				BattleSetSeq(wp, SEQ_ACT, 0x6000004);
				break;
			}
		}
		if (unit->flippedTurns != 0) {
			BattleSetSeq(wp, SEQ_ACT, 0x6000005);
			break;
		}
		BattleCommandInit(wp);
		BattleSetSeq(wp, SEQ_ACT, 0x6000001);
		break;
	}

	case 0x6000001:
		speed = battleCameraGetPosMoveSpeed();
		if (speed > float_2_8041c29c) {
			break;
		}
		BattleSetSeq(wp, SEQ_ACT, 0x6000002);
		// fallthrough
	case 0x6000002:
		if (BattleCommandInput(wp)) {
			wp->flags &= ~0x80;
			BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		}
		break;

	case 0x6000005:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		*(void**)((u8*)unit + 0x29C) = BtlUnit_GetData(unit, 2);
		*(s32*)((u8*)unit + 0x2A8) = 1;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000003:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		*(s32*)((u8*)unit + 0x2A8) = 1;
		unit->flags |= 0x04000000;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000004:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		*(void**)((u8*)unit + 0x29C) = BtlUnit_GetData(unit, 3);
		*(s32*)((u8*)unit + 0x2A8) = 0x1B;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000006: {
		if ((*(s32*)((u8*)BattleGetUnitPtr(wp, wp->activeUnitId) + 0x2A8) & 1) == 0) {
			BattleStatusWindowSystemOff();
		}
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		event = NULL;
		if (wp->flags & 0x40000000) {
			if (!(unit->mAttributeFlags & 0x2000) &&
				!(*(s32*)((u8*)unit + 0x2A8) & 8) && !(*(s32*)((u8*)unit + 0x2A8) & 0x10)) {
				event = evtEntry(btldefaultevt_CantMoveZeroGravity, 0xA, 0);
				goto act_done;
			}
		}
		if (*(s32*)((u8*)unit + 0x2A8) & 0x10) {
			if (*(void**)((u8*)unit + 0x29C) == NULL) {
				goto act_done;
			}
			event = evtEntry(*(void**)((u8*)unit + 0x29C), 0xA, 0);
			goto act_done;
		}
		if (*(s32*)((u8*)unit + 0x2A8) & 2) {
			event = evtEntry(*(void**)((u8*)unit + 0x29C), 0xA, 0);
			goto act_done;
		}
		if (BtlUnit_CheckStatus(unit, 1) || BtlUnit_CheckStatus(unit, 2) ||
			BtlUnit_CheckStatus(unit, 9)) {
			goto act_done;
		}
		if (BtlUnit_CheckStatus(unit, 0x1B) && !(unit->mAttributeFlags & 0x10000)) {
			goto act_done;
		}
		if (BtlUnit_CheckStatus(unit, 6)) {
			if (irand(100) < 0) {
				event = evtEntry(btldefaultevt_BiribiriMove, 0xA, 0);
				goto act_done;
			}
		}
		if (BtlUnit_CheckStatus(unit, 5)) {
			if (irand(100) < 0x32) {
				unit->mTokenFlags |= 0x10;
				event = evtEntry(*(void**)((u8*)unit + 0x2A0), 0xA, 0);
				goto act_done;
			}
		}
		event = evtEntry(*(void**)((u8*)unit + 0x29C), 0xA, 0);
	act_done:
		if (event != NULL) {
			unit->attackEventId = event->eventId;
			event->unitId = unit->unitId;
			if (unit->waitEventId != 0) {
				evtDeleteID(unit->waitEventId);
				unit->waitEventId = 0;
			}
		}
		BattleIncSeq(wp, SEQ_ACT);
		break;
	}

	case 0x6000007:
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		BattleIncSeq(wp, SEQ_ACT);
		BattleStatusChangeMsgAdjust(wp);
		// fallthrough
	case 0x6000008:
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		if ((BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) != 0) {
			break;
		}
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	case 0x6000009: {
		if ((wp->flags & 0x30) == 0) {
			BattleStatusWindowSystemOn();
		}
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		if (unit != NULL) {
			unit->flags &= ~0x04000000;
			if (unit->mTokenFlags & 8) {
				BtlUnit_SetStatus(unit, 0x10, 1, -0x63);
				unit->mTokenFlags &= ~8;
			}
			if (wp->field_18FE1[0] == 1) {
				wp->field_18FE1[0] = 0;
			}
			*(s32*)((u8*)unit + 0x258) = 0;
			switch (unit->moveState) {
			case 5:
				unit->moveState = 6;
				break;
			case 6:
				unit->moveState = 7;
				break;
			case 8:
				unit->moveState = 9;
				break;
			case 7:
			case 9:
				unit->moveState = 0;
				break;
			}
			if (unit->moveState == 0) {
				BOOL flag = TRUE;
				if (wp->flags & 0x2000) {
					flag = TRUE;
				}
				if (flag) {
					if (BtlUnit_CheckStatusFlag(unit, 0x02000000)) {
						BtlUnit_OffStatusFlag(unit, 0x02000000);
					} else {
						unit->movesRemaining--;
					}
					if (wp->flags & 0x2000) {
						unit->movesRemaining = 0;
					}
					if (unit->movesRemaining <= 0) {
						unit->moveState = 10;
						if (!(unit->mTokenFlags & 0x2000)) {
							unit->activeTurns = 0;
						} else {
							unit->mTokenFlags &= ~0x2000;
						}
						if (BtlUnit_GetBelong(unit) == 0) {
							if (*(s32*)((u8*)unit + 0x2C) == 0x4000002) {
								if (!BattlePhaseEndCheck() && !(wp->flags & 0x2000)) {
									unit->mMoveColorLv = 0x60;
								}
							}
						}
					}
				}
			} else if (unit->moveState == 3) {
				unit->moveState = 0;
			} else if (unit->moveState == 4) {
				unit->moveState = 0;
			}
		}
		BattleConsumeReserveItem();
		wp->flags &= ~4;
		BattleAudience_PerAct();
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	}
	case 0x600000A:
		if ((u8)BattleAudience_CheckReaction() != 0) {
			break;
		}
		if (!BattleCheckConcluded(wp) && (u8)BattleBreakSlot_CheckReaction() != 0) {
			break;
		}
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	case 0x600000B:
		if ((BattleStatusChangeAnnouceMain(wp) | BattleStatusChangeMsgMain(wp)) != 0) {
			break;
		}
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	case 0x600000C: {
		if (!BattleWaitAllActiveEvtEnd(wp)) {
			break;
		}
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		if (unit != NULL) {
			*(s32*)((u8*)unit + 0x2C8) = 0;
			if (*(void**)((u8*)unit + 0x2C4) != NULL) {
				event = evtEntry(*(void**)((u8*)unit + 0x2C4), 0xA, 0);
				if (event != NULL) {
					*(s32*)((u8*)unit + 0x2C8) = event->eventId;
					event->unitId = unit->unitId;
					if (unit->waitEventId != 0) {
						evtDeleteID(unit->waitEventId);
						unit->waitEventId = 0;
					}
				}
			}
		}
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	}
	case 0x600000D:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		if (unit != NULL && *(s32*)((u8*)unit + 0x2C8) != 0) {
			if (evtCheckID(*(s32*)((u8*)unit + 0x2C8))) {
				break;
			}
			*(s32*)((u8*)unit + 0x2C8) = 0;
		}
		BattleIncSeq(wp, SEQ_ACT);
		// fallthrough
	case 0x600000E:
		if (BattleWaitAllActiveEvtEnd(wp)) {
			BattleIncSeq(wp, SEQ_MOVE);
		}
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
	*(s32*)(wp->field_18C70 + 0x36C) = 0;
	*(s32*)(wp->field_18C70 + 0x368) = 0;
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

// NONMATCHING (99.19%): all instructions match except the count/wp callee-saved pair is swapped
// (target count r30/wp r31, compiler picks count r31/wp r30). Declaration order, raw vs header-field
// offsets, 128 pragma combos, compilers 1.3-2.7, inline helpers, f32/Vec/array temps and loop forms tried.
// Rechecked: every GC compiler 1.3-2.7, optimization_level 0-4, opt_lifetimes/opt_loop_invariants/
// peephole/scheduling pairs, wp/count aliases (u8* casts, selective per-loop use, 1-6 deep), extra locals,
// algebraic rewrites, inline swap/count helpers, function reordering and -fp_contract off all keep the
// same swap; the generated instruction sequence is otherwise byte-for-byte identical.
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

// NONMATCHING (99.55%): the four event-id guards use while(...){...;break;} for the original's check-after-body
// layout, and the local `void* p` (copied from wp before the loop) fixes the prologue scheduling. Only the
// wp/i callee-saved pair is still swapped (we get wp r28/i r30; the original has wp r30/i r28).
s32 BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp) { //1:1
	s32 i;
	BOOL result = TRUE;
	BattleUnit* unit;
	void* p;

	p = wp;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr((BattleWork*)p, i);
		if (unit) {
			while (unit->phaseEventId) {
				if (evtCheckID(unit->phaseEventId)) {
					result = FALSE;
				}
				else {
					unit->phaseEventId = 0;
				}
				break;
			}
			while (unit->attackEventId) {
				if (evtCheckID(unit->attackEventId)) {
					result = FALSE;
				}
				else {
					unit->attackEventId = 0;
				}
				break;
			}
			while (unit->damageEventId) {
				if (evtCheckID(unit->damageEventId)) {
					result = FALSE;
				}
				else {
					unit->damageEventId = 0;
				}
				break;
			}
			while (*(s32*)(unit->field_0x2B4 + 4)) {
				if (evtCheckID(*(s32*)(unit->field_0x2B4 + 4))) {
					result = FALSE;
				}
				else {
					*(s32*)(unit->field_0x2B4 + 4) = 0;
				}
				break;
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
#pragma dont_inline reset
