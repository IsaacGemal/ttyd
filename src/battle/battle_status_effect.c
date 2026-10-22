#include "manager/fontmgr.h"
#include "battle/battle_status_effect.h"
#include "driver/windowdrv.h"
#include "battle/battle_unit.h"
#include "battle/battle.h"
#include "driver/icondrv.h"
#include <string.h>
#include "eff/eff_sleep.h"
#include "battle/battle_damage.h"
#include "battle/battle_sub.h"
#include "driver/msgdrv.h"
#include "eff/eff_fire.h"
#include "eff/eff_ice.h"
#include "eff/eff_spark.h"


//.sdata
//Status Type Priority Data
s8 _st_pri_data[] = {
	STATUS_CHARGE, //charge is elevated
	STATUS_POISON, //poison is elevated
	STATUS_SLEEP,
	STATUS_STOP,
	STATUS_DIZZY,
	STATUS_CONFUSE,
	STATUS_ELECTRIC,
	STATUS_DODGY,
	STATUS_BURN,
	STATUS_FREEZE,
	STATUS_HUGE,
	STATUS_TINY,
	STATUS_ATTACK_UP,
	STATUS_ATTACK_DOWN,
	STATUS_DEFENSE_UP,
	STATUS_DEFENSE_DOWN,
	STATUS_ALLERGIC,
	STATUS_INVISIBLE,
	STATUS_FAST,
	STATUS_SLOW,
	STATUS_PAYBACK,
	STATUS_HOLD_FAST,
	STATUS_HP_REGEN,
	STATUS_FP_REGEN,
	-1
};

//.data
typedef struct BattleStatusChangeMsgData {
	s8 type;             // 0x0
	u8 pad_0x1[3];       // 0x1
	const char* mJapaneseName;   // 0x4
	const char* mMessageName;    // 0x8
	const char* mMessageNameAlt; // 0xC
} BattleStatusChangeMsgData;

BattleStatusChangeMsgData _st_chg_msg_data[] = {
	{ STATUS_POISON, { 0 }, "\202\307\202\255", "msg_st_chg_poison", "msg_st_chg_poison" },
	{ STATUS_CHARGE, { 0 }, "\203`\203\203\201[\203W", "msg_st_chg_charge", "msg_st_chg_charge3" },
	{ STATUS_SLEEP, { 0 }, "\202\313\202\336\202\350", "msg_st_chg_sleep", "msg_st_chg_sleep" },
	{ STATUS_STOP, { 0 }, "\203X\203g\203b\203v", "msg_st_chg_stan", "msg_st_chg_stan" },
	{ STATUS_DIZZY, { 0 }, "\203O\203\213\203O\203\213", "msg_st_chg_guruguru", "msg_st_chg_guruguru" },
	{ STATUS_CONFUSE, { 0 }, "\202\261\202\361\202\347\202\361", "msg_st_chg_confusion", "msg_st_chg_confusion" },
	{ STATUS_ELECTRIC, { 0 }, "\203r\203\212\203r\203\212", "msg_st_chg_biribiri", "msg_st_chg_biribiri" },
	{ STATUS_DODGY, { 0 }, "\203\210\203P\203\210\203P", "msg_st_chg_dodge", "msg_st_chg_dodge" },
	{ STATUS_BURN, { 0 }, "\202\320\202\276\202\351\202\334", "msg_st_chg_fire", "msg_st_chg_fire" },
	{ STATUS_FREEZE, { 0 }, "\202\261\202\250\202\350\202\303\202\257", "msg_st_chg_freeze", "msg_st_chg_freeze" },
	{ STATUS_HUGE, { 0 }, "\203f\203J\203f\203J", "msg_st_chg_big", "msg_st_chg_big" },
	{ STATUS_TINY, { 0 }, "\203~\203j\203~\203j", "msg_st_chg_small", "msg_st_chg_small" },
	{ STATUS_ATTACK_UP, { 0 }, "\202\302\202\346\202\302\202\346", "msg_st_chg_tuyotuyo", "msg_st_chg_tuyotuyo" },
	{ STATUS_ATTACK_DOWN, { 0 }, "\202\346\202\355\202\346\202\355", "msg_st_chg_yowayowa", "msg_st_chg_yowayowa" },
	{ STATUS_DEFENSE_UP, { 0 }, "\203J\203`\203J\203`", "msg_st_chg_hard", "msg_st_chg_hard" },
	{ STATUS_DEFENSE_DOWN, { 0 }, "\203t\203j\203\203\203t\203j\203\203", "msg_st_chg_soft", "msg_st_chg_soft" },
	{ STATUS_ALLERGIC, { 0 }, "\203A\203\214\203\213\203M\201[", "msg_st_chg_allergy", "msg_st_chg_allergy" },
	{ STATUS_INVISIBLE, { 0 }, "\202\306\202\244\202\337\202\242", "msg_st_chg_trans", "msg_st_chg_trans" },
	{ STATUS_FAST, { 0 }, "\202\315\202\342\202\242", "msg_st_chg_quick", "msg_st_chg_quick" },
	{ STATUS_SLOW, { 0 }, "\202\314\202\353\202\242", "msg_st_chg_slow", "msg_st_chg_slow" },
	{ STATUS_PAYBACK, { 0 }, "\202\265\202\251\202\246\202\265", "msg_st_chg_counter", "msg_st_chg_counter" },
	{ STATUS_HOLD_FAST, { 0 }, "\203{\203\200\203J\203E\203\223\203^\201[", "msg_st_chg_bombcounter", "msg_st_chg_bombcounter" },
	{ STATUS_HP_REGEN, { 0 }, "\202\266\202\355\202\266\202\355\202g\202o", "msg_st_chg_reg_hp", "msg_st_chg_reg_hp" },
	{ STATUS_FP_REGEN, { 0 }, "\202\266\202\355\202\266\202\355\202e\202o", "msg_st_chg_reg_fp", "msg_st_chg_reg_fp" },
	{ -1, { 0 }, "", "", "" },
};

