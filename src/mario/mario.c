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

MarioWork* marioGetPtr(void) {
	return mp;
}

s32 marioGetColor(void) {
	return mp->colorId;
}

// NONMATCHING (0.17%): empty placeholder, not decompiled yet.
void marioSetCharMode(s32 mode) {

}

// NONMATCHING (0.17%): empty placeholder, not decompiled yet.
void marioSetFamicomMode(s32 mode) {

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

// NONMATCHING (0.72%): empty placeholder, not decompiled yet.
void marioReset(void) {

}

// NONMATCHING (3.57%): empty placeholder, not decompiled yet.
void marioOfsRotReset(void) {

}

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

// NONMATCHING (0.35%): empty placeholder, not decompiled yet.
void marioInit(void) {

}

// NONMATCHING (0.40%): empty placeholder, not decompiled yet.
void marioReInit(void) {

}

// NONMATCHING (0.16%): empty placeholder, not decompiled yet.
void marioMain(void) {

}

// NONMATCHING (85.70%): 4 instructions differ, 15 missing, plus register differences.
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

// NONMATCHING (0.13%): empty placeholder, not decompiled yet.
void marioMove(void) {

}

// NONMATCHING (0.27%): empty placeholder, not decompiled yet.
void marioMoveMain(void) {

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

// NONMATCHING (0.29%): empty placeholder, not decompiled yet.
void marioMakeDispDir(void) {

}

// NONMATCHING (0.23%): empty placeholder, not decompiled yet.
void marioRearAnime(void) {

}

// NONMATCHING (0.38%): empty placeholder, not decompiled yet.
void marioPreDisp(void) {

}

// NONMATCHING (0.26%): empty placeholder, not decompiled yet.
void marioDispBlurSub(s32* unused, s32 blurId) {

}

// NONMATCHING (1.03%): 96 missing.
void marioDispBlur(CameraId camId, void* param) {
	MarioWork* wp = param; //cast to correct type

}

// NONMATCHING (6.54%): 309 missing, plus register differences.
void marioDisp(CameraId camId, void* param) {
	MarioWork* wp = param; //cast to correct type
	Mtx mtx1; //TODO: rename
	f32 scale;

	scale = marioGetScale(); //inlined
	MTXScale(mtx1, mp->scale.x * scale, mp->scale.y * scale, mp->scale.z * scale);

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
