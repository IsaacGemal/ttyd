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
	const char* field_0x4; // 0x4
	const char* field_0x8; // 0x8
	const char* field_0xC; // 0xC
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

// local overlays for BattleUnit regions the shared header leaves unnamed
typedef struct BattleUnitEffectFields {
	u8 pad_0[0x320];
	EffectEntry* mSleepEff;     // 0x320
	EffectEntry* mFireEff;      // 0x324
	EffectEntry* mFreezeEff;    // 0x328
	EffectEntry* mBiribiriEff;  // 0x32C
	IconEntry* mKagegakureIcon; // 0x330
} BattleUnitEffectFields;

typedef struct BattleUnitKindOffsetFields {
	u8 pad_0[0x48];
	f32 mField_0x48; // 0x48
	f32 mField_0x4C; // 0x4C
	f32 mField_0x50; // 0x50
} BattleUnitKindOffsetFields;

typedef struct BattleEffectPos {
	u32 field_0x0; // 0x0
	Vec position;  // 0x4
} BattleEffectPos;

typedef struct BattleEffectPosScale {
	u32 field_0x0;  // 0x0
	Vec position;   // 0x4
	f32 field_0x10; // 0x10
	u8 pad_0x14[0x24 - 0x14];
	f32 mScale; // 0x24
} BattleEffectPosScale;

typedef struct BattleStatusChangeEntry {
	s8 type;       // 0x0
	u8 field_0x1;  // 0x1
	u8 field_0x2;  // 0x2
	u8 field_0x3;  // 0x3
	s32 field_0x4; // 0x4
	s32 field_0x8; // 0x8
} BattleStatusChangeEntry;

typedef struct BattleUnitStatusChangeFields {
	u8 pad_0[0xAE4];
	BattleStatusChangeEntry mStatusChange[6]; // 0xAE4
} BattleUnitStatusChangeFields;

typedef struct BattleStatusChangeMsgWork {
	BattleStatusChangeEntry entries[0x1C]; // 0x0
	char message[0x100];                   // 0x150
	u64 field_0x250;                       // 0x250
} BattleStatusChangeMsgWork;

// NONMATCHING (60.57%): 7 instructions differ, 16 missing, 10 extra, plus register differences.
void _st_chg_msg_disp(CameraId camera, void* param) {
	BattleWork* wp = _battleWorkPointer;
	u16 lines;
	u32 packed;
	s32 width;
	s32 half;
	GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
	f32 f31;
	char* msg = (char*)((u8*)wp + 0x18ED8);

	packed = FontGetMessageWidthLine(msg, &lines);
	width = packed & 0xFFFF;
	half = (packed >> 1) & 0x7FFF;
	f31 = 0.0f - (f32)half;
	windowDispGX_Waku_col(0, color, f31 - 20.0f, -117.0f + (f32)(lines * 0x1D), (f32)(width + 0x28),
	                      (f32)((lines + 1) * 0x1D + 1), 10.0f);
	FontDrawStart();
	FontDrawMessage((s32)f31, (s32)-120.0f + lines * 0x1D, msg);
}

