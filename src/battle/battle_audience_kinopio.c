#include "battle/battle_stage_object.h"
#include "battle/battle_camera.h"
#include "battle/battle_audience.h"
#include "battle/battle.h"
#include "battle/battle_stage.h"
#include "battle/battle_unit.h"
#include "battle/battle_damage.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "manager/evtmgr.h"
#include "system.h"
#include "eff/eff_sleep.h"
#include <string.h>
#include "battle/battle_break_slot.h"
#include "battle/battle_audience_kinopio.h"
#include "eff/eff_sandars.h"
#include "eff/n64/eff_onpu_n64.h"
#include "eff/eff_small_star.h"
#include <stdlib.h>
#include "math_ext.h"

#pragma pool_data on // anchor the .rodata blob at vec3_802f8c78 so pooled loads are base+offset


extern BattleWork* _battleWorkPointer;

static f32 x_873;
static f32 y_874;
static f32 z_875;

// .rodata
const Vec vec3_802f8c78 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_802f8c84 = { 0.0f, 0.0f, 0.0f };
__declspec(section ".rodata") Vec vec3_802f8c90 = { 0.0f, 0.0f, 0.0f };
const s32 dat_802f8c9c[24][2] = {
    { 0, 3 },   { -1, 2 },  { 0, 2 },   { 1, 2 },   { -2, 1 },  { -1, 1 },
    { 0, 1 },   { 1, 1 },   { 2, 1 },   { -3, 0 },  { -2, 0 },  { -1, 0 },
    { 1, 0 },   { 2, 0 },   { 3, 0 },   { -2, -1 }, { -1, -1 }, { 0, -1 },
    { 1, -1 },  { 2, -1 },  { -1, -2 }, { 0, -2 },  { 1, -2 },  { 0, -3 },
};

extern u8 weapon_teresa_trans[];
static s32 intrudeSound;

const f64 double_to_int_802f8d60 = 4503601774854144.0;
const char str_802F8D68[] = "SFX_AUDIENCE_HEIHO_MOVE1";
const f64 double_450_802f8d88 = 450.0;
const f64 double_300_802f8d90 = 300.0;
const char str_802F8D98[] = "SFX_AUDIENCE_TELESA_MOVE1";
const char str_802F8DB4[] = "SFX_AUDIENCE_TELESA_INVISIBLE1";
const char str_802F8DD4[] = "SFX_AUDIENCE_RUN3";
const char str_802F8DE8[] = "SFX_AUDIENCE_PANIC_SCREAM2";


// .sdata2 constants referenced by symbol name in the original
f32 float_270_8041e170 = 270.0f;
f32 float_90_8041e174 = 90.0f;
f32 float_0_8041e178 = 0.0f;
f32 float_6_8041e17c = 6.0f;
f32 float_neg60_8041e180 = -60.0f;
f32 float_60_8041e184 = 60.0f;
f32 float_1_8041e188 = 1.0f;
f32 float_neg1_8041e18c = -1.0f;
f32 float_neg100_8041e190 = -100.0f;
f32 float_0p1_8041e194 = 0.1f;
f32 float_0p05_8041e198 = 0.05f;
f32 float_35_8041e19c = 35.0f;
f32 float_5_8041e1a0 = 5.0f;
f32 float_neg0p3_8041e1a4 = -0.3f;
f32 float_100_8041e1a8 = 100.0f;
f32 float_25_8041e1ac = 25.0f;
f32 float_110_8041e1b0 = 110.0f;
f32 float_10_8041e1b4 = 10.0f;
f32 float_neg0p7_8041e1b8 = -0.7f;
f32 float_neg1p5_8041e1bc = -1.5f;
f32 float_300_8041e1c0 = 300.0f;
f32 float_698_8041e1c4 = 698.0f;
f32 float_185_8041e1c8 = 185.0f;
f32 float_20_8041e1cc = 20.0f;
f32 float_40_8041e1d0 = 40.0f;
f32 float_80_8041e1d4 = 80.0f;
f32 float_15_8041e1d8 = 15.0f;
f32 float_0p0025_8041e1dc = 0.0025f;
f32 float_45_8041e1e0 = 45.0f;
f32 float_32767_8041e1e4 = 32767.0f;
f32 float_neg0p5_8041e1e8 = -0.5f;
f32 float_2_8041e1ec = 2.0f;
f32 float_120_8041e1f0 = 120.0f;
f32 float_8_8041e1f4 = 8.0f;
f32 float_600_8041e1f8 = 600.0f;
f32 float_1000_8041e1fc = 1000.0f;
f32 float_neg0p15_8041e200 = -0.14999999f;
f32 float_4_8041e204 = 4.0f;
f32 float_neg0p2_8041e208 = -0.2f;
f32 float_3p4_8041e20c = 3.4f;
f32 float_1p5_8041e210 = 1.5f;
f32 float_12_8041e214 = 12.0f;
f32 float_0p75_8041e218 = 0.75f;
f32 float_0p5_8041e21c = 0.5f;
f32 float_0p25_8041e220 = 0.25f;
f32 float_1p75_8041e224 = 1.75f;
f32 float_1p25_8041e228 = 1.25f;
f32 float_0p005_8041e22c = 0.005f;
f32 float_188_8041e230 = 188.0f;
f32 float_16_8041e234 = 16.0f;
f32 float_24_8041e238 = 24.0f;
f32 float_32_8041e23c = 32.0f;
f32 float_36_8041e240 = 36.0f;
f32 float_17_8041e244 = 17.0f;
f32 float_0p00175_8041e248 = 0.00175f;
f32 float_22_8041e24c = 22.0f;
f32 float_0p00375_8041e250 = 0.00375f;
f32 float_64_8041e254 = 64.0f;
f32 float_31_8041e258 = 31.0f;
f32 float_0p00125_8041e25c = 0.00125f;
f32 float_6p2832_8041e260 = 6.283184f;

// Helpers implemented in battle/battle_audience.c and elsewhere (declared locally;
// these should move to a header).


