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
#include "party/party_yoshi.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "evt/evt_cam.h"
#include "motion/mot_damage.h"
#include "motion/mot_slit.h"
#include "mario/mario_kemuri.h"
#include "party/party_nokonoko.h"
#include "party/party_bomhei.h"
#include "eff/eff_splash.h"

// Prototypes for functions this unit calls whose headers don't declare them
// yet. Each one is listed in REPORT.md so it can move to a header later.
extern s32 evt_gazigazi_appear[];
extern s32 quake_evt[];
extern s32 quake_bgmode_evt[];
extern s32 evt_gazigazi[];

// .rodata named constants.
const Vec vec3_802f3238 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3244 = { 0.0f, 0.0f, 0.0f };

// marioDmgRetQuakeOn
void marioDmgRetQuakeOn(void) {
	evtEntry(!marioBgmodeChk() ? quake_evt : quake_bgmode_evt, 0, 0);
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

// gazigazi
USER_FUNC(gazigazi) {
	MarioWork* mario = marioGetPtr();

	event->lwData[0] = (s32)mario->position.x;
	event->lwData[1] = (s32)(mario->damageRootY);
	event->lwData[2] = (s32)mario->position.z;
	return EVT_RETURN_DONE;
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

// set_damage_root_ypos
void set_damage_root_ypos(f32 y) {
	marioGetPtr()->damageRootY = y;
}
// The 0x98/0xA4 Vec copies below are copied through double-volatile casts to keep the original
// unscheduled per-component load/store order (the plain copy hoists `lwz 0x8(r30)` and `lwz 0xC(r30)`
// one instruction too early). That fix is worth 0.80% and is guarded by MUST_MATCH (rule 7).
// case 0x14 keeps the returned Vec in a pointer local and assigns floorY from it in an if/else; the
// plain ternary gives floorY f31 and spd f30, the reversed of the original's spd=f31/floorY=f30.
void mot_damage(void) {
	MarioWork* mario;
	HitEntry* hit;
	f32 spd;
	Vec target;
	f32 floorY;
	f32 landY;
	Vec* rp;
	mario = marioGetPtr();
	if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
		mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
		if (mario->jumpVelY == 0.0f) {
			mario->jumpVelY = -6.0f;
			mario->jumpAccelY = 0.0f;
			mario->jumpJerkY = 0.0f;
			mario->jumpJerkStepY = 0.0f;
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
#ifdef MUST_MATCH
		// The original copies each Vec with an unscheduled per-component load/store pair.
		*(volatile Vec*)&mario->dispOfs = *(volatile Vec*)&vec3_802f3238;
		*(volatile Vec*)&mario->dispOfs2 = *(volatile Vec*)&vec3_802f3244;
#else
		mario->dispOfs = vec3_802f3238;
		mario->dispOfs2 = vec3_802f3244;
#endif
		npcStopForEvent();
	}

	switch (mario->currSubMotionId) {
	case 0:
		if (mario->position.y >= mario->damageRootY - 160.0f) {
			marioMakeJumpPara();
			if (mario->jumpVelY <= -6.0f) {
				mario->jumpVelY = -6.0f;
				mario->jumpAccelY = 0.0f;
				mario->jumpJerkY = 0.0f;
				mario->jumpJerkStepY = 0.0f;
			}
			mario->position.y = mario->position.y + mario->jumpVelY;
		} else {
			mario->jumpVelY = 0.3f;
			mario->currSubMotionId = 1;
			mario->multiTimer = 0x28;
		}
		break;
	case 1:
		if (--mario->multiTimer > 0) {
			break;
		}
		mario->multiTimer = 0;
		mario->currSubMotionId = 0xA;
		mario->field_0x2AC = 20.0f;
		break;
	case 0xA:
		if ((++mario->multiTimer) == 2) {
			mario->currSubMotionId = 0xB;
			psndSFXOn_3D((const char*)0x195, &mario->position);
			if ((HitEntry*)mario->hitobjDamageReturn != NULL) {
				evtEntry(evt_gazigazi, 0, 0);
			}
		}
		// fallthrough
	case 0xB:
		{
			f32 oldY = mario->position.y;

			spd = mario->field_0x2AC;
			mario->position.y = oldY + spd;
			spd -= 1.0f;
			if (spd < 10.0f) {
				spd = 10.0f;
			}
			mario->field_0x2AC = spd;
			if (mario->position.y >= mario->damageRootY && mario->damageRootY > oldY) {
				Vec splashPos = mario->position;

				if (marioBgmodeChk() != 1) {
					effSplashEntry(1, splashPos.x, splashPos.y, splashPos.z, 1.0f);
				}
				psndSFXOn_3D((const char*)0xba, &splashPos);
			}
			if (mario->position.y < mario->damageRootY + 500.0f) {
				break;
			}
		}
		hit = (HitEntry*)mario->hitobjDamageReturn;
		if (hit != NULL) {
			Vec* p = hitGetDamageReturnPos(hit);
			Vec tmp = (Vec){ 0.0f, 0.0f, 0.0f };

			tmp.x = p->x;
			tmp.y = p->y;
			tmp.z = p->z;
			target = tmp;
		} else {
			target = (Vec){ 0.0f, 0.0f, 0.0f };
			if (strcmp(gp->currentMapName, "muj_00") == 0) {
				target = (Vec){ 400.0f, 35.0f, -100.0f };
			}
			if (strcmp(gp->currentMapName, "gor_00") == 0) {
				target = (Vec){ 0.0f, 0.0f, -200.0f };
			}
			if (strcmp(gp->currentMapName, "gor_04") == 0) {
				target = (Vec){ 146.0f, 0.0f, 146.0f };
			}
		}
		if (mario->flags & MARIO_FLAG_BG_MODE) {
			Vec bgTmp = (Vec){ 0.0f, 0.0f, 0.0f };

			bgTmp.x = mario->field_0xE0.x;
			bgTmp.y = mario->field_0xE0.y;
			bgTmp.z = mario->field_0xE0.z;
			target = bgTmp;
		}
		spd = target.z;
		mario->baseSpeed = 8.0f;
		mario->multiTimer = (s16)(distABf(target.x, target.z, mario->position.x, mario->position.z) / 8.0f);
		mario->directionView = angleABf(mario->position.x, mario->position.z, target.x, target.z);
		mario->wCamVal1 = (target.y - mario->damageRootY) / (f32)mario->multiTimer;
		mario->currSubMotionId = 0xC;
		mario->dispFlags |= 1;
		if (mario->multiTimer == 0) {
			mario->multiTimer = 0xA;
			mario->wCamVal1 = (target.y - mario->damageRootY) / (f32)mario->multiTimer;
			mario->currSubMotionId = 0xE;
		}
		break;
	case 0xC:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if (--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0xD;
			mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
			marioSetFallPara();
			mario->dispFlags &= ~1;
			mario->multiTimer = 1;
		}
		break;
	case 0xD:
		if (--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0x14;
		}
		break;
	case 0xE:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if (--mario->multiTimer <= 0) {
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
		if ((HitEntry*)mario->hitobjDamageReturn != NULL) {
			rp = hitGetDamageReturnPos((HitEntry*)mario->hitobjDamageReturn);
			floorY = rp->y;
		} else {
			floorY = mario->position.y;
		}
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
		if (--mario->multiTimer > 0) {
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
		if (--mario->multiTimer <= 0) {
			marioChgMot(0);
			marioUpdateCamPos();
			npcStartForEvent();
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
	if (mario->currSubMotionId >= 0x14) {
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
		mario->damageRootY = mario->position.y;
		mario->multiTimer = 5;
		mario->field_0x2AC = 4.0f;
	}

	switch (mario->currSubMotionId) {
	case 0:
		if (--mario->multiTimer > 0) {
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
		spd = mario->field_0x2AC;
		mario->position.y = mario->position.y + spd;
		spd -= 1.0f;
		if (spd < 10.0f) {
			spd = 10.0f;
		}
		mario->field_0x2AC = spd;
		if (mario->position.y < mario->damageRootY + 500.0f) {
			break;
		}
		hit = (HitEntry*)mario->hitobjDamageReturn;
		if (hit != NULL) {
			Vec* p = hitGetDamageReturnPos(hit);
			Vec tmp = (Vec){ 0.0f, 0.0f, 0.0f };
			tmp.x = p->x;			tmp.y = p->y;
			tmp.z = p->z;
			position = tmp;
		} else {
			position = (Vec){ 0.0f, 0.0f, 0.0f };
		}
		spd = position.z;
		mario->baseSpeed = 8.0f;
		mario->multiTimer = (s16)(distABf(position.x, position.z, mario->position.x, mario->position.z) / 8.0f);
		mario->directionView = angleABf(mario->position.x, mario->position.z, position.x, position.z);
		mario->wCamVal1 = (position.y - mario->damageRootY) / (f32)mario->multiTimer;
		mario->currSubMotionId = 0xC;
		break;
	case 0xC:
		mario->field_EC.y = mario->field_EC.y + mario->wCamVal1;
		movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
		mario->field_EC.x = mario->position.x;
		mario->field_EC.z = mario->position.z;
		if (--mario->multiTimer <= 0) {
			mario->currSubMotionId = 0xD;
			mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
			marioSetFallPara();
		}
		break;
	case 0xD:
		if (--mario->multiTimer <= 0) {
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
		if (--mario->multiTimer > 0) {
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
		if (--mario->multiTimer <= 0) {
			marioChgMot(0);
			marioUpdateCamPos();
		}
		break;
	default:
		break;
	}
}

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
	if (mario->currSubMotionId >= 0x14) {
		marioGetScreenPos(mario->position, &x, &y, &z);
		if (marioChkInScreen((s32)x, (s32)y)) {
			return 1;
		}
	}
	return 0;
}