BOOL BattleStatusChangeMsgMain(BattleWork* wp) {
	BattleStatusChangeMsgWork* work = (BattleStatusChangeMsgWork*)((u8*)wp + 0x18D88);
	BattleStatusChangeEntry* entry;
	BOOL shown = FALSE;
	s8* pri;
	s8 type;

#ifdef MUST_MATCH
	// MUST_MATCH: naming the loaded byte in a local keeps it in r4 as in retail (extsb r0, r4 both times).
	for (pri = _st_pri_data; (type = *pri) != -1; pri++) {
		entry = &work->entries[type];
#else
	for (pri = _st_pri_data; *pri != -1; pri++) {
		entry = &work->entries[*pri];
#endif

		if (entry->type != -1) {
			BattleUnit* unit;

			shown = TRUE;
			unit = BattleGetUnitPtr(wp, entry->field_0x4);
			if (entry->field_0x2 != 0) {
				entry->field_0x2--;
				if (BattlePadCheckTrigger(0x300) != 0) {
					entry->field_0x2 = 0;
				}
				if (entry->field_0x2 == 0) {
					entry->type = -1;
				}
			} else {
				s32 i;

				sprintf(work->message, "");
				for (i = 0; i < 0x1C; i++) {
					if (entry->type == _st_chg_msg_data[i].type || _st_chg_msg_data[i].type == -1) {
						if (entry->field_0x8 != 0) {
							sprintf(work->message, msgSearch(_st_chg_msg_data[i].field_0x8));
						} else {
							sprintf(work->message, msgSearch(_st_chg_msg_data[i].field_0xC));
						}
						break;
					}
				}
				entry->field_0x2 = 0x5A;
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

void BattleStatusChangeMsgAdjust(BattleWork* wp) {
	BattleStatusChangeMsgWork* work = (BattleStatusChangeMsgWork*)((u8*)wp + 0x18D88);
	s8 i;

	for (i = 0; i < 0x1C; i++) {
		BattleStatusChangeEntry* entry = &work->entries[i];
		s32 idx = i;

		if (entry->type != -1) {
			s32 j;

			entry->field_0x1 = 0;
			for (j = 0; j < 0x40; j++) {
				BattleUnit* unit = BattleGetUnitPtr(wp, j);

				if (unit != NULL && !BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL)) {
					s32 found = 0;
					s32 k;
					BattleStatusChangeEntry* info = ((BattleUnitStatusChangeFields*)unit)->mStatusChange;

					for (k = 0; k < 6; k++) {
						if (i == info[k].type) {
							found = 1;
							break;
						}
					}
					if (found) {
						BattleStatusChangeMsgSetAnnouce(unit, idx, info[k].field_0x8);
					}
				}
			}
			if (entry->field_0x1 == 0) {
				entry->field_0x4 = -1;
				entry->type = -1;
			} else if (work->field_0x250 & (s64)(1 << idx)) {
				entry->field_0x4 = -1;
				entry->type = -1;
			}
		}
	}
}

// Keep BattleStatusChangeMsgSetAnnouce out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BattleStatusChangeMsgSetAnnouce(BattleUnit* unit, s32 type, s32 msg) {
	BattleStatusChangeEntry* entry = &((BattleStatusChangeMsgWork*)((u8*)_battleWorkPointer + 0x18D88))->entries[type];

	entry->field_0x4 = unit->unitId;
	entry->type = type;
	entry->field_0x2 = 0;
	entry->field_0x8 = msg;
	if (unit->currentKind == 0xDE) {
		entry->field_0x1 |= 1;
		return;
	}
	if (unit->currentKind >= 0xE0 && unit->currentKind < 0xE7) {
		if (unit->mAlliance == 0) {
			entry->field_0x1 |= 2;
			return;
		}
	}
	entry->field_0x1 |= 4;
}
#pragma dont_inline reset

void BattleStatusChangeMsgWorkInit(void) {
	BattleStatusChangeMsgWork* work = (BattleStatusChangeMsgWork*)((u8*)_battleWorkPointer + 0x18D88);
	s32 i;

	memset(work, 0, sizeof(BattleStatusChangeMsgWork));
	for (i = 0; i < 0x1C; i++) {
		work->entries[i].type = -1;
	}
}

// Keep BattleStatusChangeAnnouceMain_Unit out of line: the original calls it rather than inlining.
#pragma dont_inline on
BOOL BattleStatusChangeAnnouceMain_Unit(BattleUnit* unit) {
	return FALSE;
}
#pragma dont_inline reset

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

// NONMATCHING (88.68%): s8 pri and do/while loop now match; remaining differences are type sign-extension scheduling and count/pos/disp register allocation
void BattleStatusChangeInfoSetAnnouce(BattleUnit* unit, s8 type, s8 p1, s32 p2) {
	BattleStatusChangeEntry* list = ((BattleUnitStatusChangeFields*)unit)->mStatusChange;
	s8 pri = _get_pri(type);
	s32 i = 0;
	BattleStatusChangeEntry entry;
	BattleStatusChangeEntry tmp;
	s32 j;
	s32 count;
	u8 pos;
	s32 disp;

	do {
		s8 p = _get_pri(list[i].type);

		if (p <= pri) {
			break;
		}
		i++;
	} while (i < 6);
	if (i == 6) {
		return;
	}
	entry.type = type;
	entry.field_0x1 = p1;
	entry.field_0x8 = p2;
	if (list[i].type == type) {
		list[i] = entry;
	} else {
		for (j = i; j < 6; j++) {
			tmp = list[j];
			list[j] = entry;
			entry = tmp;
		}
	}
	count = 0;
	pos = 0;
	for (j = 0; j < 6; j++) {
		if (list[j].type != -1) {
			list[j].field_0x2 = pos;
			pos += 0xA;
			count++;
			list[j].field_0x4 = 0;
		} else {
			break;
		}
	}
	disp = ((u8)pos != 0) ? 0x50 : 0x3C;
	for (j = count; j >= 0; j--) {
		if (list[j].type != -1) {
			list[j].field_0x3 = disp;
			disp += 0xA;
		}
	}
}

// Keep _get_pri out of line: the original calls it rather than inlining.
#pragma dont_inline on
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
#pragma dont_inline reset

#pragma dont_inline on
void BattleStatusChangeInfoWorkInit(BattleUnit* unit) {
	BattleStatusChangeEntry* entries = ((BattleUnitStatusChangeFields*)unit)->mStatusChange;
	s32 i;

	for (i = 0; i < 6; i++) {
		BattleStatusChangeEntry* entry = &entries[i];
		entry->type = -1;
		entry->field_0x1 = 0;
		entry->field_0x2 = 0;
		entry->field_0x3 = 0;
		entry->field_0x4 = 0;
	}
}
#pragma dont_inline reset

// NOTE: mBurnDamageFlameState/mbBurnDamageFlameActive are declared s16 in battle_unit.h, but the
// original loads them with lhz and tests them unsigned (cmplwi); cast to u16 to match. Header likely wrong.
BOOL BSE_TurnFirstProcessEffectMain(BattleUnit* unit) {
	BattleUnitPart* part;
	BattleEffectPosScale* work;
	f32 x, y, z;
	f32 scale;

	part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
	if (!*(u16*)&unit->mBurnDamageFlameState) {
		return FALSE;
	}
	if (!*(u16*)&unit->mbBurnDamageFlameActive) {
		return FALSE;
	}
	switch (*(u16*)&unit->mbBurnDamageFlameActive) {
	case 1:
		switch (*(u16*)&unit->mBurnDamageFlameState) {
		case 1:
			*(u16*)&unit->mBurnDamageFlameState += 1;
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
			work->mScale = unit->mBurnDamageFlameScale;
			if (unit->mBurnDamageFlameAnimTimer >= 60) {
				*(u16*)&unit->mBurnDamageFlameState += 1;
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

void BSE_TurnFirstProcessEffectEntry(BattleUnit* unit, s16 active) {
	unit->mBurnDamageFlameState = 1;
	unit->mbBurnDamageFlameActive = active;
}

// Keep BSE_KagegakureDelete out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_KagegakureDelete(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	IconEntry* entry = fields->mKagegakureIcon;

	if (entry != NULL) {
		iconDelete(entry->name);
		fields->mKagegakureIcon = NULL;
	}
}
#pragma dont_inline reset

// Keep BSE_Kagegakure out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_Kagegakure(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	BattleUnitPart* part;
	char name[0x14];
	f32 x, y, z;
	BOOL active = FALSE;

	if ((_battleWorkPointer->dispFlags & 1) && (unit->mAttributeFlags & kVeiled)) {
		active = TRUE;
	}
	if (active) {
		if (fields->mKagegakureIcon == NULL) {
			sprintf(name, "_biK_%02x", unit->unitId);
			iconEntry(name, 0x99);
			fields->mKagegakureIcon = iconNameToPtr(name);
		}
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		iconSetPos(fields->mKagegakureIcon->name, x, y, z + 5.0f);
	} else {
		BSE_KagegakureDelete(unit);
	}
}
#pragma dont_inline reset

// Keep BSE_FreezeDelete out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_FreezeDelete(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	EffectEntry* effect = fields->mFreezeEff;

	if (effect != NULL) {
		effSoftDelete(effect);
		fields->mFreezeEff = NULL;
	}
}
#pragma dont_inline reset

// Keep BSE_Freeze out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_Freeze(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
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
		if (fields->mFreezeEff == NULL) {
			fields->mFreezeEff = effIceEntry(0, -1, -1000.0f, -1000.0f, -1000.0f, 1.0f);
		}
		work = fields->mFreezeEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		widthScale = (f32)unit->mWidth / 20.0f;
		heightScale = (f32)unit->mHeight / 36.0f;
		scale = (widthScale <= heightScale) ? widthScale : heightScale;
		scale *= unit->mSizeMultiplier;
		work->field_0x10 = scale;
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		work->position.x = x;
		work->position.y = y + 20.0f * scale;
		work->position.z = z + 10.0f;
	} else {
		BSE_FreezeDelete(unit);
	}
}
#pragma dont_inline reset

// Keep BSE_FireDelete out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_FireDelete(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	EffectEntry* effect = fields->mFireEff;

	if (effect != NULL) {
		effDelete(effect);
		fields->mFireEff = NULL;
	}
}
#pragma dont_inline reset

// dont_inline: without it BattleStatusEffectMain inlines BSE_Fire; the original calls it.
#pragma dont_inline on
void BSE_Fire(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	BattleEffectPos* work;
	BattleUnitPart* part;
	f32 x, y, z;
	f32 direction;
	BOOL active = FALSE;

	if (BtlUnit_CheckStatus(unit, STATUS_BURN)) {
		active = TRUE;
	}
	if (active) {
		if (fields->mFireEff == NULL) {
			fields->mFireEff = effFireEntry(0, 0, -1000.0f, -1000.0f, -1000.0f, 1.0f);
		}
		work = fields->mFireEff->userdata;
		part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
		BtlUnit_GetPartsWorldPos(part, &x, &y, &z);
		if (unit->faceDirection >= 0) {
			direction = 1.0f;
		} else {
			direction = -1.0f;
		}
		x += unit->mSizeMultiplier * (((BattleUnitKindOffsetFields*)unit->mKindParams)->mField_0x48 * direction);
		y += ((BattleUnitKindOffsetFields*)unit->mKindParams)->mField_0x4C * unit->mSizeMultiplier;
		z += ((BattleUnitKindOffsetFields*)unit->mKindParams)->mField_0x50 * unit->mSizeMultiplier;
		work->position.x = x;
		work->position.y = y;
		work->position.z = z + 5.0f;
	} else {
		BSE_FireDelete(unit);
	}
}
#pragma dont_inline reset

// Keep BSE_BiribiriDelete out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_BiribiriDelete(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	EffectEntry* effect = fields->mBiribiriEff;

	if (effect != NULL) {
		effDelete(effect);
		fields->mBiribiriEff = NULL;
	}
}
#pragma dont_inline reset

