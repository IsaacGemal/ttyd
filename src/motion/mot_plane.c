#include "motion/mot_plane.h"
#include "mot/mot_plane.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "motion/mot_damage.h"
#include "motion/mot_jump.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include "memory.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "manager/evtmgr.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "matching.h"

// Raw accessor for the MarioWork word at 0x18C (MarioWork.field_0x18C).
#define MF32(d, off) (*(f32*)((u8*)(d) + (off)))

// The plane data block of mario->motStruct.
#define DATA ((MotionPlaneData*)mario->motStruct)

// .rodata zero vectors used by the plane motion (original vec3_802c5c9c / vec3_802c5cd8...).
// They are defined at the source positions that reproduce the original .rodata order.

// .data:0x0. Plane geometry/trig table read by chkCollision/flyMain (41 words, original order).
f32 planeData[41] = {
    5.0f,        -0.02f,       -0.03f,       25.0f,       -45.0f,
    0.4f,        1.0f,         0.1f,        -0.1f,        1.5f,
    0.2f,        0.05f,        3.4f,         1.0f,        0.025f,
    0.045f,      0.8f,         1.5f,        -0.3f,       -5.0f,
    3.9f,        -0.112f,       0.118f,       0.5f,        0.4f,
    -8.0f,       -2.0f,        -4.0f,        -2.0f,        0.2f,
    0.0f,        1.2f,         1.0f,        15.0f,      -30.0f,
    12.0f,       0.0f,        30.0f,       -30.0f,        6.0f,
    28.0f,
};

static EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

// .rodata:0x14, after the SFX string emitted by sound_evt above.
const Vec vec3_802c5c9c = { 0.0f, 40.0f, 0.0f };

s32 marioPlaneGetFlyingDir(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
		return 1;
	}
	return -1;
}

s32 marioGetPlaneStatus(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
#ifdef MUST_MATCH
	// The original compares currSubMotionId with cmpwi (signed); the header types it u32.
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
#else
	if (mario->currSubMotionId >= 0xC && mario->currSubMotionId < 0x14) {
#endif
		return 2;
	}
	return 1;
}

s32 mario_plane_cancel(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
		mario->currSubMotionId = 0x32;
		mario->field_0x2AC = mario->position.y;
		return 1;
	}
	return 0;
}

// Paper animation names indexed by mario's current color (paper_plane@sda21 in the asm).
const char* paper_plane[4] = {
    "p_hikohki",
    "p_hikohki_g",
    "p_hikohki_y",
    "p_hikohki_v",
};

// .rodata:0x50, after the four paper-plane names emitted by paper_plane above.
const Vec vec3_802c5cd8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ce4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5cf0 = { 0.0f, 0.0f, 0.0f };
// .rodata:0x90: the int->float conversion magic. Do NOT define it here: MWCC pools its own anonymous copy
// for the compiler-generated `(f32)(s8)...` conversions, and defining a named object too emits a second copy
// at 0x98 that shifts every following string by 8. Left undefined so the pooled copy lands at 0x90.
// .rodata:0x98..0xB8 ("M_J_1B", "PM_P_1A", "PM_P_1B", "M_J_1C"): used as string literals inside mot_plane so
// MWCC dedups them with marioForcePlaneAnime's "PM_P_1B" and pools them after the magic at 0x98..0xB8.

// Floats and short pose names are compiler-pooled literals. The deferred pool helpers
// below establish the original first-use order without duplicating named constants.

void marioForcePlaneAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(paper_plane[marioGetColor()]);
    marioChgPaper("PM_P_1B");
    marioChgPose("M_Z_1");
    mario->dispFlags |= 0x8;
}

static void flyMain(void);

// Inlined reset block the original duplicates at every mot_plane exit (fields set to 20/37,
// zero vectors, paper off). Reproduces the same scheduling as mot_plane_post.
static inline void planeReset(void) {
	MarioWork* m = marioGetPtr();

	m->wPlayerCollisionBox.x = 20.0f;
#ifdef MUST_MATCH
	// Volatile store/loads keep the following volatile Vec copies below it, as the original schedules them.
	*(volatile f32*)&m->wPlayerCollisionBox.y = 37.0f;
	*(volatile Vec*)&m->dispRot = *(volatile Vec*)&vec3_802c5cd8;
	*(volatile Vec*)&m->dispPivot = *(volatile Vec*)&vec3_802c5ce4;
	*(volatile Vec*)&m->dispOfs = *(volatile Vec*)&vec3_802c5cf0;
#else
	m->wPlayerCollisionBox.y = 37.0f;
	m->dispRot = vec3_802c5cd8;
	m->dispPivot = vec3_802c5ce4;
	m->dispOfs = vec3_802c5cf0;
#endif
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
}

