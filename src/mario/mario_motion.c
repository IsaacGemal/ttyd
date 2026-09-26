#include "eff/eff_kemuri.h"
#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_party.h"
#include "party.h"
#include "mario/mario_sbr.h"
#include "mot/mot_slit.h"
#include "party/party_vivian.h"
#include "driver/hitdrv.h"
#include "driver/casedrv.h"
#include "driver/npcdrv.h"
#include "driver/camdrv.h"
#include "system.h"
#include <math.h>
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "mario/mariost.h"

// From src/mario/mario.c; no header declares it yet.
void marioSlitForceCancel(void);
void marioReInit_ship(void);

// Motion table entry. Lives at marioMotTbl (0x8030E5E0), 12 bytes each.
typedef struct MotTblEntry {
    void (*unk0)(void);  // 0x0
    u32 unk4;            // 0x4
    void (*unk8)(void);  // 0x8
} MotTblEntry;

extern MotTblEntry marioMotTbl[];

// .sdata jump parameter tables, defined in this unit's reference assembly.
f32 marioJumpData[10] = { 8.0f, -0.84f, 0.08f, -0.02f, -0.1f, -0.2f, 0.024f, -0.004f, 32.0f, -18.0f };
const Vec vec3_802c59f0 = { 0.0f, 0.0f, 0.0f };
extern f32 marioJumpSwData[];

// From other units not decompiled yet: no header declares them.
s32 kpaGetBodyStatus(void);
s32 marioGetHammerLevel(void);
s32 marioChkHammer2(void);
s32 marioSlitChkWallAround(void);
s32 marioGetRub(u32 mask, s32* out1, s32* out2, f32* out3);
s32 marioShipChanging(void);
void camFollowYOn(void);
void marioResetRollSpd(void);
void yoshi_jumpStand(void);
s32 marioChkShipPlace(HitEntry* hit);
MapObject* mobjHitObjPtrToPtr(HitEntry* hit);
s32 mobjGetHint(void* entry);
void* fbatNpcTalkCheck(void);
s32 kpaHipAttackOk(void);
s32 marioChkHipAttack(void);
void mario_plane_cancel(void);
void allPartyForceRideOn(void);
void* marioGetTouchNpcPtr(void);
void peach_talk(void);
void kpaChgPose(const char* a, const char* b);
s32 getRollEvtFlag(void);
void marioSetPeakYpos(void);
void kpaSetJumpPara(void);
void kpaClearHitobjRide(void);
s32 kpaChkHitobjRide(void);
s32 marioKeyOffChk(void);
s32 marioCtrlOffChk(void);
s32 marioPriCheckJabara(void);
void mot_fall_roll(void);
void marioLandOn(void);
void kpaCamFollow(void);
void set_damage_root_ypos(f32 y);
s32 kpaFireAttackCheck(void);
void quake_kpaLandOn(void);
void N_marioSlitSetAfterLand(void);
void unk_800a4e40(void);
s32 marioChkSlitEnd(void);
void motSlitCancel(void);
void marioDmgRetQuakeOn(void);
void marioUpdateCamPos(void);
void* marioChkLandon(f32* pos, f32 spd);
// Header declares only 3 float args, but the original call passes 4 (f1..f4).


void marioChgStayMotion(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SHIP && mario->currMotionId != MARIO_MOTION_YOSHI &&
        mario->currMotionId != MARIO_MOTION_SLIT && mario->currMotionId != MARIO_MOTION_ROLL) {
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_STAY;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}

void marioChgTalkMotion(void) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = MARIO_MOTION_TALK;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
}

void marioChgGetItemMotion(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == MARIO_MOTION_ROLL) {
        marioResetRollSpd();
    } else if (mario->currMotionId == MARIO_MOTION_SLIT) {
        mario->baseSpeed = 0.0f;
    } else if (mario->currMotionId == MARIO_MOTION_SHIP) {
        mario->baseSpeed = 0.0f;
    } else if (mario->currMotionId != MARIO_MOTION_YOSHI) {
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_GET_ITEM;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}

void marioChgShipMotion(void) {
    marioReInit_ship();
}

void marioChgRollMotion(void) {
    MarioWork* mario = marioGetPtr();

    mario->currSubMotionId = 0x32;
    mario->currMotionId = MARIO_MOTION_ROLL;
}

void marioChgJumpStandMotion(f32 arg) {
    MarioWork* mario = marioGetPtr();

    camFollowYOn();
    if (marioChkSts(0x10)) {
        *(f32*)((u8*)mario + 0x2B8) = arg;
        effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_JUMP_STAND;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        *(f32*)((u8*)mario + 0x2B8) = arg;
        yoshi_jumpStand();
    } else {
        *(f32*)((u8*)mario + 0x2B8) = arg;
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_JUMP_STAND;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}

void marioChgSmallJumpMotion(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    mario->trigFlags |= MARIO_TRIG_FLAG_SET_JUMP_PARAMS;
    mario2 = marioGetPtr();
    if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario2->dispFlags & (1 << 27)) {
                mario2->dispFlags &= ~(1 << 27);
            }
            if (mario2->dispFlags & (1 << 2)) {
                mario2->dispFlags &= ~(1 << 2);
            }
        }
        mario2->prevMotionId = mario2->currMotionId;
        mario2->currMotionId = MARIO_MOTION_JUMP;
        if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario2->prevMotionId].unk8();
        }
        mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
    mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
    mario->hitobjJumpFrom = mario->hitobjStandOn;
    mario->hitobjStandOn = NULL;
    *(f32*)((u8*)mario + 0x7C) = 6.0f;
    *(f32*)((u8*)mario + 0x80) = -0.5f;
    *(f32*)((u8*)mario + 0x84) = 0.0f;
    *(f32*)((u8*)mario + 0x88) = 0.0f;
}

BOOL marioChkItemGetMotion(void) {
    BOOL result = FALSE;
    MarioWork* mario = marioGetPtr();
    u32 motion = mario->currMotionId;

    if (motion == MARIO_MOTION_STAY || (motion >= MARIO_MOTION_WALK && motion <= MARIO_MOTION_DASH) ||
        motion == MARIO_MOTION_YOSHI || motion == MARIO_MOTION_ROLL || motion == MARIO_MOTION_SLIT) {
        if (!(mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION)) {
            result = TRUE;
        }
    } else if (motion == MARIO_MOTION_SHIP) {
        if (marioShipChanging() == 3) {
            result = TRUE;
        }
    }
    return result;
}

