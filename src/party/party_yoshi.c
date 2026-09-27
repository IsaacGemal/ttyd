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

// .sbss
s32 jumpheight;
s32 evt_yoshi_frm;
f32 evt_yoshi_spd;

// Local declarations: not yet in a header.
// NONMATCHING (0.00%): not implemented yet (bulk of the file).

// Sweat/smoke struct stored in PartyEntry::useStruct (0x170) while riding Yoshi.
typedef struct PartyYoshiUse {
    u8 field_00[0x10];
    s32 timer;          // 0x10
    EffectEntry* effect; // 0x14
} PartyYoshiUse;

typedef struct YoshiZeroVec6 {
    Vec a;
    Vec b;
} YoshiZeroVec6;

// .rodata
const Vec vec3_802cccd0[3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

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

// NONMATCHING (85.25%): 2 instructions differ, 9 missing, 6 extra, plus register differences. Known issue: negates then truncates to s8; the original sign-extends first (differs only for -128).
void L_getStick(PartyEntry* party, f32* outAngle, f32* outDist) {
    MarioWork* mario = party->playerPtr;

    if ((s8)*(u8*)((u8*)mario + 0x39) != 0) {
        *outDist = 0.0f;
        *outAngle = party->field_FC;
        return;
    }

    {
        s8 x = *(s8*)((u8*)party + 0x158);
        s8 y = -*(s8*)((u8*)party + 0x159);
        f32 dist = distABf(0.0f, 0.0f, (f32)x, (f32)-y);
        f32 ang;

        if (dist >= 70.0f) {
            dist = 70.0f;
        }
        ang = angleABf(0.0f, 0.0f, (f32)x, (f32)-y);
        if (dist == 0.0f) {
            ang = party->field_FC;
        }
        ang = revise360(ang + *(f32*)((u8*)party + 0xF8));
        *outAngle = ang;

        if (dist > 0.0f) {
            if (x * x + y * y > 0xBD1) {
                dist = 1.5f * (mario->mBaseDashSpeed * marioGetMoveRate(ang));
            } else {
                dist = 1.5f * (mario->mBaseWalkSpeed * marioGetMoveRate(ang));
            }
            if (hitGetAttr(mario->hitobjStandOn) & 0x100) {
                dist *= 0.75f;
                mario->flags |= 0x04000000;
            } else {
                mario->flags &= ~0x04000000;
            }
            dist *= mario->playerGravity;
        }
        *outDist = dist;
    }
}

void yoshi_init(PartyEntry* party) {
    party->field_33 = 4;
    party->field_32 = 0;
    *(f32*)((u8*)party + 0xF0) = float_26;
    *(f32*)((u8*)party + 0xF4) = float_20;
    party->yoshiPsndSFXId = -1;
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

// NONMATCHING (95.60%): the pose-flag test emits a single `beq end`
// where the original emits `bne body; b end`.
void yoshi_jumpStand(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        return;
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (party->useMotionId < 0x3C || party->useMotionId > 0x42) {
            party->useMotionId = 0x3C;
        }
    } else {
        return;
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

void L_sweatEntry(PartyEntry* party, void* param) {
    party->field_138 = param;

    if (yoshiGetStatus() == 1) {
        MarioWork* mario = party->playerPtr;
        if ((mario->flags & 0xF0000) == 0) {
            mario->hitobjStandOn = param;
        }
    }
}

s32 evt_get_yoshi_frm(void) {
    return evt_yoshi_frm;
}

void evt_set_yoshi_spd(f32 spd) {
    evt_yoshi_spd = spd;
}

void evt_set_yoshi_frm(s32 msec) {
    evt_yoshi_frm = sysMsec2Frame(msec);
}

// NONMATCHING (93.55%): the three strcmp literals need to share the vec3
// rodata base+0x30/0x40/0x50; compiler gives each its own literal.
void unk_800b45e0(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    const char* anim = party->field_18;

    if (strcmp(anim, "PYS_S_2") == 0) {
        u16 t = *(u16*)((u8*)mario + 0x28) + 1;
        *(u16*)((u8*)mario + 0x28) = t;
        if (t >= 0x34) {
            *(u16*)((u8*)mario + 0x28) = 0;
        }
        party->field_20 = *(u16*)((u8*)mario + 0x28);
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)((u8*)mario + 0x28));
    } else if (strcmp(anim, "PYS_R_2") == 0) {
        u16 t = *(u16*)((u8*)mario + 0x28) + 1;
        *(u16*)((u8*)mario + 0x28) = t;
        if (t >= 0xA) {
            *(u16*)((u8*)mario + 0x28) = 0;
        }
        party->field_20 = *(u16*)((u8*)mario + 0x28);
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)((u8*)mario + 0x28));
    } else if (strcmp(anim, "PYS_F_1") == 0) {
        u16 t = *(u16*)((u8*)mario + 0x28) + 1;
        *(u16*)((u8*)mario + 0x28) = t;
        if (t >= 0xA) {
            *(u16*)((u8*)mario + 0x28) = 0;
        }
        party->field_20 = *(u16*)((u8*)mario + 0x28);
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(u16*)((u8*)mario + 0x28));
    }
}

