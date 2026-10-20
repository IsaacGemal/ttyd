#include "mario/mario_hit.h"
#include "party.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "driver/animdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "memory.h"
#include "system.h"
#include "pmario_sound.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "dolphin/types.h"
#include <string.h>
// #include <math.h>  // drags the extern-inline sqrtf/sqrt into .rodata
#include "matching.h"
#include "party/party_yoshi.h"
#include "party/party_move.h"
#include "party/party_hit.h"
#include "motion/mot_damage.h"
#include "mario/mario_cam.h"
#include "motion/mot_jump.h"
#include "eff/n64/eff_sweat_n64.h"
#include "math_ext.h"
#pragma pool_data on // the original addresses its .rodata through one base register

// .sbss
f32 evt_yoshi_spd;
s32 evt_yoshi_frm;
f32 jumpheight;

#define YOSHI_S_2 "PYS_S_2"
#define YOSHI_R_2 "PYS_R_2"
#define YOSHI_H_1 "PYS_H_1"
#define YOSHI_H_2 "PYS_H_2"
#define YOSHI_F_1 "PYS_F_1"

// Local declarations: not yet in a header.

// Sweat/smoke struct stored in PartyEntry::useStruct (0x170) while riding Yoshi.
typedef struct PartyYoshiUse {
    u8 field_00[0x4];
    s32 direction;      // 0x4
    s32 counter;        // 0x8
    f32 anim;           // 0xC
    s32 timer;          // 0x10
    EffectEntry* effect; // 0x14
} PartyYoshiUse;

// Not yet in a header (marioJumpStandData is static in motion/mot_jump.c;
// the pose-speed tables and the M_Y strings live in party_yoshi's .sdata).

typedef struct YoshiZeroVec6 {
    Vec a;
    Vec b;
} YoshiZeroVec6;

// .rodata
const Vec vec3_802cccd0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cccdc = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ccce8 = {0.0f, 0.0f, 0.0f};

// .sdata
static f32 marioJumpStandData[10] = {
    1.52f, -0.188f, 2.925f, -0.847f, -0.1f,
    -0.4f, 0.024f, -0.004f, 0.125f, 0.7f,
};
f32 ofs_PYS_R_2[10] = {
    0.0f, 0.0f, 0.3659999966621399f, 0.5550000071525574f, 0.3799999952316284f,
    0.0f, 0.0f, 0.3659999966621399f, 0.5550000071525574f, 0.3799999952316284f,
};
f32 ofs_PYS_F_1[10] = {
    0.0f, 0.0f, 0.43299999833106995f, 0.5569999814033508f, 0.5120000243186951f,
    0.0f, 0.0f, 0.43299999833106995f, 0.5569999814033508f, 0.5120000243186951f,
};

// .data
f32 ofs_PYS_S_2[52] = {
    0.0f, -0.010999999940395355f, -0.041999999433755875f, -0.08699999749660492f,
    -0.1420000046491623f, -0.20200000703334808f, -0.2619999945163727f, -0.31700000166893005f,
    -0.3619999885559082f, -0.3930000066757202f, -0.40400001406669617f, -0.40400001406669617f,
    -0.40400001406669617f, -0.40400001406669617f, -0.40400001406669617f, -0.3869999945163727f,
    -0.3409999907016754f, -0.2759999930858612f, -0.20200000703334808f, -0.12800000607967377f,
    -0.06300000101327896f, -0.017000000923871994f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, -0.010999999940395355f,
    -0.041999999433755875f, -0.08699999749660492f, -0.1420000046491623f, -0.20200000703334808f,
    -0.2619999945163727f, -0.31700000166893005f, -0.3619999885559082f, -0.3930000066757202f,
    -0.40400001406669617f, -0.40400001406669617f, -0.40400001406669617f, -0.40400001406669617f,
    -0.40400001406669617f, -0.3869999945163727f, -0.3409999907016754f, -0.2759999930858612f,
    -0.20200000703334808f, -0.12800000607967377f, -0.06300000101327896f, -0.017000000923871994f,
    0.0f, 0.0f, 0.0f, 0.0f,
};

// The original's .rodata is these three Vecs (0x00), then the compiler's
// int->float magic double (0x28), then the PYS_* strings (0x30..0x57) and the
// mask double (0x58). Ours matches byte-for-byte; see the yoshi_use note.

// .sdata2
static const f32 float_0 = 0.0f;
static const f32 float_1p5 = 1.5f;
static const f32 float_0p75 = 0.75f;
static const f32 float_10 = 10.0f;
static const f32 float_11 = 11.0f;
static const f32 float_neg0p4 = -0.4f;
static const f32 float_neg15 = -15.0f;
static const f32 float_1 = 1.0f;
static const f32 float_8 = 8.0f;
static const f32 float_neg6 = -6.0f;
static const f32 float_5 = 5.0f;
static const f32 float_30 = 30.0f;
static const f32 float_neg0p07 = -0.07f;
static const f32 float_neg0p5 = -0.5f;
static const f32 float_0p01 = 0.01f;
static const f32 float_256 = 256.0f;
static const f32 float_16 = 16.0f;
static const f32 float_0p1 = 0.1f;
static const f32 float_90 = 90.0f;
static const f32 float_270 = 270.0f;
static const f32 float_180 = 180.0f;
static const f32 float_neg0p01 = -0.01f;
static const f32 float_3p1416 = 3.1415927f;
static const f32 float_0p5 = 0.5f;
static const f32 float_0p37 = 0.37f;
static const f32 float_7 = 7.0f;
static const f32 float_360 = 360.0f;
static const f32 float_18 = 18.0f;
static const f32 float_neg1 = -1.0f;
static const f32 float_2 = 2.0f;
static const f32 float_100 = 100.0f;
static const f32 float_0p7 = 0.7f;
static const f32 float_0p125 = 0.125f;
static const f32 float_neg10 = -10.0f;
static const f32 float_26 = 26.0f;
static const f32 float_20 = 20.0f;


void evt_set_yoshi_frm(s32 msec) {
    evt_yoshi_frm = sysMsec2Frame(msec);
}

void evt_set_yoshi_spd(f32 spd) {
    evt_yoshi_spd = spd;
}

s32 evt_get_yoshi_frm(void) {
    return evt_yoshi_frm;
}

void L_sweatEntry(PartyEntry* party, void* param) {
    party->groundHit = param;

    if (yoshiGetStatus() == 1) {
        MarioWork* mario = party->playerPtr;
        if ((mario->flags & 0xF0000) == 0) {
            mario->hitobjStandOn = param;
        }
    }
}

void L_sweatMain(PartyEntry* party) {
    if (party->useStruct == NULL) {
        return;
    }
    if (((PartyYoshiUse*)party->useStruct)->effect == NULL) {
        return;
    }

    {
        f32 y = party->position.y + float_18;
        f32 z = party->position.z;
        f32 x = party->position.x;

        ((f32*)((PartyYoshiUse*)party->useStruct)->effect->userdata)[1] = x;
        ((f32*)((PartyYoshiUse*)party->useStruct)->effect->userdata)[2] = y;
        ((f32*)((PartyYoshiUse*)party->useStruct)->effect->userdata)[3] = z;
    }

    if (--((PartyYoshiUse*)party->useStruct)->timer <= 0) {
        effDelete(((PartyYoshiUse*)party->useStruct)->effect);
        ((PartyYoshiUse*)party->useStruct)->effect = NULL;
    }
}

s32 yoshiGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 status = 5;

    if (party == NULL || party->currentMemberId != PARTY_MEMBER_YOSHI || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    } else if (party->useMotionId >= 0xA && party->useMotionId <= 0xD) {
        status = 1;
    } else if (party->useMotionId >= 0x14 && party->useMotionId <= 0x16) {
        status = 2;
    } else if (party->useMotionId >= 0x64 && party->useMotionId <= 0x65) {
        status = 5;
    } else if (party->useMotionId >= 0x3C && party->useMotionId <= 0x41) {
        status = 3;
    } else if (party->useMotionId >= 0x32 && party->useMotionId <= 0x34) {
        status = 4;
    }
    return status;
}

