#include "battle/battle_status_icon.h"
#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "driver/icondrv.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;
extern GXColor dat_804221ec;
extern s32 dat_80422200;
extern s32 dat_80422204;
extern f32 float_0p10207_80422208;
extern s32 dat_8042220c;
extern s32 dat_80422210;

typedef struct IconDispTableEntry IconDispTableEntry;
struct IconDispTableEntry {
	s32 id;
	BOOL (*callback)(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry); //0x4
	s32 texId;
	s32 flags;
};

// .rodata objects of this unit: 22 Vecs, then the int->float conversion doubles.
const Vec vec3_803011e8 = {1.0f, 1.0f, 1.0f};
const Vec vec3_803011f4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_80301200 = {0.0f, 0.0f, 0.0f};
const Vec vec3_8030120c = {0.0f, 0.0f, 0.0f};
const Vec vec3_80301218 = {1.0f, 1.0f, 1.0f};
const Vec vec3_80301224 = {0.0f, 0.0f, 0.0f};
const Vec vec3_80301230 = {0.0f, 0.0f, 0.0f};
const Vec vec3_8030123c = {0.0f, 4.0f, 0.0f};
const Vec vec3_80301248 = {1.0f, 1.0f, 1.0f};
const Vec vec3_80301254 = {0.0f, 0.0f, 0.0f};
const Vec vec3_80301260 = {0.0f, 0.0f, 0.0f};
const Vec vec3_8030126c = {0.0f, 0.0f, 0.0f};
const Vec vec3_80301278 = {1.0f, 1.0f, 1.0f};
const Vec vec3_80301284 = {1.0f, 1.0f, 1.0f};
const Vec vec3_80301290 = {0.8f, 0.8f, 1.0f};
const Vec vec3_8030129c = {0.0f, 0.0f, 0.0f};
const Vec vec3_803012a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_803012b4 = {0.0f, 0.0f, 0.0f};
// The four Vecs _bsi_regeneration copies. Non-const and pinned to .rodata under MUST_MATCH so MWCC
// reaches them (and the u8->f32 mask) through the .rodata base register; see the note above the
// function. Defined const otherwise.
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_803012c0 = {1.0f, 1.0f, 1.0f};
#else
const Vec vec3_803012c0 = {1.0f, 1.0f, 1.0f};
#endif
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_803012cc = {0.0f, 0.0f, 0.0f};
#else
const Vec vec3_803012cc = {0.0f, 0.0f, 0.0f};
#endif
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_803012d8 = {0.0f, 0.0f, 0.0f};
#else
const Vec vec3_803012d8 = {0.0f, 0.0f, 0.0f};
#endif
#ifdef MUST_MATCH
__declspec(section ".rodata") Vec vec3_803012e4 = {0.0f, 0.0f, 0.0f};
#else
const Vec vec3_803012e4 = {0.0f, 0.0f, 0.0f};
#endif

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

#pragma optimize_for_size on // -O4,s: the table loop needs base+index addressing and its register colours
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
#pragma optimize_for_size reset
void BattleStatusIconDelete(BattleUnit* unit) {
	; //stubbed in retail
}

// -O4,s (over the default -O4,p) is required: under -O4,p MWCC folds the mIcons +0x20 array offset
// into the element field displacements (base = icon + off); the original keeps it on the scaled index
// (addi r29,r26,0x20; add r29,r30,r29). The loop must index mIcons directly (and the float use be
// written as i * -2) for the -O4,s strength reduction to produce the original's i/2i/offset counters.
#pragma optimize_for_size on
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

		// btlDispTexPlane is called through a pointer type with an extra trailing s32 argument (0).
		if (!(entry->flags & 1)) {
			if (!(icon->mFlags & 1)) {
#ifdef MUST_MATCH
				((void (*)(u32, GXColor, f32, f32, f32, f32, f32, s32))btlDispTexPlane)(
				    0x23, color, pos.x, pos.y, pos.z, 0.8f, 0.8f, 0);
#else
				btlDispTexPlane(
				    0x23, color, pos.x, pos.y, pos.z, 0.8f, 0.8f);
#endif
			}
			else {
#ifdef MUST_MATCH
				((void (*)(u32, GXColor, f32, f32, f32, f32, f32, s32))btlDispTexPlane)(
				    0x23, color, pos.x, pos.y, pos.z, 0.8f, -0.8f, 0);
#else
				btlDispTexPlane(
				    0x23, color, pos.x, pos.y, pos.z, 0.8f, -0.8f);
#endif
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
				iconNumberDispGx(mtx2, entry->turnCount, 1, dat_804221ec);
			}
		}
	}
}
#pragma optimize_for_size reset

