#include "battle/battle_audience.h"
#include "battle/battle.h"
#include "battle/battle_audience_kind_data.h"
#include "battle/battle_break_slot.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "math_ext.h"
#include "system.h"
#include <string.h>
#include "pmario_sound.h"
#include "statuswindow.h"
#include "battle/battle_weapon_power.h"
#include <stdio.h>
#include <stdlib.h>
#include "battle/battle_audience_kinopio.h"
#include "data/item_icon_view.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "driver/swdrv.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_80399940[2] = { 0xBF800000, 0x3F800000 };

extern u8 weapon_data_akikan[];
extern u8 weapon_data_brick[];
extern u8 weapon_data_bone[];
extern u8 weapon_data_hammer[];

#ifdef MUST_MATCH
// For the struct-copy that BattleAudience_Case_FallObject_Aud uses to load its two const offset
// tables; MWCC only emits the paired lwzu/stwu copy loop for a struct assignment, not for memcpy.
typedef struct BattleAudienceFallOffsetTable1 {
    s32 v[16];
} BattleAudienceFallOffsetTable1;

typedef struct BattleAudienceFallOffsetTable2 {
    s32 v[48];
} BattleAudienceFallOffsetTable2;
#endif


// LOCAL_PROTO(psndSFX_get_vol): retail battle callers consume a full-width result; sharing the u8 implementation signature adds a mask and regresses BtlUnit_snd_se_pos.
extern s32 psndSFX_get_vol(s32 id);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
extern s32 psndSFX_pit(s32 id, s32 pit);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, s32 vol);


const char str_802F9578[] = "btl_msg_audience_puni_escape";
const char str_802F9598[] = "btl_msg_audience_puni_meeting";
const char str_802F95B8[] = "btl_msg_audience_pansy_sleep";
const char str_802F95D8[] = "btl_msg_audience_heavybomb_dynamite";

const f32 R_audience_delta_multiplier[] = {
	8.0f, 11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 26.0f, 29.0f, 32.0f, 35.0f, 38.0f, 41.0f, 44.0f,
	47.0f, 50.0f, 53.0f, 56.0f, 59.0f, 62.0f, 65.0f, 68.0f, 71.0f, 74.0f, 77.0f, 80.0f, 83.0f,
	86.0f, 89.0f, 92.0f, 95.0f, 98.0f, 101.0f, 104.0f, 107.0f, 110.0f, 113.0f, 116.0f, 119.0f,
	122.0f, 125.0f, 128.0f, 131.0f, 134.0f, 137.0f, 140.0f, 143.0f, 146.0f, 149.0f, 152.0f, 155.0f,
	158.0f, 161.0f, 164.0f, 167.0f, 170.0f, 173.0f, 176.0f, 179.0f, 182.0f, 185.0f, 188.0f, 191.0f,
	194.0f, 197.0f, 200.0f,
};

const s32 R_num_audience_type_weights[] = { 20, 40, 20, 5, 3, 2, 1, 1, 1, 1, 1 };

// Match-only: the original's vec3_802f972c is a zero Vec the case-0 block copies as an
// rvalue aggregate.  Expressing it as a compound literal makes MWCC pool the template object
// at this source position, which is exactly the deleted vec3_802f972c slot in .rodata, and
// reproduces the original case-0 load/store schedule (a named load schedules the arithmetic
// z,x,y instead of the original y,z,x).  The helper must stay here, not above the function:
// the template's .rodata offset follows the helper's source position, not the call site.
static inline Vec battleAudienceItemZeroVec(void) { return (Vec){ 0.0f, 0.0f, 0.0f }; }
#ifndef MUST_MATCH
const Vec vec3_802f972c = { 0.0f, 0.0f, 0.0f };
#endif
const Vec vec3_802f9738 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9744 = { 0.9f, 0.9f, 0.9f };
// Match-only: reading these through a near-data constant lands the loads in the original's
// registers (a direct `vec3_802f9744` copy compiles to `lwzu`, a pooled zero schedules late, and
// the pooled 360 in DispAudience lands in a different register).
static const Vec* const p_vec3_802f9744 = &vec3_802f9744;
static const f32 zero_8041e618 = 0.0f;
static const f32 f360_8041e6b4 = 360.0f;
const Vec vec3_802f9750 = { 0.75f, 0.75f, 0.75f };
const Vec vec3_802f975c = { -96.0f, 106.0f, 0.0f };
const Vec vec3_802f9768 = { 0.0f, 0.0f, -1.0f };
const f32 dat_802f9774[] = {
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.075f, 0.0f, 0.075f, 1.0f, 0.075f, 1.0f, 1.075f,
	0.0f, 0.075f, 0.0f, 1.075f, 1.0f, 1.075f, 0.0f, 1.075f,
};
const Vec vec3_802f97f4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9800 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f980c = { 0.0f, 0.0f, -1.0f };
const Vec vec3_802f9818 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9824 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9830 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f983c = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f9848 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9854 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9860 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f986c = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f9878 = { 0.0f, 0.0f, 0.0f };
const s32 dat_802f9884[] = { 10000, 1000, 100, 10, 5, 2, 1, 1, 1, 1 };
const s32 dat_802f98ac[] = { 1, 1, 2, 2, 4, 4, 8, 8, 16, 16, 16, 16, 8, 8, 4, 4, 2, 2, 1, 1 };
const s32 dat_802f98fc[] = { -1, 1, 0, 1, 1, 1, -1, 0, 1, 0, -1, -1, 0, -1, 1, -1 };
const s32 dat_802f993c[] = {
	0, 3, -1, 2, 0, 2, 1, 2, -2, 1, -1, 1, 0, 1, 1, 1, 2, 1, -3, 0, -2, 0, -1, 0, 1, 0, 2, 0, 3, 0, -2, -1,
	-1, -1, 0, -1, 1, -1, 2, -1, -1, -2, 0, -2, 1, -2, 0, -3,
};
const f64 double_to_int_802f9a00 = 4503601774854144.0;
const f64 double_to_int_mask_802f9a08 = 4503599627370496.0;

const char str_802F9A10[] = "SFX_AUDIENCE_PAKU_EAT1";
const char str_802F9A28[] = "SFX_AUDIENCE_BOMB_BURST1";
const char str_802F9A44[] = "SFX_AUDIENCE_BOMB_READY1";
const char str_802F9A60[] = "SFX_AUDIENCE_SHELL_TRANSFORM1";
const char str_802F9A80[] = "SFX_AUDIENCE_PANSY_SINGING1";
const char str_802F9A9C[] = "SFX_AUDIENCE_ZZZ1";
const char str_802F9AB0[] = "SFX_AUDIENCE_SLEEP1";
const char str_802F9AC4[] = "SFX_AUDIENCE_SLEEP2";
const char str_802F9AD8[] = "SFX_AUDIENCE_SLEEP3";
const char str_802F9AEC[] = "SFX_AUDIENCE_RUN1";
const char str_802F9B00[] = "SFX_AUDIENCE_RUN2";
const char str_802F9B14[] = "SFX_AUDIENCE_THROW_OBJECT1";
const char str_802F9B30[] = "SFX_AUDIENCE_THROW_OBJECT2";
const char str_802F9B4C[] = "SFX_AUDIENCE_HANDBEAT1";
const char str_802F9B64[] = "SFX_AUDIENCE_CALL_ALLY1";
const char str_802F9B7C[] = "SFX_AUDIENCE_CALL_ALLY2";
const char str_802F9B94[] = "SFX_AUDIENCE_WHISTLE1";
const char str_802F9BAC[] = "SFX_AUDIENCE_WHISTLE2";
const char str_802F9BC4[] = "SFX_AUDIENCE_WHISTLE3";
const char str_802F9BDC[] = "SFX_AUDIENCE_CHEER_ALLY1";
const char str_802F9BF8[] = "SFX_AUDIENCE_CHEER_ALLY2";
const char str_802F9C14[] = "SFX_AUDIENCE_CHEER_ALLY3";
const char str_802F9C30[] = "SFX_AUDIENCE_CHEER_ALLY4";
const char str_802F9C4C[] = "SFX_AUDIENCE_NOISY1";
const char str_802F9C60[] = "SFX_AUDIENCE_NOISY2";
const char str_802F9C74[] = "SFX_AUDIENCE_NOISY3";
const char str_802F9C88[] = "SFX_AUDIENCE_NOISY4";
const char str_802F9C9C[] = "SFX_AUDIENCE_CLAPPING1";
const char str_802F9CB4[] = "SFX_AUDIENCE_CLAPPING2";
const char str_802F9CCC[] = "SFX_AUDIENCE_CLAPPING3";
const char str_802F9CE4[] = "SFX_AUDIENCE_CLAPPING4";
const char str_802F9CFC[] = "SFX_AUDIENCE_BOO_ALLY1";
const char str_802F9D14[] = "SFX_AUDIENCE_BOO_ALLY2";
const char str_802F9D2C[] = "SFX_AUDIENCE_BOO_ALLY3";
const char str_802F9D44[] = "SFX_AUDIENCE_BOO_ALLY4";
const char str_802F9D5C[] = "SFX_AUDIENCE_BOO_ENEMY1";
const char str_802F9D74[] = "SFX_AUDIENCE_BOO_ENEMY2";
const char str_802F9D8C[] = "SFX_AUDIENCE_BOO_ENEMY3";
const char str_802F9DA4[] = "SFX_AUDIENCE_BOO_ENEMY4";
const char str_802F9DBC[] = "SFX_BTL_SYS_AP_STAR1";
const char str_802F9DD4[] = "battle/audience/%s";
const char str_802F9DE8[] = "battle/audience/audience_normal.tpl";
const char str_802F9E0C[] = "audience_luigi.tpl";

f32 float_1_8041e620 = 1.0f;
f32 float_neg1_8041e664 = -1.0f;
f32 float_0p25_8041e63c = 0.25f;
f32 float_200_8041e648 = 200.0f;
f32 float_26_8041e650 = 26.0f;
f32 float_75_8041e658 = 75.0f;
f32 float_63_8041e644 = 63.0f;
f32 float_250_8041e654 = 250.0f;
f32 float_100_8041e64c = 100.0f;
f32 float_3_8041e65c = 3.0f;
f32 float_2_8041e660 = 2.0f;
f32 float_0_8041e618 = 0.0f;
f32 float_0p05_8041e624 = 0.049999997f;
f32 float_0p15_8041e630 = 0.14999999f;
f32 float_neg0p25_8041e640 = -0.25f;
f32 float_neg0p05_8041e614 = -0.049999997f;
f32 float_neg0p1_8041e634 = -0.099999994f;
f32 float_neg0p2_8041e638 = -0.19999999f;
f32 float_0p02_8041e61c = 0.02f;
f32 float_5_8041e62c = 5.0f;
f32 float_0p01_8041e628 = 0.01f;
f32 float_1p5_8041e670 = 1.5f;

// These .sdata2/.sdata objects live in this unit but are not defined yet.
// Declared extern so PerAct/ApSrcCtrlProcess can reference them.
extern f32 float_neg5_8041e710;
extern f32 float_25_8041e6dc;
extern f32 float_40_8041e690;
extern f32 float_20_8041e6a8;
extern f32 float_15_8041e6f4;
extern f32 float_neg15_8041e714;
extern f32 float_neg20_8041e6a0;
extern f32 float_neg10_8041e6fc;
extern f32 float_30_8041e6f0;
extern f32 float_64_8041e6d4;
extern f32 float_50_8041e6d8;
extern f32 float_32767_8041e674;
extern f32 float_0p9_8041e6e0;
extern f32 float_0p8_8041e69c;
extern f32 float_6p2832_8041e6e8;
extern f32 float_0p2_8041e67c;
extern f32 float_0p05_8041e6e4;
extern f32 float_128_8041e6ec;
extern s32 msg_puni_all_escape[9];

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

extern BattleWork* _battleWorkPointer;
extern f32 float_10_8041e694;
extern f32 float_7_8041e6f8;
extern f32 float_18_8041e700;
extern f32 float_neg0p2_8041e704;
extern f32 float_0p1_8041e678;
extern f32 float_neg0p1_8041e6bc;
extern f32 float_neg500_8041e6c0;
extern f32 float_neg280_8041e6c4;
extern f32 float_neg160_8041e6c8;
extern f32 float_deg2rad_8041e6a4;
extern u32 dat_8041e608;
extern f32 float_neg2000_8041e680;
extern f32 float_270_8041e684;

//local prototypes


#pragma dont_inline on
FileEntry* tplRead(const char* path) {
	return fileAllocf(4, "%s/%s", getMarioStDvdRoot(), path);
}

BattleAudience* BattleAudienceBaseGetPtr(void) {
	return &_battleWorkPointer->audience;
}

BattleAudienceMember* BattleAudienceGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->members[id];
}

BattleAudienceItem* BattleAudienceItemGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->items[id];
}

BattleAudienceWindow* BattleAudienceWinGetPtr(void) {
	return &BattleAudienceBaseGetPtr()->window;
}

BattleAudienceSound* BattleAudienceSoundGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->sounds[id];
}
#pragma dont_inline reset

// MUST_MATCH: the retail Init passes an unsigned index to GetExist (same asm, different scheduling).
#ifdef MUST_MATCH
#define INIT_EXIST_ARG(i) ((u32)(i))
#else
#define INIT_EXIST_ARG(i) (i)
#endif
void BattleAudience_Init(void) {
	BattleAudience* audience;
	BattleAudienceWindow* window;
	int i, j, index;
	u8 status;

	audience = BattleAudienceBaseGetPtr();
	window = BattleAudienceWinGetPtr();
	pouchGetPtr(); //useless call
	memset(audience, 0, sizeof(BattleAudience));
	BattleAudienceSettingAudience();
	audience->normalTex = tplRead(str_802F9DE8);
	BattleAudienceGuestTPLRead(0, AUDIENCE_LUIGI, str_802F9E0C);
	BattleAudienceSetThrowItemMax();

	audience->mTurnEndPhaseEventChance = 0;

	audience->mCurrentAudienceIntCountLeft = 0;
	audience->mCurrentAudienceIntCountRight = 0;
	audience->mCurrentAudienceIntCount = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = (i * 20) + j + 10;
			if ((u8)BattleAudience_GetExist(INIT_EXIST_ARG(index)) == 1) {
				status = BattleAudienceGetPtr(index)->status;
				if (status != 12 && status != 11) { //TODO: add status enum
					audience->mCurrentAudienceIntCountRight++;
				}
			}
		}
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = (i * 20) + j;
			if ((u8)BattleAudience_GetExist(INIT_EXIST_ARG(index)) == 1) {
				status = BattleAudienceGetPtr(index)->status;
				if (status != 12 && status != 11) { //TODO: add status enum
					audience->mCurrentAudienceIntCountLeft++;
				}
			}
		}
	}

	audience->mCurrentAudienceIntCount = audience->mCurrentAudienceIntCountRight + audience->mCurrentAudienceIntCountLeft;
	window->count = (f32)audience->mCurrentAudienceIntCount;
	BattleAudienceSoundNoiseAlways();
	BattleAudienceSoundSetVol(2, 60, 180);
	audience->excited = FALSE;
	BattleAudienceNumToTarget();
}

void BattleAudience_Main(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	if (!(audience->flags & 0x10000) && wp->flags & 0x80) {
		BattleAudience_WinSetActive(1);
	}
	BattleAudienceCtrlProcess();
	BattleAudienceItemCtrlProcess();
	BattleAudienceApSrcCtrlProcess();
	BattleAudienceWinCtrlProcess();
	BattleAudienceSoundMain();
	if (wp->flags & 0x80) {
		audience->flags |= 0x10000;
	}
	else {
		audience->flags &= ~0x10000;
	}
}

// Keep BattleAudience_ActInit out of line: the original PerAct emits `bl BattleAudience_ActInit`.
#pragma dont_inline on
void BattleAudience_ActInit(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	audience->mNumStylishCommandsThisAttack = 0;
	audience->flags &= ~0x10;
	audience->flags &= ~0x20;
	audience->flags &= ~0x40;
	audience->flags &= ~0x80;
	audience->flags &= ~0x100;
	audience->flags &= ~0x200;
	audience->flags &= ~0x400;
	audience->flags &= ~0x800;
	audience->flags &= ~0x1000;
}
#pragma dont_inline reset

#pragma opt_common_subs off // needed: with CSE on MWCC keeps audience+0x10000 in a callee-saved register
void BattleAudience_PerAct(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	StarPowerInfo* info;
	u32 flags;
	s32 flag0, flag1, flag2;
	f32 delta;
	f32 value;
	BOOL positive;

	pouchGetPtr();
	delta = float_0_8041e618;
	value = delta;
	if (audience->flags & 0x100) {
		if (!(audience->flags & 0x200)) {
			BattleAudienceCheer(3);
		}
	}
	else {
		BattleAudienceCheer(2);
	}
	if (audience->flags & 0x40000) {
		return;
	}
	if (!(audience->flags & 0x400) && (audience->flags & 0x800)) {
		delta -= float_5_8041e62c;
		BattleAudienceAddPhaseEvtList(5);
	}
	if (*(StarPowerInfo**)((u8*)audience + 0x137C8) != NULL && ((flag0 = (flags = audience->flags) & 0x10) || (flags & 0x20))) {
		if (flag0) {
			positive = TRUE;
			*(s32*)((u8*)audience + 0x137CC) = *(s32*)((u8*)audience + 0x137CC) + 1;
			if (*(s32*)((u8*)audience + 0x137CC) > 2) {
				*(s32*)((u8*)audience + 0x137CC) = 2;
			}
			value = (f32)*(s32*)((u8*)audience + 0x137CC) * (*(StarPowerInfo**)((u8*)audience + 0x137C8))->mSpAcSuccessMultiplier;
			*(s32*)((u8*)audience + 0x137D0) = 0;
			delta += value;
		}
		else {
			if (!(flags & 0x1000)) {
				*(s32*)((u8*)audience + 0x137CC) = 0;
				positive = FALSE;
				*(s32*)((u8*)audience + 0x137D0) = *(s32*)((u8*)audience + 0x137D0) + 1;
				if (*(s32*)((u8*)audience + 0x137D0) > 5) {
					*(s32*)((u8*)audience + 0x137D0) = 5;
				}
				value = float_neg5_8041e710;
				delta += value;
			}
		}
	}
	if ((flag1 = audience->flags & 0x40) || (audience->flags & 0x80)) {
		if (flag1) {
			positive = TRUE;
			*(s32*)((u8*)audience + 0x137CC) = *(s32*)((u8*)audience + 0x137CC) + 1;
			if (*(s32*)((u8*)audience + 0x137CC) > 2) {
				*(s32*)((u8*)audience + 0x137CC) = 2;
			}
			value = (f32)*(s32*)((u8*)audience + 0x137CC);
			*(s32*)((u8*)audience + 0x137D0) = 0;
			delta += value;
		}
		else {
			*(s32*)((u8*)audience + 0x137CC) = 0;
			positive = FALSE;
		}
	}
	if (positive == TRUE) {
		if (value >= float_25_8041e6dc) {
			BattleAudienceAddPhaseEvtList(0);
		}
		if (value >= float_40_8041e690) {
			BattleAudienceAddPhaseEvtList(3);
		}
		if (value >= float_20_8041e6a8) {
			BattleAudienceAddPhaseEvtList(8);
		}
		if (value >= float_15_8041e6f4) {
			BattleAudienceAddPhaseEvtList(13);
		}
	}
	else {
		if (value <= float_neg15_8041e714) {
			BattleAudienceAddPhaseEvtList(1);
		}
		if (value <= float_neg20_8041e6a0) {
			BattleAudienceAddPhaseEvtList(2);
		}
		if (value <= float_neg20_8041e6a0) {
			BattleAudienceAddPhaseEvtList(5);
		}
		if (value <= float_neg15_8041e714) {
			BattleAudienceAddPhaseEvtList(9);
		}
		if (value <= float_neg20_8041e6a0) {
			BattleAudienceAddPhaseEvtList(11);
		}
		if (value <= float_neg10_8041e6fc) {
			BattleAudienceAddPhaseEvtList(13);
		}
	}
	info = *(StarPowerInfo**)((u8*)audience + 0x137C8);
	if (info != NULL && ((flag2 = audience->flags & 0x100) || (audience->flags & 0x200))) {
		if (flag2) {
			value = (f32)(*(u8*)((u8*)info + 8) * (*(s32*)((u8*)audience + 0x137D4) * 2));
			delta += value;
			if (value >= float_30_8041e6f0) {
				BattleAudienceAddPhaseEvtList(0);
			}
			if (value >= float_40_8041e690) {
				BattleAudienceAddPhaseEvtList(3);
			}
			if (value >= float_25_8041e6dc) {
				BattleAudienceAddPhaseEvtList(8);
			}
			if (value >= float_20_8041e6a8) {
				BattleAudienceAddPhaseEvtList(13);
			}
			BattleAudienceAddPuni(0xA);
		}
		if (audience->flags & 0x200) {
			delta -= float_5_8041e62c;
		}
	}
	if (audience->flags & 0x8000) {
		audience->event = evtEntry(msg_puni_all_escape, 0, 0x20);
		audience->flags &= ~0x8000;
	}
	BattleAudienceAddTargetNum(audience->mAudienceDeltaMultiplier * (float_0p01_8041e628 * delta), float_0_8041e618);
	BattleAudience_ActInit();
}
#pragma opt_common_subs reset

