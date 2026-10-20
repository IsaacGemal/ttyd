#include "party.h"
#include "party/party_nokonoko.h"
#include "party/party_move.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_hit.h"
#include "mario/mariost.h"
#include "driver/mobjdrv.h"
#include "eff/n64/eff_small_star_n64.h"
#include "eff/eff_kemutest.h"
#include "party/party_bomhei.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "driver/hitdrv.h"
#include "driver/itemdrv.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include "math_ext.h"
#include "system.h"
#include "matching.h"

// nokonoko_use sound effect handle (sbss).
u32 nokoSe;

#pragma pool_data on
static const f32 float_37 = 37.0f;
static const f32 float_24 = 24.0f;

static const f32 float_3p1416 = 3.1415927f;
static const f32 float_180 = 180.0f;
static const f32 float_0p1 = 0.1f;
static const f32 float_0 = 0.0f;
static const f32 float_neg1 = -1.0f;

#ifdef MUST_MATCH
// MUST_MATCH: nokonoko_use reads the pose names (str_PNK_K_2/158) as offsets from this first .rodata object.
#define NK_STR(off, lit) (lit)
#else
#define NK_STR(off, lit) (lit)
#endif
#define NKF_180 180.0

static const f32 float_90 = 90.0f;
static const f32 float_0p5 = 0.5f;
static const f32 float_10 = 10.0f;
static const f32 float_30 = 30.0f;
static const f32 float_100 = 100.0f;
static const f32 float_11 = 11.0f;
static const f32 float_6 = 6.0f;
static const f32 float_1 = 1.0f;
static const f32 float_0p4 = 0.4f;

s32 nokonokoGetStatus(PartyEntry* party) {
    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    {
        s32 result = 7;
        if (party->useMotionId <= 2) {
            result = 1;
        } else if (party->useMotionId >= 0xA && party->useMotionId <= 0xD) {
            result = 2;
        } else if (party->useMotionId >= 0x14 && party->useMotionId <= 0x15) {
            result = 3;
        } else if (party->useMotionId >= 0x32 && party->useMotionId <= 0x47) {
            result = 4;
        } else if (party->useMotionId >= 0x50 && party->useMotionId <= 0x64) {
            result = 5;
        } else if (party->useMotionId >= 0x6E && party->useMotionId <= 0x72) {
            result = 6;
        }
        return result;
    }
}

s32 nokonoko_holdItem(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    return party->tail.koops.heldItem != NULL;
}

void nokonoko_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 0xE);
    }
}

void nokonoko_init(PartyEntry* party) {
    party->field_33 = 2;
    party->field_32 = 0;
    party->height = float_37;
    party->radius = float_24;
    nokoSe = 0xFFFFFFFFu;
}

void nokonoko_move(PartyEntry* party) {
    marioGetPtr();
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyWalkInit(party, 1);
    }
    partyStepSpeed(party);
    partyRecordFootmark(party);
    switch (party->field_3B) {
    case 0:
        party_move_mode_stay(party);
        break;
    case 1:
        party_move_mode_walk(party);
        break;
    case 2:
        party_move_mode_behind_mario(party);
        break;
    case 3:
        party_move_mode_beside_mario(party);
        break;
    case 4:
        break;
    default:
        break;
    }
}

void unk_800cd9a0(void) {
    PartyEntry* party;

    marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (nokonokoGetStatus(party)) {
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
            party->flags2 &= ~0x20;
            party->flags2 &= ~0x10;
            party->height = float_37;
            *(s32*)((u8*)party + 0x178) = 0;
            *(s32*)((u8*)party + 0x17C) = 0;
            *(s32*)((u8*)party + 0x180) = 0;
        }
        if (party->field_34 != 8 && party->field_34 != 0xD) {
            partyChgRunMode(party, 2);
            party->field_3B = 0;
        }
    }
}

void nokonoko_use_post(PartyEntry* party) {
    if (party->tail.koops.heldItem != NULL) {
        itemNokoForceGet(party->tail.koops.heldItem);
        party->tail.koops.heldItem = NULL;
    }
    marioGetPtr();
    if (nokoSe != 0xFFFFFFFFu) {
        psndSFXOff(nokoSe);
        nokoSe = 0xFFFFFFFFu;
    }
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags2 &= ~0x20;
    party->flags2 &= ~0x10;
    party->height = float_37;
    *(s32*)((u8*)party + 0x178) = 0;
    *(s32*)((u8*)party + 0x17C) = 0;
    *(s32*)((u8*)party + 0x180) = 0;
}

// The `NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C))` updates stay raw: `party->field_10C` there
// schedules the add differently.
#define NKF(off) (*(f32*)((u8*)party + (off)))
#define NKS(off) (*(s32*)((u8*)party + (off)))
#define NKM(off) (*(f32*)((u8*)mario + (off)))