// Icon parameter table base; fields accessed at +0x60..+0x8C are Vec entries 8..11.
// -1 sentinel written to entry+0x3C.
extern s32 dat_804221fc;
extern s32 dat_804221f8;
extern s32 dat_804221f4;
extern s32 dat_804221f0;

BOOL _bsi_default_turn(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	const Vec* vec = &vec3_803011e8;
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
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = vec[0];
	entry->pivot = vec[1];
	entry->rotation = vec[2];
	entry->color = *(GXColor*)&dat_804221f0;
	entry->flags = disp->flags;
	entry->offset = vec[3];
	return TRUE;
}

BOOL _bsi_guruguru(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	const Vec* vec = &vec3_803011e8;
	s32 type = disp->id;
	s8 turns;
	s8 strength;
	s32 turnCount;
	Vec tmp;
	f32* counter = &unit->mStatusIconWork.mGuruguruAngle;

#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = 0.0f;
		return FALSE;
	}

	*counter += 4.0f;
#ifdef MUST_MATCH
	((void(*)(BattleUnit*, s8, s8*, s8*))BtlUnit_GetStatus)(unit, type, &turns, &strength);
#else
	BtlUnit_GetStatus(unit, type, &turns, &strength);
#endif
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = vec[4];
	entry->pivot = vec[5];
	// MWCC forwards vec[6].x/.y straight to the entry but keeps .z in a stack temporary which the
	// counter store then reuses; assigning entry->rotation and then .z omits that temporary.
	tmp = vec[6];
	tmp.z = *counter;
	entry->rotation = tmp;
	entry->color = *(GXColor*)&dat_804221f4;
	entry->flags = disp->flags;
	entry->offset = vec[7];
	return TRUE;
}

BOOL _bsi_allergy(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	const Vec* vec = &vec3_803011e8;
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = &unit->mStatusIconWork.mAllergyCounter;

#ifdef MUST_MATCH
	// The header's BtlUnit_CheckStatus/BtlUnit_GetStatus take StatusEffectType (int), but the
	// callee sign-extends it (battle_status_effect.h says it "should be an s8"). Calling through
	// an s8-typed function pointer makes MWCC coerce the argument at each call like the original;
	// an (s8) cast on the argument is hoisted into its own register and does not match.
	// The same casts are used in every _bsi_* function below.
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
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = vec[8];
	entry->pivot = vec[9];
	entry->rotation = vec[10];
	entry->color = *(GXColor*)&dat_804221f8;
	entry->flags = disp->flags;
	entry->offset = vec[11];

	*counter = *counter + 1;
	if (*counter & 0x10) {
		entry->flags |= 2;
	}
	return TRUE;
}

