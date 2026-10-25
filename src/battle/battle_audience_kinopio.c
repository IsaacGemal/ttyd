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

#ifdef MUST_MATCH
// An ordinary, immediately generated helper pools the int->float conversion double ahead of the
// deferred functions' strings; nothing calls it and the linker strips the body.
#pragma defer_codegen off
static f32 audience_pool_i2f(s32 v) {
    return (f32)v;
}
#pragma defer_codegen reset
#endif


// BattleWeapon table used by BattleCheckDamage in the Teresa transform state machine. Original symbol
// weapon_teresa_trans (.data:0x0), local; every pointer field is NULL.
static BattleWeapon weapon_teresa_trans = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    NULL,                                     // hpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00020008,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    100,                                      // invisibleChance
    2,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};
static s32 intrudeSound;

const char str_SFX_AUDIENCE_HEIHO_MOVE1[] = "SFX_AUDIENCE_HEIHO_MOVE1";
const f64 double_450_802f8d88 = 450.0;
const f64 double_300_802f8d90 = 300.0;
const char str_SFX_AUDIENCE_TELESA_MOVE1[] = "SFX_AUDIENCE_TELESA_MOVE1";
const char str_SFX_AUDIENCE_TELESA_INVISIBLE1[] = "SFX_AUDIENCE_TELESA_INVISIBLE1";
const char str_SFX_AUDIENCE_RUN3[] = "SFX_AUDIENCE_RUN3";
const char str_SFX_AUDIENCE_PANIC_SCREAM2[] = "SFX_AUDIENCE_PANIC_SCREAM2";

// 4-byte zero word that ends the unit's .rodata: an all-zero object needs explicit_zero_data to be emitted
// and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".rodata") const u32 gap_04_802F8E04_rodata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset


// The original unit owns 61 float constants in .sdata2 (0x8041E170..0x8041E260). They are written
// as literals in the functions below, which makes MWCC pool them in the original's order and with
// the original's values; naming them as globals changes the compiler's scheduling and register
// choices and drops many functions below 100%.

// Helpers implemented in battle/battle_audience.c and elsewhere (declared locally;
// these should move to a header).


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
	member->position.x += member->velocity.x;
	member->velocity.x += member->accel.x;
	member->position.y += member->velocity.y;
	member->velocity.y += member->accel.y;
	member->position.z += member->velocity.z;
	member->velocity.z += member->accel.z;
	if (member->velocity.x > zero) {
		member->yaw = 90.0f;
	}
	else if (member->velocity.x < zero) {
		member->yaw = 270.0f;
	}
	if (member->jumpOffset.y < zero) {
		member->jumpOffset.y = zero;
		member->jumpVelY = zero;
		member->jumpAccelY = zero;
	}
	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		member->jumpVelY = zero;
		member->jumpOffset.y = zero;
		member->jumpAccelY = zero;
	}
	else {
		member->jumpOffset.y = member->jumpOffset.y + member->jumpVelY;
		member->jumpVelY = member->jumpVelY + member->jumpAccelY;
	}
	}
	if (member->type == AUDIENCE_BOO) {
		f32 step = 0.05f;
		f32 twoPi = 6.283184f;
		f32 angle = member->bobAngle;
		member->bobAngle = angle + step;
		if (member->bobAngle >= twoPi) {
			member->bobAngle = member->bobAngle - twoPi;
		}
		member->bobOffsetY = 5.0f * (f32)sin(member->bobAngle);
		member->dispPos.x = member->position.x + member->jumpOffset.x;
		member->dispPos.y = member->position.y + member->jumpOffset.y + member->bobOffsetY;
		member->dispPos.z = member->position.z + member->jumpOffset.z;
	}
	else {
		member->dispPos.x = member->position.x + member->jumpOffset.x;
		member->dispPos.y = member->position.y + member->jumpOffset.y;
		member->dispPos.z = member->position.z + member->jumpOffset.z;
	}
}