#pragma dont_inline on // dispatcher must emit real `bl`s, not inline the state machines
void BattleAudienceCtrlProcessKinopio(s32 index) {
	BattleAudienceMember* member;
	BOOL front;

	member = BattleAudienceGetPtr(index);
	front = BattleAudience_GetFront(index);
	(void)front;
	switch (member->status) {
	case 0:
		BattleAudienceCtrlProcessKinopioWait(index, front);
		break;
	case 1:
		BattleAudienceCtrlProcessKinopioWaitHurimuki(index, front);
		break;
	case 2:
		BattleAudienceCtrlProcessKinopioWaitKubiKatamuke(index, front);
		break;
	case 3:
		BattleAudienceCtrlProcessKinopioWaitJump(index, front);
		break;
	case 4:
		BattleAudienceCtrlProcessKinopioJoyRaiseArm(index, front);
		break;
	case 5:
		BattleAudienceCtrlProcessKinopioJoyJump(index, front);
		break;
	case 6:
		BattleAudienceCtrlProcessKinopioJoyJump2(index, front);
		break;
	case 7:
		BattleAudienceCtrlProcessKinopioItemOn(index, front);
		break;
	case 8:
		BattleAudienceCtrlProcessKinopioItemThrow(index);
		break;
	case 9:
		BattleAudienceCtrlProcessKinopioBooing(index);
		break;
	case 0xa:
		BattleAudienceCtrlProcessKinopioDamage(index);
		break;
	case 0xb:
		BattleAudienceCtrlProcessKinopioEnter(index);
		break;
	case 0xc:
		BattleAudienceCtrlProcessKinopioEscape(index);
		break;
	case 0xd:
		BattleAudienceCtrlProcessKinopioSleep(index, front);
		break;
	case 0xe:
		BattleAudienceCtrlProcessKinopioTransEvt(index);
		break;
	case 0xf:
		BattleAudienceCtrlProcessKinopioIntrude(index);
		break;
	case 0x10:
		BattleAudienceCtrlProcessKinopioSing(index);
		break;
	case 0x11:
		BattleAudienceCtrlProcessKinopioShell(index);
		break;
	case 0x12:
		BattleAudienceCtrlProcessKinopioIgnite(index, front);
		break;
	case 0x13:
		BattleAudienceCtrlProcessKinopioFire(index, front);
		break;
	case 0x14:
		BattleAudienceCtrlProcessKinopioEat(index);
		break;
	}
	{
	f32 zero = 0.0f;
	member->position.x += *(f32*)((u8*)member + 0x78);
	*(f32*)((u8*)member + 0x78) +=
	    *(f32*)((u8*)member + 0x84);
	member->position.y += *(f32*)((u8*)member + 0x7c);
	*(f32*)((u8*)member + 0x7c) +=
	    *(f32*)((u8*)member + 0x88);
	member->position.z += *(f32*)((u8*)member + 0x80);
	*(f32*)((u8*)member + 0x80) +=
	    *(f32*)((u8*)member + 0x8c);
	if (*(f32*)((u8*)member + 0x78) > zero) {
		member->field_0xE8 = float_90_8041e174;
	}
	else if (*(f32*)((u8*)member + 0x78) < zero) {
		member->field_0xE8 = float_270_8041e170;
	}
	if (*(f32*)((u8*)member + 0xd0) < zero) {
		*(f32*)((u8*)member + 0xd0) = zero;
		*(f32*)((u8*)member + 0xdc) = zero;
		*(f32*)((u8*)member + 0xe4) = zero;
	}
	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		*(f32*)((u8*)member + 0xdc) = zero;
		*(f32*)((u8*)member + 0xd0) = zero;
		*(f32*)((u8*)member + 0xe4) = zero;
	}
	else {
		*(f32*)((u8*)member + 0xd0) =
		    *(f32*)((u8*)member + 0xd0) + *(f32*)((u8*)member + 0xdc);
		*(f32*)((u8*)member + 0xdc) =
		    *(f32*)((u8*)member + 0xdc) + *(f32*)((u8*)member + 0xe4);
	}
	}
	if (member->type == 2) {
		f32 c = 0.05f;
		f32 tau = 6.283184f;
		f32 angle = *(f32*)&member->field_0x12C;
		*(f32*)&member->field_0x12C = angle + c;
		if (*(f32*)&member->field_0x12C >= tau) {
			*(f32*)&member->field_0x12C = *(f32*)&member->field_0x12C - tau;
		}
		*(f32*)((u8*)member + 0x130) =
		    float_5_8041e1a0 * (f32)sin(*(f32*)((u8*)member + 0x12c));
		*(f32*)((u8*)member + 0xc0) =
		    member->position.x + *(f32*)((u8*)member + 0xcc);
		*(f32*)((u8*)member + 0xc4) = member->position.y +
		                              *(f32*)((u8*)member + 0xd0) +
		                              *(f32*)((u8*)member + 0x130);
		*(f32*)((u8*)member + 0xc8) =
		    member->position.z + *(f32*)((u8*)member + 0xd4);
	}
	else {
		*(f32*)((u8*)member + 0xc0) =
		    member->position.x + *(f32*)((u8*)member + 0xcc);
		*(f32*)((u8*)member + 0xc4) =
		    member->position.y + *(f32*)((u8*)member + 0xd0);
		*(f32*)((u8*)member + 0xc8) =
		    member->position.z + *(f32*)((u8*)member + 0xd4);
	}
}

