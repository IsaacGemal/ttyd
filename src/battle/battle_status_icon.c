#include "battle/battle_status_icon.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;

typedef struct IconDispTableEntry IconDispTableEntry;
struct IconDispTableEntry {
	s32 id;
	BOOL (*callback)(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry); //0x4
	s32 texId;
	s32 flags;
};


IconDispTableEntry _disp_table[] = {
	{STATUS_SLEEP, _bsi_default_turn, 15, 1},
	{STATUS_POISON, _bsi_default_turn, 13, 1},
	{STATUS_PAYBACK, _bsi_default_turn, 14, 1},
	{STATUS_STOP, _bsi_default_turn, 16, 1},
	{STATUS_BURN, _bsi_default_turn, 17, 1},
	{STATUS_FREEZE, _bsi_default_turn, 18, 1},
	{STATUS_FAST, _bsi_default_turn, 19, 1},
	{STATUS_SLOW, _bsi_default_turn, 20, 1},
	{STATUS_DEFENSE_UP, _bsi_default_turn, 21, 1},
	{STATUS_DEFENSE_DOWN, _bsi_default_turn, 22, 1},
	{STATUS_DODGY, _bsi_default_turn, 28, 1},
	{STATUS_HUGE, _bsi_default_turn, 29, 1},
	{STATUS_TINY, _bsi_default_turn, 30, 1},
	{STATUS_CONFUSE, _bsi_default_turn, 31, 1},
	{STATUS_ATTACK_UP, _bsi_default_turn, 32, 1},
	{STATUS_ATTACK_DOWN, _bsi_default_turn, 33, 1},
	{STATUS_INVISIBLE, _bsi_default_turn, 34, 1},
	{STATUS_DIZZY, _bsi_guruguru, 26, 0},
	{STATUS_ELECTRIC, _bsi_biribiri, 25, 0},
	{STATUS_ALLERGIC, _bsi_allergy, 27, 0},
	{STATUS_HP_REGEN, _bsi_regeneration, 23, 0},
	{STATUS_FP_REGEN, _bsi_regeneration, 24, 0},
	{-1, NULL, 0, 0},
};

//local prototypes

void BattleStatusIconInit(BattleUnit* unit) {
	memset(&unit->mStatusIconWork, 0, sizeof(BattleWorkStatusIcon));
}