//local prototypes

// additional local prototypes (targets not yet declared in shared headers)

typedef struct BattleEffectPos {
	u32 mType; // 0x0
	Vec position;  // 0x4
} BattleEffectPos;

typedef struct BattleEffectPosScale {
	u32 mType;      // 0x0
	Vec position;   // 0x4
	f32 mIceScale;  // 0x10
	u8 pad_0x14[0x24 - 0x14];
	f32 mFireScale; // 0x24
} BattleEffectPosScale;

void BattleStatusEffectInit(BattleUnit* unit) {
	; //stubbed in retail
}

void BattleStatusEffectMain(BattleUnit* unit) {
	BSE_Sleep(unit);
	BSE_Biribiri(unit);
	BSE_Fire(unit);
	BSE_Freeze(unit);
	BSE_Kagegakure(unit);
}

void BattleStatusEffectDelete(BattleUnit* unit) {
	BSE_SleepDelete(unit);
	BSE_BiribiriDelete(unit);
	BSE_FireDelete(unit);
	BSE_FreezeDelete(unit);
	BSE_KagegakureDelete(unit);
}

void BSE_Sleep(BattleUnit* unit) {
	BattleEffectPosScale* work;
	BattleUnitPart* part;
	f32 x, y, z;
	f32 scale;
	f32 heightScale;
	BOOL active = FALSE;

	if (BtlUnit_CheckStatus(unit, STATUS_SLEEP)) {
		active = TRUE;
	}
	if (active) {
		if (unit->mSleepEff == NULL) {
			unit->mSleepEff = effSleepEntry(0, 0, -1000.0f, -1000.0f, -1000.0f, 1.0f, 60.0f);
		}
		work = unit->mSleepEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		scale = 0.5f * (unit->mSizeMultiplier * (f32)BtlUnit_GetWidth(unit));
		heightScale = 0.95f * (unit->mSizeMultiplier * (f32)BtlUnit_GetHeight(unit));
		if (unit->mAttributeFlags & kCeiling) {
			heightScale = 0.0f;
		}
		x += scale;
		y += heightScale;
		work->position.x = x;
		work->position.y = y;
		work->position.z = z;
	} else {
		BSE_SleepDelete(unit);
	}
}