void marioRideYoshi(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    party->field_8 &= ~0x8;
    party->field_8 &= ~0x4;
    party->field_8 &= ~0x2;
    party->flags |= PARTY_FLAG_IS_BEING_USED;
    party->field_34 = 3;
    party->useMotionId = 0xB;

    if (party->useStruct == NULL) {
        party->useStruct = __memAlloc(HEAP_DEFAULT, 0x18);
        memset(party->useStruct, 0, 0x18);
        ((PartyYoshiUse*)party->useStruct)->effect = NULL;
    }

    {
        PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
        if (extra != NULL) {
            party_force_ride_yoshi(extra);
        }
    }
}

s32 marioGetoffYoshi(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    if (yoshiGetStatus() == 1) {
        party->useMotionId = 0x64;
        return 1;
    }
    return 0;
}

// 100%: the volatile reads/writes reproduce the original's non-pipelined
// load/store interleave in the two 6-float copies (and the playerPtr load order).
// MUST_MATCH only: the natural form is under #else.
void marioYoshiForceCancel(void) {
    const Vec* vec = &vec3_802cccd0;
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (yoshiGetStatus() != 0) {
#ifdef MUST_MATCH
        MarioWork* pl = *(MarioWork* volatile*)&party->playerPtr;
        Vec* dst = (Vec*)((u8*)pl + 0x98);
        *(volatile Vec*)&dst[0] = *(const volatile Vec*)&vec[0];
        *(volatile Vec*)&dst[1] = *(const volatile Vec*)&vec[1];
#else
        MarioWork* pl = party->playerPtr;
        *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(YoshiZeroVec6*)vec;
#endif

        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
#ifdef MUST_MATCH
                *(volatile Vec*)((u8*)extra + 0x7C) = *(const volatile Vec*)&vec[2];
#else
                extra->dispOffset = vec[2];
#endif
            }
        }
        marioChgPose("M_S_1");
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if (party->useStruct != NULL) {
                __memFree(HEAP_DEFAULT, party->useStruct);
                party->useStruct = NULL;
            }
            if (mario->currMotionId == MARIO_MOTION_DAMAGE_TOGE) {
                partyChgRunMode(party, 0xD);
                {
                    PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                    if (extra != NULL) {
                        partyChgRunMode(extra, 0xD);
                    }
                }
            } else {
                goto not_toge;
            }
        } else {
        not_toge:
            if (party->field_34 != 8 && party->field_34 != 0xD) {
                partyChgRunMode(party, 2);
                party->field_3B = 0;
            }
        }
    }
}

void yoshi_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (yoshiGetStatus() == 1) {
            party->useMotionId = 0x64;
        }
        yoshi_use(party);
    } else {
        if (partyGoodbyeMain(party)) {
            partyChgRunMode(party, 0xE);
        }
    }
}

// 100%: same volatile-copy MUST_MATCH trick as marioYoshiForceCancel; the
// natural form is under #else.
void yoshi_exit(PartyEntry* arg) {
    const Vec* vec = &vec3_802cccd0;
    MarioWork* mario;
    PartyEntry* party;

    arg->playerPtr->flags &= ~MARIO_FLAG_CHG_MOT_DISABLED;
    mario = marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (yoshiGetStatus() != 0) {
#ifdef MUST_MATCH
        MarioWork* pl = *(MarioWork* volatile*)&party->playerPtr;
        Vec* dst = (Vec*)((u8*)pl + 0x98);
        *(volatile Vec*)&dst[0] = *(const volatile Vec*)&vec[0];
        *(volatile Vec*)&dst[1] = *(const volatile Vec*)&vec[1];
#else
        MarioWork* pl = party->playerPtr;
        *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(YoshiZeroVec6*)vec;
#endif

        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
#ifdef MUST_MATCH
                *(volatile Vec*)((u8*)extra + 0x7C) = *(const volatile Vec*)&vec[2];
#else
                extra->dispOffset = vec[2];
#endif
            }
        }
        marioChgPose("M_S_1");
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if (party->useStruct != NULL) {
                __memFree(HEAP_DEFAULT, party->useStruct);
                party->useStruct = NULL;
            }
            if (mario->currMotionId == MARIO_MOTION_DAMAGE_TOGE) {
                partyChgRunMode(party, 0xD);
                {
                    PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                    if (extra != NULL) {
                        partyChgRunMode(extra, 0xD);
                    }
                }
            } else {
                goto exit_not_toge;
            }
        } else {
        exit_not_toge:
            if (party->field_34 != 8 && party->field_34 != 0xD) {
                partyChgRunMode(party, 2);
                party->field_3B = 0;
            }
        }
    }
    marioChgMot(0);
}

// The three early-exit tests must share one `||` (as in yoshiGetStatus): a
// flat chain keeps them in one condition and makes the flag test emit the
// original's `bne body; b end` instead of `beq end`.
void yoshi_jumpStand(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL || party->currentMemberId != PARTY_MEMBER_YOSHI || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    if (party->useMotionId < 0x3C || party->useMotionId > 0x42) {
        party->useMotionId = 0x3C;
    }
}

void unk_800b474c(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();

    party->groundHit = NULL;
    if (yoshiGetStatus() == 1) {
        MarioWork* pl = party->playerPtr;
        if ((pl->flags & 0xF0000) == 0) {
            pl->hitobjStandOn = NULL;
        }
    }
    marioChgMot(0x20);
    party->useMotionId = 0xA;
    partyChgPose(party, YOSHI_S_2);
    marioChgPose("M_Y_2");
    mario->field_0x2B0 = 0.0f;
    *(f32*)((u8*)mario + 0x2B4) = 0.0f;
    partyChgRunMode(party, 0xD);
    {
        PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
        if (extra != NULL) {
            partyChgRunMode(extra, 0xD);
        }
    }
}

// 100%: the compiler pools this function's literals under the vec3_802cccd0
// anchor with a shared base register (r31), so the string and mask offsets
// are the original's.  The original .rodata is vec3 (0x00) / double_to_int
// (0x28) / PYS_S_2 (0x30) / PYS_H_1 (0x38) / PYS_R_2 (0x40) / PYS_H_2 (0x48)
// / PYS_F_1 (0x50) / mask (0x58).
void unk_800b45e0(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (strcmp(party->field_18, YOSHI_S_2) == 0) {
        s32 t = *(u16*)&mario->poseTime + 1;
        *(u16*)&mario->poseTime = t;
        if ((u16)t >= 0x34) {
            *(u16*)&mario->poseTime = 0;
        }
        party->field_20 = *(u16*)&mario->poseTime;
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)&mario->poseTime);
    } else if (strcmp(party->field_18, YOSHI_R_2) == 0) {
        s32 t = *(u16*)&mario->poseTime + 1;
        *(u16*)&mario->poseTime = t;
        if ((u16)t >= 0xA) {
            *(u16*)&mario->poseTime = 0;
        }
        party->field_20 = *(u16*)&mario->poseTime;
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)&mario->poseTime);
    } else if (strcmp(party->field_18, YOSHI_F_1) == 0) {
        s32 t = *(u16*)&mario->poseTime + 1;
        *(u16*)&mario->poseTime = t;
        if ((u16)t >= 0xA) {
            *(u16*)&mario->poseTime = 0;
        }
        party->field_20 = *(u16*)&mario->poseTime;
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)&mario->poseTime);
    }
}

void yoshi_init(PartyEntry* party) {
    party->field_33 = 4;
    party->field_32 = 0;
    party->height = float_26;
    party->radius = float_20;
    party->yoshiPsndSFXId = -1;
}
#define P_P(off) (*(void**)((u8*)party + (off)))
#define M_F(off) (*(f32*)((u8*)mario + (off)))
#define M_S(off) (*(s32*)((u8*)mario + (off)))
#define M_P(off) (*(void**)((u8*)mario + (off)))