BOOL BattleAudience_CheckReaction(void) {
	BattleAudience* audience;
	EventEntry* evt;

	audience = BattleAudienceBaseGetPtr();
	BattleAudienceGetPtr(0); //unused
	evt = audience->event;
	if (evt && evtCheckID(evt->eventId)) {
		return TRUE;
	}
	audience->event = NULL;
	return FALSE;
}

// check_exe_phase_evt_status must be called out of line (26 bl), not inlined: the
// retail callers use its byte result.
#pragma dont_inline on
BOOL check_exe_phase_evt_status(s32 index, s32 type) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if ((s32)member->type != (s32)type) {
		return FALSE;
	}
	if (member->status == 13) {
		return FALSE;
	}
	if (member->status == 12) {
		return FALSE;
	}
	if (member->status == 11) {
		return FALSE;
	}
	if (member->status == 21) {
		return FALSE;
	}
	if (member->status == 22) {
		return FALSE;
	}
	return (member->flags >> 3) & 1;
}
#pragma dont_inline reset

//TODO: re-type "value" for btlseq "states"
void BattleAudience_PerPhase(u32 value) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	s32 i, itemId;

	audience = BattleAudienceBaseGetPtr();
	if (value == 0x4000002 && !(audience->flags & 0x40000)) {
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetExist(i) == 1 && member->status == 7) {
				itemId = member->itemId;
				if (itemId != -1) {
					BattleAudienceItemGetPtr(itemId)->flags = 0;
				}
				member->itemId = -1;
				BattleAudience_ChangeStatus(i, 0);
			}
		}
		for (i = 0; i < 100; i++) {
			if (BattleAudienceItemGetPtr(i)->state <= 1) {
				BattleAudienceItemGetPtr(i)->flags = 0;
			}
		}
	}
	if (value == 0x4000004) {
		BattleAudienceSetThrowItemMax();
		audience->flags |= 8;
		audience->mCheckPhaseReactionState = 0;
	}
}

#pragma opt_common_subs off // needed: with CSE on MWCC keeps audience+0x10000 in a callee-saved register
BOOL BattleAudience_CheckReactionPerPhase(void) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	u8* aud;
	s32 evtList[200];
	s32 phaseList[14];
	s32 i, j, k, n, count, idx;

	audience = BattleAudienceBaseGetPtr();
	aud = (u8*)audience;
	pouchGetPtr();
	if (!(audience->flags & 8)) {
		return FALSE;
	}
	switch (audience->mCheckPhaseReactionState) {
	case 0:
		audience->mCheckPhaseReactionState = 5;
		if (audience->flags & 0x40000) {
			audience->mCheckPhaseReactionState = 15;
			break;
		}
		// fall through
	case 5:
		count = 0;
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->type == AUDIENCE_BULKY_BOBOMB &&
					member->status == 18) {
				member->field_0x12C--;
				if (member->field_0x12C <= 0) {
					BattleAudience_ChangeStatus(i, 19);
					count++;
				}
			}
		}
		if (count >= 1) {
			audience->mCheckPhaseReactionState = 6;
		}
		else {
			audience->mCheckPhaseReactionState = 7;
		}
		// fall through
	case 6:
	case 7:
		for (j = 0; j < 200; j++) {
			member = BattleAudienceGetPtr(j);
			if ((u8)BattleAudience_GetSysCtrl(j) == 1 && member->type == AUDIENCE_BULKY_BOBOMB &&
					member->status == 19) {
				break;
			}
		}
		if (j == 200) {
			if (audience->mCheckPhaseReactionState == 6) {
				audience->mCheckPhaseReactionState = 8;
			}
			else {
				audience->mCheckPhaseReactionState = 13;
			}
		}
		break;
	case 8:
		audience->event = evtEntry(msg_heavy_bomb_fire, 0, 0x20);
		audience->mCheckPhaseReactionState = 9;
		break;
	case 9:
		if (!evtCheckID(audience->event->eventId)) {
			audience->event = NULL;
			audience->mCheckPhaseReactionState = 10;
		}
		break;
	case 10:
		if (audience->flags & 0x8000) {
			audience->event = evtEntry(msg_puni_all_escape, 0, 0x20);
			audience->flags &= ~0x8000;
			audience->mCheckPhaseReactionState = 11;
		}
		else {
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 11:
		if (!evtCheckID(audience->event->eventId)) {
			audience->event = NULL;
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 13:
		memcpy(phaseList, aud + 0x138CC, *(s32*)(aud + 0x13904) * 4);
		n = *(s32*)(aud + 0x13904);
		*(s32*)(aud + 0x13904) = 0;
		for (i = 0; i < n; i++) {
			switch (phaseList[i]) {
			case 0:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 0)) {
						BattleAudienceAddPhaseEvtList(0);
						break;
					}
				}
				break;
			case 1:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 8)) {
						if (BattleAudienceGetPtr(j)->status != 0x11) {
							BattleAudienceAddPhaseEvtList(1);
							break;
						}
					}
				}
				break;
			case 2:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 4)) {
						BattleAudienceAddPhaseEvtList(2);
						break;
					}
				}
				break;
			case 3:
				if (*(s32*)(aud + 0x137B0) != 0) {
					for (j = 0; j < audience->mMaxAudience; ) {
						if (!(u8)BattleAudience_GetExist(j)) {
							BattleAudienceAddPhaseEvtList(3);
							break;
						}
						member = BattleAudienceGetPtr(j);
						if (member->flags & 0x80) {
							j += 2;
						}
						else {
							j += 1;
						}
					}
				}
				break;
			case 4:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 2)) {
						BattleAudienceAddPhaseEvtList(4);
						break;
					}
				}
				break;
			case 5:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 1)) {
						BattleAudienceAddPhaseEvtList(5);
						break;
					}
				}
				break;
			case 6:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 1)) {
						BattleAudienceAddPhaseEvtList(6);
						break;
					}
				}
				break;
			case 7:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 3)) {
						BattleAudienceAddPhaseEvtList(7);
						break;
					}
				}
				break;
			case 8:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 5)) {
						BattleAudienceAddPhaseEvtList(8);
						break;
					}
				}
				break;
			case 9:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if ((u8)check_exe_phase_evt_status(j, 5)) {
							BattleAudienceAddPhaseEvtList(9);
							break;
						}
					}
				}
				break;
			case 10:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 6)) {
						BattleAudienceAddPhaseEvtList(0xA);
						break;
					}
				}
				break;
			case 11:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if ((u8)check_exe_phase_evt_status(j, 9) &&
								BattleAudienceGetPtr(j)->status != 0x12) {
							BattleAudienceAddPhaseEvtList(0xB);
							break;
						}
					}
				}
				break;
			case 12:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if ((u8)check_exe_phase_evt_status(j, 0xB) &&
								BattleAudienceDetectPakkunEatTarget(j) != -1) {
							BattleAudienceAddPhaseEvtList(0xC);
							break;
						}
					}
				}
				break;
			case 13:
				for (j = 0; j <= 59; j++) {
					if ((u8)check_exe_phase_evt_status(j, 0xC)) {
						BattleAudienceAddPhaseEvtList(0xD);
						break;
					}
				}
				break;
			}
		}
		if (*(s32*)(aud + 0x13904) >= 1) {
			*(s32*)(aud + 0x138CC) = ((s32*)(aud + 0x138CC))[irand(*(s32*)(aud + 0x13904))];
			*(s32*)(aud + 0x13904) = 1;
		}
		else {
			*(s32*)(aud + 0x13904) = 0;
		}
		audience->mCheckPhaseReactionState = 12;
		*(s32*)(aud + 0x137E0) = 0;
		// fall through
	case 12:
		if (*(s32*)(aud + 0x13904) == 1) {
		switch (*(s32*)(aud + 0x138CC)) {
		case 0:
			k = 0;
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 0)) {
					evtList[k++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(k)], 0, 1);
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 1:
			for (i = 0; i < 200; i++) {
				if ((u8)check_exe_phase_evt_status(i, 8)) {
					if (BattleAudienceGetPtr(i)->status != 0x11) {
						BattleAudience_ChangeStatus(i, 0x11);
						BattleAudienceGetPtr(i)->field_0x12C = 5;
					}
				}
			}
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 2:
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 4)) {
					BattleAudienceItemOn(i, 0xEE, 0x64);
				}
			}
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 3:
			switch (*(s32*)(aud + 0x137E0)) {
			case 0:
				BattleAudienceAddPuni(0xC8);
				*(s32*)(aud + 0x137E0) = 5;
				break;
			case 5:
				for (i = 0; i < 200; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xB) {
						break;
					}
				}
				if (i == 200) {
					*(s32*)(aud + 0x137E0) = 0xA;
				}
				break;
			case 10:
				*(s32*)(aud + 0x137E0) = 0xF;
				audience->event = evtEntry(msg_puni_all_enter, 0, 0x20);
				break;
			case 15:
				if (!evtCheckID(audience->event->eventId)) {
					audience->event = NULL;
					*(s32*)(aud + 0x13904) = 0;
				}
				break;
			}
			break;
		case 4:
			switch (*(s32*)(aud + 0x137E0)) {
			case 5:
				goto scan4;
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if ((u8)check_exe_phase_evt_status(i, 2)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0xE);
				*(s32*)(aud + 0x137E0) = 5;
				goto scan4;
			default:
				if (*(s32*)(aud + 0x137E0) >= 5) {
					break;
				}
				break;
			}
			break;
		scan4:
			for (i = 0; i <= 59; i++) {
				if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xE) {
					break;
				}
			}
			if (i == 0x3C) {
				*(s32*)(aud + 0x13904) = 0;
			}
			break;
		case 5:
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 1)) {
					BattleAudienceItemOn(i, 0xED, 0x64);
				}
			}
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 6:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 1)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0, 1);
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 7:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 3)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0xEF, 1);
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 8:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 5)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0, 1);
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 9:
			switch (*(s32*)(aud + 0x137E0)) {
			case 5:
				goto scan9;
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if ((u8)check_exe_phase_evt_status(i, 5)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0xF);
				*(s32*)(aud + 0x137E0) = 5;
				goto scan9;
			default:
				if (*(s32*)(aud + 0x137E0) >= 5) {
					break;
				}
				break;
			}
			break;
		scan9:
			for (i = 0; i <= 59; i++) {
				if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xF) {
					break;
				}
			}
			if (i == 0x3C) {
				*(s32*)(aud + 0x13904) = 0;
			}
			break;
		case 10:
			switch (*(s32*)(aud + 0x137E0)) {
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if ((u8)check_exe_phase_evt_status(i, 6)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0x10);
				*(s32*)(aud + 0x137E0) = 5;
				// fall through
			case 5:
				for (i = 0; i <= 59; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0x10) {
						break;
					}
				}
				if (i == 0x3C) {
					*(s32*)(aud + 0x137E0) = 0xA;
				}
				break;
			case 10:
				*(s32*)(aud + 0x137E0) = 0xF;
				audience->event = evtEntry(msg_pansy_sing, 0, 0x20);
				break;
			case 15:
				if (!evtCheckID(audience->event->eventId)) {
					audience->event = NULL;
					*(s32*)(aud + 0x13904) = 0;
				}
				break;
			}
			break;
		case 11:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 9) &&
						BattleAudienceGetPtr(i)->status != 0x12) {
					evtList[count++] = i;
				}
			}
			idx = evtList[irand(count)];
			member = BattleAudienceGetPtr(idx);
			BattleAudience_ChangeStatus(idx, 0x12);
			member->field_0x12C = 5;
			*(s32*)(aud + 0x13904) = 0;
			break;
		case 12:
			switch (*(s32*)(aud + 0x137E0)) {
			case 5:
				goto scan12;
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if ((u8)check_exe_phase_evt_status(i, 0xB) &&
							BattleAudienceDetectPakkunEatTarget(i) != -1) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0x14);
				*(s32*)(aud + 0x137E0) = 5;
				goto scan12;
			default:
				if (*(s32*)(aud + 0x137E0) >= 5) {
					break;
				}
				break;
			}
			break;
		scan12:
			for (i = 0; i <= 59; i++) {
				if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0x14) {
					break;
				}
			}
			if (i == 0x3C) {
				*(s32*)(aud + 0x13904) = 0;
			}
			break;
		case 13:
			for (i = 0; i <= 59; i++) {
				if ((u8)check_exe_phase_evt_status(i, 0xC)) {
					BattleAudienceItemOn(i, 0, 1);
					break;
				}
			}
			*(s32*)(aud + 0x13904) = 0;
			break;
		}
		}
		else {
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 15:
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->status == 0xD) {
				*(s32*)(member->field_0xEC + 8) -= 1;
				if (*(s32*)(member->field_0xEC + 8) <= 0) {
					BattleAudience_ChangeStatus(i, 3);
					*(s32*)(member->field_0xEC + 8) = 0;
				}
			}
		}
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->type == 8 &&
					member->status == 0x11) {
				member->field_0x12C--;
				if (member->field_0x12C <= 0) {
					member->field_0x4 = 4;
				}
			}
		}
		BattleBreakSlot_DecBreakTurn();
		// fall through
	case 20:
		audience->flags &= ~8;
		*(s32*)(aud + 0x13904) = 0;
		return FALSE;
	}
	return TRUE;
}
#pragma opt_common_subs reset

void BattleAudience_Disp(void) {
	BattleAudienceBaseGetPtr();
	BattleAudienceAnimProcess();
	if (!(BattleAudienceBaseGetPtr()->flags & 0x20000)) {
		dispEntry(CAMERA_3D, 0.0f, 1, BattleAudienceDispAudience, NULL);
		dispEntry(CAMERA_3D, 0.0f, 2, BattleAudienceDispAudience, NULL);
		dispEntry(CAMERA_3D, 0.0f, 1, BattleAudienceDispItem, NULL);
		dispEntry(CAMERA_2D, 499.0f, 0, BattleAudienceDispApSrc, NULL);
	}
	if (pouchGetPtr()->mStarPowersObtained) {
		dispEntry(CAMERA_2D, 499.0f, 1, BattleAudienceDispWin, NULL);
	}
}

void BattleAudience_End(void) {
	BattleAudience* audience;
	FileEntry* obj;
	int i;

	audience = BattleAudienceBaseGetPtr();
	for (i = 0; i < 2; i++) {
		obj = audience->guestTex[i];
		if (obj) {
			fileFree(obj);
		}
	}

	obj = audience->normalTex;
	if (obj) {
		fileFree(obj);
	}

	if (pouchGetPtr()->mStarPowersObtained) {
		if (audience->mTargetAudienceCount < 0.0f) {
			audience->mTargetAudienceCount = 0.0f;
		}
		if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
			audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
		}
		pouchSetAudienceNum(audience->mTargetAudienceCount);
	}
}

// final dispatch is a `switch`; the clamp index is a separate `deltaIdx` ternary, the first
// shuffle and weight sort use a separate `k` counter, `i = 0` moved before `target = 0`,
// `listTotal` after `kind`, the randomMul cast is inlined into the FMA, the sort's tie test
// reads `weights[j] == weights[k]` and its swap loads `order[j]` first. Left: the sort/miss/list
// callee-saved rotation (a 4-cycle on r21/r23/r28/r31); `diff_shape.py`: same calls/branches.
// NONMATCHING (98.73%): see above.
void BattleAudienceSettingAudience(void) {
	s32 kind;
	s32 deltaIdx;
	s32 i, j, k;
	s32 target;
	s32 targetNum;
	s32 order[12];
	s32 weights[12];
	s32 counts[13];
	s32 typeWeights[11];
	f32 deltaMultiplier[65];
	s32 list[200];
	f32 oldAudience;
	s32 mode;
	s32 miss;
	s32 maxKinds;
	s32 sum;
	s32 total;
	u8* base;
	PouchData* pouch;
	u8* setupBase;
	s32 listTotal;
	BattleAudience* audience;

	base = (u8*)_battleWorkPointer + 0x20000;
	audience = BattleAudienceBaseGetPtr();
	setupBase = (u8*)_battleWorkPointer->fbatInfo->setup + 0xAC;
	pouch = pouchGetPtr();

#ifdef MUST_MATCH
	*(BattleAudienceDeltaMultiplierTable*)deltaMultiplier =
	    *(const BattleAudienceDeltaMultiplierTable*)R_audience_delta_multiplier;
#else
	memcpy(deltaMultiplier, R_audience_delta_multiplier, sizeof(deltaMultiplier));
#endif

	switch (pouch->rank) {
	case 0:
		audience->mMaxAudience = 50;
		break;
	case 1:
		audience->mMaxAudience = 100;
		break;
	case 2:
		audience->mMaxAudience = 150;
		break;
	case 3:
		audience->mMaxAudience = 200;
		break;
	}

	if (pouchGetPtr()->mStarPowersObtained == 0) {
		audience->mMaxAudience = 0;
	}

	deltaIdx = (pouch->level - 1 < 0x41) ? pouch->level - 1 : 0x40;
	audience->mAudienceDeltaMultiplier = deltaMultiplier[deltaIdx];

	if (*(u32*)(base - 0x6F98) & 0x10000) {
		audience->mTargetAudienceCount = (f32) * (s32*)(base - 0x6F8C);
		if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
			audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
		}
	}
	else {
		oldAudience = pouchGetAudienceNum();
		audience->mTargetAudienceCount =
		    float_0p01_8041e628 * ((pouchGetAudienceNum() - audience->mAudienceDeltaMultiplier) * (f32)(irand(10) - 13 + irand(10))) + oldAudience;
	}

	switch (*(s32*)setupBase) {
	case 0:
	case 1:
		audience->mAddedAudienceCount = float_0_8041e618;
		break;
	case 4:
		audience->mAddedAudienceCount = float_0p5_8041e708 * audience->mTargetAudienceCount;
		break;
	case 2:
	case 3:
	case 5:
	case 6:
		audience->mAddedAudienceCount = (f32)audience->mMaxAudience - audience->mTargetAudienceCount;
		break;
	}

	switch (*(s32*)setupBase) {
	case 2:
		for (i = 0; i < 12; i++) {
			if (i == 1 || i == 7) {
				weights[i] = 1;
			}
			else {
				weights[i] = 0;
			}
		}
		mode = 0;
		break;
	case 5:
		for (i = 0; i < 12; i++) {
			if (i == 1) {
				weights[i] = 1;
			}
			else {
				weights[i] = 0;
			}
		}
		mode = 0;
		break;
	case 6:
		for (i = 0; i < 12; i++) {
			if (audience_kind[i].field_0x0 & 1) {
				weights[i] = 2;
			}
			else {
				weights[i] = 1;
			}
		}
		mode = 1;
		break;
	default: {
#ifdef MUST_MATCH
		*(BattleAudienceTypeWeights*)typeWeights = *(const BattleAudienceTypeWeights*)R_num_audience_type_weights;
#else
		memcpy(typeWeights, R_num_audience_type_weights, sizeof(typeWeights));
#endif
		sum = 0;
		j = irand(100);
		maxKinds = 12;
		for (i = 1; i <= 11; i++) {
			sum += typeWeights[i - 1];
			if (j < sum) {
				maxKinds = i;
				break;
			}
		}

		for (i = 0; i < 12; i++) {
			order[i] = i;
		}
		for (k = 0; k < 24; k++) {
			s32 a = irand(12);
			s32 b = irand(12);
			s32 t = order[a];
			order[a] = order[b];
			order[b] = t;
		}

		i = 0;
		target = 0;
		for (; i <= 11; i++) {
			kind = order[i];
			if (target >= maxKinds) {
				weights[kind] = 0;
			}
			else {
				if (((s8*)setupBase)[4 + kind] != 0) {
					weights[kind] = ((s8*)setupBase)[4 + kind] * (irand(10) + 1);
					if (audience_kind[kind].field_0x0 & 1) {
						weights[kind] *= 2;
					}
					target++;
				}
				else {
					weights[kind] = 0;
				}
			}
		}

		if (*(s32*)(base - 0x6F88) != 0) {
			switch (*(s32*)(base - 0x6F88)) {
			case 1:
				kind = 0;
				break;
			case 2:
				kind = 10;
				break;
			case 3:
				kind = 8;
				break;
			case 4:
				kind = 4;
				break;
			case 5:
				kind = 7;
				break;
			case 6:
				kind = 2;
				break;
			case 7:
				kind = 1;
				break;
			case 8:
				kind = 3;
				break;
			case 9:
				kind = 5;
				break;
			case 10:
				kind = 6;
				break;
			case 11:
				kind = 9;
				break;
			case 12:
				kind = 11;
				break;
			}
			for (i = 0; i < 12; i++) {
				if (i == kind) {
					weights[i] = 1;
				}
				else {
					weights[i] = 0;
				}
			}
		}
		mode = 1;
		break;
	}
	}

	memset(counts, 0, sizeof(counts));

	targetNum = (s32)(audience->mTargetAudienceCount + audience->mAddedAudienceCount);
	if (targetNum < 0) {
		targetNum = 0;
	}
	if (targetNum > audience->mMaxAudience) {
		targetNum = audience->mMaxAudience;
	}

	total = 0;
	sum = 0;
	for (i = 0; i < 12; i++) {
		sum += weights[i];
	}

	for (i = 0; i < 12; i++) {
		counts[i] = targetNum * weights[i] / sum;
		if (audience_kind[i].field_0x0 & 1) {
			counts[i] /= 2;
			total += counts[i] * 2;
		}
		else {
			total += counts[i];
		}
	}

	for (i = 0; i < 12; i++) {
		order[i] = i;
	}

	for (k = 0; k <= 10; k++) {
		for (j = k + 1; j <= 11; j++) {
			if ((weights[j] == weights[k] && irand(2) == 0) || weights[k] < weights[j]) {
				s32 t = order[j];
				order[j] = order[k];
				order[k] = t;
			}
		}
	}

	miss = 0;
	for (i = 0; total < targetNum; i = (i + 1) % 12) {
		kind = order[i];
		if (weights[kind] <= 0) {
			continue;
		}
		if (audience_kind[kind].field_0x0 & 1) {
			if (total <= targetNum - 2) {
				counts[kind]++;
				total += 2;
				miss = 0;
			}
			else if (++miss >= 12) {
				break;
			}
		}
		else {
			counts[kind]++;
			total++;
		}
	}

	if (1 == mode && total != 0 && (irand(20) == 0 || (*(u32*)(base - 0x6F98) & 0x40000))) {
		for (i = 0; i < 12; i++) {
			order[i] = i;
		}
		for (i = 0; i < 24; i++) {
			s32 a = irand(12);
			s32 b = irand(12);
			s32 t = order[a];
			order[a] = order[b];
			order[b] = t;
		}
		for (i = 0; i < 12; i++) {
			kind = order[i];
			if (counts[kind] > 0) {
				counts[kind]--;
				counts[12]++;
				break;
			}
		}
	}

	switch (*(s32*)setupBase) {
	case 2:
		while ((kind = BattleAudience_NoUsedHaitiRand()) != -1) {
			if (kind % 20 <= 9) {
				BattleAudience_Entry(kind, AUDIENCE_PUNI, 0);
			}
			else {
				BattleAudience_Entry(kind, AUDIENCE_XNAUT, 0);
			}
		}
		break;
	default: {
		kind = 0;
		listTotal = 0;
		for (; kind <= 12; kind++) {
			for (j = 0; j < counts[kind]; j++) {
				list[listTotal++] = kind;
			}
		}
		for (i = 0; i < 150; i++) {
			s32 a = irand(listTotal);
			s32 b = irand(listTotal);
			s32 t = list[a];
			list[a] = list[b];
			list[b] = t;
		}
		for (i = 0; i < listTotal; i++) {
			kind = BattleAudience_NoUsedFCHaitiRand();
			if (kind == -1) {
				break;
			}
			BattleAudience_Entry(kind, (u8)list[i], 0);
		}
		break;
	}
	}

	memcpy((u8*)audience + 0x13794, weights, 0x30);
}