// Koops' overworld use state machine.  Reconstructed from the original asm; the
// many switches mirror the jump tables in party_nokonoko.s.
void nokonoko_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    u32 res;
    Vec out;
    Vec out2;
    Vec v;
    Vec scr;
    f32 sx, sy, sz;
    f32 a, b;
    f32 f0, f1, f2, f3, f4, f5, f6, f31;

    if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
        if (marioKeyOffChk()) {
            scr = party->position;

#ifdef MUST_MATCH
            // MUST_MATCH: the original passes the copy's address (by-value Vec is passed as a
            // pointer in the ABI), which lets `scr` keep a fixed slot at 0x114 like the original.
            ((void (*)(Vec*, f32*, f32*, f32*))marioGetScreenPos)(&scr, &sx, &sy, &sz);
#else
            marioGetScreenPos(scr, &sx, &sy, &sz);
#endif
            if (!marioChkInScreen((s32)sx, (s32)sy)) {
                a = mario->position.x;
                b = mario->position.z;
                movePos(&a, &b, 160.0f,
                        angleABf(mario->position.x, mario->position.z, party->position.x, party->position.z));
                party->position.x = a;
                party->position.z = b;
                nokonoko_finish(party);
                return;
            }
        }
    }

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        party->tail.koops.heldItem = NULL;
        NKS(0x178) = 0;
        NKS(0x17c) = 0;
        NKS(0x180) = 0;
        if (party->flags & PARTY_FLAG_IS_EVT_USE) {
            if (party->unk16C > 0) {
                party->tail.koops.moveFlags |= 0x1;
            }
        } else {
            party->tail.koops.moveFlags &= ~0x1;
        }
        party->useMotionId = 0;
    }

    switch (party->useMotionId) {
    case 0:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            break;
        }
        partyClearJumpPara(party);
        party->flags |= PARTY_FLAG_DISABLE_FLOORS;
        party->flags2 |= 0x10;
        partyChgPose(party, NK_STR(0x18, "PNK_K_2"));
        party->height = 18.5f;
        party->useMotionId = 1;
        party->timer = 100;
        party->tail.koops.heldItem = NULL;
        party->tail.koops.dashing = 0;
        marioAdjustMoveDir();
        mario->baseSpeed = 0.0f;
        party->targetPos = mario->position;
        {
            movePos(&party->targetPos.x, &party->targetPos.z, 2.0f,
                    toMovedirSimple(mario->dispReqDir));
        }
        party->timer = 4;
        party->jumpPara1 = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->speed = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z) / (f32)party->timer;
        party->field_10C = revise360(45.0f + party->field_10C);
        party->useMotionId = 1;
        marioChgSmallJumpMotion();
        mario->trigFlags |= 0x20000;
        mario->baseSpeed = 0.0f;
        break;

    case 1:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            break;
        }
        party->field_10C = revise360(45.0f + party->field_10C);
        party->position.x += (party->targetPos.x - party->position.x) / (f32)party->timer;
        party->position.y += (party->targetPos.y - party->position.y) / (f32)party->timer;
        party->position.z += (party->targetPos.z - party->position.z) / (f32)party->timer;
        if (--party->timer > 0) {
            break;
        }
        party->position = party->targetPos;
        party->speed = 0.0f;
        party->field_110 = mario->dispReqDir;
        party->field_10C = mario->dispReqDir;
        party->useMotionId = 2;
        partyChgPose(party, NK_STR(0x20, "PNK_A_1"));
        /* fallthrough */
    case 2:
        if (*(u16*)&mario->currMotionId == 0x20 || *(u16*)&mario->currMotionId == 0x1f) {
            party->useMotionId = 0x46;
            break;
        }
        if (mario->hitobjStandOn == NULL) {
            break;
        }
        marioChgMot(3);
        mario->flags |= 0x80;
        mario->baseSpeed = 0.0f;
        party->useMotionId = 10;
        party->timer = 15;
        party->speed = 0.0f;
        party->targetDist = 0.0f;
        party->dir = toMovedir(mario->dispReqDir);
        psndSFXOn_3D((const char*)0x92d, &party->position);
        break;

    case 10:
        psndSFXOn_3D((const char*)0x92a, &party->position);
        f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
        f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
        effSmallStarN64Entry(3, 3, party->position.x, party->position.y + party->height, party->position.z,
                             f4, -1.0f, -f31);
        party->flags |= PARTY_FLAG_DISABLE_FLOORS;
        party->flags |= PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
        party->useMotionId = 11;
        /* fallthrough */
    case 11:
        party->speed += 2.0f;
        if (party->speed > 10.0f) {
            party->speed = 10.0f;
        }
        /* fallthrough */
    case 12:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount & 1) {
            f32 k = 0.25f;
            EffectEntry* eff = effKemuTestEntry(0, party->position.x, party->height * k + party->position.y,
                                                party->position.z, 0.3f);
            effKemuTestDrawCam(eff, party->camId);
        }
        party->targetDist += party->speed;
        if (party->tail.koops.heldItem == NULL) {
            party->tail.koops.heldItem = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->radius + 12.0f);
            if (party->tail.koops.heldItem != NULL) {
                itemPickUp(party->tail.koops.heldItem);
                party->useMotionId = 0x32;
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                     f4, -1.0f, -f31);
                break;
            }
        }
        if (ccwall(party, 0)) {
            party->useMotionId = 0x32;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                 f4, -1.0f, -f31);
            break;
        }
        if (!(party->targetDist >= 120.0f)) {
            break;
        }
        party->useMotionId = 0xC;
        party->speed += 0.15f * (-1.0f - party->speed);
        if (!(party->speed <= 0.0f)) {
            break;
        }
        party->speed = 0.0f;
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
            if (mario->buttonHeld & 0x400) {
                party->useMotionId = 0xD;
                party->timer = 0;
            } else {
                party->useMotionId = 0x32;
            }
            break;
        }
        if (party->tail.koops.moveFlags & 1) {
            party->timer = 5;
            party->useMotionId = 0x15;
            party->tail.koops.dashing = 1;
            if (nokoSe == 0xFFFFFFFFu) {
                nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
            }
        } else {
            party->useMotionId = 0x32;
        }
        break;

    case 13:
        if (++party->timer >= 5) {
            party->timer = 5;
            party->useMotionId = 0x14;
            party->tail.koops.dashing = 1;
            if (nokoSe == 0xFFFFFFFFu) {
                nokoSe = psndSFXOn_3D((const char*)0x92e, &party->position);
            }
        }
        /* fallthrough */
    case 20:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount % 30 == 0) {
            EffectEntry* eff = effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f);
            effKemuTestDrawCam(eff, party->camId);
        }
        if (ccwall(party, 0)) {
            party->useMotionId = 0x32;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                 f4, -1.0f, -f31);
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            break;
        }
        if (party->tail.koops.heldItem == NULL) {
            party->tail.koops.heldItem = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->radius + 12.0f);
            if (party->tail.koops.heldItem != NULL) {
                itemPickUp(party->tail.koops.heldItem);
                party->useMotionId = 0x32;
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                     f4, -1.0f, -f31);
                marioGetPtr();
                if (nokoSe != 0xFFFFFFFFu) {
                    psndSFXOff(nokoSe);
                    nokoSe = 0xFFFFFFFFu;
                }
                break;
            }
        }
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
            u16 mot = *(u16*)&mario->currMotionId;
            if (mot == 0x19) { goto L_800CC4E8; }
            if (mot == 0x14) { goto L_800CC4E8; }
            if (mot == 0x18) { goto L_800CC4E8; }
            if (mot == 0x16) { goto L_800CC4E8; }
            if ((u16)(mot - 0x1f) <= 1) { goto L_800CC4E8; }
            if (mario->flags & 0x20) { goto L_800CC4E8; }
            if (marioChkKey() != 0) {
                goto L_800CC514;
            }
        L_800CC4E8:
            nokonoko_finish(party);
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            break;
        L_800CC514:
            if (mario->buttonHeld & 0x400) {
                break;
            }
            party->useMotionId = (party->tail.koops.dashing != 0) ? 0x50 : 0x32;
            marioGetPtr();
            if (nokoSe != 0xFFFFFFFFu) {
                psndSFXOff(nokoSe);
                nokoSe = 0xFFFFFFFFu;
            }
            break;
        }
        if (party->tail.koops.moveFlags & 1) {
            party->useMotionId = 0x50;
        } else {
            party->useMotionId = 0x32;
        }
        marioGetPtr();
        if (nokoSe != 0xFFFFFFFFu) {
            psndSFXOff(nokoSe);
            nokoSe = 0xFFFFFFFFu;
        }
        break;

    case 21:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (gp->retraceCount % 30 == 0) {
            EffectEntry* eff = effKemuTestEntry(5, party->position.x, party->position.y, party->position.z, 1.0f);
            effKemuTestDrawCam(eff, party->camId);
        }
        break;

    default:
        break;
    }

    switch (party->useMotionId) {
    case 0x32:
        partyClearJumpPara(party);
        party->useMotionId = 0x33;
        party->speed = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        psndSFXOn_3D((const char*)0x92b, &party->position);
        break;

    case 0x33:
        if (party->tail.koops.dashing != 0) {
            f3 = 20.0f;
        } else {
            f3 = 10.0f;
        }
        party->speed += 0.3f * (f3 - party->speed);
        if (party->speed >= f3) {
            party->speed = f3;
            if (party->targetDist < 10.0f) {
                party->useMotionId = 0x34;
            }
        }
        /* fallthrough */
    case 0x34:
        if (ccwall(party, 1)) {
            f32 ang, spd;
            partyGetMoveDirSpd(party, &ang, &spd);
            marioChkWallAround(&party->position, 1, spd, ang, party->radius, party->height);
            party->useMotionId = 0x46;
            psndSFXOn_3D((const char*)0x92c, &party->position);
            f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
            f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
            effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                 f4, -1.0f, -f31);
            return;
        }
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->speed = 0.1f * (-1.0f - party->speed) + party->speed;
        party->targetDist -= party->speed;
        if (!(party->targetDist <= 0.0f)) {
            break;
        }
        party->targetDist = 0.0f;
        party->useMotionId = 0x35;
        party->speed = 0.1f * (-1.0f - party->speed) + party->speed;
        party->tail.koops.slideDecay = 50.0f;
        break;

    case 0x35:
        if (ccwall(party, 1)) {
            party->useMotionId = 0x46;
            return;
        }
        party->tail.koops.slideDecay -= 2.2f;
        if (party->tail.koops.slideDecay <= 0.0f) {
            party->tail.koops.slideDecay = 0.0f;
        }
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        f3 = -2.0f - party->speed;
        f2 = (party->tail.koops.dashing != 0) ? 0.2f : 0.1f;