// Rider height offset for Yoshi's current walk/run/flutter pose frame.
static inline f32 yoshiRideHeight(PartyEntry* party) {
    MarioWork* pm = party->playerPtr;
    f32 ofs = 0.0f;
    if (strcmp(party->field_18, YOSHI_S_2) == 0) {
        ofs = 2.0f * ofs_PYS_S_2[*(u16*)&pm->poseTime] + ofs;
    } else if (strcmp(party->field_18, YOSHI_R_2) == 0) {
        ofs = 2.0f * ofs_PYS_R_2[*(u16*)&pm->poseTime] + ofs;
    } else if (strcmp(party->field_18, YOSHI_F_1) == 0) {
        ofs = 2.0f * ofs_PYS_F_1[*(u16*)&pm->poseTime] + ofs;
    }
    return ofs;
}

// Records the ceiling above Yoshi's head in the rider's work.
static inline void yoshiSearchHead(PartyEntry* party) {
    MarioWork* pl = party->playerPtr;
    Vec pos = party->position;
    f32 h = partyGetHeight(party);
    pl->hitobjHead = marioSearchHead(&pos, &h, 10.0f);
}

#define VEC_S2 YOSHI_S_2

void yoshi_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    PartyEntry* extra;
    PartyYoshiUse* use;
    HitEntry* hit;
    f32 outAngle;
    f32 outDist;
    f32 f30;
    f32 f31;
    f32 fx;
    f32 fz;

    if (mario->keyOffCount == 0) {
        partyUpdateKeyData(party);
    } else {
        L_partyClearCont(party);
    }

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        party->flags &= ~0x70;
        party->flags &= ~0x03000000;
        party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        party->useStruct = __memAlloc(HEAP_DEFAULT, 0x18);
        memset(party->useStruct, 0, 0x18);
        ((PartyYoshiUse*)party->useStruct)->effect = NULL;
        if (party->flags & PARTY_FLAG_IS_EVT_USE) {
            evt_yoshi_spd = 1.0f;
        } else {
            evt_yoshi_frm = -1;
            evt_yoshi_spd = 0.0f;
        }
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
            party->useMotionId = 0;
        } else {
            party->useMotionId = 4;
        }
        party->timer = 3;
        mario->baseSpeed = 0.0f;
        mario->field_0x2A4 = 0.0f;
        M_F(0x2A0) = 0.0f;
        mario->field_0x2B0 = 0.0f;
        M_F(0x2B4) = 0.0f;
        ((PartyYoshiUse*)party->useStruct)->direction = 0;
    }

    switch (party->useMotionId) {
    case 0:
        if (--party->timer > 0) {
            if (marioKeyOffChk() == 0 && mario->hitobjStandOn != NULL) {
                return;
            }
            if (party->flags & PARTY_FLAG_IS_BEING_USED) {
                party->flags &= ~PARTY_FLAG_IS_BEING_USED;
                if (party->useStruct != NULL) {
                    __memFree(HEAP_DEFAULT, party->useStruct);
                    party->useStruct = NULL;
                }
            }
            if (party->field_34 != 8 && party->field_34 != 0xD) {
                partyChgRunMode(party, 2);
                party->field_3B = 0;
            }
            return;
        }
        party->useMotionId = 4;
        /* fallthrough */
    case 4:
        {
            f32 a;
            if (mario->directionView <= 180.0f) {
                a = 90.0f;
            } else {
                a = 270.0f;
            }
            mario->directionView = a;
        }
        mario->moveDir = mario->directionView;
        party->targetPos = mario->position;
        party->timer = 4;
        party->jumpPara1 = 0.0f;
        party->jumpPara3 = 0.0f;
        party->tail.yoshi.jumpVel = 0.0f;
        party->tail.yoshi.jumpAccel = 0.0f;
        party->jumpPara2 = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->speed = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z) / (f32)party->timer;
        party->useMotionId = 5;
        /* fallthrough */
    case 5:
        party->targetPos = mario->position;
        party->position.x += (party->targetPos.x - party->position.x) / (f32)party->timer;
        party->position.y += (party->targetPos.y - party->position.y) / (f32)party->timer;
        party->position.z += (party->targetPos.z - party->position.z) / (f32)party->timer;
        if (--party->timer != 0) {
            searchGround(party);
            if (party->groundHit == NULL) {
                return;
            }
            party->position.x += party->moveDelta.x;
            party->position.y += party->moveDelta.y;
            party->position.z += party->moveDelta.z;
            return;
        }
        mario->trigFlags |= 0x20000;
        marioChgSmallJumpMotion();
        mario->baseSpeed = 0.0f;
        party->field_FC = mario->moveDir;
        party->dir = mario->directionView;
        party->field_110 = mario->dispReqDir;
        party->field_10C = mario->unk1A8;
        party->speed = 0.0f;
        party->useMotionId = 7;
        if (party->groundHit != NULL) {
            party->position.x += party->moveDelta.x;
            party->position.y += party->moveDelta.y;
            party->position.z += party->moveDelta.z;
        }
        partyChkGnd(party);
        return;
    case 7:
        if (party->flags & PARTY_FLAG_IS_EVT_USE) goto L_30B0;
        if (marioKeyOffChk() != 0) {
            party->useMotionId = 0x65;
            goto L_312C;
        }
        if (party->flags & PARTY_FLAG_IS_GROUNDED) goto L_30B0;
        party->useMotionId = 0x65;
        goto L_312C;
    L_30B0:
        if (mario->hitobjStandOn == NULL) {
            if (party->groundHit != NULL) {
                party->position.x += party->moveDelta.x;
                party->position.y += party->moveDelta.y;
                party->position.z += party->moveDelta.z;
            }
            partyChkGnd(party);
            return;
        }
        party->useMotionId = 0xA;
        extra = partyGetPtr(marioGetExtraPartyId());
        if (extra != NULL) {
            partyChgRunMode(extra, 0xC);
        }
        partyChkGnd(party);
        break;
    default:
        break;
    }

