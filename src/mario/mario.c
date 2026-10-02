#include "mario/mario.h"
#include "mario/mariost.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "party/party_vivian.h"
#include "system.h"
#include "party.h"
#include "mario/mario_party.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include <string.h>
#include "driver/seqdrv.h"
#include "party/party_yoshi.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "motion/mot_roll.h"
#include "party/party_kuribo.h"
#include "motion/mot_ship.h"
#include "motion/mot_slit.h"
#include "party/party_bomhei.h"
#include "party/party_nokonoko.h"
#include "koopa.h"
#include "peach.h"
#include "motion/mot_plane.h"

//.data
const char* dotMarioPose[4][6] = {
	{"Z_1", "S_1", "W_1", "R_1", "U_1", "J_1"},
	{"Z_15", "S_2", "W_15", "R_15", "U_2", "J_2"},
	{"Z_17", "S_3", "W_17", "R_17", "U_3", "J_3"},
	{"Z_18", "S_4", "W_18", "R_18", "U_4", "J_4"}
};

//.sdata
const char* marioAnimeGroupData[4][3] = {
	{"a_mario", "a_mario_r", "e_mario"},
	{"a_mario_l", "a_mario_lr", "e_mario_l"},
	{"a_mario_w", "a_mario_wr", "e_mario_w"},
	{"a_mario_wl", "a_mario_wlr", "e_mario_wl"}
};

//unreferenced vec constants kept to preserve the original .rodata layout
const Vec vec3_802c3698 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802c36a4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802c36b0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802c36bc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802c36c8 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802c36d4 = {0.0f, 74.0f, 0.0f};
const Vec vec3_802c36e0 = {1.0f, 1.0f, 1.0f};
const Vec vec3_802c36ec = {0.0f, 74.0f, 0.0f};
const Vec vec3_802c36f8 = {0.0f, -10000.0f, 0.0f};
const Vec vec3_802c3704 = {0.0f, 0.0f, 0.0f};

//.sdata2
const u32 dat_80419e34 = 0xFFFFFF00;

//.bss
Vec blurPos[8];

//.sbss
MarioWork* mp;

//local prototypes

//locals for symbols with no home header
extern Vec R_last_position;
#include "mario/mario_balloon.h"
#include "mario/mario_pouch.h"
#include "memory.h"
extern s32 blurPosId;
#include "mario/mario_kemuri.h"
#include "driver/itemdrv.h"
#include "motion/mot_damage.h"
#include "math_ext.h"
extern s32 __float_nan[];
#include "driver/shadowdrv.h"

MarioWork* marioGetPtr(void) {
	return mp;
}

s32 marioGetColor(void) {
	return mp->colorId;
}

#ifdef MUST_MATCH
// MUST_MATCH: folding the colour selection into a helper keeps the value in r3, as the original does.
static inline s32 marioCharModeColor(BOOL badge1, BOOL badge2) {
	if (badge1 && !badge2) {
		return 1;
	}
	else if (!badge1 && badge2) {
		return 2;
	}
	else if (badge1 && badge2) {
		return 3;
	}
	return 0;
}
#endif

void marioSetCharMode(s32 mode) {
	MarioWork* wp = mp;
	s32 materialFlag;
	s32 lightFlag;

	if (mode == 1) {
		wp->characterId = 1;
		peachPreInit();
		if (wp->paperAnimGroupId[0] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[0]);
			wp->paperAnimGroupId[0] = -1;
		}
		if (wp->paperAnimGroupId[1] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[1]);
			wp->paperAnimGroupId[1] = -1;
		}
		if (wp->paperAnimGroupId[2] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[2]);
			wp->paperAnimGroupId[2] = -1;
		}
		if (!(wp->peachFlags & 0x2000)) {
			wp->paperAnimGroupId[0] = animPoseEntry("c_peach", 2);
			wp->paperAnimGroupId[1] = animPoseEntry("c_peach_r", 2);
			npcSetMarioAutoTalkPose("P_S_1", "P_T_1");
		}
		else {
			wp->paperAnimGroupId[0] = animPoseEntry("c_zako_n", 2);
			wp->paperAnimGroupId[1] = animPoseEntry("c_peach_r", 2);
			npcSetMarioAutoTalkPose("S_1", "T_1");
			wp->wPlayerCollisionBox.y = 30.0f;
			wp->wPlayerCollisionBox.x = 30.0f;
		}
		marioSetSpec();
		wp->trigFlags |= 0x1000;
		return;
	}
	if (mode == 2) {
		kpaPreInit();
		if (wp->paperAnimGroupId[0] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[0]);
			wp->paperAnimGroupId[0] = -1;
		}
		if (wp->paperAnimGroupId[1] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[1]);
			wp->paperAnimGroupId[1] = -1;
		}
		if (wp->paperAnimGroupId[2] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[2]);
			wp->paperAnimGroupId[2] = -1;
		}
		wp->paperAnimGroupId[0] = animPoseEntry(kpaGetStageViewType() == 1 ? "c_koopa" : "c_koopa2", 2);
		wp->characterId = 2;
		npcSetMarioAutoTalkPose("KPA_S_1", "KPA_T_1");
		kpaSetLevel(1);
		wp->trigFlags |= 0x1000;
		return;
	}
	if (!(wp->flags & 0x80000000)) {
		s32 id;

		if (wp->dispFlags & 0x10000000) {
			id = 2;
		}
		else if (wp->dispFlags & 0x80000000) {
			id = 1;
		}
		else {
			id = 0;
		}
		materialFlag = animPoseGetMaterialFlag(wp->paperAnimGroupId[id]);
		if (mp->dispFlags & 0x10000000) {
			id = 2;
		}
		else if (mp->dispFlags & 0x80000000) {
			id = 1;
		}
		else {
			id = 0;
		}
		lightFlag = animPoseGetMaterialLightFlag(wp->paperAnimGroupId[id]);
		wp->characterId = 0;
		if (mp->characterId == 0) {
			BOOL badge1 = pouchEquipCheckBadge(0x147);
			BOOL badge2 = pouchEquipCheckBadge(0x148);
#ifdef MUST_MATCH
			s32 color = marioCharModeColor(badge1, badge2);
#else
			s32 color;

			if (badge1 && !badge2) {
				color = 1;
			}
			else if (!badge1 && badge2) {
				color = 2;
			}
			else if (badge1 && badge2) {
				color = 3;
			}
			else {
				color = 0;
			}
#endif

			if (mp->characterId == 0 && mp->colorId != color) {
				mp->colorId = color;
				marioSetCharMode(0);
			}
		}
		if (wp->paperAnimGroupId[0] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[0]);
			wp->paperAnimGroupId[0] = -1;
		}
		if (wp->paperAnimGroupId[1] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[1]);
			wp->paperAnimGroupId[1] = -1;
		}
		if (wp->paperAnimGroupId[2] >= 0) {
			animPoseRelease(wp->paperAnimGroupId[2]);
			wp->paperAnimGroupId[2] = -1;
		}
		wp->paperAnimGroupId[0] = animPoseEntry(marioAnimeGroupData[mp->colorId][0], 2);
		wp->paperAnimGroupId[1] = animPoseEntry(marioAnimeGroupData[mp->colorId][1], 2);
		wp->paperAnimGroupId[2] = animPoseEntry(marioAnimeGroupData[mp->colorId][2], 2);
		animPoseSetMaterialFlagOn(wp->paperAnimGroupId[0], materialFlag);
		animPoseSetMaterialFlagOn(wp->paperAnimGroupId[1], materialFlag);
		animPoseSetMaterialFlagOn(wp->paperAnimGroupId[2], materialFlag);
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[0], lightFlag);
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[1], lightFlag);
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[2], lightFlag);
		npcSetMarioAutoTalkPose("M_S_1", "M_I_2");
		wp->trigFlags |= 0x1000;
	}
	else {
		marioSetFamicomMode(1);
	}
	if (!(wp->flags & 0x2000000)) {
		MarioMotion motion = wp->currMotionId;

		if (motion == 0x18) {
			marioForcePlaneAnime();
			return;
		}
		if (motion == 0x19) {
			marioForceShipAnime();
			return;
		}
		if (motion == 0x16) {
			marioForceRollAnime();
			return;
		}
		if (wp->dispFlags & 0x01000000) {
			marioForceSlitAnime();
			return;
		}
		if (motion == 0x1c) {
			N_marioForceVivianAnime();
			return;
		}
		if (motion == 0x1a) {
			marioChgPose("M_Y_2");
			marioChgPose("M_Y_1");
			return;
		}
	}
	if (wp->currMotionId != 0x18 && wp->currMotionId != 0x19 && wp->currMotionId != 0x14) {
		marioSetSpec();
	}
}

void marioSetFamicomMode(s32 mode) {
	MarioWork* wp = mp;

	if (mode == 1) {
		if (!(wp->flags & MARIO_FLAG_IS_8_BIT)) {
			wp->flags |= MARIO_FLAG_IS_8_BIT;
			marioChgMot(MARIO_MOTION_STAY);
			if (wp->paperAnimGroupId[0] >= 0) {
				animPoseRelease(wp->paperAnimGroupId[0]);
			}
			if (wp->paperAnimGroupId[1] >= 0) {
				animPoseRelease(wp->paperAnimGroupId[1]);
			}
			wp->paperAnimGroupId[0] = animPoseEntry("d_mario", 2);
			wp->paperAnimGroupId[1] = animPoseEntry(marioAnimeGroupData[mp->colorId][1], 2);
			wp->characterId = 0;
			npcSetMarioAutoTalkPose("M_S_1", "M_I_2");
			wp->animName = NULL;
			marioChgPose("M_S_1");
		}
		else {
			wp->animName = NULL;
			marioChgPose("M_S_1");
		}
	}
	else {
		if (wp->flags & MARIO_FLAG_IS_8_BIT) {
			wp->flags &= ~MARIO_FLAG_IS_8_BIT;
			if (wp->paperAnimGroupId[0] >= 0) {
				animPoseRelease(wp->paperAnimGroupId[0]);
			}
			if (wp->paperAnimGroupId[1] >= 0) {
				animPoseRelease(wp->paperAnimGroupId[1]);
			}
			wp->paperAnimGroupId[0] = animPoseEntry(marioAnimeGroupData[mp->colorId][0], 2);
			wp->paperAnimGroupId[1] = animPoseEntry(marioAnimeGroupData[mp->colorId][1], 2);
			wp->characterId = 0;
			npcSetMarioAutoTalkPose("M_S_1", "M_I_2");
			wp->animName = NULL;
			marioChgPose("M_S_1");
		}
		else {
			wp->animName = NULL;
			marioChgPose("M_S_1");
		}
	}
}

void marioSetSpec(void) {
	MarioWork* wp = mp;

	if (wp->characterId == 1) {
		(wp->wPlayerCollisionBox).x = 20.0f;
		(wp->wPlayerCollisionBox).y = 47.0f;
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.0f;
	}
	else if (wp->characterId == 2) {
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.25f;
	}
	else {
		if (!(wp->flags & 0x2000000)) {
			(wp->wPlayerCollisionBox).x = 20.0f;
			(wp->wPlayerCollisionBox).y = 37.0f;
		}
		else {
			(wp->wPlayerCollisionBox).x = 10.0f;
			(wp->wPlayerCollisionBox).y = 25.9f;
		}
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.25f;
	}
}