void unk_800b474c(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();

    party->field_138 = NULL;
    if (yoshiGetStatus() == 1) {
        MarioWork* pl = party->playerPtr;
        if ((pl->flags & 0xF0000) == 0) {
            pl->hitobjStandOn = NULL;
        }
    }
    marioChgMot(0x20);
    party->useMotionId = 0xA;
    partyChgPose(party, "PYS_S_2");
    marioChgPose("M_Y_2");
    *(f32*)((u8*)mario + 0x2B0) = 0.0f;
    *(f32*)((u8*)mario + 0x2B4) = 0.0f;
    partyChgRunMode(party, 0xD);
    {
        PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
        if (extra != NULL) {
            partyChgRunMode(extra, 0xD);
        }
    }
}

// NONMATCHING (94.90%): register interleave in the 6-float vec copy.
void marioYoshiForceCancel(void) {
    const Vec* vec = vec3_802cccd0;
    MarioWork* mario = marioGetPtr();
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (yoshiGetStatus() != 0) {
        MarioWork* pl = party->playerPtr;
        *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(YoshiZeroVec6*)vec;

        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                *(Vec*)((u8*)extra + 0x7C) = vec[2];
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

// NONMATCHING (95.13%): shared cancel body is duplicated inline; remaining
// difference is load/store interleaving in the 6-float copy.
void yoshi_exit(PartyEntry* arg) {
    const Vec* vec = vec3_802cccd0;
    MarioWork* mario;
    PartyEntry* party;

    arg->playerPtr->flags &= ~MARIO_FLAG_CHG_MOT_DISABLED;
    mario = marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (yoshiGetStatus() != 0) {
        MarioWork* pl = party->playerPtr;
        *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(YoshiZeroVec6*)vec;

        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                *(Vec*)((u8*)extra + 0x7C) = vec[2];
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

void L_yoshiFlyMove(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();
    f32 f31;
    f32 f30;
    f32 f29;
    f32 lim;

    inertia(party);
    f30 = *(f32*)((u8*)mario + 0x2B0);
    f31 = -*(f32*)((u8*)mario + 0x2B4);

    f29 = float_0p1 * distABf(0.0f, 0.0f, f30, f31);
    lim = mario->mBaseDashSpeed * MATCH_F32(float_1);
    if (f29 >= lim) {
        f29 = lim;
    }

    f31 = angleABf(0.0f, 0.0f, f30, f31);
    if (f29 < float_0p01) {
        f29 = 0.0f;
    }

    party->speed = f29;
    if (f29 != MATCH_F32(float_0)) {
        party->dir = revise360(f31 + *(f32*)((u8*)party + 0xF8));
        party->field_FC = revise360(f31 + *(f32*)((u8*)party + 0xF8));
    }
    *(f32*)((u8*)mario + 0x18C) = f29;
}

// NONMATCHING (95.12%): the t=f4*f4+f5*f5 association and the float_0p01
// operand order are reproduced. Remaining in the else branch: fmadds register
// order, `fneg` of float_16 vs a pooled -16, and the `f5 -= 0.0f` position.
void inertia(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 f3 = (f32)(s8)*(u8*)((u8*)party + 0x158);
    f32 f4 = *(f32*)((u8*)mario + 0x2B0);
    f32 f5 = *(f32*)((u8*)mario + 0x2B4);

    if (f3 == 0.0f) {
        f32 k = float_neg0p07;
        if (*(u32*)mario->field_0x1F0 != 0) {
            k = float_neg0p5;
        }
        f4 = f4 * k + f4;
        f5 = f5 * k + f5;
        if (__fabsf(f4) < float_0p01) {
            f4 = 0.0f;
        }
        if (__fabsf(f5) < float_0p01) {
            f5 = 0.0f;
        }
    } else {
        f32 t = f4 * f4 + f5 * f5;

        if (t <= float_256) {
            s32 v = *(s32*)((u8*)party->useStruct + 4);
            f4 = f3 * MATCH_F32(float_0p01) + f4;
            if (v > 0 && f4 <= 0.0f) {
                f4 = 0.0f;
            }
            if (v < 0 && f4 >= 0.0f) {
                f4 = 0.0f;
            }
            f5 = f5 - MATCH_F32(float_0);
        } else {
            f5 = f5 - MATCH_F32(float_0);
            f4 = f3 * float_0p01 + f4;
            if (f4 < -float_16) {
                f4 = -float_16;
            }
            if (f4 > float_16) {
                f4 = float_16;
            }
            if (f5 < -float_16) {
                f5 = -float_16;
            }
            if (f5 > float_16) {
                f5 = float_16;
            }
        }
    }
    *(f32*)((u8*)mario + 0x2B0) = f4;
    *(f32*)((u8*)mario + 0x2B4) = f5;
}

void searchGround2(PartyEntry* party) {
    HitEntry* hit = NULL;
    MarioWork* mario = party->playerPtr;

    if (!(party->flags & PARTY_FLAG_FORCE_FALL)) {
        hit = partySearchGround(party, 11.0f, 0.0f);
    }

    if (hit == NULL) {
        party->field_138 = hit;
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
        party->field_138 = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        party->jumpPara3 = -0.4f;
        *(f32*)((u8*)party + 0x174) = -0.4f;
        *(f32*)((u8*)party + 0x178) = 0.0f;
        party->jumpPara2 = 0.0f;
        *(f32*)((u8*)party + 0x178) = *(f32*)((u8*)party + 0x178) + party->jumpPara2;
        *(f32*)((u8*)party + 0x174) = *(f32*)((u8*)party + 0x174) + *(f32*)((u8*)party + 0x178);
        party->jumpPara3 = party->jumpPara3 + *(f32*)((u8*)party + 0x174);
        if (party->jumpPara3 < -15.0f) {
            party->jumpPara3 = -15.0f;
        }
        party->position.y += party->jumpPara3;
        party->useMotionId = 0x46;
        return;
    }

    if (attr & 0x800) {
        MarioWork* m = marioGetPtr();

        party->field_138 = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        marioChgMot(0x20);
        party->useMotionId = 0xA;
        partyChgPose(party, "PYS_S_2");
        marioChgPose("M_Y_2");
        *(f32*)((u8*)m + 0x2B0) = 0.0f;
        *(f32*)((u8*)m + 0x2B4) = 0.0f;
        partyChgRunMode(party, 0xD);
        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                partyChgRunMode(extra, 0xD);
            }
        }
    }

    party->flags |= 0x2;
    party->jumpPara1 = 0.0f;
    party->position.y = party->field_E4;
    party->flags &= ~0x70;
    mario->flags &= ~0xF0000;
    }
}

// NONMATCHING (98.20%): the partyChgPose "PYS_S_2" string is referenced via
// the vec3 rodata base+0x30 in the original instead of its own lis.
void searchGround(PartyEntry* party) {
    const Vec* vec = vec3_802cccd0;
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
        f31 = party->field_E4;
    }

    if (hit == NULL) {
        goto L_2804;
    }

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
            goto end;
        }
        {
            Vec pos = party->position;

            marioSetSplash(0, &pos);
        }
        party->field_138 = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        if (!(party->flags & 0x20)) {
            party->flags |= 0x20;
            party->jumpPara3 = -0.4f;
            *(f32*)((u8*)party + 0x174) = -0.4f;
            *(f32*)((u8*)party + 0x178) = 0.0f;
            party->jumpPara2 = 0.0f;
        }
        *(f32*)((u8*)party + 0x178) = *(f32*)((u8*)party + 0x178) + party->jumpPara2;
        *(f32*)((u8*)party + 0x174) = *(f32*)((u8*)party + 0x174) + *(f32*)((u8*)party + 0x178);
        party->jumpPara3 = party->jumpPara3 + *(f32*)((u8*)party + 0x174);
        if (party->jumpPara3 < -15.0f) {
            party->jumpPara3 = -15.0f;
        }
        party->position.y += party->jumpPara3;
        set_damage_root_ypos(mario->position.y - 8.0f);
        {
            MarioWork* pl = party->playerPtr;
            *(YoshiZeroVec6*)((u8*)pl + 0x98) = *(YoshiZeroVec6*)vec;
        }
        {
            PartyEntry* extra = partyGetPtr(marioGetExtraPartyId());
            if (extra != NULL) {
                *(Vec*)((u8*)extra + 0x7C) = vec[2];
            }
        }
        party->flags |= PARTY_FLAG_FORCE_FALL;
        if (party->jumpPara3 <= -6.0f) {
            party->jumpPara3 = -6.0f;
            *(f32*)((u8*)party + 0x174) = 0.0f;
            *(f32*)((u8*)party + 0x178) = 0.0f;
            party->jumpPara2 = 0.0f;
        }
        *(f32*)((u8*)mario + 0x7C) = party->jumpPara3;
        *(f32*)((u8*)mario + 0x80) = *(f32*)((u8*)party + 0x174);
        *(f32*)((u8*)mario + 0x84) = *(f32*)((u8*)party + 0x178);
        *(f32*)((u8*)mario + 0x88) = party->jumpPara2;
        marioChgMot(0x1F);
        goto end;
    }

    if (attr & 0xC00) {
        MarioWork* m = marioGetPtr();

        party->field_138 = NULL;
        if (yoshiGetStatus() == 1) {
            MarioWork* pl = party->playerPtr;
            if ((pl->flags & 0xF0000) == 0) {
                pl->hitobjStandOn = NULL;
            }
        }
        marioChgMot(0x20);
        party->useMotionId = 0xA;
        partyChgPose(party, "PYS_S_2");
        marioChgPose("M_Y_2");
        *(f32*)((u8*)m + 0x2B0) = 0.0f;
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
        goto end;
    }

    if (hit != NULL) {
        if (hitGetAttr(hit) & 0x40000000) {
            *(void**)((u8*)mario + 0x204) = hit;
        }
    }

    if (!(party->flags & 0x30)) {
        f32 out1;
        f32 out2;
        f32 out3;
        u32 r = partyChkFrontStep(party, 11.0f, &out1, &out2, &out3);
        f32 d = out1 - party->position.y;

        if (r != 0 && d <= 11.0f && d >= 5.0f && *(f32*)((u8*)party + 0xE0) <= 30.0f) {
            *(f32*)((u8*)party + 0x120) = out1;
            if (party->speed != 0.0f) {
                u32 changed;

                if (party->useMotionId == 5) {
                    changed = 0;
                } else {
                    party->useMotionId = 0x28;
                    changed = 1;
                }
                if (changed != 0) {
                    goto end;
                }
            }
        }
    }