// NONMATCHING (97.53%): constants are literals, declarations ordered step/rangeA/rangeB/level.
// The itemId == -1 branch folds `t = 0x14` to `cmpwi r28,0x14` and contracts `(f32)t + 10.0f*rangeA`
// to fmadds where the original keeps t in r27 and converts via the 2^52 magic.
void BattleAudienceCtrlProcessKinopioWait(s32 index, s32 kind) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	f32 step;
	f32 rangeA;
	f32 rangeB;
	f32 level;
	f32 ratio;

	pouchGetPtr();
	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	ratio = (audience->mTargetAudienceCount + audience->mAddedAudienceCount) /
	        audience->mAudienceDeltaMultiplier;
	// The period (level) and the step per frame (step) come from the same ladder. The original leaves the
	// period unset in the last case, where the step is 0.
	if (ratio >= 0.75f) {
		level = 1.0f;
		step = 1.0f;
	}
	else if (ratio >= 0.5f) {
		level = 2.0f;
		step = 0.5f;
	}
	else if (ratio >= 0.25f) {
		level = 4.0f;
		step = 0.25f;
	}
	else {
		step = 0.0f;
	}
	if (ratio >= 1.75f) {
		rangeA = 10.0f;
		rangeB = 4.0f;
	}
	else if (ratio >= 1.5f) {
		rangeA = 5.0f;
		rangeB = 2.0f;
	}
	else if (ratio >= 1.25f) {
		rangeA = 2.0f;
		rangeB = 1.0f;
	}
	else if (ratio >= 1.0f) {
		rangeA = 1.0f;
		rangeB = 0.0f;
	}
	else {
		rangeA = 0.0f;
		rangeB = rangeA;
	}
	switch (member->type) {
	case 4:
		if (*(s32*)((u8*)member + 0x30) > 0xb4) {
			if (*(s32*)((u8*)member + 0x30) <= 0xbc) {
				*(f32*)((u8*)member + 0x64) += 0.005f * step;
			}
			else if ((f32)(*(s32*)((u8*)member + 0x30)) <=
			         8.0f * level + 188.0f) {
				*(f32*)((u8*)member + 0x64) -= 0.005f * step;
			}
			else if ((f32)(*(s32*)((u8*)member + 0x30)) <=
			         16.0f * level + 188.0f) {
				*(f32*)((u8*)member + 0x64) += 0.005f * step;
			}
			else if ((f32)(*(s32*)((u8*)member + 0x30)) <=
			         24.0f * level + 188.0f) {
				*(f32*)((u8*)member + 0x64) -= 0.005f * step;
			}
			else if ((f32)(*(s32*)((u8*)member + 0x30)) <=
			         32.0f * level + 188.0f) {
				*(f32*)((u8*)member + 0x64) += 0.005f * step;
			}
			else if ((f32)(*(s32*)((u8*)member + 0x30)) <=
			         40.0f * level + 188.0f) {
				*(f32*)((u8*)member + 0x64) -= 0.005f * step;
			}
			else {
				*(f32*)((u8*)member + 0x64) = 1.0f;
				*(s32*)((u8*)member + 0x30) = 0;
			}
		}
		(*(s32*)((u8*)member + 0x30))++;
		break;
	case 6:
		if ((f32)(*(s32*)((u8*)member + 0x30)) >= 36.0f * level) {
			*(f32*)((u8*)member + 0x64) = 1.0f;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if ((f32)(*(s32*)((u8*)member + 0x30)) <= 17.0f * level) {
			*(f32*)((u8*)member + 0x64) += 0.00175f * step;
		}
		else {
			*(f32*)((u8*)member + 0x64) -= 0.00175f * step;
		}
		(*(s32*)((u8*)member + 0x30))++;
		break;
	case 7:
		if ((f32)(*(s32*)((u8*)member + 0x30)) >= 22.0f * level) {
			*(f32*)((u8*)member + 0x64) = 1.0f;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if ((f32)(*(s32*)((u8*)member + 0x30)) <= 10.0f * level) {
			*(f32*)((u8*)member + 0x64) += 0.00375f * step;
		}
		else {
			*(f32*)((u8*)member + 0x64) -= 0.00375f * step;
		}
		(*(s32*)((u8*)member + 0x30))++;
		break;
	case 9:
		if ((f32)(*(s32*)((u8*)member + 0x30)) >= 64.0f * level) {
			*(f32*)((u8*)member + 0x64) = 1.0f;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if ((f32)(*(s32*)((u8*)member + 0x30)) <= 31.0f * level) {
			*(f32*)((u8*)member + 0x64) += 0.00125f * step;
		}
		else {
			*(f32*)((u8*)member + 0x64) -= 0.00125f * step;
		}
		(*(s32*)((u8*)member + 0x30))++;
		break;
	case 2:
		break;
	default:
		if ((f32)(*(s32*)((u8*)member + 0x30)) >= 32.0f * level) {
			*(f32*)((u8*)member + 0x64) = 1.0f;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if ((f32)(*(s32*)((u8*)member + 0x30)) <= 15.0f * level) {
			*(f32*)((u8*)member + 0x64) += 0.0025f * step;
		}
		else {
			*(f32*)((u8*)member + 0x64) -= 0.0025f * step;
		}
		(*(s32*)((u8*)member + 0x30))++;
		break;
	}
	if (member->flags & 0x40) {
		member->flags &= ~0x60;
		BattleAudience_ChangeStatus(index, 0xa);
		if (BattleAudienceItemGetPtr(0)->mItemType < 0xec ||
		    BattleAudienceItemGetPtr(0)->mItemType >= 0xf0) {
			BattleAudienceAddTargetNum(
			    -0.14999999f * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			    0.0f);
		}
	}
	else if (member->flags & 0x20) {
	}
	else if (member->itemId == -1) {
		s32 r = irand(0x57e40);
		if (r < 0xa) {
			BattleAudience_ChangeStatus(index, 1);
		}
		else {
			s32 t = 0x14;
			if (r < t) {
				BattleAudience_ChangeStatus(index, 2);
			}
			else {
			t = (s32)((f32)t + 10.0f * rangeA);
			if (r < t) {
				BattleAudience_ChangeStatus(index, 4);
			}
			else if (BattleBreakSlot_GetBreakTurn() >= 1 || audience->excited == 1) {
				if (r < t + 0x12c && member->type != AUDIENCE_PIRANHA_PLANT) {
					BattleAudience_ChangeStatus(index, 3);
				}
			}
			else {
				t += (s32)(10.0f * rangeB);
				if (r < t) {
					s32 id = BattleAudience_GetAudienceNoFromOffset(index, 0, 1);
					if (id != -1) {
						BattleAudienceGetPtr(id);
						if (member->type != AUDIENCE_PIRANHA_PLANT &&
						    (u8)BattleAudience_GetExist(id) == 1) {
							BattleAudience_ChangeStatus(index, 3);
						}
					}
				}
			}
		}
		}
	}
}

void BattleAudienceCtrlProcessKinopioWaitHurimuki(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	if (*(s32*)((u8*)member + 0x30) <= 0xf) {
		member->mRotation.y =
		    float_12_8041e214 * (f32)(*(s32*)((u8*)member + 0x30));
	}
	if (*(s32*)((u8*)member + 0x30) >= 0x1e) {
		member->mRotation.y =
		    float_12_8041e214 * (f32)(0x2d - *(s32*)((u8*)member + 0x30));
	}
	if (*(s32*)((u8*)member + 0x30) > 0x2d) {
		BattleAudience_ChangeStatus(index, 0);
	}
	else {
		(*(s32*)((u8*)member + 0x30))++;
	}
}

void BattleAudienceCtrlProcessKinopioWaitKubiKatamuke(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	if (*(s32*)((u8*)member + 0x30) >= 0 && *(s32*)((u8*)member + 0x30) <= 0xa) {
		*(f32*)((u8*)member + 0x6c) =
		    float_1p5_8041e210 * (f32)(*(s32*)((u8*)member + 0x30));
	}
	if (*(s32*)((u8*)member + 0x30) >= 0x14 && *(s32*)((u8*)member + 0x30) <= 0x28) {
		*(f32*)((u8*)member + 0x6c) -= float_1p5_8041e210;
	}
	if (*(s32*)((u8*)member + 0x30) >= 0x32 && *(s32*)((u8*)member + 0x30) <= 0x3c) {
		*(f32*)((u8*)member + 0x6c) += float_1p5_8041e210;
	}
	if (*(s32*)((u8*)member + 0x30) > 0x3c) {
		BattleAudience_ChangeStatus(index, 0);
	}
	else {
		(*(s32*)((u8*)member + 0x30))++;
	}
}

// The original source writes these four constants as literals, which MWCC pools itself. The
// file's named float globals are non-const (so they live in .sdata and schedule differently),
// which colours the 0x1c/0x24 pair f0/f1 instead of the target's f1/f0. Under MUST_MATCH the
// literals reproduce the original pooling; the named-global form (what the file's other
// functions use) is the #else. Matches at 100%.
void BattleAudienceCtrlProcessKinopioWaitJump(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 2, 0);
	}
	else {
		BattleAudience_SetAnim(index, 3, 0);
	}
	switch (member->field_0x4) {
	case 0:
		if (member->type == 7) {
			*(s32*)member->field_0x8 = 3;
		}
		else {
			*(s32*)member->field_0x8 = 1;
		}
		member->field_0x4++;
		// fallthrough
	case 1:
		if (member->type == 7) {
#ifdef MUST_MATCH
			f32 v = 5.0f;
			f32 w = -1.0f;
#else
			f32 v = float_5_8041e1a0;
			f32 w = float_neg1_8041e18c;
#endif
			*(f32*)(member->field_0xC0 + 0x1c) = v;
			*(f32*)(member->field_0xC0 + 0x24) = w;
		}
		else {
#ifdef MUST_MATCH
			f32 v = 3.4f;
			f32 w = -0.2f;
#else
			f32 v = float_3p4_8041e20c;
			f32 w = float_neg0p2_8041e208;
#endif
			*(f32*)(member->field_0xC0 + 0x1c) = v;
			*(f32*)(member->field_0xC0 + 0x24) = w;
		}
		member->field_0x4++;
		// fallthrough
	case 2:
		if (float_0_8041e178 == *(f32*)((u8*)member + 0xe4)) {
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->field_0x4 = 1;
			}
		}
		break;
	}
}

#pragma dont_inline on // recursive call stays a real `bl` in the original
void BattleAudienceCtrlProcessKinopioJoyRaiseArm(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 0) {
		BattleAudience_SetAnim(index, 4, 0);
	}
	switch (member->field_0x4) {
	case 0:
		if (irand(2) == 0) {
			member->field_0xE8 = float_270_8041e170;
		}
		else {
			member->field_0xE8 = float_90_8041e174;
		}
		member->field_0x4++;
		// fallthrough
	case 1:
		*(f32*)((u8*)member + 0x70) = intplGetValue(4, ((volatile f32)float_0_8041e178, (volatile f32)float_10_8041e1b4, *(s32*)member->field_0x8), 0xf,
		    float_0_8041e178, float_10_8041e1b4);
		(*(s32*)member->field_0x8)++;
		if (*(s32*)member->field_0x8 > 0x1e) {
			f32 one = 1.0f;
			f32 zero = 0.0f;
			*(f32*)(member->field_0x8 + 4) = *(f32*)(member->field_0x8 + 4) - one;
			if (*(f32*)(member->field_0x8 + 4) <= zero) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				*(s32*)member->field_0x8 = 0;
			}
		}
		break;
	}
}
#pragma dont_inline reset