BOOL marioChkTalkable(void) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* party;
    BOOL result = FALSE;

    party = partyGetPtr(marioGetPartyId());
    if (party != NULL && party->currentMemberId == PARTY_MEMBER_YOSHI) {
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            return FALSE;
        }
    }
    switch (mario->currMotionId) {
    case MARIO_MOTION_STAY:
    case MARIO_MOTION_WALK:
    case MARIO_MOTION_DASH:
        if (!(mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION)) {
            if (marioBgmodeChk() != TRUE) {
                if (!(mario->dispFlags & (1 << 24))) {
                    result = TRUE;
                }
            }
        }
        break;
    }
    if (vivianGetStatus() != 0) {
        result = FALSE;
    }
    return result;
}

void marioMotion(void) {
    MarioWork* mario = marioGetPtr();

    *(s32*)((u8*)mario + 0x1FC) = 0;
    do {
        marioMotTbl[mario->currMotionId].unk0();
    } while (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION);
}

void marioChgMot(s32 motion) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = motion;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
}

void marioChgMotSub(s32 motion, s32 unk) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = motion;
        if (unk != 0) {
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
        } else if (mario->prevMotionId == MARIO_MOTION_JABARA) {
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
}

void marioChgMot2(s32 motion) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = motion;
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
    }
}

void marioClearJumpPara(void) {
    MarioWork* mario = marioGetPtr();

    // 0x7C..0x88 jump params, not named in mario.h
    *(f32*)((u8*)mario + 0x7C) = 0.0f;
    *(f32*)((u8*)mario + 0x80) = 0.0f;
    *(f32*)((u8*)mario + 0x84) = 0.0f;
    *(f32*)((u8*)mario + 0x88) = 0.0f;
}

void marioSetFallPara(void) {
    MarioWork* mario = marioGetPtr();
    f32* para = marioJumpData + 4;

    if (mario->currMotionId == MARIO_MOTION_JUMP_SW) {
        para = marioJumpSwData + 4;
    }
    *(f32*)((u8*)mario + 0x7C) = para[0];
    *(f32*)((u8*)mario + 0x80) = para[1];
    *(f32*)((u8*)mario + 0x84) = para[2];
    *(f32*)((u8*)mario + 0x88) = para[3];
    if (mario->playerGravity != 1.0f) {
        *(f32*)((u8*)mario + 0x7C) = 0.34f;
        *(f32*)((u8*)mario + 0x80) = -0.002f;
        *(f32*)((u8*)mario + 0x84) = -0.01f;
        *(f32*)((u8*)mario + 0x88) = 0.0f;
    }
}

f32 marioGetFallSpd(void) {
    MarioWork* mario = marioGetPtr();

    *(f32*)((u8*)mario + 0x84) += *(f32*)((u8*)mario + 0x88);
    if (*(f32*)((u8*)mario + 0x84) > 0.0f) {
        *(f32*)((u8*)mario + 0x84) = 0.0f;
    }
    *(f32*)((u8*)mario + 0x80) += *(f32*)((u8*)mario + 0x84);
    *(f32*)((u8*)mario + 0x7C) += *(f32*)((u8*)mario + 0x80);
    if (*(f32*)((u8*)mario + 0x7C) <= marioJumpData[9]) {
        *(f32*)((u8*)mario + 0x7C) = marioJumpData[9];
        *(f32*)((u8*)mario + 0x80) = 0.0f;
        *(f32*)((u8*)mario + 0x84) = 0.0f;
        *(f32*)((u8*)mario + 0x88) = 0.0f;
    }
    return *(f32*)((u8*)mario + 0x7C);
}

f32 marioMakeJumpPara(void) {
    MarioWork* mario = marioGetPtr();
    f32 div;

    if (mario->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) {
        div = 1.7f;
    } else {
        div = 1.0f;
    }
    *(f32*)((u8*)mario + 0x84) += *(f32*)((u8*)mario + 0x88) / div;
    *(f32*)((u8*)mario + 0x80) += *(f32*)((u8*)mario + 0x84) / div;
    *(f32*)((u8*)mario + 0x7C) += *(f32*)((u8*)mario + 0x80) / div;
    return *(f32*)((u8*)mario + 0x7C);
}

void mot_getItem(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = 0.0f;
        marioAdjustMoveDir();
        marioChgPose("M_I_U");
    }
}

BOOL marioChkJump(void) {
    MarioWork* mario = marioGetPtr();
    CaseEntry* caseEntry;

    if (!(*(u16*)((u8*)mario + 0x248) & (1 << 8))) {
        return FALSE;
    }
    if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        return FALSE;
    }
    if (*(void**)((u8*)mario + 0x1DC) != NULL) {
        if ((u32)hitGetAttr((HitEntry*)*(void**)((u8*)mario + 0x1DC)) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)*(void**)((u8*)mario + 0x1DC))) == 1) {
                return FALSE;
            }
        }
        caseEntry = caseCheckHitObj(*(void**)((u8*)mario + 0x1DC));
        if (caseEntry != NULL) {
            if (caseEntry->activeConditionId == 9 || caseEntry->activeConditionId == 4 ||
                caseEntry->activeConditionId == 12) {
                return FALSE;
            }
        }
    }
    if (*(void**)((u8*)mario + 0x1E0) != NULL) {
        if ((u32)hitGetAttr((HitEntry*)*(void**)((u8*)mario + 0x1E0)) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)*(void**)((u8*)mario + 0x1E0))) == 1) {
                return FALSE;
            }
        }
        caseEntry = caseCheckHitObj(*(void**)((u8*)mario + 0x1E0));
        if (caseEntry != NULL) {
            if (caseEntry->activeConditionId == 9 || caseEntry->activeConditionId == 4 ||
                caseEntry->activeConditionId == 12) {
                return FALSE;
            }
        }
    }
    if (mario->hitobjStandOn != NULL) {
        if ((u32)hitGetAttr((HitEntry*)mario->hitobjStandOn) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr((HitEntry*)mario->hitobjStandOn)) == 1) {
                return FALSE;
            }
        }
        caseEntry = caseCheckHitObj(mario->hitobjStandOn);
        if (caseEntry != NULL) {
            if (caseEntry->activeConditionId == 5) {
                return FALSE;
            }
        }
    }
    if (fbatNpcTalkCheck() != NULL) {
        return FALSE;
    }
    mario = marioGetPtr();
    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = MARIO_MOTION_JUMP;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
    return TRUE;
}