#ifdef MUST_MATCH
        // Volatile re-read of the speed, as retail reloads it after the f3/f2 setup.
        party->speed = f3 * f2 + *(volatile f32*)&party->speed;
#else
        party->speed = f3 * f2 + party->speed;
#endif
        party->targetDist -= party->speed;
        if (!(party->speed <= 0.0f)) {
            break;
        }
        party->speed = 0.0f;
        res = lookupSafetyPos(party->targetPos, &out, party->height);
        if (party->groundHit != NULL && (res == 1 || res == 2 || res == 4 || res == 8)) {
            party->useMotionId = 0x46;
            break;
        }
        switch (res) {
        case 3:
            lookupSafetyPosSub(party->targetPos, &out, party->height, 135.0f);
            break;
        case 5:
            lookupSafetyPosSub(party->targetPos, &out, party->height, 315.0f);
            break;
        case 10:
            lookupSafetyPosSub(party->targetPos, &out, party->height, 225.0f);
            break;
        case 12:
            lookupSafetyPosSub(party->targetPos, &out, party->height, 315.0f);
            break;
        case 7: case 14: case 15:
            party->useMotionId = 0x46;
            break;
        case 1:
            lookupSafetyPosSub(party->targetPos, &out, party->height, 270.0f);
            break;
        case 8:
            lookupSafetyPos3(party->targetPos, &out, party->height);
            break;
        case 0: case 2: case 4: case 6: case 9: case 11: case 13:
        default:
            lookupSafetyPos2(party->targetPos, &out, party->height);
            break;
        }
        party->useMotionId = 0x36;
        party->targetPos = out;
        party->targetDist = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        break;

    case 0x36:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->speed += 0.5f;
        if (party->speed >= 4.0f) {
            party->speed = 4.0f;
        }
        party->targetDist -= party->speed;
        if (party->targetDist <= 10.0f) {
            party->useMotionId = 0x46;
            party->speed = 0.0f;
            return;
        }
        break;

    case 0x37:
        if (ccwall(party, 1)) {
            party->useMotionId = 0x46;
            return;
        }
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->tail.koops.slideDecay -= 2.2f;
        if (party->tail.koops.slideDecay <= 0.0f) {
            party->tail.koops.slideDecay = 0.0f;
        }
        f3 = -2.0f - party->speed;
        f2 = (party->tail.koops.dashing != 0) ? 0.2f : 0.1f;