void BattleAudienceCtrlProcessKinopioJoyJump(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 2, 0);
	}
	else {
		BattleAudience_SetAnim(index, 3, 0);
	}
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = 1;
		member->field_0x4++;
		// fallthrough
	case 1:
		*(f32*)((u8*)member + 0xdc) =
		    3.4f + (f32)rand() / 32767.0f;
		*(f32*)((u8*)member + 0xe4) = -0.2f;
		member->field_0x4++;
		// fallthrough
	case 2:
		if (float_0_8041e178 == *(f32*)((u8*)member + 0xe4)) {
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->field_0x4 = 1;
			}
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioJoyJump2(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 2, 0);
	}
	else {
		BattleAudience_SetAnim(index, 3, 0);
	}
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = 2;
		member->field_0x4++;
		// fallthrough
	case 1: {
		f32 neg = -0.2f;
		*(f32*)((u8*)member + 0xdc) = 4.0f + (f32)rand() / 32767.0f;
		*(f32*)((u8*)member + 0xe4) = neg;
		member->field_0x4++;
	}
		// fallthrough
	case 2:
	{
		f32 zero = 0.0f;
		member->mRotation.y += 20.0f;
		if (zero == *(f32*)((u8*)member + 0xe4)) {
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->field_0x4 = 1;
			}
		}
		break;
	}
	}
}

void BattleAudienceCtrlProcessKinopioItemOn(s32 index, s32 kind) {
	BattleWork* work = _battleWorkPointer;
	BattleAudience* audience;
	BattleAudienceMember* member;
	BattleAudienceItem* item;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0, 0);
	}
	else {
		BattleAudience_SetAnim(index, 5, 0);
	}
	if (member->flags & 0x40) {
		item = BattleAudienceItemGetPtr(member->itemId);
		member->flags &= ~0x60;
		member->itemId = -1;
		item->flags = 0;
		BattleAudience_ChangeStatus(index, 0xa);
		if (BattleAudienceItemGetPtr(0)->mItemType < 0xec ||
		    BattleAudienceItemGetPtr(0)->mItemType >= 0xf0) {
			BattleAudienceAddTargetNum(
			    float_neg0p15_8041e200 * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			    float_0_8041e178);
		}
	}
	else if (!(member->flags & 0x20) && (work->flags & 0x80)) {
		item = BattleAudienceItemGetPtr(member->itemId);
		if (!(audience->flags & 0x40000)) {
			(*(s32*)((u8*)item + 0x8))--;
			if ((item->flags & 1) && *(s32*)((u8*)item + 0x8) <= 0 && item->state == 1) {
				BattleAudience_ChangeStatus(index, 8);
				BattleAudienceSoundItemThrow(0, index);
			}
		}
	}
}

void BattleAudienceCtrlProcessKinopioItemThrow(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	BattleAudience_SetAnim(index, 6, 0);
	if (BattleAudience_GetAnimEnd(index) == 1) {
		BattleAudience_ChangeStatus(index, 0);
	}
	if (member->flags & 0x40) {
		member->flags &= ~0x60;
		BattleAudience_ChangeStatus(index, 0xa);
		if (BattleAudienceItemGetPtr(0)->mItemType < 0xec ||
		    BattleAudienceItemGetPtr(0)->mItemType >= 0xf0) {
			BattleAudienceAddTargetNum(
			    float_neg0p15_8041e200 * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			    float_0_8041e178);
		}
	}
	else if (member->flags & 0x20) {
		return;
	}
}

void BattleAudienceCtrlProcessKinopioBooing(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	BattleAudience_SetAnim(index, 4, 0);
	switch (member->field_0x4) {
	case 0:
		if (irand(2) == 0) {
			member->field_0xE8 = float_270_8041e170;
		}
		else {
			member->field_0xE8 = float_90_8041e174;
		}
		member->field_0x4++;
		// fallthrough
	case 1:
		*(f32*)((u8*)member + 0x70) = intplGetValue(4, ((volatile f32)float_0_8041e178, (volatile f32)float_10_8041e1b4, *(s32*)member->field_0x8), 0xf,
		    float_0_8041e178, float_10_8041e1b4);
		(*(s32*)member->field_0x8)++;
		if (*(s32*)member->field_0x8 > 0x1e) {
			f32 one = 1.0f;
			f32 zero = 0.0f;
			*(f32*)(member->field_0x8 + 4) = *(f32*)(member->field_0x8 + 4) - one;
			if (*(f32*)(member->field_0x8 + 4) <= zero) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				*(s32*)member->field_0x8 = 0;
			}
		}
		break;
	}
}

#pragma opt_propagation off // keep the constant loads after the anim call in the original's order
void BattleAudienceCtrlProcessKinopioDamage(s32 index) {
	BattleAudienceMember* member;
#ifdef MUST_MATCH
	f32 v;
	f32 w;
	f32 a;
#endif

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	switch (member->field_0x4) {
	case 0:
		BattleAudience_SetAnim(index, 9, 0);
#ifdef MUST_MATCH
		v = float_8_8041e1f4;
		w = float_neg1_8041e18c;
		*(f32*)((u8*)member + 0xdc) = v;
		a = float_270_8041e170;
		*(f32*)((u8*)member + 0xe4) = w;
		*(s32*)((u8*)member + 0x30) = 0;
		member->field_0xE8 = a;
#else
		*(f32*)((u8*)member + 0xdc) = float_8_8041e1f4;
		*(f32*)((u8*)member + 0xe4) = float_neg1_8041e18c;
		*(s32*)((u8*)member + 0x30) = 0;
		member->field_0xE8 = float_270_8041e170;
#endif
		member->field_0x4++;
		// fallthrough
	case 1:
		if (*(s32*)((u8*)member + 0x30) >= 8) {
			*(f32*)((u8*)member + 0x64) = float_1_8041e188;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if (*(s32*)((u8*)member + 0x30) <= 4) {
			*(f32*)((u8*)member + 0x64) -= float_0p1_8041e194;
		}
		else {
			*(f32*)((u8*)member + 0x64) += float_0p1_8041e194;
		}
		(*(s32*)((u8*)member + 0x30))++;
		if (BattleAudience_GetAnimEnd(index) == 1) {
			*(f32*)((u8*)member + 0x64) = float_1_8041e188;
			*(s32*)((u8*)member + 0x30) = 0;
			if ((u8)BattleAudience_GetEscapeChangeOK(index) == 1) {
				BattleAudience_ChangeStatus(index, 0xc);
				psndSFXOn_3D(str_802F8DE8, &member->position);
			}
			else {
				BattleAudience_ChangeStatus(index, 0);
			}
		}
		break;
	}
}
#pragma opt_propagation reset

void BattleAudienceCtrlProcessKinopioEnter(s32 index) {
	BattleAudienceMember* member;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	camGetPtr(8);
	BattleAudience_SetAnim(index, 7, 0);
	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		switch (member->field_0x4) {
		case 0:
		{
			f32 zero = 0.0f;
			member->position.x = member->mHomePosition.x;
			member->position.y = -100.0f;
			member->position.z = member->mHomePosition.z;
			*(f32*)((u8*)member + 0x78) = zero;
			*(f32*)((u8*)member + 0x7c) = zero;
			*(f32*)((u8*)member + 0x80) = zero;
			{
				s32 t = irand(0x3c);
				t = t + *(s32*)member->field_0x8;
				*(s32*)member->field_0x8 = t + 1;
			}
			member->field_0x4++;
			break;
		}
		case 1:
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				f32 one = 1.0f;
				f32 zero = 0.0f;
				*(f32*)((u8*)member + 0x78) = zero;
				*(f32*)((u8*)member + 0x7c) = one;
				*(f32*)((u8*)member + 0x80) = zero;
				member->field_0x4++;
			}
			break;
		case 2:
			if (member->position.y >= member->mHomePosition.y) {
				member->position.y = member->mHomePosition.y;
				BattleAudience_ChangeStatus(index, 0);
				member->field_0x4++;
			}
			break;
		}
	}
	else {
		switch (member->field_0x4) {
		case 0:
			member->position.x = (f32)(irand(0x190) - 0xc8);
		{
			f32 thousand = 1000.0f;
			member->position.y = member->mHomePosition.y;
			member->position.z = thousand;
			{
				f32 zero = 0.0f;
				*(f32*)((u8*)member + 0x78) = zero;
				*(f32*)((u8*)member + 0x7c) = zero;
				*(f32*)((u8*)member + 0x80) = zero;
			}
			{
				s32 t = irand(0x3c);
				t = t + *(s32*)member->field_0x8;
				*(s32*)member->field_0x8 = t + 1;
			}
			member->field_0x4++;
			break;
		}
		case 1:
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				f32 dx = member->mHomePosition.x - member->position.x;
				f32 dz = member->mHomePosition.z - member->position.z;
				f32 len = (f32)sqrt(dx * dx + dz * dz);
				f32 zero = 0.0f;
				f32 ten = 10.0f;
				*(f32*)((u8*)member + 0x78) = ten * (dx / len);
				*(f32*)((u8*)member + 0x7c) = zero;
				*(f32*)((u8*)member + 0x80) = ten * (dz / len);
				*(f32*)((u8*)member + 0xc) = ten;
				*(f32*)((u8*)member + 0x10) = ten;
				*(f32*)((u8*)member + 0x14) = len;
				member->field_0x4++;
			}
			break;
		case 2:
			*(f32*)((u8*)member + 0xc) += *(f32*)((u8*)member + 0x10);
			if (*(f32*)((u8*)member + 0xc) >= *(f32*)((u8*)member + 0x14)) {
				member->position.x = member->mHomePosition.x;
				member->position.y = member->mHomePosition.y;
				member->position.z = member->mHomePosition.z;
				BattleAudience_ChangeStatus(index, 0);
			}
			break;
		}
	}
}