BOOL _bsi_biribiri(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	const Vec* vec = &vec3_803011e8;
	s32 type = disp->id;
	s32 turnCount;
	s8 turns;
	s8 strength;
	s16* counter = &unit->mStatusIconWork.mBiribiriState;
	s16* duration = &unit->mStatusIconWork.mBiribiriDuration;

#ifdef MUST_MATCH
	if (!((BOOL(*)(BattleUnit*, s8))BtlUnit_CheckStatus)(unit, type)) {
#else
	if (!BtlUnit_CheckStatus(unit, type)) {
#endif
		*counter = 0;
		*duration = 0;
		return FALSE;
	}

	entry->scale = vec[12];

	switch (*counter) {
	case 0:
		*counter = *counter + 1;
		*duration = irand(6) + 4;
		// fallthrough
	case 1:
		entry->scale = vec[13];
		*duration = *duration - 1;
		if (*duration < 0) {
			*counter = *counter + 1;
			*duration = irand(100) + 30;
		}
		break;
	case 2:
		entry->scale = vec[14];
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
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->pivot = vec[15];
	entry->rotation = vec[16];
	entry->color = *(GXColor*)&dat_804221fc;
	entry->flags = disp->flags;
	entry->offset = vec[17];
	return TRUE;
}

// The original copies four Vecs straight out of .rodata (vec3_803012c0..vec3_803012e4) and reads
// the u8->f32 mask as `lfd f2, 0x108(r31)`, i.e. through the .rodata base register r31 that also
// serves the copies. A const source lets MWCC fold the copy to an absolute relocation and pool its
// own copy of the mask instead, so the four objects below are non-const and pinned to .rodata under
// MUST_MATCH (like itemdrv's vec3_802cac1c): that makes the compiler materialise each copy's source
// address from the .rodata base and reuse it for the mask, and keeps the target's load/store
// interleaving. The values and therefore the .rodata bytes are unchanged.
#pragma optimize_for_size on // the target's modulo/division by 0x2d0 is a real divw, not the magic-multiply form
BOOL _bsi_regeneration(BattleUnit* unit, IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry) {
	s32 type = disp->id;
	s16* counter;
	s32 turnCount;
	s32 offset;
	s8 turns;
	s8 strength;
	GXColor startColor;
	GXColor endColor;
	GXColor color;

	if (type == STATUS_HP_REGEN) {
		startColor = *(GXColor*)&dat_80422200;
		endColor = *(GXColor*)&dat_80422204;
		counter = &unit->mStatusIconWork.mRegenHpCounter;
	}
	else if (type == STATUS_FP_REGEN) {
		startColor = *(GXColor*)&float_0p10207_80422208;
		endColor = *(GXColor*)&dat_8042220c;
		counter = &unit->mStatusIconWork.mRegenFpCounter;
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
	entry->type = type;
	turnCount = turns >= 0 ? turns : -turns;
	entry->turnCount = turnCount;
	entry->texId = disp->texId;
	entry->scale = vec3_803012c0;
	entry->pivot = vec3_803012cc;
	entry->rotation = vec3_803012d8;
	color = *(GXColor*)&dat_80422210;
	color.r = intplGetValue(0xb, offset, 0x168, (f32)startColor.r, (f32)endColor.r);
	color.g = intplGetValue(0xb, offset, 0x168, (f32)startColor.g, (f32)endColor.g);
	color.b = intplGetValue(0xb, offset, 0x168, (f32)startColor.b, (f32)endColor.b);
	entry->color = color;
	entry->flags = disp->flags;
	entry->offset = vec3_803012e4;
	return TRUE;
}
#pragma optimize_for_size reset

// .sdata2 named objects this unit owns, in the original's address order.
#pragma force_active on
__declspec(section ".sdata2") GXColor dat_804221ec = { 0xff, 0xff, 0xff, 0xff };
__declspec(section ".sdata2") s32 dat_804221f0 = 0xffffffff;
__declspec(section ".sdata2") s32 dat_804221f4 = 0xffffffff;
__declspec(section ".sdata2") s32 dat_804221f8 = 0xffffffff;
__declspec(section ".sdata2") s32 dat_804221fc = 0xffffffff;
__declspec(section ".sdata2") s32 dat_80422200 = 0xed145bff;
__declspec(section ".sdata2") s32 dat_80422204 = 0xffea05ff;
__declspec(section ".sdata2") f32 float_0p10207_80422208 = 0.10206984728574753f;
__declspec(section ".sdata2") s32 dat_8042220c = 0x61e9e7ff;
__declspec(section ".sdata2") s32 dat_80422210 = 0x000000ff;
#pragma force_active reset