#ifdef MUST_MATCH
        // Volatile re-read of the speed, as retail reloads it after the f3/f2 setup.
        party->speed = f3 * f2 + *(volatile f32*)&party->speed;
#else
        party->speed = f3 * f2 + party->speed;
#endif
        if (party->speed <= 0.0f) {
            party->useMotionId = 0x46;
            party->speed = 0.0f;
            return;
        }
        break;

    case 0x46:
        f3 = party->position.x - mario->position.x;
        f4 = party->position.y - mario->position.y;
        f2 = party->position.z - mario->position.z;
        {
            f32 d4 = f3 * f3;
            f32 d3 = f4 * f4;
            f32 d2 = f2 * f2;
            f1 = (f32)sqrt(d4 + d3 + d2);
        }
        if (f1 < 5.0f) {
            f0 = (mario->directionView >= NKF_180) ? -90.0f : 90.0f;
            movePos(&party->position.x, &party->position.z,
                    0.5f * mario->wPlayerCollisionBox.x, mario->directionView + f0);
            ccwall(party, 1);
        }
        party->speed = 0.0f;
        party->flags2 &= ~0x10;
        party->flags2 |= 0x20;
        party->useMotionId = 0x47;
        break;

    case 0x47:
        if (party->tail.koops.heldItem != NULL) {
            itemNokoForceGet(party->tail.koops.heldItem);
            party->tail.koops.heldItem = NULL;
        }
        nokonoko_finish(party);
        return;

    default:
        break;
    }

    switch (party->useMotionId) {
    case 0x50:
        party->useMotionId = 0x51;
        party->speed = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->targetDist += 75.0f;
        psndSFXOn_3D((const char*)0x92b, &party->position);
        /* fallthrough */
    case 0x51:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->speed += 3.0f;
        if (party->speed >= 18.0f) {
            party->speed = 18.0f;
        }
        if (party->tail.koops.heldItem == NULL) {
            party->tail.koops.heldItem = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->radius + 12.0f);
            if (party->tail.koops.heldItem != NULL) {
                itemPickUp(party->tail.koops.heldItem);
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                     f4, -1.0f, -f31);
            }
        }
        hit = ccwall(party, 1);
        if (hit != NULL) {
            if (!(hitGetAttr(hit) & 0x80000000)) {
                party->useMotionId = 0x64;
                return;
            }
            mobjHitObjPtrToPtr(hit);
            party->useMotionId = 0x6e;
            return;
        }
        party->targetDist -= party->speed;
        if (party->targetDist <= 20.0f) {
            party->useMotionId = 0x52;
        }
        break;

    case 0x52:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->speed += 0.35f * (-1.0f - party->speed);
        if (party->tail.koops.heldItem == NULL) {
            party->tail.koops.heldItem = itemNearDistCheck(party->position.x, party->position.y, party->position.z,
                                                0.5f * party->radius + 12.0f);
            if (party->tail.koops.heldItem != NULL) {
                itemPickUp(party->tail.koops.heldItem);
                psndSFXOn_3D((const char*)0x92c, &party->position);
                f31 = (f32)cos(6.2831855f * party->dir / 360.0f);
                f4 = (f32)sin(6.2831855f * party->dir / 360.0f);
                effSmallStarN64Entry(3, 1, party->position.x, party->position.y + party->height, party->position.z,
                                     f4, -1.0f, -f31);
            }
        }
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
            break;
        }
        party->targetDist -= party->speed;
        if (party->targetDist <= 0.0f) {
            party->targetDist = distABf(party->targetPos.x, party->targetPos.z, party->position.x, party->position.z);
            party->useMotionId = 0x54;
            party->tail.koops.slideDecay = 50.0f;
            party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        }
        break;

    case 0x54:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
            break;
        }
        party->targetDist -= party->speed;
        if (party->targetDist <= 30.0f) {
            party->useMotionId = 0x56;
            party->speed = 0.0f;
            party->timer = sysMsec2Frame(500);
        }
        break;

    case 0x56:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        if (--party->timer <= 0) {
            party->useMotionId = 0x64;
        }
        if (ccwall(party, 1)) {
            party->useMotionId = 0x64;
        }
        break;

    case 0x64:
        ccwall(party, 1);
        f3 = party->position.x - mario->position.x;
        f4 = party->position.y - mario->position.y;
        f2 = party->position.z - mario->position.z;
        {
            f32 d4 = f3 * f3;
            f32 d3 = f4 * f4;
            f32 d2 = f2 * f2;
            f1 = (f32)sqrt(d4 + d3 + d2);
        }
        if (f1 < 5.0f) {
            f0 = (mario->directionView >= NKF_180) ? 90.0f : -90.0f;
            movePos(&party->position.x, &party->position.z,
                    0.5f * mario->wPlayerCollisionBox.x, mario->directionView + f0);
            ccwall(party, 1);
        }
        party->speed = 0.0f;
        party->flags2 &= ~0x10;
        if (party->tail.koops.heldItem != NULL) {
            itemNokoForceGet(party->tail.koops.heldItem);
            party->tail.koops.heldItem = NULL;
        }
        if (party->groundHit == NULL) {
            party->useMotionId = 0x58;
            break;
        }
        if (party->tail.koops.heldItem != NULL) {
            itemNokoForceGet(party->tail.koops.heldItem);
            party->tail.koops.heldItem = NULL;
        }
        nokonoko_finish(party);
        return;

    case 0x58:
        party->speed = 0.0f;
        res = lookupSafetyPos(party->targetPos, &out2, party->height);
        if (party->groundHit != NULL && (res == 1 || res == 2 || res == 4 || res == 8)) {
            party->useMotionId = 0x46;
            break;
        }
        switch (res) {
        case 3:
            lookupSafetyPosSub(party->targetPos, &out2, party->height, 135.0f);
            break;
        case 5:
            lookupSafetyPosSub(party->targetPos, &out2, party->height, 315.0f);
            break;
        case 10:
            lookupSafetyPosSub(party->targetPos, &out2, party->height, 225.0f);
            break;
        case 12:
            lookupSafetyPosSub(party->targetPos, &out2, party->height, 315.0f);
            break;
        case 7: case 14: case 15:
            party->useMotionId = 0x46;
            break;
        case 1:
            lookupSafetyPosSub(party->targetPos, &out2, party->height, 270.0f);
            break;
        case 8:
            lookupSafetyPos3(party->targetPos, &out2, party->height);
            break;
        case 0: case 2: case 4: case 6: case 9: case 11: case 13:
        default:
            lookupSafetyPos2(party->targetPos, &out2, party->height);
            break;
        }
        party->useMotionId = 0x5A;
        party->targetPos = out2;
        party->targetDist = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->tail.koops.slideDecay = 50.0f;
        break;

    case 0x5A:
        f1 = revise360(party->dir - party->playerTargetYaw);
        f2 = (f1 > 180.0f) ? 1.0f : -1.0f;
        NKF(0x10C) = revise360(50.0f * f2 + NKF(0x10C));
        party->speed += 0.5f;
        if (party->speed >= 4.0f) {
            party->speed = 4.0f;
        }
        party->targetDist -= party->speed;
        if (party->targetDist <= 10.0f) {
            party->useMotionId = 0x46;
            party->speed = 0.0f;
        }
        break;

    case 0x6E:
        party->speed = 0.0f;
        party->jumpPara1 = 15.0f;
        party->jumpPara2 = 0.0f;
        party->timer = sysMsec2Frame(1000);
        party->useMotionId = 0x6f;
        /* fallthrough */
    case 0x6F:
        party->position.y += party->jumpPara1;
        {
            Vec vscr = party->position;
            f32 vsx, vsy, vsz;
            marioGetScreenPos(vscr, &vsx, &vsy, &vsz);
            if (vsy <= -300.0f) {
                party->timer = sysMsec2Frame(800);
                party->useMotionId = 0x70;
            }
        }
        break;

    case 0x70:
        if (--party->timer > 0) {
            break;
        }
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
            partyGetReturnPos(&v);
        } else {
            *(Vec*)&v = mario->position;
            f1 = mario->dispReqDir;
            {
                s32 flag = 0;
                if (f1 >= 90.0f && f1 <= 270.0f) {
                    flag = 1;
                }
                movePos(&v.x, &v.z, 30.0f, toMovedir2(flag ? 0.0f : 180.0f, -30.0f));
            }
        }
        party->position.x = v.x;
        party->position.y = v.y + 1000.0f;
        party->position.z = v.z;
        party->jumpPara1 = 0.0f;
        party->jumpPara2 = -1.0f;
        party->speed = 0.0f;
        party->flags |= PARTY_FLAG_IS_FALLING;
        party->flags |= PARTY_FLAG_FORCE_FALL;
        party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
        party->flags2 &= ~0x20;
        party->flags2 &= ~0x10;
        party->height = 37.0f;
        party->useMotionId = 0x71;
        break;

    case 0x71:
        {
            Vec wscr = party->position;
            f32 wsx, wsy, wsz;
            marioGetScreenPos(wscr, &wsx, &wsy, &wsz);
            if (wsy > 240.0f) {
                party->flags &= ~PARTY_FLAG_FORCE_FALL;
            }
            (void)wsx;
            (void)wsz;
        }
        partyMove(party);
        if (party->jumpPara1 <= -6.0f) {
            party->jumpPara2 = 0.0f;
            party->jumpPara1 = -6.0f;
        }
        if (party->position.y < -2000.0f) {
            party->position = mario->position;
            party->position.y += 200.0f;
        }
        if (party->groundHit != NULL) {
            party->field_110 = mario->dispReqDir;
            party->field_10C = party->field_110;
            if (((hitGetAttr((HitEntry*)party->groundHit) >> 9) & 1) == 0) {
                party->useMotionId = 0x72;
                party->timer = 30;
            }
        }
        break;

    case 0x72:
        if (--party->timer > 0) {
            break;
        }
        nokonoko_finish(party);
        return;

    default:
        break;
    }

    if (party->useMotionId != 0x64) {
        partyMoveNoHosei(party);
    }
    return;
}
#undef NKF
#undef NKS
#undef NKM