void BattleAudienceCtrlProcessKinopioEscape(s32 index) {
	BattleAudienceMember* member;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	BattleAudience_SetAnim(index, 7, 0);
	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		switch (member->field_0x4) {
		case 0:
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				*(f32*)((u8*)member + 0x78) = 0.0f;
				*(f32*)((u8*)member + 0x7c) = -1.0f;
				*(f32*)((u8*)member + 0x80) = 0.0f;
				member->field_0x4++;
			}
			break;
		case 1:
			if (member->position.y <= float_neg100_8041e190) {
				BattleAudience_Delete(index);
				member->field_0x4++;
			}
			break;
		}
	}
	else {
		switch (member->field_0x4) {
		case 0:
			(*(s32*)member->field_0x8)--;
			if (*(s32*)member->field_0x8 <= 0) {
				f32 v = ((2.0f * (f32)rand()) / 32767.0f -
				            1.0f) -
				        (member->position.x / 120.0f);
				{
					f32 zero = 0.0f;
					f32 eight = 8.0f;
					f32 six = 6.0f;
					f32 neg0p5 = -0.5f;
					*(f32*)((u8*)member + 0x78) = v;
					*(f32*)((u8*)member + 0x7c) = zero;
					*(f32*)((u8*)member + 0x80) = eight;
					*(f32*)((u8*)member + 0xdc) = six;
					*(f32*)((u8*)member + 0xe4) = neg0p5;
				}
				member->field_0x4++;
				psndSFXOn_3D(str_802F8DD4, &member->position);
			}
			break;
		case 1:
			if (float_0_8041e178 == *(f32*)((u8*)member + 0xe4)) {
				member->field_0x4++;
			}
			break;
		case 2:
			if (member->position.z >= float_600_8041e1f8) {
				BattleAudience_Delete(index);
			}
			break;
		}
	}
}