BOOL marioChkTransform(void) {
    MarioWork* mario = marioGetPtr();

    if (!(*(u16*)((u8*)mario + 0x248) & (1 << 11))) {
        return FALSE;
    }
    if (!(mario->trigFlags & (1 << 2)) && (s8)*(u8*)((u8*)mario + 0x3B) != 0) {
        return FALSE;
    }
    if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        return FALSE;
    }
    if ((s32)pouchCheckItem(5) > 0) {
        if (marioChkShipPlace((HitEntry*)mario->hitobjStandOn) != 0) {
            mario = marioGetPtr();
            if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario->dispFlags & (1 << 27)) {
                        mario->dispFlags &= ~(1 << 27);
                    }
                    if (mario->dispFlags & (1 << 2)) {
                        mario->dispFlags &= ~(1 << 2);
                    }
                }
                mario->prevMotionId = mario->currMotionId;
                mario->currMotionId = MARIO_MOTION_SHIP;
                if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario->prevMotionId].unk8();
                }
                mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
            return FALSE;
        }
    }
    if ((s32)pouchCheckItem(4) > 0) {
        if (marioChkPlanePlace((HitEntry*)mario->hitobjStandOn) != 0) {
            mario = marioGetPtr();
            if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario->dispFlags & (1 << 27)) {
                        mario->dispFlags &= ~(1 << 27);
                    }
                    if (mario->dispFlags & (1 << 2)) {
                        mario->dispFlags &= ~(1 << 2);
                    }
                }
                mario->prevMotionId = mario->currMotionId;
                mario->currMotionId = MARIO_MOTION_PLANE;
                if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario->prevMotionId].unk8();
                }
                mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
            return FALSE;
        }
    }
    return TRUE;
}

BOOL marioChkItemMotion(void) {
    MarioWork* mario = marioGetPtr();
    s32 mask;

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return FALSE;
    }
    if (marioGetHammerLevel() == 0) {
        return FALSE;
    }
    if (marioChkHammer2() != 0) {
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_HAMMER_2;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
        return TRUE;
    }
    mask = *(u16*)((u8*)mario + 0x248) & (1 << 9);
    if (mask == 0) {
        return FALSE;
    }
    if (mario->flags & (MARIO_FLAG_TUBE_MODE | MARIO_FLAG_PAPER_MODE)) {
        return FALSE;
    }
    mario = marioGetPtr();
    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = MARIO_MOTION_HAMMER;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
    return TRUE;
}

s32 L_marioChkRub(void) {
    return 0;
}

s32 marioRollChgChk(void) {
    MarioWork* mario = marioGetPtr();
    s32 unk10;
    s32 unk0C;
    s32 unk08;
    Vec pos;

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return 0;
    }
    if (!(*(u16*)((u8*)mario + 0x246) & (1 << 5))) {
        return 0;
    }
    if (marioGetRub(0x20, &unk0C, &unk10, (f32*)&unk08) != 1) {
        return 0;
    }
    if (unk10 < 3) {
        return 0;
    }
    if ((s32)pouchCheckItem(3) == 0) {
        return 0;
    }
    if (marioSlitChkWallAround() == 0) {
        pos = mario->position;
        if (marioCreviceWallChk(&pos) == 0) {
            if (*(u16*)((u8*)mario + 0x246) & (1 << 5)) {
                mario = marioGetPtr();
                if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                    if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                        if (mario->dispFlags & (1 << 27)) {
                            mario->dispFlags &= ~(1 << 27);
                        }
                        if (mario->dispFlags & (1 << 2)) {
                            mario->dispFlags &= ~(1 << 2);
                        }
                    }
                    mario->prevMotionId = mario->currMotionId;
                    mario->currMotionId = MARIO_MOTION_ROLL;
                    if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                        marioMotTbl[mario->prevMotionId].unk8();
                    }
                    mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
                }
                return MARIO_MOTION_ROLL;
            }
        }
    }
    return 0;
}

void marioChgMotJump2(void) {
    MarioWork* mario = marioGetPtr();

    if (kpaGetBodyStatus() != 2 && mario->currMotionId != MARIO_MOTION_ROLL) {
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_JUMP_NPC;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}

void mot_kaze(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    BOOL flag;

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= 0xFFF0FFFF;
        mario->dispFlags |= 0x100;
        mario->multiTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = 0.0f;
        *(f32*)((u8*)mario + 0x2A0) = 0.0f;
        marioChgPose("M_P_2");
    }

    switch (mario->currSubMotionId) {
    case 0:
        *(f32*)((u8*)mario + 0x2A0) += 5.0f;
        if (*(f32*)((u8*)mario + 0x2A0) > 60.0f) {
            *(f32*)((u8*)mario + 0x2A0) = 60.0f;
        }
        mario->unk1A8 = revise360(mario->unk1A8 + *(f32*)((u8*)mario + 0x2A0));
        if (!(*(u16*)((u8*)mario + 0x246) & (1 << 4))) {
            if (mario->unk1A8 > 90.0f && mario->unk1A8 < 270.0f) {
                flag = TRUE;
            } else {
                flag = FALSE;
            }
            if (flag) {
                *(f32*)((u8*)mario + 0x1AC) = 180.0f;
            } else {
                *(f32*)((u8*)mario + 0x1AC) = 0.0f;
            }
            mario->currSubMotionId = 100;
            mario->multiTimer = 10;
        }
        break;
    case 100:
        *(f32*)((u8*)mario + 0x2A0) -= 6.0f;
        if (*(f32*)((u8*)mario + 0x2A0) < 0.0f) {
            *(f32*)((u8*)mario + 0x2A0) = 0.0f;
            mario->dispFlags &= ~0x100;
            mario2 = marioGetPtr();
            if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario2->dispFlags & (1 << 27)) {
                        mario2->dispFlags &= ~(1 << 27);
                    }
                    if (mario2->dispFlags & (1 << 2)) {
                        mario2->dispFlags &= ~(1 << 2);
                    }
                }
                mario2->prevMotionId = mario2->currMotionId;
                mario2->currMotionId = MARIO_MOTION_STAY;
                if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario2->prevMotionId].unk8();
                }
                mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
        }
        mario->unk1A8 = revise360(mario->unk1A8 + *(f32*)((u8*)mario + 0x2A0));
        if (mario->unk1A8 > 90.0f && mario->unk1A8 < 270.0f) {
            flag = TRUE;
        } else {
            flag = FALSE;
        }
        if (flag) {
            *(f32*)((u8*)mario + 0x1AC) = 180.0f;
        } else {
            *(f32*)((u8*)mario + 0x1AC) = 0.0f;
        }
        break;
    }
}

