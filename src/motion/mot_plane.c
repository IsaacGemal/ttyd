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

// Raw MotionPlaneData accessors for fields the header does not name yet.
#define MF32(d, off) (*(f32*)((u8*)(d) + (off)))
#define MFVEC(d, off) (*(Vec*)((u8*)(d) + (off)))

// Raw MarioWork accessor for the unnamed Vec fields the original writes.
#define MWVEC(m, off) (*(Vec*)((u8*)(m) + (off)))
// MotionPlaneData fields the header does not name yet (macro expands to raw offsets).
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
		*(f32*)mario->field_0x2AC = mario->position.y;
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
const Vec vec3_802c5cfc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5d08 = { 0.0f, 0.0f, 0.0f };
// .rodata:0x90: the int->float conversion magic. Do NOT define it here: MWCC pools its own anonymous copy
// for the compiler-generated `(f32)(s8)...` conversions, and defining a named object too emits a second copy
// at 0x98 that shifts every following string by 8. Left undefined so the pooled copy lands at 0x90.
// .rodata:0x98..0xB8 ("M_J_1B", "PM_P_1A", "PM_P_1B", "M_J_1C"): used as string literals inside mot_plane so
// MWCC dedups them with marioForcePlaneAnime's "PM_P_1B" and pools them after the magic at 0x98..0xB8.

// .sdata2 constant pool in the original's address order (build/G8MJ01/asm/motion/mot_plane.s:2739),
// so every `@sda21` offset resolves the same. Read through MATCH_F32 so MWCC does not pool a copy.
const f32 float_4_8041ac00 = 4.0f;
const f32 float_90_8041ac04 = 90.0f;
const f32 float_10_8041ac08 = 10.0f;
const f32 float_0_8041ac0c = 0.0f;
const f32 float_10p1_8041ac10 = 10.1f;
const f32 float_neg1_8041ac14 = -1.0f;
const f32 float_2_8041ac18 = 2.0f;
const f32 float_1_8041ac1c = 1.0f;
const f32 float_270_8041ac20 = 270.0f;
const f32 float_20_8041ac24 = 20.0f;
const f32 float_37_8041ac28 = 37.0f;
const f32 float_60_8041ac2c = 60.0f;
const f32 float_0p1_8041ac30 = 0.1f;
const f32 float_150_8041ac34 = 150.0f;
const f32 float_330_8041ac38 = 330.0f;
const f32 float_210_8041ac3c = 210.0f;
const f32 float_3p1416_8041ac40 = 3.1415927f;
const f32 float_180_8041ac44 = 180.0f;
const f32 float_neg0p001_8041ac48 = -0.001f;
const f32 float_0p5_8041ac4c = 0.5f;
const f32 float_6_8041ac50 = 6.0f;
const f32 float_neg80_8041ac54 = -80.0f;
const f32 float_neg22_8041ac58 = -22.0f;
const f32 float_1000_8041ac5c = 1000.0f;
const f32 float_0p4_8041ac60 = 0.4f;
const f32 float_0p01_8041ac64 = 0.01f;
const f32 float_0p125_8041ac68 = 0.125f;
const f32 float_30_8041ac6c = 30.0f;
const f32 float_630_8041ac70 = 630.0f;
const f32 float_neg630_8041ac74 = -630.0f;
const char str_8041AC78[] = "M_Z_1";
const f32 float_1024_8041ac80 = 1024.0f;
const f32 float_1260_8041ac84 = 1260.0f;
const f32 float_neg1080_8041ac88 = -1080.0f;
const f32 float_12p333_8041ac8c = 12.333333f;
const f32 float_0p02_8041ac90 = 0.02f;
const f32 float_0p004_8041ac94 = 0.004f;
const f32 float_70_8041ac98 = 70.0f;
const f32 float_neg70_8041ac9c = -70.0f;
const f32 float_100_8041aca0 = 100.0f;
const f32 float_0p3_8041aca4 = 0.3f;
const f32 float_28_8041aca8 = 28.0f;
const f32 float_neg20_8041acac = -20.0f;
const f32 float_neg2_8041acb0 = -2.0f;

void marioForcePlaneAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(paper_plane[marioGetColor()]);
    marioChgPaper("PM_P_1B");
    marioChgPose(str_8041AC78);
    mario->dispFlags |= 0x8;
}

static void flyMain(void);

// Inlined reset block the original duplicates at every mot_plane exit (fields set to 20/37,
// zero vectors, paper off). Reproduces the same scheduling as mot_plane_post.
static inline void planeReset(void) {
	MarioWork* m = marioGetPtr();

	m->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
#ifdef MUST_MATCH
	// Volatile store/loads keep the following volatile Vec copies below it, as the original schedules them.
	*(volatile f32*)&m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
	*(volatile Vec*)((u8*)m + 0xBC) = *(volatile Vec*)&vec3_802c5cd8;
	*(volatile Vec*)((u8*)m + 0xB0) = *(volatile Vec*)&vec3_802c5ce4;
	*(volatile Vec*)((u8*)m + 0x98) = *(volatile Vec*)&vec3_802c5cf0;
#else
	m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
	MWVEC(m, 0xBC) = vec3_802c5cd8;
	MWVEC(m, 0xB0) = vec3_802c5ce4;
	MWVEC(m, 0x98) = vec3_802c5cf0;
#endif
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
}