void BSE_SleepDelete(BattleUnit* unit) {
	EffectEntry* effect = unit->mSleepEff;

	if (effect != NULL) {
		effDelete(effect);
		unit->mSleepEff = NULL;
	}
}

void BSE_Biribiri(BattleUnit* unit) {
	BattleEffectPos* work;
	BattleUnitPart* part;
	f32 x, y, z;
	f32 heightScale;
	BOOL active = FALSE;

	if (BtlUnit_CheckStatus(unit, STATUS_ELECTRIC)) {
		active = TRUE;
	}
	if (active) {
		if (unit->mBiribiriEff == NULL) {
			unit->mBiribiriEff = effSparkEntry(0, 10, -1000.0f, -1000.0f, -1000.0f, (f32)(BtlUnit_GetWidth(unit) / 2));
		}
		work = unit->mBiribiriEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		heightScale = (f32)(BtlUnit_GetHeight(unit) / 2) * unit->mSizeMultiplier;
		if (unit->mAttributeFlags & kCeiling) {
			heightScale = 0.0f;
		}
		y += heightScale;
		work->position.x = x;
		work->position.y = y;
		work->position.z = z + 10.0f;
	} else {
		BSE_BiribiriDelete(unit);
	}
}

void BSE_BiribiriDelete(BattleUnit* unit) {
	EffectEntry* effect = unit->mBiribiriEff;

	if (effect != NULL) {
		effDelete(effect);
		unit->mBiribiriEff = NULL;
	}
}

void BSE_Fire(BattleUnit* unit) {
	BattleEffectPos* work;
	BattleUnitPart* part;
	f32 x, y, z;
	f32 direction;
	BOOL active = FALSE;

	if (BtlUnit_CheckStatus(unit, STATUS_BURN)) {
		active = TRUE;
	}
	if (active) {
		if (unit->mFireEff == NULL) {
			unit->mFireEff = effFireEntry(0, 0, -1000.0f, -1000.0f, -1000.0f, 1.0f);
		}
		work = unit->mFireEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		if (unit->faceDirection >= 0) {
			direction = 1.0f;
		} else {
			direction = -1.0f;
		}
		x += unit->mSizeMultiplier * (unit->mKindParams->burnFlameOffset.x * direction);
		y += unit->mKindParams->burnFlameOffset.y * unit->mSizeMultiplier;
		z += unit->mKindParams->burnFlameOffset.z * unit->mSizeMultiplier;
		work->position.x = x;
		work->position.y = y;
		work->position.z = z + 5.0f;
	} else {
		BSE_FireDelete(unit);
	}
}

void BSE_FireDelete(BattleUnit* unit) {
	EffectEntry* effect = unit->mFireEff;

	if (effect != NULL) {
		effDelete(effect);
		unit->mFireEff = NULL;
	}
}

void BSE_Freeze(BattleUnit* unit) {
	BattleEffectPosScale* work;
	BattleUnitPart* part;
	f32 x, y, z;
	f32 scale;
	f32 widthScale;
	f32 heightScale;
	BOOL active = FALSE;

	if (BtlUnit_CheckStatus(unit, STATUS_FREEZE)) {
		active = TRUE;
	}
	if (active) {
		if (unit->mFreezeEff == NULL) {
			unit->mFreezeEff = effIceEntry(0, -1, -1000.0f, -1000.0f, -1000.0f, 1.0f);
		}
		work = unit->mFreezeEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		widthScale = (f32)unit->mWidth / 20.0f;
		heightScale = (f32)unit->mHeight / 36.0f;
		scale = (widthScale <= heightScale) ? widthScale : heightScale;
		scale *= unit->mSizeMultiplier;
		work->mIceScale = scale;
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		work->position.x = x;
		work->position.y = y + 20.0f * scale;
		work->position.z = z + 10.0f;
	} else {
		BSE_FreezeDelete(unit);
	}
}