//TODO: finish
void marioEntry(void) {
	MarioWork* wp = mp;

	wp->flags |= MARIO_FLAG_ACTIVE;
	marioChgPose("M_S_1");
	if (wp->flags & MARIO_FLAG_BG_MODE) {
		if (wp->currMotionId == MARIO_MOTION_SLIT) {
			wp->flags |= 0x100000;
			marioSlitForceCancel();
		}
		marioChgMot(MARIO_MOTION_STAY);
	}
	else {
		if (wp->currMotionId == MARIO_MOTION_SHIP) {
			marioReInit_ship();
		}
		else if (wp->currMotionId == MARIO_MOTION_ROLL) {
			marioReInit_roll();
		}
		else if (wp->currMotionId == MARIO_MOTION_SLIT) {
			marioReInit_slit();
		}
		else if (wp->currMotionId == MARIO_MOTION_YOSHI) {
			marioRideYoshi();
		}
		else {
			marioChgMot(MARIO_MOTION_STAY);
		}
	}
}

void marioItemGetDisable(void) {
	MarioWork* wp = mp;

	wp->flags |= 0x4000;
	wp->field_0x7A = 0x14;
}

void marioItemGetChk(void) {
	MarioWork* work;
	s32 var;

	work = mp;
	var = mp->field_0x7A;
	if (var > 0) {
		var--;
		work->field_0x7A = (s16)var;
		if ((s16)var <= 0) {
			work->field_0x7A = 0;
			mp->flags &= ~0x4000u;
		}
	}
}

BOOL marioItemGetOk(void) {
	return (mp->flags & 0x4000) == 0;
}

BOOL marioCaseEventValidChk(void) {
	if (mp->currMotionId == MARIO_MOTION_PLANE) {
		return FALSE;
	}
	else {
		return vivianGetStatus() == 0;
	}
}

BOOL unk_8005bb80(void) {
	if (mp->currMotionId == MARIO_MOTION_ROLL || mp->flags & 0x100000) {
		return FALSE;
	}
	return TRUE;
}

// NONMATCHING (0.50%): 1 instruction differs, 276 missing.
BOOL marioCheckMenuDisable(void) {
	return TRUE;
}

BOOL marioChkKey(void) {
	PartyEntry* party = partyGetPtr(marioGetPartyId());

	if (seqGetSeq() != 2) {
		return FALSE;
	}
	if (marioKeyOffChk()) {
		return FALSE;
	}
	if (marioShipChanging() == 1 || marioShipChanging() == 2) {
		return FALSE;
	}
	if (!marioRollKeyDisable()) {
		return FALSE;
	}
	if (!marioSlitKeyDisable()) {
		return FALSE;
	}
	if (party == NULL) {
		return TRUE;
	}
	if (christineGetStatus() == 1) {
		return FALSE;
	}
	if (!mario_bomhei_keychk2()) {
		return FALSE;
	}
	if (yoshiGetStatus() != 0) {
		return FALSE;
	}
	if (party->flags & PARTY_FLAG_IS_BEING_USED) {
		if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ || party->currentMemberId == PARTY_MEMBER_FLURRIE || party->currentMemberId == PARTY_MEMBER_VIVIAN) {
			return FALSE;
		}
	}
	if (nokonoko_holdItem()) {
		return FALSE;
	}
	return TRUE;
}

BOOL marioChkCtrl(void) {
	return ((s8*)mp->field_0x32)[6] == 0;
}

s32 N_marioChkUseParty(void) {
	if ((u16)gp->buttonNew[0] & 0x100f) {
		return 1;
	}
	return ((s8*)mp->field_0x32)[6];
}

void marioFBattlePrepare(void) {
	MarioWork* wp = mp;

	R_last_position = wp->position;
	marioAdjustMoveDir();
	if (wp->currMotionId != MARIO_MOTION_JABARA) {
		wp->flags |= MARIO_FLAG_PRE_BATTLE_PHYSICS;
	}
	partyUsePost(partyGetPtr(marioGetPartyId()));
	partyCtrlOff();
}

void marioFBattlePost(void) {
	MarioWork* wp = mp;

	wp->flags &= ~MARIO_FLAG_PRE_BATTLE_PHYSICS;
	if (wp->currMotionId == MARIO_MOTION_JABARA) {
		wp->position = R_last_position;
	}
	if (wp->currMotionId == MARIO_MOTION_YOSHI) {
		marioYoshiForceCancel();
	}
	partyCtrlOn();
	if (wp->currMotionId == MARIO_MOTION_HAMMER || wp->currMotionId == MARIO_MOTION_JABARA) {
		marioChgMot(MARIO_MOTION_STAY);
	}
}

s32 marioCtrlOffChk(void) {
	return ((s8*)mp->field_0x32)[6];
}

s32 marioKeyOffChk(void) {
	return ((s8*)mp->field_0x32)[7];
}

s32 marioCtrlOff(void) {
	MarioWork* wp = mp;

	wp->flags |= MARIO_FLAG_CTRL_OFF;
	((s8*)wp->field_0x32)[6]++;
	return ((s8*)wp->field_0x32)[6];
}

s32 marioCtrlOn(void) {
	MarioWork* wp = mp;

	((s8*)wp->field_0x32)[6]--;
	if (((s8*)wp->field_0x32)[6] == 0) {
		wp->flags &= ~MARIO_FLAG_CTRL_OFF;
	}
	return ((s8*)wp->field_0x32)[6];
}

s32 marioCtrlOff2(void) {
	MarioWork* wp = mp;

	wp->flags |= MARIO_FLAG_CTRL_OFF_2;
	wp->flags |= MARIO_FLAG_CTRL_OFF;
	((s8*)wp->field_0x32)[6]++;
	return ((s8*)wp->field_0x32)[6];
}

s32 marioCtrlOn2(void) {
	MarioWork* wp = mp;

	wp->flags &= ~MARIO_FLAG_CTRL_OFF_2;
	((s8*)wp->field_0x32)[6]--;
	if (((s8*)wp->field_0x32)[6] == 0) {
		wp->flags &= ~MARIO_FLAG_CTRL_OFF;
	}
	return ((s8*)wp->field_0x32)[6];
}

s32 marioKeyOff(void) {
	MarioWork* wp = mp;

	wp->flags |= MARIO_FLAG_KEY_OFF;
	((s8*)wp->field_0x32)[7]++;
	return ((s8*)wp->field_0x32)[7];
}

s32 marioKeyOn(void) {
	MarioWork* wp = mp;

	((s8*)wp->field_0x32)[7]--;
	if (((s8*)wp->field_0x32)[7] == 0) {
		wp->flags &= ~MARIO_FLAG_KEY_OFF;
	}
	return ((s8*)wp->field_0x32)[7];
}

s16 marioSetMutekiTime(s32 msec) {
	MarioWork* wp = mp;
	PartyEntry* party;

	wp->unk4C = sysMsec2Frame(msec);
	party = partyGetPtr(marioGetPartyId());
	if (party != NULL) {
		*(s16*)(party->field_22 + 8) = wp->unk4C;
	}
	return wp->unk4C;
}

BOOL marioBgmodeChk(void) {
	return (mp->flags >> 25) & 1;
}

void marioBgmodeOn(void) {
	mp->flags |= MARIO_FLAG_BG_MODE;
	marioSetSpec();
	camFollowYOff();
}

void marioBgmodeOff(void) {
	mp->flags &= ~MARIO_FLAG_BG_MODE;
	marioSetSpec();
	camFollowYOn();
}

BOOL marioChkSts(s32 flags) {
	MarioWork* wp = mp;
	BOOL result = FALSE;

	if (flags & 1) {
		if (wp->flags & MARIO_FLAG_IS_JUMPING) {
			result = TRUE;
		}
	}
	if (flags & 2) {
		if (wp->flags & MARIO_FLAG_IS_FALLING) {
			result = TRUE;
		}
	}
	if (flags & 4) {
		if (wp->flags & MARIO_FLAG_PAPER_MODE) {
			result = TRUE;
		}
	}
	if (flags & 0x20) {
		if (wp->currMotionId == MARIO_MOTION_KAZE) {
			result = TRUE;
		}
	}
	if (flags & 0x10) {
		if (wp->currMotionId == MARIO_MOTION_ROLL) {
			result = TRUE;
		}
	}
	if (flags & 8) {
		if (wp->currMotionId == MARIO_MOTION_JABARA) {
			result = TRUE;
		}
	}
	return result;
}

void marioResetHitObj(const char* name) {
	MarioWork* wp = mp;

	if (*(void**)((u8*)wp + 0x1dc) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1dc)), name)) {
		*(void**)((u8*)wp + 0x1dc) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1e0) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1e0)), name)) {
		*(void**)((u8*)wp + 0x1e0) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1e4) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1e4)), name)) {
		*(void**)((u8*)wp + 0x1e4) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1e8) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1e8)), name)) {
		*(void**)((u8*)wp + 0x1e8) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1ec) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1ec)), name)) {
		*(void**)((u8*)wp + 0x1ec) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1f0) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1f0)), name)) {
		*(void**)((u8*)wp + 0x1f0) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1f4) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1f4)), name)) {
		*(void**)((u8*)wp + 0x1f4) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1f8) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1f8)), name)) {
		*(void**)((u8*)wp + 0x1f8) = NULL;
	}
	if (*(void**)((u8*)wp + 0x1fc) != NULL && !strcmp(hitGetName(*(HitEntry**)((u8*)wp + 0x1fc)), name)) {
		*(void**)((u8*)wp + 0x1fc) = NULL;
	}
}

// The original calls marioOfsRotReset(), whose body the compiler inlined here; mirror that with an inline helper
// so the four zero-vector stores are emitted at the call site.
static inline void marioOfsRotResetInline(void) {
	MarioWork* wp = mp;

	*(Vec*)(wp->field_0x98 + 0x24) = vec3_802c3698;
	*(Vec*)(wp->field_0x98 + 0x18) = vec3_802c36a4;
	*(Vec*)(wp->field_0x98 + 0x00) = vec3_802c36b0;
	*(Vec*)(wp->field_0x98 + 0x0C) = vec3_802c36bc;
}

// NONMATCHING (91.09%): the compiler hoists the zero-vector loads of the inlined marioOfsRotReset ahead of the
// dispFlags clears; every other instruction matches. The clears are split around the copy to steer that schedule.
void marioReset(void) {
	MarioWork* wp = mp;
	MarioWork* wp2;

	if (wp->characterId == 1) {
		wp->wPlayerCollisionBox.x = 20.0f;
		wp->wPlayerCollisionBox.y = 47.0f;
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.0f;
	}
	else if (wp->characterId == 2) {
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.25f;
	}
	else {
		if (!(wp->flags & MARIO_FLAG_BG_MODE)) {
			wp->wPlayerCollisionBox.x = 20.0f;
			wp->wPlayerCollisionBox.y = 37.0f;
		}
		else {
			wp->wPlayerCollisionBox.x = 10.0f;
			wp->wPlayerCollisionBox.y = 25.9f;
		}
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.25f;
	}

	wp->flags &= ~MARIO_FLAG_PAPER_MODE;
	wp->flags &= ~MARIO_FLAG_TUBE_MODE;

	wp2 = mp;

	if ((wp2->dispFlags & 0x40000000) && wp2->paperAnimName != NULL) {
		animPoseSetPaperAnimGroup(wp2->paperAnimGroupId[0], NULL, FALSE);
		wp2->paperAnimName = NULL;
		wp2->dispFlags &= ~0x40000000;
		wp2 = mp;
		if (wp2->paperPoseId >= 0) {
			animPaperPoseRelease(wp2->paperPoseId);
			wp2->paperPoseId = -1;
		}
	}

	wp2 = mp;
	if ((wp2->dispFlags & 0x40000000) && !(wp2->dispFlags & 0x2) && !(wp2->flags & MARIO_FLAG_IS_8_BIT)) {
		if (wp2->paperAnimName == NULL || strcmp(wp2->paperAnimName, NULL) != 0) {
			wp2->paperAnimName = NULL;
			wp2->poseTime = 0;
			wp2->trigFlags |= MARIO_TRIG_FLAG_CHG_PAPER_ANIM;
		}
	}

#ifdef MUST_MATCH
	// MUST_MATCH: this split of the two clears around the inlined copy is what makes the compiler keep
	// the copy's loads after both clears, as the original does; the natural order is under #else.
	wp->dispFlags &= ~0x8;
	marioOfsRotResetInline();
	wp->dispFlags &= ~0x100;
#else
	wp->dispFlags &= ~0x8;
	wp->dispFlags &= ~0x100;
	marioOfsRotResetInline();
#endif
	wp->baseSpeed = 0.0f;
	marioChgMot(0);
}