// needs dont_inline: BattleAudience_Init must emit a call to this function, not inline it
#pragma dont_inline on
void BattleAudienceGuestTPLRead(s32 id, AudienceMemberType member, const char* path) {
	char full_path[256];

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (id < 2) {
		sprintf(full_path, str_802F9DD4, path);
		audience->guestTex[id] = tplRead(full_path);
		audience->guestType[id] = member;
	}
}
#pragma dont_inline reset

// 100%: writing the first loop's index as `j + i * 20 + 10` (instead of `i * 20 + 10 + j`) and
// declaring `s32 r;` immediately after `wp` (before every other local) put `wpHigh` (wp + 0x20000)
// in r27 and `r` in r28 as the original does. The last hold-out was the `wp->flags & 0x80` tail:
// MWCC merged the `audience + 0x137F0` base of the increment with the compare/switch above into one
// callee-saved register (r30, wp's dead register), while retail recomputes it in a caller-saved
// temp. Reading the increment through `&audience->window + 0x94` (the same address) breaks the
// merge; see the MUST_MATCH block below.
void BattleAudienceCtrlProcess(void) {
	BattleWork* wp = _battleWorkPointer;
	s32 r;
	u8* wpHigh = (u8*)wp + 0x20000;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member;
	s32 order[200];
	s32 i;
	s32 j;
	s32 index;
	s32 temp;
	s32 a;
	s32 b;

	audience->mCurrentAudienceIntCountLeft = 0;
	audience->mCurrentAudienceIntCountRight = 0;
	audience->mCurrentAudienceIntCount = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = j + i * 20 + 10;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				member = BattleAudienceGetPtr(index);
				if (member->status != 0xC && member->status != 0xB) {
					audience->mCurrentAudienceIntCountRight++;
				}
			}
		}
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = i * 20 + j;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				member = BattleAudienceGetPtr(index);
				if (member->status != 0xC && member->status != 0xB) {
					audience->mCurrentAudienceIntCountLeft++;
				}
			}
		}
	}

	i = 0;
	audience->mCurrentAudienceIntCount =
	    audience->mCurrentAudienceIntCountRight + audience->mCurrentAudienceIntCountLeft;

	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		if (member->flags & 0x10) {
			f32 zero = 0.0f;

			*(f32*)((u8*)member + 0xD0) += *(f32*)((u8*)member + 0xDC);
			*(f32*)((u8*)member + 0xDC) += *(f32*)((u8*)member + 0xE4);
			if (*(f32*)((u8*)member + 0xD0) <= zero) {
				*(f32*)((u8*)member + 0xD0) = zero;
				*(f32*)((u8*)member + 0xDC) = zero;
				*(f32*)((u8*)member + 0xE4) = zero;
			}
		}
		else if ((u8)BattleAudience_GetExist(i) != 0) {
			switch (member->type) {
			case AUDIENCE_TOAD:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_XNAUT:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_BOO:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_HAMMER_BRO:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_DULL_BONES:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_SHY_GUY:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_CRAZY_DAYZEE:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_PUNI:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_KOOPA:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_BULKY_BOBOMB:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_GOOMBA:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_PIRANHA_PLANT:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_LUIGI:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			}
		}
	}

	if (*(u32*)(wpHigh - 0x6f98) & 0x20000) {
		*(s32*)((u8*)audience + 0x137F4) = 0x2710;
	}

	if (wp->flags & 0x80) {
		if (*(s32*)((u8*)audience + 0x137F0) < *(s32*)((u8*)audience + 0x137F4)) {
			switch (*(s32*)((u8*)audience + 0x137F0)) {
			case 0:
				r = 0x708;
				break;
			case 1:
				r = 0x1770;
				break;
			case 2:
				r = 0x7530;
				break;
			}
			if (*(u32*)(wpHigh - 0x6f98) & 0x20000) {
				r = 0x3C;
			}
			if (irand(r) == 0) {
				for (i = 0; i < 200; i++) {
					order[i] = i;
				}
				for (i = 0; i < 1000; i++) {
					a = irand(200);
					b = irand(200);
					temp = order[a];
					order[a] = order[b];
					order[b] = temp;
				}
				for (i = 0; i < 200; i++) {
					if (BattleAudienceItemOn(order[i], 0, 1) == 1) {
#ifdef MUST_MATCH
						// Match-only: the window-relative spelling of this field's address keeps MWCC
						// from merging it with the compare/switch above into one callee-saved base
						// (retail recomputes it in a caller-saved temp). 0x137F0 - 0x1375C == 0x94.
						(*(s32*)((unsigned char*)&audience->window + 0x94))++;
#else
						(*(s32*)((u8*)audience + 0x137F0))++;
#endif
						break;
					}
				}
			}
		}
	}
}