void mot_shadow(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= 0xFFF0FFFF;
        mario->dispFlags &= ~(1 << 24);
        mario->multiTimer = 0;
        mario->currSubMotionId = 0;
        marioChgPose("M_S_1");
        *(f32*)((u8*)mario + 0xBC) = 90.0f;
        *(f32*)((u8*)mario + 0xC0) = 0.0f;
        *(f32*)((u8*)mario + 0xC4) = 0.0f;
    }
    if ((s32)mario->currSubMotionId != 0) {
        return;
    }
    if (!(*(u16*)((u8*)mario + 0x248) & (1 << 9))) {
        return;
    }
    *(f32*)((u8*)mario + 0xBC) = 0.0f;
    *(f32*)((u8*)mario + 0xC4) = 0.0f;
    mario->multiTimer = 0;
    mario = marioGetPtr();
    if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
        if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
            if (mario->dispFlags & (1 << 27)) {
                mario->dispFlags &= ~(1 << 27);
            }
            if (mario->dispFlags & (1 << 2)) {
                mario->dispFlags &= ~(1 << 2);
            }
        }
        mario->prevMotionId = mario->currMotionId;
        mario->currMotionId = MARIO_MOTION_STAY;
        if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
            marioMotTbl[mario->prevMotionId].unk8();
        }
        mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
    }
}

void mot_partyUse(void) {
    marioGetPtr();
}

void mot_dummy(void) {
    marioGetPtr();
}

void mot_forceReset(void) {
    MarioWork* mario;
    MarioWork* mario2;
    PartyEntry* party;

    mario = marioGetPtr();
    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->currSubMotionId = 0;
    }

    switch (mario->currSubMotionId) {
    case 0:
        marioSlitForceCancel();
        mario2 = marioGetPtr();
        if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario2->dispFlags & (1 << 27)) {
                    mario2->dispFlags &= ~(1 << 27);
                }
                if (mario2->dispFlags & (1 << 2)) {
                    mario2->dispFlags &= ~(1 << 2);
                }
            }
            mario2->prevMotionId = mario2->currMotionId;
            mario2->currMotionId = MARIO_MOTION_STAY;
            if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario2->prevMotionId].unk8();
            }
            mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
        mario->currSubMotionId = 1;
        break;
    case 1:
        marioReset();
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            party->position = mario->position;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            party->position = mario->position;
        }
        break;
    }
}

// NOTE: real assembly order has these before many other functions; fixed up at the end.

void marioBoots(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return;
    }
    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        return;
    }
    if (mario->characterId == 2) {
        if (*(s16*)((u8*)mario + 0x52) < 6) {
            return;
        }
        if (kpaHipAttackOk() == 0) {
            return;
        }
        if (!(*(u16*)((u8*)mario + 0x248) & (1 << 8))) {
            return;
        }
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_HIP;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    } else {
        if (!(mario->flags & MARIO_FLAG_HAS_INPUT_JUMP)) {
            return;
        }
        if (marioChkHipAttack() == 0) {
            return;
        }
        if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
            if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                if (mario->dispFlags & (1 << 27)) {
                    mario->dispFlags &= ~(1 << 27);
                }
                if (mario->dispFlags & (1 << 2)) {
                    mario->dispFlags &= ~(1 << 2);
                }
            }
            mario->prevMotionId = mario->currMotionId;
            mario->currMotionId = MARIO_MOTION_HIP;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}

void marioChgMoveMotion(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    if (mario->currMotionId == MARIO_MOTION_PLANE) {
        mario_plane_cancel();
    } else if (mario->currMotionId == MARIO_MOTION_ROLL) {
        allPartyForceRideOn();
    } else if (mario->currMotionId != MARIO_MOTION_YOSHI) {
        if (0.0f == mario->baseSpeed) {
            mario2 = marioGetPtr();
            if (mario2->currMotionId != MARIO_MOTION_SHIP && mario2->currMotionId != MARIO_MOTION_YOSHI &&
                mario2->currMotionId != MARIO_MOTION_SLIT && mario2->currMotionId != MARIO_MOTION_ROLL) {
                mario2 = marioGetPtr();
                if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                    if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                        if (mario2->dispFlags & (1 << 27)) {
                            mario2->dispFlags &= ~(1 << 27);
                        }
                        if (mario2->dispFlags & (1 << 2)) {
                            mario2->dispFlags &= ~(1 << 2);
                        }
                    }
                    mario2->prevMotionId = mario2->currMotionId;
                    mario2->currMotionId = MARIO_MOTION_STAY;
                    if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                        marioMotTbl[mario2->prevMotionId].unk8();
                    }
                    mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
                }
            }
        }
        if (mario->baseSpeed > mario->mBaseWalkSpeed) {
            mario2 = marioGetPtr();
            if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario2->dispFlags & (1 << 27)) {
                        mario2->dispFlags &= ~(1 << 27);
                    }
                    if (mario2->dispFlags & (1 << 2)) {
                        mario2->dispFlags &= ~(1 << 2);
                    }
                }
                mario2->prevMotionId = mario2->currMotionId;
                mario2->currMotionId = MARIO_MOTION_DASH;
                if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario2->prevMotionId].unk8();
                }
                mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
        } else {
            mario2 = marioGetPtr();
            if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario2->dispFlags & (1 << 27)) {
                        mario2->dispFlags &= ~(1 << 27);
                    }
                    if (mario2->dispFlags & (1 << 2)) {
                        mario2->dispFlags &= ~(1 << 2);
                    }
                }
                mario2->prevMotionId = mario2->currMotionId;
                mario2->currMotionId = MARIO_MOTION_WALK;
                if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario2->prevMotionId].unk8();
                }
                mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
        }
    }
}

void mot_talk(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* npc;

    if (mario->characterId == 1) {
        peach_talk();
        return;
    }
    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        f32 camYaw;
        f32 angle;
        f32 d;
        f32 r;

        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = 0.0f;
        npc = marioGetTouchNpcPtr();
        if (npc == NULL) {
            marioAdjustMoveDir();
        } else {
            camYaw = camGetPtr(4)->viewYaw;
            angle = angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z);
            angle = angle - camYaw;
            d = 270.0f - (f32)(s32)(angle * 10.0f + 0.5f) / 10.0f;
            r = revise360(d);
            if (r >= 90.0f && r <= 270.0f) {
                *(f32*)((u8*)mario + 0x19C) = toMovedir(90.0f);
                mario->directionView = *(f32*)((u8*)mario + 0x19C);
                *(f32*)((u8*)mario + 0x1AC) = 180.0f;
            } else {
                *(f32*)((u8*)mario + 0x19C) = toMovedir(90.0f);
                mario->directionView = *(f32*)((u8*)mario + 0x19C);
                *(f32*)((u8*)mario + 0x1AC) = 0.0f;
            }
        }
        if (mario->characterId == 2) {
            kpaChgPose("KPA_S_1", "KPA2_S_1");
            return;
        }
        {
            FieldBattleData* fbat = fbatGetPointer();
            if (*(void**)((u8*)fbat + 0x4) != NULL) {
                marioChgPose(*(const char**)((u8*)*(void**)((u8*)fbat + 0x4) + 0x10C));
            }
        }
        if (strcmp(mario->animName, "M_I_2") == 0) {
            mario->multiTimer = 0x3C;
            mario->currSubMotionId = 10;
        }
    }
    switch (mario->currSubMotionId) {
    case 0:
        break;
    case 10:
        if ((s32)--mario->multiTimer <= 0) {
            marioChgPose("M_S_1");
            mario->currSubMotionId = 11;
        }
        break;
    default:
        break;
    }
}