void BattleAudienceCtrlProcessKinopioSleep(s32 index, s32 kind) {
	BattleAudienceMember* member;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->field_0x4 <= 1) {
		if ((u8)kind == 1) {
			BattleAudience_SetAnim(index, 0xa, 0);
		}
		else {
			BattleAudience_SetAnim(index, 0xb, 0);
		}
		if (*(s32*)((u8*)member + 0x30) >= 0x140) {
			*(f32*)((u8*)member + 0x64) = float_1_8041e188;
			*(s32*)((u8*)member + 0x30) = 0;
		}
		if (*(s32*)((u8*)member + 0x30) <= 0xa0) {
			*(f32*)((u8*)member + 0x64) -= float_0p0025_8041e1dc;
		}
		else {
			*(f32*)((u8*)member + 0x64) += float_0p0025_8041e1dc;
		}
		(*(s32*)((u8*)member + 0x30))++;
		*(u8*)((u8*)member + 0x43) = *(u8*)((u8*)member + 0x43) + *(u8*)((u8*)member + 0x18);
		if (*(u8*)((u8*)member + 0x43) >= 0xc8) {
			*(s8*)((u8*)member + 0x18) = -4;
		}
		else if (*(u8*)((u8*)member + 0x43) <= 5) {
			*(u8*)((u8*)member + 0x18) = 4;
		}
	}
	switch (member->field_0x4) {
	case 0:
		BattleAudienceSoundSleep(index);
		member->field_0x4++;
		// fallthrough
	case 1:
		if (irand(0x8ca0) == 0) {
			effSleepEntry(0, 0xb4, member->position.x, float_25_8041e1ac + member->position.y,
			    member->position.z, float_1_8041e188, float_45_8041e1e0);
			BattleAudienceSoundZZZ(index);
		}
		break;
	case 2:
		*(f32*)((u8*)member + 0xdc) =
		    6.0f + (f32)rand() / 32767.0f;
		*(f32*)((u8*)member + 0xe4) = -0.5f;
		if ((u8)kind == 1) {
			BattleAudience_SetAnim(index, 0, 0);
		}
		else {
			BattleAudience_SetAnim(index, 1, 0);
		}
		*(f32*)((u8*)member + 0x64) = float_1_8041e188;
		*(u8*)((u8*)member + 0x43) = 0;
		member->field_0x4++;
		// fallthrough
	case 3:
		if (float_0_8041e178 == *(f32*)((u8*)member + 0xe4)) {
			BattleAudience_ChangeStatus(index, 0);
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioTransEvt(s32 index) {
	BattleWork* work;
	BattleAudienceMember* member;
	BattleUnit* unit;
	BattleUnitPart* part;
	s8 turns;
	s8 strength;

	work = _battleWorkPointer;
	member = BattleAudienceGetPtr(index);
	switch (member->field_0x4) {
	case 0:
		BattleAudience_SetAnim(index, 1, 0);
		*(BattleUnit**)((u8*)member + 0xec) = BattleAudienceDetectTargetAll();
		*(s32*)member->field_0x8 = 0;
		psndSFXOn_3D(str_802F8D98, &member->position);
		member->field_0x4 = 5;
		// fallthrough
	case 5:
		unit = *(BattleUnit**)((u8*)member + 0xec);
		part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
		(*(s32*)member->field_0x8)++;
		member->position.x = intplGetValue(5, *(s32*)member->field_0x8, 0x3c,
		    member->mHomePosition.x, 0.0f);
		member->position.y = intplGetValue(5, *(s32*)member->field_0x8, 0x3c,
		    member->mHomePosition.y, 80.0f);
		member->position.z = intplGetValue(5, *(s32*)member->field_0x8, 0x3c,
		    member->mHomePosition.z, 100.0f);
		if (*(s32*)member->field_0x8 >= 0x3c) {
			BattleAudience_SetAnim(index, 7, 0);
			BtlUnit_GetHitPos(unit, part, &x_873, &y_874, &z_875);
			if (member->position.x >= x_873) {
				member->field_0xE8 = float_270_8041e170;
			}
			else {
				member->field_0xE8 = float_90_8041e174;
			}
			member->field_0x4 = 0xa;
			*(s32*)member->field_0x8 = 0;
			unit = *(BattleUnit**)((u8*)member + 0xec);
			part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
			BtlUnit_GetHitPos(unit, part, &x_873, &y_874, &z_875);
			psndSFXOn_3D(str_802F8D98, &member->position);
		}
		break;
	case 0xa:
		(*(s32*)member->field_0x8)++;
		member->position.x =
		    intplGetValue(0, *(s32*)member->field_0x8, 0x1e, 0.0f, x_873);
		member->position.y = intplGetValue(4, *(s32*)member->field_0x8, 0x1e, 80.0f,
		    y_874 - 15.0f);
		member->position.z =
		    intplGetValue(0, *(s32*)member->field_0x8, 0x1e, 100.0f, z_875);
		if (*(s32*)member->field_0x8 == 0x1e) {
			unit = *(BattleUnit**)((u8*)member + 0xec);
			part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
			BtlUnit_GetStatus(unit, 0x12, &turns, &strength);
			if (turns < 1 && !BtlUnit_CheckShadowGuard(unit)) {
				BattleCheckDamage(BattleGetSystemPtr(work), unit, part,
				    (BattleWeapon*)weapon_teresa_trans, 0x100);
				BtlUnit_snd_se(unit, str_802F8DB4, -250000000, 0);
			}
		}
		if (*(s32*)member->field_0x8 >= 0x78) {
			BattleAudience_ChangeStatus(index, 0xb);
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioIntrude(s32 index) {
	BattleWork* work = _battleWorkPointer;
	PouchData* pouch;
	BattleAudienceMember* member;
	f32 dir;
	f32 dx;
	f32 t;
	f32 dz;
	f32 len;
	f32 vx;
	f32 vz;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	pouch = pouchGetPtr();
	BattleAudience_SetAnim(index, 7, 0);
	switch (member->field_0x4) {
	case 0:
		if (irand(2) == 0) {
			*(f32*)(member->field_0x8 + 4) = 1.0f;
		}
		else {
			*(f32*)(member->field_0x8 + 4) = -1.0f;
		}
		if (pouch->rank == 0) {
			member->field_0x4 = 3;
			break;
		}
		if (irand(2) == 0) {
			member->field_0x4 = 3;
			break;
		}
		member->field_0x4 = 0x32;
		break;
	case 3: {
		f32 dz;
		f32 dx;
		member->field_0x4 = 5;
		dir = *(f32*)(member->field_0x8 + 4);
		t = 25.0f * dir;
		dx = t - member->position.x;
		dz = 110.0f - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		*(f32*)(member->field_0x8 + 0x70) = vx;
		*(f32*)(member->field_0x8 + 0x74) = 0.0f;
		*(f32*)(member->field_0x8 + 0x80) = 0.0f;
		*(f32*)(member->field_0x8 + 0x78) = vz;
		intrudeSound = psndSFXOn(str_802F8D68);
	}
		// fallthrough
	case 5:
		if (member->position.z <= 110.0f) {
			member->field_0x4 = 0xa;
			member->position.z = 110.0f;
			*(f32*)(member->field_0x8 + 0x70) = 0.0f;
			*(f32*)(member->field_0x8 + 0x74) = 10.0f;
			*(f32*)(member->field_0x8 + 0x80) = -0.7f;
			*(f32*)(member->field_0x8 + 0x78) = -1.5f;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0xa: {
		f32 zero = 0.0f;
		if (*(f32*)(member->field_0x8 + 0x74) <= zero &&
		    member->position.y <= zero) {
			f32 dz;
			f32 dx;
			member->field_0x4 = 0xf;
			member->position.y = zero;
			*(f32*)(member->field_0x8 + 0x74) = zero;
			*(f32*)(member->field_0x8 + 0x80) = zero;
			dir = *(f32*)(member->field_0x8 + 4);
			t = 300.0f * dir;
			dx = t - member->position.x;
			dz = zero - member->position.z;
			len = (f32)sqrt(dx * dx + dz * dz);
			vx = 5.0f * (dx / len);
			vz = 5.0f * (dz / len);
			*(f32*)(member->field_0x8 + 0x70) = vx;
			*(f32*)(member->field_0x8 + 0x74) = zero;
			*(f32*)(member->field_0x8 + 0x78) = vz;
			btl_camera_set_mode(0, 3);
			btl_camera_set_moveto(0, 0x1e, 4, 698.0f * *(f32*)(member->field_0x8 + 4),
			    100.0f, zero, zero,
			    60.0f, zero);
			intrudeSound = psndSFXOn(str_802F8D68);
		}
		break;
	}
	case 0xf:
		member->mRotation.y = *(f32*)((u8*)camGetPtr(4) + 0x114);
		if (__fabs(member->position.x) >= *(volatile f64*)&double_450_802f8d88) {
			member->field_0x4 = 0x10;
			*(s32*)member->field_0x8 = 0x23;
			*(Vec*)(member->field_0x8 + 0x70) = vec3_802f8c84;
		}
		break;
	case 0x10:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4 = 0x12;
			*(s32*)member->field_0x8 = 0x19;
			btl_camera_set_mode(0, 0);
			btl_camera_set_moveSpeedLv(0, 2);
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x12:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4 = 0x13;
			*(s32*)member->field_0x8 = 0x3c;
			btl_camera_set_moveSpeedLv(0, 1);
			effSmallStarEntry(0, 4, 185.0f * *(f32*)(member->field_0x8 + 4),
			    20.0f, 40.0f, 0.0f,
			    -1.0f, 0.0f);
		}
		break;
	case 0x13:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4 = 0x14;
			btl_camera_set_moveSpeedLv(0, 1);
		}
		// fallthrough
	case 0x14:
		member->field_0x12C = BattleStage_ObjectFallCheck();
		member->field_0x4 = 0x19;
		break;
	case 0x19:
		if (!evtCheckID(member->field_0x12C)) {
			member->field_0x4 = 0x1e;
			member->mRotation.y = 0.0f;
		}
		break;
	case 0x1e:
		BattleAudience_ChangeStatus(index, 0xb);
		break;
	case 0x32: {
		f32 dz;
		f32 dx;
		member->field_0x4 = 0x37;
		dir = *(f32*)(member->field_0x8 + 4);
		t = 25.0f * dir;
		dx = t - member->position.x;
		dz = 110.0f - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		*(f32*)(member->field_0x8 + 0x70) = vx;
		*(f32*)(member->field_0x8 + 0x74) = 0.0f;
		*(f32*)(member->field_0x8 + 0x80) = 0.0f;
		*(f32*)(member->field_0x8 + 0x78) = vz;
		intrudeSound = psndSFXOn(str_802F8D68);
	}
		// fallthrough
	case 0x37:
		if (member->position.z <= 110.0f) {
			member->field_0x4 = 0x3c;
			member->position.z = 110.0f;
			*(f32*)(member->field_0x8 + 0x70) = 0.0f;
			*(f32*)(member->field_0x8 + 0x74) = 10.0f;
			*(f32*)(member->field_0x8 + 0x80) = -0.7f;
			*(f32*)(member->field_0x8 + 0x78) = -1.5f;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x3c:
		if (*(f32*)(member->field_0x8 + 0x74) <= 0.0f &&
		    member->position.y <= 0.0f) {
			f32 zero = 0.0f;
			member->field_0x4 = 0x41;
			*(s32*)member->field_0x8 = 0;
			member->position.y = zero;
			*(f32*)(member->field_0x8 + 0x70) = zero;
			*(f32*)(member->field_0x8 + 0x74) = zero;
			*(f32*)(member->field_0x8 + 0x80) = zero;
			*(f32*)(member->field_0x8 + 0x78) = zero;
			intrudeSound = psndSFXOn(str_802F8D68);
		}
		break;
	case 0x41:
	case 0x42:
	case 0x43: {
		f32 one = 1.0f;
		(*(s32*)member->field_0x8)++;
		if (one == *(f32*)(member->field_0x8 + 4)) {
			member->field_0xE8 = 270.0f;
		}
		if (-1.0f == *(f32*)(member->field_0x8 + 4)) {
			member->field_0xE8 = 90.0f;
		}
		if (*(s32*)member->field_0x8 <= 5) {
			member->position.x = member->position.x -
			                     5.0f * *(f32*)(member->field_0x8 + 4);
		}
		else if (*(s32*)member->field_0x8 <= 0xa) {
			member->position.x = member->position.x +
			                     5.0f * *(f32*)(member->field_0x8 + 4);
		}
		if (*(s32*)member->field_0x8 == 0xa) {
			if (member->field_0x4 == 0x43) {
				member->field_0x4 = 0x46;
			}
			else {
				member->field_0x4 = member->field_0x4 + 1;
			}
			*(s32*)member->field_0x8 = 0;
		}
	}
		break;
	case 0x46: {
		f32 zero = 0.0f;
		f32 dz;
		f32 dx;
		BattleStage_NozzleWorkCheck(1);
		member->field_0x12C = *(s32*)((u8*)work + 0x18298);
		member->field_0x4 = 0x4b;
		dir = *(f32*)(member->field_0x8 + 4);
		t = 300.0f * dir;
		dx = t - member->position.x;
		dz = zero - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		*(f32*)(member->field_0x8 + 0x70) = vx;
		*(f32*)(member->field_0x8 + 0x74) = zero;
		*(f32*)(member->field_0x8 + 0x78) = vz;
	}
		break;
	case 0x4b:
		if (__fabs(member->position.x) >= *(volatile f64*)&double_300_802f8d90) {
			member->field_0x4 = 0x50;
			*(Vec*)(member->field_0x8 + 0x70) = vec3_802f8c90;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x50:
		if (!evtCheckID(member->field_0x12C)) {
			member->field_0x4 = 0x55;
		}
		break;
	case 0x55:
		BattleAudience_ChangeStatus(index, 0xb);
		break;
	}
}


void BattleAudienceCtrlProcessKinopioSing(s32 index) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	s32 i;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = 0x1e;
		member->field_0x4++;
		// fallthrough
	case 1:
		(*(s32*)member->field_0x8)--;
		member->mRotation.y += 6.0f;
		if (*(s32*)member->field_0x8 == 0xf) {
			BattleAudience_SetAnim(index, 0xe, 0);
			BattleAudienceSoundSing(index);
		}
		if (*(s32*)member->field_0x8 <= 0) {
			*(s32*)member->field_0x8 = 0x78;
			member->field_0x4++;
		}
		break;
	case 2:
		(*(s32*)member->field_0x8)--;
		if (gp->retraceCount % 0xa == 0) {
			f32 y;
			f32 z;
			f32 x;
			z = member->position.z + (f32)irand(0xa) - float_5_8041e1a0;
			y = member->position.y + (f32)irand(0xa);
			x = member->position.x + (f32)irand(0xc8) - float_100_8041e1a8;
			effOnpuN64Entry(0, x, y, z);
		}
		if (*(s32*)member->field_0x8 <= 0) {
			for (i = 0; i < audience->mMaxAudience; i++) {
				if (index != i) {
					BattleAudienceCtrlProcessKinopioSingSub(i);
				}
			}
			*(s32*)member->field_0x8 = 0x1e;
			member->field_0x4++;
		}
		break;
	case 3:
		(*(s32*)member->field_0x8)--;
		member->mRotation.y += 6.0f;
		if (*(s32*)member->field_0x8 <= 0xf) {
			BattleAudience_SetAnim(index, 1, 0);
		}
		if (*(s32*)member->field_0x8 <= 0) {
			BattleAudience_ChangeStatus(index, 0);
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioSingSub(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if (member->flags & 0x100) {
		BattleAudienceCtrlProcessKinopioSingSub(index - 1);
	}
	else if (member->status != 0x11) {
		BattleAudience_ChangeStatus(index, 0xd);
		*(s32*)((u8*)member + 0xf4) = 3;
	}
}

void BattleAudienceCtrlProcessKinopioShell(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if (member->field_0x4 >= 2 && member->field_0x4 <= 5) {
		BattleAudience_SetAnim(index, 0xf, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = irand(0x1e);
		member->field_0x4++;
		// fallthrough
	case 1:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4++;
		}
		break;
	case 2:
		{
#ifdef MUST_MATCH
			// literals so MWCC pools them and assigns f1/f0 like the original; the
			// named globals make it load them into the opposite registers
			f32 v = 5.0f;
			f32 w = -0.3f;
#else
			f32 v = float_5_8041e1a0;
			f32 w = float_neg0p3_8041e1a4;
#endif
			*(f32*)(member->field_0xC0 + 0x1c) = v;
			*(f32*)(member->field_0xC0 + 0x24) = w;
		}
		BattleAudienceSoundShell(index);
		member->field_0x4++;
		break;
	case 4:
		*(s32*)member->field_0x8 = irand(0x1e);
		member->field_0x4++;
		// fallthrough
	case 5:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4++;
		}
		break;
	case 6:
		{
#ifdef MUST_MATCH
			// literals so MWCC pools them and assigns f1/f0 like the original; the
			// named globals make it load them into the opposite registers
			f32 v = 5.0f;
			f32 w = -0.3f;
#else
			f32 v = float_5_8041e1a0;
			f32 w = float_neg0p3_8041e1a4;
#endif
			*(f32*)(member->field_0xC0 + 0x1c) = v;
			*(f32*)(member->field_0xC0 + 0x24) = w;
		}
		*(s32*)member->field_0x8 = 0xa;
		BattleAudienceSoundShell(index);
		member->field_0x4++;
		break;
	case 7:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			BattleAudience_ChangeStatus(index, 0);
		}
		member->field_0x12C = 0;
		break;
	}
}

void BattleAudienceCtrlProcessKinopioIgnite(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0x10, 0);
	}
	else {
		BattleAudience_SetAnim(index, 0x11, 0);
	}
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = 0x1e;
		BattleAudienceSoundBombIgnite(index, 0xb4, 0x78);
		member->field_0x4++;
		// fallthrough
	case 1:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			*(s32*)member->field_0x8 = 0xa;
			member->field_0x4++;
		}
		break;
	case 2:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4++;
		}
		break;
#ifdef MUST_MATCH
	case 3: // empty case only so MWCC picks the original's compare tree
		break;
#endif
	}
}


