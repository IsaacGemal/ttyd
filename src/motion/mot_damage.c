#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"

// Prototypes for functions this unit calls whose headers don't declare them
// yet. Each one is listed in REPORT.md so it can move to a header later.
void marioClearSlitFloor(void);
void marioSlitForceCancel(void);
void marioYoshiForceCancel(void);
void unk_800cd9a0(void);
void marioNoUpdateCamPos(void);
void bomhei_use_cancel2(void);
void mario_simple_kemuri(void);
void marioSetFallPara(void);
f32 marioGetFallSpd(void);
void* marioChkLandon(f32* pos, f32 spd);
void marioUpdateCamPos(void);
void marioMakeJumpPara(void);
void npcStopForEvent(void);
void npcStartForEvent(void);
void effSplashEntry(s32 type, f32 x, f32 y, f32 z, f32 scale);
USER_FUNC(evt_cam_shake);
USER_FUNC(gazigazi);
extern s32 evt_gazigazi_appear[];

// .rodata named constants.
const Vec vec3_802f3238 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3244 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3250 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f325c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3268 = { 400.0f, 35.0f, -100.0f };
const Vec vec3_802f3274 = { 0.0f, 0.0f, -200.0f };
const Vec vec3_802f3280 = { 146.0f, 0.0f, 146.0f };
const Vec vec3_802f328c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3298 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f32a4 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802f32b0 = 4503601774854144.0;

EVT_BEGIN(quake_evt)
CALL(evt_cam_shake, 4, FLOAT(0.01f), FLOAT(0.0f), 0x32)
RETURN
EVT_END()

EVT_BEGIN(quake_bgmode_evt)
CALL(evt_cam_shake, 4, FLOAT(0.006f), FLOAT(0.0f), 0x1E)
RETURN
EVT_END()

EVT_BEGIN(evt_gazigazi)
CALL(gazigazi)
SPAWN_EVENT(evt_gazigazi_appear)
RETURN
EVT_END()

// marioWaitDamageTogeReturnFall
s32 marioWaitDamageTogeReturnFall(void) {
	MarioWork* mario = marioGetPtr();
	f32 x, y, z;

	if (mario->currMotionId != MARIO_MOTION_DAMAGE_TOGE) {
		return -1;
	}
	if ((s32)mario->currSubMotionId >= 0x14) {
		marioGetScreenPos(mario->position, &x, &y, &z);
		if (marioChkInScreen((s32)x, (s32)y)) {
			return 1;
		}
	}
	return 0;
}