L_312C:
    switch (party->useMotionId) {
    case 0xA:
        psndSFXOn_3D((const char*)0x93F, &party->position);
        party->flags &= ~PARTY_FLAG_FORCE_FALL;
        /* fallthrough */
    case 0xB:
        marioChgMot(0x1A);
        partyChgPose(party, YOSHI_H_1);
        marioChgPose("M_Y_1");
        party->speed = 0.0f;
        party->targetDist = 0.0f;
        party->position.x = mario->position.x;
        party->position.y = mario->position.y;
        party->position.z = mario->position.z;
        mario->field_EC.x = party->position.x;
        mario->field_EC.z = party->position.z;
        party->useMotionId = 0xC;
        mario->field_0x11C = party->position;
        goto L_3588;
    case 0xC:
        party->playerPtr->poseTime = 0;
        party->useMotionId = 0xD;
        /* fallthrough */
    case 0xD:
        if (!(mario->flags & 0x20)) goto L_3354;
        marioChgPose("M_Y_2");
        partyChgPose(party, YOSHI_R_2);
        party->field_12C = NULL;
        party->field_134 = NULL;
        party->headHit = NULL;
        M_P(0x1E0) = NULL;
        M_P(0x1EC) = NULL;
        M_P(0x1F0) = NULL;
        M_P(0x1F4) = NULL;
        f30 = mario->directionView;
        f31 = mario->baseSpeed;
        yoshiSearchHead(party);
        checkWall(party, f31, f30);
        mario->field_EC.x = party->position.x;
        mario->field_EC.z = party->position.z;
        mario->position.x = party->position.x;
        mario->position.z = party->position.z;
        mario->unk1A8 = party->field_10C;
        mario->dispReqDir = party->field_110;
        party_ride_yoshi_force_move();
        mario->field_0x11C = party->position;
        searchGround(party);
        if (--mario->forceMoveTimer <= 0) {
            mario->forceMoveTimer = 0;
            mario->flags &= ~0x20;
        }
        use = party->useStruct;
        use->counter++;
        if (((PartyYoshiUse*)party->useStruct)->counter % 10 == 0) {
            psndSFXOn_3D((const char*)0x940, &party->position);
        }
        goto L_3588;
    L_3354:
        if (party->field_34 == 8) goto L_3588;
        if (yoshiGetStatus() == 1) {
            party->moveDelta.x = mario->riderDelta.x;
            party->moveDelta.y = mario->riderDelta.y;
            party->moveDelta.z = mario->riderDelta.z;
        }
        if (party->flags & PARTY_FLAG_IS_GROUNDED) {
            if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
                if (party->buttonNew & 0x100) {
                    party->useMotionId = 0x14;
                    goto L_3588;
                }
            } else {
                if (evt_yoshi_frm >= 0) {
                    party->useMotionId = 0x14;
                    goto L_3588;
                }
            }
        }
        if (party->flags & PARTY_FLAG_IS_EVT_USE) {
            outAngle = mario->directionView;
            outDist = evt_yoshi_spd;
        } else {
            L_getStick(party, &outAngle, &outDist);
        }
        if (outDist > 0.0f) {
            party->speed = outDist;
            party->dir = outAngle;
            party->field_FC = outAngle;
            marioChgPose("M_Y_2");
            partyChgPose(party, YOSHI_R_2);
            use = party->useStruct;
            use->counter++;
            if (((PartyYoshiUse*)party->useStruct)->counter % 10 == 0) {
                psndSFXOn_3D((const char*)0x940, &party->position);
            }
        } else {
            if (strcmp(party->field_18, VEC_S2) != 0) {
                mario->poseTime = 0;
            }
            partyChgPose(party, YOSHI_S_2);
            party->speed = 0.0f;
            ((PartyYoshiUse*)party->useStruct)->counter = 0;
        }
        inertia(party);
        party->position.x = mario->position.x;
        party->position.z = mario->position.z;
        party->field_12C = NULL;
        party->field_134 = NULL;
        M_P(0x1E0) = NULL;
        M_P(0x1EC) = NULL;
        M_P(0x1F0) = NULL;
        f30 = party->dir;
        f31 = party->speed;
        yoshiSearchHead(party);
        checkWall(party, f31, f30);
        mario->field_EC.x = party->position.x;
        mario->field_EC.z = party->position.z;
        mario->position.x = party->position.x;
        mario->position.z = party->position.z;
        mario->unk1A8 = party->field_10C;
        mario->dispReqDir = party->field_110;
        if (party->buttonNew & 0x600) {
            party->useMotionId = 0x64;
        }
        searchGround(party);
        M_F(0x18C) = f31;
        goto L_3588;
    default:
        goto L_3588;
    }

L_3588:
    switch (party->useMotionId) {
    case 0x64:
        {
#ifdef MUST_MATCH
            // Volatile, as in marioYoshiForceCancel: keeps the playerPtr load ahead of the copies.
            MarioWork* pl64 = *(MarioWork* volatile*)&party->playerPtr;
            *(volatile Vec*)((u8*)pl64 + 0x98) = *(const volatile Vec*)&vec3_802cccd0;
            *(volatile Vec*)((u8*)pl64 + 0xA4) = *(const volatile Vec*)&vec3_802cccdc;
#else
            MarioWork* pl64 = party->playerPtr;
            *(YoshiZeroVec6*)((u8*)pl64 + 0x98) = *(const YoshiZeroVec6*)&vec3_802cccd0;
#endif
        }
        extra = partyGetPtr(marioGetExtraPartyId());
        if (extra != NULL) {
#ifdef MUST_MATCH
            *(volatile Vec*)((u8*)extra + 0x7C) = *(const volatile Vec*)&vec3_802ccce8;
#else
            extra->dispOffset = vec3_802ccce8;
#endif
        }
        marioChgSmallJumpMotion();
        mario->baseSpeed = 0.0f;
        partyChgPose(party, YOSHI_H_2);
        party->speed = 0.0f;
        party->useMotionId = 0x65;
        if (party->groundHit != NULL) {
            party->position.x += party->moveDelta.x;
            party->position.y += party->moveDelta.y;
            party->position.z += party->moveDelta.z;
        }
        partyChkGnd(party);
        return;
    case 0x65:
        if (mario->hitobjStandOn == NULL) {
            if (party->groundHit != NULL) {
                party->position.x += party->moveDelta.x;
                party->position.y += party->moveDelta.y;
                party->position.z += party->moveDelta.z;
            }
            partyChkGnd(party);
            return;
        }
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        if (party->field_34 != 8) {
            partyChgRunMode(party, 2);
            party->field_3B = 0;
        }
        if (party->groundHit != NULL) {
            party->position.x += party->moveDelta.x;
            party->position.y += party->moveDelta.y;
            party->position.z += party->moveDelta.z;
        }
        partyChkGnd(party);
        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }
        return;
    default:
        goto L_3754;
    }

L_3754:
    switch (party->useMotionId) {
    case 0x28:
        party->flags |= PARTY_FLAG_IS_STEPPING;
        party->jumpPara1 = 0.0f;
        party->jumpPara2 = 0.0f;
        party->jumpPara3 = 0.0f;
        party->timer = 3;
        party->jumpPara1 = (party->field_120 - party->position.y) / (f32)party->timer;
        party->useMotionId = 0x29;
        /* fallthrough */
    case 0x29:
        party->position.y += party->jumpPara1;
        if (--party->timer <= 0) {
            party->flags &= ~PARTY_FLAG_IS_STEPPING;
            party->useMotionId = 0xD;
        }
        break;
    default:
        break;
    }