void BattleAudienceCtrlProcessKinopioFire(s32 index, s32 kind) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	s32 offsets[24][2];
	s32 i;

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0x10, 0);
	}
	else {
		BattleAudience_SetAnim(index, 0x11, 0);
	}
	switch (member->field_0x4) {
	case 0:
		*(s32*)member->field_0x8 = 0xa;
		member->field_0x4++;
		// fallthrough
	case 1:
#ifdef MUST_MATCH
		{
			f32 v = *(f32*)((u8*)member + 0x60);
			f32 step = float_0p05_8041e198;
			*(f32*)((u8*)member + 0x60) = v + step;
			v = *(f32*)((u8*)member + 0x64);
			*(f32*)((u8*)member + 0x64) = v + step;
		}
#else
		*(f32*)((u8*)member + 0x60) += float_0p05_8041e198;
		*(f32*)((u8*)member + 0x64) += float_0p05_8041e198;
#endif
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
#ifdef MUST_MATCH
			{
				typedef struct { s32 d[24][2]; } Tbl;
				*(Tbl*)offsets = *(const Tbl*)dat_802f8c9c;
			}
#else
			memcpy(offsets, dat_802f8c9c, sizeof(offsets));
#endif
			BattleAudienceSoundBombFire(index);
			member->flags &= ~0x4;
			if (index < 0x3c) {
				effSandarsEntry(4, member->position.x, float_35_8041e19c + member->position.y,
				    member->position.z, float_0_8041e178);
			}
			btl_camera_shake_h(0, 0x1e, 0, float_5_8041e1a0, float_0_8041e178);
			for (i = 0; i < 0x18; i++) {
				s32 no = BattleAudience_GetAudienceNoFromOffset(
				    index, offsets[i][0], offsets[i][1]);
				if (no != -1) {
					BattleAudienceCtrlProcessKinopioFireSub(no);
				}
			}
			BattleAudiencePuniAllEscape();
			*(s32*)member->field_0x8 = 0x3c;
			member->field_0x4++;
		}
		break;
	case 2:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			member->field_0x4++;
		}
		break;
	case 3:
		BattleAudience_Delete(index);
		audience->mTargetAudienceCount -= float_1_8041e188;
		break;
	}
}

