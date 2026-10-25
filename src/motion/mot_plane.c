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

// vec3_802c5c9c (.rodata) is defined at the source position that reproduces the original .rodata order.

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
	if (mario->motionPlaneData->flags & 1) {
		return 1;
	}
	return -1;
}

s32 marioGetPlaneStatus(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if (mario->currSubMotionId >= 0xC && mario->currSubMotionId < 0x14) {
		return 2;
	}
	return 1;
}

s32 mario_plane_cancel(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if (mario->currSubMotionId >= 0xC && mario->currSubMotionId < 0x14) {
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

// Inlined reset block the original duplicates at every mot_plane exit and in mot_plane_post (fields
// set to 20/37, zero vectors, paper off).
static inline void planeReset(void) {
	MarioWork* m = marioGetPtr();

	m->wPlayerCollisionBox.x = 20.0f;
	m->wPlayerCollisionBox.y = 37.0f;
	m->dispRot = (Vec){ 0.0f, 0.0f, 0.0f };
	m->dispPivot = (Vec){ 0.0f, 0.0f, 0.0f };
	m->dispOfs = (Vec){ 0.0f, 0.0f, 0.0f };
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
}

// Plane state machine.
void mot_plane(void) {
	MarioWork* mario = marioGetPtr();
	s32 wrap;

	if (mario->trigFlags & 1) {
		mario->trigFlags &= ~1;
		mario->motStruct = __memAlloc(HEAP_DEFAULT, sizeof(MotionPlaneData));
		memset(mario->motStruct, 0, sizeof(MotionPlaneData));
		{
			MarioWork* m = marioGetPtr();
			m->motionPlaneData->sfxId = -1;
		}
		mario->currSubMotionId = 0;
	}

	switch (mario->currSubMotionId) {
	case 0: {
		MarioWork* m = marioGetPtr();
		s32 attr;

		m->camZoomWeightTarget = 0.4f;
		m->camShiftRate.z = 0.01f;
#ifdef MUST_MATCH
		// Keep the store above the per-component vector copy, as in the original.
		*(volatile f32*)&m->camZoom = 2.0f;
		*(volatile Vec*)&m->camZoomFocusOfs = *(volatile Vec*)&vec3_802c5c9c;
#else
		m->camZoom = 2.0f;
		m->camZoomFocusOfs = vec3_802c5c9c;
#endif
		marioPreJump();
		marioChgPose("M_J_1B");
		mario->dispFlags |= 0x100;
		mario->currSubMotionId = 1;
		mario->airTimer = 0;
		mario->baseSpeed = 0.0f;
		mario->planeTurnRate = 0.0f;
		mario->wCamVal1 = mario->position.y;
		mario->field_0x2AC = 0.0f;
		attr = hitGetAttr(mario->hitobjJumpFrom);
		mario->motionPlaneData->flags = (attr >> 4) & 1;
		psndSFXOn_3D((char*)0x17d, &mario->position);
		psndSFXOn_3D((char*)0xaf, &mario->position);
	}
		/* fall through */
	case 1: {
		f32 dir;
		f32 t;

		marioMakeJumpPara();
		dir = (mario->motionPlaneData->flags & 1) ? 1.0f : -1.0f;
		t = 30.0f * dir;
		mario->planeTurnRate += 0.125f * (t - mario->planeTurnRate);
		mario->dispDir += mario->planeTurnRate;
		wrap = 0;
		if (mario->motionPlaneData->flags & 1) {
			if (mario->dispDir >= 630.0f) {
				wrap = 1;
			}
		} else {
			if (mario->dispDir <= -630.0f) {
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
			evt->lwData[0] = (s32)(1024.0f * mario->position.x) - 0x0DB58580;
			evt->lwData[1] = (s32)(1024.0f * mario->position.y) - 0x0DB58580;
			evt->lwData[2] = (s32)(1024.0f * mario->position.z) - 0x0DB58580;
		}
		if (mario->jumpVelY > 0.0f) {
			mario->position.y += mario->jumpVelY;
		}
		break;
	}
	case 2: {
		f32 dir;
		f32 t;

		animPoseSetLocalTime(mario->paperPoseId, 0.0f);
		animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], 0.0f);
		dir = (mario->motionPlaneData->flags & 1) ? 1.0f : -1.0f;
		t = 30.0f * dir;
		mario->planeTurnRate += 0.125f * (t - mario->planeTurnRate);
		mario->dispDir += mario->planeTurnRate;
		wrap = 0;
		if (mario->motionPlaneData->flags & 1) {
			if (mario->dispDir >= 1260.0f) {
				mario->dispDir = 1260.0f;
				wrap = 1;
			}
		} else {
			if (mario->dispDir <= -1080.0f) {
				mario->dispDir = -1080.0f;
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
		mario->motionPlaneData->heading = (mario->motionPlaneData->flags & 1) ? 90.0f
		                                                               : 270.0f;
		mario->planeTurnRate = 0.0f;
		mario->motionPlaneData->pitch = 0.0f;
		mario->motionPlaneData->bank = 0.0f;
		mario->motionPlaneData->field_0x14 = 0.0f;
		mario->motionPlaneData->field_0x10 = 0.0f;
		if (mario->motionPlaneData->flags & 1) {
			mario->dispPivot.x = planeData[33];
			mario->dispOfs.x = planeData[34];
		} else {
			mario->dispPivot.x = planeData[35];
			mario->dispOfs.x = planeData[36];
		}
		mario->wPlayerCollisionBox.x = 20.0f;
		mario->wPlayerCollisionBox.y = 12.333333f;
		mario->baseSpeed = __fabsf(0.02f * (f32)mario->wStickDir1);
		mario->currSubMotionId = 0xC;
		mario->baseSpeed = 1.0f;
		mario->motionPlaneData->field_0x10 = 2.0f;
		m = marioGetPtr();
		m->camZoomWeightTarget = 0.0f;
		m->camShiftRate.z = 0.004f;
		{
			f32 k70 = 70.0f;
			dir = (mario->motionPlaneData->flags & 1) ? -1.0f : 1.0f;
			mario->camShiftSideTarget = k70 * dir;
		}
		mario->camShiftUpTarget = -70.0f;
		break;
	}
	default:
		break;
	}

	// Common per-frame tail.
	if (mario->position.y < 100.0f) {
		mario->camShiftUpTarget = __fabsf(planeData[38]);
	}
	flyMain();
	if (marioKeyOffChk() && !(mario->trigFlags & 4)) {
		if (mario_plane_cancel()) {
			return;
		}
	}
	if (mario->currSubMotionId == 0x14 || mario->currSubMotionId == 0x1E) {
		MarioWork* m;
		camFollowYOff();
		mario->camShiftUpTarget = 0.0f;
		mario->camShiftSideTarget = 0.0f;
		marioClearJumpPara();
		mario->baseSpeed = 0.0f;
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (m->motionPlaneData->sfxId != -1) {
				psndSFXOff(m->motionPlaneData->sfxId);
				m->motionPlaneData->sfxId = -1;
			}
		}
		planeReset();
		allPartyRideOff2();
		if (mario->currSubMotionId == 0x14) {
			marioChgMot(3);
		} else if (mario->currSubMotionId == 0x1E) {
			marioChgMot(0);
		}
	}

	switch (mario->currSubMotionId) {
	case 0x28: {
		MarioWork* m;

		camFollowYOff();
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (m->motionPlaneData->sfxId != -1) {
				psndSFXOff(m->motionPlaneData->sfxId);
				m->motionPlaneData->sfxId = -1;
			}
		}
		mario->multiTimer = sysMsec2Frame(0x208);
		mario->currSubMotionId = 0x29;
		break;
	}
	case 0x29:
		if (--mario->multiTimer <= 0) {
			planeReset();
			marioChgPose("M_J_1C");
			mario->position.y = 1.0f + (mario->field_0x2AC - 37.0f);
			mario->baseSpeed = 0.0f;
			marioSetFallPara();
			mario->jumpVelY = 0.0f;
			mario->jumpAccelY *= 0.5f;
			mario->jumpJerkY *= 0.5f;
			mario->jumpJerkStepY *= 0.5f;
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
			if (m->motionPlaneData->sfxId != -1) {
				psndSFXOff(m->motionPlaneData->sfxId);
				m->motionPlaneData->sfxId = -1;
			}
		}
		planeReset();
		mario->dispFlags &= ~0x100;
		mario->baseSpeed = 0.0f;
		mario->planeTurnRate = 0.0f;
		mario->position.y = 1.0f + mario->field_0x2AC;
		marioPreJump();
		mario->jumpVelY *= 0.5f;
		marioChgPose("M_J_1B");
		mario->currSubMotionId = 0x50;
		psndSFXOn_3D((char*)0x180, &mario->position);
		allPartyRideOff2();
	}
		/* fall through */
	case 0x50: {
		f32 dir;

		dir = (mario->motionPlaneData->flags & 1) ? 1.0f : -1.0f;
		mario->planeTurnRate += 0.3f * (28.0f * dir - mario->planeTurnRate);
		mario->dispDir = revise360(mario->dispDir + mario->planeTurnRate);
		marioMakeJumpPara();
		mario->position.y += mario->jumpVelY;
		marioChkOverhead();
		if (mario->jumpVelY < 0.0f || mario->hitobjHead != NULL) {
			marioSetFallPara();
			mario->jumpVelY *= 0.5f;
			mario->jumpAccelY *= 0.5f;
			mario->jumpJerkY *= 0.5f;
			mario->jumpJerkStepY *= 0.5f;
			mario->currSubMotionId = 0x5A;
			mario->dispFlags |= 0x100;
			mario->multiTimer = 0x14;
		}
		break;
	}
	case 0x5A: {
		f32 dir;

		if (mario->multiTimer > 0) {
			dir = (mario->motionPlaneData->flags & 1) ? 1.0f : -1.0f;
			mario->planeTurnRate += 0.3f * (28.0f * dir - mario->planeTurnRate);
			mario->dispDir = revise360(mario->dispDir + mario->planeTurnRate);
			if (--mario->multiTimer <= 0) {
				mario->dispFlags &= ~0x100;
			}
		}
		marioMakeJumpPara();
		if (mario->jumpVelY <= -20.0f) {
			mario->jumpVelY = -20.0f;
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
			mario->camShiftSideTarget = 0.0f;
			mario->camShiftUpTarget = 0.0f;
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
		mario->jumpVelY = -2.0f;
		mario->jumpAccelY = 0.0f;
		mario->jumpJerkY = 0.0f;
		mario->jumpJerkStepY = 0.0f;
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

		mario->camShiftUpTarget = 0.0f;
		mario->camShiftSideTarget = 0.0f;
		marioClearJumpPara();
		m = marioGetPtr();
		if (m->motStruct != NULL) {
			if (m->motionPlaneData->sfxId != -1) {
				psndSFXOff(m->motionPlaneData->sfxId);
				m->motionPlaneData->sfxId = -1;
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
	mario->field_0x18C = mario->baseSpeed;
}


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
			if (m->motionPlaneData->sfxId == -1) {
				m->motionPlaneData->sfxId =
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
	if (mario->currSubMotionId != 0xF) {
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
	mario->motionPlaneData->heading = revise360(mario->motionPlaneData->bank * planeData[14] + mario->motionPlaneData->heading);
	if (mario->motionPlaneData->flags & 1) {
		if (mario->motionPlaneData->heading <= 60.0f) {
			mario->motionPlaneData->heading = 0.1f * (60.0f - mario->motionPlaneData->heading) + mario->motionPlaneData->heading;
		}
		if (mario->motionPlaneData->heading >= 150.0f) {
			mario->motionPlaneData->heading = 0.1f * (150.0f - mario->motionPlaneData->heading) + mario->motionPlaneData->heading;
		}
	} else {
		if (mario->motionPlaneData->heading >= 330.0f) {
			mario->motionPlaneData->heading = 0.1f * (330.0f - mario->motionPlaneData->heading) + mario->motionPlaneData->heading;
		}
		if (mario->motionPlaneData->heading <= 210.0f) {
			mario->motionPlaneData->heading = 0.1f * (210.0f - mario->motionPlaneData->heading) + mario->motionPlaneData->heading;
		}
	}
	{
		f32 a = mario->motionPlaneData->heading;
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
		mario->dispDir = revise360(v);
	}
	{
		u8 stick = mario->wStickDir1;

		if ((s8)(stick & 0xff) != 0) {
			if (mario->baseSpeed) {
				f3 = __fabsf((f32)mario->wStickDir1);
				if (mario->motionPlaneData->flags & 1) {
					if (mario->wStickDir1 < 0) {
						if (mario->motionPlaneData->pitch >= planeData[3]) {
							mario->motionPlaneData->pitch = 0.1f * (planeData[3] - mario->motionPlaneData->pitch) + mario->motionPlaneData->pitch;
						} else {
							mario->motionPlaneData->pitch = 0.1f * (f3 * planeData[5]) + mario->motionPlaneData->pitch;
						}
					}
					if (mario->wStickDir1 > 0) {
						if (mario->motionPlaneData->pitch <= planeData[4]) {
							mario->motionPlaneData->pitch = planeData[4];
						} else {
							mario->motionPlaneData->pitch = mario->motionPlaneData->pitch - 0.1f * (f3 * planeData[6]);
						}
					}
				} else {
					if (mario->wStickDir1 > 0) {
						if (mario->motionPlaneData->pitch >= planeData[3]) {
							mario->motionPlaneData->pitch = 0.1f * (planeData[3] - mario->motionPlaneData->pitch) + mario->motionPlaneData->pitch;
						} else {
							mario->motionPlaneData->pitch = 0.1f * (f3 * planeData[5]) + mario->motionPlaneData->pitch;
						}
					}
					if (mario->wStickDir1 < 0) {
						if (mario->motionPlaneData->pitch <= planeData[4]) {
							mario->motionPlaneData->pitch = planeData[4];
						} else {
							mario->motionPlaneData->pitch = mario->motionPlaneData->pitch - 0.1f * (f3 * planeData[6]);
						}
					}
				}
			}
		} else {
			if (mario->motionPlaneData->pitch <= 0.0f || mario->motionPlaneData->field_0x14 <= 0.0f) {
				if (mario->motionPlaneData->field_0x14 > 0.0f && mario->motionPlaneData->field_0x14 < 1.0f) {
					mario->motionPlaneData->pitch = MATCH_F32(mario->motionPlaneData->pitch) + planeData[30];
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
					mario->motionPlaneData->field_0x1C = f31;
					mario->motionPlaneData->pitch = mario->motionPlaneData->pitch + mario->motionPlaneData->field_0x1C;
				} else {
					mario->motionPlaneData->pitch += -0.001f;
					mario->baseSpeed += 0.5f;
					if (mario->baseSpeed > 2.0f) {
						mario->baseSpeed = 2.0f;
					}
				}
			}
			if (mario->motionPlaneData->pitch <= planeData[4]) {
				mario->motionPlaneData->pitch = planeData[4];
			}
			if (mario->motionPlaneData->pitch >= 6.0f) {
				mario->motionPlaneData->field_0x14 = 0.0f;
			}
		}
	}
	mario->baseSpeed = mario->baseSpeed * planeData[1] + mario->baseSpeed;
	{
		MotionPlaneData* dv = mario->motionPlaneData;

		if (dv->pitch >= 0.0f) {
			dv->flags |= 0x10;
			{
				f32 s = sin(3.1415927f * (mario->motionPlaneData->pitch * planeData[7]) / 180.0f);
				f32 x = planeData[8] * s;

				x = mario->baseSpeed * x;
				mario->baseSpeed = mario->baseSpeed + x;
			}
			if (mario->wStickDir1 != 0 && mario->motionPlaneData->pitch >= planeData[3]) {
				mario->baseSpeed = mario->baseSpeed + planeData[2];
			}
			if (mario->baseSpeed <= 0.0f) {
				mario->baseSpeed = 0.0f;
			}
			f31 = 0.0f;
			if (mario->wStickDir1 == 0) {
				if (mario->baseSpeed < planeData[20]) {
					f32 s, a;

					f31 = planeData[0] - mario->baseSpeed;
					s = sin(3.1415927f * mario->motionPlaneData->pitch / 180.0f);
					a = planeData[21] * s;
					f31 = f31 * a;
					f31 = f31 + mario->motionPlaneData->field_0x14;
					mario->motionPlaneData->field_0x14 = f31;
				} else {
					mario->motionPlaneData->field_0x14 = planeData[22];
				}
				mario->motionPlaneData->pitch = mario->motionPlaneData->pitch + mario->motionPlaneData->field_0x14;
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
							mario->motionPlaneData->field_0x1C = v;
						}
					}
				}
				mario->motionPlaneData->pitch = mario->motionPlaneData->pitch + mario->motionPlaneData->field_0x1C;
			}
			{
				f32 add;

				if (mario->wStickDir1 != 0) {
					f32 s = sin(3.1415927f * (planeData[0] - mario->baseSpeed) / 180.0f);

					add = planeData[18] * s;
					mario->motionPlaneData->field_0x18 = mario->motionPlaneData->field_0x18 + add;
					if (mario->motionPlaneData->field_0x18 <= planeData[19]) {
						mario->motionPlaneData->field_0x18 = planeData[19];
					}
				} else {
					add = 0.1f * -mario->motionPlaneData->field_0x18 + mario->motionPlaneData->field_0x18;
					mario->motionPlaneData->field_0x18 = add;
				}
				mario->motionPlaneData->pitch = mario->motionPlaneData->pitch + add;
			}
		} else if (dv->pitch >= -80.0f) {
			if (dv->flags & 0x10) {
				dv->flags = dv->flags & ~0x10;
			}
			{
				f32 s = sin(3.1415927f * (planeData[9] * __fabsf(mario->motionPlaneData->pitch)) / 180.0f);

				mario->motionPlaneData->field_0x10 = planeData[10] * s;
				mario->baseSpeed = mario->baseSpeed + mario->motionPlaneData->field_0x10;
			}
			f3 = 0.0f;
			if (mario->motionPlaneData->pitch > 0.0f) {
				f3 = 0.0f;
			} else {
				f3 = __fabsf(mario->motionPlaneData->pitch);
			}
			{
				f32 x, y;

				f31 = sin(3.1415927f * (planeData[12] * f3) / 180.0f);
				y = sin(3.1415927f * (mario->baseSpeed * planeData[11]) / 180.0f);
				x = planeData[13] * (y * f31);
				mario->motionPlaneData->field_0x14 = mario->motionPlaneData->field_0x14 + x;
			}
			if ((s8)(mario->wStickDir1 & 0xff) != 0) {
				if (mario->motionPlaneData->flags & 1) {
					if (mario->wStickDir1 > 0 && mario->motionPlaneData->field_0x14 >= 1.0f) {
						mario->motionPlaneData->field_0x14 = 1.0f;
					}
				} else {
					if (mario->wStickDir1 < 0 && mario->motionPlaneData->field_0x14 >= 1.0f) {
						mario->motionPlaneData->field_0x14 = 1.0f;
					}
				}
				onPlace = 0;
				m = marioGetPtr();
				if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
					onPlace = 1;
				}
				if (onPlace == 0) {
					mario->motionPlaneData->field_0x1C += 0.1f * (1.0f - mario->motionPlaneData->field_0x1C);
					if (mario->motionPlaneData->field_0x1C >= 0.0f) {
						mario->motionPlaneData->field_0x1C = 0.0f;
					}
				}
			}
			onPlace = 0;
			m = marioGetPtr();
			if (m->hitobjUnder != NULL && (hitGetAttr(m->hitobjUnder) & 0x38) != 0) {
				onPlace = 1;
			}
			if (onPlace != 0) {
				mario->motionPlaneData->field_0x24 = -22.0f;
			} else {
				mario->motionPlaneData->field_0x24 += 0.1f * (planeData[4] - mario->motionPlaneData->field_0x24);
			}
			mario->motionPlaneData->pitch = mario->motionPlaneData->pitch + mario->motionPlaneData->field_0x14;
			if (mario->motionPlaneData->pitch <= mario->motionPlaneData->field_0x24) {
				mario->motionPlaneData->pitch = mario->motionPlaneData->field_0x24;
			}
			mario->motionPlaneData->field_0x18 = 0.1f * -mario->motionPlaneData->field_0x18 + mario->motionPlaneData->field_0x18;
		}
	}
	if (mario->baseSpeed >= planeData[0]) {
		mario->baseSpeed = planeData[0];
	}
	mario->position.y = mario->position.y + mario->motionPlaneData->field_0x18;
	mario->position.y = mario->position.y - planeData[29];
	mario->dispRot.z = revise360(-mario->motionPlaneData->pitch);
	{
		f32 sin2, cos2, sin1, cos1;

		mario->directionView = mario->motionPlaneData->heading;
		sincosf(&sin1, &cos1, mario->directionView);
		mario->position.z = mario->baseSpeed * cos1 + mario->position.z;
		sincosf(&sin2, &cos2, 90.0f + mario->motionPlaneData->pitch);
		mario->position.y = mario->baseSpeed * cos2 * (cos2 >= 0.0f ? planeData[31] : planeData[32]) + mario->position.y;
		mario->position.x = sin2 * (mario->baseSpeed * sin1) + mario->position.x;
	}
	if (mario->baseSpeed <= planeData[16]) {
		s32 frames = sysMsec2Frame((s32)(1000.0f * planeData[17]));

		if (mario->motionPlaneData->flags & 1) {
			if ((f32)mario->wStickDir1 < 0.0f) {
				s32 c = mario->motionPlaneData->stallTimer + 1;

				mario->motionPlaneData->stallTimer = c;
				if (c >= frames) {
					mario->field_0x2AC = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		} else {
			if ((f32)mario->wStickDir1 > 0.0f) {
				s32 c = mario->motionPlaneData->stallTimer + 1;

				mario->motionPlaneData->stallTimer = c;
				if (c >= frames) {
					mario->field_0x2AC = mario->position.y;
					mario->currSubMotionId = 0x3C;
					return;
				}
			}
		}
	} else {
		mario->motionPlaneData->stallTimer = 0;
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

void mot_plane_post(void) {
	MarioWork* mario = marioGetPtr();
	MarioWork* m;

	camFollowYOn();
	m = marioGetPtr();
	if (m->motStruct != NULL) {
		if (m->motionPlaneData->sfxId != -1) {
			psndSFXOff(m->motionPlaneData->sfxId);
			m->motionPlaneData->sfxId = -1;
		}
	}
	planeReset();
	mario->camShiftSideTarget = 0.0f;
	mario->camShiftUpTarget = 0.0f;
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

	sincosf(&sinA, &cosA, 90.0f + mario->motionPlaneData->pitch);
	sincosf(&sinC, &cosC, mario->motionPlaneData->heading);

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
	mario->motionPlaneData->field_0x3C = v3C;

	sincosf(&sinR, &cosR, mario->motionPlaneData->bank);
	sincosf(&sinA, &cosA, mario->motionPlaneData->pitch);
	mario->hitobjFront = NULL;
	{
		f32 vx, vy, vz;
		f32 pd40 = planeData[40];
		sincosf(&sinC, &cosC, mario->motionPlaneData->heading);
		vx = mario->dispPivot.x + (mario->position.x + mario->dispOfs.x);
		vy = mario->dispPivot.y + (mario->position.y + mario->dispOfs.y);
		vz = mario->dispPivot.z + (mario->position.z + mario->dispOfs.z);
		vx = pd40 * (sinC * -cosA) + vx;
		vz = pd40 * (cosC * -cosA) + vz;
		vy = sinA * pd40 + vy;
		sincosf(&sinA, &cosA, mario->motionPlaneData->pitch - 90.0f);
		vx = vx + (planeData[39] * (cosA * sinC) - planeData[39] * (sinR * cosC));
		vy = vy + planeData[39] * (sinA * cosR);
		v30 = (Vec){ 0.0f, 0.0f, 0.0f };
		v30.x = vx;
		v30.y = vy;
		vz = vz + (planeData[39] * (cosA * cosC) - planeData[39] * (sinR * sinC));
		v30.z = vz;
		mario->motionPlaneData->field_0x30 = v30;
	}

	ten = 10.0f;
	{
		MotionPlaneData* d = mario->motionPlaneData;
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
		f32 a0 = mario->motionPlaneData->field_0x3C.x;
		f32 a1 = mario->motionPlaneData->field_0x3C.y;
		f32 a2 = mario->motionPlaneData->field_0x3C.z;
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
		f32 a0 = mario->motionPlaneData->field_0x30.x;
		f32 a1 = mario->motionPlaneData->field_0x30.y;
		f32 a2 = mario->motionPlaneData->field_0x30.z;
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
			f32 a0 = mario->motionPlaneData->field_0x3C.x;
			f32 a1 = mario->motionPlaneData->field_0x3C.y;
			f32 a2 = mario->motionPlaneData->field_0x3C.z;
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
		PSVECSubtract(&mario->motionPlaneData->field_0x3C, &mario->motionPlaneData->field_0x30, &dir);
		dist = PSVECMag(&dir);
		PSVECNormalize(&dir, &dir);
		hit = marioHitCheckVec(&mario->motionPlaneData->field_0x30, &dir, &normal, NULL, &dist);
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = normal.y;
		}
	}

	mario->hitobjHead = NULL;
	if (mario->hitobjStandOn == NULL) {
		{
			f32 a0 = mario->motionPlaneData->field_0x30.x;
			f32 a1 = mario->motionPlaneData->field_0x30.y;
			f32 a2 = mario->motionPlaneData->field_0x30.z;
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