// The four Vec stores and loads match; only the `lwz r5, mp` slot differs Known-hard
// load-hoist class: scheduling on hoists the next vector's load past the previous store, scheduling off
// puts `lwz mp` after the pool `addi`.
#pragma scheduling off // target does not hoist the next vector's load past the previous store
// NONMATCHING (90.50%): `lwz r5, mp` lands after `addi r4` instead of between `lis` and `addi`.
void marioOfsRotReset(void) {
	u8* base = (u8*)mp;

	*(Vec*)(base + 0xBC) = vec3_802c3698;
	*(Vec*)(base + 0xB0) = vec3_802c36a4;
	*(Vec*)(base + 0x98) = vec3_802c36b0;
	*(Vec*)(base + 0xA4) = vec3_802c36bc;
}
#pragma scheduling reset

void marioSoundInit(void) {
	s32* p = (s32*)(mp->field_0x250 + 0x28);

	p[0] = -1;
	p[1] = -1;
	p[2] = -1;
}

void marioPoseInit(void) {
	MarioWork* wp = mp;

	wp->paperAnimGroupId[0] = animPoseEntry(marioAnimeGroupData[mp->colorId][0], 2);
	wp->paperAnimGroupId[1] = animPoseEntry(marioAnimeGroupData[mp->colorId][1], 2);
	wp->paperAnimGroupId[2] = animPoseEntry(marioAnimeGroupData[mp->colorId][2], 2);
}

// NONMATCHING (96.16%): complete; the only diffs are the scheduler evaluating the two named rodata
// Vec copies (mp->scale, mp->bottomlessResetPosition) source-first instead of destination-first.
// Anonymous (Vec){...} literals match, but would move .rodata shared with other functions.
void marioInit(void) {
	MarioWork* wp;
	s32 i;

	mp = (MarioWork*)__memAlloc(HEAP_DEFAULT, 0x2e0);
	memset(mp, 0, 0x2e0);
	wp = mp;
	*(s32*)((u8*)wp + 0x278) = -1;
	*(s32*)((u8*)wp + 0x27c) = -1;
	*(s32*)((u8*)wp + 0x280) = -1;
	pouchInit();
	marioResetGravity();
	marioInitCamId();
	mp->colorId = 0;
	animGroupBaseAsync(marioAnimeGroupData[mp->colorId][0], 2, NULL);
	animGroupBaseAsync(marioAnimeGroupData[mp->colorId][1], 2, NULL);
	animGroupBaseAsync(marioAnimeGroupData[mp->colorId][2], 2, NULL);
	mp->paperAnimGroupId[0] = -1;
	mp->paperAnimGroupId[1] = -1;
	mp->paperAnimGroupId[2] = -1;
	mp->paperPoseId = -1;
	if (mp->characterId == 0) {
		mp->mBaseWalkSpeed = 1.0f;
		mp->mBaseDashSpeed = 2.25f;
	}
	else {
		mp->mBaseWalkSpeed = 1.0f;
		mp->mBaseDashSpeed = 2.0f;
	}
	mp->currPartySlotId[0] = -1;
	mp->currPartySlotId[1] = -1;
	mp->wPlayerCollisionBox.x = 20.0f;
	mp->wPlayerCollisionBox.y = 37.0f;
	mp->scale = vec3_802c36c8;
	*(Vec*)((u8*)mp + 0x110) = mp->position;
	mp->wPlayerCollisionBox.z = 20.0f;
	marioChgPose("M_S_1");
	*(s32*)((u8*)mp + 0x20) = 0;
	mp->bottomlessResetPosition = vec3_802c36d4;
	marioChgMot(0);
	marioBaloonInit();
	marioGetRubInit();
	blurPosId = 0;
	for (i = 0; i < 8; i++) {
		blurPos[i] = mp->position;
	}
	marioResetCamShift();
	marioResetCamShiftRate();
	marioResetCamFollowRate();
	if ((s32)pouchCheckItem(ITEM_HAMMER) == 0) {
		pouchGetItem(ITEM_HAMMER);
	}
	partyInit();
}

// The declaration order below steers MWCC's allocator to the target's registers.
// NONMATCHING (92.67%): only camId/motionId/partySlot0/partySlot1/animGroup0 still land in the
// wrong callee-saved register and the scale/bottomless Vec word copies schedule r3/r0 one step
// differently; everything else, all constants and all calls match.
void marioReInit(void) {
	s32 animGroup1;
	s32 animGroup2;
	MarioWork* wp;
	s32 animGroup0;
	s32 partySlot1;
	s32 trigFlags;
	s32 peachFlags;
	u16 motionId;
	s32 camId;
	s32 partySlot0;
	s32 characterId;
	s32 highFlag;
	s32 paperPoseId;
	s32 prevParty0;
	s32 prevParty1;
	s32 field28C;
	f32 unk1A8;
	f32 unk1AC;
	f32 dirView;
	f32 unk19C;
	s32 i;

	wp = mp;
	highFlag = wp->flags & 0x80000000;
	camId = marioGetCamId();
	paperPoseId = wp->paperPoseId;
	characterId = wp->characterId;
	partySlot0 = wp->currPartySlotId[0];
	partySlot1 = wp->currPartySlotId[1];
	peachFlags = wp->peachFlags;
	field28C = *(s32*)wp->field_0x28C;
	trigFlags = wp->trigFlags;
	motionId = wp->currMotionId;
	unk1A8 = wp->unk1A8;
	unk1AC = *(f32*)wp->unk1AC;
	dirView = wp->directionView;
	unk19C = *(f32*)wp->unk19C;
	prevParty0 = wp->prevPartyMemberId[0];
	prevParty1 = wp->prevPartyMemberId[1];
	animGroup0 = wp->paperAnimGroupId[0];
	animGroup1 = wp->paperAnimGroupId[1];
	animGroup2 = wp->paperAnimGroupId[2];

	if (wp->paperPoseId >= 0) {
		animPoseSetPaperAnimGroup(wp->paperAnimGroupId[0], NULL, 0);
		animPoseRelease(wp->paperPoseId);
		wp->paperPoseId = -1;
		paperPoseId = -1;
	}

	memset(wp, 0, sizeof(MarioWork));

	*(s32*)(mp->field_0x250 + 0x28) = -1;
	*(s32*)(mp->field_0x250 + 0x2C) = -1;
	*(s32*)(mp->field_0x250 + 0x30) = -1;
	if (highFlag != 0) {
		wp->flags |= 0x80000000;
	}

	wp->peachFlags = peachFlags;
	*(s32*)wp->field_0x28C = field28C;
	wp->camId = camId;
	wp->characterId = characterId;
	wp->trigFlags = trigFlags;
	wp->currMotionId = motionId;
	wp->unk1A8 = unk1A8;
	*(f32*)wp->unk1AC = unk1AC;
	wp->directionView = dirView;
	*(f32*)wp->unk19C = unk19C;
	wp->prevPartyMemberId[0] = prevParty0;
	wp->prevPartyMemberId[1] = prevParty1;
	wp->currPartySlotId[0] = partySlot0;
	wp->currPartySlotId[1] = partySlot1;
	wp->paperAnimGroupId[0] = animGroup0;
	wp->paperAnimGroupId[1] = animGroup1;
	wp->paperAnimGroupId[2] = animGroup2;
	wp->paperPoseId = paperPoseId;

	if (wp->characterId == 0) {
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.25f;
	}
	else {
		wp->mBaseWalkSpeed = 1.0f;
		wp->mBaseDashSpeed = 2.0f;
	}

	marioSetSpec();

	wp->flags &= ~1;

	if (wp->paperAnimGroupId[0] >= 0) {
		animPoseSetMaterialFlagOff(wp->paperAnimGroupId[0], -1);
	}
	if (wp->paperAnimGroupId[1] >= 0) {
		animPoseSetMaterialFlagOff(wp->paperAnimGroupId[1], -1);
	}
	if (wp->paperAnimGroupId[2] >= 0) {
		animPoseSetMaterialFlagOff(wp->paperAnimGroupId[2], -1);
	}
	if (wp->paperAnimGroupId[0] >= 0) {
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[0], 2);
	}
	if (wp->paperAnimGroupId[1] >= 0) {
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[1], 2);
	}
	if (wp->paperAnimGroupId[2] >= 0) {
		animPoseSetMaterialLightFlagOn(wp->paperAnimGroupId[2], 2);
	}

	wp->scale = vec3_802c36e0;
	wp->bottomlessResetPosition = vec3_802c36ec;

	partyReInit();
	marioBaloonInit();
	marioGetRubInit();

	blurPosId = 0;
	for (i = 0; i < 8; i++) {
		blurPos[i] = mp->position;
	}

	if (wp->characterId == 0) {
		marioSetCharMode(0);
	}
	if (wp->characterId == 2) {
		marioSetCharMode(2);
	}

	marioResetCamShift();
	marioResetCamShiftRate();
	marioResetCamFollowRate();

	if ((s32)pouchCheckItem(ITEM_HAMMER) == 0) {
		pouchGetItem(ITEM_HAMMER);
	}

	*(Vec*)(wp->field_0xD4 + 0xC) = vec3_802c36f8;
}