void BattleAudienceCtrlProcessKinopioFireSub(s32 index) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if (member->flags & 0x100) {
		BattleAudienceCtrlProcessKinopioFireSub(index - 1);
		return;
	}
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return;
	}
	if (member->type == AUDIENCE_BULKY_BOBOMB) {
		if (member->status != 0x13) {
			BattleAudience_ChangeStatus(index, 0x13);
			return;
		}
		return;
	}
	if (member->status != 0x11) {
		BattleAudience_Delete(index);
		audience->mTargetAudienceCount -= float_1_8041e188;
	}
}


// NONMATCHING (98.25%): all constants are literals. Case 9 is the only thing left and it is pure
// register colouring the compiler fixes regardless of source shape: target id=r28, col=r5,
// 0x14=r6, 0x7fff=r7, magic=f4, 0.1=f2, row=f1, x=f3; ours r27/r6/r7/r5 and f1/f3/f2/f4.
void BattleAudienceCtrlProcessKinopioEat(s32 index) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	BattleStage* stage = BattleStageGetPtr();
	BattleAudienceMember* target;
	s32 candidates[60];
	s32 found;
	s32 i;
	s32 count;

	switch (member->field_0x4) {
	case 0:
		found = BattleAudienceDetectPakkunEatTarget(index);
		if (found != -1) {
			*(s16*)(member->field_0xEC + 4) = found;
			target = BattleAudienceGetPtr(*(s16*)(member->field_0xEC + 4));
			if (member->position.x >= target->position.x) {
				member->field_0xE8 = 270.0f;
			}
			else {
				member->field_0xE8 = 90.0f;
			}
			BattleAudience_SetAnim(index, 0, 0);
		}
		else {
			BattleAudience_ChangeStatus(index, 0);
			break;
		}
		*(s32*)member->field_0x8 = 0x1e;
		member->field_0x4++;
		// fallthrough
	case 1:
		target = BattleAudienceGetPtr(*(s16*)(member->field_0xEC + 4));
		if (*(s32*)member->field_0x8 > 0) {
			(*(s32*)member->field_0x8)--;
		}
		if (*(s32*)member->field_0x8 <= 0 && 0.0f == *(f32*)((u8*)target + 0xd0)) {
			BattleAudience_SetAnim(index, 0x12, 0);
			target->flags |= 2;
			member->field_0x4 = 3;
		}
		break;
	case 3:
		target = BattleAudienceGetPtr(*(s16*)(member->field_0xEC + 4));
		if (member->position.x >= target->position.x) {
			*(f32*)((u8*)member + 0x6c) -= 6.0f;
		}
		else {
			*(f32*)((u8*)member + 0x6c) += 6.0f;
		}
		if (*(f32*)((u8*)member + 0x6c) <= -60.0f ||
		    *(f32*)((u8*)member + 0x6c) >= 60.0f) {
			member->field_0x4++;
		}
		break;
	case 4:
		target = BattleAudienceGetPtr(*(s16*)(member->field_0xEC + 4));
		target->flags = 0;
		audience->mTargetAudienceCount -= 1.0f;
		BattleAudienceSoundPakkunEat(index);
		member->field_0x4++;
		break;
	case 5: {
		f32 zero = 0.0f;
		if (*(f32*)((u8*)member + 0x6c) > zero) {
			*(f32*)((u8*)member + 0x6c) -= 6.0f;
			if (*(f32*)((u8*)member + 0x6c) <= zero) {
				member->field_0x4 = 7;
			}
		}
		else {
			*(f32*)((u8*)member + 0x6c) += 6.0f;
			if (*(f32*)((u8*)member + 0x6c) >= zero) {
				member->field_0x4 = 7;
			}
		}
		break;
	}
	case 7:
		found = BattleAudienceDetectPakkunEatTarget(index);
		if (found != -1) {
			member->field_0x4 = 0x63;
			*(s32*)member->field_0x8 = 0xa;
			break;
		}
		count = 0;
		for (i = 0; i <= 0x3b; i++) {
			if (!(u8)BattleAudience_GetExist(i) && !(BattleAudienceGetPtr(i)->flags & 0x100) &&
			    BattleAudienceDetectPakkunEatTarget(i) != -1) {
				candidates[count++] = i;
			}
		}
		if (count >= 1) {
			member->field_0x4 = 8;
			member->field_0x12C = candidates[irand(count)];
		}
		else if (i == 0x3c) {
			member->field_0x4 = 0x63;
			*(s32*)member->field_0x8 = 0xa;
		}
		break;
	case 8: {
		f32 zero = 0.0f;
		f32 neg1 = -1.0f;
		*(f32*)((u8*)member + 0x78) = zero;
		*(f32*)((u8*)member + 0x7c) = neg1;
		*(f32*)((u8*)member + 0x80) = zero;
		member->field_0x4++;
		break;
	}
	case 9:
		if (member->position.y <= -100.0f) {
			s32 id = member->field_0x12C;
			s32 r = rand();
			s32 col = id / 0x14;
			s32 row = id % 0x14;
			s32 xi = row * 0x19 - 0xeb;
			f32 x = (f32)(xi + (r * 0xa) / 0x7fff);

			f32 z = 0.1f * (f32)row + (f32)(col * 0x1e + 0x73);
			member->position.x = x;
			member->mHomePosition.x = x;
			member->mHomePosition.y = *(f32*)((u8*)stage + 0xb34);
			member->position.z = z;
			member->mHomePosition.z = z;
			member->field_0x4++;
		}
		break;
	case 0xa: {
		f32 zero = 0.0f;
		f32 one = 1.0f;
		*(f32*)((u8*)member + 0x78) = zero;
		*(f32*)((u8*)member + 0x7c) = one;
		*(f32*)((u8*)member + 0x80) = zero;
		member->field_0x4++;
		break;
	}
	case 0xb: {
		f32 stageY;
		if (member->position.y >= *(f32*)((u8*)stage + 0xb34)) {
			stageY = *(f32*)((u8*)stage + 0xb34);
			member->mHomePosition.y = stageY;
			member->position.y = stageY;
			*(f32*)((u8*)member + 0x7c) = 0.0f;
			member->field_0x4 = 0x63;
			*(s32*)member->field_0x8 = 0xa;
			target = BattleAudienceGetPtr(member->field_0x12C);
			memcpy(target, member, 0x134);
			BattleAudience_Delete(index);
		}
		break;
	}
	case 0x63:
		(*(s32*)member->field_0x8)--;
		if (*(s32*)member->field_0x8 <= 0) {
			BattleAudience_ChangeStatus(index, 0);
		}
		break;
	}
}