// MWCC inlines the small BattleAudienceGetPtr/_GetExist helpers here and the extra
// address computations shift the whole body; the original keeps them as calls.
#pragma dont_inline on
BOOL BattleAudienceItemOn(s32 index, u16 itemId, s32 numItems) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	BattleAudienceItem* item;
	s32 i;
	if (audience->flags & 0x40000) {
		return FALSE;
	}
	if (audience_kind[member->type].itemTable->type == 0) {
		return FALSE;
	}
	if (itemId >= 0x79 && itemId < 0xec && swGet(0xeb) == 0) {
		return FALSE;
	}
	if (index % 20 < 4 || index % 20 > 0xf) {
		return FALSE;
	}
	if ((u8)BattleAudience_GetExist(index) == 0) {
		return FALSE;
	}
	if (member->status == 7) {
		return FALSE;
	}
	if (member->status == 8) {
		return FALSE;
	}
	if (member->status == 0xd) {
		return FALSE;
	}
	if (member->status == 0xc) {
		return FALSE;
	}
	if (member->status == 0xb) {
		return FALSE;
	}
	if (member->status == 0x12) {
		return FALSE;
	}
	if (member->status == 0x11) {
		return FALSE;
	}
	if (member->status == 0x15) {
		return FALSE;
	}
	if (member->status == 0xa) {
		return FALSE;
	}
	if (member->flags & 8) {
		i = 0;
		item = BattleAudienceItemGetPtr(0);
		for (; i < numItems; i++, item++) {
			if (!(item->flags & 1)) {
				item->flags |= 1;
				item->state = 0;
				((s32*)item->field_0x8)[1] = index;
				item->mItemType = itemId;
				BattleAudience_ChangeStatus(index, 7);
				member->itemId = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void BattleAudienceSetThrowItemMax(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 r = irand(100);
	if (r < 0x50) {
		*(s32*)((u8*)audience + 0x137F4) = 0;
	}
	else if (r < 0x5a) {
		*(s32*)((u8*)audience + 0x137F4) = 1;
	}
	else if (r < 0x5f) {
		*(s32*)((u8*)audience + 0x137F4) = 2;
	}
	else if (r < 0x64) {
		*(s32*)((u8*)audience + 0x137F4) = 3;
	}
	*(s32*)((u8*)audience + 0x137F0) = 0;
	*(s32*)((u8*)audience + 0x137F8) = 0;
}

BattleUnit* BattleAudienceDetectTargetAll(void) {
	s32 ids[64];
	s32 offset;
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit = NULL;
	s32 count;
	s32 sel;
	BattleAudienceBaseGetPtr();
	count = 0;
	offset = 0;
	i = 0;
	for (; i < 0x40; i++) {
		unit = wp->units[i];
		if (unit != NULL) {
			BattleUnitPart* part = BattleGetUnitPartsPtr(i, BtlUnit_GetBodyPartsId(unit));
			if (!(part->attributes & kNotTargetable) && unit->currentHp > 0 && (unit->currentKind < 0xD8 || unit->currentKind >= 0xDA)) {
				ids[count] = i;
				count++;
				offset += 4;
			}
		}
	}
	if (count == 0) {
		return NULL;
	}
	sel = irand(count);
	return BattleGetUnitPtr(wp, ids[sel]);
}

BattleUnit* BattleAudienceDetectTargetPlayer(void) {
	s32 ids[64];
	s32 offset;
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit = NULL;
	s32 count;
	s32 sel;
	BattleAudienceBaseGetPtr();
	count = 0;
	offset = 0;
	i = 0;
	for (; i < 0x40; i++) {
		unit = wp->units[i];
		if (unit != NULL) {
			BattleUnitPart* part = BattleGetUnitPartsPtr(i, BtlUnit_GetBodyPartsId(unit));
			if (unit->mAlliance == 0 && unit->currentHp > 0 && !(part->attributes & kNotTargetable)) {
				ids[count] = i;
				count++;
				offset += 4;
			}
		}
	}
	if (count == 0) {
		return NULL;
	}
	sel = irand(count);
	return BattleGetUnitPtr(wp, ids[sel]);
}

// The weight-count loop is written as `for (;;) { if (type == 0) break; ... }` so MWCC does not rotate it (a
// `while`/`for` rotated the condition to the bottom). What got it to 98.46%:
//  - every `float_*_8041e6xx` external was replaced by its literal (0.0f, 1.0f, 10.0f, …), which is what the
//    original source used: with literals MWCC colours the FP registers as the target does;
//  - `tbl` (lbl_80399940) is initialised first and `i` is declared right after it, so the base pointer is
//    hoisted into r29 and the saved-GPR assignment matches the original exactly;
//  - the outer `if (item+0x44)` was inverted so the else body is the fall-through branch;
//  - the `case 0` vecA block is written/ordered so MWCC's scheduler reproduces the original's float
//    load/store schedule (wrapped in MUST_MATCH; the #else order is the natural one);
//  - the case-0 vecA block copies the zero Vec from a compound-literal rvalue (the
//    `battleAudienceItemZeroVec` helper above), not from a named global: MWCC then emits the
//    template object at the helper's source position, which is the original `vec3_802f972c`
//    .rodata slot, and schedules the arithmetic `y, z, x` like the original instead of `z, x, y`.
//    The helper's position is load-bearing (the template's offset follows it), so it sits with
//    the data, not above this function; the `#else` keeps the natural named-object form.
//  - the case-3 inner `if (0x44 == 2)` jumps straight to the case-4 body like the original (MUST_MATCH
//    `goto`; without it MWCC falls through the `state != 4` check, 1 instruction);
//  - the deactivate path reuses `member` for the second `BattleAudienceGetPtr` call, which is what the
//    original did (it materialises -1 in r0, not r22).
// 100%: every call, immediate, value, branch and the case-0 scheduling match.
void BattleAudienceItemCtrlProcess(void) {
#ifdef MUST_MATCH
	u8* tbl = (u8*)lbl_80399940;
#endif
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceItem* item;
	BattleAudienceMember* member;
	BattleAudienceKind* kind;
	BattleUnit* unit;
	BattleUnitPart* part;
	BattleAudienceItemKind* kindItems;
	u8* aud;
	Vec hitPos;
	Vec vecA;
	Vec vecB;
	f32 dist;
	f32 dx;
	s8 turns;
	s8 strength;
	s32 j;
	s32 count;
	s32 offset;
	s32 total;
	s32 random;
	s32 acc;

	i = 0;
	aud = (u8*)audience;
	item = BattleAudienceItemGetPtr(0);
	for (; i < 100; i++, item++) {
		if (!(item->flags & 1)) {
			continue;
		}
		member = BattleAudienceGetPtr(*(s32*)((u8*)item + 0xc));
		kind = &audience_kind[(s32)member->type];
		if (member->status == 0xd || member->status == 0x13 || member->status == 0x15 || member->status == 0xc) {
			goto deactivate;
		}
		if ((u8)BattleAudience_GetExist(*(s32*)((u8*)item + 0xc)) == 0) {
			goto deactivate;
		}
		switch (item->state) {
		case 0:
			item->flags |= 2;
			if (item->mItemType == 0) {
				kindItems = kind->itemTable;
				count = 0;
				offset = 0;
				total = 0;
				for (;;) {
					if (kindItems[offset].type == 0) {
						break;
					}
					total += kindItems[offset].chance;
					count++;
					offset++;
				}
				if (count == 0) {
					goto remove;
				}
				random = irand(total);
				offset = 0;
				acc = 0;
				for (;;) {
					if (random >= acc && random < acc + kindItems[offset].chance) {
						break;
					}
					acc += kindItems[offset].chance;
					offset++;
				}
				item->mItemType = kindItems[offset].type;
			}
			*(s32*)((u8*)item + 0x8) = i + 0xb4;
#ifdef MUST_MATCH
			/* Statement order is a scheduling aid only: it makes MWCC emit the original's
			   float load/store schedule.  The #else block is behaviourally identical. */
			*(f32*)((u8*)item + 0x20) = 1.0f;
			*(f32*)((u8*)item + 0x24) = 0.0f;
			vecA = battleAudienceItemZeroVec();
			vecA.x = member->position.x + kind->field_0xC;
			vecA.y = member->position.y + kind->field_0x10 - 10.0f;
			vecA.z = member->position.z + 1.0f;
			*(Vec*)((u8*)item + 0x14) = vecA;
#else
			vecA = vec3_802f972c;
			vecA.x = member->position.x + kind->field_0xC;
			vecA.y = member->position.y + kind->field_0x10 - 10.0f;
			vecA.z = member->position.z + 1.0f;
			*(f32*)((u8*)item + 0x20) = 1.0f;
			*(f32*)((u8*)item + 0x24) = 0.0f;
			*(Vec*)((u8*)item + 0x14) = vecA;
#endif
			item->state++;
			/* fallthrough */
		case 1:
			if (*(s32*)((u8*)item + 0x8) <= 0) {
				for (j = 0; j < 100; j++) {
					if (j == i) {
						continue;
					}
					if (!(BattleAudienceItemGetPtr(j)->flags & 1)) {
						continue;
					}
					if (BattleAudienceItemGetPtr(j)->state != 1) {
						continue;
					}
					*(s32*)((u8*)item + 0x44) = 2;
					break;
				}
				if (j == 100) {
					*(s32*)((u8*)item + 0x44) = 1;
				}
				unit = BattleAudienceDetectTargetPlayer();
				if (unit == NULL) {
					goto remove;
				}
				*(BattleUnit**)((u8*)item + 0x40) = unit;
				part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
				BtlUnit_GetHitPos(unit, part, &hitPos.x, &hitPos.y, &hitPos.z);
				dx = hitPos.x - *(f32*)((u8*)item + 0x14);
				dist = 30.0f + (f32)__fabs(dx) / 15.0f;
				*(f32*)((u8*)item + 0x28) = dx / dist;
				*(f32*)((u8*)item + 0x34) = 0.0f;
				*(f32*)((u8*)item + 0x2c) = 7.0f;
				*(f32*)((u8*)item + 0x38) = 2.0f * ((hitPos.y - *(f32*)((u8*)item + 0x18)) - *(f32*)((u8*)item + 0x2c) * dist) / (dist * dist);
				*(f32*)((u8*)item + 0x30) = (hitPos.z - *(f32*)((u8*)item + 0x1c)) / dist;
				*(f32*)((u8*)item + 0x3c) = 0.0f;
				*(s32*)((u8*)item + 0x8) = (s32)dist;
				item->state++;
			}
			else {
				offset = 0xb4 - *(s32*)((u8*)item + 0x8);
				if (offset >= 5) {
					offset = 5;
				}
				vecB = vec3_802f9738;
				vecB.x = *(f32*)((u8*)member + 0xc0) + kind->field_0xC;
				vecB.y = *(f32*)((u8*)member + 0xc4) + kind->field_0x10 + intplGetValue(0, offset, 5, -10.0f, 0.0f);
				vecB.z = *(f32*)((u8*)member + 0xc8) + 1.0f;
				*(Vec*)((u8*)item + 0x14) = vecB;
			}
			break;
		case 2:
			if (!(wp->flags & 0x80)) {
				*(s32*)((u8*)item + 0x44) = 0;
			}
			if (*(s32*)((u8*)item + 0x8) == 0x14) {
				BattleAudience_ChangeStatus(*(s32*)((u8*)item + 0xc), 0);
			}
			if (*(s32*)((u8*)item + 0x8) == 0) {
				item->state++;
			}
			if (*(s32*)((u8*)item + 0x8) > 0) {
				*(f32*)((u8*)item + 0x14) += *(f32*)((u8*)item + 0x28);
				*(f32*)((u8*)item + 0x18) += *(f32*)((u8*)item + 0x2c);
				*(f32*)((u8*)item + 0x1c) += *(f32*)((u8*)item + 0x30);
				*(f32*)((u8*)item + 0x28) += *(f32*)((u8*)item + 0x34);
				*(f32*)((u8*)item + 0x2c) += *(f32*)((u8*)item + 0x38);
				*(f32*)((u8*)item + 0x30) += *(f32*)((u8*)item + 0x3c);
				if (item->mItemType == 0xef) {
					*(f32*)((u8*)item + 0x24) += 18.0f;
				}
				*(s32*)((u8*)item + 0x8) -= 1;
			}
			break;
		case 3:
			if (!(wp->flags & 0x80)) {
				*(s32*)((u8*)item + 0x44) = 0;
			}
			unit = *(BattleUnit**)((u8*)item + 0x40);
			BtlUnit_GetStatus(unit, STATUS_INVISIBLE, &turns, &strength);
			if (turns >= 1 || BtlUnit_CheckShadowGuard(unit)) {
				*(s32*)((u8*)item + 0x44) = 0;
			}
			if (item->flags & 4) {
				*(s32*)((u8*)item + 0x44) = 0;
			}
			if (item->mItemType >= 0x80 && item->mItemType < 0xec && pouchGetEmptyHaveItemCnt() <= 0) {
				*(s32*)((u8*)item + 0x44) = 0;
			}
			if (*(s32*)((u8*)item + 0x44) != 0) {
				*(s32*)(aud + 0x137f8) += 1;
				if (*(s32*)(aud + 0x137f8) > 3) {
					*(s32*)(aud + 0x137f8) = 3;
				}
				if (*(s32*)((u8*)item + 0x44) == 2) {
					item->state = 4;
#ifdef MUST_MATCH
					goto case4_body;
#endif
				}
				else {
					if (item->mItemType >= 0x79 && item->mItemType < 0xec) {
						BattleAudience_SetPresentItemType(0);
					}
					else if (item->mItemType >= 0xec && item->mItemType < 0xf0) {
						BattleAudience_SetPresentItemType(1);
					}
					BattleAudience_SetPresentTargetUnitId(BtlUnit_GetUnitId(unit));
					switch (item->mItemType) {
					case 0xec:
						#ifdef MUST_MATCH
						memcpy(aud + 0x137fc, tbl + 0x40, 0xc0);
#else
						memcpy(aud + 0x137fc, weapon_data_akikan, 0xc0);
#endif
						*(s32*)(aud + 0x1381c) = *(s32*)(aud + 0x1381c) + *(s32*)(aud + 0x137f8) - 1;
						break;
					case 0xed:
						#ifdef MUST_MATCH
						memcpy(aud + 0x137fc, tbl + 0x100, 0xc0);
#else
						memcpy(aud + 0x137fc, weapon_data_brick, 0xc0);
#endif
						*(s32*)(aud + 0x1381c) = *(s32*)(aud + 0x1381c) + *(s32*)(aud + 0x137f8) - 1;
						break;
					case 0xee:
						#ifdef MUST_MATCH
						memcpy(aud + 0x137fc, tbl + 0x1c0, 0xc0);
#else
						memcpy(aud + 0x137fc, weapon_data_bone, 0xc0);
#endif
						*(s32*)(aud + 0x1381c) = *(s32*)(aud + 0x1381c) + *(s32*)(aud + 0x137f8) - 1;
						break;
					case 0xef:
						#ifdef MUST_MATCH
						memcpy(aud + 0x137fc, tbl + 0x280, 0xc0);
#else
						memcpy(aud + 0x137fc, weapon_data_hammer, 0xc0);
#endif
						*(s32*)(aud + 0x1381c) = *(s32*)(aud + 0x1381c) + *(s32*)(aud + 0x137f8) - 1;
						break;
					}
					BattleAudience_SetPresentItemNo(item->mItemType);
					switch (BattleAudience_GetPresentItemType()) {
					case 0:
						item->flags &= ~2;
						*(s32*)((u8*)item + 0x8) = 5;
						item->state = 7;
						break;
					case 1:
						BattleAudienceSoundStop(10);
						BattleAudienceSoundStop(11);
						if (*(f32*)((u8*)item + 0x28) < 0.0f) {
							*(f32*)((u8*)item + 0x28) *= -0.2f;
						}
						*(f32*)((u8*)item + 0x2c) = 1.5f;
						*(f32*)((u8*)item + 0x30) = 0.0f;
						item->state = 4;
						break;
					}
					*(s32*)(aud + 0x137f8) = 0;
				}
			}
			else {
				item->state = 4;
			}
			if (item->state != 4) {
				break;
			}
			/* fall through */
#ifdef MUST_MATCH
		case4_body:
#endif
		case 4:
			*(f32*)((u8*)item + 0x14) += *(f32*)((u8*)item + 0x28);
			*(f32*)((u8*)item + 0x18) += *(f32*)((u8*)item + 0x2c);
			*(f32*)((u8*)item + 0x1c) += *(f32*)((u8*)item + 0x30);
			*(f32*)((u8*)item + 0x28) += *(f32*)((u8*)item + 0x34);
			*(f32*)((u8*)item + 0x2c) += *(f32*)((u8*)item + 0x38);
			*(f32*)((u8*)item + 0x30) += *(f32*)((u8*)item + 0x3c);
			if (*(f32*)((u8*)item + 0x18) <= 0.0f) {
				BattleAudienceSoundStop(10);
				BattleAudienceSoundStop(11);
				item->state = 5;
				*(f32*)((u8*)item + 0x18) = 0.1f;
				*(f32*)((u8*)item + 0x2c) = 1.0f;
				*(f32*)((u8*)item + 0x30) = 0.0f;
			}
			break;
		case 5:
			*(f32*)((u8*)item + 0x14) += *(f32*)((u8*)item + 0x28);
			*(f32*)((u8*)item + 0x18) += *(f32*)((u8*)item + 0x2c);
			*(f32*)((u8*)item + 0x1c) += *(f32*)((u8*)item + 0x30);
			*(f32*)((u8*)item + 0x28) += *(f32*)((u8*)item + 0x34);
			*(f32*)((u8*)item + 0x2c) += *(f32*)((u8*)item + 0x38);
			*(f32*)((u8*)item + 0x30) += *(f32*)((u8*)item + 0x3c);
			if (*(f32*)((u8*)item + 0x18) <= 0.0f) {
				*(s32*)((u8*)item + 0x8) = 0x3c;
				item->state = 6;
				*(f32*)((u8*)item + 0x18) = 0.0f;
			}
			break;
		case 6:
			if (*(s32*)((u8*)item + 0x8) <= 0) {
				goto remove;
			}
			if (*(s32*)((u8*)item + 0x8) < 0x1e) {
				if ((*(s32*)((u8*)item + 0x8) & 3) <= 1) {
					item->flags |= 2;
				}
				else {
					item->flags &= ~2;
				}
			}
			*(s32*)((u8*)item + 0x8) -= 1;
			break;
		case 7:
			*(s32*)((u8*)item + 0x8) -= 1;
			if (*(s32*)((u8*)item + 0x8) > 0) {
				break;
			}
			/* fall through */
		remove:
			BattleAudienceSoundStop(10);
			BattleAudienceSoundStop(11);
			if (*(s32*)((u8*)item + 0xc) != -1) {
				BattleAudienceGetPtr(*(s32*)((u8*)item + 0xc));
				BattleAudience_ChangeStatus(*(s32*)((u8*)item + 0xc), 0);
			}
		deactivate:
			item->flags = 0;
			*(s32*)((u8*)item + 0x8) = 0;
			item->state = 0;
			if (*(s32*)((u8*)item + 0xc) != -1) {
				member = BattleAudienceGetPtr(*(s32*)((u8*)item + 0xc));
				member->itemId = -1;
			}
			break;
		}
	}
}

// Functionally complete. `opt_common_subs off` is load-bearing: with CSE on the active count lands
// in r29 (retail r28) and the whole first block shifts (98.62 -> 99.34). Storing rand()'s result
// in a local `rv` before the case-0 conversion then pulled the int->float magic double into retail's
// f2 (99.34 -> 99.40); it is the only handle, a direct `(f32)rand()` picks f3. Assigning float_50 to
// `start` before multiplying (`start = float_50; start = start * (f32)rv;` instead of one
// expression) made the multiply overwrite float_50's register, retail's `fmuls f3,f3,f4`
// (99.40 -> 99.44). What is left is FP-register tie-breaks only: the case-0 conversion result lands
// f2 (retail f4) so float_25 lands f4 (retail f2); case 10 loads star+0x2c before star+0x20 (retail
// the reverse, so c sits f0 where retail has f2 and vice versa); and the vec3_802f9744 copy loads a
// pointer (retail lis+addi); every reshaping/pragma tried is equal or worse.
#pragma opt_common_subs off // needed: with CSE on the active count lands in r29 and the int->float mask in f3
// NONMATCHING (99.44%): FP-register tie-breaks only (see above).
void BattleAudienceApSrcCtrlProcess(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	u8* star;
	Vec pos;
	Vec pos2;
	s32 active;
	s32 inactive;
	s32 i;
	f32 radius;
	f32 dx;
	f32 start;
	f32 end;
	f32 zero;
	f32 k25;
	s32 rv;

	if (!(audience->flags & 2)) {
		return;
	}
	active = 0;
	for (i = 0; i < 100; i++) {
		if (*(s32*)((u8*)audience + 0x10E7C + i * 0x60) & 1) {
			active++;
		}
	}
	if (psndSFXChk(*(s32*)((u8*)audience + 8)) != -1) {
		psndSFX_vol(*(s32*)((u8*)audience + 8),
		            (s32)(float_64_8041e6d4 + float_63_8041e644 * (f32)active / float_100_8041e64c));
	}
	if (*(s32*)((u8*)audience + 0x137C4) > 0) {
		if (BattleAudienceApSrcEntry() == 1) {
			*(s32*)((u8*)audience + 0x137C4) = *(s32*)((u8*)audience + 0x137C4) - 1;
		}
		if (active == 0) {
			if (psndSFXChk(*(s32*)((u8*)audience + 8)) == -1) {
				*(s32*)((u8*)audience + 8) = psndSFXOn(str_802F9DBC);
			}
		}
	}
	else {
		inactive = 0;
		for (i = 0; i < 100; i++) {
			if (*(s32*)((u8*)audience + 0x10E7C + i * 0x60) & 1) {
				break;
			}
			inactive++;
		}
		if (inactive == 100) {
			audience->flags &= ~2;
			psndSFXOff(*(s32*)((u8*)audience + 8));
			*(s32*)((u8*)audience + 8) = 0;
			*(s32*)((u8*)audience + 0x137C4) = 0;
			BattleStatusWindowAPRecoveryOff();
			return;
		}
	}
	for (i = 0; i < 100; i++) {
		star = (u8*)audience + 0x10E7C + i * 0x60;
		pouchGetPtr();
		if (!(*(s32*)star & 1)) {
			continue;
		}
		switch (*(s32*)(star + 4)) {
		case 0:
			*(s32*)(star + 4) = 5;
			*(Vec*)(star + 0x2C) = *(Vec*)(star + 8);
			*(Vec*)(star + 0x20) = *(Vec*)(star + 0x2C);
			rv = rand();
			start = float_50_8041e6d8;
			start = start * (f32)rv;
			k25 = float_25_8041e6dc;
			start = start / float_32767_8041e674 - k25;
			*(f32*)(star + 0x2C) += start;
			*(f32*)(star + 0x30) += k25;
			*(s32*)(star + 0x44) = 0;
			*(s32*)(star + 0x48) = 0x1E;
			// fall through
		case 5:
			*(s32*)(star + 0x44) = *(s32*)(star + 0x44) + 1;
			start = *(f32*)(star + 0x20);
			end = *(f32*)(star + 0x2C);
			*(f32*)(star + 8) = intplGetValue(5, *(s32*)(star + 0x44), *(s32*)(star + 0x48), start, end);
			start = *(f32*)(star + 0x24);
			end = *(f32*)(star + 0x30);
			*(f32*)(star + 0xC) = intplGetValue(5, *(s32*)(star + 0x44), *(s32*)(star + 0x48), start, end);
			if (*(s32*)(star + 0x44) >= *(s32*)(star + 0x48) - 0x14) {
				*(f32*)(star + 0x3C) = intplGetValue(4, *(s32*)(star + 0x44) - (*(s32*)(star + 0x48) - 0x14), 0xA,
				                                     float_0p9_8041e6e0, float_0p8_8041e69c);
			}
			if (*(s32*)(star + 0x44) < *(s32*)(star + 0x48)) {
				continue;
			}
			*(s32*)(star + 4) = 0xA;
			*(s32*)(star + 0x44) = 0;
			*(s32*)(star + 0x48) = irand(0xA) + 0xF;
			*(Vec*)(star + 0x38) = *p_vec3_802f9744;
			break;
		case 10:
			*(s32*)(star + 0x44) = *(s32*)(star + 0x44) + 1;
			if (*(s32*)(star + 0x44) < *(s32*)(star + 0x48)) {
				continue;
			}
			*(s32*)(star + 4) = 0xF;
			*(s32*)(star + 0x44) = 0;
			*(s32*)(star + 0x48) = irand(0x14) + 0x14;
			*(Vec*)(star + 0x20) = *(Vec*)(star + 8);
			pos = statusGetApPos();
			*(Vec*)(star + 0x2C) = pos;
			radius = *(f32*)(star + 0x24) - *(f32*)(star + 0x30);
			dx = *(f32*)(star + 0x2C);
			dx = *(f32*)(star + 0x20) - dx;
			*(f32*)(star + 0x4C) = (*(f32*)(star + 0x50) = sqrt(dx * dx + radius * radius));
			*(f32*)(star + 0x54) = asin(radius / *(f32*)(star + 0x4C));
			*(f32*)(star + 0x58) = float_0p05_8041e6e4 +
			                        float_0p2_8041e67c * (f32)rand() / float_32767_8041e674;
			break;
		case 15:
			end = float_0_8041e618;
			*(s32*)(star + 0x44) = *(s32*)(star + 0x44) + 1;
			start = *(f32*)(star + 0x50);
			*(f32*)(star + 0x4C) = intplGetValue(4, *(s32*)(star + 0x44), *(s32*)(star + 0x48), start, end);
			zero = zero_8041e618;
			start = *(f32*)(star + 0x54) + *(f32*)(star + 0x58);
			*(f32*)(star + 0x54) = start;
			if (*(f32*)(star + 0x54) < zero) {
				*(f32*)(star + 0x54) += float_6p2832_8041e6e8;
			}
			*(f32*)(star + 8) = *(f32*)(star + 0x4C) * (f32)cos(*(f32*)(star + 0x54)) + *(f32*)(star + 0x2C);
			*(f32*)(star + 0xC) = *(f32*)(star + 0x4C) * (f32)sin(*(f32*)(star + 0x54)) + *(f32*)(star + 0x30);
			if (*(s32*)(star + 0x44) >= *(s32*)(star + 0x48)) {
				*(s32*)(star + 4) = 0x14;
				*(s32*)(star + 0x44) = 0;
				*(s32*)(star + 0x48) = 0xF;
				pos2 = statusGetApPos();
				*(Vec*)(star + 8) = pos2;
			}
			break;
		case 20:
			start = float_128_8041e6ec;
			end = zero_8041e618;
			*(s32*)(star + 0x44) = *(s32*)(star + 0x44) + 1;
			*(u8*)(star + 0x5C) = (u8)intplGetValue(4, *(s32*)(star + 0x44), *(s32*)(star + 0x48), start, end);
			if (*(s32*)(star + 0x44) >= *(s32*)(star + 0x48)) {
				*(s32*)star = 0;
				pouchAddAP(1);
			}
			break;
		default:
			break;
		}
	}
}
#pragma opt_common_subs reset


// hoisting `z0 = float_0` before the multiplies (88.93 -> 91.21), `k1 = float_1` before the position
// stores and a separate `k20 = float_20_8041e6a8` for the y add emit the original's early loads;
// making the `star+0x44` store `volatile` serialises it before the constant-Vec copy (91.72 -> 92.21).
// NONMATCHING (92.21%): what remains is the anonymous-i2f double register, the hoisted `star+0x2c`
// copy loads and FP scheduling.
BOOL BattleAudienceApSrcEntry(void) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	CameraEntry* cam;
	u8* star;
	f32 sx;
	f32 sy;
	f32 z0;
	f32 k1;
	f32 k20;
	s32 ids[200];
	s32 idx;
	s32 count;
	s32 offset;
	s32 i;

	audience = BattleAudienceBaseGetPtr();
	i = 0;
	star = (u8*)audience->starpower;
	for (; i < 100; i++, star += 0x60) {
		if (!(*(s32*)star & 1)) {
			break;
		}
	}
	if (i == 100) {
		return FALSE;
	}
	idx = 0;
	count = 0;
	offset = 0;
	member = BattleAudienceGetPtr(0);
	for (; idx < 200; idx++) {
		if ((u8)BattleAudience_GetExist(idx) == 1) {
			ids[count] = idx;
			count++;
			offset += 4;
		}
	}
	if (count == 0) {
		return TRUE;
	}
	*(s32*)star = 1;
	*(s32*)(star + 4) = 0;
#ifdef MUST_MATCH
	// Match-only: the original stores star+0x44 before the constant Vec copy and serialises that copy's
	// loads/stores (lwz/lwz/stw/stw/lwz/stw); the named-global form lets MWCC software-pipeline it.
	*(volatile s32*)(star + 0x44) = 0;
	*(volatile Vec*)(star + 0x38) = *(const volatile Vec*)&vec3_802f9750;
#else
	*(Vec*)(star + 0x38) = vec3_802f9750;
	*(s32*)(star + 0x44) = 0;
#endif
	*(u8*)(star + 0x5c) = 0x80;
	member = BattleAudienceGetPtr(ids[irand(count)]);
	k1 = float_1_8041e620;
	k20 = float_20_8041e6a8;
	*(f32*)(star + 8) = member->position.x;
	*(f32*)(star + 0xc) = k20 + member->position.y;
	*(f32*)(star + 0x10) = k1 + member->position.z;
	cam = camGetPtr(CAMERA_3D);
	PSMTXMultVec(cam->view, (Vec*)(star + 8), (Vec*)(star + 8));
	PSMTX44MultVec(cam->projection, (Vec*)(star + 8), (Vec*)(star + 8));
	z0 = float_0_8041e618;
	sx = float_304_8041e6cc;
	sy = float_240_8041e6d0;
	*(f32*)(star + 8) *= sx;
	*(f32*)(star + 0xc) *= sy;
	*(f32*)(star + 0x10) = z0;
	*(f32*)(star + 0x14) = float_40_8041e690 * (f32)rand() / float_32767_8041e674 - float_20_8041e6a8;
	*(f32*)(star + 0x18) = float_10_8041e694 * (f32)rand() / float_32767_8041e674;
	*(f32*)(star + 0x1c) = float_0_8041e618;
	*(Vec*)(star + 0x20) = *(Vec*)(star + 8);
#ifdef MUST_MATCH
	*(volatile Vec*)(star + 0x2c) = *(const volatile Vec*)&vec3_802f975c;
#else
	*(Vec*)(star + 0x2c) = vec3_802f975c;
#endif
	return TRUE;
}

// The target colours the first cast's 0.0f constant f0 (window->count f1, cast temp f2, diff f3); MWCC colours
// ours f3 instead, shifting the first block and both step clamps up a register, which also turns the positive
// clamp's bge+b diamond into a plain bge. Loading the subtrahend first (`diff = window->count; diff =
// (f32)audience->mCurrentAudienceIntCount - diff;`) gets one more instruction to match than the plain one-liner.
// Writing the zero test constant-first (`float_0 != diff`) makes objdiff's first `fcmpu` operand order match
// (the constant is FRA) and is worth 95.87 -> 95.91.
// NONMATCHING (95.91%): the positive clamp is a ternary (`step = step < float_0p1 ? float_0p1 : step;`)
// and the two final stores go through locals; the rest is FP register colouring (the first cast's
// zero lands in f3 instead of f0, rotating both clamps).
void BattleAudienceWinCtrlProcess(void) {
	BattleAudience* audience;
	BattleAudienceWindow* window;
	u8* win;
	f32 diff;

	audience = BattleAudienceBaseGetPtr();
	window = BattleAudienceWinGetPtr();
	diff = window->count;
	diff = (f32)audience->mCurrentAudienceIntCount - diff;
	if (float_0_8041e618 != diff) {
		if (diff > float_0_8041e618) {
			f32 step = float_0p1_8041e678 * diff;
			step = step < float_0p1_8041e678 ? float_0p1_8041e678 : step;
			window->count += step;
			if ((f32)audience->mCurrentAudienceIntCount - window->count < float_0_8041e618) {
				window->count = (f32)audience->mCurrentAudienceIntCount;
			}
		}
		else {
			f32 step = float_0p1_8041e678 * diff;
			if (step > float_neg0p1_8041e6bc) {
				step = float_0p1_8041e678;
			}
			else {
				step = float_neg0p1_8041e6bc * diff;
			}
			window->count -= step;
			if ((f32)audience->mCurrentAudienceIntCount - window->count > float_0_8041e618) {
				window->count = (f32)audience->mCurrentAudienceIntCount;
			}
		}
	}

	win = (u8*)window;
	if (window->mActive) {
		*(s32*)(win + 0x14) += 1;
		if (*(s32*)(win + 0x14) > 0xF) {
			*(s32*)(win + 0x14) = 0xF;
		}
	}
	else {
		*(s32*)(win + 0x14) -= 1;
		if (*(s32*)(win + 0x14) < 0) {
			*(s32*)(win + 0x14) = 0;
		}
	}

	*(f32*)(win + 0x8) =
	    intplGetValue(4, *(s32*)(win + 0x14), 0xF, float_neg500_8041e6c0, float_neg280_8041e6c4);
	{
		f32 a = float_neg160_8041e6c8;
		f32 b = float_0_8041e618;
		*(f32*)(win + 0xC) = a;
		*(f32*)(win + 0x10) = b;
	}
}

void BattleAudienceGXInit(void) {
	BattleAudienceBaseGetPtr();
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	GXSetTevOrder(0, 0, 0, 4);
}

// Runs each active member's animation command script. Opcodes are negative words; the script
// base is 0x24, the current command 0x28, the wait timer 0x2C, and 0x34/0x40 are a 3-deep
// loop-counter stack and its depth (none of these have named struct fields yet).
void BattleAudienceAnimProcess(void) {
	BattleAudienceMember* member;
	s32 cmd;
	s32 target;
	s32 depth;
	s32 i;
	s32* anim;

	BattleAudienceBaseGetPtr();
	for (i = 0, member = BattleAudienceGetPtr(0); i < 200; i++, member++) {
		if (!(member->flags & 1)) {
			continue;
		}
		if (!(member->flags & 4)) {
			continue;
		}
		anim = *(s32**)((u8*)member + 0x28);
		if (*(s32*)((u8*)member + 0x2C) > 0) {
			*(s32*)((u8*)member + 0x2C) -= 1;
			continue;
		}
		switch (*anim) {
		case -2:
			member->field_0x1A = anim[1];
			*(s32*)((u8*)member + 0x2C) = anim[2];
			*(s32*)((u8*)member + 0x28) += 0xC;
			break;
		case -3:
			*(s32*)((u8*)member + 0x28) = *(s32*)((u8*)member + 0x24);
			break;
		case -4:
			*(s32*)((u8*)member + 0x28) += 8;
			break;
		case -5:
			target = anim[1];
			*(s32*)((u8*)member + 0x28) = *(s32*)((u8*)member + 0x24);
			while (!((*(s32**)((u8*)member + 0x28))[0] == -4 && (*(s32**)((u8*)member + 0x28))[1] == target)) {
				*(s32*)((u8*)member + 0x28) += 4;
			}
			break;
		case -6:
			*(s32*)((u8*)member + 0x34 + *(u8*)((u8*)member + 0x40) * 4) = anim[1];
			*(u8*)((u8*)member + 0x40) += 1;
			*(s32*)((u8*)member + 0x28) += 8;
			break;
		case -7:
			*(s32*)((u8*)member + 0x30 + *(u8*)((u8*)member + 0x40) * 4) -= 1;
			if (*(s32*)((u8*)member + 0x30 + *(u8*)((u8*)member + 0x40) * 4) <= 0) {
				*(u8*)((u8*)member + 0x40) -= 1;
				*(s32*)((u8*)member + 0x28) += 4;
			} else {
				depth = 1;
				do {
					*(s32*)((u8*)member + 0x28) -= 4;
					cmd = **(s32**)((u8*)member + 0x28);
					if (cmd == -6) {
						depth--;
					}
					if (cmd == -7) {
						depth++;
					}
				} while (depth > 0);
				*(s32*)((u8*)member + 0x28) += 8;
			}
			break;
		case -8:
			*(s32*)((u8*)member + 0x2C) = anim[1];
			*(s32*)((u8*)member + 0x28) += 8;
			break;
		case -1:
			break;
		}
	}
}

extern f32 float_0p015625_8041e6ac;
extern f32 float_deg2rad_8041e6b0;
extern f32 float_360_8041e6b4;
extern f32 float_180_8041e6b8;
extern u32 unk_80423080;
typedef struct BattleAudienceTexCoords {
	f32 v[32];
} BattleAudienceTexCoords;

#ifdef MUST_MATCH
// Same idea as BattleAudienceDispApSrc_color below: routing the colour through an inline puts the
// named local at r1+0x8 and the hidden by-value copy at r1+0xc, matching the original. The #else
// plain assignment puts them the other way round.
static inline GXColor BattleAudienceDispAudience_color(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = *(GXColor*)&unk_80423080;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}
#endif

// NONMATCHING (95.98%): functionally complete. The angle wrap now fuses and the texture scale uses
// explicit k/s locals in the MUST_MATCH arm; what still differs is FP scheduling and register
// allocation in the matrix build (including the trig call order) and the GX texcoord output.
void BattleAudienceDispAudience(CameraId camId, void* param) {
	BattleAudience* audience;
	CameraEntry* cam;
	s32 i;
	BattleAudienceMember* member;
	s32 flip;
	s32 texBase;
	s32 front;
	s32 j;
	GXTexObj texObj;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxDraw;
	BattleAudienceTexCoords texCoords;
#ifdef MUST_MATCH
	// Match-only: this declaration order lands the trig results in the original's FPRs (scaleY is
	// parked after rz, cy/cz the other way round). Same code, only the allocator's tie-break moves.
	f32 scaleX;
	f32 cx;
	f32 scaleZ;
	f32 sy;
	f32 cy;
	f32 cz;
	f32 sz;
	f32 neg20;
	f32 rx;
	f32 rz;
	f32 scaleY;
	f32 offsetX;
	f32 offsetY;
	f32 angle;
	f32 ry;
	f32 offsetZ;
	f32 sx;
#else
	f32 scaleX;
	f32 cx;
	f32 scaleZ;
	f32 sy;
	f32 scaleY;
	f32 cy;
	f32 sz;
	f32 neg20;
	f32 rx;
	f32 rz;
	f32 cz;
	f32 offsetX;
	f32 offsetY;
	f32 angle;
	f32 ry;
	f32 offsetZ;
	f32 sx;
#endif
	u8 cr;
	u8 cg;
	u8 cb;
	u8 ca;
	GXColor color;

	audience = BattleAudienceBaseGetPtr();
	cam = camGetPtr(CAMERA_3D);

	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_RASC);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	i = 0;
	member = BattleAudienceGetPtr(0);
	neg20 = -float_20_8041e6a8;

	for (; i < 0xC8; i++, member++) {
		if (dispGetCurWork()->renderMode != 2) {
			if (*(u8*)((u8*)member + 0x44) != 0xFF) {
				continue;
			}
		}
		else if (*(u8*)((u8*)member + 0x44) == 0xFF) {
			continue;
		}
		if (!(member->flags & 1)) {
			continue;
		}
		if (!(member->flags & 4)) {
			continue;
		}

		if (member->type <= AUDIENCE_PIRANHA_PLANT) {
			TEXGetGXTexObjFromPalette((TEXPalette*)audience->normalTex->data[0], &texObj, member->field_0x1A);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			{
				f32 w = (f32)GXGetTexObjWidth(&texObj);
#ifdef MUST_MATCH
				f32 k = audience_kind[member->type].field_0x18;
				f32 s = k * w;
				s = float_0p015625_8041e6ac * s;
				*(f32*)member->field_0x54 = *(f32*)(member->field_0x54 + 0xC) * s;
#else
				*(f32*)((u8*)member + 0x54) = *(f32*)((u8*)member + 0x60) *
					(audience_kind[member->type].field_0x18 * w * float_0p015625_8041e6ac);
#endif
			}
			{
				f32 h = (f32)GXGetTexObjHeight(&texObj);
#ifdef MUST_MATCH
				f32 k = audience_kind[member->type].field_0x1C;
				f32 s = k * h;
				s = float_0p015625_8041e6ac * s;
				*(f32*)(member->field_0x54 + 4) = *(f32*)(member->field_0x54 + 0x10) * s;
#else
				*(f32*)((u8*)member + 0x58) = *(f32*)((u8*)member + 0x64) *
					(audience_kind[member->type].field_0x1C * h * float_0p015625_8041e6ac);
#endif
			}
		}
		else {
			for (j = 0; j < 2; j++) {
				if (member->type == audience->guestType[j]) {
					TEXGetGXTexObjFromPalette((TEXPalette*)audience->guestTex[j]->data[0], &texObj,
					                          member->field_0x1A);
					GXLoadTexObj(&texObj, GX_TEXMAP0);
					{
						f32 w = (f32)GXGetTexObjWidth(&texObj);
#ifdef MUST_MATCH
						f32 k = audience_kind[member->type].field_0x18;
						f32 s = k * w;
						s = float_0p015625_8041e6ac * s;
						*(f32*)member->field_0x54 = *(f32*)(member->field_0x54 + 0xC) * s;
#else
						*(f32*)((u8*)member + 0x54) = *(f32*)((u8*)member + 0x60) *
							(audience_kind[member->type].field_0x18 * w * float_0p015625_8041e6ac);
#endif
					}
					{
						f32 h = (f32)GXGetTexObjHeight(&texObj);
#ifdef MUST_MATCH
						f32 k = audience_kind[member->type].field_0x1C;
						f32 s = k * h;
						s = float_0p015625_8041e6ac * s;
						*(f32*)(member->field_0x54 + 4) = *(f32*)(member->field_0x54 + 0x10) * s;
#else
						*(f32*)((u8*)member + 0x58) = *(f32*)((u8*)member + 0x64) *
							(audience_kind[member->type].field_0x1C * h * float_0p015625_8041e6ac);
#endif
					}
					break;
				}
			}
		}

		scaleX = *(f32*)((u8*)member + 0x54);
		offsetX = *(f32*)((u8*)member + 0xA8) * scaleX;
		scaleY = *(f32*)((u8*)member + 0x58);
		offsetY = *(f32*)((u8*)member + 0xAC) * scaleY;
		scaleZ = *(f32*)((u8*)member + 0x5C);
		offsetZ = *(f32*)((u8*)member + 0xB0) * scaleZ;
		PSMTXScale(mtxScale, scaleX, scaleY, scaleZ);

		rx = member->mRotation.x * float_deg2rad_8041e6b0;
		ry = member->mRotation.y * float_deg2rad_8041e6b0;
		rz = member->mRotation.z * float_deg2rad_8041e6b0;
		sx = (f32)sin(rx);
		cx = (f32)cos(rx);
		sy = (f32)sin(ry);
		cy = (f32)cos(ry);
		cz = (f32)cos(rz);
		sz = (f32)sin(rz);

		mtxRot[2][0] = sy * cz + sx * cy * sz;
		mtxRot[0][3] = 0.0f;
		mtxRot[0][2] = -cx * sy;
		mtxRot[0][0] = cy * cz - sx * sy * sz;
		mtxRot[1][0] = cx * sz;
		mtxRot[0][1] = -cy * sz - sx * sy * cz;
		mtxRot[1][2] = -sx;
		mtxRot[1][3] = 0.0f;
		mtxRot[2][1] = -sy * sz + sx * cy * cz;
		mtxRot[1][1] = cx * cz;
		mtxRot[2][2] = cx * cy;
		mtxRot[2][3] = 0.0f;

		*(Vec*)((u8*)member + 0x90) = vec3_802f9768;
		PSMTXMultVec(mtxRot, (Vec*)((u8*)member + 0x90), (Vec*)((u8*)member + 0x90));

		mtxScale[0][3] += offsetX;
		mtxScale[1][3] += offsetY;
		mtxScale[2][3] += offsetZ;
		PSMTXConcat(mtxRot, mtxScale, mtxScale);

		mtxScale[0][3] = mtxScale[0][3] + (*(f32*)((u8*)member + 0xCC) + (-offsetX + member->position.x));
		mtxScale[2][3] = mtxScale[2][3] + (*(f32*)((u8*)member + 0xD4) + (-offsetZ + member->position.z));
		if (member->type != AUDIENCE_BOO) {
			mtxScale[1][3] = mtxScale[1][3] + (*(f32*)((u8*)member + 0xD0) + (-offsetY + member->position.y));
		}
		else {
			mtxScale[1][3] = mtxScale[1][3] + (*(f32*)((u8*)member + 0x130) +
			                                      (*(f32*)((u8*)member + 0xD0) + (-offsetY + member->position.y)));
		}

		PSMTXConcat(cam->view, mtxScale, mtxDraw);
		GXLoadPosMtxImm(mtxDraw, 0);

		texCoords = *(BattleAudienceTexCoords*)dat_802f9774;

		texBase = 0;
		front = BattleAudience_GetFront(i);
		cr = *(u8*)((u8*)member + 0x41);
		cg = *(u8*)((u8*)member + 0x42);
		cb = *(u8*)((u8*)member + 0x43);
		ca = *(u8*)((u8*)member + 0x44);
		if (*(s32**)((u8*)member + 0x24) == member->animTable[9]) {
			texBase = 8;
		}

		if (member->field_0xE8 >= f360_8041e6b4) {
			member->field_0xE8 = member->field_0xE8 -
			                     f360_8041e6b4 * (f32)(s32)(member->field_0xE8 / f360_8041e6b4);
		}
		if (member->field_0xE8 < 0.0f) {
			angle = (f32)(1 - (s32)(member->field_0xE8 / f360_8041e6b4));
			member->field_0xE8 = member->field_0xE8 + f360_8041e6b4 * angle;
		}
		angle = member->field_0xE8;
		if (angle > 0.0f && angle <= float_180_8041e6b8) {
			flip = 1;
		}
		if (angle == 0.0f || (angle > float_180_8041e6b8 && angle < f360_8041e6b4)) {
			flip = 0;
		}
		if ((u8)front == 1) {
			if (*(s32**)((u8*)member + 0x24) != member->animTable[9] &&
			    *(s32**)((u8*)member + 0x24) != member->animTable[8] &&
			    *(s32**)((u8*)member + 0x24) != member->animTable[7]) {
				flip ^= 1;
			}
		}
		if (flip == 1) {
			texBase += 4;
		}

#ifdef MUST_MATCH
		color = BattleAudienceDispAudience_color(cr, cg, cb, ca);
#else
		color = *(GXColor*)&unk_80423080;
		color.r = cr;
		color.g = cg;
		color.b = cb;
		color.a = ca;
#endif
		GXSetChanMatColor(GX_COLOR0A0, color);

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(neg20 + *(f32*)(member->field_0x54 + 0x18), float_40_8041e690 + *(f32*)(member->field_0x54 + 0x1C),
		               *(f32*)(member->field_0x54 + 0x20));
		GXTexCoord2f32(texCoords.v[texBase * 2], texCoords.v[texBase * 2 + 1]);
		GXPosition3f32(float_20_8041e6a8 + *(f32*)(member->field_0x54 + 0x18), float_40_8041e690 + *(f32*)(member->field_0x54 + 0x1C),
		               *(f32*)(member->field_0x54 + 0x20));
		GXTexCoord2f32(texCoords.v[(texBase + 1) * 2], texCoords.v[(texBase + 1) * 2 + 1]);
		GXPosition3f32(float_20_8041e6a8, 0.0f, 0.0f);
		GXTexCoord2f32(texCoords.v[(texBase + 2) * 2], texCoords.v[(texBase + 2) * 2 + 1]);
		GXPosition3f32(neg20, 0.0f, 0.0f);
		GXTexCoord2f32(texCoords.v[(texBase + 3) * 2], texCoords.v[(texBase + 3) * 2 + 1]);
	}
}

void BattleAudienceDispItem(CameraId camId, void* param) {
	BattleAudienceItem* item;
	Mtx mtx;
	Mtx rotMtx;
	s32 i;
	u16 iconId;

	BattleAudienceGXInit();
	i = 0;
	item = BattleAudienceItemGetPtr(0);
	for (; i < 100; i++, item++) {
		if (!(item->flags & 1)) {
			continue;
		}
		if (!(item->flags & 2)) {
			continue;
		}
		iconId = itemDataTable[item->mItemType].icon_id;
		PSMTXTrans(mtx, 0.0f, -20.0f, 0.0f);
		PSMTXRotRad(rotMtx, 'z', float_deg2rad_8041e6a4 * *(f32*)((u8*)item + 0x24));
		PSMTXConcat(rotMtx, mtx, mtx);
		PSMTXTransApply(mtx, mtx, 0.0f, 20.0f, 0.0f);
		PSMTXScaleApply(mtx, mtx, *(f32*)((u8*)item + 0x20), *(f32*)((u8*)item + 0x20), 1.0f);
		PSMTXTransApply(mtx, mtx, *(f32*)((u8*)item + 0x14), *(f32*)((u8*)item + 0x18),
		                *(f32*)((u8*)item + 0x1c));
#ifdef MUST_MATCH
		// iconDispGx2's header says s16 but the original reads a zero-extended u16 (no extsh).
		((void (*)(Mtx, s16, u16))iconDispGx2)(mtx, 0, iconId);
#else
		iconDispGx2(mtx, 0, iconId);
#endif
	}
}

// The inline's returned colour lands where the original keeps it: the named local at r1+0x8,
// the hidden by-value copy at r1+0xc. The plain assignment (the #else arm) puts them the other way round.
#ifdef MUST_MATCH
static inline GXColor BattleAudienceDispApSrc_color(u8 alpha) {
	GXColor color = *(GXColor*)&dat_8041e608;
	color.a = alpha;
	return color;
}
#endif

void BattleAudienceDispApSrc(CameraId camId, void* param) {
	BattleAudience* audience;
	u8* star;
	Mtx rot;
	Mtx scale;
	Mtx trans;
	Mtx mtx;
	GXColor color;
	s32 i;

	audience = BattleAudienceBaseGetPtr();
	camGetPtr(CAMERA_2D);
	BattleAudienceGXInit();
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	star = (u8*)audience->starpower;
	for (i = 0; i < 100; i++) {
		if (*(s32*)star & 1) {
			PSMTXScale(scale, *(f32*)(star + 0x38), *(f32*)(star + 0x3C), *(f32*)(star + 0x40));
			PSMTXRotRad(rot, 'z', float_0_8041e618);
			PSMTXTrans(trans, *(f32*)(star + 8), *(f32*)(star + 0xC), float_0_8041e618);
			PSMTXConcat(scale, rot, mtx);
			PSMTXConcat(trans, mtx, mtx);
#ifdef MUST_MATCH
			color = BattleAudienceDispApSrc_color(*(u8*)(star + 0x5C));
#else
			color = *(GXColor*)&dat_8041e608;
			color.a = *(u8*)(star + 0x5C);
#endif
			iconDispGxCol(mtx, 0x10, 0x1B2, color);
		}
		star += 0x60;
	}
}

void BattleAudienceDispWin(CameraId camId, void* param) {
	BattleAudienceWindow* win;
	GXColor color;
	GXColor numberColor;
	Vec pos;
	Mtx mtx;

	extern f32 float_90_8041e68c;
	extern f32 float_10_8041e694;
	extern f32 float_17_8041e698;
	extern u32 dat_8041e60c;
	extern u32 dat_8041e610;

	BattleAudienceBaseGetPtr();
	win = BattleAudienceWinGetPtr();
	color = *(GXColor*)&dat_8041e60c;
	windowDispGX_Waku_col(0, color, *(f32*)((u8*)win + 0x8), *(f32*)((u8*)win + 0xc), float_90_8041e68c,
	                      float_40_8041e690, float_10_8041e694);

	pos = vec3_802f97f4;
	pos.x = float_17_8041e698 + *(f32*)((u8*)win + 0x8);
	pos.y = *(f32*)((u8*)win + 0xc) - float_40_8041e690;
	iconDispGx(&pos, 0x10, 0x209, float_0p8_8041e69c);

	PSMTXTrans(mtx, float_75_8041e658 + *(f32*)((u8*)win + 0x8),
	           *(f32*)((u8*)win + 0xc) - float_40_8041e690, float_0_8041e618);
	numberColor = *(GXColor*)&dat_8041e610;
	iconNumberDispGx(mtx, (s32)win->count, FALSE, numberColor);
}

// NONMATCHING (99.91%): `audience_kind[type]` (no `(u8)` cast) is required so MWCC uses the
// parameter directly, and `m * 0x19 - 0xeb + r` (not `m * 0x19 + r - 0xeb`) is required so `r`
// lands in r6. The only remaining diff is one commuted `fadds` in the second branch.
BOOL BattleAudience_Entry(s32 index, AudienceMemberType type, u8 status) {
	BattleAudienceMember* member;
	f32 f;
	s32 r;
	s32 m;
	BattleAudienceBaseGetPtr();
	BattleStageGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((audience_kind[type].field_0x0 & 1) == 0) {
		if (member->flags & 1) {
			return FALSE;
		}
		if (member->flags & 0x100) {
			return FALSE;
		}
		member->flags = 5;
		r = rand() * 10 / 0x7fff;
		m = index % 20;
		f = (f32)(m * 0x19 - 0xeb + r);
		member->position.x = f;
		member->mHomePosition.x = f;
		BattleAudience_Entry_Sub(index, type, status);
	}
	else {
		if (index % 2 == 1) {
			return FALSE;
		}
		if (member->flags & 1) {
			return FALSE;
		}
		if (member->flags & 0x100) {
			return FALSE;
		}
		member->flags = 0x85;
		r = rand() * 10 / 0x7fff;
		m = index % 20;
		f = float_12p5_8041e688 + (f32)(m * 0x19 - 0xe6) + (f32)r - float_5_8041e62c;
		member->position.x = f;
		member->mHomePosition.x = f;
		BattleAudience_Entry_Sub(index, type, status);
		(&member[1])->flags = 0x101;
	}
	return TRUE;
}

#pragma opt_dead_assignments off // needed: stops MWCC folding col/row back together so the divide lands in the original's register
// NONMATCHING (90.35%): the i2f magic pools an @N double instead of double_to_int_802f9a00, so its `lis r3`
// steals the register the original puts `col = index / 20` in and the early block + seven Vec copies rotate.
// Writing `row` as `index - col * 20` (== `index % 20`) and this pragma lifted it 88.54 -> 90.22 -> 90.35.
void BattleAudience_Entry_Sub(s32 index, u8 type, u8 status) {
	BattleAudienceMember* member;
	BattleStage* stage;
#ifdef MUST_MATCH
	const Vec* vec = (const Vec*)str_802F9578;
#endif
	s32 col;
	s32 row;
	f32 y;
	f32 z;

	BattleAudienceBaseGetPtr();
	stage = BattleStageGetPtr();
	member = BattleAudienceGetPtr(index);

	member->field_0x4 = 0;
	*(s32*)member->field_0x8 = 0;
	member->status = status;
	member->type = type;
	member->itemId = -1;
	member->animTable = audience_kind[type].animTable;
	*(s32*)((u8*)member + 0x30) = 0;
	BattleAudience_SetAnim(index, 1, 1);

	col = index / 20;
	row = index - col * 20;
	*(u8*)((u8*)member + 0x40) = 0;
	*(u8*)((u8*)member + 0x43) = 0;
	*(u8*)((u8*)member + 0x42) = 0;
	*(u8*)((u8*)member + 0x41) = 0;
	*(u8*)((u8*)member + 0x44) = 0xff;

	y = *(f32*)((u8*)stage + 0xB34);
	z = float_0p1_8041e678 * (f32)row + (f32)(col * 0x1e + 0x73);
	member->position.y = y;
	member->mHomePosition.y = y;
	member->position.z = z;
	member->mHomePosition.z = z;

	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		f32 off = float_0p2_8041e67c;
		member->mHomePosition.z += off;
		member->position.z += off;
	}
	if (member->status == 0xb) {
		member->position.y = float_neg2000_8041e680;
	}

#ifdef MUST_MATCH
	// Match-only: addressing the vec3_802f98xx constants through the .rodata section symbol
	// (like BattleAudience_ChangeStatus) serialises each 12-byte copy; the named globals let MWCC
	// software-pipeline the loads across the previous copy's stores.
	*(Vec*)((unsigned char*)member + 0x84) = *(const Vec*)((const u8*)vec + 0x288);
	*(Vec*)((unsigned char*)member + 0x78) = *(Vec*)((unsigned char*)member + 0x84);
	*(Vec*)((unsigned char*)member + 0x90) = *(const Vec*)((const u8*)vec + 0x294);
	member->mRotation = *(const Vec*)((const u8*)vec + 0x2a0);
	member->mRotationOffset = *(const Vec*)((const u8*)vec + 0x2ac);
	*(Vec*)((unsigned char*)member + 0x54) = *(const Vec*)((const u8*)vec + 0x2b8);
	*(Vec*)((unsigned char*)member + 0x60) = *(const Vec*)((const u8*)vec + 0x2c4);
	*(Vec*)((unsigned char*)member + 0x6c) = *(const Vec*)((const u8*)vec + 0x2d0);
#else
	*(Vec*)((u8*)member + 0x84) = vec3_802f9800;
	*(Vec*)((u8*)member + 0x78) = *(Vec*)((u8*)member + 0x84);
	*(Vec*)((u8*)member + 0x90) = vec3_802f980c;
	member->mRotation = vec3_802f9818;
	member->mRotationOffset = vec3_802f9824;
	*(Vec*)((u8*)member + 0x54) = vec3_802f9830;
	*(Vec*)((u8*)member + 0x60) = vec3_802f983c;
	*(Vec*)((u8*)member + 0x6c) = vec3_802f9848;
#endif
	member->field_0xE8 = float_270_8041e684;

	if ((index >= 4 && index <= 0xf) ||
	    (index >= 0x18 && index <= 0x23) ||
	    (index >= 0x2c && index <= 0x37)) {
		member->flags |= 8;
	}
}
#pragma opt_dead_assignments reset