void mot_damageToge(void) {
	MarioWork* mario;
	HitEntry* hit;
	f32 spd;
	Vec position;
	f32 landY;

	mario = marioGetPtr();
	if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
		mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
		marioClearSlitFloor();
		mario->dispFlags &= ~0x100;
		marioAdjustMoveDir();
		marioSlitForceCancel();
		marioYoshiForceCancel();
		unk_800cd9a0();
		marioNoUpdateCamPos();
		bomhei_use_cancel2();
		mario->currSubMotionId = 0;
		mario->airTimer = 0;
		mario->baseSpeed = 0.0f;
		*(f32*)(mario->field_0x28C + 0x14) = mario->position.y;
		mario->multiTimer = 5;
		*(f32*)mario->field_0x2AC = 4.0f;
	}

	switch (mario->currSubMotionId) {
	case 0:
		if ((s32)--mario->multiTimer > 0) {
			break;
		}
		if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
			marioChgEvtPose("M_D_7");
		} else {
			marioChgPose("M_D_6");
		}
		psndSFXOn_3D((const char*)0xba, &mario->position);
		mario_simple_kemuri();
		mario->currSubMotionId = 0xB;
		// fallthrough
	case 0xB:
		spd = *(f32*)mario->field_0x2AC;
		mario->position.y = mario->position.y + spd;
		spd -= 1.0f;
		if (spd < 10.0f) {
			spd = 10.0f;
		}
		*(f32*)mario->field_0x2AC = spd;
		if (mario->position.y < *(f32*)(mario->field_0x28C + 0x14) + 500.0f) {
			break;
		}
		hit = *(HitEntry**)(mario->field_0x1F0 + 0x14);
		if (hit != NULL) {
			Vec* p = hitGetDamageReturnPos(hit);
			Vec tmp = vec3_802f3298;
			tmp.x = p->x;			tmp.y = p->y;
			tmp.z = p->z;
			position = tmp;
		} else {
			position = vec3_802f32a4;
		}
		spd = position.z;
		mario->baseSpeed = 8.0f;
		mario->multiTimer = (s16)(distABf(position.x, position.z, mario->position.x, mario->position.z) / 8.0f);
		mario->directionView = angleABf(mario->position.x, mario->position.z, position.x, position.z);
		mario->wCamVal1 = (position.y - *(f32*)(mario->field_0x28C + 0x14)) / (f32)(s32)mario->multiTimer;
		mario->currSubMotionId = 0xC;
		break;
	case 0xC:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if ((s32)--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0xD;
			mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
			marioSetFallPara();
		}
		break;
	case 0xD:
		if ((s32)--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0x14;
		}
		break;
	case 0x14:
		mario->hitobjStandOn = NULL;
		spd = marioGetFallSpd();
		if (spd <= -10.0f) {
			spd = -10.0f;
		}
		hit = marioChkLandon(&landY, spd);
		if (hit == NULL) {
			mario->position.y = mario->position.y + spd;
		} else {
			marioChgPose("M_D_2");
			evtEntry(!marioBgmodeChk() ? quake_evt : quake_bgmode_evt, 0, 0);
			psndSFXOn_3D((const char*)0xcb, &mario->position);
			psndSFXOn_3D((const char*)0x197, &mario->position);
			mario_simple_kemuri();
			mario->hitobjStandOn = hit;
			mario->hitobjJumpFrom = NULL;
			mario->position.y = landY;
		}
		if (mario->hitobjStandOn == NULL) {
			break;
		}
		// fallthrough
	case 0x1E:
		mario->multiTimer = 0x32;
		mario->currSubMotionId = 0x1F;
		// fallthrough
	case 0x1F:
		if ((s32)--mario->multiTimer > 0) {
			break;
		}
		pouchAddHP(-1);
		if (pouchGetHP() <= 0) {
			mario->currSubMotionId = 0x21;
			marioUpdateCamPos();
			break;
		}
		marioChgPose("M_U_3");
		mario->multiTimer = 0xC;
		mario->currSubMotionId = 0x20;
		// fallthrough
	case 0x20:
		if ((s32)--mario->multiTimer <= 0) {
			marioChgMot(0);
			marioUpdateCamPos();
		}
		break;
	default:
		break;
	}
}

// marioWaitDamageReturnFall
s32 marioWaitDamageReturnFall(void) {
	MarioWork* mario = marioGetPtr();
	f32 x, y, z;

	if (mario->currMotionId != MARIO_MOTION_DAMAGE) {
		return -1;
	}
	if ((s32)mario->currSubMotionId >= 0x14) {
		marioGetScreenPos(mario->position, &x, &y, &z);
		if (marioChkInScreen((s32)x, (s32)y)) {
			return 1;
		}
	}
	return 0;
}