// Keep BSE_Biribiri out of line: the original calls it rather than inlining.
#pragma dont_inline on
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
		if (((BattleUnitEffectFields*)unit)->mBiribiriEff == NULL) {
			((BattleUnitEffectFields*)unit)->mBiribiriEff = effSparkEntry(0, 10, -1000.0f, -1000.0f, -1000.0f, (f32)(BtlUnit_GetWidth(unit) / 2));
		}
		work = ((BattleUnitEffectFields*)unit)->mBiribiriEff->userdata;
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
#pragma dont_inline reset

// Keep BSE_SleepDelete out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BSE_SleepDelete(BattleUnit* unit) {
	BattleUnitEffectFields* fields = (BattleUnitEffectFields*)unit;
	EffectEntry* effect = fields->mSleepEff;

	if (effect != NULL) {
		effDelete(effect);
		fields->mSleepEff = NULL;
	}
}
#pragma dont_inline reset

// dont_inline: without it BattleStatusEffectMain inlines BSE_Sleep; the original calls it.
#pragma dont_inline on
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
		if (((BattleUnitEffectFields*)unit)->mSleepEff == NULL) {
			((BattleUnitEffectFields*)unit)->mSleepEff = effSleepEntry(0, 0, -1000.0f, -1000.0f, -1000.0f, 1.0f, 60.0f);
		}
		work = ((BattleUnitEffectFields*)unit)->mSleepEff->userdata;
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
#pragma dont_inline reset

void BattleStatusEffectDelete(BattleUnit* unit) {
	BSE_SleepDelete(unit);
	BSE_BiribiriDelete(unit);
	BSE_FireDelete(unit);
	BSE_FreezeDelete(unit);
	BSE_KagegakureDelete(unit);
}

void BattleStatusEffectMain(BattleUnit* unit) {
	BSE_Sleep(unit);
	BSE_Biribiri(unit);
	BSE_Fire(unit);
	BSE_Freeze(unit);
	BSE_Kagegakure(unit);
}

void BattleStatusEffectInit(BattleUnit* unit) {
	; //stubbed in retail
}