L_3800:
    switch (party->useMotionId) {
    default:
        goto L_3CE4;
    case 0x14:
        ((PartyYoshiUse*)party->useStruct)->direction =
            (mario->unk1A8 >= 90.0f && mario->unk1A8 <= 270.0f) ? 1 : -1;
        mario->hitobjJumpFrom = mario->hitobjStandOn;
        mario->hitobjStandOn = NULL;
        mario->field_0x11C = party->position;
        partyChgPose(party, YOSHI_F_1);
        marioChgPose("M_Y_3");
        mario->airTimer = 0;
        party->useMotionId = 0x15;
        party->speed += -0.01f;
        if (party->speed <= 0.0f) {
            party->speed = 0.0f;
        }
        ((PartyYoshiUse*)party->useStruct)->anim = 1.0f;
        M_F(0x2A0) = (f32)sin(3.1415927f * ((PartyYoshiUse*)party->useStruct)->anim / 180.0f);
        mario->field_0x2A4 = 0.0f;
        M_F(0x2A8) = party->position.y;
        party->yoshiPsndSFXId = psndSFXOn_3D((const char*)0x941, &party->position);
        /* fallthrough */
    case 0x15:
        f30 = (f32)sin(3.1415927f * ((PartyYoshiUse*)party->useStruct)->anim / 180.0f);
        {
            f32 mult;
            if (((PartyYoshiUse*)party->useStruct)->anim <= 180.0f) {
                mult = 0.5f;
            } else {
                mult = 0.37f;
            }
            M_F(0x2A0) = f30 * mult;
        }
        mario->field_0x2A4 = mario->field_0x2A4 + M_F(0x2A0);
        ((PartyYoshiUse*)party->useStruct)->anim += 7.0f;
        if (((PartyYoshiUse*)party->useStruct)->anim >= 360.0f) {
            ((PartyYoshiUse*)party->useStruct)->anim = 360.0f;
            party->useMotionId = 0x16;
        }
        /* fallthrough */
    case 0x16:
        {
            f32 px = party->position.x;
            f32 py = party->position.y + 18.0f;
            f32 pz = party->position.z;
            if (((PartyYoshiUse*)party->useStruct)->effect == NULL) {
                f32 c = (party->field_110 == 180.0f) ? -1.0f : 1.0f;
                ((PartyYoshiUse*)party->useStruct)->effect =
                    effSweatN64Entry(1, px, py, pz, 1.0f, 70.0f * c, 0x16);
                ((PartyYoshiUse*)party->useStruct)->timer = 0x14;
            }
        }
        hit = partySearchGround(party, 11.0f, 11.0f);
        if (hit == NULL) {
            party->position.y = M_F(0x2A8) + mario->field_0x2A4;
        } else {
            s32 attr = hitGetAttr(hit);
            if (attr & 0x200) {
                if (party->position.y <= party->groundY) {
                    party->useMotionId = 0x46;
                }
            } else if (attr & 0xC00) {
                if (party->position.y <= party->groundY) {
                    unk_800b474c(party);
                    return;
                }
            }
        L_3AB0:
            L_sweatEntry(party, NULL);
            M_F(0x2A8) = party->groundY;
            party->position.y = party->groundY + mario->field_0x2A4;
        }
    L_3AD4:
        party->position.x = mario->position.x;
        party->position.z = mario->position.z;
        if (party->flags & PARTY_FLAG_IS_EVT_USE) {
            outAngle = mario->directionView;
            outDist = evt_yoshi_spd;
            party->speed = evt_yoshi_spd;
        } else {
            L_getStick(party, &outAngle, &outDist);
            L_yoshiFlyMove(party);
        }
    L_3B20:
        party->field_12C = NULL;
        party->field_134 = NULL;
        f30 = party->dir;
        f31 = party->speed;
        yoshiSearchHead(party);
        checkWall(party, f31, f30);
        if (party->groundHit == NULL && mario->airTimer >= 0xA) {
            searchGround2(party);
            if (party->groundHit != NULL) {
                mario->hitobjJumpFrom = NULL;
                if ((u32)party->yoshiPsndSFXId != 0xFFFFFFFFu) {
                    psndSFXOff(party->yoshiPsndSFXId);
                    party->yoshiPsndSFXId = -1;
                }
                party->useMotionId = 0xA;
                partyChgPose(party, VEC_S2);
                marioChgPose("M_Y_2");
                mario->field_0x2B0 = 0.0f;
                M_F(0x2B4) = 0.0f;
                if (party->flags & PARTY_FLAG_IS_EVT_USE) {
                    evt_yoshi_spd = 0.0f;
                    evt_yoshi_frm = -1;
                    party->speed = 0.0f;
                    party->useMotionId = 0x34;
                }
                goto L_3CE4;
            }
            ((PartyYoshiUse*)party->useStruct)->direction = 0;
        }
    L_3C2C:
        mario->field_EC.x = party->position.x;
        mario->field_EC.z = party->position.z;
        mario->position.x = party->position.x;
        mario->position.z = party->position.z;
        mario->unk1A8 = party->field_10C;
        mario->dispReqDir = party->field_110;
        mario->airTimer++;
        if (mario->airTimer > 0xE) {
            if (party->flags & PARTY_FLAG_IS_EVT_USE) {
                if (mario->airTimer > 0x44) {
                    evt_yoshi_frm = 0;
                    party->jumpPara1 = 0.0f;
                    party->jumpPara3 = -1.0f;
                    party->useMotionId = 0x32;
                }
                goto L_3CE4;
            }
            if (mario->airTimer <= 0x44 && (party->button & 0x100) && marioKeyOffChk() == 0) {
                goto L_3CE4;
            }
            party->jumpPara1 = 0.0f;
            party->jumpPara3 = -1.0f;
            party->useMotionId = 0x32;
        }

    L_3CE4:
        unk_800b45e0(party);
        if (party->useMotionId == 0xD || yoshiGetStatus() == 3 || yoshiGetStatus() == 4 || yoshiGetStatus() == 2) {
            fx = 0.0f;
            fz = 0.0f;
            movePos(&fx, &fz, -6.0f, toMovedir(party->field_10C));
            M_F(0x98) = fx;
            M_F(0xA0) = fz;
            M_F(0xA8) = 8.0f + yoshiRideHeight(party);
            extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                extra->dispOffset.y = yoshiRideHeight(party);
            }
        }

    L_3EA4:
        switch (party->useMotionId) {
        case 0x3C:
            party->speed = 0.0f;
            party->flags &= ~PARTY_FLAG_IS_FALLING;
            party->flags |= PARTY_FLAG_IS_JUMPING;
            mario->airTimer = 0;
            mario->airTimerPeak = 0;
            P_P(0x13C) = party->groundHit;
            L_sweatEntry(party, NULL);
            mario->hitobjStandOn = NULL;
            mario->field_0x11C = mario->position;
            jumpheight = 100.0f;
            mario->field_0x128 = 0.0f;
            party->useMotionId = 0x3D;
            party->timer = 0;
            party->jumpPara3 = marioJumpStandData[0];
            party->tail.yoshi.jumpVel = marioJumpStandData[1];
            party->tail.yoshi.jumpAccel = marioJumpStandData[2];
            party->jumpPara2 = marioJumpStandData[3];
            /* fallthrough */
        case 0x3D:
            {
                f32 old = party->jumpPara3;
                party->tail.yoshi.jumpAccel = party->tail.yoshi.jumpAccel + party->jumpPara2;
                party->tail.yoshi.jumpVel = party->tail.yoshi.jumpVel + party->tail.yoshi.jumpAccel;
                party->jumpPara3 = party->jumpPara3 + party->tail.yoshi.jumpVel;
                if (old >= party->jumpPara3) {
                    party->useMotionId = 0x3E;
                }
            }
            party->position.y += party->jumpPara3;
            mario->field_0x128 += party->jumpPara3;
            goto L_4188;
        case 0x3E:
            party->position.y += party->jumpPara3;
            mario->field_0x128 += party->jumpPara3;
            if (mario->field_0x128 >= 0.7f * marioGetJumpHeight()) {
                party->useMotionId = 0x3F;
            }
            goto L_4188;
        case 0x3F:
            {
                f32 v = party->jumpPara3;
                party->jumpPara3 += 0.125f * (-1.0f - v);
            }
            party->position.y += party->jumpPara3;
            mario->field_0x128 += party->jumpPara3;
            if (party->jumpPara3 <= 0.0f) {
                party->useMotionId = 0x40;
                marioSetPeakYpos();
                party->jumpPara3 = -0.4f;
                party->tail.yoshi.jumpVel = -0.4f;
                party->tail.yoshi.jumpAccel = 0.0f;
                party->jumpPara2 = 0.0f;
                party->flags &= ~PARTY_FLAG_IS_JUMPING;
                party->flags |= PARTY_FLAG_IS_FALLING;
                party->useMotionId = 0x40;
            }
            goto L_4188;
        case 0x40:
            party->tail.yoshi.jumpAccel = party->tail.yoshi.jumpAccel + party->jumpPara2;
            party->tail.yoshi.jumpVel = party->tail.yoshi.jumpVel + party->tail.yoshi.jumpAccel;
            party->jumpPara3 = party->jumpPara3 + party->tail.yoshi.jumpVel;
            if (party->jumpPara3 <= 0.0f) {
                marioSetPeakYpos();
                party->jumpPara3 = -0.4f;
                party->tail.yoshi.jumpVel = -0.4f;
                party->tail.yoshi.jumpAccel = 0.0f;
                party->jumpPara2 = 0.0f;
                party->flags &= ~PARTY_FLAG_IS_JUMPING;
                party->flags |= PARTY_FLAG_IS_FALLING;
                party->useMotionId = 0x41;
            }
            party->position.y += party->jumpPara3;
            goto L_4188;
        case 0x41:
            if (--party->timer <= 0) {
                party->speed = 0.0f;
                party->jumpPara1 = 0.0f;
                party->jumpPara3 = -1.0f;
                party->useMotionId = 0x32;
            }
            goto L_4188;
        default:
            goto L_4188;
        }

    L_4188:
        if (party->useMotionId >= 0x40 && party->useMotionId <= 0x41) {
            mario->airTimer++;
            searchGround(party);
            if (party->groundHit != NULL) {
                mario->hitobjJumpFrom = NULL;
                party->jumpPara1 = 0.0f;
                party->jumpPara3 = -1.0f;
                party->useMotionId = 0x32;
            }
        }
        switch (party->useMotionId) {
        case 0x32:
        case 0x33:
            party->position.x = mario->position.x;
            party->position.z = mario->position.z;
            if (party->flags & PARTY_FLAG_IS_EVT_USE) {
                outAngle = mario->directionView;
                outDist = evt_yoshi_spd;
                party->speed = evt_yoshi_spd;
            } else {
                L_getStick(party, &outAngle, &outDist);
                L_yoshiFlyMove(party);
            }
            if (!(party->button & 0x100) || marioKeyOffChk() != 0) {
                if (party->useMotionId == 0x32) {
                    party->useMotionId = 0x33;
                }
                {
                    MarioWork* m = marioGetPtr();
                    m->spinSpeed = m->spinSpeed + m->field_0x2A4;
                }
            }
            if (M_F(0x2A0) <= -10.0f) {
                M_F(0x2A0) = -10.0f;
            }
            party->field_12C = NULL;
            party->field_134 = NULL;
            M_P(0x1E0) = NULL;
            M_P(0x1EC) = NULL;
            M_P(0x1F0) = NULL;
            f30 = party->dir;
            f31 = party->speed;
            yoshiSearchHead(party);
            checkWall(party, f31, f30);
            searchGround(party);
            if (party->groundHit != NULL) {
                mario->hitobjJumpFrom = NULL;
                party->useMotionId = 0xD;
                partyChgPose(party, VEC_S2);
                marioChgPose("M_Y_2");
                mario->field_0x2B0 = 0.0f;
                M_F(0x2B4) = 0.0f;
                ((PartyYoshiUse*)party->useStruct)->direction = 0;
                if (party->flags & PARTY_FLAG_IS_EVT_USE) {
                    evt_yoshi_spd = 0.0f;
                    evt_yoshi_frm = -1;
                    party->speed = 0.0f;
                    party->useMotionId = 0x34;
                }
            }
            break;
        case 0x34:
            if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
                party->useMotionId = 0xD;
            }
            break;
        default:
            break;
        }

    L_4394:
        L_sweatMain(party);
        if (party->useMotionId == 0x46) {
            if (marioChkDamageSink() != 0) {
                party->position = party->playerPtr->position;
                return;
            }
            party->useMotionId = 0x47;
            return;
        }
        if (party->useMotionId == 0x47) {
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if (party->useStruct != NULL) {
                __memFree(HEAP_DEFAULT, party->useStruct);
                party->useStruct = NULL;
            }
            marioSetFallPara();
            marioChgMot(0x1F);
            partyChgRunMode(party, 0xD);
            extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                partyChgRunMode(extra, 0xD);
            }
            return;
        }
        if (party->useMotionId >= 0xA && party->useMotionId <= 0xD && mario->currMotionId != 0x1A) {
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if (party->useStruct != NULL) {
                __memFree(HEAP_DEFAULT, party->useStruct);
                party->useStruct = NULL;
            }
            partyChgRunMode(party, 2);
            party->field_3B = 0;
            {
#ifdef MUST_MATCH
                // Volatile, as in marioYoshiForceCancel: keeps the playerPtr load ahead of the copies.
                MarioWork* pl = *(MarioWork* volatile*)&party->playerPtr;
                *(volatile Vec*)((u8*)pl + 0x98) = *(const volatile Vec*)&vec3_802cccd0;
                *(volatile Vec*)((u8*)pl + 0xA4) = *(const volatile Vec*)&vec3_802cccdc;
#else
                MarioWork* pl = party->playerPtr;
                *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(const YoshiZeroVec6*)&vec3_802cccd0;
#endif
            }
            extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
#ifdef MUST_MATCH
                *(volatile Vec*)((u8*)extra + 0x7C) = *(const volatile Vec*)&vec3_802ccce8;
#else
                extra->dispOffset = vec3_802ccce8;
#endif
            }
            return;
        }
        if (party->groundHit != NULL) {
            party->position.x += party->moveDelta.x;
            party->position.y += party->moveDelta.y;
            party->position.z += party->moveDelta.z;
        }
        mario->field_EC.x = party->position.x;
        mario->field_EC.z = party->position.z;
        mario->unk1A8 = party->field_10C;
        mario->dispReqDir = party->field_110;
        mario->position = party->position;
        marioUpdateCamCollision(party->position);
    }
}

