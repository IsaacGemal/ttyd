#include "eff/eff_kemuri.h"
#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_party.h"
#include "party.h"
#include "mario/mario_sbr.h"
#include "mot/mot_slit.h"
#include "motion/mot_damage.h"
#include "motion/mot_dokan.h"
#include "motion/mot_hip.h"
#include "motion/mot_jabara.h"
#include "motion/mot_roll.h"
#include "motion/mot_ship.h"
#include "motion/mot_stay.h"
#include "motion/mot_walk.h"
#include "motion/mot_yoshi.h"
#include "party/party_cloud.h"
#include "party/party_vivian.h"
#include "driver/hitdrv.h"
#include "driver/casedrv.h"
#include "driver/npcdrv.h"
#include "driver/camdrv.h"
#include "system.h"
#include "math_ext.h"
#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "mario/mariost.h"
#include "koopa.h"
#include "driver/mobjdrv.h"
#include "mario/mario_cam.h"
#include "motion/mot_roll.h"
#include "party/party_yoshi.h"
#include "peach.h"
#include "mario/mario_hit.h"
#include "motion/mot_slit.h"
#include "motion/mot_ship.h"
#include "motion/mot_hip.h"
#include "motion/mot_plane.h"
#include "motion/mot_damage.h"
#include "motion/mot_hammer.h"
#include "koopa_motion.h"
#include "koopa_hit.h"
#include "motion/mot_jabara.h"
#include "evt/evt_cmd.h"
#include "motion/mot_jump.h"
#include "party/party_cloud.h"

// From src/mario/mario.c; no header declares it yet.

// Motion table entry. Lives at marioMotTbl (0x8030E5E0), 12 bytes each.
typedef struct MotTblEntry {
    void (*unk0)(void);  // 0x0
    u32 unk4;            // 0x4
    void (*unk8)(void);  // 0x8
} MotTblEntry;

extern MotTblEntry marioMotTbl[];

// .sdata jump parameter tables, defined in this unit's reference assembly.
f32 marioJumpData[10] = { 8.0f, -0.84f, 0.08f, -0.02f, -0.1f, -0.2f, 0.024f, -0.004f, 32.0f, -18.0f };
extern f32 marioJumpSwData[];


// clang-format off