void BSE_FreezeDelete(BattleUnit* unit) {
	EffectEntry* effect = unit->mFreezeEff;

	if (effect != NULL) {
		effSoftDelete(effect);
		unit->mFreezeEff = NULL;
	}
}

void BSE_Kagegakure(BattleUnit* unit) {
	BattleUnitPart* part;
	char name[0x14];
	f32 x, y, z;
	BOOL active = FALSE;

	if ((_battleWorkPointer->dispFlags & 1) && (unit->mAttributeFlags & kVeiled)) {
		active = TRUE;
	}
	if (active) {
		if (unit->mKagegakureIcon == NULL) {
			sprintf(name, "_biK_%02x", unit->unitId);
			iconEntry(name, 0x99);
			unit->mKagegakureIcon = iconNameToPtr(name);
		}
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		iconSetPos(unit->mKagegakureIcon->name, x, y, z + 5.0f);
	} else {
		BSE_KagegakureDelete(unit);
	}
}

void BSE_KagegakureDelete(BattleUnit* unit) {
	IconEntry* entry = unit->mKagegakureIcon;

	if (entry != NULL) {
		iconDelete(entry->name);
		unit->mKagegakureIcon = NULL;
	}
}

void BSE_TurnFirstProcessEffectEntry(BattleUnit* unit, s16 active) {
	unit->mBurnDamageFlameState = 1;
	unit->mbBurnDamageFlameActive = active;
}