L_2804:
    party->field_138 = hit;
    if (yoshiGetStatus() == 1) {
        MarioWork* pl = party->playerPtr;
        if ((pl->flags & 0xF0000) == 0) {
            pl->hitobjStandOn = hit;
        }
    }
    if (hit != NULL) {
        goto L_2968;
    }

    party->flags &= ~0x2;
    if (!(party->flags & 0x20)) {
        party->flags |= 0x20;
        party->jumpPara3 = -0.4f;
        *(f32*)((u8*)party + 0x174) = -0.4f;
        *(f32*)((u8*)party + 0x178) = 0.0f;
        party->jumpPara2 = 0.0f;
    }
    *(f32*)((u8*)party + 0x178) = *(f32*)((u8*)party + 0x178) + party->jumpPara2;
    *(f32*)((u8*)party + 0x174) = *(f32*)((u8*)party + 0x174) + *(f32*)((u8*)party + 0x178);
    party->jumpPara3 = party->jumpPara3 + *(f32*)((u8*)party + 0x174);
    if (party->jumpPara3 < -15.0f) {
        party->jumpPara3 = -15.0f;
    }
    party->position.y += party->jumpPara3;
    goto end;

L_2968:
    party->flags |= 0x2;
    if (party->flags & 0x20) {
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

end: ;
}