void marioJump(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    MarioWork* mario3;
    f32 oldVy;
    f32 grav;
    f32* p;

    if (getRollEvtFlag() != 0) {
        return;
    }
    if ((s32)mario->currMotionId != MARIO_MOTION_JUMP_NPC && (s32)mario->currMotionId >= MARIO_MOTION_JUMP_NPC &&
        (s32)mario->currMotionId < MARIO_MOTION_JUMP_SMALL) {
        return;
    }
    if ((s32)mario->currSubMotionId < 10) {
        return;
    }
    oldVy = *(f32*)((u8*)mario + 0x7C);

    mario2 = marioGetPtr();
    grav = (mario2->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) ? 1.7f : 1.0f;
    *(f32*)((u8*)mario2 + 0x84) += *(f32*)((u8*)mario2 + 0x88) / grav;
    *(f32*)((u8*)mario2 + 0x80) += *(f32*)((u8*)mario2 + 0x84) / grav;
    *(f32*)((u8*)mario2 + 0x7C) += *(f32*)((u8*)mario2 + 0x80) / grav;

    if (*(f32*)((u8*)mario2 + 0x7C) > 0.0f) {
        if (mario->airTimer >= 4 && !(*(u16*)((u8*)mario + 0x246) & (1 << 8))) {
            *(f32*)((u8*)mario2 + 0x7C) += -0.526f;
        }
    }
    if (*(f32*)((u8*)mario2 + 0x7C) <= 0.0f) {
        if (oldVy > 0.0f) {
            *(s16*)((u8*)mario + 0x52) = mario->airTimer;
        }
        marioSetPeakYpos();
        mario2 = marioGetPtr();
        p = marioJumpData + 4;
        if (mario2->currMotionId == MARIO_MOTION_JUMP_SW) {
            p = marioJumpSwData + 4;
        }
        *(f32*)((u8*)mario2 + 0x7C) = p[0];
        *(f32*)((u8*)mario2 + 0x80) = p[1];
        *(f32*)((u8*)mario2 + 0x84) = p[2];
        *(f32*)((u8*)mario2 + 0x88) = p[3];
        if (mario2->playerGravity != 1.0f) {
            *(f32*)((u8*)mario2 + 0x7C) = 0.34f;
            *(f32*)((u8*)mario2 + 0x80) = -0.002f;
            *(f32*)((u8*)mario2 + 0x84) = -0.01f;
            *(f32*)((u8*)mario2 + 0x88) = 0.0f;
        }
        mario2 = marioGetPtr();
        grav = (mario2->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) ? 1.7f : 1.0f;
        *(f32*)((u8*)mario2 + 0x84) += *(f32*)((u8*)mario2 + 0x88) / grav;
        *(f32*)((u8*)mario2 + 0x80) += *(f32*)((u8*)mario2 + 0x84) / grav;
        *(f32*)((u8*)mario2 + 0x7C) += *(f32*)((u8*)mario2 + 0x80) / grav;
        if (marioChkSts(MARIO_MOTION_HIP) == 0) {
            mario3 = marioGetPtr();
            if (!(mario3->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario3->prevMotionId == MARIO_MOTION_HIP || mario3->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario3->dispFlags & (1 << 27)) {
                        mario3->dispFlags &= ~(1 << 27);
                    }
                    if (mario3->dispFlags & (1 << 2)) {
                        mario3->dispFlags &= ~(1 << 2);
                    }
                }
                mario3->prevMotionId = mario3->currMotionId;
                mario3->currMotionId = MARIO_MOTION_FALL;
                if (marioMotTbl[mario3->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario3->prevMotionId].unk8();
                }
                mario3->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
        } else {
            *(f32*)((u8*)mario + 0x7C) = -0.03f;
            *(f32*)((u8*)mario + 0x80) = -0.02f;
            *(f32*)((u8*)mario + 0x84) = -0.02f;
            *(f32*)((u8*)mario + 0x88) = -0.002f;
            mario->flags &= ~MARIO_FLAG_IS_JUMPING;
            mario->flags |= MARIO_FLAG_IS_FALLING;
        }
    }
    *(f32*)((u8*)mario + 0x90) += *(f32*)((u8*)mario + 0x7C);
}