// MUST_MATCH: the original builds the coin position in an inlined struct return, so the
// temporary lives at r1+0x14 and the by-pointer argument copy at r1+0x20.
static inline Vec marioCoinPos(f32 x, f32 y, f32 z) {
	Vec v = vec3_802c3704;

	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

// MUST_MATCH: the blur history update is inlined in the original, and every copy re-reads mp.
static inline void marioUpdateBlurPos(void) {
	if ((*(u32*)(mp->field_0x32 + 2) & 1) && (mp->dispFlags & 0x08000000)) {
		s32 id = blurPosId;
		s32 next = id + 1;
		Vec* p = &blurPos[id];

		*p = mp->position;
		blurPosId = next;
		if (next >= 8) {
			blurPosId = next - 8;
		}
	}
}

// NONMATCHING (97.97%): scratch-register allocation in the blur-history copies (blurPosId in r6 here, r7 in
// the original) and the unit's mp reload placement; the rest are .sdata2 symbol names.
void marioMain(void) {
	MarioWork* wp = mp;
	f32 groundZ;
	f32 groundX;
	f32 groundY;

	if (!(wp->flags & MARIO_FLAG_ACTIVE)) {
		return;
	}

	if (((u16)gp->buttonNew[0] & 0x100f) ? 1 : *(s8*)(wp->field_0x32 + 6)) {
		if (wp->characterId == 0) {
			BOOL badge1 = pouchEquipCheckBadge(0x147);
			BOOL badge2 = pouchEquipCheckBadge(0x148);
#ifdef MUST_MATCH
			s32 color = marioCharModeColor(badge1, badge2);
#else
			s32 color;

			if (badge1 && !badge2) {
				color = 1;
			}
			else if (!badge1 && badge2) {
				color = 2;
			}
			else if (badge1 && badge2) {
				color = 3;
			}
			else {
				color = 0;
			}
#endif

			if (mp->characterId == 0 && mp->colorId != color) {
				mp->colorId = color;
				marioSetCharMode(0);
			}
		}

		{
			MarioWork* p = mp;

			if (pouchEquipCheckBadge(0x143) != 0) {
				if (p->currMotionId == 0) {
					p->trigFlags |= 0x1000;
				}
				else if (p->currMotionId == 1) {
					p->trigFlags |= 0x1000;
				}
				else if (p->currMotionId == 2) {
					marioChgMot(1);
					p->trigFlags |= 0x1000;
				}
			}
		}
	}

	marioSetDirEventMain();
	wp->playerGravity = 1.0f;
	if (strcmp(gp->currentMapName, "moo_00") == 0
	    || strcmp(gp->currentMapName, "moo_01") == 0
	    || strcmp(gp->currentMapName, "moo_02") == 0
	    || strcmp(gp->currentMapName, "moo_05") == 0
	    || strcmp(gp->currentMapName, "moo_06") == 0
	    || strcmp(gp->currentMapName, "moo_07") == 0) {
		wp->playerGravity = 0.5f;
	}

	marioPreCamera();
	marioHitCheckWidth();

	if (wp->unk4C > 0) {
		wp->unk4C--;
	}
	if (*(s16*)(wp->field_0x2AC + 0x14) > 0) {
		*(s16*)(wp->field_0x2AC + 0x14) -= 1;
		if (*(s16*)(wp->field_0x2AC + 0x14) % 30 == 0 && *(s16*)(wp->field_0x2AC + 0x14) > 0) {
			Vec pos = marioCoinPos(wp->position.x, wp->position.y + 30.0f, wp->position.z);

			itemCoinDrop(&pos);
		}
	}
	if (wp->unk5E > 0 && --wp->unk5E <= 0) {
		wp->dispFlags &= ~0x100000;
	}

	if (wp->currMotionId == MARIO_MOTION_DOKAN) {
		marioMotion();
		mp->hitobjStandOn = marioSearchGround(&groundY, &groundX, &groundZ, 0.0f, 0.0f);
		marioPreDisp();
		partyMain();
		marioCamMain();
		marioUpdateBlurPos();
		{
			MarioWork* p = mp;
			*(s16*)(p->field_0x52 + 2) = p->airTimer;
			*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
			if (pouchGetHP() <= 0) {
				seqSetSeq(5, NULL, NULL);
			}
			*(Vec*)(p->unk104 + 0xC) = p->position;
			p->riderDelta.x = 0.0f;
			p->riderDelta.y = 0.0f;
			p->riderDelta.z = 0.0f;
			*(u32*)(p->field_0x32 + 2) += 1;
		}
		return;
	}

	if (wp->flags & MARIO_FLAG_CTRL_OFF) {
		if (wp->flags & MARIO_FLAG_CTRL_OFF_2) {
			marioCtrlOff2Main();
		}
	}
	else {
		marioUpdateKeyData();
		marioGetStick(&wp->controlStickAngle, &wp->controlStickSensitivity);
		marioUseParty();
		marioMotion();

		if (wp->characterId == 2) {
			kpaMain();
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}
		if (wp->characterId == 1) {
			peachMain();
			return;
		}

		if (wp->currMotionId == MARIO_MOTION_PLANE || (u16)(wp->currMotionId - MARIO_MOTION_DAMAGE) <= 1 || wp->currMotionId == MARIO_MOTION_BOTTOMLESS) {
			marioSearchUnder();
			marioPreDisp();
			marioBottomless();
			partyMain();
			marioCamMain();
			*(void**)((u8*)wp + 0x1f4) = NULL;
			*(void**)((u8*)wp + 0x1e0) = NULL;
			*(void**)((u8*)wp + 0x1dc) = NULL;
			wp->hitobjPush = NULL;
			*(void**)((u8*)wp + 0x1f0) = NULL;
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}
		else if (wp->currMotionId == MARIO_MOTION_VIVIAN) {
			marioChkGnd();
			marioChkToge();
			marioPreDisp();
			marioBalloonMain();
			marioBottomless();
			partyMain();
			marioCamMain();
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}
		else if (wp->currMotionId == MARIO_MOTION_JABARA) {
			marioMove();
			marioChkToge();
			marioPreDisp();
			marioBalloonMain();
			marioBottomless();
			partyMain();
			marioCamMain();
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}
		else if (wp->currMotionId == MARIO_MOTION_YOSHI) {
			marioChkToge();
			marioBottomless();
			marioPreDisp();
			marioBalloonMain();
			mario_kemuri_80130474();
			partyMain();
			marioCamMain();
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}
		else if (wp->currMotionId == MARIO_MOTION_JUMP_3) {
			marioBottomless();
			marioPreDisp();
			marioBalloonMain();
			mario_kemuri_80130474();
			partyMain();
			marioCamMain();
			marioUpdateBlurPos();
			{
				MarioWork* p = mp;
				*(s16*)(p->field_0x52 + 2) = p->airTimer;
				*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
				if (pouchGetHP() <= 0) {
					seqSetSeq(5, NULL, NULL);
				}
				*(Vec*)(p->unk104 + 0xC) = p->position;
				p->riderDelta.x = 0.0f;
				p->riderDelta.y = 0.0f;
				p->riderDelta.z = 0.0f;
				*(u32*)(p->field_0x32 + 2) += 1;
			}
			return;
		}

		if (wp->flags & 0x40000) {
			marioMove();
			marioChkOverhead();
			if (wp->airTimer > 100) {
				marioChgMot(MARIO_MOTION_FALL);
				marioSetFallPara();
			}
		}
		else if (wp->flags & 0x10000) {
			marioJump();
			marioChkOverhead();
			marioMove();
			{
				BOOL isRoll = FALSE;

				if (mp->currMotionId == MARIO_MOTION_ROLL) {
					isRoll = TRUE;
				}
				if (!isRoll) {
					if (wp->airTimer > 5) {
						marioChkGnd2();
					}
				}
			}
		}
		else if (wp->flags & 0x20000) {
			if (wp->currMotionId != MARIO_MOTION_SHIP) {
				marioMove();
				if (wp->currMotionId == MARIO_MOTION_HIP) {
					f32 overheadY = marioChkOverhead();

					if (*(void**)((u8*)wp + 0x1f4) != NULL) {
						wp->position.y = overheadY;
					}
				}
				else {
					marioChkOverhead();
				}
				marioFall();
				if (wp->currMotionId != MARIO_MOTION_HIP && wp->currMotionId != MARIO_MOTION_HIP_2) {
					BOOL isRoll = FALSE;

					if (mp->currMotionId == MARIO_MOTION_ROLL) {
						isRoll = TRUE;
					}
					if (!isRoll && wp->airTimer > 5) {
						marioChkGnd2();
					}
				}
				else {
					marioChkGnd2();
				}
			}
		}
		else {
			marioMove();
			marioChkOverhead();
			marioChkGnd();
		}
		marioBoots();
	}

	marioForceMoveMain();
	marioBottomless();
	marioPreDisp();
	marioBalloonMain();
	mario_kemuri_80130474();
	partyMain();
	marioCamMain();
	if ((*(u32*)(wp->field_0x32 + 2) & 1) && (wp->dispFlags & 0x08000000)) {
		s32 id = blurPosId;
		Vec* p = &blurPos[id];

		p->x = wp->position.x;
		p->y = wp->position.y;
		p->z = wp->position.z;
		blurPosId = id + 1;
		if (blurPosId >= 8) {
			blurPosId = id + 1 - 8;
		}
	}
	{
		MarioWork* p = mp;
		*(s16*)(p->field_0x52 + 2) = p->airTimer;
		*(void**)((u8*)p + 0x208) = p->hitobjStandOn;
		if (pouchGetHP() <= 0) {
			seqSetSeq(5, NULL, NULL);
		}
		*(Vec*)(p->unk104 + 0xC) = p->position;
		p->riderDelta.x = 0.0f;
		p->riderDelta.y = 0.0f;
		p->riderDelta.z = 0.0f;
		*(u32*)(p->field_0x32 + 2) += 1;
	}
}

// NONMATCHING (89.49%): 15 instructions differ, plus register differences.
void marioCtrlOff2Main(void) {
	MarioWork* wp = mp;
	s32 flags;

	if (wp->currMotionId != MARIO_MOTION_PLANE && wp->currMotionId != MARIO_MOTION_SHIP
	    && (u16)(wp->currMotionId - MARIO_MOTION_DAMAGE) > 2 && wp->currMotionId != MARIO_MOTION_JABARA
	    && wp->currMotionId != MARIO_MOTION_YOSHI && wp->currMotionId != MARIO_MOTION_JUMP_3
	    && wp->currMotionId != MARIO_MOTION_JUMP_STAND) {
		if (vivianGetStatus() != 0) {
			return;
		}
		marioMotion();
		flags = wp->flags;
		if (flags & 0x40000) {
			marioMove();
			marioChkOverhead();
			if (wp->airTimer > 100) {
				marioChgMot(MARIO_MOTION_FALL);
				marioSetFallPara();
			}
			return;
		}
		if (flags & 0x10000) {
			marioJump();
			marioChkOverhead();
			marioMove();
			if (mp->currMotionId == MARIO_MOTION_ROLL) {
				return;
			}
			if (wp->airTimer <= 5) {
				return;
			}
			marioChkGnd2();
			return;
		}
		if (flags & 0x20000) {
			if (wp->currMotionId == MARIO_MOTION_SHIP) {
				return;
			}
			marioMove();
			if (wp->currMotionId == MARIO_MOTION_HIP) {
				f32 overheadY = marioChkOverhead();
				if (*(void**)((u8*)wp + 0x1f4) != NULL) {
					wp->position.y = overheadY;
				}
			}
			else {
				marioChkOverhead();
			}
			marioFall();
			if (wp->currMotionId == MARIO_MOTION_HIP || wp->currMotionId == MARIO_MOTION_HIP_2
			    || (mp->currMotionId != MARIO_MOTION_ROLL && wp->airTimer > 5)) {
				marioChkGnd2();
			}
			marioChkWallAround(&wp->position, 0, 0.0f, 0.0f, wp->wPlayerCollisionBox.z, wp->wPlayerCollisionBox.y);
		}
		else {
			marioChkGnd();
		}
	}
}

#ifdef MUST_MATCH
// The original expands the first marioGetScreenPos call in an earlier inlining phase, so its frame sits at the
// low addresses (0x44/0x60); routing it through this static inline reproduces that split. Plain call is natural.
static inline void marioScreenPos(Vec position, f32* x, f32* y, f32* z) {
	marioGetScreenPos(position, x, y, z);
}
#endif

void marioMove(void) {
	MarioWork* wp = mp;
	Vec lastPosition = wp->position;
	// Declaration order is load-bearing for the frame layout (see party_force_reset_outofscreen): the
	// compiler lays these scalars out in reverse, which is what gives x at 0x8 and sx at 0x1c.
	f32 sx;
	f32 sy;
	f32 sz;
	f32 z;
	f32 y;
	f32 x;
	// `zero` (not the literal) makes MWCC keep the field in f1 for the `!=` compares, as in retail.
	f32 zero = 0.0f;
	s32 stickDir;

	marioMoveMain();

	{
		MarioWork* wp2 = mp;

#ifdef MUST_MATCH
		marioScreenPos(wp2->position, &x, &y, &z);
#else
		marioGetScreenPos(wp2->position, &x, &y, &z);
#endif
		wp2->unk168 = x;
		wp2->unk16C = y;
		*(s32*)wp2->unk170 = z;
	}

	if (wp->dispFlags & 0x100000) {
		if (wp->controlStickSensitivity != zero || !(wp->dispFlags & 0x1000000)) {
			marioCamZoomOff();
		}
	}

	if (wp->flags & 0x2000000) {
		if (strcmp(gp->currentMapName, "aji_13") != 0) {
			if (wp->controlStickSensitivity != zero && !(wp->dispFlags & 0x20000)) {
				camFollowYOff();
			}
		}
		else {
			marioGetScreenPos(wp->position, &sx, &sy, &sz);
			if (sy >= 400.0f) {
				camFollowYOn();
			}
		}

		if (wp->currMotionId == MARIO_MOTION_WALK || wp->currMotionId == MARIO_MOTION_DASH) {
			if (lastPosition.x - wp->position.x != 0.0f) {
				if (PSVECDistance(&lastPosition, &wp->position) <= 1.0f) {
					marioChgPose("M_W_1");
				}
				else {
					marioChgPose("M_R_1");
				}
			}
			else {
				// wStickDir1 is signed in retail (lbz + extsb), but the header declares it u8.
				stickDir = (s8)wp->wStickDir1;
				if (stickDir * stickDir < 25) {
					marioChgPose("M_Z_1");
				}
				else if (stickDir * stickDir < 3025) {
					marioChgPose("M_W_1");
				}
				else {
					marioChgPose("M_R_1");
				}
			}
		}
	}
}

extern s32 __float_nan[];
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5
#define fpclassify(x) marioMoveMain_fpclassify(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline s32 marioMoveMain_fpclassify(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
	case 0x7F800000:
		if (*(s32*)&x & 0x007FFFFF) {
			return FP_NAN;
		} else {
			return FP_INFINITE;
		}
	case 0:
		if (*(s32*)&x & 0x007FFFFF) {
			return FP_SUBNORMAL;
		} else {
			return FP_ZERO;
		}
	default:
		return FP_NORMAL;
	}
}