void checkWall(PartyEntry* party, f32 a, f32 b) {
    MarioWork* mario = marioGetPtr();
    Vec pos;
    HitEntry* hit;

    pos = party->position;
    hit = partySearchWallFront(party, &pos, a, b);

    if (hit != NULL) {
        party->field_12C = hit;
        *(HitEntry**)((u8*)mario + 0x1E0) = hit;
        if (a) {
            if ((party->flags & 0x100) && marioChkBero(hit) == 1) {
                party->field_134 = hit;
                *(HitEntry**)((u8*)mario + 0x1EC) = hit;
                *(HitEntry**)((u8*)mario + 0x1F0) = hit;
            } else {
                party->field_134 = NULL;
                *(HitEntry**)((u8*)mario + 0x1EC) = NULL;
                *(HitEntry**)((u8*)mario + 0x1F0) = NULL;
            }
        }

        if (hitGetAttr(hit) & 0x800) {
            MarioWork* m = marioGetPtr();

            party->field_138 = NULL;
            if (yoshiGetStatus() == 1) {
                MarioWork* pl = party->playerPtr;
                if ((pl->flags & 0xF0000) == 0) {
                    pl->hitobjStandOn = NULL;
                }
            }
            marioChgMot(0x20);
            party->useMotionId = 0xA;
            partyChgPose(party, "PYS_S_2");
            marioChgPose("M_Y_2");
            *(f32*)((u8*)m + 0x2B0) = 0.0f;
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
                goto end;
            }
        }
        party->position = pos;
    } else {
        if (!a) {
            pos = party->position;
            hit = partySearchWallFront(party, &pos, a, party->dir);
            party->field_12C = hit;
            *(HitEntry**)((u8*)mario + 0x1E0) = hit;
            if (hit == NULL) {
                f32 dir = toMovedir(party->field_10C);

                pos = party->position;
                hit = partySearchWallFront(party, &pos, a, dir);
                party->field_12C = hit;
                *(HitEntry**)((u8*)mario + 0x1E0) = hit;
            }
            if (*(u16*)((u8*)party + 0x152) & 0x100) {
                HitEntry* h;

                pos = party->position;
                h = partySearchWallFront(party, &pos, a, party->dir);
                if (h == NULL) {
                    *(HitEntry**)((u8*)party + 0x130) = h;
                    *(HitEntry**)((u8*)mario + 0x1DC) = h;
                } else {
                    *(HitEntry**)((u8*)party + 0x130) = party->field_12C;
                    *(HitEntry**)((u8*)mario + 0x1DC) = party->field_12C;
                }
            }

            {
                f32 unk;
                HitEntry* h2 = dou10_yoko_yari2(&unk);

                if (h2 != NULL && (hitGetAttr(h2) & 0x800)) {
                    MarioWork* m = marioGetPtr();

                    party->field_138 = NULL;
                    if (yoshiGetStatus() == 1) {
                        MarioWork* pl = party->playerPtr;
                        if ((pl->flags & 0xF0000) == 0) {
                            pl->hitobjStandOn = NULL;
                        }
                    }
                    marioChgMot(0x20);
                    party->useMotionId = 0xA;
                    partyChgPose(party, "PYS_S_2");
                    marioChgPose("M_Y_2");
                    *(f32*)((u8*)m + 0x2B0) = 0.0f;
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

                party->field_138 = NULL;
                if (yoshiGetStatus() == 1) {
                    MarioWork* pl = party->playerPtr;
                    if ((pl->flags & 0xF0000) == 0) {
                        pl->hitobjStandOn = NULL;
                    }
                }
                marioChgMot(0x20);
                party->useMotionId = 0xA;
                partyChgPose(party, "PYS_S_2");
                marioChgPose("M_Y_2");
                *(f32*)((u8*)m + 0x2B0) = 0.0f;
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

    marioChkWallAround(&pos, 1, a, b, *(f32*)((u8*)party + 0xF4), mario->wPlayerCollisionBox.y);
    party->position.x = pos.x;
    party->position.z = pos.z;

end: ;
}
