#include "battle/battle_status_icon.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;
extern GXColor dat_804221e8;
extern GXColor dat_804221ec;
extern f32 float_0p8_8042221c;
extern f32 float_neg0p8_80422220;
extern f32 float_deg2rad_80422224;
extern f32 float_21p6_80422228;
extern f32 float_16_8042222c;
extern f32 float_13p44_80422230;
extern f32 float_25p6_80422234;
extern f32 float_19p2_80422238;
extern f32 float_901_8042223c;
extern f32 float_0_80422214;
extern s32 dat_80422200;
extern s32 dat_80422204;
extern f32 float_0p10207_80422208;
extern s32 dat_8042220c;
extern s32 dat_80422210;

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

// NONMATCHING (86.59%): the callback is passed &icon->mIcons[i] here, but the original indexes by
// icon->mNumStatusIcons (mulli from 0x794(r31)); using the correct index scores 79.74 because the
// full 23-entry _disp_table changes _disp_table's addressing, so the table below is still truncated
// to one entry. The rest is register/scheduling differences (the original keeps unit/icon/table/i in
// r30/r31/r28/r27 and uses stmw).
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

// NONMATCHING (98.51%): MWCC folds the mIcons array offset 0x20 into the element field offsets
// (base = icon + off) where the original adds it to the scaled index first (addi r29,r26,0x20; add
// r29,r30,r29). Same addresses; the int->float 2^52 pool entry is an anonymous @NNL relocation name.
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
#ifdef MUST_MATCH
	s32 two;
	s32 off;
#endif

	color = dat_804221e8;
	btlGetScreenPoint(&icon->mPosition, &screenPos);
#ifdef MUST_MATCH
	for (i = icon->mNumStatusIcons - 1, off = i * 0x44, two = i * 2; i >= 0; i--, two -= 2, off -= 0x44) {
		entry = (BattleWorkStatusIconEntry*)((u8*)&icon->mIcons[0] + off);
#else
	for (i = icon->mNumStatusIcons - 1; i >= 0; i--) {
		entry = &icon->mIcons[i];
#endif
		pos = screenPos;
		pos.y = icon->mDistanceBetweenIcons * (f32)i + pos.y;
		pos.x += icon->mScreenSpaceOffset.x;
		pos.y += icon->mScreenSpaceOffset.y;
		pos.z += icon->mScreenSpaceOffset.z;
#ifdef MUST_MATCH
		pos.z -= (f32)(-two);
#else
		pos.z -= (f32)(i * -2);
#endif

		if (!(*(s32*)((u8*)entry + 0x40) & 1)) {
			if (!(icon->mFlags & 1)) {
#ifdef MUST_MATCH
				((void (*)(u32, GXColor, f32, f32, f32, f32, f32, s32))btlDispTexPlane)(
				    0x23, color, pos.x, pos.y, pos.z, float_0p8_8042221c, float_0p8_8042221c, 0);
#else
				btlDispTexPlane(0x23, color, pos.x, pos.y, pos.z, float_0p8_8042221c, float_0p8_8042221c);
#endif
			}
			else {
#ifdef MUST_MATCH
				((void (*)(u32, GXColor, f32, f32, f32, f32, f32, s32))btlDispTexPlane)(
				    0x23, color, pos.x, pos.y, pos.z, float_0p8_8042221c, float_neg0p8_80422220, 0);
#else
				btlDispTexPlane(0x23, color, pos.x, pos.y, pos.z, float_0p8_8042221c, float_neg0p8_80422220);
#endif
			}
		}

		if (*(u32*)((u8*)entry + 0x8) != 0) {
			if (!(*(s32*)((u8*)entry + 0x40) & 2)) {
				PSMTXTrans(mtxPos, pos.x + *(f32*)((u8*)entry + 0x30),
				           pos.y + *(f32*)((u8*)entry + 0x34),
				           pos.z + *(f32*)((u8*)entry + 0x38));
				PSMTXScale(mtxScale, float_0p8_8042221c * *(f32*)((u8*)entry + 0xC),
				           float_0p8_8042221c * *(f32*)((u8*)entry + 0x10),
				           *(f32*)((u8*)entry + 0x14));
				PSMTXTrans(mtxTransNeg, -*(f32*)((u8*)entry + 0x18),
				           -*(f32*)((u8*)entry + 0x1C),
				           -*(f32*)((u8*)entry + 0x20));
				PSMTXTrans(mtxTransPos, *(f32*)((u8*)entry + 0x18),
				           *(f32*)((u8*)entry + 0x1C),
				           *(f32*)((u8*)entry + 0x20));
				PSMTXRotRad(mtxRotX, 'x', float_deg2rad_80422224 * *(f32*)((u8*)entry + 0x24));
				PSMTXRotRad(mtxRotY, 'y', float_deg2rad_80422224 * *(f32*)((u8*)entry + 0x28));
				PSMTXRotRad(mtxRotZ, 'z', float_deg2rad_80422224 * *(f32*)((u8*)entry + 0x2C));
				PSMTXIdentity(mtx);
				PSMTXConcat(mtxRotZ, mtxScale, mtx);
				PSMTXConcat(mtxTransNeg, mtx, mtx);
				PSMTXConcat(mtxRotZ, mtx, mtx);
				PSMTXConcat(mtxRotY, mtx, mtx);
				PSMTXConcat(mtxRotX, mtx, mtx);
				PSMTXConcat(mtxTransPos, mtx, mtx);
				PSMTXConcat(mtxPos, mtx, mtx);
				btlDispTexPlane2(mtx, *(u32*)((u8*)entry + 0x8), *(GXColor*)((u8*)entry + 0x3C), 0);
			}
			if (*(s32*)((u8*)entry + 0x4) != 0) {
				pos2 = pos;
				pos2.x += float_21p6_80422228;
				pos2.y -= float_16_8042222c;
				PSMTXIdentity(mtx2);
				PSMTXTrans(mtxPos, pos2.x, pos2.y, pos2.z);
				PSMTXConcat(mtxPos, mtx2, mtx2);
				iconNumberDispGx(mtx2, *(s32*)((u8*)entry + 0x4), 1, dat_804221ec);
			}
		}
	}
}

