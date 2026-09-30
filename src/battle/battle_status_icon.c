#include "battle/battle_status_icon.h"
#include "battle/battle.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;

typedef struct IconDispTableEntry IconDispTableEntry;
struct IconDispTableEntry {
	s32 id;
	BOOL (*callback)(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry); //0x4
	s32 something;
	s32 something2;
};

IconDispTableEntry _disp_table[] = {
	{1, NULL, 15, 1}
};

//local prototypes

void BattleStatusIconInit(BattleUnit* unit) {
	memset(&unit->mStatusIconWork, 0, sizeof(BattleWorkStatusIcon));
}

// NONMATCHING (86.59%): 6 instructions differ, 7 missing, 4 extra, plus register differences.
void BattleStatusIconMain(BattleUnit* unit) {
	BattleWorkStatusIcon* icon = &unit->mStatusIconWork;
	BattleWork* work = _battleWorkPointer;
	int i;

	icon->mNumStatusIcons = 0;
	if (work->flags & 0x10000 || work->dispFlags & 1) {
		BtlUnit_GetPos(unit, &icon->mPosition.x, &icon->mPosition.y, &icon->mPosition.z);
		icon->mPosition.x += ((f32)(unit->mStatusIconOffset[0]) * unit->mSizeMultiplier);
		icon->mPosition.y += ((f32)(unit->mStatusIconOffset[1]) * unit->mSizeMultiplier);
		icon->mScreenSpaceOffset.x = 13.440001f;
		icon->mScreenSpaceOffset.z = 0.0f;
		icon->mFlags = 0;
		if (unit->mAttributeFlags & kCeiling) {
			icon->mDistanceBetweenIcons = 25.6f;
			icon->mScreenSpaceOffset.y = 0.0f;
		}
		else {
			icon->mDistanceBetweenIcons = 25.6f;
			icon->mScreenSpaceOffset.y = 19.200001f;
		}

		for (i = 0; i < 0x17; i++) {
			if (_disp_table[i].id == -1) break;
			if (_disp_table[i].callback(unit, &_disp_table[i], &icon->mIcons[i])) {
				icon->mNumStatusIcons++;
			}
		}
		if (icon->mNumStatusIcons > 0) {
			dispEntry(CAMERA_2D, 901.0f, 1, BattleStatusIconDisp, unit);
		}
	}
}

void BattleStatusIconDelete(BattleUnit* unit) {
	; //stubbed in retail
}

// NONMATCHING (0.46%): 218 missing.
void BattleStatusIconDisp(CameraId camId, void* param) {
	BattleUnit* unit = param; //cast to proper type
}

// Icon parameter table base; fields accessed at +0x60..+0x8C are Vec entries 8..11.
extern Vec vec3_803011e8[];
// -1 sentinel written to entry+0x3C.
extern s32 dat_804221f8;
extern s32 dat_804221f4;
extern s32 dat_804221f0;
extern f32 float_0_80422214;
extern f32 float_4_80422218;

BOOL _bsi_default_turn(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	Vec* vec = vec3_803011e8;
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;

	if (_battleWorkPointer->flags & 0x10000) {
		if (type != 1) {
#ifdef MUST_MATCH
			if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
			if (!BtlUnit_CheckStatus(unit, type)) {
#endif
				return FALSE;
			}
		}
		if (!(unit->currentKind >= 1 && unit->currentKind < 0xD8) &&
		    !(unit->currentKind >= 0xDE && unit->currentKind < 0xE0) &&
		    !(unit->currentKind >= 0xE0 && unit->currentKind < 0xE7)) {
			return FALSE;
		}
	}
	else {
#ifdef MUST_MATCH
		if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
		if (!BtlUnit_CheckStatus(unit, type)) {
#endif
			return FALSE;
		}
	}

#ifdef MUST_MATCH
	((void(*)(BattleUnit*, s8, s8*, s8*))BtlUnit_GetStatus)(unit, type, &turns, &strength);
#else
	BtlUnit_GetStatus(unit, type, &turns, &strength);
#endif
	*(s32*)((u8*)entry + 0x0) = type;
#ifdef MUST_MATCH
	turnCount = -turns;
	if (turns >= 0) {
		turnCount = turns;
	}
#else
	turnCount = turns < 0 ? -turns : turns;
#endif
	*(s32*)((u8*)entry + 0x4) = turnCount;
	*(s32*)((u8*)entry + 0x8) = disp->something;
	*(Vec*)((u8*)entry + 0xC) = vec[0];
	*(Vec*)((u8*)entry + 0x18) = vec[1];
	*(Vec*)((u8*)entry + 0x24) = vec[2];
	*(s32*)((u8*)entry + 0x3C) = dat_804221f0;
	*(s32*)((u8*)entry + 0x40) = disp->something2;
	*(Vec*)((u8*)entry + 0x30) = vec[3];
	return TRUE;
}