void marioChgMoveMotion(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;

    if (mario->currMotionId == MARIO_MOTION_PLANE) {
        mario_plane_cancel();
    } else if (mario->currMotionId == MARIO_MOTION_ROLL) {
        allPartyForceRideOn();
    } else if (mario->currMotionId != MARIO_MOTION_YOSHI) {
        if (!mario->baseSpeed) {
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


void marioChgStayMotion(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId != MARIO_MOTION_SHIP && mario->currMotionId != MARIO_MOTION_YOSHI &&
        mario->currMotionId != MARIO_MOTION_SLIT && mario->currMotionId != MARIO_MOTION_ROLL) {
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
        mario->jumpHeight = arg;
        effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);
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
            mario->currMotionId = MARIO_MOTION_JUMP_STAND;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        mario->jumpHeight = arg;
        yoshi_jumpStand();
    } else {
        mario->jumpHeight = arg;
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
    mario->jumpVelY = 6.0f;
    mario->jumpAccelY = -0.5f;
    mario->jumpJerkY = 0.0f;
    mario->jumpJerkStepY = 0.0f;
}


BOOL marioChkItemGetMotion(void) {
    BOOL result = FALSE;
    MarioWork* mario = marioGetPtr();
    u32 motion = mario->currMotionId;

    if (motion == MARIO_MOTION_STAY ||
        (u16)(motion - MARIO_MOTION_WALK) <= (MARIO_MOTION_DASH - MARIO_MOTION_WALK) ||
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
        if (mario->airTimerPeak < 6) {
            return;
        }
        if (kpaHipAttackOk() == 0) {
            return;
        }
        if (!(mario->buttonNew & (1 << 8))) {
            return;
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
            mario->currMotionId = MARIO_MOTION_HIP;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
    }
}


void marioMotion(void) {
    MarioWork* mario = marioGetPtr();

    mario->hitobjHammer = 0;
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

    mario->jumpVelY = 0.0f;
    mario->jumpAccelY = 0.0f;
    mario->jumpJerkY = 0.0f;
    mario->jumpJerkStepY = 0.0f;
}


// The ROLL case sets up the array base pointer before the 1.0f comparison constant is loaded;
// the const pointer keeps the compiler from sinking that setup below the first array load.
void marioSetJumpPara(void) {
    MarioWork* mario = marioGetPtr();
    f32* p;
    f32 s;

    switch (mario->currMotionId) {
    case MARIO_MOTION_JUMP_SW:
        p = marioJumpSwData;
        mario->jumpVelY = *p++;
        mario->jumpAccelY = *p++;
        mario->jumpJerkY = *p++;
        mario->jumpJerkStepY = *p++;
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
                mario->jumpVelY = s * p[0];
                mario->jumpAccelY = s * p[1];
                mario->jumpJerkY = s * p[2];
                mario->jumpJerkStepY = s * p[3];
            }
        } else {
            p = marioJumpData;
            if (!(mario->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS)) {
                mario->jumpVelY = p[0];
                mario->jumpAccelY = p[1];
                mario->jumpJerkY = p[2];
                mario->jumpJerkStepY = p[3];
            } else {
                s = 0.5f;
                mario->jumpVelY = s * p[0];
                mario->jumpAccelY = s * p[1];
                mario->jumpJerkY = s * p[2];
                mario->jumpJerkStepY = s * p[3];
            }
            if (mario->playerGravity != 1.0f) {
                mario->jumpVelY = 2.0f;
                mario->jumpAccelY = -0.003f;
                mario->jumpJerkY = 0.029f;
                mario->jumpJerkStepY = -0.003f;
            }
        }
        break;
    case MARIO_MOTION_JUMP_NPC:
        mario->jumpVelY = 2.0f;
        mario->jumpAccelY = -0.04f;
        mario->jumpJerkY = -0.01f;
        mario->jumpJerkStepY = -0.003f;
        break;
    case MARIO_MOTION_ROLL: {
        // A const pointer pins the base-pointer setup ahead of the 1.0f load.
        f32* const q = marioJumpData;
        mario->jumpVelY = q[0];
        mario->jumpAccelY = q[1];
        mario->jumpJerkY = q[2];
        mario->jumpJerkStepY = q[3];
        if (mario->playerGravity != 1.0f) {
            mario->jumpVelY = 2.7f;
            mario->jumpAccelY = -0.002f;
            mario->jumpJerkY = 0.016f;
            mario->jumpJerkStepY = -0.002f;
        }
        break;
    }
    default:
        break;
    }
    if (mario->currMotionId != MARIO_MOTION_JUMP_3 && mario->characterId == 2) {
        kpaSetJumpPara();
    }
}


void marioSetFallPara(void) {
    MarioWork* mario = marioGetPtr();
    f32* para = marioJumpData + 4;

    if (mario->currMotionId == MARIO_MOTION_JUMP_SW) {
        para = marioJumpSwData + 4;
    }
    mario->jumpVelY = para[0];
    mario->jumpAccelY = para[1];
    mario->jumpJerkY = para[2];
    mario->jumpJerkStepY = para[3];
    if (mario->playerGravity != 1.0f) {
        mario->jumpVelY = 0.34f;
        mario->jumpAccelY = -0.002f;
        mario->jumpJerkY = -0.01f;
        mario->jumpJerkStepY = 0.0f;
    }
}


f32 marioGetFallSpd(void) {
    MarioWork* mario = marioGetPtr();

    mario->jumpJerkY += mario->jumpJerkStepY;
    if (mario->jumpJerkY > 0.0f) {
        mario->jumpJerkY = 0.0f;
    }
    mario->jumpAccelY += mario->jumpJerkY;
    mario->jumpVelY += mario->jumpAccelY;
    if (mario->jumpVelY <= marioJumpData[9]) {
        mario->jumpVelY = marioJumpData[9];
        mario->jumpAccelY = 0.0f;
        mario->jumpJerkY = 0.0f;
        mario->jumpJerkStepY = 0.0f;
    }
    return mario->jumpVelY;
}


f32 marioMakeJumpPara(void) {
    MarioWork* mario = marioGetPtr();
    f32 div;

    if (mario->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) {
        div = 1.7f;
    } else {
        div = 1.0f;
    }
    mario->jumpJerkY += mario->jumpJerkStepY / div;
    mario->jumpAccelY += mario->jumpJerkY / div;
    mario->jumpVelY += mario->jumpAccelY / div;
    return mario->jumpVelY;
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
    // The explicit JUMP_NPC case makes MWCC emit the original's split compare (`beq 4; blt 4`)
    // ahead of the 5..8 range instead of a `subi`/`cmplwi` range check.
    switch (mario->currMotionId) {
    case MARIO_MOTION_JUMP_NPC:
        break;
    case MARIO_MOTION_JUMP_SW:
    case MARIO_MOTION_JUMP_STAND:
    case MARIO_MOTION_JUMP_2:
    case MARIO_MOTION_JUMP_3:
        return;
    default:
        break;
    }
    if (mario->currSubMotionId < 10) {
        return;
    }
    oldVy = mario->jumpVelY;

    mario2 = marioGetPtr();
    grav = (mario2->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) ? 1.7f : 1.0f;
    mario2->jumpJerkY += mario2->jumpJerkStepY / grav;
    mario2->jumpAccelY += mario2->jumpJerkY / grav;
    mario2->jumpVelY += mario2->jumpAccelY / grav;

    if (mario->jumpVelY > 0.0f) {
        if (mario->airTimer >= 4 && !(mario->buttonHeld & (1 << 8))) {
            mario->jumpVelY += -0.526f;
        }
    }
    if (mario->jumpVelY <= 0.0f) {
        if (oldVy > 0.0f) {
            mario->airTimerPeak = mario->airTimer;
        }
        marioSetPeakYpos();
        mario2 = marioGetPtr();
        p = marioJumpData + 4;
        if (mario2->currMotionId == MARIO_MOTION_JUMP_SW) {
            p = marioJumpSwData + 4;
        }
        mario2->jumpVelY = p[0];
        mario2->jumpAccelY = p[1];
        mario2->jumpJerkY = p[2];
        mario2->jumpJerkStepY = p[3];
        if (1.0f != mario2->playerGravity) {
            mario2->jumpVelY = 0.34f;
            mario2->jumpAccelY = -0.002f;
            mario2->jumpJerkY = -0.01f;
            mario2->jumpJerkStepY = 0.0f;
        }
        mario2 = marioGetPtr();
        grav = (mario2->flags & MARIO_FLAG_PRE_BATTLE_PHYSICS) ? 1.7f : 1.0f;
        mario2->jumpJerkY += mario2->jumpJerkStepY / grav;
        mario2->jumpAccelY += mario2->jumpJerkY / grav;
        mario2->jumpVelY += mario2->jumpAccelY / grav;
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
            mario->jumpVelY = -0.03f;
            mario->jumpAccelY = -0.02f;
            mario->jumpJerkY = -0.02f;
            mario->jumpJerkStepY = -0.002f;
            mario->flags &= ~MARIO_FLAG_IS_JUMPING;
            mario->flags |= MARIO_FLAG_IS_FALLING;
        }
    }
    mario->position.y += mario->jumpVelY;
}