static inline f32 marioMoveMain_sqrtf(f32 x) {
	const f64 _half = .5;
	const f64 _three = 3.0;

	if (x > 0.0f) {
		f64 guess = __frsqrte(x);

		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		guess = _half * guess * (_three - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return NAN;
	} else if (isnan(x)) {
		return NAN;
	}
	return x;
}

// MWCC allocates f29 to the x-accumulator and f30 to the speed (the original is the reverse), so ~28
// instructions differ only in those two registers; the three inline-sqrtf double offsets (0x330 vs the
// original 0x340) are a unit-wide .rodata layout difference that resolves once the rest of the unit is
// written; and N_dou10_yoko_yari3 is missing one `addi r3, wp+0x210` because its header prototype is
// declared (void).
// NONMATCHING (98.99%): register allocation of the x-accumulator/speed floats and the missing addi noted above.
void marioMoveMain(void) {
	MarioWork* wp = mp;
	PartyEntry* party;
	HitEntry* hit;
	f32 f30;
	f32 f31;
	f32 f29;
	f32 sinVal;
	f32 cosVal;
	f32 cap;
	f32 zero;

	if (wp->currMotionId != MARIO_MOTION_SHIP) {
		if (wp->flags & 0x01000000) {
			*(void**)((u8*)wp + 0x1F4) = NULL;
		}
		*(void**)((u8*)wp + 0x1E0) = NULL;
		*(void**)((u8*)wp + 0x1DC) = NULL;
		*(void**)((u8*)wp + 0x1EC) = NULL;
		*(void**)((u8*)wp + 0x1F0) = NULL;
	}
	if (wp->flags & MARIO_FLAG_CTRL_OFF) {
		return;
	}
	if (wp->currMotionId == MARIO_MOTION_JUMP_3) {
		return;
	}
	if (getRollEvtFlag() != 0) {
		return;
	}

	if (wp->flags & MARIO_FLAG_FORCED_MOVEMENT) {
		if (wp->currMotionId == MARIO_MOTION_YOSHI) {
			party = partyGetPtr(marioGetPartyId());
			*(f32*)wp->unk19C = wp->directionView;
			movePos(&party->position.x, &party->position.z, wp->baseSpeed, wp->directionView);
			wp->position.x = party->position.x;
			wp->position.z = party->position.z;
			return;
		} else if (wp->currMotionId == MARIO_MOTION_ROLL) {
			marioSetRollDispDir();
			movePos(&wp->position.x, &wp->position.z, wp->baseSpeed, wp->directionView);
			return;
		} else if (wp->currMotionId == MARIO_MOTION_SHIP) {
			marioShipSetDispDir();
			movePos(&wp->position.x, &wp->position.z, wp->baseSpeed, wp->directionView);
			return;
		} else if (wp->currMotionId != MARIO_MOTION_SLIT) {
			*(f32*)wp->unk19C = wp->directionView;
			movePos(&wp->position.x, &wp->position.z, wp->baseSpeed, wp->directionView);
			return;
		} else {
			movePos(&wp->position.x, &wp->position.z, wp->baseSpeed, wp->directionView);
			return;
		}
	}

	if (wp->currMotionId == MARIO_MOTION_SHIP) {
		return;
	}
	if (L_marioChkRub() != 0) {
		return;
	}

	f30 = 0.0f;
	f31 = f30;
	if (!(wp->flags & MARIO_FLAG_STOP_AIR_MOVEMENT) && wp->airTimer != 0) {
		f29 = wp->controlStickSensitivity * wp->playerGravity;
		if (wp->flags & MARIO_FLAG_PAPER_MODE) {
			if (wp->currMotionId != MARIO_MOTION_JUMP_STAND) {
				f29 = f30;
			} else {
				f29 *= 0.015f;
			}
		} else if (pouchEquipCheckBadge(0x143)) {
			f29 *= 0.01f;
		} else {
			s32 isRoll = FALSE;

			if (marioGetPtr()->currMotionId == MARIO_MOTION_ROLL) {
				isRoll = TRUE;
			}
			if (isRoll == TRUE) {
				f29 *= 0.01f;
			} else {
				f29 *= 0.015f;
				if (!(wp->flags & (MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING | MARIO_FLAG_IS_STEPPING))) {
					f29 *= 0.25f;
				}
				if (wp->flags & MARIO_FLAG_BG_MODE) {
					f29 *= 0.4f;
				}
			}
		}
		sincosf(&sinVal, &cosVal, wp->controlStickAngle);
		f30 = f29 * sinVal;
		f31 = f29 * cosVal;
	}

	f29 = wp->baseSpeed;
	if (wp->hitobjStandOn != NULL) {
		if (hitGetAttr((HitEntry*)wp->hitobjStandOn) & 0x40000000) {
			*(void**)((u8*)wp + 0x204) = wp->hitobjStandOn;
		}
		if (hitGetAttr((HitEntry*)wp->hitobjStandOn) & 0x100) {
			wp->flags |= 0x04000000;
			f29 *= 0.75f;
		} else {
			wp->flags &= ~0x04000000;
		}
	} else {
		wp->flags &= ~0x04000000;
	}

	sincosf(&sinVal, &cosVal, wp->directionView);
	f30 = f29 * sinVal + f30;
	f29 = f29 * cosVal + f31;
	if (!(wp->flags & (0x02000000 | MARIO_FLAG_HAS_INPUT_JUMP))) {
		if (*(f32*)(wp->field_0x1F0 + 0x30) != 0.0f) {
			f30 *= (f32)cos(3.1415927f * *(f32*)(wp->field_0x1F0 + 0x24) / 180.0f);
		}
		if (*(f32*)(wp->field_0x1F0 + 0x28) != 0.0f) {
			f29 *= (f32)cos(3.1415927f * *(f32*)(wp->field_0x1F0 + 0x24) / 180.0f);
		}
	}

	zero = 0.0f;
	f29 = f29 + wp->riderDelta.z;
	f30 = f30 + wp->riderDelta.x;
	f31 = f30 * f30 + f29 * f29;
	wp->position.y = wp->position.y + (zero + wp->riderDelta.y);
	f31 = marioMoveMain_sqrtf(f31);

	if (f31) {
		f29 = angleABf(0.0f, 0.0f, f30, f29);
	} else {
		f29 = toMovedir(wp->unk1A8);
	}

	if (!(wp->flags & MARIO_FLAG_PAPER_MODE)) {
		if (wp->flags & (MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING)) {
			cap = wp->mBaseDashSpeed * wp->playerGravity;
			if (f31 > cap) {
				f31 = cap;
			}
		}
	}
	if (!(wp->flags & 0x01000000)) {
		f31 *= marioGetMoveRate(*(f32*)wp->unk19C);
	}
	if (wp->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) {
		f31 = 0.0f;
	}
	*(f32*)wp->unk18C = f31;

	if (wp->flags & MARIO_FLAG_USE_FORCE_DIRECTION) {
		f29 = wp->unk1A4;
		wp->directionView = f29;
	} else if (wp->flags & MARIO_FLAG_WALLS_DISABLED) {
		f29 = wp->directionView;
	}

	if (wp->flags & MARIO_FLAG_WALLS_DISABLED) {
		movePos(&wp->position.x, &wp->position.z, wp->baseSpeed, f29);
		return;
	}

	if (!(wp->flags & 0x01000000)) {
		marioCheckWall(f31, f29);
	} else {
		if (!(wp->flags & MARIO_FLAG_IS_JUMPING)) {
			marioCheckWall(f31, f29);
		} else {
			marioCheckWall_roll(f31, f29);
		}
		N_dou10_yoko_yari3();
		hit = (HitEntry*)*(void**)((u8*)wp + 0x1E0);
		if (hit != NULL && (hitGetAttr(hit) & 0x800)) {
			set_damage_root_ypos(wp->position.y);
			marioChgMot(MARIO_MOTION_DAMAGE_TOGE);
			return;
		}
	}
	marioChkToge();
}

BOOL marioChkPushAnime(void) {
	MarioWork* wp = mp;
	BOOL valid = FALSE;
	if (!strcmp(wp->animName, "M_O_1") || !strcmp(wp->animName, "M_O_2") || !strcmp(wp->animName, "M_O_2R")) {
		valid = TRUE;
	}
	return valid;
}

s32 marioAnimeId(void) {
	s32 flags = mp->dispFlags;
	s32 id;

	if (flags & 0x10000000) {
		id = 2;
	}
	else if (flags & 0x80000000) {
		id = 1;
	}
	else {
		id = 0;
	}
	return id;
}

void unk_80058374(void) {
	MarioWork* wp = mp;

	if (wp->currMotionId != MARIO_MOTION_SHIP && wp->currMotionId != MARIO_MOTION_ROLL && wp->currMotionId != MARIO_MOTION_PLANE && wp->currMotionId != MARIO_MOTION_JABARA && wp->currMotionId != MARIO_MOTION_YOSHI) {
#ifdef MUST_MATCH
		// original layout is a one-case switch: an `if` compiles to `bne end`, this to `beq body; b end`
		switch (wp->dispFlags & 0x1000000) {
		case 0:
			marioChgPose("M_D_1"); //inline
			break;
		}
#else
		if (!(wp->dispFlags & 0x1000000)) {
			marioChgPose("M_D_1"); //inline
		}
#endif
	}
}

void marioChgEvtPose(const char* name) {
	MarioWork* wp = mp;
	const char* animName;

	if (!(mp->dispFlags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->animName;
		if (!animName || strcmp(animName, name)) {
			wp->animName = name;
			wp->dispFlags &= ~0x20000000;
			wp->dispFlags |= 0x10000000;
			wp->trigFlags |= 0x1000;
		}
	}
}

const char* toDotMarioPose(const char* name) {
	const char* dot = NULL;

	if (!strcmp("M_Z_1", name)) {
		dot = dotMarioPose[mp->colorId][0];
	}
	else if (!strcmp("M_S_1", name)) {
		dot = dotMarioPose[mp->colorId][1];
	}
	else if (!strcmp("M_W_1", name)) {
		dot = dotMarioPose[mp->colorId][2];
	}
	else if (!strcmp("M_R_1", name)) {
		dot = dotMarioPose[mp->colorId][3];
	}
	else if (!strcmp("M_J_1B", name)) {
		dot = dotMarioPose[mp->colorId][5];
	}
	else if (!strcmp("M_J_1C", name)) {
		dot = dotMarioPose[mp->colorId][5];
	}

	return dot;
}

void marioChgPose(const char* name) {
	MarioWork* wp = mp;
	const char* dot;
	const char* animName;

	if (!(wp->dispFlags & 2)) {
		animName = wp->animName;
		if (!animName || strcmp(animName, name)) {
			if (!(wp->flags & 0x80000000)) { //TODO: flip {}?
				wp->animName = name;
				wp->poseTime = 0;
				wp->dispFlags &= ~0x30000000;
				wp->trigFlags |= 0x1000;
			}
			else {
				dot = toDotMarioPose(name); //inline, TODO
				if (dot) {
					wp->animName = dot;
					wp->poseTime = 0;
					wp->dispFlags &= ~0x30000000;
					wp->trigFlags |= 0x1000;
				}
			}
		}
	}
}

void marioChgPoseTime(const char* name, s16 frames) {
	MarioWork* wp = mp;
	const char* animName;

	if (!(mp->dispFlags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->animName;
		if (!animName || strcmp(animName, name)) {
			wp->animName = name;
			wp->poseTime = frames;
			wp->dispFlags &= ~0x30000000;
			wp->trigFlags |= 0x1000;
		}
	}
}

void marioChgPaper(const char* name) {
	MarioWork* wp = mp;
	s32 flags = wp->dispFlags;
	const char* animName;

	if (flags & 0x40000000 && !(flags & 2) && !(mp->flags & 0x80000000)) {
		animName = mp->paperAnimName;
		if (!animName || strcmp(animName, name)) { //if different, do stuff
			wp->paperAnimName = name;
			wp->poseTime = 0;
			wp->trigFlags |= 0x4000;
		}
	}
}

// NONMATCHING (78.52%): 4 missing, 6 extra, plus register differences.
void marioSetPaperAnimeLocalTime(s16 frames) { //f32 cast reuse, flags check
	MarioWork* wp = mp;
	s32 group;

	wp->poseTime = frames;
	animPoseSetLocalTime(wp->paperPoseId, (f32)frames);
	group = marioAnimeId(); //inlined
	animPoseSetLocalTime(wp->paperAnimGroupId[group], (f32)frames);
}

//TODO: move below marioPaperOff when -inline deferred is on
inline void marioPaperRelease(void) {
	MarioWork* wp = mp;
	s32 poseId;

	poseId = wp->paperPoseId;
	if (poseId >= 0) {
		animPaperPoseRelease(poseId);
		wp->paperPoseId = -1;
	}
}

// NONMATCHING (95.91%): 1 missing, 1 extra, plus register differences.
BOOL marioPaperOn(const char* name) { //almost 1:1, some regalloc
	MarioWork* wp = mp;
	BOOL flag;

	marioPaperRelease();
	wp->dispFlags |= 0x40000000; //paper on
	wp->paperPoseId = animPaperPoseEntry(name, 2);
	flag = FALSE;
	if (   !strcmp(name, "p_roll")
		|| !strcmp(name, "p_plane")
		|| !strcmp(name, "p_hune")
		|| !strcmp(name, "p_jyabara")
		|| !strcmp(name, "p_dokan_x")
		|| !strcmp(name, "p_dokan_y"))
	{
		flag = TRUE;
	}
	animPoseSetPaperAnimGroup(wp->paperAnimGroupId[0], name, !flag);
	return TRUE;
}

void marioPaperOff(void) { //1:1
	MarioWork* wp = mp;

	if (wp->dispFlags & 0x40000000) {
		if (wp->paperAnimName) {
			animPoseSetPaperAnimGroup(mp->paperAnimGroupId[0], NULL, FALSE);
			wp->paperAnimName = 0;
			wp->dispFlags &= ~0x40000000;
			marioPaperRelease();
		}
	}
}

void marioPaperLightOn(void) { //1:1
	animPoseSetMaterialLightFlagOn(mp->paperAnimGroupId[0], 1);
}

void marioPaperLightOff(void) { //1:1
	animPoseSetMaterialLightFlagOff(mp->paperAnimGroupId[0], 1);
}

void marioGetScreenPos(Vec position, f32* x, f32* y, f32* z) { //1:1
	CameraEntry* camera;
	f32 proj[7];
	f32 view[6];

	camera = camGetPtr(CAMERA_3D);
	GXSetProjection(camera->projection, camera->projType);
	GXGetProjectionv(proj);
	GXGetViewportv(view);
	GXProject(position.x, position.y, position.z, camera->view, proj, view, x, y, z);
}

BOOL marioChkInScreen(s32 x, s32 y) { //1:1
	BOOL valid = FALSE;
	if (x >= 0 && x <= 600 && y >= 0 && y <= 535) {
		valid = TRUE;
	}
	return valid != FALSE; //why
}

f32 marioGetScale(void) { //1:1
	return mp->flags & 0x2000000 ? 1.2f : 2.0f;
}

// Adjusts Mario's display direction (unk1AC target, unk1A8 smoothed) from the
// angle between his facing (unk19C) and the camera yaw (targetYaw), then steers
// unk1A8 toward the target through the trigFlags 0x100/0x200/0x400 turn states.
void marioMakeDispDir(void) {
	MarioWork* wp = mp;
	f32* marioDir = (f32*)((u8*)wp + 0x1AC);
	f32* marioDirView = (f32*)((u8*)wp + 0x19C);

	if (!(wp->dispFlags & 0x01000000)) {
		if (!(wp->dispFlags & 0x100)) {
			f32 angle;
			f32 dir;

			angle = revise360(*marioDirView - wp->targetYaw);
			dir = (s32)(*marioDir + 0.5f);
			if (!(wp->dispFlags & 0x01000000)) {
				if (dir == 290.0f || dir == 250.0f) {
					dir = 180.0f;
					if (angle >= 180.0f) {
						dir = 0.0f;
					}
				}
			}
			if (wp->baseSpeed) {
				if (angle >= 190.0f && angle <= 350.0f) {
					dir = 0.0f;
				}
				else if (angle >= 10.0f && angle <= 170.0f) {
					dir = 180.0f;
				}
			}
			*marioDir = dir;
			if (wp->dispFlags & 0x200) {
				wp->unk1A8 = *marioDir;
			}
			if (wp->unk1A8 == *marioDir) {
				return;
			}
		}
		if (wp->currMotionId == 0x12) {
			if (wp->unk1A8 <= 180.0f) {
				if (*marioDir == 0.0f) {
					wp->unk1A8 = 0.0f;
					wp->trigFlags &= ~0x100;
				}
				else if (*marioDir == 180.0f) {
					wp->unk1A8 = 180.0f;
					wp->trigFlags &= ~0x100;
				}
			}
			else if (wp->unk1A8 > 180.0f) {
				if (*marioDir == 0.0f) {
					wp->unk1A8 = 0.0f;
					wp->trigFlags &= ~0x100;
				}
				else {
					wp->unk1A8 = 180.0f;
					wp->trigFlags &= ~0x100;
				}
			}
		}
		else if (wp->unk1A8 <= 180.0f) {
			f32 dirRounded = (s32)*marioDir;

			if (dirRounded == 0.0f) {
				f32 delta;

				if (wp->trigFlags & 0x400) {
					wp->trigFlags &= ~0x400;
					wp->trigFlags &= ~0x100;
				}
				wp->trigFlags |= 0x200;
				if (wp->trigFlags & 0x100) {
					delta = 0.3f * (10.0f - wp->unk1A8);
				}
				else {
					delta = 0.125f * (-float_30_80419f70 - wp->unk1A8);
				}
				wp->unk1A8 += delta;
				if (!(wp->trigFlags & 0x100)) {
					if (wp->unk1A8 < -float_20_80419fac) {
						wp->trigFlags |= 0x100;
					}
				}
				else {
					if (wp->unk1A8 >= 0.0f) {
						wp->unk1A8 = 0.0f;
						wp->trigFlags &= ~0x100;
					}
				}
			}
			else if (dirRounded == 180.0f) {
				f32 delta;

				if (wp->trigFlags & 0x200) {
					wp->trigFlags &= ~0x200;
					wp->trigFlags &= ~0x100;
				}
				wp->trigFlags |= 0x400;
				if (wp->trigFlags & 0x100) {
					delta = 0.3f * (170.0f - wp->unk1A8);
				}
				else {
					delta = 0.125f * (210.0f - wp->unk1A8);
				}
				wp->unk1A8 += delta;
				if (!(wp->trigFlags & 0x100)) {
					if (wp->unk1A8 > 200.0f) {
						wp->trigFlags |= 0x100;
					}
				}
				else {
					if (wp->unk1A8 <= 180.0f) {
						wp->unk1A8 = 180.0f;
						wp->trigFlags &= ~0x100;
					}
				}
			}
		}
		else if (wp->unk1A8 > 180.0f) {
			if (*marioDir == 0.0f) {
				f32 delta;

				if (wp->trigFlags & 0x400) {
					wp->trigFlags &= ~0x400;
					wp->trigFlags &= ~0x100;
				}
				wp->trigFlags |= 0x200;
				if (wp->trigFlags & 0x100) {
					delta = 0.3f * (10.0f - wp->unk1A8);
				}
				else {
					delta = 0.125f * (-float_30_80419f70 - wp->unk1A8);
				}
				wp->unk1A8 += delta;
				if (!(wp->trigFlags & 0x100)) {
					if (wp->unk1A8 < -float_20_80419fac) {
						wp->trigFlags |= 0x100;
					}
				}
				else {
					if (wp->unk1A8 >= 0.0f) {
						wp->unk1A8 = 0.0f;
						wp->trigFlags &= ~0x100;
					}
				}
			}
			else if (*marioDir == 180.0f) {
				f32 delta;

				if (wp->trigFlags & 0x200) {
					wp->trigFlags &= ~0x200;
					wp->trigFlags &= ~0x100;
				}
				wp->trigFlags |= 0x400;
				if (wp->trigFlags & 0x100) {
					delta = 0.3f * (170.0f - wp->unk1A8);
				}
				else {
					delta = 0.125f * (210.0f - wp->unk1A8);
				}
				wp->unk1A8 += delta;
				if (!(wp->trigFlags & 0x100)) {
					if (wp->unk1A8 > 200.0f) {
						wp->trigFlags |= 0x100;
					}
					else if (wp->unk1A8 <= 180.0f) {
						wp->unk1A8 = 180.0f;
						wp->trigFlags &= ~0x100;
					}
				}
			}
		}
	}
}

void marioRearAnime(void) { //1:1
	MarioWork* wp = mp;
	const char* pose;
	f32 angle;

	if (!(wp->dispFlags & 0x70000004) && !(wp->flags & 0x2000)) {
		angle = revise360(*(f32*)&wp->unk19C[0] - wp->targetYaw);
		if (angle >= 290.0f || angle <= 70.0f) {
			pose = toRearPose(wp->animName);
			if (pose) {
				wp->dispFlags |= 0x80000000;
				marioChgPose(pose);
			}
			else {
				pose = toFrontPose(wp->animName);
				if (!pose) {
					wp->dispFlags &= ~0x80000000;
				}
			}
		}
		else {
			wp->dispFlags &= ~0x80000000;
			pose = toFrontPose(wp->animName);
			if (pose) {
				marioChgPose(pose);
			}
		}
	}
	else if (wp->dispFlags & 0x80000000) {
		wp->dispFlags &= ~0x80000000;
		pose = toFrontPose(wp->animName);
		if (pose) {
			marioChgPose(pose);
		}
	}
}

// NONMATCHING (99.45%): mp is held in r4 then copied to r31 (the target loads r31 directly), and the FPR chosen
// for ofsZ is f3 here, f5 in the target.
void marioPreDisp(void) {
	MarioWork* wp = mp;
	Vec pos;
	f32 size;
	f32 ofsY;
	f32 ofsZ;
	f32 ofsX;
	f32 c;
	f32 diff;
	s32 v;

	if (wp->characterId == 0 || wp->characterId == 1) {
		marioRearAnime();
	}

	if (*(s8*)&wp->field_0x32[0x38 - 0x32] == 0 && !(wp->dispFlags & 0x100)) {
		marioMakeDispDir();
	}

	if (wp->trigFlags & 0x5000) {
		if (wp->trigFlags & 0x1000) {
			wp->trigFlags &= ~0x1000;
		}
		if (wp->trigFlags & 0x4000) {
			wp->trigFlags &= ~0x4000;
		}

		animPoseSetAnim(wp->paperAnimGroupId[marioAnimeId()], wp->animName, 1);
		if (wp->paperAnimName) {
			// header declares this 2-arg, but the target passes r5=1 (same cast as msgdrv.c)
			((void (*)(s32, const char*, BOOL))animPoseSetPaperAnim)(wp->paperAnimGroupId[0], wp->paperAnimName, 1);
		}
		animPoseSetLocalTime(wp->paperAnimGroupId[marioAnimeId()], (f32)*(u16*)&wp->poseTime);
	}

	animPoseMain(wp->paperAnimGroupId[marioAnimeId()]);

	if (wp->dispFlags & 1) {
		return;
	}

	pos = wp->position;
	PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &pos, &pos);

	ofsY = 5000.0f * pos.z + 5000.0f;
	if (ofsY < 0.0f) {
		ofsY = 0.0f;
	}
	else if (ofsY > 10000.0f) {
		ofsY = 10000.0f;
	}
	ofsY *= -1.0f;

	if (!(wp->dispFlags & 0x08000000) || (wp->flags & 0x40000000)) {
		if (wp->characterId == 2) {
			v = kpaNoHitEnemyCheck();
		}
		else {
			v = wp->unk4C;
		}
		if (v > 0) {
			if (!(v & 2)) {
				dispEntry(marioGetCamId(), ofsY, 1, marioDisp, wp);
			}
		}
		else {
			dispEntry(marioGetCamId(), ofsY, 1, marioDisp, wp);
		}
	}
	else {
		dispEntry(marioGetCamId(), ofsY, 2, marioDispBlur, wp);
	}

	if (wp->dispFlags & 0x1000) {
		return;
	}
	if (wp->flags & 0x02000000) {
		return;
	}

	if (wp->currMotionId == MARIO_MOTION_PLANE) {
		shadowEntryMode(2, wp->position.x, *(f32*)wp->field_0x1C0, wp->position.z, 13.333333f);
		return;
	}

	if (wp->characterId == 2) {
		size = 30.0f;
		ofsX = 14.0f * (f32)sin(3.1415927f * wp->targetYaw / 180.0f);
		c = (f32)cos(3.1415927f * wp->targetYaw / 180.0f);
		ofsZ = 14.0f * -c;
	}
	else {
		ofsX = 0.0f;
		size = 13.333333f;
		ofsZ = 0.0f;
	}

	if (wp->hitobjStandOn != NULL) {
		f32 hitSize;
		s32 mode;

		if (wp->flags & 0x02000000) {
			hitSize = 0.6f * size;
		}
		else {
			hitSize = size;
		}
		mode = 2;
		if (wp->flags & 0x02000000) {
			mode = 0;
		}
		shadowEntryMode(mode, wp->position.x + ofsX, wp->position.y, wp->position.z + ofsZ, hitSize);
		return;
	}

	{
		diff = wp->position.y - *(f32*)wp->field_0x1C0;
		if (diff >= -2.0f) {
			size = size - diff / 13.333333f;
			if (size < 0.1f) {
				size = 0.0f;
			}
			if (size) {
				s32 mode = 2;
				if (wp->flags & 0x02000000) {
					mode = 0;
				}
				shadowEntryMode(mode, wp->position.x + ofsX, wp->position.y, wp->position.z + ofsZ, size);
			}
		}
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: returning the built GXColor from an inline lands the local at the original's stack slot
// (sp+0x8) and the call argument at sp+0xC, instead of the other way round (brief code shape).
static inline GXColor marioBlurColor(u8 alpha) {
	GXColor color = *(GXColor*)(void*)&dat_80419e34;

	color.a = alpha;
	return color;
}
#endif

// NONMATCHING (99.95%): body and control flow match; only the GXColor build in the
// evt-color call differs, where MWCC swaps the two scratch registers (original keeps the
// colour word in r3 and the mp pointer in r4; ours the other way round).
void marioDispBlurSub(s32* unused, s32 blurId) {
	MarioWork* wp = mp;
	Mtx mtx;
	Mtx temp;
	Mtx scale;
	GXColor color;
	f32 scaleFactor;
	f32 s;

	scaleFactor = wp->flags & 0x2000000 ? 1.2f : 2.0f;
	PSMTXScale(mtx, wp->scale.x * scaleFactor, wp->scale.y * scaleFactor, wp->scale.z * scaleFactor);

	if (!(wp->dispFlags & 0x8)) {
		if (wp->currMotionId == 0x14) {
			f32 angle = revise360(*(f32*)(wp->field_0x98 + 0x24));

			s = (angle >= 90.0f && angle <= 270.0f) ? -1.0f : 1.0f;
			PSMTXScale(scale, 1.0f, 1.0f, s);
			PSMTXConcat(scale, mtx, mtx);
		}
	}

	if (!(wp->dispFlags & 0xC00)) {
		if (!(wp->dispFlags & 0x8)) {
			f32 angle = revise360(wp->unk1A8);

			s = (angle >= 90.0f && angle <= 270.0f) ? -1.0f : 1.0f;
			PSMTXScale(scale, 1.0f, 1.0f, s);
			PSMTXConcat(scale, mtx, mtx);
		}
	}
	else {
		if (wp->dispFlags & 0x400) {
			PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
		}
		else {
			PSMTXScale(scale, 1.0f, 1.0f, -1.0f);
		}
		PSMTXConcat(scale, mtx, mtx);
	}

	PSMTXTrans(temp, -*(f32*)(wp->field_0x98 + 0x18), -*(f32*)(wp->field_0x98 + 0x1C), -*(f32*)(wp->field_0x98 + 0x20));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->currMotionId == 0x18) {
		PSMTXRotRad(temp, 'x', 0.017453292f * *(f32*)(wp->field_0x98 + 0x24));
		PSMTXConcat(temp, mtx, mtx);
		PSMTXRotRad(temp, 'z', 0.017453292f * *(f32*)(wp->field_0x98 + 0x2C));
		PSMTXConcat(temp, mtx, mtx);
	}
	else {
		PSMTXRotRad(temp, 'z', 0.017453292f * *(f32*)(wp->field_0x98 + 0x2C));
		PSMTXConcat(temp, mtx, mtx);
		PSMTXRotRad(temp, 'x', 0.017453292f * *(f32*)(wp->field_0x98 + 0x24));
		PSMTXConcat(temp, mtx, mtx);
	}

	PSMTXRotRad(temp, 'y', 0.017453292f * revise360(wp->unk1A8 - wp->targetYaw));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->dispFlags & 0xC00) {
		f32 angle = revise360(wp->unk1A8);

		if (angle >= 90.0f && angle <= 270.0f) {
			PSMTXRotRad(temp, 'y', 0.017453292f * ((wp->dispFlags & 0x400) ? 180.0f : 0.0f));
			PSMTXConcat(temp, mtx, mtx);
		}
		else {
			PSMTXRotRad(temp, 'y', 0.017453292f * ((wp->dispFlags & 0x400) ? 0.0f : 180.0f));
			PSMTXConcat(temp, mtx, mtx);
		}
	}

	PSMTXTrans(temp, *(f32*)(wp->field_0x98 + 0x18), *(f32*)(wp->field_0x98 + 0x1C), *(f32*)(wp->field_0x98 + 0x20));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->flags & 0x1000000) {
		PSMTXScale(scale, 1.0f, *(f32*)(wp->unk104 + 0x2C), 1.0f);
		PSMTXConcat(scale, mtx, mtx);
	}

	if (wp->currMotionId == 0x1A) {
		PSMTXRotRad(temp, 'z', 0.017453292f * *(f32*)(wp->field_0x2AC + 0x0C));
		PSMTXConcat(temp, mtx, mtx);
	}

	{
		f32 zero = 0.0f;
		f32 x = *(f32*)(wp->field_0x98 + 0x0C) + (*(f32*)(wp->field_0x98 + 0x00) + blurPos[blurId].x);
		f32 y = zero + (*(f32*)(wp->field_0x98 + 0x10) + (*(f32*)(wp->field_0x98 + 0x04) + blurPos[blurId].y));
		f32 z = *(f32*)(wp->field_0x98 + 0x14) + (*(f32*)(wp->field_0x98 + 0x08) + blurPos[blurId].z);

		PSMTXTrans(temp, x, y, z);
	}
	PSMTXConcat(temp, mtx, mtx);

	animPoseSetMaterialFlagOn(wp->paperAnimGroupId[marioAnimeId()], 0x40);

#ifdef MUST_MATCH
	color = marioBlurColor(0x80 - blurId * 16);
#else
	color = *(const GXColor*)&dat_80419e34;
	color.a = 0x80 - blurId * 16;
#endif
	animPoseSetMaterialEvtColor(wp->paperAnimGroupId[marioAnimeId()], color);

	animPoseDrawMtx(wp->paperAnimGroupId[marioAnimeId()], mtx, 1, 0.0f, 1.0f);
	animPoseDrawMtx(wp->paperAnimGroupId[marioAnimeId()], mtx, 2, 0.0f, 1.0f);
	animPoseDrawMtx(wp->paperAnimGroupId[marioAnimeId()], mtx, 3, 0.0f, 1.0f);
}

// NONMATCHING (1.03%): 96 missing.
void marioDispBlur(CameraId camId, void* param) {
	MarioWork* wp = param; //cast to correct type

}

void marioDisp(CameraId camId, void* param) {
	MarioWork* wp = mp;
	Mtx mtx;
	Mtx temp;
	Mtx scale;
	f32 scaleFactor;
	f32 s;

	scaleFactor = wp->flags & 0x2000000 ? 1.2f : 2.0f;
	PSMTXScale(mtx, wp->scale.x * scaleFactor, wp->scale.y * scaleFactor, wp->scale.z * scaleFactor);

	if (!(wp->dispFlags & 0x8)) {
		if (wp->currMotionId == 0x14) {
			f32 angle = revise360(*(f32*)(wp->field_0x98 + 0x24));

			s = (angle >= 90.0f && angle <= 270.0f) ? -1.0f : 1.0f;
			PSMTXScale(scale, 1.0f, 1.0f, s);
			PSMTXConcat(scale, mtx, mtx);
		}
	}

	if (!(wp->dispFlags & 0xC00)) {
		if (wp->dispFlags & 0x10) {
			PSMTXScale(scale, 1.0f, 1.0f, -1.0f);
			PSMTXConcat(scale, mtx, mtx);
		}
		else if (!(wp->dispFlags & 0x8)) {
			f32 angle = revise360(wp->unk1A8);

			s = (angle >= 90.0f && angle <= 270.0f) ? -1.0f : 1.0f;
			PSMTXScale(scale, 1.0f, 1.0f, s);
			PSMTXConcat(scale, mtx, mtx);
		}
	}
	else {
		if (wp->dispFlags & 0x400) {
			PSMTXScale(scale, 1.0f, 1.0f, 1.0f);
		}
		else {
			PSMTXScale(scale, 1.0f, 1.0f, -1.0f);
		}
		PSMTXConcat(scale, mtx, mtx);
	}

	PSMTXTrans(temp, -*(f32*)(wp->field_0x98 + 0x18), -*(f32*)(wp->field_0x98 + 0x1C), -*(f32*)(wp->field_0x98 + 0x20));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->currMotionId == 0x18) {
		PSMTXRotRad(temp, 'x', 0.017453292f * *(f32*)(wp->field_0x98 + 0x24));
		PSMTXConcat(temp, mtx, mtx);
		PSMTXRotRad(temp, 'z', 0.017453292f * *(f32*)(wp->field_0x98 + 0x2C));
		PSMTXConcat(temp, mtx, mtx);
	}
	else {
		PSMTXRotRad(temp, 'z', 0.017453292f * *(f32*)(wp->field_0x98 + 0x2C));
		PSMTXConcat(temp, mtx, mtx);
		PSMTXRotRad(temp, 'x', 0.017453292f * *(f32*)(wp->field_0x98 + 0x24));
		PSMTXConcat(temp, mtx, mtx);
	}

	PSMTXRotRad(temp, 'y', 0.017453292f * revise360(wp->unk1A8 - wp->targetYaw));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->dispFlags & 0xC00) {
		f32 angle = revise360(wp->unk1A8);

		if (angle >= 90.0f && angle <= 270.0f) {
			PSMTXRotRad(temp, 'y', 0.017453292f * ((wp->dispFlags & 0x400) ? 180.0f : 0.0f));
			PSMTXConcat(temp, mtx, mtx);
		}
		else {
			PSMTXRotRad(temp, 'y', 0.017453292f * ((wp->dispFlags & 0x400) ? 0.0f : 180.0f));
			PSMTXConcat(temp, mtx, mtx);
		}
	}

	PSMTXTrans(temp, *(f32*)(wp->field_0x98 + 0x18), *(f32*)(wp->field_0x98 + 0x1C), *(f32*)(wp->field_0x98 + 0x20));
	PSMTXConcat(temp, mtx, mtx);

	if (wp->flags & 0x1000000) {
		PSMTXScale(scale, 1.0f, *(f32*)(wp->unk104 + 0x2C), 1.0f);
		PSMTXConcat(scale, mtx, mtx);
	}

	{
		f32 x = *(f32*)(wp->field_0x98 + 0x0C) + (wp->position.x + *(f32*)(wp->field_0x98 + 0x00));
		f32 y = *(f32*)(wp->field_0x98 + 0x10) + (wp->position.y + *(f32*)(wp->field_0x98 + 0x04));
		f32 z = *(f32*)(wp->field_0x98 + 0x14) + (wp->position.z + *(f32*)(wp->field_0x98 + 0x08));

		PSMTXTrans(temp, x, y, z);
	}
	PSMTXConcat(temp, mtx, mtx);

	animPoseDrawMtx(wp->paperAnimGroupId[marioAnimeId()], mtx, 2, 0.0f, 1.0f);
	animPoseDrawMtx(wp->paperAnimGroupId[marioAnimeId()], mtx, 1, 0.0f, 1.0f);

	s = (wp->flags & 0x04000000) ? 0.75f : 1.0f;
	animPoseSetLocalTimeRate(wp->paperAnimGroupId[marioAnimeId()], s);
}