void nokonoko_finish(PartyEntry* party) {
    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    if (party->tail.koops.heldItem != NULL) {
        itemNokoForceGet(party->tail.koops.heldItem);
        party->tail.koops.heldItem = NULL;
    }
    marioGetPtr();
    if (nokoSe != 0xFFFFFFFFu) {
        psndSFXOff(nokoSe);
        nokoSe = 0xFFFFFFFFu;
    }
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    party->flags &= ~PARTY_FLAG_DISABLE_FRONT_WALL_CHK;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags2 &= ~0x20;
    party->flags2 &= ~0x10;
    party->height = float_37;
    *(s32*)((u8*)party + 0x178) = 0;
    *(s32*)((u8*)party + 0x17C) = 0;
    *(s32*)((u8*)party + 0x180) = 0;
    if (party->field_34 == 9) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_EVT_USE)) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    } else {
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        partyChgRunMode(party, 0);
    }
}

void nokotaro_hold_cancel(PartyEntry* party) {
    party->useMotionId = 0x14;
}

static inline float fsub32(f32 arg0, f32 arg1)
{
  return arg0 - arg1;
}

#ifdef MUST_MATCH
// Same-byte match for the original's fadds f0, f5, f0 operand order in the first probe.
static inline float fadd32(f32 arg0, f32 arg1)
{
  return arg0 + arg1;
}
#endif