void mot_bottomless(void) {
    f32 landY;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 f31;
    f32 f29;
    f32 f30;
    Vec vec2;
    Vec pos;
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    void* hit;
    f32* p;

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->baseSpeed = 0.0f;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        if (*(f32*)((u8*)mario + 0xE4) < -2000.0f) {
            if (strcmp(gp->currentMapName, "win_03") == 0) {
                f31 = 250.0f;
                landY = 13.0f;
                f29 = mario->position.z;
            } else if (strcmp(gp->currentMapName, "win_01") == 0) {
                f31 = 220.0f;
                landY = 1.0f;
                f29 = mario->position.z;
            } else if (strcmp(gp->currentMapName, "tik_01") == 0) {
                f31 = 100.0f;
                landY = 120.0f;
                f29 = mario->position.z;
            }
        } else {
            f31 = *(f32*)((u8*)mario + 0xE0);
            landY = *(f32*)((u8*)mario + 0xE4);
            f29 = *(f32*)((u8*)mario + 0xE8);
        }
        pos = vec3_802c59f0;
        pos.x = f31;
        pos.y = 300.0f + landY;
        pos.z = f29;
        vec2 = pos;
        while (1) {
            marioGetScreenPos(vec2, &sx, &sy, &sz);
            if (marioChkInScreen((s32)sx, (s32)sy) == 0) {
                break;
            }
            vec2.y += 10.0f;
        }
        mario->position.y = vec2.y;
        *(f32*)((u8*)mario + 0x2AC) = 8.0f;
        mario->multiTimer = (s16)(distABf(f31, f29, mario->position.x, mario->position.z) /
                                  *(f32*)((u8*)mario + 0x2AC));
        *(f32*)((u8*)mario + 0x2B0) = angleABf(mario->position.x, mario->position.z, f31, f29);
    }
    switch (mario->currSubMotionId) {
    case 0:
        movePos(&mario->position.x, &mario->position.z, *(f32*)((u8*)mario + 0x2AC),
                *(f32*)((u8*)mario + 0x2B0));
        mario->field_EC.x = mario->position.x;
        mario->field_EC.z = mario->position.z;
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 2;
            mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
            mario2 = marioGetPtr();
            p = marioJumpData + 4;
            if (mario2->currMotionId == MARIO_MOTION_JUMP_SW) {
                p = marioJumpSwData + 4;
            }
            *(f32*)((u8*)mario2 + 0x7C) = p[0];
            *(f32*)((u8*)mario2 + 0x80) = p[1];
            *(f32*)((u8*)mario2 + 0x84) = p[2];
            *(f32*)((u8*)mario2 + 0x88) = p[3];
            if (mario2->playerGravity != 1.0f) {
                *(f32*)((u8*)mario2 + 0x7C) = 0.34f;
                *(f32*)((u8*)mario2 + 0x80) = -0.002f;
                *(f32*)((u8*)mario2 + 0x84) = -0.01f;
                *(f32*)((u8*)mario2 + 0x88) = 0.0f;
            }
        }
        break;
    case 2:
        *(void**)((u8*)mario + 0x1E4) = NULL;
        mario2 = marioGetPtr();
        *(f32*)((u8*)mario2 + 0x84) += *(f32*)((u8*)mario2 + 0x88);
        if (*(f32*)((u8*)mario2 + 0x84) > 0.0f) {
            *(f32*)((u8*)mario2 + 0x84) = 0.0f;
        }
        *(f32*)((u8*)mario2 + 0x80) += *(f32*)((u8*)mario2 + 0x84);
        *(f32*)((u8*)mario2 + 0x7C) += *(f32*)((u8*)mario2 + 0x80);
        if (*(f32*)((u8*)mario2 + 0x7C) <= marioJumpData[9]) {
            *(f32*)((u8*)mario2 + 0x7C) = marioJumpData[9];
            *(f32*)((u8*)mario2 + 0x80) = 0.0f;
            *(f32*)((u8*)mario2 + 0x84) = 0.0f;
            *(f32*)((u8*)mario2 + 0x88) = 0.0f;
        }
        f30 = *(f32*)((u8*)mario2 + 0x7C);
        if (f30 < -10.0f) {
            f30 = -10.0f;
        }
        hit = marioChkLandon(&landY, f30);
        if (hit == NULL) {
            mario->position.y += f30;
        } else {
            psndSFXOn_3D((const char*)0xB9, &mario->position);
            marioChgPose("M_D_2");
            psndSFXOn_3D((const char*)0x197, &mario->position);
            marioDmgRetQuakeOn();
            *(void**)((u8*)mario + 0x1E4) = hit;
            *(void**)((u8*)mario + 0x1E8) = NULL;
            mario->position.y = landY;
        }
        if (*(void**)((u8*)mario + 0x1E4) == NULL) {
            break;
        }
        // fall through
    case 10:
        mario->multiTimer = 0x32;
        mario->currSubMotionId = 0xB;
        // fall through
    case 11:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        pouchAddHP(-1);
        if (pouchGetHP() <= 0) {
            mario->currSubMotionId = 0xD;
            marioUpdateCamPos();
            break;
        }
        marioChgPose("M_U_3");
        mario->multiTimer = 0xC;
        mario->currSubMotionId = 0xC;
        // fall through
    case 12:
        if ((s32)--mario->multiTimer <= 0) {
            mario = marioGetPtr();
            if (!(mario->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                if (mario->prevMotionId == MARIO_MOTION_HIP || mario->prevMotionId == MARIO_MOTION_HIP_2) {
                    if (mario->dispFlags & (1 << 27)) {
                        mario->dispFlags &= ~(1 << 27);
                    }
                    if (mario->dispFlags & (1 << 2)) {
                        mario->dispFlags &= ~(1 << 2);
                    }
                }
                mario->prevMotionId = mario->currMotionId;
                mario->currMotionId = MARIO_MOTION_STAY;
                if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                    marioMotTbl[mario->prevMotionId].unk8();
                }
                mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
            }
            marioUpdateCamPos();
        }
        break;
    default:
        break;
    }
}

void marioSetJumpPara(void) {
    MarioWork* mario = marioGetPtr();
    f32* p;
    f32 s;

    switch (mario->currMotionId) {
    case MARIO_MOTION_JUMP_SW:
        p = marioJumpSwData;
        *(f32*)((u8*)mario + 0x7C) = *p++;
        *(f32*)((u8*)mario + 0x80) = *p++;
        *(f32*)((u8*)mario + 0x84) = *p++;
        *(f32*)((u8*)mario + 0x88) = *p++;
        break;
    case MARIO_MOTION_JUMP:
    case MARIO_MOTION_UPSTAIRS:
    case MARIO_MOTION_JABARA:
    case MARIO_MOTION_PLANE:
    case MARIO_MOTION_SHIP:
        if (mario->trigFlags & MARIO_TRIG_FLAG_SET_JUMP_PARAMS) {
            p = marioJumpData;
            if (mario->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) {
                s = 0.5f;
                *(f32*)((u8*)mario + 0x7C) = s * p[0];
                *(f32*)((u8*)mario + 0x80) = s * p[1];
                *(f32*)((u8*)mario + 0x84) = s * p[2];
                *(f32*)((u8*)mario + 0x88) = s * p[3];
            }
        } else {
            p = marioJumpData;
            if (!(mario->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS)) {
                *(f32*)((u8*)mario + 0x7C) = p[0];
                *(f32*)((u8*)mario + 0x80) = p[1];
                *(f32*)((u8*)mario + 0x84) = p[2];
                *(f32*)((u8*)mario + 0x88) = p[3];
            } else {
                s = 0.5f;
                *(f32*)((u8*)mario + 0x7C) = s * p[0];
                *(f32*)((u8*)mario + 0x80) = s * p[1];
                *(f32*)((u8*)mario + 0x84) = s * p[2];
                *(f32*)((u8*)mario + 0x88) = s * p[3];
            }
            if (mario->playerGravity != 1.0f) {
                *(f32*)((u8*)mario + 0x7C) = 2.0f;
                *(f32*)((u8*)mario + 0x80) = -0.003f;
                *(f32*)((u8*)mario + 0x84) = 0.029f;
                *(f32*)((u8*)mario + 0x88) = -0.003f;
            }
        }
        break;
    case MARIO_MOTION_JUMP_NPC:
        *(f32*)((u8*)mario + 0x7C) = 2.0f;
        *(f32*)((u8*)mario + 0x80) = -0.04f;
        *(f32*)((u8*)mario + 0x84) = -0.01f;
        *(f32*)((u8*)mario + 0x88) = -0.003f;
        break;
    case MARIO_MOTION_ROLL:
        *(f32*)((u8*)mario + 0x7C) = marioJumpData[0];
        *(f32*)((u8*)mario + 0x80) = marioJumpData[1];
        *(f32*)((u8*)mario + 0x84) = marioJumpData[2];
        *(f32*)((u8*)mario + 0x88) = marioJumpData[3];
        if (mario->playerGravity != 1.0f) {
            *(f32*)((u8*)mario + 0x7C) = 2.7f;
            *(f32*)((u8*)mario + 0x80) = -0.002f;
            *(f32*)((u8*)mario + 0x84) = 0.016f;
            *(f32*)((u8*)mario + 0x88) = -0.002f;
        }
        break;
    default:
        break;
    }
    if (mario->currMotionId != MARIO_MOTION_JUMP_3 && mario->characterId == 2) {
        kpaSetJumpPara();
    }
}