const char* toRearPose(const char* name) { //1:1
	const char* rear = NULL;
	if (mp->characterId == 1 && !(mp->peachFlags & 0x2000)) {
		if (!strcmp(name, "P_Z_1")) {
			rear = "P_Z_1R";
		}
		else if (!strcmp(name, "P_S_1")) {
			rear = "P_S_1R";
		}
		else if (!strcmp(name, "P_W_1")) {
			rear = "P_W_1R";
		}
		else if (!strcmp(name, "P_R_1")) {
			rear = "P_R_1R";
		}
		else if (!strcmp(name, "P_T_1")) {
			rear = "P_T_1R";
		}
		else if (!strcmp(name, "P_Z_2")) {
			rear = "P_Z_2R";
		}
		else if (!strcmp(name, "P_S_2")) {
			rear = "P_S_2R";
		}
		else if (!strcmp(name, "P_W_2")) {
			rear = "P_W_2R";
		}
		else if (!strcmp(name, "P_R_2")) {
			rear = "P_R_2R";
		}
		else if (!strcmp(name, "P_T_2")) {
			rear = "P_T_2R";
		}
		else if (!strcmp(name, "P_Z_3")) {
			rear = "P_Z_3R";
		}
		else if (!strcmp(name, "P_S_3")) {
			rear = "P_S_3R";
		}
		else if (!strcmp(name, "P_W_3")) {
			rear = "P_W_3R";
		}
		else if (!strcmp(name, "P_R_3")) {
			rear = "P_R_3R";
		}
		else if (!strcmp(name, "P_T_3")) {
			rear = "P_T_3R";
		}
		else if (!strcmp(name, "P_O_Z_1")) {
			rear = "P_O_Z_1R";
		}
		else if (!strcmp(name, "P_O_S_1")) {
			rear = "P_O_S_1R";
		}
		else if (!strcmp(name, "P_O_F_1")) {
			rear = "P_O_F_1R";
		}
		else if (!strcmp(name, "P_O_R_1")) {
			rear = "P_O_R_1R";
		}
		else if (!strcmp(name, "P_O_N_1")) {
			rear = "P_O_N_1R";
		}
		else if (!strcmp(name, "P_O_S_2")) {
			rear = "P_O_S_2R";
		}
		else if (!strcmp(name, "P_O_F_2")) {
			rear = "P_O_F_2R";
		}
		else if (!strcmp(name, "P_O_R_2")) {
			rear = "P_O_R_2R";
		}
		else if (!strcmp(name, "P_O_N_2")) {
			rear = "P_O_N_2R";
		}
		else if (!strcmp(name, "P_O_D_1")) {
			rear = "P_O_D_1R";
		}
		else if (!strcmp(name, "P_O_D_2")) {
			rear = "P_O_D_2R";
		}
		else if (!strcmp(name, "P_A_1")) {
			rear = "P_A_1R";
		}
		return rear;
	}
	else {
		if (!strcmp(name, "M_Z_1")) {
			rear = "M_Z_1R";
		}
		else if (!strcmp(name, "M_S_1")) {
			rear = "M_S_1R";
		}
		else if (!strcmp(name, "M_W_1")) {
			rear = "M_W_1R";
		}
		else if (!strcmp(name, "M_R_1")) {
			rear = "M_R_1R";
		}
		else if (!strcmp(name, "M_J_1B")) {
			rear = "M_J_1BR";
		}
		else if (!strcmp(name, "M_J_1C")) {
			rear = "M_J_1CR";
		}
		else if (!strcmp(name, "M_H_1")) {
			rear = "M_H_1R";
		}
		else if (!strcmp(name, "M_H_2")) {
			rear = "M_H_2R";
		}
		else if (!strcmp(name, "M_H_4")) {
			rear = "M_H_4R";
		}
		else if (!strcmp(name, "M_H_5")) {
			rear = "M_H_5R";
		}
		else if (!strcmp(name, "M_H_7")) {
			rear = "M_H_7R";
		}
		else if (!strcmp(name, "M_H_8")) {
			rear = "M_H_8R";
		}
		else if (!strcmp(name, "M_O_2")) {
			rear = "M_O_2R";
		}
		else if (!strcmp(name, "M_I_2")) {
			rear = "M_I_2R";
		}
		else if (!strcmp(name, "M_U_2")) {
			rear = "M_U_2R";
		}
		else if (!strcmp(name, "M_C_1")) {
			rear = "M_C_1R";
		}
		return rear;
	}
}