// Plane state machine.
// Matching local declaration/assignment order preserves the retail register allocation.
#ifdef MUST_MATCH
// Keep literal-initialized factors in the original load order and registers.
#pragma opt_propagation off
#endif
void mot_plane(void) {
	MarioWork* mario = marioGetPtr();
	s32 wrap;

	if (mario->trigFlags & 1) {
		MotionPlaneData* data;
#ifdef MUST_MATCH
		s32 inactive;
#endif
		mario->trigFlags &= ~1;
		mario->motStruct = __memAlloc(HEAP_DEFAULT, sizeof(MotionPlaneData));
		memset(mario->motStruct, 0, sizeof(MotionPlaneData));
#ifdef MUST_MATCH
		data = marioGetPtr()->motStruct;
		inactive = -1;
		data->sfxId = inactive;
#else
		{
			MarioWork* m2 = marioGetPtr();
			data = m2->motStruct;
		}
		data->sfxId = -1;
#endif
		mario->currSubMotionId = 0;
	}

	switch (mario->currSubMotionId) {
	case 0: {
		MarioWork* m = marioGetPtr();
		s32 attr;

		m->unk148 = 0.4f;
		m->unk150.z = 0.01f;
#ifdef MUST_MATCH
		// Keep the store above the per-component vector copy, as in the original.
		*(volatile f32*)&m->field_68 = 2.0f;
		*(volatile Vec*)&m->field_6C = *(volatile Vec*)&vec3_802c5c9c;
#else
		m->field_68 = 2.0f;
		m->field_6C = vec3_802c5c9c;
#endif
		marioPreJump();
		marioChgPose("M_J_1B");
		mario->dispFlags |= 0x100;
		mario->currSubMotionId = 1;
		mario->airTimer = 0;
		{
			f32 zero = 0.0f;
			mario->baseSpeed = zero;
			mario->field_0x2A4 = zero;
			mario->wCamVal1 = mario->position.y;
			mario->field_0x2AC = zero;
		}
		attr = hitGetAttr(mario->hitobjJumpFrom);
		((MotionPlaneData*)mario->motStruct)->flags = (attr >> 4) & 1;
		psndSFXOn_3D((char*)0x17d, &mario->position);
		psndSFXOn_3D((char*)0xaf, &mario->position);
	}
		/* fall through */
	case 1: {
		f32 dir;
		f32 v;
		f32 k;
		f32 m;
		f32 t;

		marioMakeJumpPara();
		k = 0.125f;
		m = 30.0f;
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? 1.0f : -1.0f;
		v = mario->field_0x2A4;
		t = m * dir;
		mario->field_0x2A4 = k * (t - v) + v;
		mario->unk1A8 += mario->field_0x2A4;
		wrap = 0;
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
			if (mario->unk1A8 >= 630.0f) {
				wrap = 1;
			}
		} else {
			if (mario->unk1A8 <= -630.0f) {
				wrap = 1;
			}
		}
		if (wrap) {
			EventEntry* evt;
			mario->currSubMotionId = 2;
			mario->multiTimer = 0x14;
			marioPaperOn(paper_plane[marioGetColor()]);
			marioChgPaper("PM_P_1A");
			marioChgPose("M_Z_1");
			mario->dispFlags |= 0x8;
			animPoseSetLocalTime(mario->paperPoseId, 0.0f);
			animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], 0.0f);
			evt = evtEntry(sound_evt, 0, 0);
			{
				f32 sc = 1024.0f;
				evt->lwData[0] = (s32)(sc * mario->position.x) - 0x0DB58580;
				evt->lwData[1] = (s32)(sc * mario->position.y) - 0x0DB58580;
				evt->lwData[2] = (s32)(sc * mario->position.z) - 0x0DB58580;
			}
		}
		if (mario->jumpVelY > 0.0f) {
			mario->position.y += mario->jumpVelY;
		}
		break;
	}
	case 2: {
		f32 dir;
		f32 v;
		f32 k;
		f32 m;
		f32 t;

		animPoseSetLocalTime(mario->paperPoseId, 0.0f);
		animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], 0.0f);
		k = 0.125f;
		m = 30.0f;
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? 1.0f : -1.0f;
		v = mario->field_0x2A4;
		t = m * dir;
		mario->field_0x2A4 = k * (t - v) + v;
		mario->unk1A8 += mario->field_0x2A4;
		wrap = 0;
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
#ifdef MUST_MATCH
			f32 lim;
			f32 value;
			value = mario->unk1A8;
			lim = 1260.0f;
			if (value >= lim) {
#else
			f32 lim = 1260.0f;
			if (mario->unk1A8 >= lim) {
#endif
				mario->unk1A8 = lim;
				wrap = 1;
			}
		} else {
#ifdef MUST_MATCH
			f32 lim;
			f32 value;
			value = mario->unk1A8;
			lim = -1080.0f;
			if (value <= lim) {
#else
			f32 lim = -1080.0f;
			if (mario->unk1A8 <= lim) {
#endif
				mario->unk1A8 = lim;
				wrap = 1;
			}
		}
		if (wrap) {
			mario->multiTimer = 0x3C;
			mario->currSubMotionId = 3;
		}
		break;
	}
	case 3: {
		MarioWork* m;
		f32 dir;

		if (--mario->multiTimer > 0) {
			if (mario->multiTimer == 0x28) {
				allPartyRidePlane();
			}
			break;
		}
		marioChgPaper("PM_P_1B");
		((MotionPlaneData*)mario->motStruct)->heading = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? 90.0f
		                                                               : 270.0f;
		{
			f32 zero = 0.0f;
			mario->field_0x2A4 = zero;
			((MotionPlaneData*)mario->motStruct)->pitch = zero;
			((MotionPlaneData*)mario->motStruct)->bank = zero;
			((MotionPlaneData*)mario->motStruct)->field_0x14 = zero;
			((MotionPlaneData*)mario->motStruct)->field_0x10 = zero;
		}
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
			mario->dispPivot.x = planeData[33];
			mario->dispOfs.x = planeData[34];
		} else {
			mario->dispPivot.x = planeData[35];
			mario->dispOfs.x = planeData[36];
		}
		mario->wPlayerCollisionBox.x = 20.0f;
		mario->wPlayerCollisionBox.y = 12.333333f;
		mario->baseSpeed = __fabsf(0.02f * (f32)(s8)mario->wStickDir1);
		mario->currSubMotionId = 0xC;
		mario->baseSpeed = 1.0f;
		((MotionPlaneData*)mario->motStruct)->field_0x10 = 2.0f;
		m = marioGetPtr();
		m->unk148 = 0.0f;
		m->unk150.z = 0.004f;
		{
			f32 k70 = 70.0f;
			dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? -1.0f : 1.0f;
			mario->unk138 = k70 * dir;
		}
		mario->unk140 = -70.0f;
		break;
	}
	default:
		break;
	}

	// Common per-frame tail.
	if (mario->position.y < 100.0f) {
		mario->unk140 = __fabsf(planeData[38]);
	}
	flyMain();
	if (marioKeyOffChk() && !(mario->trigFlags & 4)) {
		if (mario_plane_cancel()) {
			return;
		}
	}
	if ((s32)mario->currSubMotionId == 0x14 || (s32)mario->currSubMotionId == 0x1E) {
		MarioWork* m;
		camFollowYOff();
		{
			f32 zero = 0.0f;
			mario->unk140 = zero;
			mario->unk138 = zero;
		}
		marioClearJumpPara();
		mario->baseSpeed = 0.0f;
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->sfxId != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->sfxId);
				((MotionPlaneData*)m->motStruct)->sfxId = -1;
			}
		}
		planeReset();
		allPartyRideOff2();
		if ((s32)mario->currSubMotionId == 0x14) {
			marioChgMot(3);
		} else if ((s32)mario->currSubMotionId == 0x1E) {
			marioChgMot(0);
		}
	}

	switch (mario->currSubMotionId) {
	case 0x28: {
		MarioWork* m;

		camFollowYOff();
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->sfxId != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->sfxId);
				((MotionPlaneData*)m->motStruct)->sfxId = -1;
			}
		}
		mario->multiTimer = sysMsec2Frame(0x208);
		mario->currSubMotionId = 0x29;
		break;
	}
	case 0x29:
		if (--mario->multiTimer <= 0) {
			f32 t;
#ifdef MUST_MATCH
			f32 b;
			f32 a;
			f32 one;
#else
			f32 a;
			f32 b;
#endif
			planeReset();
			marioChgPose("M_J_1C");
			a = mario->field_0x2AC;
			b = 37.0f;
#ifdef MUST_MATCH
			one = 1.0f;
#endif
			t = a - b;
#ifdef MUST_MATCH
			mario->position.y = one + t;
#else
			mario->position.y = t + 1.0f;
#endif
			mario->baseSpeed = 0.0f;
			marioSetFallPara();
			mario->jumpVelY = 0.0f;
			{
				f32 half = 0.5f;
				mario->jumpAccelY *= half;
				mario->jumpJerkY *= half;
				mario->jumpJerkStepY *= half;
			}
			mario->currSubMotionId = 0x5A;
			mario->dispFlags |= 0x100;
			allPartyRideOff2();
		}
		break;
	case 0x3C:
		camFollowYOff();
		mario->multiTimer = sysMsec2Frame(0x208);
		mario->currSubMotionId = 0x3D;
		/* fall through */
	case 0x3D:
		if (mario->baseSpeed > 1.0f) {
			mario->currSubMotionId = 0xE;
			break;
		}
		if (--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0x34;
		}
		break;
	case 0x32:
		camFollowYOff();
		mario->multiTimer = sysMsec2Frame(0x208);
		mario->currSubMotionId = 0x34;
		break;
	case 0x33:
		camFollowYOff();
		mario->multiTimer = sysMsec2Frame(0x64);
		mario->currSubMotionId = 0x34;
		break;
	case 0x34:
		if (--mario->multiTimer > 0) {
			break;
		}
		/* fall through */
	case 0x35: {
		MarioWork* m;

		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->sfxId != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->sfxId);
				((MotionPlaneData*)m->motStruct)->sfxId = -1;
			}
		}
		planeReset();
		mario->dispFlags &= ~0x100;
		{
			f32 zero = 0.0f;
			mario->baseSpeed = zero;
			mario->field_0x2A4 = zero;
		}
		mario->position.y = 1.0f + mario->field_0x2AC;
		marioPreJump();
		{
#ifdef MUST_MATCH
			f32 half;
			f32 value;
			value = mario->jumpVelY;
			half = 0.5f;
			mario->jumpVelY = value * half;
#else
			f32 half = 0.5f;
			mario->jumpVelY = mario->jumpVelY * 0.5f;
#endif
		}
		marioChgPose("M_J_1B");
		mario->currSubMotionId = 0x50;
		psndSFXOn_3D((char*)0x180, &mario->position);
		allPartyRideOff2();
	}
		/* fall through */
	case 0x50: {
		f32 dir;
		f32 v;

		f32 k28;
		f32 k3;
		// Declare in the original's register order (k28=f2, k3=f3) but assign in its load order (k3 first).
		k3 = 0.3f;
		k28 = 28.0f;
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? 1.0f : -1.0f;
		v = mario->field_0x2A4;
		mario->field_0x2A4 = k3 * (k28 * dir - v) + v;
		mario->unk1A8 = revise360(mario->unk1A8 + mario->field_0x2A4);
		marioMakeJumpPara();
		mario->position.y += mario->jumpVelY;
		marioChkOverhead();
		if (mario->jumpVelY < 0.0f || mario->hitobjHead != NULL) {
#ifdef MUST_MATCH
			f32 value;
#endif
			f32 half;
			marioSetFallPara();
#ifdef MUST_MATCH
			value = mario->jumpVelY;
#endif
			half = 0.5f;
#ifdef MUST_MATCH
			mario->jumpVelY = value * half;
#else
			mario->jumpVelY *= half;
#endif
			mario->jumpAccelY *= half;
			mario->jumpJerkY *= half;
			mario->jumpJerkStepY *= half;
			mario->currSubMotionId = 0x5A;
			mario->dispFlags |= 0x100;
			mario->multiTimer = 0x14;
		}
		break;
	}
	case 0x5A: {
		f32 dir;
		f32 v;

		if (mario->multiTimer > 0) {
			f32 k28;
			f32 k3;
			k3 = 0.3f;
			k28 = 28.0f;
			dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? 1.0f : -1.0f;
			v = mario->field_0x2A4;
			mario->field_0x2A4 = k3 * (k28 * dir - v) + v;
			mario->unk1A8 = revise360(mario->unk1A8 + mario->field_0x2A4);
			if (--mario->multiTimer <= 0) {
				mario->dispFlags &= ~0x100;
			}
		}
		marioMakeJumpPara();
		{
#ifdef MUST_MATCH
			f32 neg20;
			f32 value;
			value = mario->jumpVelY;
			neg20 = -20.0f;
			if (value <= neg20) {
#else
			f32 neg20 = -20.0f;
			if (mario->jumpVelY <= neg20) {
#endif
				mario->jumpVelY = neg20;
			}
		}
		mario->hitobjStandOn = NULL;
		{
			f32 sy;
			f32 sx;
			f32 sz;
			f32 y = mario->jumpVelY;
			HitEntry* hit = marioSearchGround(&sy, &sx, &sz, y, y);

			if (hit != NULL) {
				mario->position.y = sy;
				mario->hitobjStandOn = hit;
			} else {
				mario->position.y += mario->jumpVelY;
			}
		}
		if (mario->hitobjStandOn != NULL) {
			camFollowYOn();
			allPartyRideOff2();
			mario->airTimer = 0;
			mario->airTimerPeak = 0;
			mario->flags &= ~0x80;
			marioAdjustMoveDir();
			planeReset();
			if (mario->motStruct != NULL) {
				__memFree(HEAP_DEFAULT, mario->motStruct);
				mario->motStruct = NULL;
			}
			{
				f32 zero = 0.0f;
				mario->unk138 = zero;
				mario->unk140 = zero;
			}
			marioChgMot(0);
		}
		break;
	}
	case 0x46:
		set_damage_root_ypos(mario->position.y);
		{
			Vec pos = mario->position;
			marioSetSplash(0, &pos);
		}
		mario->multiTimer = 0x10;
		mario->currSubMotionId = 0x47;
		{
#ifdef MUST_MATCH
			f32 zero;
			f32 neg2;
			neg2 = -2.0f;
			zero = 0.0f;
#else
			f32 neg2 = -2.0f;
			f32 zero = 0.0f;
#endif
			mario->jumpVelY = neg2;
			mario->jumpAccelY = zero;
			mario->jumpJerkY = zero;
			mario->jumpJerkStepY = zero;
		}
		camFollowYOff();
		/* fall through */
	case 0x47:
		marioMakeJumpPara();
		mario->position.y += mario->jumpVelY;
		if (mario->position.y >= mario->field_0x2AC - 100.0f) {
			break;
		}
		mario->currSubMotionId = 0x48;
		/* fall through */
	case 0x48: {
		MarioWork* m;
		f32 zero = 0.0f;

		mario->unk140 = zero;
		mario->unk138 = zero;
		marioClearJumpPara();
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->sfxId != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->sfxId);
				((MotionPlaneData*)m->motStruct)->sfxId = -1;
			}
		}
		mario->baseSpeed = 0.0f;
		planeReset();
		allPartyRideOff2();
		marioSetFallPara();
		camFollowYOn();
		marioChgMot(0x1F);
		break;
	}
	default:
		break;
	}

	marioUpdateCamCollision(mario->position);
	MF32(mario, 0x18C) = mario->baseSpeed;
}
#ifdef MUST_MATCH