static void ccfloor(PartyEntry* party, s32 flag, f32 step, f32* hitX, f32* hitY, f32* hitZ, f32* distance,
                    f32* hitNX, f32* hitNY, f32* hitNZ);

// Four floor/wall probes: straight ahead, then dir-90, dir+90 and the f31 sweeps.
HitEntry* ccwall(PartyEntry* party, s32 flag) {
    HitEntry* hit;
    f32 sinV, cosV;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 sin2, cos2;
    f32 sin3, cos3;
    f32 sin4, cos4;
    f32 y;
    f32 f31;
    f32 ssin, scos;
    f32 spd;
    f32 dot;
    f32 oy;
    f32 z, x;

    y = float_10;
    ccfloor(party, flag, float_1, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ);

    party->frontHit = NULL;
    party->field_12C = NULL;
    y += float_1;

    x = party->position.x;
    z = party->position.z;
    oy = party->position.y + y;
    distance = float_0p5 * party->radius + party->speed;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, x, oy, z, sinV, float_0, cosV, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin2, &cos2, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin2 * party->radius);
        party->position.z = hitZ + float_0p5 * (cos2 * party->radius);
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (party->tail.koops.dashing != 0 || flag == 0) {
            party->frontHit = hit;
        }
        party->field_12C = hit;
        return hit;
    }

    f31 = float_0p4 * party->radius;
    z = party->position.z + f31 * (f32)cos((f64)(float_3p1416 * (party->dir - float_90) / float_180));
    x = party->position.x + f31 * (f32)sin((f64)(float_3p1416 * (party->dir - float_90) / float_180));
    oy = party->position.y + y;
    distance = float_0p4 * party->radius + party->speed;
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, x, oy, z, sinV, float_0, cosV, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin3, &cos3, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin3 * party->radius);
        party->position.z = hitZ + float_0p5 * (cos3 * party->radius);
        party->position.z -= f31 * (f32)cos((f64)(float_3p1416 * (party->dir - float_90) / float_180));
        party->position.x -= f31 * (f32)sin((f64)(float_3p1416 * (party->dir - float_90) / float_180));
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (party->tail.koops.dashing != 0 || flag == 0) {
            party->frontHit = hit;
        }
        party->field_12C = hit;
        party->position.x = hitX;
        return hit;
    }

    f31 = float_0p4 * party->radius;
    z = party->position.z + f31 * (f32)cos((f64)(float_3p1416 * (party->dir + float_90) / float_180));
    x = party->position.x + f31 * (f32)sin((f64)(float_3p1416 * (party->dir + float_90) / float_180));
    oy = party->position.y + y;