void BattleStatusIconMain(BattleUnit* unit) {
	BattleWorkStatusIcon* icon = &unit->mStatusIconWork;
	BattleWork* work = _battleWorkPointer;
	BattleWorkStatusIconEntry* entry;
	u32 i;

	icon->mNumStatusIcons = 0;
	if (work->flags & 0x10000 || work->dispFlags & 1) {
		BtlUnit_GetPos(unit, &icon->mPosition.x, &icon->mPosition.y, &icon->mPosition.z);
		icon->mPosition.x += ((f32)(unit->mStatusIconOffset[0]) * unit->mSizeMultiplier);
		icon->mPosition.y += ((f32)(unit->mStatusIconOffset[1]) * unit->mSizeMultiplier);
		icon->mScreenSpaceOffset.x = 13.440001f;
		icon->mScreenSpaceOffset.z = 0.0f;
		icon->mFlags = 0;
		if (!(unit->mAttributeFlags & kCeiling)) {
			icon->mDistanceBetweenIcons = 25.6f;
			icon->mScreenSpaceOffset.y = 19.200001f;
		}
		else {
			icon->mDistanceBetweenIcons = 25.6f;
			icon->mScreenSpaceOffset.y = 0.0f;
		}

		for (i = 0; i < 0x17; i++) {
			entry = &icon->mIcons[icon->mNumStatusIcons];
			if (_disp_table[i].id == -1) break;
			if (_disp_table[i].callback(unit, &_disp_table[i], entry)) {
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

// The loop must index mIcons directly (and the float use be written as i * -2) for -O4,s strength
// reduction to produce the original's i/2i/offset counters.
void BattleStatusIconDisp(CameraId camId, void* param) {
	BattleUnit* unit = param;
	BattleWorkStatusIcon* icon = &unit->mStatusIconWork;
	BattleWorkStatusIconEntry* entry;
	Mtx mtx;
	Mtx mtxPos;
	Mtx mtxScale;
	Mtx mtxTransNeg;
	Mtx mtxTransPos;
	Mtx mtxRotX;
	Mtx mtxRotY;
	Mtx mtxRotZ;
	Mtx mtx2;
	Vec screenPos;
	Vec pos;
	Vec pos2;
	GXColor color;
	s32 i;

	color = (GXColor){ 0xff, 0xff, 0xff, 0xff };
	btlGetScreenPoint(&icon->mPosition, &screenPos);
	for (i = icon->mNumStatusIcons - 1; i >= 0; i--) {
		entry = &icon->mIcons[i];
		pos = screenPos;
		pos.y = icon->mDistanceBetweenIcons * (f32)i + pos.y;
		pos.x += icon->mScreenSpaceOffset.x;
		pos.y += icon->mScreenSpaceOffset.y;
		pos.z += icon->mScreenSpaceOffset.z;
		pos.z -= (f32)(i * -2);

		if (!(entry->flags & 1)) {
			if (!(icon->mFlags & 1)) {
				btlDispTexPlane(pos.x, pos.y, pos.z, 0.8f, 0.8f, 0x23, color, 0);
			}
			else {
				btlDispTexPlane(pos.x, pos.y, pos.z, 0.8f, -0.8f, 0x23, color, 0);
			}
		}

		if (entry->texId != 0) {
			if (!(entry->flags & 2)) {
				PSMTXTrans(mtxPos, pos.x + entry->offset.x,
				           pos.y + entry->offset.y,
				           pos.z + entry->offset.z);
				PSMTXScale(mtxScale, 0.8f * entry->scale.x,
				           0.8f * entry->scale.y,
				           entry->scale.z);
				PSMTXTrans(mtxTransNeg, -entry->pivot.x,
				           -entry->pivot.y,
				           -entry->pivot.z);
				PSMTXTrans(mtxTransPos, entry->pivot.x,
				           entry->pivot.y,
				           entry->pivot.z);
				PSMTXRotRad(mtxRotX, 'x', 0.017453292f * entry->rotation.x);
				PSMTXRotRad(mtxRotY, 'y', 0.017453292f * entry->rotation.y);
				PSMTXRotRad(mtxRotZ, 'z', 0.017453292f * entry->rotation.z);
				PSMTXIdentity(mtx);
				PSMTXConcat(mtxRotZ, mtxScale, mtx);
				PSMTXConcat(mtxTransNeg, mtx, mtx);
				PSMTXConcat(mtxRotZ, mtx, mtx);
				PSMTXConcat(mtxRotY, mtx, mtx);
				PSMTXConcat(mtxRotX, mtx, mtx);
				PSMTXConcat(mtxTransPos, mtx, mtx);
				PSMTXConcat(mtxPos, mtx, mtx);
				btlDispTexPlane2(mtx, entry->texId, entry->color, 0);
			}
			if (entry->turnCount != 0) {
				pos2 = pos;
				pos2.x += 21.6f;
				pos2.y -= 16.0f;
				PSMTXIdentity(mtx2);
				PSMTXTrans(mtxPos, pos2.x, pos2.y, pos2.z);
				PSMTXConcat(mtxPos, mtx2, mtx2);
				iconNumberDispGx(mtx2, entry->turnCount, 1, (GXColor){ 0xff, 0xff, 0xff, 0xff });
			}
		}
	}
}

BOOL _bsi_default_turn(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;

	if (_battleWorkPointer->flags & 0x10000) {
		if (type != 1) {
			if (!BtlUnit_CheckStatus(unit, type)) {
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
		if (!BtlUnit_CheckStatus(unit, type)) {
			return FALSE;
		}
	}

	BtlUnit_GetStatus(unit, type, &turns, &strength);
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	entry->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->color = (GXColor){ 0xff, 0xff, 0xff, 0xff };
	entry->flags = disp->flags;
	entry->offset = (Vec){ 0.0f, 0.0f, 0.0f };
	return TRUE;
}

BOOL _bsi_guruguru(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s8 turns;
	s8 strength;
	s32 turnCount;
	f32* counter = &unit->mStatusIconWork.mGuruguruAngle;

	if (!BtlUnit_CheckStatus(unit, type)) {
		*counter = 0.0f;
		return FALSE;
	}

	*counter += 4.0f;
	BtlUnit_GetStatus(unit, type, &turns, &strength);
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	entry->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->rotation = (Vec){ 0.0f, 0.0f, *counter };
	entry->color = (GXColor){ 0xff, 0xff, 0xff, 0xff };
	entry->flags = disp->flags;
	entry->offset = (Vec){ 0.0f, 4.0f, 0.0f };
	return TRUE;
}

BOOL _bsi_allergy(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = &unit->mStatusIconWork.mAllergyCounter;

	if (!BtlUnit_CheckStatus(unit, type)) {
		*counter = 0;
		return FALSE;
	}

	BtlUnit_GetStatus(unit, type, &turns, &strength);
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	entry->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->color = (GXColor){ 0xff, 0xff, 0xff, 0xff };
	entry->flags = disp->flags;
	entry->offset = (Vec){ 0.0f, 0.0f, 0.0f };

	*counter = *counter + 1;
	if (*counter & 0x10) {
		entry->flags |= 2;
	}
	return TRUE;
}

BOOL _bsi_biribiri(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = &unit->mStatusIconWork.mBiribiriState;
	s16* duration = &unit->mStatusIconWork.mBiribiriDuration;

	if (!BtlUnit_CheckStatus(unit, type)) {
		*counter = 0;
		*duration = 0;
		return FALSE;
	}

	entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };

	switch (*counter) {
	case 0:
		*counter = *counter + 1;
		*duration = irand(6) + 4;
		// fallthrough
	case 1:
		entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
		*duration = *duration - 1;
		if (*duration < 0) {
			*counter = *counter + 1;
			*duration = irand(100) + 30;
		}
		break;
	case 2:
		entry->scale = (Vec){ 0.8f, 0.8f, 1.0f };
		*duration = *duration - 1;
		if (*duration < 0) {
			*duration = irand(6) + 4;
			*counter = 1;
		}
		break;
	}

	BtlUnit_GetStatus(unit, type, &turns, &strength);
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->color = (GXColor){ 0xff, 0xff, 0xff, 0xff };
	entry->flags = disp->flags;
	entry->offset = (Vec){ 0.0f, 0.0f, 0.0f };
	return TRUE;
}

BOOL _bsi_regeneration(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s16* counter;
	s32 turnCount;
	s32 offset;
	s8 turns;
	s8 strength;
	GXColor startColor;
	GXColor endColor;

	if (type == STATUS_HP_REGEN) {
		startColor = (GXColor){ 0xed, 0x14, 0x5b, 0xff };
		endColor = (GXColor){ 0xff, 0xea, 0x05, 0xff };
		counter = &unit->mStatusIconWork.mRegenHpCounter;
	}
	else if (type == STATUS_FP_REGEN) {
		startColor = (GXColor){ 0x3d, 0xd1, 0x09, 0xff };
		endColor = (GXColor){ 0x61, 0xe9, 0xe7, 0xff };
		counter = &unit->mStatusIconWork.mRegenFpCounter;
	}
	else {
		return FALSE;
	}

	if (!BtlUnit_CheckStatus(unit, type)) {
		*counter = 0;
		return FALSE;
	}

	*counter = *counter + 1;
	offset = (*counter * 0xc) % 0x2d0;
	if (offset >= 0x168) {
		offset = 0x2d0 - offset;
	}
	BtlUnit_GetStatus(unit, type, &turns, &strength);
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	entry->pivot = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->rotation = (Vec){ 0.0f, 0.0f, 0.0f };
	entry->color = (GXColor){ intplGetValue(0xb, offset, 0x168, (f32)startColor.r, (f32)endColor.r),
	                          intplGetValue(0xb, offset, 0x168, (f32)startColor.g, (f32)endColor.g),
	                          intplGetValue(0xb, offset, 0x168, (f32)startColor.b, (f32)endColor.b), 0xff };
	entry->flags = disp->flags;
	entry->offset = (Vec){ 0.0f, 0.0f, 0.0f };
	return TRUE;
}