// Full transcription of the plane state machine. Now byte-identical (100.00%): leaving the int->float
// "magic double" undefined lets MWCC's pooled anonymous copy land at .rodata+0x90, which is where the
// original keeps double_to_int_802c5d18; the four plane-name strings then sit at 0x98/0xA0/0xA8/0xB0 as
// in the original. The named f32 constants float_0p02/float_1/float_2 are referenced by their
// plain file-scope value (not MATCH_F32) so MWCC constant-folds them and schedules the loads exactly
// as the original does. Raw offsets used: mario
// 0x2A4/0x2AC/0x7C/0x80/0x84/0x88/0x18C/0x1F4/0x98/0xB0/0xBC and MotionPlaneData 0x4/0x8/0xC/0x10/0x14/0x2C.
void mot_plane(void) {
	MarioWork* mario = marioGetPtr();
	s32 wrap;

	if (mario->trigFlags & 1) {
		MotionPlaneData* data;
		mario->trigFlags &= ~1;
		mario->motStruct = __memAlloc(HEAP_DEFAULT, sizeof(MotionPlaneData));
		memset(mario->motStruct, 0, sizeof(MotionPlaneData));
		{
			MarioWork* m2 = marioGetPtr();
			data = m2->motStruct;
		}
		data->field_0x28 = -1;
		mario->currSubMotionId = 0;
	}

	switch (mario->currSubMotionId) {
	case 0: {
		MarioWork* m = marioGetPtr();
		s32 attr;

		m->unk148 = MATCH_F32(float_0p4_8041ac60);
		m->unk150.z = MATCH_F32(float_0p01_8041ac64);
		*(volatile f32*)&m->field_68 = float_2_8041ac18;
		// volatile keeps the store above the copy and the copy as a load-load-store-store pair (original order).
		*(volatile Vec*)&m->field_6C = *(volatile Vec*)&vec3_802c5c9c;
		marioPreJump();
		marioChgPose("M_J_1B");
		mario->dispFlags |= 0x100;
		mario->currSubMotionId = 1;
		mario->airTimer = 0;
		{
			f32 zero = MATCH_F32(float_0_8041ac0c);
			mario->baseSpeed = zero;
			MF32(mario, 0x2A4) = zero;
			mario->wCamVal1 = mario->position.y;
			MF32(mario, 0x2AC) = zero;
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
		k = MATCH_F32(float_0p125_8041ac68);
		m = MATCH_F32(float_30_8041ac6c);
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? float_1_8041ac1c : MATCH_F32(float_neg1_8041ac14);
		v = MF32(mario, 0x2A4);
		t = m * dir;
		MF32(mario, 0x2A4) = k * (t - v) + v;
		mario->unk1A8 += MF32(mario, 0x2A4);
		wrap = 0;
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
			if (mario->unk1A8 >= MATCH_F32(float_630_8041ac70)) {
				wrap = 1;
			}
		} else {
			if (mario->unk1A8 <= MATCH_F32(float_neg630_8041ac74)) {
				wrap = 1;
			}
		}
		if (wrap) {
			EventEntry* evt;
			mario->currSubMotionId = 2;
			mario->multiTimer = 0x14;
			marioPaperOn(paper_plane[marioGetColor()]);
			marioChgPaper("PM_P_1A");
			marioChgPose(str_8041AC78);
			mario->dispFlags |= 0x8;
			animPoseSetLocalTime(mario->paperPoseId, MATCH_F32(float_0_8041ac0c));
			animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], MATCH_F32(float_0_8041ac0c));
			evt = evtEntry(sound_evt, 0, 0);
			{
				f32 sc = MATCH_F32(float_1024_8041ac80);
				evt->lwData[0] = (s32)(sc * mario->position.x) - 0x0DB58580;
				evt->lwData[1] = (s32)(sc * mario->position.y) - 0x0DB58580;
				evt->lwData[2] = (s32)(sc * mario->position.z) - 0x0DB58580;
			}
		}
		if (MF32(mario, 0x7C) > MATCH_F32(float_0_8041ac0c)) {
			mario->position.y += MF32(mario, 0x7C);
		}
		break;
	}
	case 2: {
		f32 dir;
		f32 v;
		f32 k;
		f32 m;
		f32 t;

		animPoseSetLocalTime(mario->paperPoseId, MATCH_F32(float_0_8041ac0c));
		animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], MATCH_F32(float_0_8041ac0c));
		k = MATCH_F32(float_0p125_8041ac68);
		m = MATCH_F32(float_30_8041ac6c);
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? float_1_8041ac1c : MATCH_F32(float_neg1_8041ac14);
		v = MF32(mario, 0x2A4);
		t = m * dir;
		MF32(mario, 0x2A4) = k * (t - v) + v;
		mario->unk1A8 += MF32(mario, 0x2A4);
		wrap = 0;
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
			f32 lim = float_1260_8041ac84;
			if (mario->unk1A8 >= lim) {
				mario->unk1A8 = lim;
				wrap = 1;
			}
		} else {
			f32 lim = float_neg1080_8041ac88;
			if (mario->unk1A8 <= lim) {
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
		MF32(mario->motStruct, 0x8) = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? MATCH_F32(float_90_8041ac04)
		                                                               : MATCH_F32(float_270_8041ac20);
		{
			f32 zero = MATCH_F32(float_0_8041ac0c);
			MF32(mario, 0x2A4) = zero;
			MF32(mario->motStruct, 0x4) = zero;
			MF32(mario->motStruct, 0xC) = zero;
			MF32(mario->motStruct, 0x14) = zero;
			MF32(mario->motStruct, 0x10) = zero;
		}
		if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
			MF32(mario, 0xB0) = planeData[33];
			MF32(mario, 0x98) = planeData[34];
		} else {
			MF32(mario, 0xB0) = planeData[35];
			MF32(mario, 0x98) = planeData[36];
		}
		mario->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
		mario->wPlayerCollisionBox.y = MATCH_F32(float_12p333_8041ac8c);
		mario->baseSpeed = __fabsf((*(const f32*)&float_0p02_8041ac90) * (f32)(s8)mario->wStickDir1);
		mario->currSubMotionId = 0xC;
		mario->baseSpeed = float_1_8041ac1c;
		MF32(mario->motStruct, 0x10) = float_2_8041ac18;
		m = marioGetPtr();
		m->unk148 = MATCH_F32(float_0_8041ac0c);
		m->unk150.z = MATCH_F32(float_0p004_8041ac94);
		{
			f32 k70 = MATCH_F32(float_70_8041ac98);
			dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? MATCH_F32(float_neg1_8041ac14) : float_1_8041ac1c;
			mario->unk138 = k70 * dir;
		}
		mario->unk140 = MATCH_F32(float_neg70_8041ac9c);
		break;
	}
	default:
		break;
	}

	// Common per-frame tail.
	if (mario->position.y < MATCH_F32(float_100_8041aca0)) {
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
			f32 zero = MATCH_F32(float_0_8041ac0c);
			mario->unk140 = zero;
			mario->unk138 = zero;
		}
		marioClearJumpPara();
		mario->baseSpeed = MATCH_F32(float_0_8041ac0c);
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
				((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
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
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
				((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
			}
		}
		mario->multiTimer = sysMsec2Frame(0x208);
		mario->currSubMotionId = 0x29;
		break;
	}
	case 0x29:
		if (--mario->multiTimer <= 0) {
			f32 t;
			f32 a;
			f32 b;
			planeReset();
			marioChgPose("M_J_1C");
			a = MF32(mario, 0x2AC);
			b = (*(const f32*)&float_37_8041ac28);
			t = a - b;
			mario->position.y = t + float_1_8041ac1c;
			mario->baseSpeed = MATCH_F32(float_0_8041ac0c);
			marioSetFallPara();
			MF32(mario, 0x7C) = MATCH_F32(float_0_8041ac0c);
			{
				f32 half = float_0p5_8041ac4c;
				MF32(mario, 0x80) *= half;
				MF32(mario, 0x84) *= half;
				MF32(mario, 0x88) *= half;
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
		if (mario->baseSpeed > float_1_8041ac1c) {
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
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
				((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
			}
		}
		planeReset();
		mario->dispFlags &= ~0x100;
		{
			f32 zero = MATCH_F32(float_0_8041ac0c);
			mario->baseSpeed = zero;
			MF32(mario, 0x2A4) = zero;
		}
		mario->position.y = float_1_8041ac1c + MF32(mario, 0x2AC);
		marioPreJump();
		{
			f32 half = float_0p5_8041ac4c;
			MF32(mario, 0x7C) = MF32(mario, 0x7C) * MATCH_F32(float_0p5_8041ac4c);
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
		k3 = MATCH_F32(float_0p3_8041aca4);
		k28 = MATCH_F32(float_28_8041aca8);
		dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? float_1_8041ac1c : MATCH_F32(float_neg1_8041ac14);
		v = MF32(mario, 0x2A4);
		MF32(mario, 0x2A4) = k3 * (k28 * dir - v) + v;
		mario->unk1A8 = revise360(mario->unk1A8 + MF32(mario, 0x2A4));
		marioMakeJumpPara();
		mario->position.y += MF32(mario, 0x7C);
		marioChkOverhead();
		if (MF32(mario, 0x7C) < MATCH_F32(float_0_8041ac0c) || *(void**)((u8*)mario + 0x1F4) != NULL) {
			f32 half;
			marioSetFallPara();
			half = float_0p5_8041ac4c;
			MF32(mario, 0x7C) *= half;
			MF32(mario, 0x80) *= half;
			MF32(mario, 0x84) *= half;
			MF32(mario, 0x88) *= half;
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
			k3 = MATCH_F32(float_0p3_8041aca4);
			k28 = MATCH_F32(float_28_8041aca8);
			dir = (((MotionPlaneData*)mario->motStruct)->flags & 1) ? float_1_8041ac1c : MATCH_F32(float_neg1_8041ac14);
			v = MF32(mario, 0x2A4);
			MF32(mario, 0x2A4) = k3 * (k28 * dir - v) + v;
			mario->unk1A8 = revise360(mario->unk1A8 + MF32(mario, 0x2A4));
			if (--mario->multiTimer <= 0) {
				mario->dispFlags &= ~0x100;
			}
		}
		marioMakeJumpPara();
		{
			f32 neg20 = float_neg20_8041acac;
			if (MF32(mario, 0x7C) <= neg20) {
				MF32(mario, 0x7C) = neg20;
			}
		}
		mario->hitobjStandOn = NULL;
		{
			f32 sy;
			f32 sx;
			f32 sz;
			f32 y = MF32(mario, 0x7C);
			HitEntry* hit = marioSearchGround(&sy, &sx, &sz, y, y);

			if (hit != NULL) {
				mario->position.y = sy;
				mario->hitobjStandOn = hit;
			} else {
				mario->position.y += MF32(mario, 0x7C);
			}
		}
		if (mario->hitobjStandOn != NULL) {
			camFollowYOn();
			allPartyRideOff2();
			mario->airTimer = 0;
			*(s16*)mario->field_0x52 = 0;
			mario->flags &= ~0x80;
			marioAdjustMoveDir();
			planeReset();
			if (mario->motStruct != NULL) {
				__memFree(HEAP_DEFAULT, mario->motStruct);
				mario->motStruct = NULL;
			}
			{
				f32 zero = MATCH_F32(float_0_8041ac0c);
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
			f32 neg2 = MATCH_F32(float_neg2_8041acb0);
			f32 zero = float_0_8041ac0c;
			MF32(mario, 0x7C) = neg2;
			MF32(mario, 0x80) = zero;
			MF32(mario, 0x84) = zero;
			MF32(mario, 0x88) = zero;
		}
		camFollowYOff();
		/* fall through */
	case 0x47:
		marioMakeJumpPara();
		mario->position.y += MF32(mario, 0x7C);
		if (mario->position.y >= MF32(mario, 0x2AC) - MATCH_F32(float_100_8041aca0)) {
			break;
		}
		mario->currSubMotionId = 0x48;
		/* fall through */
	case 0x48: {
		MarioWork* m;
		f32 zero = MATCH_F32(float_0_8041ac0c);

		mario->unk140 = zero;
		mario->unk138 = zero;
		marioClearJumpPara();
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
				psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
				((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
			}
		}
		mario->baseSpeed = MATCH_F32(float_0_8041ac0c);
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

// TEMP-LOCAL: keep flyMain a real bl while it is a stub (retail emits one).
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
			if ((u32)((MotionPlaneData*)m->motStruct)->field_0x28 == 0xFFFFFFFFu) {
				((MotionPlaneData*)m->motStruct)->field_0x28 =
				    psndSFXOn_3D((const char*)0x17F, &m->position);
			}
			/* fallthrough */
		case 0xE:
			if (MF32(mario, 0x9C) > planeData[38]) {
				MF32(mario, 0x9C) -= float_2_8041ac18;
				MF32(mario, 0xB4) += float_2_8041ac18;
				mario->position.y += float_2_8041ac18;
			} else {
				mario->currSubMotionId = 0xF;
			}
			break;
	}
	if ((s32)mario->currSubMotionId != 0xF) {
		return;
	}
	if (*(u16*)&mario->field_0x245[0x248 - 0x245] & 0x100) {
		mario->currSubMotionId = 0x33;
		MF32(mario, 0x2AC) = mario->position.y;
		return;
	}
	if (*(u16*)&mario->field_0x245[0x248 - 0x245] & 0x200) {
		mario->currSubMotionId = 0x33;
		MF32(mario, 0x2AC) = mario->position.y;
		return;
	}
	MF32(DATA, 0x8) = revise360(MF32(DATA, 0xC) * planeData[14] + MF32(DATA, 0x8));
	if (DATA->flags & 1) {
		if (MF32(DATA, 0x8) <= (*(const f32*)&float_60_8041ac2c)) {
			MF32(DATA, 0x8) = (*(const f32*)&float_0p1_8041ac30) * ((*(const f32*)&float_60_8041ac2c) - MF32(DATA, 0x8)) + MF32(DATA, 0x8);
		}
		if (MF32(DATA, 0x8) >= (*(const f32*)&float_150_8041ac34)) {
			MF32(DATA, 0x8) = (*(const f32*)&float_0p1_8041ac30) * ((*(const f32*)&float_150_8041ac34) - MF32(DATA, 0x8)) + MF32(DATA, 0x8);
		}
	} else {
		if (MF32(DATA, 0x8) >= (*(const f32*)&float_330_8041ac38)) {
			MF32(DATA, 0x8) = (*(const f32*)&float_0p1_8041ac30) * ((*(const f32*)&float_330_8041ac38) - MF32(DATA, 0x8)) + MF32(DATA, 0x8);
		}
		if (MF32(DATA, 0x8) <= (*(const f32*)&float_210_8041ac3c)) {
			MF32(DATA, 0x8) = (*(const f32*)&float_0p1_8041ac30) * ((*(const f32*)&float_210_8041ac3c) - MF32(DATA, 0x8)) + MF32(DATA, 0x8);
		}
	}
	{
		f32 a = MF32(DATA, 0x8);
		f32 v;
		s32 flag = 0;

		if (a >= MATCH_F32(float_90_8041ac04) && a <= (*(const f32*)&float_270_8041ac20)) {
			flag = 1;
		}
		if (flag) {
			v = -a - MATCH_F32(float_90_8041ac04);
		} else {
			v = (*(const f32*)&float_270_8041ac20) - a;
		}
		mario->unk1A8 = revise360(v);
	}
	{
		u8 stick = mario->wStickDir1;

		if ((s8)(stick & 0xff) != 0) {
			if (mario->baseSpeed != MATCH_F32(float_0_8041ac0c)) {
				f3 = __fabsf((f32)(s8)mario->wStickDir1);
				if (DATA->flags & 1) {
					if ((s8)mario->wStickDir1 < 0) {
						if (MF32(DATA, 0x4) >= planeData[3]) {
							MF32(DATA, 0x4) = (*(const f32*)&float_0p1_8041ac30) * (planeData[3] - MF32(DATA, 0x4)) + MF32(DATA, 0x4);
						} else {
							MF32(DATA, 0x4) = (*(const f32*)&float_0p1_8041ac30) * (f3 * planeData[5]) + MF32(DATA, 0x4);
						}
					}
					if ((s8)mario->wStickDir1 > 0) {
						if (MF32(DATA, 0x4) <= planeData[4]) {
							MF32(DATA, 0x4) = planeData[4];
						} else {
							MF32(DATA, 0x4) = MF32(DATA, 0x4) - (*(const f32*)&float_0p1_8041ac30) * (f3 * planeData[6]);
						}
					}
				} else {
					if ((s8)mario->wStickDir1 > 0) {
						if (MF32(DATA, 0x4) >= planeData[3]) {
							MF32(DATA, 0x4) = (*(const f32*)&float_0p1_8041ac30) * (planeData[3] - MF32(DATA, 0x4)) + MF32(DATA, 0x4);
						} else {
							MF32(DATA, 0x4) = (*(const f32*)&float_0p1_8041ac30) * (f3 * planeData[5]) + MF32(DATA, 0x4);
						}
					}
					if ((s8)mario->wStickDir1 < 0) {
						if (MF32(DATA, 0x4) <= planeData[4]) {
							MF32(DATA, 0x4) = planeData[4];
						} else {
							MF32(DATA, 0x4) = MF32(DATA, 0x4) - (*(const f32*)&float_0p1_8041ac30) * (f3 * planeData[6]);
						}
					}
				}
			}
		} else {
			if (MF32(DATA, 0x4) <= float_0_8041ac0c || MF32(DATA, 0x14) <= float_0_8041ac0c) {
				if (MF32(DATA, 0x14) > float_0_8041ac0c && MF32(DATA, 0x14) < float_1_8041ac1c) {
					MF32(DATA, 0x4) = MATCH_F32(MF32(DATA, 0x4)) + planeData[30];
				}
				{
					f31 = planeData[0] - mario->baseSpeed;
					if (f31 <= float_0_8041ac0c) {
						f31 = float_0_8041ac0c;
					}
					f31 = f31 * (MATCH_F32(float_90_8041ac04) / planeData[0]);
					f31 = planeData[27] * (f32)sin((*(const f32*)&float_3p1416_8041ac40) * f31 / (*(const f32*)&float_180_8041ac44));
					if (f31 <= planeData[28]) {
						f31 = planeData[28];
					}
				}
				onPlace = 0;
				m = marioGetPtr();
				if (*(HitEntry**)((u8*)m + 0x1F8) != NULL && (hitGetAttr(*(HitEntry**)((u8*)m + 0x1F8)) & 0x38) != 0) {
					onPlace = 1;
				}
				if (onPlace == 0) {
					MF32(DATA, 0x1C) = f31;
					MF32(DATA, 0x4) = MF32(DATA, 0x4) + MF32(DATA, 0x1C);
				} else {
					MF32(DATA, 0x4) += (*(const f32*)&float_neg0p001_8041ac48);
					mario->baseSpeed += float_0p5_8041ac4c;
					if (mario->baseSpeed > float_2_8041ac18) {
						mario->baseSpeed = float_2_8041ac18;
					}
				}
			}
			if (MF32(DATA, 0x4) <= planeData[4]) {
				MF32(DATA, 0x4) = planeData[4];
			}
			if (MF32(DATA, 0x4) >= (*(const f32*)&float_6_8041ac50)) {
				MF32(DATA, 0x14) = float_0_8041ac0c;
			}
		}
	}
	mario->baseSpeed = mario->baseSpeed * planeData[1] + mario->baseSpeed;
	{
		MotionPlaneData* dv = DATA;

		if (MF32(dv, 0x4) >= float_0_8041ac0c) {
			dv->flags |= 0x10;
			{
				f32 s = sin((*(const f32*)&float_3p1416_8041ac40) * (MF32(DATA, 0x4) * planeData[7]) / (*(const f32*)&float_180_8041ac44));
				f32 x = planeData[8] * s;

				x = mario->baseSpeed * x;
				mario->baseSpeed = mario->baseSpeed + x;
			}
			if ((s8)mario->wStickDir1 != 0 && MF32(DATA, 0x4) >= planeData[3]) {
				mario->baseSpeed = mario->baseSpeed + planeData[2];
			}
			if (mario->baseSpeed <= float_0_8041ac0c) {
				mario->baseSpeed = float_0_8041ac0c;
			}
			f31 = float_0_8041ac0c;
			if ((s8)mario->wStickDir1 == 0) {
				if (mario->baseSpeed < planeData[20]) {
					f32 s, a;

					f31 = planeData[0] - mario->baseSpeed;
					s = sin((*(const f32*)&float_3p1416_8041ac40) * MF32(DATA, 0x4) / (*(const f32*)&float_180_8041ac44));
					a = planeData[21] * s;
					f31 = f31 * a;
					f31 = f31 + MF32(DATA, 0x14);
					MF32(DATA, 0x14) = f31;
				} else {
					MF32(DATA, 0x14) = planeData[22];
				}
				MF32(DATA, 0x4) = MF32(DATA, 0x4) + MF32(DATA, 0x14);
				if (f31 <= float_0_8041ac0c) {
					if (mario->baseSpeed <= planeData[0] * planeData[23]) {
						f31 = planeData[0] - mario->baseSpeed * planeData[24];
						if (f31 <= float_0_8041ac0c) {
							f31 = float_0_8041ac0c;
						}
						f31 = f31 * (MATCH_F32(float_90_8041ac04) / planeData[0]);
						onPlace = 0;
						m = marioGetPtr();
						if (*(HitEntry**)((u8*)m + 0x1F8) != NULL && (hitGetAttr(*(HitEntry**)((u8*)m + 0x1F8)) & 0x38) != 0) {
							onPlace = 1;
						}
						if (onPlace == 0) {
							f32 v = planeData[25] * (f32)sin((*(const f32*)&float_3p1416_8041ac40) * f31 / (*(const f32*)&float_180_8041ac44));

							if (v <= planeData[26]) {
								v = float_2_8041ac18 * planeData[26];
							}
							MF32(DATA, 0x1C) = v;
						}
					}
				}
				MF32(DATA, 0x4) = MF32(DATA, 0x4) + MF32(DATA, 0x1C);
			}
			{
				f32 add;

				if ((s8)mario->wStickDir1 != 0) {
					f32 s = sin((*(const f32*)&float_3p1416_8041ac40) * (planeData[0] - mario->baseSpeed) / (*(const f32*)&float_180_8041ac44));

					add = planeData[18] * s;
					MF32(DATA, 0x18) = MF32(DATA, 0x18) + add;
					if (MF32(DATA, 0x18) <= planeData[19]) {
						MF32(DATA, 0x18) = planeData[19];
					}
				} else {
					add = (*(const f32*)&float_0p1_8041ac30) * -MF32(DATA, 0x18) + MF32(DATA, 0x18);
					MF32(DATA, 0x18) = add;
				}
				MF32(DATA, 0x4) = MF32(DATA, 0x4) + add;
			}
		} else if (MF32(dv, 0x4) >= (*(const f32*)&float_neg80_8041ac54)) {
			if (dv->flags & 0x10) {
				dv->flags = dv->flags & ~0x10;
			}
			{
				f32 s = sin((*(const f32*)&float_3p1416_8041ac40) * (planeData[9] * __fabsf(MF32(DATA, 0x4))) / (*(const f32*)&float_180_8041ac44));

				MF32(DATA, 0x10) = planeData[10] * s;
				mario->baseSpeed = mario->baseSpeed + MF32(DATA, 0x10);
			}
			f3 = float_0_8041ac0c;
			if (MF32(DATA, 0x4) > 0.0f) {
				f3 = float_0_8041ac0c;
			} else {
				f3 = __fabsf(MF32(DATA, 0x4));
			}
			{
				f32 x, y;

				f31 = sin((*(const f32*)&float_3p1416_8041ac40) * (planeData[12] * f3) / (*(const f32*)&float_180_8041ac44));
				y = sin((*(const f32*)&float_3p1416_8041ac40) * (mario->baseSpeed * planeData[11]) / (*(const f32*)&float_180_8041ac44));
				x = planeData[13] * (y * f31);
				MF32(DATA, 0x14) = MF32(DATA, 0x14) + x;
			}
			if ((s8)(mario->wStickDir1 & 0xff) != 0) {
				if (DATA->flags & 1) {
					if ((s8)mario->wStickDir1 > 0 && MF32(DATA, 0x14) >= float_1_8041ac1c) {
						MF32(DATA, 0x14) = float_1_8041ac1c;
					}
				} else {
					if ((s8)mario->wStickDir1 < 0 && MF32(DATA, 0x14) >= float_1_8041ac1c) {
						MF32(DATA, 0x14) = float_1_8041ac1c;
					}
				}
				onPlace = 0;
				m = marioGetPtr();
				if (*(HitEntry**)((u8*)m + 0x1F8) != NULL && (hitGetAttr(*(HitEntry**)((u8*)m + 0x1F8)) & 0x38) != 0) {
					onPlace = 1;
				}
				if (onPlace == 0) {
					MF32(DATA, 0x1C) += (*(const f32*)&float_0p1_8041ac30) * (float_1_8041ac1c - MF32(DATA, 0x1C));
					if (MF32(DATA, 0x1C) >= float_0_8041ac0c) {
						MF32(DATA, 0x1C) = float_0_8041ac0c;
					}
				}
			}
			onPlace = 0;
			m = marioGetPtr();
			if (*(HitEntry**)((u8*)m + 0x1F8) != NULL && (hitGetAttr(*(HitEntry**)((u8*)m + 0x1F8)) & 0x38) != 0) {
				onPlace = 1;
			}
			if (onPlace != 0) {
				MF32(DATA, 0x24) = (*(const f32*)&float_neg22_8041ac58);
			} else {
				MF32(DATA, 0x24) += (*(const f32*)&float_0p1_8041ac30) * (planeData[4] - MF32(DATA, 0x24));
			}
			MF32(DATA, 0x4) = MF32(DATA, 0x4) + MF32(DATA, 0x14);
			if (MF32(DATA, 0x4) <= MF32(DATA, 0x24)) {
				MF32(DATA, 0x4) = MF32(DATA, 0x24);
			}
			MF32(DATA, 0x18) = (*(const f32*)&float_0p1_8041ac30) * -MF32(DATA, 0x18) + MF32(DATA, 0x18);
		}
	}
	if (mario->baseSpeed >= planeData[0]) {
		mario->baseSpeed = planeData[0];
	}
	MF32(mario, 0x90) = MF32(mario, 0x90) + MF32(DATA, 0x18);
	MF32(mario, 0x90) = MF32(mario, 0x90) - planeData[29];
	MF32(mario, 0xC4) = revise360(-MF32(DATA, 0x4));
	{
		f32 sin2, cos2, sin1, cos1;

		mario->directionView = MF32(DATA, 0x8);
		sincosf(&sin1, &cos1, mario->directionView);
		MF32(mario, 0x94) = mario->baseSpeed * cos1 + MF32(mario, 0x94);
		sincosf(&sin2, &cos2, MATCH_F32(float_90_8041ac04) + MF32(DATA, 0x4));
		MF32(mario, 0x90) = mario->baseSpeed * cos2 * (cos2 >= float_0_8041ac0c ? planeData[31] : planeData[32]) + MF32(mario, 0x90);
		MF32(mario, 0x8C) = sin2 * (mario->baseSpeed * sin1) + MF32(mario, 0x8C);
	}
	if (mario->baseSpeed <= planeData[16]) {
		s32 frames = sysMsec2Frame((s32)((*(const f32*)&float_1000_8041ac5c) * planeData[17]));

		if (DATA->flags & 1) {
			if ((f32)(s8)mario->wStickDir1 < float_0_8041ac0c) {
				s32 c = *(s32*)&DATA->field_0x4[0x20 - 0x4] + 1;

				*(s32*)&DATA->field_0x4[0x20 - 0x4] = c;
				if (c >= frames) {
					MF32(mario, 0x2AC) = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		} else {
			if ((f32)(s8)mario->wStickDir1 > float_0_8041ac0c) {
				s32 c = *(s32*)&DATA->field_0x4[0x20 - 0x4] + 1;

				*(s32*)&DATA->field_0x4[0x20 - 0x4] = c;
				if (c >= frames) {
					MF32(mario, 0x2AC) = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		}
	} else {
		*(s32*)&DATA->field_0x4[0x20 - 0x4] = 0;
	}
	*(HitEntry**)((u8*)mario + 0x1F8) = marioSearchUnder();
	if (chkCollision((f32*)((u8*)mario + 0x2AC)) != 0) {
		if (*(HitEntry**)((u8*)mario + 0x1F4) != NULL) {
			mario->currSubMotionId = 0x28;
			return;
		}
		mario->currSubMotionId = 0x32;
		{
			HitEntry* hit = NULL;

			if (*(HitEntry**)((u8*)mario + 0x1E0) != NULL) {
				hit = *(HitEntry**)((u8*)mario + 0x1E0);
			}
			if (*(HitEntry**)((u8*)mario + 0x1E4) != NULL) {
				hit = *(HitEntry**)((u8*)mario + 0x1E4);
			}
			if (hit != NULL && (hitGetAttr(hit) & 0x200) != 0) {
				mario->currSubMotionId = 0x46;
			}
		}
	}
}

// Matched at 100%. The MATCH_F32() and volatile accesses below reproduce the original's scheduling:
// the y store must stay volatile so the volatile Vec copies do not hoist above it, and the Vec
// copies are volatile so each loads/stores as an unscheduled pair.
void mot_plane_post(void) {
	MarioWork* mario = marioGetPtr();
	MarioWork* m;

	camFollowYOn();
	m = marioGetPtr();
	if (m->motStruct != NULL) {
		if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
			psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
			((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
		}
	}
	m = marioGetPtr();
#ifdef MUST_MATCH
	// Volatile store keeps the following volatile Vec loads below it, as the original schedules them.
	m->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
	*(volatile f32*)&m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
#else
	m->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
	m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
#endif

#ifdef MUST_MATCH
	// The original copies each Vec with an unscheduled per-component load/store pair.
	*(volatile Vec*)((u8*)m + 0xBC) = *(volatile Vec*)&vec3_802c5cd8;
	*(volatile Vec*)((u8*)m + 0xB0) = *(volatile Vec*)&vec3_802c5ce4;
	*(volatile Vec*)((u8*)m + 0x98) = *(volatile Vec*)&vec3_802c5cf0;
#else
	MWVEC(m, 0xBC) = vec3_802c5cd8;
	MWVEC(m, 0xB0) = vec3_802c5ce4;
	MWVEC(m, 0x98) = vec3_802c5cf0;
#endif
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
#ifdef MUST_MATCH
	{
		f32 zero = MATCH_F32(float_0_8041ac0c);
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

// The v3C block and the five marioHitCheck calls match. Remaining diffs (98.79%) are
// register/schedule only: the v30.y fmadds result lands in f1 instead of f30, which also
// puts ten in f3 instead of f7, and the first marioHitCheck hoists dist/a1 before a0/a2.
// NONMATCHING (98.79%): every constant, call argument and data reference is correct.
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

	sincosf(&sinA, &cosA, MATCH_F32(float_90_8041ac04) + MF32(DATA, 0x4));
	sincosf(&sinC, &cosC, MF32(DATA, 0x8));

	v3C = vec3_802c5cfc;
	{
		f32 t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
		t1 = MF32(mario, 0x98);
		t3 = MF32(mario, 0x9C);
		t0 = mario->position.x;
		t2 = mario->position.y;
		t4 = MF32(mario, 0xB0);
		t9 = -sinA;
		t10 = sinC * cosA;
		t11 = 4.0f;
		t12 = cosC * cosA;
		t5 = t0 + t1;
		t6 = t2 + t3;
		t4 = t4 + t5;
		t8 = MF32(mario, 0xB4) + t6;
		t4 = t11 * t10 + t4;
		t8 = t9 * t11 + t8;
		t14 = mario->position.z;
		t13 = MF32(mario, 0xA0);
		t15 = t14 + t13;
		t13 = MF32(mario, 0xB8);
		t14 = t13 + t15;
		t14 = t11 * t12 + t14;
		v3C.x = t4;
		v3C.y = t8;
		v3C.z = t14;
	}
	MFVEC(DATA, 0x3C) = v3C;

	sincosf(&sinR, &cosR, MF32(DATA, 0xC));
	sincosf(&sinA, &cosA, MF32(DATA, 0x4));
	*(HitEntry**)((u8*)mario + 0x1E0) = NULL;
	{
		f32 vx, vy, vz;
		f32 pd40 = planeData[40];
		sincosf(&sinC, &cosC, MF32(DATA, 0x8));
		vx = MF32(mario, 0xB0) + (mario->position.x + MF32(mario, 0x98));
		vy = MF32(mario, 0xB4) + (mario->position.y + MF32(mario, 0x9C));
		vz = MF32(mario, 0xB8) + (mario->position.z + MF32(mario, 0xA0));
		vx = pd40 * (sinC * -cosA) + vx;
		vz = pd40 * (cosC * -cosA) + vz;
		vy = sinA * pd40 + vy;
		sincosf(&sinA, &cosA, MF32(DATA, 0x4) - MATCH_F32(float_90_8041ac04));
		vx = vx + (planeData[39] * (cosA * sinC) - planeData[39] * (sinR * cosC));
		vy = vy + planeData[39] * (sinA * cosR);
		v30 = vec3_802c5d08;
		v30.x = vx;
		v30.y = vy;
		vz = vz + (planeData[39] * (cosA * cosC) - planeData[39] * (sinR * sinC));
		v30.z = vz;
		MFVEC(DATA, 0x30) = v30;
	}

	ten = 10.0f;
	{
		MotionPlaneData* d = DATA;
		f32 a0 = MF32(d, 0x30) - ten * sinC;
		f32 a2 = MF32(d, 0x38) - ten * cosC;
		f32 a1 = MF32(d, 0x34);
		dist = ten + mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, a2, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		*(HitEntry**)((u8*)mario + 0x1E0) = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = MF32(DATA, 0x3C);
		f32 a1 = MF32(DATA, 0x40);
		f32 a2 = MF32(DATA, 0x44);
		dist = mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, a2, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		*(HitEntry**)((u8*)mario + 0x1E0) = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = MF32(DATA, 0x30);
		f32 a1 = MF32(DATA, 0x34);
		f32 a2 = MF32(DATA, 0x38);
		mario->hitobjStandOn = NULL;
		dist = (*(const f32*)&float_10p1_8041ac10);
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1 + ten, a2, 0.0f, MATCH_F32(float_neg1_8041ac14), 0.0f);
	}
	if (hit != NULL) {
		mario->hitobjStandOn = hit;
		flag = 1;
		*out = hy;
	}

	if (flag == 0) {
		{
			f32 a0 = MF32(DATA, 0x3C);
			f32 a1 = MF32(DATA, 0x40);
			f32 a2 = MF32(DATA, 0x44);
			mario->hitobjStandOn = NULL;
			dist = (*(const f32*)&float_10p1_8041ac10);
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1 + ten, a2, 0.0f, MATCH_F32(float_neg1_8041ac14), 0.0f);
		}
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = hy;
		}
	}

	if (flag == 0) {
		PSVECSubtract(&MFVEC(DATA, 0x3C), &MFVEC(DATA, 0x30), &dir);
		dist = PSVECMag(&dir);
		PSVECNormalize(&dir, &dir);
		hit = marioHitCheckVec(&MFVEC(DATA, 0x30), &dir, &normal, NULL, &dist);
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = normal.y;
		}
	}

	*(HitEntry**)((u8*)mario + 0x1F4) = NULL;
	if (mario->hitobjStandOn == NULL) {
		{
			f32 a0 = MF32(DATA, 0x30);
			f32 a1 = MF32(DATA, 0x34);
			f32 a2 = MF32(DATA, 0x38);
			dist = float_2_8041ac18;
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1, a2, 0.0f, MATCH_F32(float_1_8041ac1c), 0.0f);
		}
		if (hit != NULL) {
			*(HitEntry**)((u8*)mario + 0x1F4) = hit;
			flag = 1;
			*out = hy;
		}
	}
	return flag;
}