void BattleAudience_Delete(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->flags & 0x80) {
		*(s32*)((u8*)member + 0x134) = 0;
	}
	member->flags = 0;
}

void BattleAudience_SetAnim(s32 id, s32 animId, BOOL force) {
	BattleAudienceMember* member;
	s32* anim;
	s32 i;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(id);
	if (!(member->flags & 1)) {
		return;
	}
	if (member->flags & 0x100) {
		return;
	}
	anim = member->animTable[animId];
	if (*(s32**)((u8*)member + 0x24) != NULL && *(s32**)((u8*)member + 0x24) == anim && !force) {
		return;
	}
	*(s32**)((u8*)member + 0x24) = anim;
	*(s32**)((u8*)member + 0x28) = anim;
	*(s32*)((u8*)member + 0x2c) = 0;
	for (i = 0; i < 3; i++) {
		*(s32*)((u8*)member + 0x34 + i * 4) = 0;
	}
	*(u8*)((u8*)member + 0x40) = 0;
}
#pragma dont_inline reset

BOOL BattleAudience_GetAnimEnd(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	if (**(s32**)((u8*)member + 0x28) != -1) {
		return FALSE;
	}
	return *(s32*)((u8*)member + 0x2c) <= 0;
}

void BattleAudience_GetPosition(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->position.x;
	*y = member->position.y;
	*z = member->position.z;
}