void marioFall(void) {
    f32 f31;
    s32 unk14;
    s32 unk10;
    f32 unkC;
    f32 landY;
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    void* hit;
    s32 rub;

    if (strcmp(gp->currentMapName, "kpa_03") == 0 && mario->characterId == 2) {
        *(void**)((u8*)mario + 0x1E4) = NULL;
        kpaClearHitobjRide();
        mario2 = marioGetPtr();
        *(f32*)((u8*)mario2 + 0x84) += *(f32*)((u8*)mario2 + 0x88);
        if (*(f32*)((u8*)mario2 + 0x84) > 0.0f) {
            *(f32*)((u8*)mario2 + 0x84) = 0.0f;
        }
        *(f32*)((u8*)mario2 + 0x80) += *(f32*)((u8*)mario2 + 0x84);
        *(f32*)((u8*)mario2 + 0x7C) += *(f32*)((u8*)mario2 + 0x80);
        if (*(f32*)((u8*)mario2 + 0x7C) <= marioJumpData[9]) {
            *(f32*)((u8*)mario2 + 0x7C) = marioJumpData[9];
            *(f32*)((u8*)mario2 + 0x80) = 0.0f;
            *(f32*)((u8*)mario2 + 0x84) = 0.0f;
            *(f32*)((u8*)mario2 + 0x88) = 0.0f;
        }
        f31 = *(f32*)((u8*)mario2 + 0x7C);
        hit = marioChkLandon(&landY, f31);
        if (hit != NULL) {
            *(void**)((u8*)mario + 0x1E4) = hit;
            *(void**)((u8*)mario + 0x1E8) = NULL;
            mario->position.y = landY;
        } else {
            mario->position.y += f31;
        }
        if (*(void**)((u8*)mario + 0x1E4) != NULL) {
            marioLandOn();
        }
        return;
    }
    if (!(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
        if (marioChkInScreen(mario->unk168, mario->unk16C) == 0) {
            mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
            mario->baseSpeed = 0.1f * -mario->baseSpeed + mario->baseSpeed;
        }
    }
    if (getRollEvtFlag() != 0) {
        return;
    }
    if (mario->currMotionId == MARIO_MOTION_HIP || mario->currMotionId == MARIO_MOTION_HIP_2) {
        return;
    }
    if (marioKeyOffChk() == 0 || marioCtrlOffChk() == 0) {
        if (!(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
            f32 f3 = mario->baseSpeed;
            f3 = 0.01f * (-0.5f - f3) + f3;
            if (f3 < 0.0f) {
                f3 = 0.0f;
            }
            mario->baseSpeed = f3;
        }
    }
    *(void**)((u8*)mario + 0x1E4) = NULL;
    if (mario->characterId == 2) {
        kpaClearHitobjRide();
    }
    if (marioChkSts(MARIO_MOTION_HIP) == 1) {
        mot_fall_roll();
        return;
    }
    rub = marioGetRub(0x100, &unk10, &unk14, &unkC);
    mario2 = marioGetPtr();
    *(f32*)((u8*)mario2 + 0x84) += *(f32*)((u8*)mario2 + 0x88);
    if (*(f32*)((u8*)mario2 + 0x84) > 0.0f) {
        *(f32*)((u8*)mario2 + 0x84) = 0.0f;
    }
    *(f32*)((u8*)mario2 + 0x80) += *(f32*)((u8*)mario2 + 0x84);
    *(f32*)((u8*)mario2 + 0x7C) += *(f32*)((u8*)mario2 + 0x80);
    if (*(f32*)((u8*)mario2 + 0x7C) <= marioJumpData[9]) {
        *(f32*)((u8*)mario2 + 0x7C) = marioJumpData[9];
        *(f32*)((u8*)mario2 + 0x80) = 0.0f;
        *(f32*)((u8*)mario2 + 0x84) = 0.0f;
        *(f32*)((u8*)mario2 + 0x88) = 0.0f;
    }
    f31 = *(f32*)((u8*)mario2 + 0x7C);
    hit = marioChkLandon(&landY, f31);
    if (hit != NULL) {
        *(void**)((u8*)mario + 0x1E4) = hit;
        *(void**)((u8*)mario + 0x1E8) = NULL;
        if (hitGetAttr((HitEntry*)hit) & (1 << 9)) {
            mario->position.y += f31;
        } else {
            mario->position.y = landY;
            if (marioPriCheckJabara() != 0 && unk10 != -1 && rub >= 0) {
                camFollowYOn();
                mario2 = marioGetPtr();
                if (!(mario2->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {
                    if (mario2->prevMotionId == MARIO_MOTION_HIP || mario2->prevMotionId == MARIO_MOTION_HIP_2) {
                        if (mario2->dispFlags & (1 << 27)) {
                            mario2->dispFlags &= ~(1 << 27);
                        }
                        if (mario2->dispFlags & (1 << 2)) {
                            mario2->dispFlags &= ~(1 << 2);
                        }
                    }
                    mario2->prevMotionId = mario2->currMotionId;
                    mario2->currMotionId = MARIO_MOTION_JABARA;
                    if (marioMotTbl[mario2->prevMotionId].unk8 != NULL) {
                        marioMotTbl[mario2->prevMotionId].unk8();
                    }
                    mario2->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
                }
                return;
            }
        }
    } else {
        mario->position.y += f31;
    }
    if (mario->characterId != 2) {
        if (*(void**)((u8*)mario + 0x1E4) != NULL) {
            marioLandOn();
        }
    } else {
        if ((u32)kpaChkHitobjRide() != 0) {
            marioLandOn();
        }
    }
}

#define MARIO_CHG_BLOCK(m, next)                                                        \
    if (!((m)->flags & MARIO_FLAG_CHG_MOT_DISABLED)) {                                  \
        if ((m)->prevMotionId == MARIO_MOTION_HIP || (m)->prevMotionId == MARIO_MOTION_HIP_2) { \
            if ((m)->dispFlags & (1 << 27)) {                                           \
                (m)->dispFlags &= ~(1 << 27);                                           \
            }                                                                           \
            if ((m)->dispFlags & (1 << 2)) {                                            \
                (m)->dispFlags &= ~(1 << 2);                                            \
            }                                                                           \
        }                                                                               \
        (m)->prevMotionId = (m)->currMotionId;                                          \
        (m)->currMotionId = (next);                                                     \
        if (marioMotTbl[(m)->prevMotionId].unk8 != NULL) {                              \
            marioMotTbl[(m)->prevMotionId].unk8();                                      \
        }                                                                               \
        (m)->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;                       \
    }

void marioLandOn(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    Vec pos;
    void* obj;
    void* stand;
    s32 attr;
    s32 action;
    s32 sfx;

    if (!(hitGetAttr((HitEntry*)mario->hitobjStandOn) & (1 << 9))) {
        mario2 = marioGetPtr();
        *(f32*)((u8*)mario2 + 0x7C) = 0.0f;
        *(f32*)((u8*)mario2 + 0x80) = 0.0f;
        *(f32*)((u8*)mario2 + 0x84) = 0.0f;
        *(f32*)((u8*)mario2 + 0x88) = 0.0f;
    }
    mario->flags &= ~MARIO_FLAG_STOP_AIR_MOVEMENT;
    mario->flags &= ~MARIO_FLAG_USE_FORCE_DIRECTION;
    mario->airTimer = 0;
    *(s16*)((u8*)mario + 0x52) = 0;
    if (mario->characterId != 2) {
        if (marioBgmodeChk() == 0) {
            camFollowYOn();
        }
    } else {
        kpaCamFollow();
    }
    stand = mario->hitobjStandOn;
    pos = mario->position;
    if (stand != NULL) {
        attr = hitGetAttr((HitEntry*)stand);
        if (attr & (1 << 8)) {
            sfx = 0x150;
        } else if (attr & (1 << 21)) {
            sfx = 0x151;
        } else if (attr & (1 << 20)) {
            sfx = 0x152;
        } else if (attr & (1 << 12)) {
            sfx = 0x153;
        } else if (attr & (1 << 22)) {
            sfx = 0x154;
        } else {
            sfx = 0x14F;
        }
        psndSFXOn_3D((const char*)sfx, &pos);
    }
    if (mario->flags & MARIO_FLAG_TUBE_MODE) {
        mario->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING);
        *(void**)((u8*)mario + 0x1E8) = NULL;
        return;
    }
    *(f32*)((u8*)mario + 0x2B0) =
        fabsf(distABf(mario->position.x, mario->position.z, *(f32*)((u8*)mario + 0x11C),
                      *(f32*)((u8*)mario + 0x124)));
    attr = hitGetAttr((HitEntry*)mario->hitobjStandOn);
    if (mario->currMotionId != MARIO_MOTION_DAMAGE && (attr & (1 << 9))) {
        set_damage_root_ypos(mario->position.y);
        mario2 = marioGetPtr();
        MARIO_CHG_BLOCK(mario2, MARIO_MOTION_DAMAGE);
        return;
    }
    if (mario->currMotionId != MARIO_MOTION_DAMAGE_TOGE && (attr & (1 << 11))) {
        set_damage_root_ypos(mario->position.y);
        mario2 = marioGetPtr();
        MARIO_CHG_BLOCK(mario2, MARIO_MOTION_DAMAGE_TOGE);
        return;
    }
    attr = hitGetAttr((HitEntry*)mario->hitobjStandOn);
    if ((u32)attr & (1 << 31)) {
        obj = mobjHitObjPtrToPtr((HitEntry*)mario->hitobjStandOn);
        if (strcmp((const char*)obj + 0x15, "MOBJ_RedSwitch") == 0) {
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_JUMP_SW);
            return;
        }
        if (strcmp((const char*)obj + 0x15, "MOBJ_BlueJumpStand") == 0) {
            mario = marioGetPtr();
            camFollowYOn();
            if (marioChkSts(MARIO_MOTION_HIP) != 0) {
                *(f32*)((u8*)mario + 0x2B8) = 100.0f;
                effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);
                mario2 = marioGetPtr();
                MARIO_CHG_BLOCK(mario2, MARIO_MOTION_JUMP_STAND);
                return;
            }
            if (mario->currMotionId == MARIO_MOTION_YOSHI) {
                *(f32*)((u8*)mario + 0x2B8) = 100.0f;
                yoshi_jumpStand();
                return;
            }
            *(f32*)((u8*)mario + 0x2B8) = 100.0f;
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_JUMP_STAND);
            return;
        }
        if (mario->characterId == 2) {
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_STAY);
            return;
        }
        if (mario->currMotionId != MARIO_MOTION_SLIT) {
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_STAY);
            return;
        }
        if (mario->flags & MARIO_FLAG_HAMMER_HIT_WALL) {
            unk_800a4e40();
        } else if (mario->dispFlags & (1 << 24)) {
            if (marioChkSlitEnd() == 0) {
                motSlitCancel();
            }
        }
        return;
    }
    if (*(f32*)((u8*)mario + 0x190) > 0.0f && !(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
        if (mario->characterId == 2) {
            if (kpaFireAttackCheck() != 0) {
                mario2 = marioGetPtr();
                MARIO_CHG_BLOCK(mario2, MARIO_MOTION_STAY);
            } else {
                mario2 = marioGetPtr();
                MARIO_CHG_BLOCK(mario2, MARIO_MOTION_WALK);
            }
            quake_kpaLandOn();
        } else {
            if (!(mario->dispFlags & (1 << 24))) {
                mario2 = marioGetPtr();
                MARIO_CHG_BLOCK(mario2, MARIO_MOTION_WALK);
            } else {
                N_marioSlitSetAfterLand();
            }
        }
        return;
    }
    if (mario->flags & MARIO_FLAG_TUBE_MODE) {
        mario2 = marioGetPtr();
        mario2->currSubMotionId = 0x32;
        mario2->currMotionId = MARIO_MOTION_ROLL;
        return;
    }
    if (mario->characterId == 2) {
        mario2 = marioGetPtr();
        MARIO_CHG_BLOCK(mario2, MARIO_MOTION_STAY);
        quake_kpaLandOn();
        return;
    }
    if (mario->currMotionId != MARIO_MOTION_SLIT) {
        if (!(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_STAY);
        } else {
            action = (mario->baseSpeed > mario->mBaseWalkSpeed) ? 2 : 1;
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, (MarioMotion)action);
        }
        return;
    }
    if (mario->flags & MARIO_FLAG_HAMMER_HIT_WALL) {
        unk_800a4e40();
    } else if (mario->dispFlags & (1 << 24)) {
        if (marioChkSlitEnd() == 0) {
            motSlitCancel();
        }
    }
}

#undef MARIO_CHG_BLOCK
