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

// The original's .rodata puts this conversion double between the vec3 array
// and the PYS_* strings; naming it pins that slot so the strings below land
// at the offsets the original code uses (see unk_800b45e0).  force_active
// keeps it: only L_getStick/inertia's own conversion constants exist in this
// object, so the named copy is otherwise unreferenced data.
#pragma force_active on
__declspec(section ".rodata") const double double_to_int_802cccf8 = 4503601774854144.0;
#pragma force_active reset

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

    if ((s8)*(u8*)((u8*)mario + 0x39) != 0) {
        *outDist = 0.0f;
        *outAngle = party->field_FC;
        return;
    }

    {
        f32 dist;
        f32 ang;

        x = *(s8*)((u8*)party + 0x158);
        y = -(s32)*(s8*)((u8*)party + 0x159);
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
        ang = revise360(ang + *(f32*)((u8*)party + 0xF8));
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

// 100%: the volatile reads/writes reproduce the original's non-pipelined
// load/store interleave in the two 6-float copies (and the playerPtr load order).
// MUST_MATCH only: the natural form is under #else.
void marioYoshiForceCancel(void) {
    const Vec* vec = vec3_802cccd0;
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
                *(Vec*)((u8*)extra + 0x7C) = vec[2];
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

// 100%: same volatile-copy MUST_MATCH trick as marioYoshiForceCancel; the
// natural form is under #else.
void yoshi_exit(PartyEntry* arg) {
    const Vec* vec = vec3_802cccd0;
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
                *(Vec*)((u8*)extra + 0x7C) = vec[2];
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

// The `zero` local (rather than a literal 0.0f) stops MWCC folding
// `f5 - 0.0f` away and lets it reuse the float_0 load held in f2 from the
// `f3 == zero` test, matching the original's else-branch fsubs; it also
// keeps the negated-16 temp from constant-folding across the comparison.
void inertia(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 f3 = (f32)(s8)*(u8*)((u8*)party + 0x158);
    f32 f4 = *(f32*)((u8*)mario + 0x2B0);
    f32 f5 = *(f32*)((u8*)mario + 0x2B4);
    f32 zero = 0.0f;

    if (f3 == zero) {
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
            f5 = f5 - zero;
            f4 = f3 * MATCH_F32(float_0p01) + f4;
            {
                f32 n = -MATCH_F32(float_16);
                if (f4 < n) {
                    f4 = n;
                }
                if (f4 > float_16) {
                    f4 = float_16;
                }
                if (f5 < n) {
                    f5 = n;
                }
                if (f5 > float_16) {
                    f5 = float_16;
                }
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

// 100%: the partyChgPose "PYS_S_2" string is pooled at vec3+0x30, so it is
// reached through the vec3 base register (r31) rather than a fresh lis; the
// 6-float zero copy is emitted as two Vec copies.
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
            Vec* dst = (Vec*)((u8*)pl + 0x98);

            dst[0] = vec[0];
            dst[1] = vec[1];
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
#ifdef MUST_MATCH
        // The pool places PYS_S_2 at vec3+0x30; the original reuses the vec3
        // base register (r31) for it instead of a fresh lis.
        partyChgPose(party, (char*)vec + 0x30);
#else
        partyChgPose(party, "PYS_S_2");
#endif
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
                        goto end;
                    }
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

// 100%: the compiler pools this function's literals under the vec3_802cccd0
// anchor with a shared base register (r31), so the string and mask offsets
// are the original's.  The original .rodata is vec3 (0x00) / double_to_int
// (0x28) / PYS_S_2 (0x30) / PYS_H_1 (0x38) / PYS_R_2 (0x40) / PYS_H_2 (0x48)
// / PYS_F_1 (0x50) / mask (0x58).  The named double above pins the 0x28 slot;
// the S/R literals carry the PYS_H strings that follow them in the original
// pool (strcmp stops at the first NUL, so the comparison is unchanged) so
// S/R/F land at 0x30/0x40/0x50.  PYS_H_1/H_2 are otherwise only used by the
// unwritten yoshi_use; when it is written this layout should be redone with
// the real named strings.
#ifdef MUST_MATCH
#define YOSHI_S_2 "PYS_S_2\0PYS_H_1"
#define YOSHI_R_2 "PYS_R_2\0PYS_H_2"
#else
#define YOSHI_S_2 "PYS_S_2"
#define YOSHI_R_2 "PYS_R_2"
#endif
#define YOSHI_F_1 "PYS_F_1"

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

#undef YOSHI_S_2
#undef YOSHI_R_2
#undef YOSHI_F_1