#undef VEC_S2
#undef P_P
#undef M_F
#undef M_S
#undef M_P


void L_yoshiFlyMove(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();
    f32 f31;
    f32 f30;
    f32 f29;
    f32 lim;
    f32 one = 1.0f;
    f32 zero = 0.0f;

    inertia(party);
    f30 = mario->field_0x2B0;
    f31 = -*(f32*)((u8*)mario + 0x2B4);

    f29 = float_0p1 * distABf(0.0f, 0.0f, f30, f31);
    lim = mario->mBaseDashSpeed * one;
    if (f29 >= lim) {
        f29 = lim;
    }

    f31 = angleABf(0.0f, 0.0f, f30, f31);
    if (f29 < float_0p01) {
        f29 = 0.0f;
    }

    party->speed = f29;
    if (f29 != zero) {
        party->dir = revise360(f31 + party->playerTargetYaw);
        party->field_FC = revise360(f31 + party->playerTargetYaw);
    }
    *(f32*)((u8*)mario + 0x18C) = f29;
}

// The `zero` local (rather than a literal 0.0f) stops MWCC folding
// `f5 - 0.0f` away and lets it reuse the float_0 load held in f2 from the
// `f3 == zero` test, matching the original's else-branch fsubs; it also
// keeps the negated-16 temp from constant-folding across the comparison.
void inertia(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 f3 = (f32)party->stickX;
    f32 f4 = mario->field_0x2B0;
    f32 f5 = *(f32*)((u8*)mario + 0x2B4);
    f32 zero = 0.0f;
    f32 p01 = 0.01f;
    f32 sixteen = 16.0f;

    if (f3 == zero) {
        f32 k = float_neg0p07;
        if (mario->hitobjWall != NULL) {
            k = float_neg0p5;
        }
        f4 = f4 * k + f4;
        f5 = f5 * k + f5;
        if (__fabsf(f4) < p01) {
            f4 = 0.0f;
        }
        if (__fabsf(f5) < p01) {
            f5 = 0.0f;
        }
    } else {
        f32 t = f4 * f4 + f5 * f5;

        if (t <= float_256) {
            s32 v = *(s32*)((u8*)party->useStruct + 4);
            f4 = f3 * p01 + f4;
            if (v > 0 && f4 <= 0.0f) {
                f4 = 0.0f;
            }
            if (v < 0 && f4 >= 0.0f) {
                f4 = 0.0f;
            }
            f5 = f5 - zero;
        } else {
            f5 = f5 - zero;
            f4 = f3 * p01 + f4;
            {
                f32 n = -sixteen;
                if (f4 < n) {
                    f4 = n;
                }
                if (f4 > sixteen) {
                    f4 = sixteen;
                }
                if (f5 < n) {
                    f5 = n;
                }
                if (f5 > sixteen) {
                    f5 = sixteen;
                }
            }
        }
    }
    mario->field_0x2B0 = f4;
    *(f32*)((u8*)mario + 0x2B4) = f5;
}