void BattleAudience_GetHomePosition(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->mHomePosition.x;
	*y = member->mHomePosition.y;
	*z = member->mHomePosition.z;
}

void BattleAudience_GetRotate(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->mRotation.x;
	*y = member->mRotation.y;
	*z = member->mRotation.z;
}

void BattleAudience_SetPosition(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->position.x = x;
	member->position.y = y;
	member->position.z = z;
}

void BattleAudience_SetRotate(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->mRotation.x = x;
	member->mRotation.y = y;
	member->mRotation.z = z;
}

void BattleAudience_SetRotateOffset(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->mRotationOffset.x = x;
	member->mRotationOffset.y = y;
	member->mRotationOffset.z = z;
}

void BattleAudience_GetItemOn(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	BattleAudienceItem* item;
	int i;

	audience = BattleAudienceBaseGetPtr();
	i = 0;
	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		//TODO: invert this to multiple break; checks?
		if ((member->flags & 1) && !(member->flags & 0x100) && !(member->flags & 2)) {
			if (member->itemId != -1) {
				item = BattleAudienceItemGetPtr(member->itemId);
				if (item->state <= 4 || item->state == 7) {
					if (memberId) {
						audience->mItemOnMemberId = i;
						*memberId = i;
					}
					audience->mItemOnMemberPos.x = member->position.x;
					audience->mItemOnMemberPos.y = member->position.y;
					audience->mItemOnMemberPos.z = member->position.z + 3.0f;

					if (x) {
						*x = audience->mItemOnMemberPos.x;
					}
					if (y) {
						*y = audience->mItemOnMemberPos.y;
					}
					if (z) {
						*z = audience->mItemOnMemberPos.z;
					}

					if (itemType) {
						if (member->itemId < 0) {
							*itemType = ITEM_NULL;
						}
						else {
							*itemType = audience->items[member->itemId].mItemType;
						}
					}
					//has to pass the 4 checks up top, otherwise we keep looking at all 200 members
					return;
				}
			}
		}
	}
	*memberId = -1;
}

//might be inlined, 1 above has additional assignments
void BattleAudience_GetItemOn2(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType) {
	BattleAudience* audience;
	BattleAudienceMember* member;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(audience->mItemOnMemberId);
	if (memberId) {
		*memberId = audience->mItemOnMemberId;
	}

	if (x) {
		*x = audience->mItemOnMemberPos.x;
	}
	if (y) {
		*y = audience->mItemOnMemberPos.y;
	}
	if (z) {
		*z = audience->mItemOnMemberPos.z;
	}

	if (itemType) {
		if (member->itemId < 0) {
			*itemType = ITEM_NULL;
		}
		else {
			*itemType = audience->items[member->itemId].mItemType;
		}
	}
}

void BattleAudience_SetTarget(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	s32 i;
	member->flags |= 0x20;
	for (i = 0; i < 200; i++) {
		if (i == index) {
			continue;
		}
		if ((u8)BattleAudience_GetExist(i)) {
			BattleAudienceMember* m = BattleAudienceGetPtr(i);
			if (m->status == 7) {
				BattleAudienceItemGetPtr(m->itemId)->flags = 0;
				m->itemId = -1;
				BattleAudience_ChangeStatus(i, 0);
			}
		}
	}
}

void BattleAudience_Attack(s32 index) {
	BattleAudienceGetPtr(index)->flags |= 0x40;
}

// dont_inline: BattleAudienceDispAudience must emit a call to this function, not inline it
#pragma dont_inline on
BOOL BattleAudience_GetFront(s32 index) {
	BattleAudienceMember* member;
	CameraEntry* cam;
	f32 dx, dy, dz;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	cam = camGetPtr(CAMERA_3D);
	dx = cam->cameraAt.x - cam->cameraPos.x;
	dy = cam->cameraAt.y - cam->cameraPos.y;
	dz = cam->cameraAt.z - cam->cameraPos.z;
	return !(dx * *(f32*)((u8*)member + 0x90) +
	         dy * *(f32*)((u8*)member + 0x94) +
	         dz * *(f32*)((u8*)member + 0x98) >=
	         float_0_8041e618);
}
#pragma dont_inline reset

// dont_inline: the retail callers reach the functions up to the next reset out of line
#pragma dont_inline on
void BattleAudience_ChangeStatus(s32 index, s32 status) {
	BattleAudienceMember* member;
#ifdef MUST_MATCH
	// Match-only: the named vec3_802f98xx globals make MWCC's scheduler hoist a later copy's loads
	// across an earlier member store (direct-global form ~70%). The original keeps each 12-byte copy
	// serialized and bases it off the .rodata section symbol (r30). Addressing the same data through
	// that symbol -- str_802F9578+0x2DC is vec3_802f9854, +0x2E8 vec3_802f9860, +0x2F4 vec3_802f986c,
	// +0x300 vec3_802f9878 -- pins the callee-saved base and reproduces the original scheduling.
	const Vec* vec = (const Vec*)str_802F9578;
#endif
	s32 zero = 0;
	f32 zeroF;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->flags & 1) {
		if (member->flags & 0x100) {
			if (index >= 1) {
				BattleAudience_ChangeStatus(index - 1, status);
			}
		} else {
			*(u8*)((u8*)member + 0x43) = zero;
			// Match-only statement order: the original loads the zero float between these two stores.
			zeroF = float_0_8041e618;
			*(u8*)((u8*)member + 0x42) = zero;
			*(u8*)((u8*)member + 0x41) = zero;
			*(u8*)((u8*)member + 0x44) = 0xFF;
#ifdef MUST_MATCH
			member->mRotation = *(const Vec*)((const u8*)vec + 0x2DC);
			*(Vec*)((u8*)member + 0x6C) = *(const Vec*)((const u8*)vec + 0x2E8);
			*(Vec*)((u8*)member + 0x60) = *(const Vec*)((const u8*)vec + 0x2F4);
			*(Vec*)((u8*)member + 0x78) = *(const Vec*)((const u8*)vec + 0x300);
#else
			member->mRotation = vec3_802f9854;
			*(Vec*)((u8*)member + 0x6C) = vec3_802f9860;
			*(Vec*)((u8*)member + 0x60) = vec3_802f986c;
			*(Vec*)((u8*)member + 0x78) = vec3_802f9878;
#endif
			*(s32*)((u8*)member + 0x30) = zero;
			member->field_0x4 = zero;
			*(s32*)((u8*)member + 0x8) = zero;
			*(f32*)((u8*)member + 0x14) = zeroF;
			*(f32*)((u8*)member + 0x10) = zeroF;
			*(f32*)((u8*)member + 0xC) = zeroF;
			member->status = status;
		}
	}
}
#pragma dont_inline reset
//breaks -inline auto,deferred
/*
BOOL BattleAudience_GetExist(s32 id) {
	return FALSE;
}
*/

#pragma dont_inline on
BOOL BattleAudience_GetExist(s32 id) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(id);
	if (!(member->flags & 1)) {
		return FALSE;
	}
	if (member->flags & 0x100) {
		return FALSE;
	}
	return !(member->flags & 2);
}

BOOL BattleAudience_GetSysCtrl(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetExist(index) == 0) {
		return FALSE;
	}
	return !(member->flags & 0x10);
}

BOOL BattleAudience_GetWaiting(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if (member->itemId != -1) {
		return FALSE;
	}
	return (u8)(member->status <= 3);
}

BOOL BattleAudience_GetEscapeChangeOK(s32 index) {
	BattleAudience* audience;
	BattleAudienceMember* member;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (audience->flags & 0x4000) {
		return FALSE;
	}
	if (!(u8)BattleAudience_GetSysCtrl(index)) {
		return FALSE;
	}
	if (member->flags & 0x200) {
		return FALSE;
	}
	if (member->status == 13) {
		return FALSE;
	}
	if (member->status == 17) {
		return FALSE;
	}
	if (member->status == 18) {
		return FALSE;
	}
	if (member->status == 14) {
		return FALSE;
	}
	if (member->status == 15) {
		return FALSE;
	}
	if (member->status == 21) {
		return FALSE;
	}
	if (member->status == 22) {
		return FALSE;
	}
	return member->status != 12;
}
#pragma dont_inline reset

#pragma dont_inline on
s32 BattleAudience_GetAudienceNoFromOffset(s32 index, s32 column, s32 row) {
	s32 audienceNo;

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	if (index % 20 + column < 0 || index % 20 + column >= 20) {
		return -1;
	}

	audienceNo = column + (index - (row * 20));
	if (audienceNo < 0 || audienceNo >= 200) {
		return -1;
	}
	return audienceNo;
}
#pragma dont_inline reset

#pragma dont_inline on
s32 BattleAudience_HaitiRandForFallObject(void) {
	s32 r = irand(3);
	s32 ret = 10;
	u32 unused = irand(12);

	switch (r) {
	case 0:
		ret = irand(12) + 4;
		break;
	case 1:
		ret = irand(10) + 5;
		break;
	case 2:
		ret = irand(8) + 6;
		break;
	}
	return ret + r * 20;
}

s32 BattleAudience_NoUsedHaitiRand(void) {
	s32 stack[200];
	s32* haiti;
	s32 memberId;
	s32 count;
	s32 i;
	s32 j;
	s32 start;

	BattleAudienceBaseGetPtr();
	haiti = haiti_data[pouchGetPtr()->rank];
	count = 0;
	for (i = 0; i < 10; i++) {
		start = (20 - haiti[i]) / 2;
		j = 0;
		while (j < haiti[i]) {
			memberId = start + i * 20;
			if (!(BattleAudienceGetPtr(memberId)->flags & 1) &&
			    !(BattleAudienceGetPtr(memberId)->flags & 0x100)) {
				stack[count] = memberId;
				count++;
			}
			j++;
			start++;
		}
	}
	if (count >= 1) {
		return stack[irand(count)];
	}
	return -1;
}

s32 BattleAudience_NoUsedFCHaitiRand(void) {
	s32 rate[10];
	s32 offset[20];
	s32 ids[200];
	s32 sums[200];
	PouchData* pouch;
	s32* haiti;
	s32 count;
	s32 total;
	s32 i, j;
	s32 start;
	s32 memberId;
	s32 pouchRank;

	BattleAudienceBaseGetPtr();
	pouch = pouchGetPtr();
#ifdef MUST_MATCH
	// A named rank (read before the const-table copies) reproduces the original's
	// `lha` placement and frees r3 for the copy loops; `pouch->rank` inline sinks.
	pouchRank = pouch->rank;
	count = 0;
	total = 0;
#else
	count = 0;
	total = 0;
#endif
#ifdef MUST_MATCH
	{
		*(BattleAudienceHaitiRateTable*)rate = *(const BattleAudienceHaitiRateTable*)dat_802f9884;
	}
	{
		*(BattleAudienceHaitiOffsetTable*)offset = *(const BattleAudienceHaitiOffsetTable*)dat_802f98ac;
	}
#else
	memcpy(rate, dat_802f9884, sizeof(rate));
	memcpy(offset, dat_802f98ac, sizeof(offset));
#endif
#ifdef MUST_MATCH
	haiti = haiti_data[pouchRank];
#else
	haiti = haiti_data[pouch->rank];
#endif
	for (i = 0; i < 10; i++) {
		start = (20 - haiti[i]) / 2;
		j = 0;
		while (j < haiti[i]) {
			memberId = start + i * 20;
			if (!(BattleAudienceGetPtr(memberId)->flags & 1) &&
			    !(BattleAudienceGetPtr(memberId)->flags & 0x100)) {
				s32 w;
				w = rate[i] * offset[start] + 1;
				ids[count] = memberId;
				if (count != 0) {
					sums[count] = sums[count - 1] + w;
				} else {
					sums[count] = w;
				}
				total += w;
				count++;
			}
			j++;
			start++;
		}
	}
	{
		s32 pick = (s32)((f32)total * (f32)rand() / float_32767_8041e674);

		for (i = 0; i < count; i++) {
			if (pick < sums[i]) {
				return ids[i];
			}
		}
	}
	return -1;
}
#pragma dont_inline reset

s32 BattleAudience_GetAudienceNum(void) {
	return BattleAudienceBaseGetPtr()->mCurrentAudienceIntCount;
}

// Keep this call out of line: the original's BattleAudience_Case_Appeal emits `bl
// BattleAudience_GetPPAudienceNum`, and MWCC would otherwise inline the wrapper.
#pragma dont_inline on
s32 BattleAudience_GetPPAudienceNum(void) {
	s32 left = BattleAudience_GetPPAudienceNum_L();
	s32 right = BattleAudience_GetPPAudienceNum_R();
	return right + left;
}
#pragma dont_inline reset

// Keep out of line: BattleAudience_ApRecoveryBuild emits `bl BattleAudience_GetPPAudienceNumKind`.
#pragma dont_inline on
s32 BattleAudience_GetPPAudienceNumKind(s32 type) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i, count = 0;

	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetPPAudienceNum_Sub(i)) {
			if (BattleAudienceGetPtr(i)->type == type) {
				count++;
			}
		}
	}
	return count;
}
#pragma dont_inline reset

#pragma dont_inline on
s32 BattleAudience_GetPPAudienceNum_R(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(10);
}

s32 BattleAudience_GetPPAudienceNum_L(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(0);
}

s32 BattleAudience_GetPPAudienceNum_RL_Sub(s32 offset) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 akk;
	s32 i, j;
	s32 count = 0;

	count = 0;
	for (i = 0, akk = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
// MUST_MATCH: this operand order makes MWCC emit `akk+offset` then `j+...`, as the original does.
#ifdef MUST_MATCH
			if (BattleAudience_GetPPAudienceNum_Sub(j + akk + offset) == 1) {
#else
			if (BattleAudience_GetPPAudienceNum_Sub(akk + (offset + j)) == 1) {
#endif
				count++;
			}
		}
		akk += 20;
	}
	return count;
}

BOOL BattleAudience_GetPPAudienceNum_Sub(s32 id) {
	BattleAudienceMember* member = BattleAudienceGetPtr(id);

	if (!(u8)BattleAudience_GetExist(id)) {
		return FALSE;
	}

	if (13 <= member->status && member->status <= 13) {
		return FALSE;
	}

	if (member->status == 17) {
		return FALSE;
	}

	return member->status != 12;
}
#pragma dont_inline reset































































// NONMATCHING (98.94%): register allocation only; the outer counter and the inner weighted-pick accumulator
// land in r26/r25 instead of r25/r26, shifting the rest of the body. Declaring the negative branch's
// `count` at function scope (instead of inside the `else`) moves it to r26 and lifts this 98.72 -> 98.94.
s32 BattleAudienceAddAudienceNum(s32 num) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 sum;
	s32 total = 0;
	u8 type;
	s32 result = 0;
	s32 count;
	s32 j;
	s32 i;
	s32 id;
	s32 waitlist[200];

	if (pouchGetPtr()->mStarPowersObtained == 0) {
		return 0;
	}
	if (num == 0) {
		return 0;
	}
	if (wp->flags & 0x100) {
		return 0;
	}
	if (num > 0) {
		for (i = 0; i < 12; i++) {
			total += *(s32*)((u8*)audience + 0x13794 + i * 4);
		}
		for (j = 0; j < num; j++) {
			sum = 0;
			id = irand(total);
			for (i = 0; i < 12; i++) {
				sum += *(s32*)((u8*)audience + 0x13794 + i * 4);
				if (id < sum) {
					type = (u8)i;
					break;
				}
			}
			id = BattleAudience_NoUsedFCHaitiRand();
			if (id == -1) {
				break;
			}
			if (BattleAudience_Entry(id, type, 0xb) == 1) {
				result++;
				*(s32*)((u8*)BattleAudienceGetPtr(id) + 0x8) = result;
			}
		}
	}
	else {
		count = 0;
		for (; count > num; count--) {
			s32 n = 0;
			for (i = 0; i < 200; i++) {
				if ((u8)BattleAudience_GetEscapeChangeOK(i)) {
					waitlist[n++] = i;
				}
			}
			if (n >= 1) {
				id = waitlist[irand(n)];
				BattleAudience_ChangeStatus(id, 0xc);
				result--;
				*(s32*)((u8*)BattleAudienceGetPtr(id) + 0x8) = -result;
			}
		}
	}
	return result;
}

