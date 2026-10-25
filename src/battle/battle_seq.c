#pragma pool_data on // the original addresses its .rodata and .data through one base register
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
#include "math_ext.h"
#include <stdio.h>
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_event_default.h"
#include "battle/battle_break_slot.h"
#include "driver/dispdrv.h"
#include "eff/eff_recovery.h"
#include "eff/n64/eff_freeze_n64.h"
#include "battle/battle_event_cmd.h"
#include "evt/evt_cmd.h"

// math.h's fabs is the __fabs intrinsic (math_ext.h instead of math.h: math.h's sqrt inline emits two
// doubles into .rodata), as in system.c.
#ifdef __MWERKS__
#define fabs(x) __fabs(x)
#endif

// This unit's .data: the float pair that opens it, the header pairs (local `$` statics in the original, so
// static here) and then the rule-message tables. Nothing names the pairs, so force_active keeps them.
#pragma force_active on
__declspec(section ".data") f32 lbl_80360FE0[2] = { -1.0f, 1.0f };
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80360FF0[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_80361000[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80361010[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
#pragma force_active reset

extern BattleWork* _battleWorkPointer;

// Weapon stat table for ITEM_LUCKY_START; _set_effect_luck copies it. It is the first .rodata object, so
// pool_data reaches the unit's strings from it.
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

const GXColor dat_8041c268 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_8041c26c = { 0xFF, 0xFF, 0xFF, 0xFF };

const char* _rule_msg_table_2bu[68] = {
    "a", "stg3_tou_cp_001", "a", "stg3_tou_cp_002",
    "a", "stg3_tou_cp_004", "stg3_tou_cp_008", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "stg3_tou_cp_000", "a",
    "a", "a", "a", "a",
    "a", "a", "stg3_tou_cp_005", "a",
    "a", "a", "a", "a",
    "stg3_tou_cp_006", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_009",
    "a", "a", "a", "a",
    "stg3_tou_cp_003", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_007",
    "a", "a", "a", "a",
    "a", "a", "a", "a",
};

const char* _rule_msg_table_1bu[68] = {
    "test", "stg3_tou_cp_011", "a", "stg3_tou_cp_012",
    "a", "stg3_tou_cp_014", "stg3_tou_cp_019", "a",
    "a", "a", "a", "stg3_tou_cp_020",
    "a", "a", "a", "a",
    "a", "a", "stg3_tou_cp_010", "a",
    "stg3_tou_cp_018", "a", "a", "a",
    "a", "a", "stg3_tou_cp_015", "a",
    "a", "a", "a", "a",
    "stg3_tou_cp_016", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_021",
    "a", "a", "a", "a",
    "stg3_tou_cp_013", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_017",
    "a", "a", "a", "stg3_tou_cp_024",
    "stg3_tou_cp_023", "stg3_tou_cp_022", "a", "a",
};

const char* _rule_msg_table_after[68] = {
    "a", "stg3_tou_cp_041", "a", "stg3_tou_cp_042",
    "a", "stg3_tou_cp_044", "stg3_tou_cp_049", "a",
    "a", "a", "a", "stg3_tou_cp_050",
    "a", "a", "a", "a",
    "a", "a", "stg3_tou_cp_040", "a",
    "stg3_tou_cp_048", "a", "a", "a",
    "a", "a", "stg3_tou_cp_045", "a",
    "a", "a", "a", "a",
    "stg3_tou_cp_046", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_051",
    "a", "a", "a", "a",
    "stg3_tou_cp_043", "a", "a", "a",
    "a", "a", "a", "a",
    "a", "a", "a", "stg3_tou_cp_047",
    "a", "a", "a", "stg3_tou_cp_054",
    "stg3_tou_cp_053", "stg3_tou_cp_052", "a", "a",
};


extern BattleWeapon marioDefaultWeapon_Jump;
extern BattleWeapon marioWeapon_KururinJump;
extern BattleWeapon marioDefaultWeapon_Hammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackKaitenHammer;
extern BattleWeapon marioDefaultWeapon_FirstAttackUltraHammer;
extern BattleWeapon partyWeapon_NokotarouFirstAttack;
extern BattleWeapon partyWeapon_SandersFirstAttack;
// clang-format off
__declspec(section ".sdata") EVT_BEGIN(nanika_luck_event)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(60)
    END_SIBLING_EVENT
    CALL(_set_effect_luck)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    RETURN
EVT_END()

__declspec(section ".sdata") EVT_BEGIN(_default_haikei_set_entry_event)
    CALL(_set_haikei_entry_scale)
    RETURN
EVT_END()
// clang-format on

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
			if (wp->commandMenu.menuFlags & 1) {
				psndSFXOn("SFX_BTL_DANGER1");
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
				if (wp->commandMenu.menuFlags & 1) {
					psndSFXOn("SFX_BTL_PINCH1");
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
			if (animEffectAsync("p_b_st", 1) &&
				animEffectAsync("p_jyabara", 1) &&
				animEffectAsync("p_kaiten_h", 1) &&
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
			wp->seqWaitFrames = 0;
			evtEntry(_default_haikei_set_entry_event, 10, 0);
			setup = wp->fbatInfo->setup;
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit == NULL) {
					continue;
				}
				if (unit->entryEventCode == NULL) {
					continue;
				}
				if (setup->flags & 0x00100000) {
					if (unit->currentKind == 0xDE) {
						continue;
					}
					if (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
						continue;
					}
				}
				if (marioGetPtr()->currMotionId == 0x1A && unit->currentKind == 0xDE) {
					evt = evtEntry(BtlUnit_GetData(unit, 0x41), 10, 0);
					unit->entryEventId = evt->eventId;
					evt->unitId = unit->unitId;
				} else {
					evt = evtEntry(unit->entryEventCode, 10, 0);
					unit->entryEventId = evt->eventId;
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
			wp->seqWaitFrames = 10;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			wp->seqWaitFrames = 0;
			break;
		default:
			wp->seqWaitFrames = 0;
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
			unit1->attackEventCode = weapon->mAttackEvtScript;
			unit1->attackEventFlags = 0;
			BattleStatusWindowEventOff();
		}
		if (BtlUnit_GetBelong(unit1) == 0) {
			BattleAudience_Case_FirstAttack_Good();
		} else {
			BattleAudience_Case_FirstAttack_Bad();
		}
		evt = evtEntry(unit1->attackEventCode, 10, 0);
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
				evt = evtEntry(unit->entryEventCode, 10, 0);
				unit->entryEventId = evt->eventId;
				evt->unitId = unit->unitId;
			}
			break;
		case 7:
		case 8:
			unit = BattleGetMarioPtr(wp);
			if (unit != NULL) {
				evt = evtEntry(unit->entryEventCode, 10, 0);
				unit->entryEventId = evt->eventId;
				evt->unitId = unit->unitId;
			}
			break;
		}
		BattleSetSeq(wp, 2, 0x2000001);
		break;
		}
		break;

	case 0x2000001:
		if (--wp->seqWaitFrames > 0) {
			break;
		}
		fadeEntry(0x17, 0xC8, dat_8041c268);
		wp->curtainSfxId = psndSFXOn("SFX_OFF_BTL_START_CURTAIN_UP1");
		BattleSetSeq(wp, 2, 0x2000002);
		/* fallthrough */
	case 0x2000002:
		if (fadeIsFinish()) {
			if ((u32)wp->curtainSfxId != 0xFFFFFFFF) {
				psndSFXOff(wp->curtainSfxId);
				wp->curtainSfxId = -1;
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
			BattleSetSeq(wp, SEQ_END, SEQ_END_INIT);
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
		if (BattleAudience_CheckReaction()) {
			break;
		}
		if (!BattleCheckConcluded(wp)) {
			if (BattleBreakSlot_CheckReaction()) {
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
		if (wp->fbatInfo->setup->flags & NPC_BATTLE_NO_ESCAPE) {
			BattleSetSeq(wp, 2, 0x2000009);
			BattleStage_WallCloseCheck();
			break;
		}
		BattleSetSeq(wp, 2, 0x200000A);
		break;

	case 0x2000009: {
		s32* p = &wp->stageObjectWork.closeWallEventId;
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
		wp->nanikaLuckEventId = 0;
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
			wp->nanikaLuckEventId = evt->eventId;
			wp->statusChangeMsg.mAnnouncedFlags |= 0xFFFFFFFFFFFFFFFFULL;
			BattleStatusChangeMsgAdjust(wp);
			BattleSetSeq(wp, 2, 0x200000B);
			wp->field_0x18FE2 = 0;
			break;
		}
		BattleSetSeq(wp, 2, 0x200000C);
		break;
	}

	case 0x200000B: {
		BOOL result;
		result = TRUE;
		if (wp->field_0x18FE2 < 0x12C) {
			wp->field_0x18FE2++;
		}
		if (wp->nanikaLuckEventId) {
			if (evtCheckID(wp->nanikaLuckEventId)) {
				result = FALSE;
			} else {
				wp->nanikaLuckEventId = 0;
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
	const char* sfxName = (0, "SFX_SYSTEM_LUCKY1");
#else
	BattleWeapon* weaponPtr = &weapon;
	f32* pz = &z;
	f32* py = &y;
	const char* sfxName = "SFX_SYSTEM_LUCKY1";
#endif
	BattleUnit* unit;
	BattleUnitPart* part;
	s32 i;
	EffectEntry* eff;
	NiceEffectWork* nice;
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
		eff = effNiceEntry(6, x - 45.0f, 60.0f + y, z);
		nice = eff->userdata;
		nice->scale = 0.75f;
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
			} else if (wp->fbatInfo->ruleKind == 0) {
				BattleIncSeq(wp, SEQ_TURN);
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			} else {
				wp->seqWaitFrames = 0x78;
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000004:
			if (wp->seqWaitFrames != 0) {
				(wp->seqWaitFrames)--;
			}
			if (wp->seqWaitFrames <= 0 || BattlePadCheckTrigger(0x300)) {
				BattleIncSeq(wp, SEQ_TURN);
			} else {
				dispEntry(CAMERA_2D, 900.0f, 1, (DispCallback)_rule_disp, NULL);
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
					BattleSetSeq(wp, SEQ_END, SEQ_END_INIT);
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
			const char* sfx = (0, "SFX_CONDITION_BREAK_ICE1");
#define BT_X x
#define BT_Y y
#define BT_Z z
#define BT_POS ((Vec*)&x)
#else
			Vec pos;
			f32* pz = &pos.z;
			f32* py = &pos.y;
			const char* sfx = "SFX_CONDITION_BREAK_ICE1";
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
					effFreezeN64Entry(0, 1, BT_X, BT_Y, BT_Z, 1.0f);
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				psndSFXOn("SFX_REST_CONDITION1");
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
				wp->burnEffectStep = 0;
				BattleIncSeq(wp, SEQ_TURN);
				btlseqTurn(wp);
			}
			break;

		case 0x3000009:
			switch (wp->burnEffectStep) {
				case 0:
					(wp->burnEffectStep)++;
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
						(wp->burnEffectStep)++;
					}
					break;
				}
				default:
					if (BattleWaitAllActiveEvtEnd(wp)) {
						if (BattleCheckConcluded(wp)) {
							BattleSetSeq(wp, SEQ_UNKNOWN, 3);
							BattleSetSeq(wp, SEQ_END, SEQ_END_INIT);
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
							unit->hpGauge.fromHp = unit->currentHp;
							BtlUnit_RecoverHp(unit, strength);
							unit->hpGauge.toHp = unit->currentHp;
							unit->hpGauge.drainTimer = unit->hpGauge.drainFrames;
							unit->hpGauge.dispTimer = unit->hpGauge.dispFrames;
							unit->hpGauge.flags |= 1;
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
			if (wp->cmdDisable[0] != 0) {
				wp->cmdDisable[0] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000042:
			if (wp->cmdDisable[1] != 0) {
				wp->cmdDisable[1] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000044:
			if (wp->cmdDisable[2] != 0) {
				wp->cmdDisable[2] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000046:
			if (wp->cmdDisable[3] != 0) {
				wp->cmdDisable[3] -= 1;
			}
			BattleIncSeq(wp, SEQ_TURN);
			BattleIncSeq(wp, SEQ_TURN);
			btlseqTurn(wp);
			break;

		case 0x3000048:
			if (wp->cmdDisable[4] != 0) {
				wp->cmdDisable[4] -= 1;
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

// The centre `cx` and the text line `y` are locals in the original: written as literals, MWCC folds
// `0.0f - w` into fneg and `(s32)120.0f` into a constant, where the original loads both at their use.
// Using _battleWorkPointer->fbatInfo directly, rather than a wp temp, gives the original's prologue order.
void _rule_disp(void) {
	FieldBattleInfo* info = _battleWorkPointer->fbatInfo;
	GXColor color;
	u16 lines;
	u16 width;
	f32 x;
	f32 cx = 0.0f;
	f32 y = 120.0f;
	char buf[0x100];

	if (evtGetValue(NULL, 0xF5DE0180) < 0xAC) {
		if (evtGetValue(NULL, 0xF5DE03C4) < 0xB) {
			sprintf(buf, msgSearch(_rule_msg_table_1bu[info->ruleKind]), info->ruleCount);
		} else {
			sprintf(buf, msgSearch(_rule_msg_table_2bu[info->ruleKind]), info->ruleCount);
		}
	} else {
		sprintf(buf, msgSearch(_rule_msg_table_after[info->ruleKind]), info->ruleCount);
	}
	width = FontGetMessageWidthLine(buf, &lines);
	lines++;
	color = dat_8041c26c;
	x = cx - (f32)(width >> 1);
#ifdef MUST_MATCH
	((void (*)(s32, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &color, x - 10.0f,
		94.0f, (f32)(width + 20), (f32)(lines * 29 + 3), 10.0f);
#else
	windowDispGX_Waku_col(x - 10.0f, 94.0f, (f32)(width + 20), (f32)(lines * 29 + 3), 10.0f, 0, color);
#endif
	FontDrawStart();
	FontDrawMessage((s32)x, (s32)y - 29, buf);
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

// Case 8's `id` and `moveState` are declared at function scope ahead of `j`, so the case 0x5000006 loop
// index is numbered below them and colours last, sharing moveState's register as in the original.
void btlseqMove(BattleWork* wp) {
	BattleUnit* unit;
	s32 i;
	s32 id;
	s32 moveState;
	s32 j;
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
			wp->merleeCurseType = 2;
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
				id = wp->phaseEvtTable[j].unitIndex;
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
				wp->phaseEvtTable[j].unitIndex = -1;
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
					BattleSetSeq(wp, SEQ_END, SEQ_END_INIT);
				}
				else {
					BattleSetSeq(wp, SEQ_MOVE, 0x5000006);
					changed = TRUE;
				}
			}
			break;
		case 0x5000008: {
			PouchData* pouch;
			memset(&wp->impendingBonuses, 0, sizeof(StarPowerInfo));
			for (i = 0; i < 64; i++) {
				unit = BattleGetUnitPtr(wp, i);
				if (unit != NULL) {
					unit->mTokenFlags &= 0x80002000;
					unit->totalHpDamageTaken = 0;
					unit->hpDamageTaken = 0;
					unit->hpDamageCount = 0;
					unit->field_0x26F = 0;
					unit->field_0x264 = 0;
					unit->fpDamageTaken = 0;
					unit->field_0x268 = 0;
					unit->field_0x26E = 0;
					unit->hitCount = 0;
				}
			}
			for (i = 0; i < 64; i++) {
				id = wp->unitIdTable[i];
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
					moveState = unit->moveState;
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
				id = wp->unitIdTable[i];
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
				BattleSetSeq(wp, SEQ_END, SEQ_END_INIT);
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
				if (wp->merleeCurseType == 2) {
					wp->merleeCurseType = 0;
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
			if (BattleAudience_CheckReactionPerPhase() == 0 && BattleWaitAllActiveEvtEnd(wp)) {
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
		s32 id = wp->unitIdTable[i];
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
		unit->damageDealt = 0;
		unit->superguardEventCode = NULL;
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
		if (speed > 2.0f) {
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
		unit->attackEventCode = BtlUnit_GetData(unit, 2);
		unit->attackEventFlags = 1;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000003:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		unit->attackEventFlags = 1;
		unit->flags |= 0x04000000;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000004:
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		unit->attackEventCode = BtlUnit_GetData(unit, 3);
		unit->attackEventFlags = 0x1B;
		BattleSetSeq(wp, SEQ_ACT, 0x6000006);
		break;

	case 0x6000006: {
		if ((BattleGetUnitPtr(wp, wp->activeUnitId)->attackEventFlags & 1) == 0) {
			BattleStatusWindowSystemOff();
		}
		unit = BattleGetUnitPtr(wp, wp->activeUnitId);
		event = NULL;
		if ((wp->flags & 0x40000000) && !(unit->mAttributeFlags & 0x2000) &&
			!(unit->attackEventFlags & 8) && !(unit->attackEventFlags & 0x10)) {
			event = evtEntry(btldefaultevt_CantMoveZeroGravity, 0xA, 0);
		} else if (unit->attackEventFlags & 0x10) {
			if (unit->attackEventCode != NULL) {
				event = evtEntry(unit->attackEventCode, 0xA, 0);
			}
		} else if (unit->attackEventFlags & 2) {
			event = evtEntry(unit->attackEventCode, 0xA, 0);
		} else if (!(BtlUnit_CheckStatus(unit, 1) || BtlUnit_CheckStatus(unit, 2) ||
			BtlUnit_CheckStatus(unit, 9)) &&
			!(BtlUnit_CheckStatus(unit, 0x1B) && !(unit->mAttributeFlags & 0x10000))) {
			if (BtlUnit_CheckStatus(unit, 6) && irand(100) < 0) {
				event = evtEntry(btldefaultevt_BiribiriMove, 0xA, 0);
			} else if (BtlUnit_CheckStatus(unit, 5) && irand(100) < 0x32) {
				unit->mTokenFlags |= 0x10;
				event = evtEntry(unit->confusionEventCode, 0xA, 0);
			} else {
				event = evtEntry(unit->attackEventCode, 0xA, 0);
			}
		}
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
			if (wp->merleeCurseType == 1) {
				wp->merleeCurseType = 0;
			}
			unit->field_0x258 = 0;
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
							if (unit->seqPhase == 0x4000002) {
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
		if (BattleAudience_CheckReaction() != 0) {
			break;
		}
		if (!BattleCheckConcluded(wp) && BattleBreakSlot_CheckReaction() != 0) {
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
			unit->actEndEventId = 0;
			if (unit->actEndEventCode != NULL) {
				event = evtEntry(unit->actEndEventCode, 0xA, 0);
				if (event != NULL) {
					unit->actEndEventId = event->eventId;
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
		if (unit != NULL && unit->actEndEventId != 0) {
			if (evtCheckID(unit->actEndEventId)) {
				break;
			}
			unit->actEndEventId = 0;
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
	wp->statusChangeMsg.mAnnouncedFlags = 0;
	BtlActRec_AddCount(&wp->actRecord[0x3B - 0x1C]);

	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit) {
			unit->activeTurns++;
			BtlUnit_ResetMoveStatus(unit);
			kind = unit->mKindParams;
			unit->turnOrder = kind->turnOrder
				+ irand(kind->turnOrderVariance)
				- kind->turnOrderVariance / 2;
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

	wp->commandMenu.restoreId = 0;
	wp->commandMenu.restoreState = 0;
	for (i = 0; i < 64; i++) {
		wp->unitIdTable[i] = -1;
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
			if (unit->seqPhase != BattleGetSeq(wp, SEQ_PHASE)) {
				continue;
			}
			if (unit->mAttributeFlags & 0x80000) {
				continue;
			}
			wp->unitIdTable[count] = unit->unitId;
			unit->protectId = -1;
			part = unit->mParts;
			while (part) {
				if (part->poseId != -1) {
					part->poseSound.flag = 1;
				}
				part = part->mNextPart;
			}
			count++;
		}
	}
	battleSortPhaseMoveTable(wp, count);
	battleMakePhaseEvtTable(wp);
}

// The work pointer is copied into its own local and copy propagation is off for this function: the
// original keeps that copy as a separate web, which takes r31 ahead of `count` (r30).
#pragma opt_propagation off
void battleSortPhaseMoveTable(BattleWork* wp, s32 count) { //1:1
	BattleWork* work = wp;
	s32 i;
	s32 j;
	BattleUnit* a;
	BattleUnit* b;

	if (count <= 0) {
		count = 0;
		for (i = 0; i < 64; i++) {
			if (work->unitIdTable[i] == -1) {
				break;
			}
			count++;
		}
	}

	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			f32 ax, ay, az;
			f32 bx, by, bz;
			a = BattleGetUnitPtr(work, work->unitIdTable[j]);
			b = BattleGetUnitPtr(work, work->unitIdTable[j + 1]);
			BtlUnit_GetHomePos(a, &ax, &ay, &az);
			BtlUnit_GetHomePos(b, &bx, &by, &bz);
			if (fabs(ax) > fabs(bx)) {
				s32 tmp = work->unitIdTable[j];
				work->unitIdTable[j] = work->unitIdTable[j + 1];
				work->unitIdTable[j + 1] = tmp;
			}
		}
	}
	for (i = 0; i < count - 1; i++) {
		for (j = 0; j < count - i - 1; j++) {
			a = BattleGetUnitPtr(work, work->unitIdTable[j]);
			b = BattleGetUnitPtr(work, work->unitIdTable[j + 1]);
			if (a->turnOrder < b->turnOrder) {
				s32 tmp = work->unitIdTable[j];
				work->unitIdTable[j] = work->unitIdTable[j + 1];
				work->unitIdTable[j + 1] = tmp;
			}
		}
	}
}
#pragma opt_propagation reset

void battleMakePhaseEvtTable(BattleWork* wp) { //1:1
	PhaseEvtEntry* table = wp->phaseEvtTable;
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
			if (x >= 0.0f) {
				table[count].value = (s32)(1000.0f + x);
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
	BattleWorkField180C0* p = &wp->stageObjectWork;
	s32 ret = BattleWaitAllActiveEvtEnd_NoBgSetEndWait(wp);

	if (!ret) {
		return ret;
	}
	if (p->nozzleWorkEventId) {
		if (evtCheckID(p->nozzleWorkEventId)) {
			return FALSE;
		}
		p->nozzleWorkEventId = 0;
	}
	if (p->fallFrameEventId) {
		if (evtCheckID(p->fallFrameEventId)) {
			return FALSE;
		}
		p->fallFrameEventId = 0;
	}
	if (p->objectTurnEventId) {
		if (evtCheckID(p->objectTurnEventId)) {
			return FALSE;
		}
		p->objectTurnEventId = 0;
	}
	if (p->objectFallEventId) {
		if (evtCheckID(p->objectFallEventId)) {
			return FALSE;
		}
		p->objectFallEventId = 0;
	}
	if (p->closeWallEventId) {
		if (evtCheckID(p->closeWallEventId)) {
			return FALSE;
		}
		p->closeWallEventId = 0;
	}
	return TRUE;
}

// The four event-id guards use while(...){...;break;} for the original's check-after-body layout, and the
// local `void* p` (copied from wp before the loop, declared first) gives wp its own web ranked above
// `result` and `i`, as in the original (wp r30, result r29, i r28).
s32 BattleWaitAllActiveEvtEnd_NoBgSetEndWait(BattleWork* wp) { //1:1
	void* p;
	BOOL result = TRUE;
	s32 i;
	BattleUnit* unit;

	p = wp;
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(p, i);
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
			while (unit->entryEventId) {
				if (evtCheckID(unit->entryEventId)) {
					result = FALSE;
				}
				else {
					unit->entryEventId = 0;
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

BOOL BattleCheckConcluded(BattleWork* wp) { //1:1
	BattleUnit* unit;
	s32 count;
	s32 i;
	s32 j;

	count = 0;
	for (i = 0; i < 3; i++) {
		wp->alliances[i].clearConditionMet = 0;
		if (wp->alliances[i].allianceId == 0) {
			count++;
			wp->alliances[i].clearConditionMet = 1;
			continue;
		}
		if (wp->alliances[i].allianceId & 1) {
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
				wp->alliances[i].clearConditionMet = 1;
				continue;
			}
		}
		if (wp->alliances[i].allianceId & 2) {
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
				wp->alliances[i].clearConditionMet = 1;
			}
		}
	}
	if (count >= 2) {
		return TRUE;
	}
	return (wp->flags & 0x30) != 0;
}

//local prototypes

void _mapobj_data_touch_scale(s32 kind) { //1:1
	BattleWork* wp = _battleWorkPointer;
	s32 i;

	for (i = 0; i < 32; i++) {
		BattleStageObject* obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == kind) {
			obj->mFlags |= BSO_FLAG_DIRTY;
		}
	}
}

// EVT command (default_haikei_set_entry_event): 60-frame delay, then animates the A1/A2
// background objects in over 15/20 frames. Returns 2 once both have finished (0 while animating).
s32 _set_haikei_entry_scale(EventEntry* event, BOOL isFirstCall) {
	BattleWork* wp = _battleWorkPointer;
	s32 ret = 0;
	f32 scale2, scale1, scaleDiv;
	BattleStageObject* obj;
	s32 i;

	if (isFirstCall) {
		event->userdata[0] = 60;
		event->userdata[1] = 0;
		if (mapGetMapObj("A1") != NULL) {
			mapObjScale("A1", 0.0f, 0.0f, 1.0f);
		}
		if (mapGetMapObj("A2") != NULL) {
			mapObjScale("A2", 0.0f, 0.0f, 1.0f);
		}
	}
	if (event->userdata[0] > 0) {
		event->userdata[0]--;
		if (event->userdata[0] > 0) {
			if (mapGetMapObj("A1") != NULL) {
				mapObjScale("A1", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(0);
			}
			if (mapGetMapObj("A2") != NULL) {
				mapObjScale("A2", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(1);
			}
			return 0;
		}
	}
	event->userdata[1]++;
	if (mapGetMapObj("A1") != NULL) {
		if (event->userdata[1] >= 40) {
			mapObjScale("A1", 1.0f, 1.0f, 1.0f);
			_mapobj_data_touch_scale(0);
		} else {
			ret = 1;
			if (event->userdata[1] < 15) {
				mapObjScale("A1", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(0);
			} else {
				mapObjScale("A1", 1.0f, 1.0f, 1.0f);
				_mapobj_data_touch_scale(0);
				if (event->userdata[1] < 25) {
					scale1 = intpl_sub(5, event->userdata[1] - 15, 10, 0.0f, 110.0f);
				} else if (event->userdata[1] < 35) {
					scale1 = intpl_sub(11, event->userdata[1] - 25, 10, 110.0f, 90.0f);
				} else if (event->userdata[1] < 40) {
					scale1 = intpl_sub(11, event->userdata[1] - 35, 5, 90.0f, 100.0f);
				}
				if (event->userdata[1] < 25) {
					scale2 = intpl_sub(5, event->userdata[1] - 15, 10, 100.0f, 90.0f);
				} else if (event->userdata[1] < 35) {
					scale2 = intpl_sub(11, event->userdata[1] - 25, 10, 90.0f, 110.0f);
				} else if (event->userdata[1] < 40) {
					scale2 = intpl_sub(11, event->userdata[1] - 35, 5, 110.0f, 100.0f);
				}
				scaleDiv = 100.0f;
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 0) {
						mapObjScale(obj->data->name, scale2 / scaleDiv, scale1 / scaleDiv, 1.0f);
						obj->mFlags |= BSO_FLAG_DIRTY;
					}
				}
			}
		}
	}
	if (mapGetMapObj("A2") != NULL) {
		if (event->userdata[1] >= 45) {
			mapObjScale("A2", 1.0f, 1.0f, 1.0f);
			_mapobj_data_touch_scale(1);
		} else {
			ret = 1;
			if (event->userdata[1] < 20) {
				mapObjScale("A2", 0.0f, 0.0f, 1.0f);
				_mapobj_data_touch_scale(1);
			} else {
				mapObjScale("A2", 1.0f, 1.0f, 1.0f);
				_mapobj_data_touch_scale(1);
				if (event->userdata[1] < 30) {
					scale1 = intpl_sub(5, event->userdata[1] - 20, 10, 0.0f, 110.0f);
				} else if (event->userdata[1] < 40) {
					scale1 = intpl_sub(11, event->userdata[1] - 30, 10, 110.0f, 90.0f);
				} else if (event->userdata[1] < 45) {
					scale1 = intpl_sub(11, event->userdata[1] - 40, 5, 90.0f, 100.0f);
				}
				if (event->userdata[1] < 30) {
					scale2 = intpl_sub(5, event->userdata[1] - 20, 10, 100.0f, 90.0f);
				} else if (event->userdata[1] < 40) {
					scale2 = intpl_sub(11, event->userdata[1] - 30, 10, 90.0f, 110.0f);
				} else if (event->userdata[1] < 45) {
					scale2 = intpl_sub(11, event->userdata[1] - 40, 5, 110.0f, 100.0f);
				}
				scaleDiv = 100.0f;
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == 1) {
						mapObjScale(obj->data->name, scale2 / scaleDiv, scale1 / scaleDiv, 1.0f);
						obj->mFlags |= BSO_FLAG_DIRTY;
					}
				}
			}
		}
	}
	return ret ? 0 : 2;
}