BOOL _bsi_guruguru(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	Vec* vec = vec3_803011e8;
	s32 type = disp->id;
	s8 turns;
	s8 strength;
	s32 turnCount;
	Vec tmp;
	f32* counter = (f32*)unit->mStatusIconWork.field_0x790; //per-status time counter

#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = float_0_80422214;
		return FALSE;
	}

	*counter += float_4_80422218;
#ifdef MUST_MATCH
	((void(*)(BattleUnit*, s8, s8*, s8*))BtlUnit_GetStatus)(unit, type, &turns, &strength);
#else
	BtlUnit_GetStatus(unit, type, &turns, &strength);
#endif
	*(s32*)((u8*)entry + 0x0) = type;
#ifdef MUST_MATCH
	turnCount = -turns;
	if (turns >= 0) {
		turnCount = turns;
	}
#else
	turnCount = turns < 0 ? -turns : turns;
#endif
	*(s32*)((u8*)entry + 0x4) = turnCount;
	*(s32*)((u8*)entry + 0x8) = disp->something;
	*(Vec*)((u8*)entry + 0xC) = vec[4];
	*(Vec*)((u8*)entry + 0x18) = vec[5];
#ifdef MUST_MATCH
	// MUST_MATCH: MWCC forwards vec[6].x/.y straight to the entry but keeps .z in a
	// stack temporary which the counter store then reuses; the direct two-assignment
	// form below omits that temporary.
	tmp = vec[6];
	tmp.z = *counter;
	*(Vec*)((u8*)entry + 0x24) = tmp;
#else
	*(Vec*)((u8*)entry + 0x24) = vec[6];
	*(f32*)((u8*)entry + 0x2C) = *counter;
#endif
	*(s32*)((u8*)entry + 0x3C) = dat_804221f4;
	*(s32*)((u8*)entry + 0x40) = disp->something2;
	*(Vec*)((u8*)entry + 0x30) = vec[7];
	return TRUE;
}

BOOL _bsi_allergy(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	Vec* vec = vec3_803011e8;
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = (s16*)unit->mStatusIconWork.field_0x796; //per-status tick counter

	// The header's BtlUnit_CheckStatus/BtlUnit_GetStatus take StatusEffectType (int), but the
	// callee sign-extends it (battle_status_effect.h says it "should be an s8"). Calling through
	// a matching function pointer makes MWCC coerce the argument like the original.
#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = 0;
		return FALSE;
	}

#ifdef MUST_MATCH
	((void(*)(BattleUnit*, s8, s8*, s8*))BtlUnit_GetStatus)(unit, type, &turns, &strength);
#else
	BtlUnit_GetStatus(unit, type, &turns, &strength);
#endif
	*(s32*)((u8*)entry + 0x0) = type;
#ifdef MUST_MATCH
	// MUST_MATCH: MWCC computes the negation speculatively and selects with `neg; blt; mr`,
	// which a ternary or abs() does not reproduce. Natural form is under #else.
	turnCount = -turns;
	if (turns >= 0) {
		turnCount = turns;
	}
#else
	turnCount = turns < 0 ? -turns : turns;
#endif
	*(s32*)((u8*)entry + 0x4) = turnCount;
	*(s32*)((u8*)entry + 0x8) = disp->something;
	*(Vec*)((u8*)entry + 0xC) = vec[8];
	*(Vec*)((u8*)entry + 0x18) = vec[9];
	*(Vec*)((u8*)entry + 0x24) = vec[10];
	*(s32*)((u8*)entry + 0x3C) = dat_804221f8;
	*(s32*)((u8*)entry + 0x40) = disp->something2;
	*(Vec*)((u8*)entry + 0x30) = vec[11];

	*counter = *counter + 1;
	if (*counter & 0x10) {
		*(s32*)((u8*)entry + 0x40) |= 2;
	}
	return TRUE;
}