// 100%: the partyChgPose "PYS_S_2" string is pooled at vec3+0x30, so it is
// reached through the vec3 base register (r31) rather than a fresh lis; the
// 6-float zero copy is emitted as two Vec copies.
void searchGround(PartyEntry* party) {
    const Vec* vec = &vec3_802cccd0;
    HitEntry* hit = NULL;
    MarioWork* mario = party->playerPtr;
    s32 attr;
    f32 f31;

    if (!(party->flags & PARTY_FLAG_FORCE_FALL)) {
        f32 v = __fabsf(party->jumpPara3);

        if (v < 1.0f) {
            v = 1.0f;
        }
        hit = partySearchGround(party, v, v);
        f31 = party->groundY;
    }

    if (hit != NULL) {
        attr = hitGetAttr(hit);
        {
            u32 id = party->yoshiPsndSFXId;
            if (id != 0xFFFFFFFFu) {
                psndSFXOff(id);
                party->yoshiPsndSFXId = -1;
            }
        }

        if (attr & 0x200) {
            u32 changed;

            if (party->useMotionId == 5) {
                changed = 0;
            } else {
                party->useMotionId = 0x46;
                changed = 1;
            }
            if (changed == 0) {
                return;
            }
            {
                Vec pos = party->position;

                marioSetSplash(0, &pos);
            }
            party->groundHit = NULL;
            if (yoshiGetStatus() == 1) {
                MarioWork* pl = party->playerPtr;
                if ((pl->flags & 0xF0000) == 0) {
                    pl->hitobjStandOn = NULL;
                }
            }
            if (!(party->flags & PARTY_FLAG_IS_FALLING)) {
                party->flags |= PARTY_FLAG_IS_FALLING;
                party->jumpPara3 = -0.4f;
                party->tail.yoshi.jumpVel = -0.4f;
                party->tail.yoshi.jumpAccel = 0.0f;
                party->jumpPara2 = 0.0f;
            }
            party->tail.yoshi.jumpAccel = party->tail.yoshi.jumpAccel + party->jumpPara2;
            party->tail.yoshi.jumpVel = party->tail.yoshi.jumpVel + party->tail.yoshi.jumpAccel;
            party->jumpPara3 = party->jumpPara3 + party->tail.yoshi.jumpVel;
            if (party->jumpPara3 < -15.0f) {
                party->jumpPara3 = -15.0f;
            }
            party->position.y += party->jumpPara3;
            set_damage_root_ypos(mario->position.y - 8.0f);
            {
                MarioWork* pl = party->playerPtr;
                Vec* dst = (Vec*)((u8*)pl + 0x98);

                dst[0] = vec[0];
                dst[1] = vec[1];
            }
            {
                PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                if (extra != NULL) {
                    extra->dispOffset = vec[2];
                }
            }
            party->flags |= PARTY_FLAG_FORCE_FALL;
            if (party->jumpPara3 <= -6.0f) {
                party->jumpPara3 = -6.0f;
                party->tail.yoshi.jumpVel = 0.0f;
                party->tail.yoshi.jumpAccel = 0.0f;
                party->jumpPara2 = 0.0f;
            }
            mario->jumpVelY = party->jumpPara3;
            mario->jumpAccelY = party->tail.yoshi.jumpVel;
            mario->jumpJerkY = party->tail.yoshi.jumpAccel;
            mario->jumpJerkStepY = party->jumpPara2;
            marioChgMot(0x1F);
            return;
        }

        if (attr & 0xC00) {
            MarioWork* m = marioGetPtr();

            party->groundHit = NULL;
            if (yoshiGetStatus() == 1) {
                MarioWork* pl = party->playerPtr;
                if ((pl->flags & 0xF0000) == 0) {
                    pl->hitobjStandOn = NULL;
                }
            }
            marioChgMot(0x20);
            party->useMotionId = 0xA;
    #ifdef MUST_MATCH
            // The pool places PYS_S_2 at vec3+0x30; the original reuses the vec3
            // base register (r31) for it instead of a fresh lis.
            partyChgPose(party, (char*)vec + 0x30);
    #else
            partyChgPose(party, YOSHI_S_2);
    #endif
            marioChgPose("M_Y_2");
            m->field_0x2B0 = 0.0f;
            *(f32*)((u8*)m + 0x2B4) = 0.0f;
            partyChgRunMode(party, 0xD);
            {
                PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                if (extra != NULL) {
                    partyChgRunMode(extra, 0xD);
                }
            }
            partyChgRunMode(party, 0xD);
            {
                PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                if (extra != NULL) {
                    partyChgRunMode(extra, 0xD);
                }
            }
            return;
        }

        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x40000000) {
                mario->hitobjDamageReturn = hit;
            }
        }

        if (!(party->flags & 0x30)) {
            f32 out1;
            f32 out2;
            f32 out3;
            u32 r = partyChkFrontStep(party, 11.0f, &out1, &out2, &out3);
            f32 d = out1 - party->position.y;

            if (r != 0 && d <= 11.0f && d >= 5.0f && party->groundSlope <= 30.0f) {
                party->field_120 = out1;
                {
                    f32 zero = 0.0f;

                    if (party->speed != zero) {
                        u32 changed;

                        if (party->useMotionId == 5) {
                            changed = 0;
                        } else {
                            party->useMotionId = 0x28;
                            changed = 1;
                        }
                        if (changed != 0) {
                            return;
                        }
                    }
                }
            }
        }
    }

    party->groundHit = hit;
    if (yoshiGetStatus() == 1) {
        MarioWork* pl = party->playerPtr;
        if ((pl->flags & 0xF0000) == 0) {
            pl->hitobjStandOn = hit;
        }
    }
    if (hit == NULL) {
        party->flags &= ~PARTY_FLAG_IS_GROUNDED;
        if (!(party->flags & PARTY_FLAG_IS_FALLING)) {
            party->flags |= PARTY_FLAG_IS_FALLING;
            party->jumpPara3 = -0.4f;
            party->tail.yoshi.jumpVel = -0.4f;
            party->tail.yoshi.jumpAccel = 0.0f;
            party->jumpPara2 = 0.0f;
        }
        party->tail.yoshi.jumpAccel = party->tail.yoshi.jumpAccel + party->jumpPara2;
        party->tail.yoshi.jumpVel = party->tail.yoshi.jumpVel + party->tail.yoshi.jumpAccel;
        party->jumpPara3 = party->jumpPara3 + party->tail.yoshi.jumpVel;
        if (party->jumpPara3 < -15.0f) {
            party->jumpPara3 = -15.0f;
        }
        party->position.y += party->jumpPara3;
    } else {
        party->flags |= PARTY_FLAG_IS_GROUNDED;
        if (party->flags & PARTY_FLAG_IS_FALLING) {
            party->flags &= ~0x70;
            mario->flags &= ~0xF0000;
            party->jumpPara1 = 0.0f;
            party->position.y = f31;
            {
                u32 id = party->yoshiPsndSFXId;
                if (id != 0xFFFFFFFFu) {
                    psndSFXOff(id);
                    party->yoshiPsndSFXId = -1;
                }
            }
        } else {
            party->position.y = f31;
        }
    }
}

void searchGround2(PartyEntry* party) {
    HitEntry* hit = NULL;
    MarioWork* mario = party->playerPtr;

    if (!(party->flags & PARTY_FLAG_FORCE_FALL)) {
        hit = partySearchGround(party, 11.0f, 0.0f);
    }

    if (hit == NULL) {
        party->groundHit = hit;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = hit;
            }
        }
        return;
    }

    {
        s32 attr = hitGetAttr(hit);

    if (attr & 0x200) {
        Vec pos = party->position;

        marioSetSplash(0, &pos);
        party->groundHit = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        party->jumpPara3 = -0.4f;
        party->tail.yoshi.jumpVel = -0.4f;
        party->tail.yoshi.jumpAccel = 0.0f;
        party->jumpPara2 = 0.0f;
        party->tail.yoshi.jumpAccel = party->tail.yoshi.jumpAccel + party->jumpPara2;
        party->tail.yoshi.jumpVel = party->tail.yoshi.jumpVel + party->tail.yoshi.jumpAccel;
        party->jumpPara3 = party->jumpPara3 + party->tail.yoshi.jumpVel;
        if (party->jumpPara3 < -15.0f) {
            party->jumpPara3 = -15.0f;
        }
        party->position.y += party->jumpPara3;
        party->useMotionId = 0x46;
        return;
    }

    if (attr & 0x800) {
        MarioWork* m = marioGetPtr();

        party->groundHit = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        marioChgMot(0x20);
        party->useMotionId = 0xA;
        partyChgPose(party, YOSHI_S_2);
        marioChgPose("M_Y_2");
        m->field_0x2B0 = 0.0f;
        *(f32*)((u8*)m + 0x2B4) = 0.0f;
        partyChgRunMode(party, 0xD);
        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                partyChgRunMode(extra, 0xD);
            }
        }
    }

    party->flags |= PARTY_FLAG_IS_GROUNDED;
    party->jumpPara1 = 0.0f;
    party->position.y = party->groundY;
    party->flags &= ~0x70;
    mario->flags &= ~0xF0000;
    }
}