//1:1
// keep AddTargetNumSub out of AddTargetNum (original has a `bl`)
#pragma dont_inline on
void BattleAudienceAddTargetNumSub(f32 value) {
	f32 subtract;
	s32 skew;

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	while (1) {
		if (value == 0.0f) return;
		if (value > 0.0f) { //positive
			subtract = min(value, 1.0f);
			value -= subtract;
			if (value < 0.0f) {
				value = 0.0f;
			}
		}
		if (value < 0.0f) { //negative
			subtract = max(value, -1.0f);
			value -= subtract;
			if (value > 0.0f) {
				value = 0.0f;
			}
		}
		if (subtract > 0.0f) {
			skew = (s32)(((audience->mTargetAudienceCount / audience->mAudienceDeltaMultiplier) - 1.0f) / 0.25f);
			if (skew >= 1) {
				audience->mTargetAudienceCount += subtract / (skew + 1);
				continue;
			}
		}
		if (subtract < 0.0f) {
			skew = (s32)(((audience->mTargetAudienceCount / audience->mAudienceDeltaMultiplier) - 1.0f) / 0.25f);
			if (skew <= -1) {
				audience->mTargetAudienceCount -= subtract / (skew - 1);
				continue;
			}
		}
		audience->mTargetAudienceCount += subtract;
	}
}

void BattleAudienceAddTargetNum(f32 value, f32 delta) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	audience->mAddedAudienceCount += delta;
	if (audience->mAddedAudienceCount > 0.0f && value < 0.0f) {
		audience->mAddedAudienceCount += value;
		if (audience->mAddedAudienceCount < 0.0f) {
			value = audience->mAddedAudienceCount;
			audience->mAddedAudienceCount = 0.0f;
		}
		else {
			value = 0.0f;
		}
	}
	else if (audience->mAddedAudienceCount < 0.0f && value > 0.0f) {
		audience->mAddedAudienceCount += value;
		if (audience->mAddedAudienceCount > 0.0f) {
			value = audience->mAddedAudienceCount;
			audience->mAddedAudienceCount = 0.0f;
		}
		else {
			value = 0.0f;
		}
	}
	BattleAudienceAddTargetNumSub(value);
	if (audience->mTargetAudienceCount < 0.0f) {
		audience->mTargetAudienceCount = 0.0f;
	}
	if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
		audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
	}
	if (audience->mAddedAudienceCount < -1000.0f) {
		audience->mAddedAudienceCount = -1000.0f;
	}
	if (audience->mAddedAudienceCount > 1000.0f) {
		audience->mAddedAudienceCount = 1000.0f;
	}
}
#pragma dont_inline reset














void BattleAudienceNoiseMain(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(2);
	s32 pp = BattleAudience_GetPPAudienceNum();

	if (!(audience->flags & 0x10000) && (wp->flags & 0x80)) {
		BattleAudienceSoundSetVol(2, 0x3C, 0x50);
	}
	if ((audience->flags & 0x10000) && !(wp->flags & 0x80)) {
		BattleAudienceSoundSetVol(2, 0, 0x50);
	}
	if (pp == 0 && *(u8*)((u8*)sound + 8) != 0) {
		psndSFXOff(*(s32*)((u8*)sound + 4));
		*(u8*)((u8*)sound + 8) = 0;
	}
	if (pp >= 1 && pp <= 0x31 && *(u8*)((u8*)sound + 8) != 1) {
		BattleAudienceSoundNoiseAlways();
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (pp >= 0x32 && pp <= 0x63 && *(u8*)((u8*)sound + 8) != 2) {
		BattleAudienceSoundNoiseAlways();
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (pp >= 0x64 && pp <= 0x95 && *(u8*)((u8*)sound + 8) != 3) {
		BattleAudienceSoundNoiseAlways();
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (pp >= 0x96 && pp <= 0xC8 && *(u8*)((u8*)sound + 8) != 4) {
		BattleAudienceSoundNoiseAlways();
		*(u8*)((u8*)sound + 8) = 4;
	}
	if (!(audience->flags & 0x2000)) {
		sound->mCurrentBaseSfxVolume = 0;
	}
	else if (pp == 0) {
		sound->mCurrentBaseSfxVolume = 0;
	}
	else {
		sound->mCurrentBaseSfxVolume = (s32)(float_63_8041e644 + float_63_8041e644 * (f32)pp / float_200_8041e648);
	}
}

// Keep the call out of line: the original emits `bl BattleAudienceSoundMain` in BattleAudience_Main.
#pragma dont_inline on
void BattleAudienceSoundMain(void) {
	BattleAudienceSound* sound;
	s32 i;

	BattleAudienceBaseGetPtr();
	BattleAudienceNoiseMain();
	i = 0;
	sound = BattleAudienceSoundGetPtr(0);
	for (; i < 0x18; i++, sound++) {
		if (psndSFXChk(*(s32*)((u8*)sound + 4)) == -1) {
			*(s32*)sound &= ~1;
		}
		else if (*(s32*)((u8*)sound + 0xC) == 0) {
			psndSFXOff(*(s32*)((u8*)sound + 4));
			*(s32*)sound &= ~1;
		}
		else {
			if (sound->mForceFadeLength > 0) {
				sound->mForceFadeTimer++;
				sound->mVolumeMultiplier = (u8)intplGetValue(0, sound->mForceFadeTimer, sound->mForceFadeLength, (f32)sound->mFadeStartVolumeMultiplier, (f32)sound->mFadeEndVolumeMultiplier);
				psndSFX_vol(*(s32*)((u8*)sound + 4), (u8)((sound->mCurrentBaseSfxVolume * sound->mVolumeMultiplier) / 100));
				if (sound->mForceFadeTimer >= sound->mForceFadeLength) {
					sound->mForceFadeLength = 0;
				}
			}
			if (*(s32*)((u8*)sound + 0xC) > 0) {
				if (*(s32*)((u8*)sound + 0xC) == sound->mFadeoutTime) {
					sound->mFadeStartVolumeMultiplier = sound->mVolumeMultiplier;
					sound->mFadeEndVolumeMultiplier = 0;
				}
				if (*(s32*)((u8*)sound + 0xC) <= sound->mFadeoutTime) {
					sound->mVolumeMultiplier = (u8)intplGetValue(0, sound->mFadeoutTime - *(s32*)((u8*)sound + 0xC), sound->mFadeoutTime, (f32)sound->mFadeStartVolumeMultiplier, (f32)sound->mFadeEndVolumeMultiplier);
					psndSFX_vol(*(s32*)((u8*)sound + 4), (u8)((sound->mCurrentBaseSfxVolume * sound->mVolumeMultiplier) / 100));
				}
				*(s32*)((u8*)sound + 0xC) -= 1;
			}
		}
	}
}
#pragma dont_inline reset


// Keep the call out of line: the original emits `bl BattleAudienceSoundNoiseAlways` in
// BattleAudienceNoiseMain and BattleAudience_Init.
#pragma dont_inline on
void BattleAudienceSoundNoiseAlways(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(2);
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	char name[0x40];

	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	if (!(*(s32*)sound->field_0x0 & 1)) {
		if (total == 0) {
			sound->field_0x0[8] = 0;
		}
		if (total >= 1 && total <= 0x31) {
			strcpy(name, str_802F9C4C);
			sound->field_0x0[8] = 1;
		}
		if (total >= 0x32 && total <= 0x63) {
			strcpy(name, str_802F9C60);
			sound->field_0x0[8] = 2;
		}
		if (total >= 0x64 && total <= 0x95) {
			strcpy(name, str_802F9C74);
			sound->field_0x0[8] = 3;
		}
		if (total >= 0x96) {
			strcpy(name, str_802F9C88);
			sound->field_0x0[8] = 4;
		}
		*(s32*)(sound->field_0x0 + 4) = BattleAudienceSound1(name, r, l);
		if (*(s32*)(sound->field_0x0 + 4) != -1) {
			*(s32*)sound->field_0x0 |= 1;
			*(s32*)(sound->field_0x0 + 0xC) = -1;
			sound->mFadeoutTime = 0;
			vol = psndSFX_get_vol(*(s32*)(sound->field_0x0 + 4));
			sound->mCurrentBaseSfxVolume = vol;
			sound->mBaseSfxVolume = vol;
			sound->mForceFadeLength = 0;
			sound->mForceFadeTimer = 0;
			sound->mVolumeMultiplier = 100;
			audience->flags |= 0x2000;
		}
	}
}
#pragma dont_inline reset

#pragma dont_inline on
void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3) {
	BattleAudienceSound* sound;

	sound = BattleAudienceSoundGetPtr(index);
	sound->mFadeStartVolumeMultiplier = sound->mVolumeMultiplier;
	sound->mFadeEndVolumeMultiplier = a2;
	sound->mForceFadeTimer = 0;
	sound->mForceFadeLength = a3;
	sound->mFadeoutTime = 0;
}
#pragma dont_inline reset

// Keep BattleAudienceNumToTarget as an out-of-line `bl`: both original callers (Main,
// Case_TurnEnd) call it, and MWCC would otherwise inline it into them.
#pragma dont_inline on
void BattleAudienceNumToTarget(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 target;
	if (!(audience->flags & 0x40000)) {
		target = (s32)(audience->mTargetAudienceCount + audience->mAddedAudienceCount);
		if (target < 0) {
			target = 0;
		}
		if (target > audience->mMaxAudience) {
			target = audience->mMaxAudience;
		}
		BattleAudienceAddAudienceNum(target - BattleAudienceNumToTargetSub());
	}
}
#pragma dont_inline reset

// Keep BattleAudience_WinSetActive out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BattleAudience_WinSetActive(u8 active) {
	BattleAudienceWinGetPtr()->mActive = active;
}
#pragma dont_inline reset

#pragma opt_lifetimes off // needed: keep the loop accumulator in f31 and the i2f double in f30 (see below)
// Four levers got this from 93.45% to 99.82%: (a) reaching the 0x137C8 store/memcpy through a `u8*` view (audlocal) makes MWCC recompute `audience + 0x10000` with addis and a fresh
// register instead of CSEing an addis result across BattleAlloc; (b) writing the two `> 0.0f`
// comparisons and the `crowdKindSum = 0.0f` reset as float literals (not float_0_8041e618) keeps each
// a separate load instead of being CSE'd; (c) the declaration order below (crowdKindSum first, then
// partyMultiplier) puts the multiplier count in f31 to match the original. (d) `opt_lifetimes off`
// resolves the loop's FPR tie: with lifetimes on MWCC kept the pooled i2f double in f31 and the
// accumulator in f30; with it off the accumulator keeps f31 and the double takes f30 like the
// original, which also fixes the FMUL and `add` operand orders that followed from that tie. (e) in the
// slot branch, an initialized accumulator (`s32 delta = 0; delta += (s32)(...)`) before the
// `*(...0x137C4) += delta;` store makes MWCC load the running count into r0 instead of r3, matching
// the original's count/delta registers. What is left is one commuted `fmuls` in that branch:
// original `fmuls f0, f0, f2` (accum * slot->field_0x14), ours `fmuls f0, f2, f0`. The else branch
// (no slot) matches exactly.
// NONMATCHING (99.94%): slot branch's final FMUL operand order only (see above).
void BattleAudience_ApRecoveryBuild(StarPowerInfo* info) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleUnit* mario;
	BattleUnit* party;
	BattleBreakSlot* slot;
	s32 i;
	f32 crowdKindSum;
	f32 partyMultiplier;
	f32 sqrtWeight;
	f32 spAcMultiplier;
	f32 stylishMultiplier;
	f32 marioMultiplier;

	{
		u8* audlocal = (u8*)audience;
		if (*(void**)(audlocal + 0x137C8) == NULL) {
			*(void**)(audlocal + 0x137C8) = BattleAlloc(0xC);
		}
		memcpy(*(void**)(audlocal + 0x137C8), info, 0xC);
	}
	mario = BattleGetMarioPtr(wp);
	party = BattleGetPartyPtr(wp);
	crowdKindSum = float_0_8041e618;
	for (i = 0; i < 13; i++) {
		crowdKindSum += audience_kind[i].field_0x14 * (f32)BattleAudience_GetPPAudienceNumKind(i);
	}
	sqrtWeight = (f32)sqrt(crowdKindSum);
	spAcMultiplier = info->mSpAcSuccessMultiplier;
	stylishMultiplier = (f32)(u8)info->mStylishCommandMultiplier;
	crowdKindSum = 0.0f;
	if (spAcMultiplier > 0.0f) {
		crowdKindSum += float_1_8041e620;
	}
	if (stylishMultiplier > 0.0f) {
		crowdKindSum += float_1_8041e620;
	}
	if (mario->mStatusFlags & 0x20000000) {
		marioMultiplier = float_3_8041e65c;
	}
	else if (mario->mStatusFlags & 0x10000000) {
		marioMultiplier = float_2_8041e660;
	}
	else {
		marioMultiplier = float_1_8041e620;
	}
	if (party != NULL) {
		if (party->mStatusFlags & 0x20000000) {
			partyMultiplier = float_2_8041e660;
		}
		else if (party->mStatusFlags & 0x10000000) {
			partyMultiplier = float_1p5_8041e670;
		}
		else {
			partyMultiplier = float_1_8041e620;
		}
	}
	else {
		partyMultiplier = float_1_8041e620;
	}
	if (BattleBreakSlot_GetBreakTurn() >= 1) {
		s32 delta = 0;
		slot = BattleBreakSlotGetPtr();
		delta += (s32)(float_1_8041e620 * sqrtWeight * (spAcMultiplier + stylishMultiplier) *
		               crowdKindSum * marioMultiplier * partyMultiplier *
		               *(f32*)((u8*)slot + 0x14));
		*(s32*)((u8*)audience + 0x137C4) += delta;
	}
	else {
		*(s32*)((u8*)audience + 0x137C4) += (s32)(float_1_8041e620 * sqrtWeight * (spAcMultiplier + stylishMultiplier) *
		                                            crowdKindSum * marioMultiplier * partyMultiplier);
	}
	if (irand(100) < (u8)info->mBingoSlotChance) {
		BattleBreakSlot_PointInc();
	}
}
#pragma opt_lifetimes reset

void BattleAudience_Case_JumpNewRecord(void) {
}

void BattleAudience_Case_FinalAttack(void) {
}

void BattleAudience_Case_Escape(void) {
}

// The assignment's operand order is load-bearing: MWCC emits the target's
// `fmuls f0, f1, f0` (converted value first) only when the int->float conversion is
// held in a named temp, and only keeps the audience field in r0 and writes the
// result back through r0 when the sum is written as `(v + 0x19) + field`. The
// natural compound assignment is the #else arm.
void BattleAudience_Case_Appeal(BattleUnit* unit) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	if ((f32)BattleAudience_GetPPAudienceNum() > float_0_8041e618) {
#ifdef MUST_MATCH
		f32 num = (f32)BattleAudience_GetPPAudienceNum();
		s32 v = (s32)(num * float_0p25_8041e63c);
		*(s32*)((u8*)audience + 0x137C4) = (v + 0x19) + *(s32*)((u8*)audience + 0x137C4);
#else
		*(s32*)((u8*)audience + 0x137C4) += (s32)(float_0p25_8041e63c * (f32)BattleAudience_GetPPAudienceNum()) + 0x19;
#endif
		if (unit->badgesEquipped.SuperAppeal != 0) {
			*(s32*)((u8*)audience + 0x137C4) += unit->badgesEquipped.SuperAppeal * 0x19;
		}
		BattleAudienceSoundCheer(0xB4, 0x3C);
		BattleAudienceSoundClap(0xB4, 0x3C);
		BattleAudienceJoy(0);
	}
	BattleAudienceAddTargetNum(float_0p05_8041e624 * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
	                           float_0_8041e618);
}

void BattleAudience_Case_TurnEnd(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* mario = BattleGetMarioPtr(wp);
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 expected;
	BattleGroupSetup* setup;
	BattleUnitSetup* enemy;
	s32 i;

	pouchGetPtr();
	expected = 0;
	setup = wp->fbatInfo->setup->groupSetup;
	enemy = setup->enemyData;
	if (!(audience->flags & 0x40000)) {
		audience->mTurnEndPhaseEventChance += 3;
		if (audience->mTurnEndPhaseEventChance > 100) {
			audience->mTurnEndPhaseEventChance = 100;
		}
		for (i = 0; i < setup->numEnemies; i++) {
			expected += enemy->mUnitKindParams->mLevel;
			enemy++;
		}
		expected = (expected * 2) / mario->mLevel;
		if (expected < 5) {
			expected = 5;
		}
		if (wp->turnCount >= expected) {
			BattleAudienceAddTargetNum(float_neg0p05_8041e614 * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			                           float_0_8041e618);
		}
		if (wp->turnCount == expected) {
			BattleAudienceAddPhaseEvtList(6);
		}
		if (irand(100) < audience->mTurnEndPhaseEventChance) {
			if (wp->turnCount >= expected) {
				BattleAudienceAddPhaseEvtList(1);
			}
			BattleAudienceAddPhaseEvtList(4);
			BattleAudienceAddPhaseEvtList(9);
			BattleAudienceAddPhaseEvtList(10);
			BattleAudienceAddPhaseEvtList(11);
			BattleAudienceAddPhaseEvtList(12);
			BattleAudienceAddPhaseEvtList(13);
			audience->mTurnEndPhaseEventChance = 0;
		}
		BattleAudienceNumToTarget();
	}
}

s32 BattleAudienceSound0(const char* name) {
	return psndSFXOn(name);
}

void BattleAudience_Case_AcrobatNoTry(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudience_Case_MarioBigDamage(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudienceJoyEnding(void) {
	BattleAudienceJoy(3);
}

BOOL BattleAudienceSoundCheck(s32 index) {
	return *(s32*)BattleAudienceSoundGetPtr(index) & 1;
}

// Keep these accessors out of line: BattleAudienceItemCtrlProcess emits a bl to each.
#pragma dont_inline on
s32 BattleAudience_GetPresentItemNo(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E4);
}

s32 BattleAudience_GetPresentTargetUnitId(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137EC);
}

s32 BattleAudience_GetPresentItemType(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E8);
}

void BattleAudience_SetPresentItemNo(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E4) = value;
}

void BattleAudience_SetPresentTargetUnitId(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137EC) = value;
}

void BattleAudience_SetPresentItemType(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E8) = value;
}
#pragma dont_inline reset