#pragma opt_propagation off // needed: with propagation on MWCC folds t=0x14 into cmpwi and the int-to-float magic into fmadds
#pragma opt_lifetimes off // needed: with lifetimes on the itemId magic colours t/magic/product r0/f1/f2 instead of r27/f2/f0
void BattleAudienceCtrlProcessKinopioWait(s32 index, s32 kind) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	f32 step;
	f32 rangeA;
	f32 rangeB;
	f32 level;
	f32 ratio;
	s32 t;

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
		step = 1.0f;
		level = 1.0f;
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
		if (member->motionTimer > 0xb4) {
			if (member->motionTimer <= 0xbc) {
				member->baseSize.y += 0.005f * step;
			}
			else if ((f32)member->motionTimer <=
			         8.0f * level + 188.0f) {
				member->baseSize.y -= 0.005f * step;
			}
			else if ((f32)member->motionTimer <=
			         16.0f * level + 188.0f) {
				member->baseSize.y += 0.005f * step;
			}
			else if ((f32)member->motionTimer <=
			         24.0f * level + 188.0f) {
				member->baseSize.y -= 0.005f * step;
			}
			else if ((f32)member->motionTimer <=
			         32.0f * level + 188.0f) {
				member->baseSize.y += 0.005f * step;
			}
			else if ((f32)member->motionTimer <=
			         40.0f * level + 188.0f) {
				member->baseSize.y -= 0.005f * step;
			}
			else {
				member->baseSize.y = 1.0f;
				member->motionTimer = 0;
			}
		}
		member->motionTimer++;
		break;
	case 6:
		if ((f32)member->motionTimer >= 36.0f * level) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if ((f32)member->motionTimer <= 17.0f * level) {
			member->baseSize.y += 0.00175f * step;
		}
		else {
			member->baseSize.y -= 0.00175f * step;
		}
		member->motionTimer++;
		break;
	case 7:
		if ((f32)member->motionTimer >= 22.0f * level) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if ((f32)member->motionTimer <= 10.0f * level) {
			member->baseSize.y += 0.00375f * step;
		}
		else {
			member->baseSize.y -= 0.00375f * step;
		}
		member->motionTimer++;
		break;
	case 9:
		if ((f32)member->motionTimer >= 64.0f * level) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if ((f32)member->motionTimer <= 31.0f * level) {
			member->baseSize.y += 0.00125f * step;
		}
		else {
			member->baseSize.y -= 0.00125f * step;
		}
		member->motionTimer++;
		break;
	case 2:
		break;
	default:
		if ((f32)member->motionTimer >= 32.0f * level) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if ((f32)member->motionTimer <= 15.0f * level) {
			member->baseSize.y += 0.0025f * step;
		}
		else {
			member->baseSize.y -= 0.0025f * step;
		}
		member->motionTimer++;
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
	else {
		// The original lays the itemId test out as a one-case switch: `beq` to the
		// body, then `b` past it (an `if` emits `bne` and falls through instead).
		switch (member->itemId) {
		case -1: {
		s32 r = irand(0x57e40);
		if (r < 0xa) {
			BattleAudience_ChangeStatus(index, 1);
		}
		else {
			t = 0x14;
			if (r < t) {
				BattleAudience_ChangeStatus(index, 2);
			}
			else {
#ifdef MUST_MATCH
			// `step` is dead after the type switch; reusing it (rather than a fresh
			// temp) colours the product f0 and the converted t f1 like the original.
			step = 10.0f * rangeA;
			t = (s32)((f32)t + step);
#else
			f32 rangeA10 = 10.0f * rangeA;
			t = (s32)((f32)t + rangeA10);
#endif
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
	}
}

#pragma opt_lifetimes reset
#pragma opt_propagation reset
void BattleAudienceCtrlProcessKinopioWaitHurimuki(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	if (member->motionTimer <= 0xf) {
		member->mRotation.y =
		    12.0f * (f32)member->motionTimer;
	}
	if (member->motionTimer >= 0x1e) {
		member->mRotation.y =
		    12.0f * (f32)(0x2d - member->motionTimer);
	}
	if (member->motionTimer > 0x2d) {
		BattleAudience_ChangeStatus(index, 0);
	}
	else {
		member->motionTimer++;
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
	if (member->motionTimer >= 0 && member->motionTimer <= 0xa) {
		member->drawOffset.x =
		    1.5f * (f32)member->motionTimer;
	}
	if (member->motionTimer >= 0x14 && member->motionTimer <= 0x28) {
		member->drawOffset.x -= 1.5f;
	}
	if (member->motionTimer >= 0x32 && member->motionTimer <= 0x3c) {
		member->drawOffset.x += 1.5f;
	}
	if (member->motionTimer > 0x3c) {
		BattleAudience_ChangeStatus(index, 0);
	}
	else {
		member->motionTimer++;
	}
}

void BattleAudienceCtrlProcessKinopioWaitJump(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 2, 0);
	}
	else {
		BattleAudience_SetAnim(index, 3, 0);
	}
	switch (member->step) {
	case 0:
		if (member->type == AUDIENCE_PUNI) {
			member->counter = 3;
		}
		else {
			member->counter = 1;
		}
		member->step++;
		// fallthrough
	case 1:
		if (member->type == AUDIENCE_PUNI) {
			f32 v = 5.0f;
			f32 w = -1.0f;
			member->jumpVelY = v;
			member->jumpAccelY = w;
		}
		else {
			f32 v = 3.4f;
			f32 w = -0.2f;
			member->jumpVelY = v;
			member->jumpAccelY = w;
		}
		member->step++;
		// fallthrough
	case 2:
		if (0.0f == member->jumpAccelY) {
			member->counter--;
			if (member->counter <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->step = 1;
			}
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioJoyRaiseArm(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 0) {
		BattleAudience_SetAnim(index, 4, 0);
	}
	switch (member->step) {
	case 0:
		if (irand(2) == 0) {
			member->yaw = 270.0f;
		}
		else {
			member->yaw = 90.0f;
		}
		member->step++;
		// fallthrough
	case 1:
		member->drawOffset.y = intplGetValue(4, member->counter, 0xf, 0.0f, 10.0f);
		member->counter++;
		if (member->counter > 0x1e) {
			f32 one = 1.0f;
			f32 zero = 0.0f;
			member->repeatCount = member->repeatCount - one;
			if (member->repeatCount <= zero) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->counter = 0;
			}
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioJoyJump(s32 index, s32 kind) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 2, 0);
	}
	else {
		BattleAudience_SetAnim(index, 3, 0);
	}
	switch (member->step) {
	case 0:
		member->counter = 1;
		member->step++;
		// fallthrough
	case 1:
		member->jumpVelY =
		    3.4f + (f32)rand() / 32767.0f;
		member->jumpAccelY = -0.2f;
		member->step++;
		// fallthrough
	case 2:
		if (0.0f == member->jumpAccelY) {
			member->counter--;
			if (member->counter <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->step = 1;
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
	switch (member->step) {
	case 0:
		member->counter = 2;
		member->step++;
		// fallthrough
	case 1: {
		f32 neg = -0.2f;
		member->jumpVelY = 4.0f + (f32)rand() / 32767.0f;
		member->jumpAccelY = neg;
		member->step++;
	}
		// fallthrough
	case 2:
	{
		f32 zero = 0.0f;
		member->mRotation.y += 20.0f;
		if (zero == member->jumpAccelY) {
			member->counter--;
			if (member->counter <= 0) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->step = 1;
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
			    -0.14999999f * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			    0.0f);
		}
	}
	else if (!(member->flags & 0x20) && (work->flags & 0x80)) {
		item = BattleAudienceItemGetPtr(member->itemId);
		if (!(audience->flags & 0x40000)) {
			item->timer--;
			if ((item->flags & 1) && item->timer <= 0 && item->state == 1) {
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
			    -0.14999999f * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			    0.0f);
		}
	}
	else if (member->flags & 0x20) {
		return;
	}
}

void BattleAudienceCtrlProcessKinopioBooing(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	BattleAudience_SetAnim(index, 4, 0);
	switch (member->step) {
	case 0:
		if (irand(2) == 0) {
			member->yaw = 270.0f;
		}
		else {
			member->yaw = 90.0f;
		}
		member->step++;
		// fallthrough
	case 1:
		member->drawOffset.y = intplGetValue(4, member->counter, 0xf, 0.0f, 10.0f);
		member->counter++;
		if (member->counter > 0x1e) {
			f32 one = 1.0f;
			f32 zero = 0.0f;
			member->repeatCount = member->repeatCount - one;
			if (member->repeatCount <= zero) {
				BattleAudience_ChangeStatus(index, 0);
			}
			else {
				member->counter = 0;
			}
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioDamage(s32 index) {
	BattleAudienceMember* member;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	switch (member->step) {
	case 0:
		BattleAudience_SetAnim(index, 9, 0);
		member->jumpVelY = 8.0f;
		member->jumpAccelY = -1.0f;
		member->motionTimer = 0;
		member->yaw = 270.0f;
		member->step++;
		// fallthrough
	case 1:
		if (member->motionTimer >= 8) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if (member->motionTimer <= 4) {
			member->baseSize.y -= 0.1f;
		}
		else {
			member->baseSize.y += 0.1f;
		}
		member->motionTimer++;
		if (BattleAudience_GetAnimEnd(index) == 1) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
			if ((u8)BattleAudience_GetEscapeChangeOK(index) == 1) {
				BattleAudience_ChangeStatus(index, 0xc);
				psndSFXOn_3D(str_SFX_AUDIENCE_PANIC_SCREAM2, &member->position);
			}
			else {
				BattleAudience_ChangeStatus(index, 0);
			}
		}
		break;
	}
}

void BattleAudienceCtrlProcessKinopioEnter(s32 index) {
	BattleAudienceMember* member;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	camGetPtr(8);
	BattleAudience_SetAnim(index, 7, 0);
	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		switch (member->step) {
		case 0:
		{
			f32 zero = 0.0f;
			member->position.x = member->mHomePosition.x;
			member->position.y = -100.0f;
			member->position.z = member->mHomePosition.z;
			member->velocity.x = zero;
			member->velocity.y = zero;
			member->velocity.z = zero;
			{
				s32 t = irand(0x3c);
				t = t + member->counter;
				member->counter = t + 1;
			}
			member->step++;
			break;
		}
		case 1:
			member->counter--;
			if (member->counter <= 0) {
				f32 one = 1.0f;
				f32 zero = 0.0f;
				member->velocity.x = zero;
				member->velocity.y = one;
				member->velocity.z = zero;
				member->step++;
			}
			break;
		case 2:
			if (member->position.y >= member->mHomePosition.y) {
				member->position.y = member->mHomePosition.y;
				BattleAudience_ChangeStatus(index, 0);
				member->step++;
			}
			break;
		}
	}
	else {
		switch (member->step) {
		case 0:
			member->position.x = (f32)(irand(0x190) - 0xc8);
		{
			f32 thousand = 1000.0f;
			member->position.y = member->mHomePosition.y;
			member->position.z = thousand;
			{
				f32 zero = 0.0f;
				member->velocity.x = zero;
				member->velocity.y = zero;
				member->velocity.z = zero;
			}
			{
				s32 t = irand(0x3c);
				t = t + member->counter;
				member->counter = t + 1;
			}
			member->step++;
			break;
		}
		case 1:
			member->counter--;
			if (member->counter <= 0) {
				f32 dx = member->mHomePosition.x - member->position.x;
				f32 dz = member->mHomePosition.z - member->position.z;
				f32 len = (f32)sqrt(dx * dx + dz * dz);
				f32 zero = 0.0f;
				f32 ten = 10.0f;
				member->velocity.x = ten * (dx / len);
				member->velocity.y = zero;
				member->velocity.z = ten * (dz / len);
				member->enterDist = ten;
				member->enterSpeed = ten;
				member->enterLength = len;
				member->step++;
			}
			break;
		case 2:
			member->enterDist += member->enterSpeed;
			if (member->enterDist >= member->enterLength) {
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
		switch (member->step) {
		case 0:
			member->counter--;
			if (member->counter <= 0) {
				member->velocity.x = 0.0f;
				member->velocity.y = -1.0f;
				member->velocity.z = 0.0f;
				member->step++;
			}
			break;
		case 1:
			if (member->position.y <= -100.0f) {
				BattleAudience_Delete(index);
				member->step++;
			}
			break;
		}
	}
	else {
		switch (member->step) {
		case 0:
			member->counter--;
			if (member->counter <= 0) {
				f32 v = ((2.0f * (f32)rand()) / 32767.0f -
				            1.0f) -
				        (member->position.x / 120.0f);
				{
					f32 zero = 0.0f;
					f32 eight = 8.0f;
					f32 six = 6.0f;
					f32 neg0p5 = -0.5f;
					member->velocity.x = v;
					member->velocity.y = zero;
					member->velocity.z = eight;
					member->jumpVelY = six;
					member->jumpAccelY = neg0p5;
				}
				member->step++;
				psndSFXOn_3D(str_SFX_AUDIENCE_RUN3, &member->position);
			}
			break;
		case 1:
			if (0.0f == member->jumpAccelY) {
				member->step++;
			}
			break;
		case 2:
			if (member->position.z >= 600.0f) {
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
	if (member->step <= 1) {
		if ((u8)kind == 1) {
			BattleAudience_SetAnim(index, 0xa, 0);
		}
		else {
			BattleAudience_SetAnim(index, 0xb, 0);
		}
		if (member->motionTimer >= 0x140) {
			member->baseSize.y = 1.0f;
			member->motionTimer = 0;
		}
		if (member->motionTimer <= 0xa0) {
			member->baseSize.y -= 0.0025f;
		}
		else {
			member->baseSize.y += 0.0025f;
		}
		member->motionTimer++;
		member->colorB = member->colorB + member->colorBStep;
		if (member->colorB >= 0xc8) {
			member->colorBStep = -4;
		}
		else if (member->colorB <= 5) {
			member->colorBStep = 4;
		}
	}
	switch (member->step) {
	case 0:
		BattleAudienceSoundSleep(index);
		member->step++;
		// fallthrough
	case 1:
		if (irand(0x8ca0) == 0) {
			effSleepEntry(0, 0xb4, member->position.x, 25.0f + member->position.y,
			    member->position.z, 1.0f, 45.0f);
			BattleAudienceSoundZZZ(index);
		}
		break;
	case 2:
		member->jumpVelY =
		    6.0f + (f32)rand() / 32767.0f;
		member->jumpAccelY = -0.5f;
		if ((u8)kind == 1) {
			BattleAudience_SetAnim(index, 0, 0);
		}
		else {
			BattleAudience_SetAnim(index, 1, 0);
		}
		member->baseSize.y = 1.0f;
		member->colorB = 0;
		member->step++;
		// fallthrough
	case 3:
		if (0.0f == member->jumpAccelY) {
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
	switch (member->step) {
	case 0:
		BattleAudience_SetAnim(index, 1, 0);
		member->targetUnit = BattleAudienceDetectTargetAll();
		member->counter = 0;
		psndSFXOn_3D(str_SFX_AUDIENCE_TELESA_MOVE1, &member->position);
		member->step = 5;
		// fallthrough
	case 5:
		unit = member->targetUnit;
		part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
		member->counter++;
		member->position.x = intplGetValue(5, member->counter, 0x3c,
		    member->mHomePosition.x, 0.0f);
		member->position.y = intplGetValue(5, member->counter, 0x3c,
		    member->mHomePosition.y, 80.0f);
		member->position.z = intplGetValue(5, member->counter, 0x3c,
		    member->mHomePosition.z, 100.0f);
		if (member->counter >= 0x3c) {
			BattleAudience_SetAnim(index, 7, 0);
			BtlUnit_GetHitPos(unit, part, &x_873, &y_874, &z_875);
			if (member->position.x >= x_873) {
				member->yaw = 270.0f;
			}
			else {
				member->yaw = 90.0f;
			}
			member->step = 0xa;
			member->counter = 0;
			unit = member->targetUnit;
			part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
			BtlUnit_GetHitPos(unit, part, &x_873, &y_874, &z_875);
			psndSFXOn_3D(str_SFX_AUDIENCE_TELESA_MOVE1, &member->position);
		}
		break;
	case 0xa:
		member->counter++;
		member->position.x =
		    intplGetValue(0, member->counter, 0x1e, 0.0f, x_873);
		member->position.y = intplGetValue(4, member->counter, 0x1e, 80.0f,
		    y_874 - 15.0f);
		member->position.z =
		    intplGetValue(0, member->counter, 0x1e, 100.0f, z_875);
		if (member->counter == 0x1e) {
			unit = member->targetUnit;
			part = BattleGetUnitPartsPtr(BtlUnit_GetUnitId(unit), BtlUnit_GetBodyPartsId(unit));
			BtlUnit_GetStatus(unit, 0x12, &turns, &strength);
			if (turns < 1 && !BtlUnit_CheckShadowGuard(unit)) {
				BattleCheckDamage(BattleGetSystemPtr(work), unit, part,
				    &weapon_teresa_trans, 0x100);
				BtlUnit_snd_se(unit, str_SFX_AUDIENCE_TELESA_INVISIBLE1, -250000000, 0);
			}
		}
		if (member->counter >= 0x78) {
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
	switch (member->step) {
	case 0:
		if (irand(2) == 0) {
			member->intrudeDir = 1.0f;
		}
		else {
			member->intrudeDir = -1.0f;
		}
		if (pouch->rank == 0) {
			member->step = 3;
			break;
		}
		if (irand(2) == 0) {
			member->step = 3;
			break;
		}
		member->step = 0x32;
		break;
	case 3: {
		f32 dz;
		f32 dx;
		member->step = 5;
		dir = member->intrudeDir;
		t = 25.0f * dir;
		dx = t - member->position.x;
		dz = 110.0f - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		member->velocity.x = vx;
		member->velocity.y = 0.0f;
		member->accel.y = 0.0f;
		member->velocity.z = vz;
		intrudeSound = psndSFXOn(str_SFX_AUDIENCE_HEIHO_MOVE1);
	}
		// fallthrough
	case 5:
		if (member->position.z <= 110.0f) {
			member->step = 0xa;
			member->position.z = 110.0f;
			member->velocity.x = 0.0f;
			member->velocity.y = 10.0f;
			member->accel.y = -0.7f;
			member->velocity.z = -1.5f;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0xa: {
		f32 zero = 0.0f;
		if (member->velocity.y <= zero &&
		    member->position.y <= zero) {
			f32 dz;
			f32 dx;
			member->step = 0xf;
			member->position.y = zero;
			member->velocity.y = zero;
			member->accel.y = zero;
			dir = member->intrudeDir;
			t = 300.0f * dir;
			dx = t - member->position.x;
			dz = zero - member->position.z;
			len = (f32)sqrt(dx * dx + dz * dz);
			vx = 5.0f * (dx / len);
			vz = 5.0f * (dz / len);
			member->velocity.x = vx;
			member->velocity.y = zero;
			member->velocity.z = vz;
			btl_camera_set_mode(0, 3);
			btl_camera_set_moveto(0, 0x1e, 4, 698.0f * member->intrudeDir,
			    100.0f, zero, zero,
			    60.0f, zero);
			intrudeSound = psndSFXOn(str_SFX_AUDIENCE_HEIHO_MOVE1);
		}
		break;
	}
	case 0xf:
		member->mRotation.y = camGetPtr(4)->viewYaw;
#ifdef MUST_MATCH
		// Read through a pointer cast, as retail loads it; the plain read scores 99.995%.
		if (__fabs(member->position.x) >= *(const f64*)&double_450_802f8d88) {
#else
		if (__fabs(member->position.x) >= double_450_802f8d88) {
#endif
			member->step = 0x10;
			member->counter = 0x23;
			member->velocity = vec3_802f8c84;
		}
		break;
	case 0x10:
		member->counter--;
		if (member->counter <= 0) {
			member->step = 0x12;
			member->counter = 0x19;
			btl_camera_set_mode(0, 0);
			btl_camera_set_moveSpeedLv(0, 2);
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x12:
		member->counter--;
		if (member->counter <= 0) {
			member->step = 0x13;
			member->counter = 0x3c;
			btl_camera_set_moveSpeedLv(0, 1);
			effSmallStarEntry(0, 4, 185.0f * member->intrudeDir,
			    20.0f, 40.0f, 0.0f,
			    -1.0f, 0.0f);
		}
		break;
	case 0x13:
		member->counter--;
		if (member->counter <= 0) {
			member->step = 0x14;
			btl_camera_set_moveSpeedLv(0, 1);
		}
		// fallthrough
	case 0x14:
		member->evtId = BattleStage_ObjectFallCheck();
		member->step = 0x19;
		break;
	case 0x19:
		if (!evtCheckID(member->evtId)) {
			member->step = 0x1e;
			member->mRotation.y = 0.0f;
		}
		break;
	case 0x1e:
		BattleAudience_ChangeStatus(index, 0xb);
		break;
	case 0x32: {
		f32 dz;
		f32 dx;
		member->step = 0x37;
		dir = member->intrudeDir;
		t = 25.0f * dir;
		dx = t - member->position.x;
		dz = 110.0f - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		member->velocity.x = vx;
		member->velocity.y = 0.0f;
		member->accel.y = 0.0f;
		member->velocity.z = vz;
		intrudeSound = psndSFXOn(str_SFX_AUDIENCE_HEIHO_MOVE1);
	}
		// fallthrough
	case 0x37:
		if (member->position.z <= 110.0f) {
			member->step = 0x3c;
			member->position.z = 110.0f;
			member->velocity.x = 0.0f;
			member->velocity.y = 10.0f;
			member->accel.y = -0.7f;
			member->velocity.z = -1.5f;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x3c:
		if (member->velocity.y <= 0.0f &&
		    member->position.y <= 0.0f) {
			f32 zero = 0.0f;
			member->step = 0x41;
			member->counter = 0;
			member->position.y = zero;
			member->velocity.x = zero;
			member->velocity.y = zero;
			member->accel.y = zero;
			member->velocity.z = zero;
			intrudeSound = psndSFXOn(str_SFX_AUDIENCE_HEIHO_MOVE1);
		}
		break;
	case 0x41:
	case 0x42:
	case 0x43: {
		f32 one = 1.0f;
		member->counter++;
		if (one == member->intrudeDir) {
			member->yaw = 270.0f;
		}
		if (-1.0f == member->intrudeDir) {
			member->yaw = 90.0f;
		}
		if (member->counter <= 5) {
			member->position.x = member->position.x -
			                     5.0f * member->intrudeDir;
		}
		else if (member->counter <= 0xa) {
			member->position.x = member->position.x +
			                     5.0f * member->intrudeDir;
		}
		if (member->counter == 0xa) {
			if (member->step == 0x43) {
				member->step = 0x46;
			}
			else {
				member->step = member->step + 1;
			}
			member->counter = 0;
		}
	}
		break;
	case 0x46: {
		f32 zero = 0.0f;
		f32 dz;
		f32 dx;
		BattleStage_NozzleWorkCheck(1);
		member->evtId = work->stageObjectWork.nozzleWorkEventId;
		member->step = 0x4b;
		dir = member->intrudeDir;
		t = 300.0f * dir;
		dx = t - member->position.x;
		dz = zero - member->position.z;
		len = (f32)sqrt(dx * dx + dz * dz);
		vx = 5.0f * (dx / len);
		vz = 5.0f * (dz / len);
		member->velocity.x = vx;
		member->velocity.y = zero;
		member->velocity.z = vz;
	}
		break;
	case 0x4b:
#ifdef MUST_MATCH
		// Read through a pointer cast, as retail loads it; the plain read scores 99.995%.
		if (__fabs(member->position.x) >= *(const f64*)&double_300_802f8d90) {
#else
		if (__fabs(member->position.x) >= double_300_802f8d90) {
#endif
			member->step = 0x50;
			member->velocity = vec3_802f8c90;
			psndSFXOff(intrudeSound);
		}
		break;
	case 0x50:
		if (!evtCheckID(member->evtId)) {
			member->step = 0x55;
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
	switch (member->step) {
	case 0:
		member->counter = 0x1e;
		member->step++;
		// fallthrough
	case 1:
		member->counter--;
		member->mRotation.y += 6.0f;
		if (member->counter == 0xf) {
			BattleAudience_SetAnim(index, 0xe, 0);
			BattleAudienceSoundSing(index);
		}
		if (member->counter <= 0) {
			member->counter = 0x78;
			member->step++;
		}
		break;
	case 2:
		member->counter--;
		if (gp->retraceCount % 0xa == 0) {
			f32 y;
			f32 z;
			f32 x;
			z = member->position.z + (f32)irand(0xa) - 5.0f;
			y = member->position.y + (f32)irand(0xa);
			x = member->position.x + (f32)irand(0xc8) - 100.0f;
			effOnpuN64Entry(0, x, y, z);
		}
		if (member->counter <= 0) {
			for (i = 0; i < audience->mMaxAudience; i++) {
				if (index != i) {
					BattleAudienceCtrlProcessKinopioSingSub(i);
				}
			}
			member->counter = 0x1e;
			member->step++;
		}
		break;
	case 3:
		member->counter--;
		member->mRotation.y += 6.0f;
		if (member->counter <= 0xf) {
			BattleAudience_SetAnim(index, 1, 0);
		}
		if (member->counter <= 0) {
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
		member->singCount = 3;
	}
}

void BattleAudienceCtrlProcessKinopioShell(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);

	if (member->step >= 2 && member->step <= 5) {
		BattleAudience_SetAnim(index, 0xf, 0);
	}
	else {
		BattleAudience_SetAnim(index, 1, 0);
	}
	switch (member->step) {
	case 0:
		member->counter = irand(0x1e);
		member->step++;
		// fallthrough
	case 1:
		member->counter--;
		if (member->counter <= 0) {
			member->step++;
		}
		break;
	case 2:
		{
			f32 v = 5.0f;
			f32 w = -0.3f;
			member->jumpVelY = v;
			member->jumpAccelY = w;
		}
		BattleAudienceSoundShell(index);
		member->step++;
		break;
	case 4:
		member->counter = irand(0x1e);
		member->step++;
		// fallthrough
	case 5:
		member->counter--;
		if (member->counter <= 0) {
			member->step++;
		}
		break;
	case 6:
		{
			f32 v = 5.0f;
			f32 w = -0.3f;
			member->jumpVelY = v;
			member->jumpAccelY = w;
		}
		member->counter = 0xa;
		BattleAudienceSoundShell(index);
		member->step++;
		break;
	case 7:
		member->counter--;
		if (member->counter <= 0) {
			BattleAudience_ChangeStatus(index, 0);
		}
		member->timer = 0;
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
	switch (member->step) {
	case 0:
		member->counter = 0x1e;
		BattleAudienceSoundBombIgnite(index, 0xb4, 0x78);
		member->step++;
		// fallthrough
	case 1:
		member->counter--;
		if (member->counter <= 0) {
			member->counter = 0xa;
			member->step++;
		}
		break;
	case 2:
		member->counter--;
		if (member->counter <= 0) {
			member->step++;
		}
		break;
	case 3:
		break;
	}
}


void BattleAudienceCtrlProcessKinopioFire(s32 index, s32 kind) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	s32 offsets[24][2];
	s32 i;
	f32 step;

	if ((u8)kind == 1) {
		BattleAudience_SetAnim(index, 0x10, 0);
	}
	else {
		BattleAudience_SetAnim(index, 0x11, 0);
	}
	switch (member->step) {
	case 0:
		member->counter = 0xa;
		member->step++;
		// fallthrough
	case 1:
		step = 0.05f;
		member->baseSize.x += step;
		member->baseSize.y += step;
		member->counter--;
		if (member->counter <= 0) {
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
				effSandarsEntry(4, member->position.x, 35.0f + member->position.y,
				    member->position.z, 0.0f);
			}
			btl_camera_shake_h(0, 0x1e, 0, 5.0f, 0.0f);
			for (i = 0; i < 0x18; i++) {
				s32 no = BattleAudience_GetAudienceNoFromOffset(
				    index, offsets[i][0], offsets[i][1]);
				if (no != -1) {
					BattleAudienceCtrlProcessKinopioFireSub(no);
				}
			}
			BattleAudiencePuniAllEscape();
			member->counter = 0x3c;
			member->step++;
		}
		break;
	case 2:
		member->counter--;
		if (member->counter <= 0) {
			member->step++;
		}
		break;
	case 3:
		BattleAudience_Delete(index);
		audience->mTargetAudienceCount -= 1.0f;
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
		audience->mTargetAudienceCount -= 1.0f;
	}
}


// NONMATCHING (99.89%): 6 register diffs in case 9, the i2f magic double and the xi conversion
// swapping f3/f4 (0.1f f2, frow f1, zterm f0 and the id/col/row GPRs r28/r5/r4 all match).
// Declaring id at function scope and storing position.x before the stage y load got here.
void BattleAudienceCtrlProcessKinopioEat(s32 index) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	BattleStage* stage = BattleStageGetPtr();
	BattleAudienceMember* target;
	s32 id;
	s32 candidates[60];
	s32 found;
	s32 i;
	s32 count;

	switch (member->step) {
	case 0:
		found = BattleAudienceDetectPakkunEatTarget(index);
		if (found != -1) {
			member->eatTargetId = found;
			target = BattleAudienceGetPtr(member->eatTargetId);
			if (member->position.x >= target->position.x) {
				member->yaw = 270.0f;
			}
			else {
				member->yaw = 90.0f;
			}
			BattleAudience_SetAnim(index, 0, 0);
		}
		else {
			BattleAudience_ChangeStatus(index, 0);
			break;
		}
		member->counter = 0x1e;
		member->step++;
		// fallthrough
	case 1:
		target = BattleAudienceGetPtr(member->eatTargetId);
		if (member->counter > 0) {
			member->counter--;
		}
		if (member->counter <= 0 && 0.0f == target->jumpOffset.y) {
			BattleAudience_SetAnim(index, 0x12, 0);
			target->flags |= 2;
			member->step = 3;
		}
		break;
	case 3:
		target = BattleAudienceGetPtr(member->eatTargetId);
		if (member->position.x >= target->position.x) {
			member->drawOffset.x -= 6.0f;
		}
		else {
			member->drawOffset.x += 6.0f;
		}
		if (member->drawOffset.x <= -60.0f ||
		    member->drawOffset.x >= 60.0f) {
			member->step++;
		}
		break;
	case 4:
		target = BattleAudienceGetPtr(member->eatTargetId);
		target->flags = 0;
		audience->mTargetAudienceCount -= 1.0f;
		BattleAudienceSoundPakkunEat(index);
		member->step++;
		break;
	case 5: {
		f32 zero = 0.0f;
		if (member->drawOffset.x > zero) {
			member->drawOffset.x -= 6.0f;
			if (member->drawOffset.x <= zero) {
				member->step = 7;
			}
		}
		else {
			member->drawOffset.x += 6.0f;
			if (member->drawOffset.x >= zero) {
				member->step = 7;
			}
		}
		break;
	}
	case 7:
		found = BattleAudienceDetectPakkunEatTarget(index);
		if (found != -1) {
			member->step = 0x63;
			member->counter = 0xa;
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
			member->step = 8;
			member->targetId = candidates[irand(count)];
		}
		else if (i == 0x3c) {
			member->step = 0x63;
			member->counter = 0xa;
		}
		break;
	case 8: {
		f32 zero = 0.0f;
		f32 neg1 = -1.0f;
		member->velocity.x = zero;
		member->velocity.y = neg1;
		member->velocity.z = zero;
		member->step++;
		break;
	}
	case 9:
		if (member->position.y <= -100.0f) {
			s32 r;
			s32 col;
			s32 row;
			f32 x;
			f32 z;

			id = member->targetId;
			r = rand() * 10 / 0x7fff;
			col = id / 0x14;
			row = id % 0x14;
			x = (f32)(row * 0x19 - 0xeb + r);
			member->mHomePosition.x = (member->position.x = x);
			member->mHomePosition.y = stage->audienceAreaSize.y;
			z = 0.1f * (f32)row + (f32)(col * 0x1e + 0x73);
			member->position.z = z;
			member->mHomePosition.z = z;
			member->step++;
		}
		break;
	case 0xa: {
		f32 zero = 0.0f;
		f32 one = 1.0f;
		member->velocity.x = zero;
		member->velocity.y = one;
		member->velocity.z = zero;
		member->step++;
		break;
	}
	case 0xb: {
		f32 stageY;
		if (member->position.y >= stage->audienceAreaSize.y) {
			stageY = stage->audienceAreaSize.y;
			member->mHomePosition.y = stageY;
			member->position.y = stageY;
			member->velocity.y = 0.0f;
			member->step = 0x63;
			member->counter = 0xa;
			target = BattleAudienceGetPtr(member->targetId);
			memcpy(target, member, 0x134);
			BattleAudience_Delete(index);
		}
		break;
	}
	case 0x63:
		member->counter--;
		if (member->counter <= 0) {
			BattleAudience_ChangeStatus(index, 0);
		}
		break;
	}
}