// The target's .sdata2 pools the literal 10 right after checkWall's "M_Y_2" (it is only
// read by yoshi_use, but the original registered it while emitting checkWall). This
// deferred helper exists only for its pool slot: source order is reversed for codegen, so
// placed here it is emitted between checkWall and searchGround2, registering 10 there.
// Nothing calls it and the linker strips the body.
#ifdef MUST_MATCH
static f32 ten_pool_helper(void) {
    return 10.0f;
}
#endif

void checkWall(PartyEntry* party, f32 a, f32 b) {
    MarioWork* mario = marioGetPtr();
    Vec pos;
    HitEntry* hit;

    pos = party->position;
    hit = partySearchWallFront(party, &pos, a, b);

    if (hit != NULL) {
        party->field_12C = hit;
        mario->hitobjFront = hit;
        if (a) {
            if ((party->flags & PARTY_FLAG_IS_BEING_USED) && marioChkBero(hit) == 1) {
                party->field_134 = hit;
                mario->hitobjPush = hit;
                mario->hitobjWall = hit;
            } else {
                party->field_134 = NULL;
                mario->hitobjPush = NULL;
                mario->hitobjWall = NULL;
            }
        }

        if (hitGetAttr(hit) & 0x800) {
            MarioWork* m = marioGetPtr();

            party->groundHit = NULL;
            if (yoshiGetStatus() == 1) {
                MarioWork* pl = party->playerPtr;
                if ((pl->flags & 0xF0000) == 0) {
                    pl->hitobjStandOn = NULL;
                }
            }
            marioChgMot(0x20);
            party->useMotionId = 0xA;
            partyChgPose(party, YOSHI_S_2);
            marioChgPose("M_Y_2");
            m->field_0x2B0 = 0.0f;
            *(f32*)((u8*)m + 0x2B4) = 0.0f;
            partyChgRunMode(party, 0xD);
            {
                PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                if (extra != NULL) {
                    partyChgRunMode(extra, 0xD);
                }
            }
        }

        {
            s32 attr = hitGetAttr(hit);

            if ((attr & 0x00800004) != 0 && (attr & 0x00080080) == 0) {
                movePos(&party->position.x, &party->position.z, a, b);
                return;
            }
        }
        party->position = pos;
    } else {
        if (!a) {
            pos = party->position;
            hit = partySearchWallFront(party, &pos, a, party->dir);
            party->field_12C = hit;
            mario->hitobjFront = hit;
            if (hit == NULL) {
                f32 dir = toMovedir(party->field_10C);

                pos = party->position;
                hit = partySearchWallFront(party, &pos, a, dir);
                party->field_12C = hit;
                mario->hitobjFront = hit;
            }
            if (party->buttonNew & 0x100) {
                HitEntry* h;

                pos = party->position;
                h = partySearchWallFront(party, &pos, a, party->dir);
                if (h == NULL) {
                    party->frontHit = h;
                    mario->actionHitObj = h;
                } else {
                    party->frontHit = party->field_12C;
                    mario->actionHitObj = party->field_12C;
                }
            }

            {
                f32 unk;
                HitEntry* h2 = dou10_yoko_yari2(&unk);

                if (h2 != NULL && (hitGetAttr(h2) & 0x800)) {
                    MarioWork* m = marioGetPtr();

                    party->groundHit = NULL;
                    if (yoshiGetStatus() == 1) {
                        MarioWork* pl = party->playerPtr;
                        if ((pl->flags & 0xF0000) == 0) {
                            pl->hitobjStandOn = NULL;
                        }
                    }
                    marioChgMot(0x20);
                    party->useMotionId = 0xA;
                    partyChgPose(party, YOSHI_S_2);
                    marioChgPose("M_Y_2");
                    m->field_0x2B0 = 0.0f;
                    *(f32*)((u8*)m + 0x2B4) = 0.0f;
                    partyChgRunMode(party, 0xD);
                    {
                        PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                        if (extra != NULL) {
                            partyChgRunMode(extra, 0xD);
                        }
                    }
                }
            }
        } else {
            f32 unk;
            HitEntry* h2 = dou10_yoko_yari2(&unk);

            if (h2 != NULL && (hitGetAttr(h2) & 0x800)) {
                MarioWork* m = marioGetPtr();

                party->groundHit = NULL;
                if (yoshiGetStatus() == 1) {
                    MarioWork* pl = party->playerPtr;
                    if ((pl->flags & 0xF0000) == 0) {
                        pl->hitobjStandOn = NULL;
                    }
                }
                marioChgMot(0x20);
                party->useMotionId = 0xA;
                partyChgPose(party, YOSHI_S_2);
                marioChgPose("M_Y_2");
                m->field_0x2B0 = 0.0f;
                *(f32*)((u8*)m + 0x2B4) = 0.0f;
                partyChgRunMode(party, 0xD);
                {
                    PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
                    if (extra != NULL) {
                        partyChgRunMode(extra, 0xD);
                    }
                }
            }
        }
    }

    marioChkWallAround(&pos, 1, a, b, party->radius, mario->wPlayerCollisionBox.y);
    party->position.x = pos.x;
    party->position.z = pos.z;
}


// 100%: `x`/`y` are s32 (the original sign-extends each byte once), the
// `!dist` zero test makes MWCC emit the original mfcr/extrwi/xori/cntlzw/srwi
// boolean, and the `flags |=` before `dist *= 0.75f` puts the fmuls where the
// original schedules it. The literals pool anonymously because the file's
// function order is not yet the original's (their addresses will land right
// once the functions are reordered).
void L_getStick(PartyEntry* party, f32* outAngle, f32* outDist) {
    s32 x;
    s32 y;
    MarioWork* mario = party->playerPtr;

    if (mario->keyOffCount != 0) {
        *outDist = 0.0f;
        *outAngle = party->field_FC;
        return;
    }

    {
        f32 dist;
        f32 ang;

        x = party->stickX;
        y = -(s32)party->stickY;
        dist = distABf(0.0f, 0.0f, (f32)x, (f32)-y);

        if (dist >= 70.0f) {
            dist = 70.0f;
        }
        ang = angleABf(0.0f, 0.0f, (f32)x, (f32)-y);
#ifdef MUST_MATCH
        // `!dist` makes MWCC emit the original mfcr/.../srwi boolean; the
        // natural `dist == 0.0f` test is under #else.
        if (!dist) {
#else
        if (dist == 0.0f) {
#endif
            ang = party->field_FC;
        }
        ang = revise360(ang + party->playerTargetYaw);
        *outAngle = ang;

        if (dist > 0.0f) {
            if (x * x + y * y > 0xBD1) {
                dist = 1.5f * (mario->mBaseDashSpeed * marioGetMoveRate(ang));
            } else {
                dist = 1.5f * (mario->mBaseWalkSpeed * marioGetMoveRate(ang));
            }
            if (hitGetAttr(mario->hitobjStandOn) & 0x100) {
                mario->flags |= 0x04000000;
                dist *= 0.75f;
            } else {
                mario->flags &= ~0x04000000;
            }
            dist *= mario->playerGravity;
        }
        *outDist = dist;
    }
}