BOOL BSE_TurnFirstProcessEffectMain(BattleUnit* unit) {
	BattleUnitPart* part;
	BattleEffectPosScale* work;
	f32 x, y, z;
	f32 scale;

	part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
	if (!unit->mBurnDamageFlameState) {
		return FALSE;
	}
	if (!unit->mbBurnDamageFlameActive) {
		return FALSE;
	}
	switch (unit->mbBurnDamageFlameActive) {
	case 1:
		switch (unit->mBurnDamageFlameState) {
		case 1:
			unit->mBurnDamageFlameState += 1;
			unit->mBurnDamageFlameScale = 1.0f;
			BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
			z += 5.0f;
			unit->mBurnDamageFlameEff = effFireEntry(0, 0, x, y, z, unit->mBurnDamageFlameScale);
			unit->mBurnDamageFlameAnimTimer = 0;
			break;
		case 2:
			work = unit->mBurnDamageFlameEff->userdata;
			unit->mBurnDamageFlameAnimTimer += 1;
			scale = 1.6f * sinfd((f32)(unit->mBurnDamageFlameAnimTimer * 3)) + 0.4f;
			unit->mBurnDamageFlameScale = scale;
			work->mFireScale = unit->mBurnDamageFlameScale;
			if (unit->mBurnDamageFlameAnimTimer >= 60) {
				unit->mBurnDamageFlameState += 1;
			}
			break;
		case 3:
			effDelete(unit->mBurnDamageFlameEff);
			unit->mBurnDamageFlameEff = NULL;
			unit->mBurnDamageFlameState = 0;
			if (!BtlUnit_CheckShadowGuard(unit)) {
				BattleDamageDirect(-5, unit, 0, 1, 0, 0x118, 0, 1);
			}
		case 0:
		default:
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

void BattleStatusChangeInfoWorkInit(BattleUnit* unit) {
	BattleStatusChangeEntry* entries = unit->mStatusChange;
	s32 i;

	for (i = 0; i < 6; i++) {
		BattleStatusChangeEntry* entry = &entries[i];
		entry->type = -1;
		entry->mFlags = 0;
		entry->mCounter = 0;
		entry->mDisplayOffset = 0;
		entry->mUnitId = 0;
	}
}

s32 _get_pri(s8 type) {
	s8* val;
	s32 priority = 24; //TODO: #define?

	for (val = _st_pri_data;; priority--, val++) {
		if (*val == -1) {
			return -1; //reached end of list
		}
		if (*val == type) {
			return priority;
		}
	}
	return -1;
}

void BattleStatusChangeInfoSetAnnouce(BattleUnit* unit, s32 type, s8 p1, s32 p2) {
	BattleStatusChangeEntry* list = unit->mStatusChange;
	s8 pri = _get_pri(type);
	s32 i = 0;
	BattleStatusChangeEntry entry;
	BattleStatusChangeEntry tmp;
	s32 j;
	u8 pos;
	s32 disp;

	do {
#ifdef MUST_MATCH
		s8 p = _get_pri(list[i].type + 0);
#else
		s8 p = _get_pri(list[i].type);
#endif

		if (pri >= p) {
			break;
		}
		i++;
	} while (i < 6);
	if (i >= 6) {
		return;
	}
	entry.type = type;
	entry.mFlags = p1;
	entry.mMsgVariant = p2;
	if (list[i].type == type) {
		list[i] = entry;
	} else {
		for (j = i; j < 6; j++) {
			tmp = list[j];
			list[j] = entry;
			entry = tmp;
		}
	}
	pos = 0;
	for (j = 0; j < 6; j++) {
		if (list[j].type != -1) {
			list[j].mCounter = pos;
			list[j].mUnitId = 0;
			pos += 0xA;
		} else {
			break;
		}
	}
	if (pos != 0) {
		disp = 0x50;
	} else {
		disp = 0x3C;
	}
	for (; j >= 0; j--) {
		if (list[j].type != -1) {
			list[j].mDisplayOffset = disp;
			disp += 0xA;
		}
	}
}

BOOL BattleStatusChangeAnnouceMain(BattleWork* wp) {
	s32 i;
	BOOL result = FALSE;

	for (i = 0; i < 0x40; i++) {
		BattleUnit* unit = BattleGetUnitPtr(wp, i);
		if (unit != NULL && BattleStatusChangeAnnouceMain_Unit(unit)) {
			result = TRUE;
		}
	}
	return result;
}

BOOL BattleStatusChangeAnnouceMain_Unit(BattleUnit* unit) {
	return FALSE;
}

void BattleStatusChangeMsgWorkInit(void) {
	BattleStatusChangeMsgWork* work = &_battleWorkPointer->statusChangeMsg;
	s32 i;

	memset(work, 0, sizeof(BattleStatusChangeMsgWork));
	for (i = 0; i < 0x1C; i++) {
		work->entries[i].type = -1;
	}
}

void BattleStatusChangeMsgSetAnnouce(BattleUnit* unit, s32 type, s32 msg) {
	// Retail adds the work offset to the raw pointer; &_battleWorkPointer->statusChangeMsg.entries[type]
	// compiles 8 bytes longer.
	BattleStatusChangeEntry* entry = &((BattleStatusChangeMsgWork*)((char*)_battleWorkPointer + offsetof(BattleWork, statusChangeMsg)))->entries[type];

	entry->mUnitId = unit->unitId;
	entry->type = type;
	entry->mCounter = 0;
	entry->mMsgVariant = msg;
	if (unit->currentKind == UNIT_MARIO) {
		entry->mFlags |= 1;
		return;
	}
	if (unit->currentKind >= TYPE_PARTNER_MIN && unit->currentKind < TYPE_PARTNER_MAX) {
		if (UNIT_ALLIANCE_ENEMY) {
			entry->mFlags |= 2;
			return;
		}
	}
	entry->mFlags |= 4;
}

void BattleStatusChangeMsgAdjust(BattleWork* wp) {
	BattleStatusChangeMsgWork* work = &wp->statusChangeMsg;
	s8 i;

	for (i = 0; i < 0x1C; i++) {
		BattleStatusChangeEntry* entry = &work->entries[i];
		s32 idx = i;

		if (entry->type != -1) {
			s32 j;

			entry->mFlags = 0;
			for (j = 0; j < 0x40; j++) {
				BattleUnit* unit = BattleGetUnitPtr(wp, j);

				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
					s32 found = 0;
					s32 k;
					BattleStatusChangeEntry* info = unit->mStatusChange;

					for (k = 0; k < 6; k++) {
						if (i == info[k].type) {
							found = 1;
							break;
						}
					}
					if (found) {
						BattleStatusChangeMsgSetAnnouce(unit, idx, info[k].mMsgVariant);
					}
				}
			}
			if (entry->mFlags == 0) {
				entry->mUnitId = -1;
				entry->type = -1;
			} else if (work->mAnnouncedFlags & (s64)(1 << idx)) {
				entry->mUnitId = -1;
				entry->type = -1;
			}
		}
	}
}

BOOL BattleStatusChangeMsgMain(BattleWork* wp) {
	BattleStatusChangeMsgWork* work = &wp->statusChangeMsg;
	BattleStatusChangeEntry* entry;
	BOOL shown = FALSE;
	s8* pri;
	s8 type;

	// Naming the loaded byte in a local keeps it in r4 (extsb r0, r4 both times).
	for (pri = _st_pri_data; (type = *pri) != -1; pri++) {
		entry = &work->entries[type];

		if (entry->type != -1) {
			BattleUnit* unit;

			shown = TRUE;
			unit = BattleGetUnitPtr(wp, entry->mUnitId);
			if (entry->mCounter != 0) {
				entry->mCounter--;
				if (BattlePadCheckTrigger(0x300) != 0) {
					entry->mCounter = 0;
				}
				if (entry->mCounter == 0) {
					entry->type = -1;
				}
			} else {
				s32 i;

				sprintf(work->message, "");
				for (i = 0; i < 0x1C; i++) {
					if (entry->type == _st_chg_msg_data[i].type || _st_chg_msg_data[i].type == -1) {
						if (entry->mMsgVariant != 0) {
							sprintf(work->message, msgSearch(_st_chg_msg_data[i].mMessageName));
						} else {
							sprintf(work->message, msgSearch(_st_chg_msg_data[i].mMessageNameAlt));
						}
						break;
					}
				}
				entry->mCounter = 0x5A;
			}
			dispEntry(0x8, 900.0f, 1, _st_chg_msg_disp, NULL);
			break;
		}
	}

	if (shown) {
		s32 i;

		for (i = 0; i < 0x40; i++) {
			BattleUnit* unit = BattleGetUnitPtr(wp, i);
			if (unit != NULL) {
				BattleStatusChangeInfoWorkInit(unit);
			}
		}
	}
	return shown;
}

// The original passes the GXColor by value; the call below goes through a by-pointer prototype, which is
// ABI-identical (a by-value GXColor struct is passed by reference) but steers MWCC's register allocation so
// the 0x4330 conversion magic, the double_to_int base and the colour land in the target's r8/r9/r10 instead
// of the swapped set (same trick as battle_message.c _disp).
void _st_chg_msg_disp(CameraId camera, void* param) {
	BattleWork* wp = _battleWorkPointer;
	BattleStatusChangeMsgWork* work = &wp->statusChangeMsg;
	u16 lines;
	u16 width;
	f32 x;
	f32 zero = 0.0f;
	f32 neg120 = -120.0f; // keep the pool load: a bare `(s32)-120.0f` folds to -120 and drops the fctiwz
	GXColor color;

	width = FontGetMessageWidthLine(work->message, &lines);
	lines++;
	color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
	x = zero - (f32)(width / 2);
#ifdef MUST_MATCH
	((void (*)(s32, GXColor*, f32, f32, f32, f32, f32))windowDispGX_Waku_col)(0, &color, x - 20.0f,
	    -117.0f + (f32)((lines - 1) * 0x1D), (f32)(width + 0x28), (f32)(lines * 0x1D + 1), 10.0f);
#else
	windowDispGX_Waku_col(x - 20.0f, -117.0f + (f32)((lines - 1) * 0x1D), (f32)(width + 0x28), (f32)(lines * 0x1D + 1),
	                      10.0f, 0, color);
#endif
	FontDrawStart();
	FontDrawMessage((s32)x, (s32)neg120 + (lines - 1) * 0x1D, work->message);
}