// NONMATCHING (99.12%): case 0x14 swaps f30/f31 (original spd=f31, floorY=f30; ours the reverse) and the
// compiler emits a second 2^52 conversion double instead of reusing double_to_int_802f32b0 (one `lfd`
// offset); the 0x98/0xA4 Vec copies also hoist the `lwz 0x8(r30)` one instruction earlier.
void mot_damage(void) {
	MarioWork* mario;
	HitEntry* hit;
	f32 spd;
	Vec target;
	f32 floorY;
	f32 landY;

	mario = marioGetPtr();
	if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
		mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
		if (*(f32*)mario->field_0x7C == 0.0f) {
			*(f32*)mario->field_0x7C = -6.0f;
			*(f32*)(mario->field_0x7C + 4) = 0.0f;
			*(f32*)(mario->field_0x7C + 8) = 0.0f;
			*(f32*)(mario->field_0x7C + 0xC) = 0.0f;
		}
		marioClearSlitFloor();
		marioOfsRotReset();
		mario->dispFlags &= ~0x100;
		marioAdjustMoveDir();
		marioSlitForceCancel();
		bomhei_use_cancel2();
		marioNoUpdateCamPos();
		if (mario->characterId == 2) {
			marioChgPose("KPA_D_1");
		} else {
			marioChgEvtPose("M_F_1");
		}
		mario->currSubMotionId = 0;
		mario->airTimer = 0;
		mario->baseSpeed = 0.0f;
		mario->multiTimer = 0x14;
		if (mario->prevMotionId != MARIO_MOTION_YOSHI && mario->prevMotionId != MARIO_MOTION_PLANE) {
			Vec pos = mario->position;

			psndSFXOn_3D((const char*)0x194, &pos);
			if (marioBgmodeChk() != 1) {
				effSplashEntry(0, pos.x, pos.y, pos.z, 1.0f);
			}
		}
		*(Vec*)mario->field_0x98 = vec3_802f3238;
		*(Vec*)(mario->field_0x98 + 0xC) = vec3_802f3244;
		npcStopForEvent();
	}

	switch (mario->currSubMotionId) {
	case 0:
		if (mario->position.y >= *(f32*)(mario->field_0x28C + 0x14) - 160.0f) {
			marioMakeJumpPara();
			if (*(f32*)mario->field_0x7C <= -6.0f) {
				*(f32*)mario->field_0x7C = -6.0f;
				*(f32*)(mario->field_0x7C + 4) = 0.0f;
				*(f32*)(mario->field_0x7C + 8) = 0.0f;
				*(f32*)(mario->field_0x7C + 0xC) = 0.0f;
			}
			mario->position.y = mario->position.y + *(f32*)mario->field_0x7C;
		} else {
			*(f32*)mario->field_0x7C = 0.3f;
			mario->currSubMotionId = 1;
			mario->multiTimer = 0x28;
		}
		break;
	case 1:
		if ((s32)--mario->multiTimer > 0) {
			break;
		}
		mario->multiTimer = 0;
		mario->currSubMotionId = 0xA;
		*(f32*)mario->field_0x2AC = 20.0f;
		break;
	case 0xA:
		if ((s32)(++mario->multiTimer) == 2) {
			mario->currSubMotionId = 0xB;
			psndSFXOn_3D((const char*)0x195, &mario->position);
			if (*(HitEntry**)(mario->field_0x1F0 + 0x14) != NULL) {
				evtEntry(evt_gazigazi, 0, 0);
			}
		}
		// fallthrough
	case 0xB:
		{
			f32 oldY = mario->position.y;

			spd = *(f32*)mario->field_0x2AC;
			mario->position.y = oldY + spd;
			spd -= 1.0f;
			if (spd < 10.0f) {
				spd = 10.0f;
			}
			*(f32*)mario->field_0x2AC = spd;
			if (mario->position.y >= *(f32*)(mario->field_0x28C + 0x14) && *(f32*)(mario->field_0x28C + 0x14) > oldY) {
				Vec splashPos = mario->position;

				if (marioBgmodeChk() != 1) {
					effSplashEntry(1, splashPos.x, splashPos.y, splashPos.z, 1.0f);
				}
				psndSFXOn_3D((const char*)0xba, &splashPos);
			}
			if (mario->position.y < *(f32*)(mario->field_0x28C + 0x14) + 500.0f) {
				break;
			}
		}
		hit = *(HitEntry**)(mario->field_0x1F0 + 0x14);
		if (hit != NULL) {
			Vec* p = hitGetDamageReturnPos(hit);
			Vec tmp = vec3_802f3250;

			tmp.x = p->x;
			tmp.y = p->y;
			tmp.z = p->z;
			target = tmp;
		} else {
			target = vec3_802f325c;
			if (strcmp(gp->currentMapName, "muj_00") == 0) {
				target = vec3_802f3268;
			}
			if (strcmp(gp->currentMapName, "gor_00") == 0) {
				target = vec3_802f3274;
			}
			if (strcmp(gp->currentMapName, "gor_04") == 0) {
				target = vec3_802f3280;
			}
		}
		if (mario->flags & MARIO_FLAG_BG_MODE) {
			Vec bgTmp = vec3_802f328c;

			bgTmp.x = *(f32*)(mario->field_0xD4 + 0xC);
			bgTmp.y = *(f32*)(mario->field_0xD4 + 0x10);
			bgTmp.z = *(f32*)(mario->field_0xD4 + 0x14);
			target = bgTmp;
		}
		spd = target.z;
		mario->baseSpeed = 8.0f;
		mario->multiTimer = (s16)(distABf(target.x, target.z, mario->position.x, mario->position.z) / 8.0f);
		mario->directionView = angleABf(mario->position.x, mario->position.z, target.x, target.z);
		mario->wCamVal1 = (target.y - *(f32*)(mario->field_0x28C + 0x14)) / (f32)(s32)mario->multiTimer;
		mario->currSubMotionId = 0xC;
		mario->dispFlags |= 1;
		if ((s32)mario->multiTimer == 0) {
			mario->multiTimer = 0xA;
			mario->wCamVal1 = (target.y - *(f32*)(mario->field_0x28C + 0x14)) / (f32)(s32)mario->multiTimer;
			mario->currSubMotionId = 0xE;
		}
		break;
	case 0xC:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if ((s32)--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0xD;
			mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
			marioSetFallPara();
			mario->dispFlags &= ~1;
			mario->multiTimer = 1;
		}
		break;
	case 0xD:
		if ((s32)--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0x14;
		}
		break;
	case 0xE:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if ((s32)--mario->multiTimer <= 0) {
			mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
			marioSetFallPara();
			mario->dispFlags &= ~1;
			mario->currSubMotionId = 0x14;
		}
		break;
	case 0x14:
		mario->hitobjStandOn = NULL;
		spd = marioGetFallSpd();
		if (spd <= -10.0f) {
			spd = -10.0f;
		}
		floorY = *(HitEntry**)(mario->field_0x1F0 + 0x14) != NULL
		             ? hitGetDamageReturnPos(*(HitEntry**)(mario->field_0x1F0 + 0x14))->y
		             : mario->position.y;
		if (marioBgmodeChk() == 1 || mario->position.y - floorY <= 50.0f) {
			hit = marioChkLandon(&landY, spd);
		} else {
			hit = NULL;
		}
		if (hit == NULL) {
			mario->position.y = mario->position.y + spd;
		} else {
			mario->hitobjStandOn = hit;
			mario->hitobjJumpFrom = NULL;
			mario->position.y = landY;
			if (mario->characterId == 2) {
				marioChgPose("KPA_D_2");
			} else {
				marioChgPose("M_D_2");
			}
			evtEntry(!marioBgmodeChk() ? quake_evt : quake_bgmode_evt, 0, 0);
			psndSFXOn_3D((const char*)0xcb, &mario->position);
			psndSFXOn_3D((const char*)0x197, &mario->position);
			mario_simple_kemuri();
		}
		if (mario->hitobjStandOn == NULL) {
			break;
		}
		// fallthrough
	case 0x1E:
		mario->multiTimer = 0x32;
		mario->currSubMotionId = 0x1F;
		// fallthrough
	case 0x1F:
		if ((s32)--mario->multiTimer > 0) {
			break;
		}
		if (mario->characterId == 0) {
			pouchAddHP(-1);
		}
		if (pouchGetHP() <= 0) {
			mario->currSubMotionId = 0x21;
			marioUpdateCamPos();
			break;
		}
		if (mario->characterId != 2) {
			marioChgPose("M_U_3");
		}
		mario->multiTimer = 0xC;
		mario->currSubMotionId = 0x20;
		// fallthrough
	case 0x20:
		if ((s32)--mario->multiTimer <= 0) {
			marioChgMot(0);
			marioUpdateCamPos();
			npcStartForEvent();
		}
		break;
	default:
		break;
	}
}