#pragma opt_propagation reset
#endif

// Flight physics and collision handling.
static void flyMain(void) {
	MarioWork* mario;
	MarioWork* m;
	f32 f31;
	f32 f3;
	s32 onPlace;

	mario = marioGetPtr();
	switch (mario->currSubMotionId) {
		case 0xC:
			if (mario->wCamVal1 >= mario->position.y) {
				mario->currSubMotionId = 0xE;
			}
			m = marioGetPtr();
			if ((u32)((MotionPlaneData*)m->motStruct)->sfxId == 0xFFFFFFFFu) {
				((MotionPlaneData*)m->motStruct)->sfxId =
				    psndSFXOn_3D((const char*)0x17F, &m->position);
			}
			/* fallthrough */
		case 0xE:
			if (mario->dispOfs.y > planeData[38]) {
				mario->dispOfs.y -= 2.0f;
				mario->dispPivot.y += 2.0f;
				mario->position.y += 2.0f;
			} else {
				mario->currSubMotionId = 0xF;
			}
			break;
	}
	if ((s32)mario->currSubMotionId != 0xF) {
		return;
	}
	if (mario->buttonNew & 0x100) {
		mario->currSubMotionId = 0x33;
		mario->field_0x2AC = mario->position.y;
		return;
	}
	if (mario->buttonNew & 0x200) {
		mario->currSubMotionId = 0x33;
		mario->field_0x2AC = mario->position.y;
		return;
	}
	DATA->heading = revise360(DATA->bank * planeData[14] + DATA->heading);
	if (DATA->flags & 1) {
		if (DATA->heading <= 60.0f) {
			DATA->heading = 0.1f * (60.0f - DATA->heading) + DATA->heading;
		}
		if (DATA->heading >= 150.0f) {
			DATA->heading = 0.1f * (150.0f - DATA->heading) + DATA->heading;
		}
	} else {
		if (DATA->heading >= 330.0f) {
			DATA->heading = 0.1f * (330.0f - DATA->heading) + DATA->heading;
		}
		if (DATA->heading <= 210.0f) {
			DATA->heading = 0.1f * (210.0f - DATA->heading) + DATA->heading;
		}
	}
	{
		f32 a = DATA->heading;
		f32 v;
		s32 flag = 0;

		if (a >= 90.0f && a <= 270.0f) {
			flag = 1;
		}
		if (flag) {
			v = -a - 90.0f;
		} else {
			v = 270.0f - a;
		}
		mario->unk1A8 = revise360(v);
	}
	{
		u8 stick = mario->wStickDir1;

		if ((s8)(stick & 0xff) != 0) {
#ifdef MUST_MATCH
			if (mario->baseSpeed) {
#else
			if (mario->baseSpeed != 0.0f) {
#endif
				f3 = __fabsf((f32)(s8)mario->wStickDir1);
				if (DATA->flags & 1) {
					if ((s8)mario->wStickDir1 < 0) {
						if (DATA->pitch >= planeData[3]) {
							DATA->pitch = 0.1f * (planeData[3] - DATA->pitch) + DATA->pitch;
						} else {
							DATA->pitch = 0.1f * (f3 * planeData[5]) + DATA->pitch;
						}
					}
					if ((s8)mario->wStickDir1 > 0) {
						if (DATA->pitch <= planeData[4]) {
							DATA->pitch = planeData[4];
						} else {
							DATA->pitch = DATA->pitch - 0.1f * (f3 * planeData[6]);
						}
					}
				} else {
					if ((s8)mario->wStickDir1 > 0) {
						if (DATA->pitch >= planeData[3]) {
							DATA->pitch = 0.1f * (planeData[3] - DATA->pitch) + DATA->pitch;
						} else {
							DATA->pitch = 0.1f * (f3 * planeData[5]) + DATA->pitch;
						}
					}
					if ((s8)mario->wStickDir1 < 0) {
						if (DATA->pitch <= planeData[4]) {
							DATA->pitch = planeData[4];
						} else {
							DATA->pitch = DATA->pitch - 0.1f * (f3 * planeData[6]);
						}
					}
				}
			}
		} else {
			if (DATA->pitch <= 0.0f || DATA->field_0x14 <= 0.0f) {
				if (DATA->field_0x14 > 0.0f && DATA->field_0x14 < 1.0f) {
					DATA->pitch = MATCH_F32(DATA->pitch) + planeData[30];
				}
				{
					f31 = planeData[0] - mario->baseSpeed;
					if (f31 <= 0.0f) {
						f31 = 0.0f;
					}
					f31 = f31 * (90.0f / planeData[0]);
					f31 = planeData[27] * (f32)sin(3.1415927f * f31 / 180.0f);
					if (f31 <= planeData[28]) {
						f31 = planeData[28];
					}
				}
				onPlace = 0;
				m = marioGetPtr();
				if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
					onPlace = 1;
				}
				if (onPlace == 0) {
					DATA->field_0x1C = f31;
					DATA->pitch = DATA->pitch + DATA->field_0x1C;
				} else {
					DATA->pitch += -0.001f;
					mario->baseSpeed += 0.5f;
					if (mario->baseSpeed > 2.0f) {
						mario->baseSpeed = 2.0f;
					}
				}
			}
			if (DATA->pitch <= planeData[4]) {
				DATA->pitch = planeData[4];
			}
			if (DATA->pitch >= 6.0f) {
				DATA->field_0x14 = 0.0f;
			}
		}
	}
	mario->baseSpeed = mario->baseSpeed * planeData[1] + mario->baseSpeed;
	{
		MotionPlaneData* dv = DATA;

		if (dv->pitch >= 0.0f) {
			dv->flags |= 0x10;
			{
				f32 s = sin(3.1415927f * (DATA->pitch * planeData[7]) / 180.0f);
				f32 x = planeData[8] * s;

				x = mario->baseSpeed * x;
				mario->baseSpeed = mario->baseSpeed + x;
			}
			if ((s8)mario->wStickDir1 != 0 && DATA->pitch >= planeData[3]) {
				mario->baseSpeed = mario->baseSpeed + planeData[2];
			}
			if (mario->baseSpeed <= 0.0f) {
				mario->baseSpeed = 0.0f;
			}
			f31 = 0.0f;
			if ((s8)mario->wStickDir1 == 0) {
				if (mario->baseSpeed < planeData[20]) {
					f32 s, a;

					f31 = planeData[0] - mario->baseSpeed;
					s = sin(3.1415927f * DATA->pitch / 180.0f);
					a = planeData[21] * s;
					f31 = f31 * a;
					f31 = f31 + DATA->field_0x14;
					DATA->field_0x14 = f31;
				} else {
					DATA->field_0x14 = planeData[22];
				}
				DATA->pitch = DATA->pitch + DATA->field_0x14;
				if (f31 <= 0.0f) {
					if (mario->baseSpeed <= planeData[0] * planeData[23]) {
						f31 = planeData[0] - mario->baseSpeed * planeData[24];
						if (f31 <= 0.0f) {
							f31 = 0.0f;
						}
						f31 = f31 * (90.0f / planeData[0]);
						onPlace = 0;
						m = marioGetPtr();
						if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
							onPlace = 1;
						}
						if (onPlace == 0) {
							f32 v = planeData[25] * (f32)sin(3.1415927f * f31 / 180.0f);

							if (v <= planeData[26]) {
								v = 2.0f * planeData[26];
							}
							DATA->field_0x1C = v;
						}
					}
				}
				DATA->pitch = DATA->pitch + DATA->field_0x1C;
			}
			{
				f32 add;

				if ((s8)mario->wStickDir1 != 0) {
					f32 s = sin(3.1415927f * (planeData[0] - mario->baseSpeed) / 180.0f);

					add = planeData[18] * s;
					DATA->field_0x18 = DATA->field_0x18 + add;
					if (DATA->field_0x18 <= planeData[19]) {
						DATA->field_0x18 = planeData[19];
					}
				} else {
					add = 0.1f * -DATA->field_0x18 + DATA->field_0x18;
					DATA->field_0x18 = add;
				}
				DATA->pitch = DATA->pitch + add;
			}
		} else if (dv->pitch >= -80.0f) {
			if (dv->flags & 0x10) {
				dv->flags = dv->flags & ~0x10;
			}
			{
				f32 s = sin(3.1415927f * (planeData[9] * __fabsf(DATA->pitch)) / 180.0f);

				DATA->field_0x10 = planeData[10] * s;
				mario->baseSpeed = mario->baseSpeed + DATA->field_0x10;
			}
			f3 = 0.0f;
			if (DATA->pitch > 0.0f) {
				f3 = 0.0f;
			} else {
				f3 = __fabsf(DATA->pitch);
			}
			{
				f32 x, y;

				f31 = sin(3.1415927f * (planeData[12] * f3) / 180.0f);
				y = sin(3.1415927f * (mario->baseSpeed * planeData[11]) / 180.0f);
				x = planeData[13] * (y * f31);
				DATA->field_0x14 = DATA->field_0x14 + x;
			}
			if ((s8)(mario->wStickDir1 & 0xff) != 0) {
				if (DATA->flags & 1) {
					if ((s8)mario->wStickDir1 > 0 && DATA->field_0x14 >= 1.0f) {
						DATA->field_0x14 = 1.0f;
					}
				} else {
					if ((s8)mario->wStickDir1 < 0 && DATA->field_0x14 >= 1.0f) {
						DATA->field_0x14 = 1.0f;
					}
				}
				onPlace = 0;
				m = marioGetPtr();
				if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
					onPlace = 1;
				}
				if (onPlace == 0) {
					DATA->field_0x1C += 0.1f * (1.0f - DATA->field_0x1C);
					if (DATA->field_0x1C >= 0.0f) {
						DATA->field_0x1C = 0.0f;
					}
				}
			}
			onPlace = 0;
			m = marioGetPtr();
			if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
				onPlace = 1;
			}
			if (onPlace != 0) {
				DATA->field_0x24 = -22.0f;
			} else {
				DATA->field_0x24 += 0.1f * (planeData[4] - DATA->field_0x24);
			}
			DATA->pitch = DATA->pitch + DATA->field_0x14;
			if (DATA->pitch <= DATA->field_0x24) {
				DATA->pitch = DATA->field_0x24;
			}
			DATA->field_0x18 = 0.1f * -DATA->field_0x18 + DATA->field_0x18;
		}
	}
	if (mario->baseSpeed >= planeData[0]) {
		mario->baseSpeed = planeData[0];
	}
	mario->position.y = mario->position.y + DATA->field_0x18;
	mario->position.y = mario->position.y - planeData[29];
	mario->dispRot.z = revise360(-DATA->pitch);
	{
		f32 sin2, cos2, sin1, cos1;

		mario->directionView = DATA->heading;
		sincosf(&sin1, &cos1, mario->directionView);
		mario->position.z = mario->baseSpeed * cos1 + mario->position.z;
		sincosf(&sin2, &cos2, 90.0f + DATA->pitch);
		mario->position.y = mario->baseSpeed * cos2 * (cos2 >= 0.0f ? planeData[31] : planeData[32]) + mario->position.y;
		mario->position.x = sin2 * (mario->baseSpeed * sin1) + mario->position.x;
	}
	if (mario->baseSpeed <= planeData[16]) {
		s32 frames = sysMsec2Frame((s32)(1000.0f * planeData[17]));

		if (DATA->flags & 1) {
			if ((f32)(s8)mario->wStickDir1 < 0.0f) {
				s32 c = DATA->stallTimer + 1;

				DATA->stallTimer = c;
				if (c >= frames) {
					mario->field_0x2AC = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		} else {
			if ((f32)(s8)mario->wStickDir1 > 0.0f) {
				s32 c = DATA->stallTimer + 1;

				DATA->stallTimer = c;
				if (c >= frames) {
					mario->field_0x2AC = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		}
	} else {
		DATA->stallTimer = 0;
	}
	mario->hitobjUnder = marioSearchUnder();
	if (chkCollision(&mario->field_0x2AC) != 0) {
		if (mario->hitobjHead != NULL) {
			mario->currSubMotionId = 0x28;
			return;
		}
		mario->currSubMotionId = 0x32;
		{
			HitEntry* hit = NULL;

			if (mario->hitobjFront != NULL) {
				hit = mario->hitobjFront;
			}
			if (mario->hitobjStandOn != NULL) {
				hit = mario->hitobjStandOn;
			}
			if (hit != NULL && (hitGetAttr(hit) & 0x200) != 0) {
				mario->currSubMotionId = 0x46;
			}
		}
	}
}

// The volatile accesses below reproduce the original's scheduling:
// the y store must stay volatile so the volatile Vec copies do not hoist above it, and the Vec
// copies are volatile so each loads/stores as an unscheduled pair.
void mot_plane_post(void) {
	MarioWork* mario = marioGetPtr();
	MarioWork* m;

	camFollowYOn();
	m = marioGetPtr();
	if (m->motStruct != NULL) {
		if (*(u32*)&((MotionPlaneData*)m->motStruct)->sfxId != 0xFFFFFFFFu) {
			psndSFXOff(((MotionPlaneData*)m->motStruct)->sfxId);
			((MotionPlaneData*)m->motStruct)->sfxId = -1;
		}
	}
	m = marioGetPtr();
#ifdef MUST_MATCH
	// Volatile store keeps the following volatile Vec loads below it, as the original schedules them.
	m->wPlayerCollisionBox.x = 20.0f;
	*(volatile f32*)&m->wPlayerCollisionBox.y = 37.0f;
#else
	m->wPlayerCollisionBox.x = 20.0f;
	m->wPlayerCollisionBox.y = 37.0f;
#endif

#ifdef MUST_MATCH
	// The original copies each Vec with an unscheduled per-component load/store pair.
	*(volatile Vec*)&m->dispRot = *(volatile Vec*)&vec3_802c5cd8;
	*(volatile Vec*)&m->dispPivot = *(volatile Vec*)&vec3_802c5ce4;
	*(volatile Vec*)&m->dispOfs = *(volatile Vec*)&vec3_802c5cf0;
#else
	m->dispRot = vec3_802c5cd8;
	m->dispPivot = vec3_802c5ce4;
	m->dispOfs = vec3_802c5cf0;
#endif
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
#ifdef MUST_MATCH
	{
		f32 zero = 0.0f;
		mario->unk138 = zero;
		mario->unk140 = zero;
	}
#else
	mario->unk138 = 0.0f;
	mario->unk140 = 0.0f;
#endif
	if (mario->motStruct != NULL) {
		__memFree(HEAP_DEFAULT, mario->motStruct);
		mario->motStruct = NULL;
	}
	allPartyRideOff2();
}

#ifdef MUST_MATCH
// Deferred generation pools the angle before the collision-box reset constants.
static f32 planePoolAngle(void) {
    return 270.0f;
}
#endif

// Compute the two plane collision points and test the flight path and ground.
s32 chkCollision(f32* out) {
	s32 flag = 0;
	MarioWork* mario = marioGetPtr();
	Vec dir;
	Vec normal;
	Vec v3C;
	Vec v30;
	f32 sinC, cosC;
	f32 sinA, cosA;
	f32 sinR, cosR;
	f32 ten;
	f32 hx, hy, hz;
	f32 dist;
	f32 nx, ny, nz;
	HitEntry* hit;

	sincosf(&sinA, &cosA, 90.0f + DATA->pitch);
	sincosf(&sinC, &cosC, DATA->heading);

	v3C = (Vec){ 0.0f, 0.0f, 0.0f };
	{
		f32 t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
		t1 = mario->dispOfs.x;
		t3 = mario->dispOfs.y;
		t0 = mario->position.x;
		t2 = mario->position.y;
		t4 = mario->dispPivot.x;
		t9 = -sinA;
		t10 = sinC * cosA;
		t11 = 4.0f;
		t12 = cosC * cosA;
		t5 = t0 + t1;
		t6 = t2 + t3;
		t4 = t4 + t5;
		t8 = mario->dispPivot.y + t6;
		t4 = t11 * t10 + t4;
		t8 = t9 * t11 + t8;
		t14 = mario->position.z;
		t13 = mario->dispOfs.z;
		t15 = t14 + t13;
		t13 = mario->dispPivot.z;
		t14 = t13 + t15;
		t14 = t11 * t12 + t14;
		v3C.x = t4;
		v3C.y = t8;
		v3C.z = t14;
	}
	DATA->field_0x3C = v3C;

	sincosf(&sinR, &cosR, DATA->bank);
	sincosf(&sinA, &cosA, DATA->pitch);
	mario->hitobjFront = NULL;
	{
		f32 vx, vy, vz;
		f32 pd40 = planeData[40];
		sincosf(&sinC, &cosC, DATA->heading);
		vx = mario->dispPivot.x + (mario->position.x + mario->dispOfs.x);
		vy = mario->dispPivot.y + (mario->position.y + mario->dispOfs.y);
		vz = mario->dispPivot.z + (mario->position.z + mario->dispOfs.z);
		vx = pd40 * (sinC * -cosA) + vx;
		vz = pd40 * (cosC * -cosA) + vz;
		vy = sinA * pd40 + vy;
		sincosf(&sinA, &cosA, DATA->pitch - 90.0f);
		vx = vx + (planeData[39] * (cosA * sinC) - planeData[39] * (sinR * cosC));
		vy = vy + planeData[39] * (sinA * cosR);
		v30 = (Vec){ 0.0f, 0.0f, 0.0f };
		v30.x = vx;
		v30.y = vy;
		vz = vz + (planeData[39] * (cosA * cosC) - planeData[39] * (sinR * sinC));
		v30.z = vz;
		DATA->field_0x30 = v30;
	}

	ten = 10.0f;
	{
		MotionPlaneData* d = DATA;
		f32 a0 = d->field_0x30.x - ten * sinC;
		f32 a2 = d->field_0x30.z - ten * cosC;
		f32 a1 = d->field_0x30.y;
		dist = ten + mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, a2, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		mario->hitobjFront = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = DATA->field_0x3C.x;
		f32 a1 = DATA->field_0x3C.y;
		f32 a2 = DATA->field_0x3C.z;
		dist = mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, a2, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		mario->hitobjFront = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = DATA->field_0x30.x;
		f32 a1 = DATA->field_0x30.y;
		f32 a2 = DATA->field_0x30.z;
		mario->hitobjStandOn = NULL;
		dist = 10.1f;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1 + ten, a2, 0.0f, -1.0f, 0.0f);
	}
	if (hit != NULL) {
		mario->hitobjStandOn = hit;
		flag = 1;
		*out = hy;
	}

	if (flag == 0) {
		{
			f32 a0 = DATA->field_0x3C.x;
			f32 a1 = DATA->field_0x3C.y;
			f32 a2 = DATA->field_0x3C.z;
			mario->hitobjStandOn = NULL;
			dist = 10.1f;
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1 + ten, a2, 0.0f, -1.0f, 0.0f);
		}
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = hy;
		}
	}

	if (flag == 0) {
		PSVECSubtract(&DATA->field_0x3C, &DATA->field_0x30, &dir);
		dist = PSVECMag(&dir);
		PSVECNormalize(&dir, &dir);
		hit = marioHitCheckVec(&DATA->field_0x30, &dir, &normal, NULL, &dist);
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = normal.y;
		}
	}

	mario->hitobjHead = NULL;
	if (mario->hitobjStandOn == NULL) {
		{
			f32 a0 = DATA->field_0x30.x;
			f32 a1 = DATA->field_0x30.y;
			f32 a2 = DATA->field_0x30.z;
			dist = 2.0f;
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1, a2, 0.0f, 1.0f, 0.0f);
		}
		if (hit != NULL) {
			mario->hitobjHead = hit;
			flag = 1;
			*out = hy;
		}
	}
	return flag;
}

#ifdef MUST_MATCH
// This is generated first in the deferred unit, pooling the collision radius before 90.
static f32 planePoolRadius(void) {
    return 4.0f;
}
#endif