// Icon parameter table base; fields accessed at +0x60..+0x8C are Vec entries 8..11.
extern Vec vec3_803011e8[];
// -1 sentinel written to entry+0x3C.
extern s32 dat_804221fc;
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

extern Vec vec3_80301278;
extern Vec vec3_80301284;
extern Vec vec3_80301290;
extern Vec vec3_8030129c;
extern Vec vec3_803012a8;
extern Vec vec3_803012b4;
// MUST_MATCH: the original reaches these six Vecs as offsets from vec3_803011e8.
#ifdef MUST_MATCH
#define BIRI_VEC(k, sym) vec[k]
#else
#define BIRI_VEC(k, sym) (sym)
#endif

BOOL _bsi_biribiri(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
#ifdef MUST_MATCH
	Vec* vec = vec3_803011e8;
#endif
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = (s16*)(unit->mStatusIconWork.field_0x796 + 6); //per-status animation state
	s16* duration = (s16*)(unit->mStatusIconWork.field_0x796 + 8); //frames left in the state

#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = 0;
		*duration = 0;
		return FALSE;
	}

	*(Vec*)((u8*)entry + 0xC) = BIRI_VEC(12, vec3_80301278);

	switch (*counter) {
	case 0:
		*counter = *counter + 1;
		*duration = irand(6) + 4;
		// fallthrough
	case 1:
		*(Vec*)((u8*)entry + 0xC) = BIRI_VEC(13, vec3_80301284);
		*duration = *duration - 1;
		if (*duration < 0) {
			*counter = *counter + 1;
			*duration = irand(100) + 30;
		}
		break;
	case 2:
		*(Vec*)((u8*)entry + 0xC) = BIRI_VEC(14, vec3_80301290);
		*duration = *duration - 1;
		if (*duration < 0) {
			*duration = irand(6) + 4;
			*counter = 1;
		}
		break;
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
	*(Vec*)((u8*)entry + 0x18) = BIRI_VEC(15, vec3_8030129c);
	*(Vec*)((u8*)entry + 0x24) = BIRI_VEC(16, vec3_803012a8);
	*(s32*)((u8*)entry + 0x3C) = dat_804221fc;
	*(s32*)((u8*)entry + 0x40) = disp->something2;
	*(Vec*)((u8*)entry + 0x30) = BIRI_VEC(17, vec3_803012b4);
	return TRUE;
}

// The remaining differences are the three intplGetValue calls: MWCC materialises the 2^52
// unsigned-int->float mask via an anonymous pool entry where the original loads it as
// vec3_803011e8 + 0x108 (lfd 0x108(r31)), and the fctiwz/byte scheduling differs.
// optimize_for_size: the original's real divw for the modulo only comes out under -O4,s.
// NONMATCHING (87.21%): under -O4,p MWCC strength-reduces the % 0x2d0 to a magic multiply, but the
// original emits a real divw (only -O4,s does that), so this function is compiled size-optimised.
BOOL _bsi_regeneration(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	Vec* vec = vec3_803011e8;
	s32 type = disp->id;
	s16* counter;
	s32 turnCount;
	s32 offset;
	s8 turns;
	s8 strength;
	GXColor startColor;
	GXColor endColor;
	GXColor color;

	if (type == 0x17) {
		startColor = *(GXColor*)&dat_80422200;
		endColor = *(GXColor*)&dat_80422204;
		counter = (s16*)(unit->mStatusIconWork.field_0x796 + 2);
	}
	else if (type == 0x18) {
		startColor = *(GXColor*)&float_0p10207_80422208;
		endColor = *(GXColor*)&dat_8042220c;
		counter = (s16*)(unit->mStatusIconWork.field_0x796 + 4);
	}
	else {
		return FALSE;
	}

#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = 0;
		return FALSE;
	}

	*counter = *counter + 1;
	offset = (*counter * 0xc) % 0x2d0;
	if (offset >= 0x168) {
		offset = 0x2d0 - offset;
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
	*(Vec*)((u8*)entry + 0xC) = vec[18];
	*(Vec*)((u8*)entry + 0x18) = vec[19];
	*(Vec*)((u8*)entry + 0x24) = vec[20];
	color = *(GXColor*)&dat_80422210;
	color.r = intplGetValue(0xb, offset, 0x168, (f32)startColor.r, (f32)endColor.r);
	color.g = intplGetValue(0xb, offset, 0x168, (f32)startColor.g, (f32)endColor.g);
	color.b = intplGetValue(0xb, offset, 0x168, (f32)startColor.b, (f32)endColor.b);
	*(GXColor*)((u8*)entry + 0x3C) = color;
	*(s32*)((u8*)entry + 0x40) = disp->something2;
	*(Vec*)((u8*)entry + 0x30) = vec[21];
	return TRUE;
}