// set_damage_root_ypos
void set_damage_root_ypos(f32 y) {
	*(f32*)(marioGetPtr()->field_0x28C + 0x14) = y;
}
// marioChkDamageSink
s32 marioChkDamageSink(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId == MARIO_MOTION_DAMAGE) {
		s32 sub = mario->currSubMotionId;

		if (sub >= 0 && sub < 0xA) {
			return 1;
		}
	}
	return 0;
}

// gazigazi
USER_FUNC(gazigazi) {
	MarioWork* mario = marioGetPtr();

	event->lwData[0] = (s32)mario->position.x;
	event->lwData[1] = (s32)(*(f32*)(mario->field_0x28C + 0x14));
	event->lwData[2] = (s32)mario->position.z;
	return EVT_RETURN_DONE;
}

// marioSetSplash
void marioSetSplash(s32 type, Vec* pos) {
	if (type == 0) {
		psndSFXOn_3D((const char*)0x194, pos);
	}
	if (marioBgmodeChk() != TRUE) {
		effSplashEntry(type != 0, pos->x, pos->y, pos->z, 1.0f);
	}
	if (type == 1) {
		psndSFXOn_3D((const char*)0xba, pos);
	}
}

// marioDmgRetQuakeOn
void marioDmgRetQuakeOn(void) {
	evtEntry(!marioBgmodeChk() ? quake_evt : quake_bgmode_evt, 0, 0);
}