#ifdef MUST_MATCH
    // The original evaluates and stores this probe's distance twice; the first is dead.
    distance = float_0p4 * party->radius + party->speed;
    distance = float_0p4 * party->radius + party->speed;
#else
    distance = float_0p4 * party->radius + party->speed;
#endif
    sincosf(&sinV, &cosV, party->dir);
    hit = hitCheckFilter(NULL, x, oy, z, sinV, float_0, cosV, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 5) == 0) {
        sincosf(&sin4, &cos4, party->dir + float_180);
        party->position.x = hitX + float_0p5 * (sin4 * party->radius);
        party->position.z = hitZ + float_0p5 * (cos4 * party->radius);
        party->position.z -= f31 * (f32)cos((f64)(float_3p1416 * (party->dir + float_90) / float_180));
        party->position.x -= f31 * (f32)sin((f64)(float_3p1416 * (party->dir + float_90) / float_180));
        spd = party->speed;
        ssin = sinV * spd;
        scos = cosV * spd;
        dot = ssin * hitNX + scos * hitNZ;
        party->position.x += float_0p5 * (ssin - dot * hitNX);
        party->position.z += float_0p5 * (scos - dot * hitNZ);
        if (party->tail.koops.dashing != 0 || flag == 0) {
            party->frontHit = hit;
        }
        party->field_12C = hit;
        party->position.x = hitX;
        return hit;
    }
    return NULL;
}

// Floor probe under the party: snaps position.y onto ground within 11 units and flags steps of at least `step`.
static void ccfloor(PartyEntry* party, s32 flag, f32 step, f32* hitX, f32* hitY, f32* hitZ, f32* distance,
                    f32* hitNX, f32* hitNY, f32* hitNZ) {
    HitEntry* hit;
    f32 oy;
    f32 x, z;

    x = party->position.x;
    z = party->position.z;
    oy = party->position.y + party->height + float_10;
#ifdef MUST_MATCH
    *distance = fadd32(party->height, float_30);
#else
    *distance = party->height + float_30;
#endif
    hit = hitCheckFilter(NULL, x, oy, z, float_0, float_neg1, float_0, hitX, hitY, hitZ,
                         distance, hitNX, hitNY, hitNZ);
    if (((hitGetAttr(hit) >> 9) & 1) == 1) {
        hit = NULL;
    }
    party->groundHit = hit;
    if (hit != NULL) {
        f32 py;
        *hitY = (f32)(s32)(float_100 * *hitY + float_0p5) / float_100;
        py = (f32)(s32)(float_100 * party->position.y + float_0p5) / float_100;
        if (__fabsf(py - *hitY) <= float_11) {
            if (py <= *hitY) {
                if (flag == 0) {
                    party->position.y = *hitY;
                    if (__fabsf(py - *hitY) >= step) {
                        party->tail.koops.moveFlags |= 0x2;
                    }
                }
            } else {
                if ((party->tail.koops.moveFlags & 0x2) != 0 && flag == 1) {
                    party->position.y = *hitY;
                }
                if (__fabsf(py - *hitY) <= float_6) {
                    party->position.y = *hitY;
                }
            }
        }
    }
}