void marioFall(void) {
    f32 f31;
    s32 unk14;
    s32 unk10;
    f32 unkC;
    f32 landY;
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    HitEntry* hit;
    s32 rub;

    if (strcmp(gp->currentMapName, "kpa_03") == 0 && mario->characterId == 2) {
        mario->hitobjStandOn = NULL;
        kpaClearHitobjRide();
        mario2 = marioGetPtr();
        mario2->jumpJerkY += mario2->jumpJerkStepY;
        if (mario2->jumpJerkY > 0.0f) {
            mario2->jumpJerkY = 0.0f;
        }
        mario2->jumpAccelY += mario2->jumpJerkY;
        mario2->jumpVelY += mario2->jumpAccelY;
        if (mario2->jumpVelY <= marioJumpData[9]) {
            mario2->jumpVelY = marioJumpData[9];
            mario2->jumpAccelY = 0.0f;
            mario2->jumpJerkY = 0.0f;
            mario2->jumpJerkStepY = 0.0f;
        }
        f31 = mario2->jumpVelY;
        hit = marioChkLandon(&landY, f31);
        if (hit != NULL) {
            mario->hitobjStandOn = hit;
            mario->hitobjJumpFrom = NULL;
            mario->position.y = landY;
        } else {
            mario->position.y += f31;
        }
        if (mario->hitobjStandOn != NULL) {
            marioLandOn();
        }
        return;
    }
    if (!(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
        if (marioChkInScreen(mario->screenX, mario->screenY) == 0) {
            mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
            mario->baseSpeed += 0.1f * -mario->baseSpeed;
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
            f3 = f3 + 0.01f * (-0.5f - f3);
            if (f3 < 0.0f) {
                f3 = 0.0f;
            }
            mario->baseSpeed = f3;
        }
    }
    mario->hitobjStandOn = NULL;
    if (mario->characterId == 2) {
        kpaClearHitobjRide();
    }
    if (marioChkSts(MARIO_MOTION_HIP) == 1) {
        mot_fall_roll();
        return;
    }
    rub = marioGetRub(0x100, &unk10, &unk14, &unkC);
    mario2 = marioGetPtr();
    mario2->jumpJerkY += mario2->jumpJerkStepY;
    if (mario2->jumpJerkY > 0.0f) {
        mario2->jumpJerkY = 0.0f;
    }
    mario2->jumpAccelY += mario2->jumpJerkY;
    mario2->jumpVelY += mario2->jumpAccelY;
    if (mario2->jumpVelY <= marioJumpData[9]) {
        mario2->jumpVelY = marioJumpData[9];
        mario2->jumpAccelY = 0.0f;
        mario2->jumpJerkY = 0.0f;
        mario2->jumpJerkStepY = 0.0f;
    }
    f31 = mario2->jumpVelY;
    hit = marioChkLandon(&landY, f31);
    if (hit != NULL) {
        mario->hitobjStandOn = hit;
        mario->hitobjJumpFrom = NULL;
        if (hitGetAttr(hit) & (1 << 9)) {
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
        if (mario->hitobjStandOn != NULL) {
            marioLandOn();
        }
    } else {
        if (kpaChkHitobjRide() != NULL) {
            marioLandOn();
        }
    }
}


void marioLandOn(void) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    Vec pos;
    MapObjectEntry* obj;
    HitEntry* stand;
    s32 attr;
    s32 action;
    s32 sfx;

    if (!(hitGetAttr(mario->hitobjStandOn) & (1 << 9))) {
        mario2 = marioGetPtr();
        mario2->jumpVelY = 0.0f;
        mario2->jumpAccelY = 0.0f;
        mario2->jumpJerkY = 0.0f;
        mario2->jumpJerkStepY = 0.0f;
    }
    mario->flags &= ~MARIO_FLAG_STOP_AIR_MOVEMENT;
    mario->flags &= ~MARIO_FLAG_USE_FORCE_DIRECTION;
    mario->airTimer = 0;
    mario->airTimerPeak = 0;
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
        attr = hitGetAttr(stand);
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
        mario->hitobjJumpFrom = NULL;
        return;
    }
    mario->field_0x2B0 =
        fabsf(distABf(mario->position.x, mario->position.z, mario->jumpFromPos.x,
                      mario->jumpFromPos.z));
    attr = hitGetAttr(mario->hitobjStandOn);
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
    attr = hitGetAttr(mario->hitobjStandOn);
    if ((u32)attr & (1 << 31)) {
        obj = mobjHitObjPtrToPtr(mario->hitobjStandOn);
        if (strcmp(obj->animName, "MOBJ_RedSwitch") == 0) {
            mario2 = marioGetPtr();
            MARIO_CHG_BLOCK(mario2, MARIO_MOTION_JUMP_SW);
            return;
        }
        if (strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0) {
            mario = marioGetPtr();
            camFollowYOn();
            if (marioChkSts(MARIO_MOTION_HIP) != 0) {
                mario->jumpHeight = 100.0f;
                effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, 0.0f);
                mario2 = marioGetPtr();
                MARIO_CHG_BLOCK(mario2, MARIO_MOTION_JUMP_STAND);
                return;
            }
            if (mario->currMotionId == MARIO_MOTION_YOSHI) {
                mario->jumpHeight = 100.0f;
                yoshi_jumpStand();
                return;
            }
            mario->jumpHeight = 100.0f;
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
        if (mario->flags & MARIO_FLAG_PAPER_MODE) {
            unk_800a4e40();
        } else if (mario->dispFlags & (1 << 24)) {
            if (marioChkSlitEnd() == 0) {
                motSlitCancel();
            }
        }
        return;
    }
    if (mario->controlStickSensitivity > 0.0f && !(mario->flags & MARIO_FLAG_FORCED_MOVEMENT)) {
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
    if (mario->flags & MARIO_FLAG_PAPER_MODE) {
        unk_800a4e40();
    } else if (mario->dispFlags & (1 << 24)) {
        if (marioChkSlitEnd() == 0) {
            motSlitCancel();
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
            angle = (f32)(s32)(angle * 10.0f + 0.5f) / 10.0f;
            d = 270.0f - angle;
            r = revise360(d);
            if (r >= 90.0f && r <= 270.0f) {
                mario->moveDir = toMovedir(90.0f);
                mario->directionView = mario->moveDir;
                mario->dispReqDir = 180.0f;
            } else {
                mario->moveDir = toMovedir(90.0f);
                mario->directionView = mario->moveDir;
                mario->dispReqDir = 0.0f;
            }
        }
        if (mario->characterId == 2) {
            kpaChgPose("KPA_S_1", "KPA2_S_1");
            return;
        }
        {
            FieldBattleData* fbat = fbatGetPointer();
            if (fbat->talkNpc != NULL) {
                marioChgPose(fbat->talkNpc->checkPose);
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
        if (--mario->multiTimer <= 0) {
            marioChgPose("M_S_1");
            mario->currSubMotionId = 11;
        }
        break;
    case 11:
        break;
    }
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

    if (!(mario->buttonNew & (1 << 8))) {
        return FALSE;
    }
    if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        return FALSE;
    }
    if (mario->actionHitObj != NULL) {
        if ((u32)hitGetAttr(mario->actionHitObj) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr(mario->actionHitObj)) == 1) {
                return FALSE;
            }
        }
        caseEntry = caseCheckHitObj(mario->actionHitObj);
        if (caseEntry != NULL) {
            if (caseEntry->activeConditionId == 9 || caseEntry->activeConditionId == 4 ||
                caseEntry->activeConditionId == 12) {
                return FALSE;
            }
        }
    }
    if (mario->hitobjFront != NULL) {
        if ((u32)hitGetAttr(mario->hitobjFront) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr(mario->hitobjFront)) == 1) {
                return FALSE;
            }
        }
        caseEntry = caseCheckHitObj(mario->hitobjFront);
        if (caseEntry != NULL) {
            if (caseEntry->activeConditionId == 9 || caseEntry->activeConditionId == 4 ||
                caseEntry->activeConditionId == 12) {
                return FALSE;
            }
        }
    }
    if (mario->hitobjStandOn != NULL) {
        if ((u32)hitGetAttr(mario->hitobjStandOn) & (1 << 31)) {
            if (mobjGetHint(mobjHitObjPtrToPtr(mario->hitobjStandOn)) == 1) {
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

    if (!(mario->buttonNew & (1 << 11))) {
        return FALSE;
    }
    if (!(mario->trigFlags & (1 << 2)) && mario->partyUseTimer != 0) {
        return FALSE;
    }
    if (mario->currMotionId == MARIO_MOTION_YOSHI) {
        return FALSE;
    }
    if (pouchCheckItem(5) > 0) {
        if (marioChkShipPlace(mario->hitobjStandOn) != 0) {
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
    if (pouchCheckItem(4) > 0) {
        if (marioChkPlanePlace(mario->hitobjStandOn) != 0) {
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

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return FALSE;
    }
    if (marioGetHammerLevel() == 0) {
        return FALSE;
    }
    if (marioChkHammer2() != 0) {
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
            mario->currMotionId = MARIO_MOTION_HAMMER_2;
            if (marioMotTbl[mario->prevMotionId].unk8 != NULL) {
                marioMotTbl[mario->prevMotionId].unk8();
            }
            mario->trigFlags |= MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        }
        return TRUE;
    }
    if ((mario->buttonNew & (1 << 9)) == 0) {
        return FALSE;
    }
    if (mario->flags & (MARIO_FLAG_TUBE_MODE | MARIO_FLAG_PAPER_MODE)) {
        return FALSE;
    }
    if ((u32)(mario->buttonNew & (1 << 9)) == 0) {
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
    f32 unk08;

    if (mario->flags & MARIO_FLAG_IS_8_BIT) {
        return 0;
    }
    if (!(mario->buttonHeld & (1 << 5))) {
        return 0;
    }
    if (marioGetRub(0x20, &unk0C, &unk10, &unk08) != 1) {
        return 0;
    }
    if (unk10 < 3) {
        return 0;
    }
    if (pouchCheckItem(3) == 0) {
        return 0;
    }
    {
        if (marioSlitChkWallAround() != 0 || marioCreviceWallChk(mario->position) != 0) {
            return 0;
        }
        if (mario->buttonHeld & (1 << 5)) {
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
    return 0;
}


void marioChgMotJump2(void) {
    MarioWork* mario = marioGetPtr();

    if (kpaGetBodyStatus() != 2 && mario->currMotionId != MARIO_MOTION_ROLL) {
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
        mario->spinSpeed = 0.0f;
        marioChgPose("M_P_2");
    }

    switch (mario->currSubMotionId) {
    case 0:
        mario->spinSpeed += 5.0f;
        if (mario->spinSpeed > 60.0f) {
            mario->spinSpeed = 60.0f;
        }
        mario->dispDir = revise360(mario->dispDir + mario->spinSpeed);
        if (!(mario->buttonHeld & (1 << 4))) {
            flag = FALSE;
            if (mario->dispDir > 90.0f && mario->dispDir < 270.0f) {
                flag = TRUE;
            }
            mario->dispReqDir = flag ? 180.0f : 0.0f;
            mario->currSubMotionId = 100;
            mario->multiTimer = 10;
        }
        break;
    case 100:
        mario->spinSpeed -= 6.0f;
        if (mario->spinSpeed < 0.0f) {
            mario->spinSpeed = 0.0f;
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
        mario->dispDir = revise360(mario->dispDir + mario->spinSpeed);
        flag = FALSE;
        if (mario->dispDir > 90.0f && mario->dispDir < 270.0f) {
            flag = TRUE;
        }
        mario->dispReqDir = flag ? 180.0f : 0.0f;
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
        mario->dispRot.x = 90.0f;
        mario->dispRot.y = 0.0f;
        mario->dispRot.z = 0.0f;
    }
    switch (mario->currSubMotionId) {
    case 0:
        break;
    default:
        return;
    }
    if (!(mario->buttonNew & (1 << 9))) {
        return;
    }
    mario->dispRot.x = 0.0f;
    mario->dispRot.z = 0.0f;
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


void mot_bottomless(void) {
    f32 landY;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 f31;
    f32 f30;
    f32 f29;
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
        if (mario->fallReturnPos.y < -2000.0f) {
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
            f31 = mario->fallReturnPos.x;
            landY = mario->fallReturnPos.y;
            f29 = mario->fallReturnPos.z;
        }
        pos = (Vec){ f31, 300.0f + landY, f29 };
        vec2 = pos;
        while (1) {
            marioGetScreenPos(vec2, &sx, &sy, &sz);
            if (marioChkInScreen((s32)sx, (s32)sy) == 0) {
                break;
            }
            vec2.y += 10.0f;
        }
        mario->position.y = vec2.y;
        mario->bottomlessSpeed = 8.0f;
        mario->multiTimer = (s16)(distABf(f31, f29, mario->position.x, mario->position.z) /
                                  mario->bottomlessSpeed);
        mario->bottomlessDir = angleABf(mario->position.x, mario->position.z, f31, f29);
    }
    switch (mario->currSubMotionId) {
    case 0:
        movePos(&mario->position.x, &mario->position.z, mario->bottomlessSpeed,
                mario->bottomlessDir);
        mario->camFollowPos.x = mario->position.x;
        mario->camFollowPos.z = mario->position.z;
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 2;
            mario->flags |= MARIO_FLAG_STOP_AIR_MOVEMENT;
            mario2 = marioGetPtr();
            p = marioJumpData + 4;
            if (mario2->currMotionId == MARIO_MOTION_JUMP_SW) {
                p = marioJumpSwData + 4;
            }
            mario2->jumpVelY = p[0];
            mario2->jumpAccelY = p[1];
            mario2->jumpJerkY = p[2];
            mario2->jumpJerkStepY = p[3];
            if (mario2->playerGravity != 1.0f) {
                mario2->jumpVelY = 0.34f;
                mario2->jumpAccelY = -0.002f;
                mario2->jumpJerkY = -0.01f;
                mario2->jumpJerkStepY = 0.0f;
            }
        }
        break;
    case 2:
        mario->hitobjStandOn = NULL;
        mario2 = marioGetPtr();
        mario2->jumpJerkY += mario2->jumpJerkStepY;
        if (mario2->jumpJerkY > 0.0f) {
            mario2->jumpJerkY = 0.0f;
        }
        mario2->jumpAccelY += mario2->jumpJerkY;
        mario2->jumpVelY += mario2->jumpAccelY;
        if (mario2->jumpVelY <= marioJumpData[9]) {
            mario2->jumpVelY = marioJumpData[9];
            mario2->jumpAccelY = 0.0f;
            mario2->jumpJerkY = 0.0f;
            mario2->jumpJerkStepY = 0.0f;
        }
        {
            f32 x = mario2->jumpVelY;
            f30 = x;
            if (x < -10.0f) {
                f30 = -10.0f;
            }
        }
        hit = marioChkLandon(&landY, f30);
        if (hit == NULL) {
            mario->position.y += f30;
        } else {
            psndSFXOn_3D((const char*)0xB9, &mario->position);
            marioChgPose("M_D_2");
            psndSFXOn_3D((const char*)0x197, &mario->position);
            marioDmgRetQuakeOn();
            mario->hitobjStandOn = hit;
            mario->hitobjJumpFrom = NULL;
            mario->position.y = landY;
        }
        if (mario->hitobjStandOn == NULL) {
            break;
        }
        // fall through
    case 10:
        mario->multiTimer = 0x32;
        mario->currSubMotionId = 0xB;
        // fall through
    case 11:
        if (--mario->multiTimer > 0) {
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
        if (--mario->multiTimer <= 0) {
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

// TODO: type
MotTblEntry marioMotTbl[41] = {
    { mot_stay, 0x00000000, mot_stay_post },
    { mot_walk, 0x00000000, NULL },
    { mot_dash, 0x00000000, NULL },
    { mot_jump, 0x00000000, NULL },
    { mot_jumpNpc, 0x00000000, NULL },
    { mot_jumpSw, 0x00000000, NULL },
    { mot_jumpStand, 0x00000000, NULL },
    { mot_jump, 0x00000000, NULL },
    { mot_jump, 0x00000000, NULL },
    { mot_jumpSmall, 0x00000000, NULL },
    { mot_fall, 0x00000000, mot_fall_post },
    { mot_fall, 0x00000000, NULL },
    { mot_upstairs, 0x00000000, NULL },
    { mot_land, 0x00000000, NULL },
    { mot_talk, 0x00000000, NULL },
    { mot_getItem, 0x00000000, NULL },
    { mot_hip, 0x00000000, mot_hip_post },
    { mot_hip, 0x00000000, mot_hip_post },
    { mot_hammer, 0x00000000, NULL },
    { mot_hammer2, 0x00000000, mot_ham2_post },
    { mot_jabara, 0x00000000, mot_jabara_post },
    { mot_slit, 0x00000000, mot_slit_post },
    { mot_roll, 0x00000000, mot_roll_post },
    { mot_kaze, 0x00000000, NULL },
    { mot_plane, 0x00000000, mot_plane_post },
    { mot_ship, 0x00000000, mot_ship_post },
    { mot_yoshi, 0x00000000, mot_yoshi_post },
    { mot_cloud, 0x00000000, cloudResetAt },
    { mot_vivian, 0x00000000, mot_vivian_post },
    { mot_dokan, 0x00000000, NULL },
    { mot_grasp, 0x00000000, NULL },
    { mot_damage, 0x00000000, NULL },
    { mot_damageToge, 0x00000000, NULL },
    { mot_bottomless, 0x00000000, NULL },
    { mot_forceReset, 0x00000000, NULL },
    { mot_shadow, 0x00000000, NULL },
    { mot_partyUse, 0x00000000, NULL },
    { kpa_swim, 0x00000000, NULL },
    { kpa_powup, 0x00000000, NULL },
    { kpa_powdown, 0x00000000, NULL },
    { mot_dummy, 0x00000000, NULL },
};
// clang-format on

#undef MARIO_CHG_BLOCK

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