void BattleAudienceSoundWhistleKind(s32 kind) {
	s32 waiting[200];
	s32 i;
	s32 vol;
	s32 member;
	s32 kindIdx;
	s32 count = 0;
	Vec pos;
	BattleAudienceSound* sound;

	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			waiting[count++] = i;
		}
	}
	if (count == 0) {
		return;
	}
	member = waiting[irand(count)];
	BattleAudienceSoundGetInfo2(member, &pos);
	if (pos.x <= float_0_8041e618) {
		kindIdx = 6;
	}
	else {
		kindIdx = 7;
	}
	sound = BattleAudienceSoundGetPtr(kindIdx);
	BattleAudienceSoundStop(kindIdx);
	switch (kind) {
	case 1:
		*(s32*)((u8*)sound + 4) = BattleAudienceSound2(str_802F9B94, member);
		break;
	case 2:
		*(s32*)((u8*)sound + 4) = BattleAudienceSound2(str_802F9BAC, member);
		break;
	case 3:
		*(s32*)((u8*)sound + 4) = BattleAudienceSound2(str_802F9BC4, member);
		break;
	}
	if (*(s32*)((u8*)sound + 4) != -1) {
		*(s32*)sound |= 1;
		*(s32*)((u8*)sound + 0xC) = -1;
		sound->mFadeoutTime = 0;
		vol = psndSFX_get_vol(*(s32*)((u8*)sound + 4));
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudienceSoundWhistle(void) {
	BattleAudienceSoundWhistleKind(irand(3) + 1);
}

void BattleAudienceSoundCheerKind(s32 kind, s32 a, s32 b) {
	BattleAudienceSound* sound;
	BattleAudienceSound* snd4;
	BattleAudienceSound* snd5;
	s32 r;
	s32 l;
	s32 flag;
	s32 vol;
	char name[0x40];

	BattleAudienceBaseGetPtr();
	flag = 0;
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	snd4 = BattleAudienceSoundGetPtr(4);
	snd5 = BattleAudienceSoundGetPtr(5);
	if ((*(s32*)snd4 & 1) && (*(s32*)snd5 & 1)) {
		if (*(s32*)((u8*)snd4 + 0xC) <= *(s32*)((u8*)snd5 + 0xC)) {
			BattleAudienceSoundStop(4);
			sound = BattleAudienceSoundGetPtr(4);
		}
		else {
			BattleAudienceSoundStop(5);
			sound = BattleAudienceSoundGetPtr(5);
		}
	}
	else if (*(s32*)snd4 & 1) {
		sound = BattleAudienceSoundGetPtr(5);
		flag = 1;
	}
	else if (*(s32*)snd5 & 1) {
		sound = BattleAudienceSoundGetPtr(4);
		flag = 1;
	}
	else {
		sound = BattleAudienceSoundGetPtr(4);
	}
	if (kind == 1) {
		strcpy(name, str_802F9BDC);
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (kind == 2) {
		strcpy(name, str_802F9BF8);
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (kind == 3) {
		strcpy(name, str_802F9C14);
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (kind == 4) {
		strcpy(name, str_802F9C30);
		*(u8*)((u8*)sound + 8) = 4;
	}
	if (flag == 1) {
		*(s32*)((u8*)sound + 4) = BattleAudienceSound1(name, r, l);
		psndSFX_pit(*(s32*)((u8*)sound + 4), 0x7F);
	}
	else {
		*(s32*)((u8*)sound + 4) = BattleAudienceSound1(name, r, l);
	}
	if (*(s32*)((u8*)sound + 4) != -1) {
		*(s32*)sound |= 1;
		*(s32*)((u8*)sound + 0xC) = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(*(s32*)((u8*)sound + 4));
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudience_Case_KillEnemy(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x1000;
}

void BattleAudience_Case_GuardGood(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x40;
}

void BattleAudience_Case_GuardBad(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x80;
}

void BattleAudience_Case_EnemyDamage(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x400;
}

void BattleAudience_Case_EnemyNoDamage(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x800;
}

void BattleAudienceJoySACLecture(void) {
	BattleAudienceSoundCheer(0x78, 0x3c);
	BattleAudienceJoy(2);
}

void BattleAudience_Case_MarioPinch(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_MarioDanger(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p15_8041e630 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_PartyDown(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p25_8041e640 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_HaikeiSet(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_GreatVictory(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p15_8041e630 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FallObject_Aud(s32 index, u8 type) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 tbl1[16];
	s32 tbl2[48];
	s32 i;
	s32 id;
	f32 one = float_1_8041e620;

#ifdef MUST_MATCH
	// Struct assignment gives the original's lwzu/stwu copy loop; memcpy doesn't.
	{
		*(BattleAudienceFallOffsetTable1*)tbl1 = *(const BattleAudienceFallOffsetTable1*)dat_802f98fc;
	}
	{
		*(BattleAudienceFallOffsetTable2*)tbl2 = *(const BattleAudienceFallOffsetTable2*)dat_802f993c;
	}
#else
	memcpy(tbl1, dat_802f98fc, sizeof(tbl1));
	memcpy(tbl2, dat_802f993c, sizeof(tbl2));
#endif

	switch (type) {
	case 0:
	case 1:
	case 3:
	case 4:
	case 5:
	case 8:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		for (i = 0; i < 8; i++) {
			id = BattleAudience_GetAudienceNoFromOffset(index, tbl1[i * 2], tbl1[i * 2 + 1]);
			if (id != -1 && (u8)BattleAudience_GetEscapeChangeOK(id)) {
				BattleAudience_ChangeStatus(id, 0xC);
				audience->mTargetAudienceCount -= float_1_8041e620;
			}
		}
		break;
	case 7:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		for (i = 0; i < 0x18; i++) {
			id = BattleAudience_GetAudienceNoFromOffset(index, tbl2[i * 2], tbl2[i * 2 + 1]);
			if (id != -1 && (u8)BattleAudience_GetEscapeChangeOK(id)) {
				BattleAudience_ChangeStatus(id, 0xC);
				audience->mTargetAudienceCount -= float_1_8041e620;
			}
		}
		break;
	case 6:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		one = float_1_8041e620;
		for (i = 0; i < 200; i++) {
			if ((u8)BattleAudience_GetEscapeChangeOK(i)) {
				BattleAudience_ChangeStatus(i, 0xC);
				audience->mTargetAudienceCount -= one;
			}
		}
		/* falls through */
	default:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		audience->mTargetAudienceCount -= float_1_8041e620;
		break;
	}
	BattleAudiencePuniAllEscape();
}

void BattleAudience_Case_FallObject_Stage(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FirstAttack_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p02_8041e61c * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FirstAttack_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p05_8041e614 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_Escape_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p2_8041e638 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_Countered(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p1_8041e634 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceAddPhaseEvtList(5);
}

void BattleAudience_Case_Escape_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p1_8041e634 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceSoundSetVol(2, 100, 30);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_FastVictory(f32 value) {
	BattleAudienceAddTargetNum(
	    float_0p01_8041e628 * (float_5_8041e62c * value) * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
	    float_0_8041e618);
}

#pragma dont_inline on
void BattleAudienceSoundGetInfo2(s32 index, Vec* pos) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	pos->x = member->position.x;
	pos->y = member->position.y;
	pos->z = member->position.z;
}

void BattleAudienceSoundGetInfo1(s32 a, s32 b, u8* c, Vec* d) {
	s32 sum = a + b;

	if (sum == 0) {
		*c = 0;
	}
	else {
		*c = (u8)(s32)(float_100_8041e64c + float_26_8041e650 * (f32)sum / float_200_8041e648);
	}
	if (b == 0 || sum == 0) {
		d->x = float_250_8041e654;
	}
	else {
		d->x = (f32)(u8)(s32)((f32)(a - b) * float_250_8041e654 / (f32)sum);
	}
	d->y = 0.0f;
	d->z = 0.0f;
}

void BattleAudienceSoundSetVolAll(u8 a2, s32 a3) {
	s32 i;
	for (i = 0; i < 24; i++) {
		BattleAudienceSoundSetVol(i, a2, a3);
	}
}

void BattleAudienceSoundStop(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(index);
	if (*(s32*)sound & 1) {
		psndSFXOff(*(s32*)((u8*)sound + 4));
		*(s32*)sound &= ~1;
	}
}

s32 BattleAudienceSound2(const char* name, s32 index) {
	Vec pos;
	BattleAudienceSoundGetInfo2(index, &pos);
	return psndSFXOn_3D(name, &pos);
}

s32 BattleAudienceSound1(const char* name, s32 a, s32 b) {
	u8 vol;
	Vec pos;
	s32 handle;
	BattleAudienceSoundGetInfo1(a, b, &vol, &pos);
	handle = psndSFXOn_3D(name, &pos);
	psndSFX_vol(handle, vol);
	return handle;
}


#define AUD_SOUND_HEAD(sound) \
	*(s32*)((u8*)(sound)) |= 1; \
	*(s32*)((u8*)(sound) + 0xC) = -1; \
	(sound)->mFadeoutTime = 0

#define AUD_SOUND_TAIL(sound, vol) \
	(sound)->mCurrentBaseSfxVolume = (vol); \
	(sound)->mBaseSfxVolume = (vol); \
	(sound)->mForceFadeLength = 0; \
	(sound)->mForceFadeTimer = 0; \
	(sound)->mVolumeMultiplier = 100

#define AUD_SOUND_ID(sound) (*(s32*)((u8*)(sound) + 4))

void BattleAudienceSoundZZZ(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x10);
	s32 vol;
	BattleAudienceSoundStop(0x10);
	BattleAudienceSound2(str_802F9A9C, index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundSing(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x11);
	s32 vol;
	BattleAudienceSoundStop(0x11);
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A80, index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundPakkunEat(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x17);
	s32 vol;
	BattleAudienceSoundStop(0x17);
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A10, index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundRun(s32 kind) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0xd);
	s32 vol;
	BattleAudienceSoundStop(0xd);
	switch (kind) {
		case 0:
			AUD_SOUND_ID(sound) = BattleAudienceSound0(str_802F9AEC);
			break;
		case 1:
			AUD_SOUND_ID(sound) = BattleAudienceSound0(str_802F9B00);
			break;
	}
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundShell(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
		idx = 0x12;
	}
	else {
		idx = 0x13;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A60, index);
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundBombFire(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
		idx = 0x15;
	}
	else {
		idx = 0x16;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A28, index);
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundBombIgnite(s32 index, s32 a, s32 b) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x14);
	s32 vol;
	BattleAudienceSoundStop(0x14);
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A44, index);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundItemThrow(s32 kind, s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
		idx = 0xa;
	}
	else {
		idx = 0xb;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		switch (kind) {
			case 0:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9B14, index);
				break;
			default:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9B30, index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundSleep(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
		idx = 0xe;
	}
	else {
		idx = 0xf;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		switch (irand(3)) {
			case 0:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AB0, index);
				break;
			case 1:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AC4, index);
				break;
			case 2:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AD8, index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

s32 BattleAudienceNumToTargetSub(void) {
	s32 i;
	s32 count;

	BattleAudienceBaseGetPtr();
	count = 0;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetExist(i) == 1) {
			if (BattleAudienceGetPtr(i)->status != 12) {
				count++;
			}
		}
	}
	return count;
}

void BattleAudience_Case_AcrobatBad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count >= 1) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x200;
}

void BattleAudience_Case_AcrobatGood(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count >= 1) {
		BattleAudienceSoundCheer(0xb4, 0x78);
		BattleAudienceSoundClap(0xb4, 0x78);
		BattleAudienceJoy(1);
	}
	audience->mNumStylishCommandsThisAttack++;
	if (audience->mNumStylishCommandsThisAttack > 5) {
		audience->mNumStylishCommandsThisAttack = 5;
	}
	audience->flags |= 0x100;
}

void BattleAudience_Case_ActionCommandGood(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	s32 count = 0;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundCheer(0xb4, 0x78);
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x10;
}

void BattleAudience_Case_ActionCommandBad(void* arg) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
		if (*(u32*)((u8*)arg + 0x1c) == (u32)weaponGetPowerFromMarioHammerLv) {
			BattleAudienceAddPhaseEvtList(7);
		}
	}
	audience->flags |= 0x20;
}

void BattleAudienceCheer(s32 a) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (*(s32*)(audience->field_13794 + 0x30) > 0x320) {
		*(s32*)(audience->field_13794 + 0x30) = 0x320;
	}
	if (*(s32*)(audience->field_13794 + 0x30) > 0) {
		if (pouchGetPtr()->maximumSP == 0) {
			*(s32*)(audience->field_13794 + 0x30) = 0;
		}
		else {
			audience->flags |= 2;
		}
		BattleAudienceJoy(a);
		if (*(s32*)(audience->field_13794 + 0x30) != 0) {
			BattleStatusWindowAPRecoveryOn();
		}
	}
}

void BattleAudiencePuniAllEscape(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 one = float_1_8041e620;
	s32 i;
	s32 count = 0;
	for (i = 0; i < 200; i++) {
		BattleAudienceMember* member = BattleAudienceGetPtr(i);
		if ((u8)BattleAudience_GetEscapeChangeOK(i)) {
			if (member->type == AUDIENCE_PUNI) {
				BattleAudience_ChangeStatus(i, 12);
				count++;
				audience->mAddedAudienceCount -= one;
			}
		}
	}
	if (count >= 1) {
		audience->flags |= 0x8000;
	}
}

void BattleAudienceSoundBooing(void) {
	s32 kind;
	s32 l;
	s32 r;
	s32 count;
	BattleAudienceBaseGetPtr();
	kind = 0;
	l = BattleAudience_GetPPAudienceNum_L();
	r = BattleAudience_GetPPAudienceNum_R();
	count = r + l;
	if (count >= 0 && count <= 0x31) {
		kind = 1;
	}
	if (count >= 0x32 && count <= 0x63) {
		kind = 2;
	}
	if (count >= 0x64 && count <= 0x95) {
		kind = 3;
	}
	if (count >= 0x96) {
		kind = 4;
	}
	BattleAudienceSoundBooingKind(kind);
}

void BattleAudienceSoundCheer(s32 a, s32 b) {
	s32 kind;
	s32 l;
	s32 r;
	s32 count;
	BattleAudienceBaseGetPtr();
	kind = 0;
	l = BattleAudience_GetPPAudienceNum_L();
	r = BattleAudience_GetPPAudienceNum_R();
	count = r + l;
	if (count >= 1 && count <= 0x31) {
		kind = 1;
	}
	if (count >= 0x32 && count <= 0x63) {
		kind = 2;
	}
	if (count >= 0x64 && count <= 0x95) {
		kind = 3;
	}
	if (count >= 0x96) {
		kind = 4;
	}
	BattleAudienceSoundCheerKind(kind, a, b);
}

void BattleAudienceSoundCallKind(s32 kind) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 vol;
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(8);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	BattleAudienceSoundStop(8);
	switch (kind) {
	case 1:
		AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B64, r, l);
		break;
	case 2:
		AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B7C, r, l);
		break;
	}
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundHandBeat(void) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(9);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	BattleAudienceSoundStop(9);
	AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B4C, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = -1;
		sound->mFadeoutTime = 0;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		vol = 0;
// MUST_MATCH: writing the guard as total==0 (else compute) is what makes MWCC lay the
// compute block out-of-line (bne+b); the equivalent total!=0 form picks the beq layout.
#ifdef MUST_MATCH
		if (total == 0) {
			vol = 0;
		}
		else {
			vol = (s32)(float_75_8041e658 + ((f32)total * (float_26_8041e650 + (f32)irand(26))) / float_200_8041e648);
		}
#else
		if (total != 0) {
			vol = (s32)(float_75_8041e658 + ((f32)total * (float_26_8041e650 + (f32)irand(26))) / float_200_8041e648);
		}
#endif
		psndSFX_vol(AUD_SOUND_ID(sound), vol);
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudienceAddPhaseEvtList(s32 evtId) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = *(s32*)((u8*)audience + 0x13904);
	s32 i = 0;
	for (; i < count; i++) {
		if (*(s32*)((u8*)audience + 0x138CC + i * 4) == evtId) {
			return;
		}
	}
	*((s32*)((u8*)audience + 0x138CC) + count) = evtId;
	*(s32*)(audience->field_138CC + 0x38) = *(s32*)(audience->field_138CC + 0x38) + 1;
}

s32 BattleAudienceDetectPakkunEatTargetSub2(s32 a, s32 b) {
	s32 no;
	BattleAudienceBaseGetPtr();
	no = BattleAudience_GetAudienceNoFromOffset(a, b, 0);
	if (no == -1) {
		return -1;
	}
	BattleAudienceGetPtr(no);
	if ((u8)BattleAudience_GetSysCtrl(no) != 1) {
		return -1;
	}
	return no;
}

s32 BattleAudienceDetectPakkunEatTargetSub(s32 a, s32 b, s32 c) {
	s32 x = BattleAudienceDetectPakkunEatTargetSub2(a, b);
	s32 y = BattleAudienceDetectPakkunEatTargetSub2(a, c);
	if (x != -1) {
		return x;
	}
	if (y == -1) {
		return -1;
	}
	return y;
}

s32 BattleAudienceDetectPakkunEatTarget(s32 a) {
	if (irand(2) == 0) {
		return BattleAudienceDetectPakkunEatTargetSub(a, -1, 1);
	}
	return BattleAudienceDetectPakkunEatTargetSub(a, 1, -1);
}

void BattleAudienceAddPuni(s32 num) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	if (pouchGetPtr()->mStarPowersObtained != 0 && *(s32*)((u8*)audience + 0x137B0) > 0 &&
			num > 0 && !(wp->flags & 0x100)) {
		f32 one = float_1_8041e620;
		for (i = 0; i < num; i++) {
			s32 idx = BattleAudience_NoUsedFCHaitiRand();
			if (idx == -1) {
				return;
			}
			if (BattleAudience_Entry(idx, AUDIENCE_PUNI, 0xb) == 1) {
				audience->mAddedAudienceCount += one;
			}
		}
	}
}

void BattleAudienceSoundClap(s32 a, s32 b) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	char name[0x3c];
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(1);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	BattleAudienceSoundStop(1);
	if (total >= 1 && total <= 0x31) {
		strcpy(name, str_802F9C9C);
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (total >= 0x32 && total <= 0x63) {
		strcpy(name, str_802F9CB4);
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (total >= 0x64 && total <= 0x95) {
		strcpy(name, str_802F9CCC);
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (total >= 0x96) {
		strcpy(name, str_802F9CE4);
		*(u8*)((u8*)sound + 8) = 4;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundBooingKind(s32 kind) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 vol;
	char name[0x34];
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(0);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	BattleAudienceSoundStop(0);
	if (kind == 1) {
		strcpy(name, str_802F9CFC);
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (kind == 2) {
		strcpy(name, str_802F9D14);
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (kind == 3) {
		strcpy(name, str_802F9D2C);
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (kind == 4) {
		strcpy(name, str_802F9D44);
		*(u8*)((u8*)sound + 8) = 4;
	}
	if (kind == 5) {
		strcpy(name, str_802F9D5C);
		*(u8*)((u8*)sound + 8) = 5;
	}
	if (kind == 6) {
		strcpy(name, str_802F9D74);
		*(u8*)((u8*)sound + 8) = 6;
	}
	if (kind == 7) {
		strcpy(name, str_802F9D8C);
		*(u8*)((u8*)sound + 8) = 7;
	}
	if (kind == 8) {
		strcpy(name, str_802F9DA4);
		*(u8*)((u8*)sound + 8) = 8;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

#ifdef MUST_MATCH
// Match-only: the third bound's `(a + b) + c` must keep its operands in that order; written
// directly MWCC commutes the final add to `c + (a + b)`. The inlined add leaves the tree alone.
static inline s32 BattleAudienceJoyAdd2(s32 x, s32 y) {
	return x + y;
}
#else
#define BattleAudienceJoyAdd2(x, y) ((x) + (y))
#endif

#pragma opt_loop_invariants off // needed: keep a,b,c,d live and recompute the branch bounds each iteration

// The frame, prologue and every call now match. The third branch bound's commuted `add` is fixed by
// routing it through the inlined BattleAudienceJoyAdd2 helper above. One commuted `add` remains: the
// second sum add (`add r29, r29, r25` where the original has `add r29, r25, r29`). `sum = b + c;
// sum += a;` is required (writing `a + sum` makes MWCC reassociate the first add and recolour the
// callee-saved set). The declaration order member/i/sum is load-bearing: MWCC colours the
// callee-saved locals in declaration order (member r31, i r30, sum r29); any other order rotates them.
// NONMATCHING (99.86%): one commuted `add` operand on the loop-invariant sum.
void BattleAudienceJoy_Sub(s32 a, s32 b, s32 c, s32 d) {
	BattleAudienceMember* member;
	s32 i;
	s32 sum;
	BattleAudience* audience;
	s32 r;
	audience = BattleAudienceBaseGetPtr();
	sum = b + c;
	sum += a;
	i = 0;
	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		if ((u8)BattleAudience_GetWaiting(i)) {
			if (member->type == AUDIENCE_PIRANHA_PLANT) {
				BattleAudience_ChangeStatus(i, 4);
				if (d == 3) {
					*(f32*)((u8*)member + 0xc) = float_3_8041e65c;
				}
				else {
					*(f32*)((u8*)member + 0xc) = float_2_8041e660;
				}
			}
			else {
				r = irand(sum);
				if (r < a) {
					BattleAudience_ChangeStatus(i, 4);
					if (d == 3) {
						*(f32*)((u8*)member + 0xc) = float_3_8041e65c;
					}
					else {
						*(f32*)((u8*)member + 0xc) = float_2_8041e660;
					}
				}
				else if (r < a + b) {
					BattleAudience_ChangeStatus(i, 5);
				}
				else if (r < BattleAudienceJoyAdd2(a + b, c)) {
					BattleAudience_ChangeStatus(i, 6);
				}
			}
		}
	}
}
#pragma opt_loop_invariants reset

void BattleAudienceJoy(s32 a) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	switch (a) {
		case 0:
			BattleAudienceJoy_Sub(0x64, 0, 0, a);
			break;
		case 1:
			BattleAudienceJoy_Sub(0, 0x64, 0, a);
			break;
		case 2:
			BattleAudienceJoy_Sub(0x46, 0x1e, 0, a);
			break;
		case 3:
		switch (*(s32*)audience->field_1390C) {
			case 0:
				BattleAudienceJoy_Sub(0x32, 0x28, 0xa, a);
				break;
			case 1:
				BattleAudienceJoy_Sub(0x28, 0x28, 0x14, a);
				break;
			case 2:
				BattleAudienceJoy_Sub(0x1e, 0x28, 0x1e, a);
				break;
			case 3:
				BattleAudienceJoy_Sub(0x14, 0x28, 0x28, a);
				break;
			case 4:
				BattleAudienceJoy_Sub(0xa, 0x28, 0x32, a);
				break;
			default:
				BattleAudienceJoy_Sub(0x32, 0x28, 0xa, a);
				break;
		}
		(*(s32*)audience->field_1390C)++;
		if (*(s32*)audience->field_1390C > 4) {
			*(s32*)audience->field_1390C = 4;
		}
			break;
	}
}

#pragma dont_inline reset