const char* toFrontPose(const char* name) { //1:1
	const char* front = NULL;
	if (mp->characterId == 1 && !(mp->peachFlags & 0x2000)) {
		if (!strcmp(name, "P_Z_1R")) {
			front = "P_Z_1";
		}
		else if (!strcmp(name, "P_S_1R")) {
			front = "P_S_1";
		}
		else if (!strcmp(name, "P_W_1R")) {
			front = "P_W_1";
		}
		else if (!strcmp(name, "P_R_1R")) {
			front = "P_R_1";
		}
		else if (!strcmp(name, "P_T_1R")) {
			front = "P_T_1";
		}
		else if (!strcmp(name, "P_Z_2R")) {
			front = "P_Z_2";
		}
		else if (!strcmp(name, "P_S_2R")) {
			front = "P_S_2";
		}
		else if (!strcmp(name, "P_W_2R")) {
			front = "P_W_2";
		}
		else if (!strcmp(name, "P_R_2R")) {
			front = "P_R_2";
		}
		else if (!strcmp(name, "P_T_2R")) {
			front = "P_T_2";
		}
		else if (!strcmp(name, "P_Z_3R")) {
			front = "P_Z_3";
		}
		else if (!strcmp(name, "P_S_3R")) {
			front = "P_S_3";
		}
		else if (!strcmp(name, "P_W_3R")) {
			front = "P_W_3";
		}
		else if (!strcmp(name, "P_R_3R")) {
			front = "P_R_3";
		}
		else if (!strcmp(name, "P_T_3R")) {
			front = "P_T_3";
		}
		else if (!strcmp(name, "P_O_Z_1R")) {
			front = "P_O_Z_1";
		}
		else if (!strcmp(name, "P_O_S_1R")) {
			front = "P_O_S_1";
		}
		else if (!strcmp(name, "P_O_F_1R")) {
			front = "P_O_F_1";
		}
		else if (!strcmp(name, "P_O_R_1R")) {
			front = "P_O_R_1";
		}
		else if (!strcmp(name, "P_O_N_1R")) {
			front = "P_O_N_1";
		}
		else if (!strcmp(name, "P_O_S_2R")) {
			front = "P_O_S_2";
		}
		else if (!strcmp(name, "P_O_F_2R")) {
			front = "P_O_F_2";
		}
		else if (!strcmp(name, "P_O_R_2R")) {
			front = "P_O_R_2";
		}
		else if (!strcmp(name, "P_O_N_2R")) {
			front = "P_O_N_2";
		}
		else if (!strcmp(name, "P_O_D_1R")) {
			front = "P_O_D_1";
		}
		else if (!strcmp(name, "P_O_D_2R")) {
			front = "P_O_D_2";
		}
		else if (!strcmp(name, "P_A_1R")) {
			front = "P_A_1";
		}
		return front;
	}
	else {
		if (!strcmp(name, "M_Z_1R")) {
			front = "M_Z_1";
		}
		else if (!strcmp(name, "M_S_1R")) {
			front = "M_S_1";
		}
		else if (!strcmp(name, "M_W_1R")) {
			front = "M_W_1";
		}
		else if (!strcmp(name, "M_R_1R")) {
			front = "M_R_1";
		}
		else if (!strcmp(name, "M_J_1BR")) {
			front = "M_J_1B";
		}
		else if (!strcmp(name, "M_J_1CR")) {
			front = "M_J_1C";
		}
		else if (!strcmp(name, "M_H_1R")) {
			front = "M_H_1";
		}
		else if (!strcmp(name, "M_H_2R")) {
			front = "M_H_2";
		}
		else if (!strcmp(name, "M_H_4R")) {
			front = "M_H_4";
		}
		else if (!strcmp(name, "M_H_5R")) {
			front = "M_H_5";
		}
		else if (!strcmp(name, "M_H_7R")) {
			front = "M_H_7";
		}
		else if (!strcmp(name, "M_H_8R")) {
			front = "M_H_8";
		}
		else if (!strcmp(name, "M_O_2R")) {
			front = "M_O_1";
		}
		else if (!strcmp(name, "M_I_2R")) {
			front = "M_I_2";
		}
		else if (!strcmp(name, "M_U_2R")) {
			front = "M_U_2";
		}
		else if (!strcmp(name, "M_C_1R")) {
			front = "M_C_1";
		}
		return front;
	}
}