s32 lookupSafetyPos(Vec pos, Vec* out, f32 height) {
    s32 result;
    Vec v1;
    Vec v2;
    Vec v3;
    Vec v4;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    MarioWork* mario;
    f32 radius;
    f32 angle;
    f32 s;
    f32 c;
    f32 x;
    f32 y;
    f32 z;
    HitEntry* hit0;
    HitEntry* hit1;
    HitEntry* hit2;
    HitEntry* hit3;

    mario = marioGetPtr();
    radius = mario->wPlayerCollisionBox.x;

    angle = revise360(toMovedir(mario->dispReqDir) - float_180);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos.x;
    v1.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos.y + height;
    z = pos.z + radius * -c;
    v1.y = pos.y;
    v1.z = z;
    distance = float_0p1 + height;
    hit0 = hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY,
                          &hitNZ);

    angle = revise360(toMovedir(mario->dispReqDir) - float_90);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos.x;
    v2.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos.y + height;
    z = pos.z + radius * -c;
    v2.y = pos.y;
    v2.z = z;
    distance = float_0p1 + height;
    hit1 = hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY,
                          &hitNZ);

    angle = revise360(toMovedir(mario->dispReqDir) + float_90);
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos.x;
    v3.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos.y + height;
    z = pos.z + radius * -c;
    v3.y = pos.y;
    v3.z = z;
    distance = float_0p1 + height;
    hit2 = hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY,
                          &hitNZ);

    angle = revise360(toMovedir(mario->dispReqDir));
    s = sin(float_3p1416 * angle / float_180);
    x = radius * s + pos.x;
    v4.x = x;
    c = cos(float_3p1416 * angle / float_180);
    y = pos.y + height;
    z = pos.z + radius * -c;
    v4.y = pos.y;
    v4.z = z;
    distance = float_0p1 + height;
    hit3 = hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY,
                          &hitNZ);

    result = 0;
    if (hit2) {
        result |= 4;
    }
    if (hit3) {
        result |= 8;
    }
    if (hit0) {
        result |= 1;
    }
    if (hit1) {
        result |= 2;
    }

    if (hit0) {
        if (hit1 && hit2 && hit3) {
            out->x = (v1.x + v3.x) * float_0p5;
            out->z = (v1.z + v3.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else if (hit1) {
            out->x = (v1.x + v2.x) * float_0p5;
            out->z = (v1.z + v2.z) * float_0p5;
            out->y = pos.y;
            return result;
        } else if (hit2) {
            out->x = (v1.x + v3.x) * float_0p5;
            out->z = (v1.z + v3.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else if (hit3) {
            out->x = (v1.x + v4.x) * float_0p5;
            out->z = (v1.z + v4.z) * float_0p5;
            out->y = v1.y;
            return result;
        } else {
            *out = v1;
        }
    } else {
        if (hit3) {
            if (hit1) {
                out->x = (v4.x + v2.x) * float_0p5;
                out->z = (v4.z + v2.z) * float_0p5;
                out->y = v4.y;
                return result;
            } else if (hit2) {
                out->x = (v3.x + v2.x) * float_0p5;
                out->z = (v3.z + v2.z) * float_0p5;
                out->y = v3.y;
                return result;
            } else {
                *out = v4;
            }
        } else {
            if (hit1) {
                *out = v2;
                return result;
            }
            if (hit2) {
                *out = v3;
                return result;
            }
        }
    }
    *out = pos;
    return result;
}

// File-local float sine, as tou_03 has; MWCC inlines it into the probes.
static f32 sinf(f32 x) {
    return sin(x);
}

void lookupSafetyPos2(Vec pos, Vec* out, f32 height) {
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 x;
    f32 radius;
    f32 s;
    f32 c;
    f32 y;
    f32 z;
    f32 baseY;
    f32 t;
    f32 u;

    radius = marioGetPtr()->wPlayerCollisionBox.x;

    { f32 a = 1.5707964f; s = sinf(a); c = cos(a); }
    baseY = pos.y;
    t = radius * s;
    x = pos.x + t;
    y = baseY + height;
    u = radius * -c;
    z = pos.z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    { f32 a = 4.712389f; s = sinf(a); c = cos(a); }
    baseY = pos.y;
    t = radius * s;
    x = pos.x + t;
    y = baseY + height;
    u = radius * -c;
    z = pos.z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    { f32 pia = 3.1415927f; s = sinf(pia); c = cos(pia); }
    x = pos.x;
    z = pos.z;
    baseY = pos.y;
    y = baseY + height;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }

    { f32 za = 0.0f; s = sinf(za); c = cos(za); }
    baseY = pos.y;
    t = radius * s;
    x = pos.x + t;
    y = baseY + height;
    u = radius * -c;
    z = pos.z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
        return;
    }
    *out = pos;
}

void lookupSafetyPos3(Vec pos, Vec* out, f32 height) {
    f32 s, c, distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    f32 x, y, z, radius, baseY;
    f32 t, u;

    radius = marioGetPtr()->wPlayerCollisionBox.x;
    { f32 a = 1.5707964f; s = sinf(a); c = cos(a); }
    baseY = pos.y;
    t = radius * s;
    x = pos.x + t;
    y = pos.y + height;
    u = radius * -c;
    z = pos.z + u;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        out->x = x;
        out->z = z;
        out->y = baseY;
    } else {
        { f32 a = 4.712389f; s = sinf(a); c = cos(a); }
        baseY = pos.y;
        t = radius * s;
        x = pos.x + t;
        y = pos.y + height;
        u = radius * -c;
        z = pos.z + u;
        distance = float_0p1 + height;
        if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
            out->x = x;
            out->z = z;
            out->y = baseY;
        } else {
            *out = pos;
        }
    }
}


void lookupSafetyPosSub(Vec pos, Vec* out, f32 height, f32 angle) {
    Vec sp;
    f32 distance;
    f32 hitX, hitY, hitZ;
    f32 hitNX, hitNY, hitNZ;
    Vec* p;
    MarioWork* mario = marioGetPtr();
    f32 rad;
    f32 x;
    f32 y;
    f32 z;
    f32 radius;
    f32 s;
    f32 c;

    radius = mario->wPlayerCollisionBox.x;
    rad = float_3p1416 * angle / float_180;
    s = sin(rad);
    x = radius * s + pos.x;
    c = cos(rad);
    y = pos.y + height;
    z = pos.z + radius * -c;
    distance = float_0p1 + height;
    if (hitCheckFilter(0, x, y, z, float_0, float_neg1, float_0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ)) {
        sp = (Vec){ 0.0f, 0.0f, 0.0f };
        sp.x = x;
        sp.y = y;
        sp.z = z;
        p = &sp;
    } else {
        p = &pos;
    }
    *out = *p;
}
