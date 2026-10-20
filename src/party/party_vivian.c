#include "party/party_vivian.h"
#include "party.h"
#include "party/party_move.h"
#include "party/party_motion.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_motion.h"
#include "mario/mario_sbr.h"
#include "mario/mario_hit.h"
#include "mario/mariost.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "eff/eff_ripple.h"
#include "pmario_sound.h"
#include "system.h"
#include <string.h>

// .sbss
s32 vivian_cancel_req;

// .rodata

// .sdata2

// 152 B
void N_marioForceVivianAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn("p_bibi");
    marioChgPose("M_B_3");
    marioChgPaper("PM_B_1");
    mario->poseTime = 0x18;
    animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)*(u16*)&mario->poseTime);
}

// 104 B
void mot_vivian(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->flags &= ~0x000F0000;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = 0.0f;
        *(u32*)&mario->field_0x1DC[4] = 0;
        *(u32*)&mario->field_0x1DC[0] = 0;
        mario->hitobjPush = NULL;
        *(u32*)&mario->field_0x1F0[0] = 0;
    }
}

// 340 B
void mot_vivian_post(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1f) {
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        *(Vec*)&mario->field_0x98[0xC] = (Vec){0.0f, 0.0f, 0.0f};
        mario->flags &= ~0x400;
        if (party != NULL) {
            partyPaperOff(party);
            party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            party->flags &= ~PARTY_FLAG_IS_EVT_USE;
            party->flags2 &= ~0x200000;
#ifdef MUST_MATCH
            *(volatile f32*)&party->tail.vivian.charge = 10.0f;
#else
            party->tail.vivian.charge = 10.0f;
#endif
            party->dispOffset = (Vec){0.0f, 0.0f, 0.0f};
            partyChgRunMode(party, 2);
            party->field_3B = 0;
            partyChgPoseId(party, 1);
            if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                *(s8*)&party->field_4D[2] &= ~1;
            }
        }
    } else {
        if (party == NULL || party->currentMemberId != PARTY_MEMBER_VIVIAN || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
            return;
        }
        vivian_cancel_req = 1;
    }
}

// 84 B
void vivian_init_801809C0(PartyEntry* party) {
    party->field_33 = 6;
    party->field_32 = 1;
    party->height = 35.0f;
    party->radius = 24.0f;
    party->tail.vivian.charge = 10.0f;
    party->field_4D[2] = 0;
    party->field_B8 = party->position;
    party->tail.vivian.ripple = NULL;
    vivian_cancel_req = 0;
}

s32 vivianGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != PARTY_MEMBER_VIVIAN || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 0;
    }
    if (party->useMotionId < 0xa) {
        return 1;
    }
    if (party->useMotionId == 0xa) {
        return 2;
    }
    if (party->useMotionId >= 0x14 && party->useMotionId <= 0x1b) {
        return 3;
    }
    return 4;
}

// 60 B
void vivianUnhold(PartyEntry* party) {
    if (party == NULL || party->currentMemberId != PARTY_MEMBER_VIVIAN || !(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    if (party->useMotionId != 0xa) {
        return;
    }
    party->useMotionId = 0x14;
}

// 28 B
void L_partyVivianTailStop(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        party->field_4D[2] |= 1;
    }
}

// 28 B
void L_partyVivianTailStart(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        *(s8*)&party->field_4D[2] &= ~1;
    }
}

// 156 B
s32 N_vivianDisplayTailCheck(PartyEntry* party) {
    if (party->currentMemberId != PARTY_MEMBER_VIVIAN) {
        return 0;
    }
    if (party->field_4D[2] & 1) {
        return 0;
    }
    if (party->flags2 & 0x800000) {
        return 0;
    }
    if (strcmp(gp->currentMapName, "bom_01") == 0 && party->position.y == -1000.0f) {
        return 0;
    }
    return 1;
}

// opt_common_subs off: needed for the target FPR colouring (field_B8.x in f1, the 0.1 constant in f6).
#pragma opt_common_subs off
// 240 B
void vivianShadow(PartyEntry* party) {
    if (party != NULL && party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        f32 dx = party->position.x - party->field_B8.x;
        f32 dy = party->position.y - party->field_B8.y;
        f32 dz = party->position.z - party->field_B8.z;
        f32 mx = 0.1f * dx;
        f32 my = 0.1f * dy;
        f32 mz = 0.1f * dz;
        party->field_B8.x = party->field_B8.x + mx;
        party->field_B8.y = party->field_B8.y + my;
        party->field_B8.z = party->field_B8.z + mz;
        if (distABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z) >= 22.0f) {
            party->field_B8 = party->position;
            movePos(&party->field_B8.x, &party->field_B8.z, 22.0f, angleABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z));
        }
    }
}

// opt_common_subs off: needed for the target FPR colouring (field_B8.x in f1, the 0.1 constant in f6).
// Without it the first loads and the constant land one register off.
// 280 B
void vivian_move(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyMoveFlyInit(party, 1);
    }
    partyFlyMain(party);
    if (party != NULL && party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        // The temps are declared mz, my, mx (reverse use order): that colouring is what puts the
        // target's `my` in f3 and `mz` in f1. Assignments stay in mx, my, mz order.
        f32 dx, dy, dz, mz, my, mx;
        dx = party->position.x - party->field_B8.x;
        dy = party->position.y - party->field_B8.y;
        dz = party->position.z - party->field_B8.z;
        mx = 0.1f * dx;
        my = 0.1f * dy;
        mz = 0.1f * dz;
        party->field_B8.x = party->field_B8.x + mx;
        party->field_B8.y = party->field_B8.y + my;
        party->field_B8.z = party->field_B8.z + mz;
        if (distABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z) >= 22.0f) {
            party->field_B8 = party->position;
            movePos(&party->field_B8.x, &party->field_B8.z, 22.0f, angleABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z));
        }
    }
}
#pragma opt_common_subs reset

static inline void vivianUseStep(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 gy, gx, gz;
    f32 sx, sy, sz;
    HitEntry* hit;

    marioGetScreenPos(mario->position, &sx, &sy, &sz);
    hit = marioSearchGround(&gy, &gx, &gz, 37.0f, -37.0f);
    if (hit != NULL && !(hitGetAttr(hit) & 0xa00)) {
        mario->hitobjStandOn = hit;
        mario->position.y = gy;
        if (mario->riderDelta.y < 0.0f) {
            gy += 0.2f;
        }
    }
    if (party->useMotionId >= 0x14 && party->useMotionId < 0x1e) {
        party->tail.vivian.moveSpeed += -0.4f;
        if (party->tail.vivian.moveSpeed < -13.0f) {
            party->tail.vivian.moveSpeed = -13.0f;
        }
        party->targetPos = mario->position;
        movePos(&party->targetPos.x, &party->targetPos.z, party->tail.vivian.moveSpeed,
                toMovedirSimple(mario->unk1A8));
        movePos(&party->targetPos.x, &party->targetPos.z, 0.0f, toMovedirSimple(mario->unk1A8));
        movePos(&party->targetPos.x, &party->targetPos.z,
                (*(f32*)&mario->unk1AC[0] == 0.0f) ? -2.5f : 2.5f,
                toMovedir(270.0f + mario->unk1A8));
    } else {
        if (party->useMotionId >= 4) {
            party->tail.vivian.moveSpeed += 0.4f;
            if (party->tail.vivian.moveSpeed >= 0.0f) {
                party->tail.vivian.moveSpeed = 0.0f;
            }
        }
        party->targetPos = mario->position;
        movePos(&party->targetPos.x, &party->targetPos.z, party->tail.vivian.moveSpeed,
                toMovedirSimple(mario->unk1A8));
        movePos(&party->targetPos.x, &party->targetPos.z,
                (*(f32*)&mario->unk1AC[0] == 0.0f) ? -2.5f : 2.5f,
                toMovedir(270.0f + mario->unk1A8));
    }
    if (strcmp(gp->currentMapName, "eki_02") == 0) {
        marioChkWallAround(&mario->position, 1, 0.0f, mario->directionView,
                           *(f32*)&mario->wPlayerCollisionBox.z, 0.5f);
    }
}

// The zero-vector copies are `(Vec){0,0,0}` templates: the compiler pools the three of them at
// .rodata 0x0/0xC/0x18, so this function's copy is the target's `lwz r3,0x18(r31)` with no extra
// address copy (a named vec3_802f80c0 would instead be materialised into r4 first).
void vivian_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        party->useMotionId = 0;
        party->tail.vivian.charge = 10.0f;
        party->field_B8 = party->position;
        vivian_cancel_req = 0;
        L_partyForceSlitOff(party);
    }
    mario->baseSpeed = 0.0f;
    party->speed = 0.0f;

    switch (party->useMotionId) {
    case 0:
        marioChgMot(0x1c);
        party->tail.vivian.moveSpeed = -13.0f;
        vivianUseStep(party);
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        party->field_10C = mario->unk1A8;
        party->field_110 = *(f32*)&mario->unk1AC[0];
        party->speed = 0.0f;
        party->jumpPara1 = 0.0f;
        party->useMotionId = 1;
        *(s32*)&party->field_22[2] = 6;
        partyChgPose(party, "PTR_A2_1");
        // fallthrough
    case 1:
        vivianUseStep(party);
        party->position.x += (party->targetPos.x - party->position.x) / (f32)*(s32*)&party->field_22[2];
        party->position.y += (party->targetPos.y - party->position.y) / (f32)*(s32*)&party->field_22[2];
        party->position.z += (party->targetPos.z - party->position.z) / (f32)*(s32*)&party->field_22[2];
        party->field_B8 = party->position;
        if (--*(s32*)&party->field_22[2] > 0) {
            break;
        }
        party->flags2 |= 0x200000;
        party->field_110 = *(f32*)&mario->unk1AC[0];
        party->field_FC = toMovedirSimple(party->field_110);
        party->field_B8 = party->position;
        mario->dispFlags |= 0x1000;
        partyChgPose(party, "PTR_A2_3");
        partyPaperOn(party, "p_bibi");
        partyChgPaper(party, "PM_B_1");
        party->dispOffset.y += 1.6f;
        mario->dispFlags |= 0x1000;
        marioPaperOn("p_bibi");
        marioChgPose("M_B_3");
        marioChgPaper("PM_B_1");
        party->useMotionId = 4;
        *(s32*)&party->field_22[2] = 0x1e;
        psndSFXOn_3D((const char*)0x936, &party->position);
        // fallthrough
    case 4:
        vivianUseStep(party);
        if (0x1e - *(s32*)&party->field_22[2] <= 0x18) {
            party->dispOffset.y += 0.4f;
            *(f32*)&mario->field_0x98[0x10] += 0.4f;
            if (party->dispOffset.y >= 8.0f) {
                party->dispOffset.y = 8.0f;
            }
            if (*(f32*)&mario->field_0x98[0x10] >= 8.0f) {
                *(f32*)&mario->field_0x98[0x10] = 8.0f;
            }
        }
        party->tail.vivian.charge += 1.0f;
        if (party->tail.vivian.charge >= 20.0f) {
            party->tail.vivian.charge = 20.0f;
            mario->dispFlags |= 0x1000;
        }
        if (--*(s32*)&party->field_22[2] > 0) {
            break;
        }
        {
            MarioWork* m = party->playerPtr;
            EffectEntry* eff = effRippleEntry(1, m->position.x, m->position.y + 1.5f, m->position.z);
            party->tail.vivian.ripple = eff;
            if (party->camId != 4) {
                effRippleSetCamId(eff, party->camId);
            }
        }
        psndSFXOn_3D((const char*)0x935, &party->position);
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            party->field_4D[2] |= 1;
        }
        party->useMotionId = 5;
        // fallthrough
    case 5:
        vivianUseStep(party);
        party->tail.vivian.charge -= 1.0f;
        if (party->tail.vivian.charge <= 10.0f) {
            party->tail.vivian.charge = 10.0f;
            party->useMotionId = 0xa;
            mario->flags |= 0x400;
        }
        break;
    case 10:
        vivianUseStep(party);
        partyUpdateKeyData(party);
        if (vivian_cancel_req) {
            party->useMotionId = 0x14;
        }
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE) && (party->buttonNew & 0x600)) {
            party->useMotionId = 0x14;
        }
        break;
    }

    mario->position.x += mario->riderDelta.x;
    mario->position.z += mario->riderDelta.z;
    mario->position.y += mario->riderDelta.y;
    if (party->useMotionId >= 2) {
        party->position = party->targetPos;
        party->field_B8 = mario->position;
    } else {
        party->field_B8 = party->position;
    }
    {
        f32 d = mario->position.y - *(f32*)&mario->unk104[0x10];
        if (d > 0.0f) {
            party->field_B8.y += d;
        }
    }
    {
        Vec v = party->field_B8;
        if (party->tail.vivian.ripple == NULL) {
            party->tail.vivian.ripple = NULL;
        } else if (*(u32*)party->tail.vivian.ripple == 0) {
            party->tail.vivian.ripple = NULL;
        } else {
            effRippleSetPosition(party->tail.vivian.ripple, v.x, v.y + 1.5f, v.z);
        }
    }

    switch (party->useMotionId - 0x14) {
    case 0:
        party->tail.vivian.moveSpeed = 0.0f;
        mario->flags &= ~0x400;
        psndSFXOn_3D((const char*)0x934, &party->position);
        party->useMotionId = 0x15;
        // fallthrough
    case 1:
        vivianUseStep(party);
        party->tail.vivian.charge += 1.0f;
        if (party->tail.vivian.charge >= 20.0f) {
            party->tail.vivian.charge = 20.0f;
            party->useMotionId = 0x16;
        }
        break;
    case 2:
        marioAdjustMoveDir();
        partyChgPose(party, "PTR_A2_4");
        party->field_20 = 0x18;
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            *(s8*)&party->field_4D[2] &= ~1;
        }
        party->useMotionId = 0x17;
        break;
    case 3:
        vivianUseStep(party);
        animPoseSetLocalTime(party->paperAnimGroup, 24.0f);
        party->field_20 = 0x18;
        party->useMotionId = 0x18;
        *(s32*)&party->field_22[2] = 0x18;
        psndSFXOn_3D((const char*)0x937, &party->position);
        // fallthrough
    case 4:
        vivianUseStep(party);
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(s32*)&party->field_22[2]);
        party->dispOffset.y -= 0.4f;
        if (party->dispOffset.y <= 2.7f) {
            party->dispOffset.y = 2.7f;
        }
        if (--*(s32*)&party->field_22[2] <= 0) {
            break;
        }
        if (*(s32*)&party->field_22[2] != 0x14) {
            break;
        }
        marioChgPose("M_S_1");
        mario->poseTime = 0x18;
        party->tail.vivian.poseTime = 0x18;
        party->useMotionId = 0x19;
        break;
    case 5:
        vivianUseStep(party);
        animPoseSetLocalTime(party->paperAnimGroup, (f32)*(s32*)&party->field_22[2]);
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)party->tail.vivian.poseTime);
        *(f32*)&mario->field_0x98[0x10] -= 0.4f;
        if (*(f32*)&mario->field_0x98[0x10] <= 2.7f) {
            *(f32*)&mario->field_0x98[0x10] = 2.7f;
        }
        party->dispOffset.y -= 0.4f;
        if (party->dispOffset.y <= 2.7f) {
            party->dispOffset.y = 2.7f;
        }
        party->tail.vivian.poseTime -= 1;
        if (--*(s32*)&party->field_22[2] > 0) {
            break;
        }
        party->useMotionId = 0x1A;
        *(s32*)&party->field_22[2] = 0x18;
        // fallthrough
    case 6:
        vivianUseStep(party);
        animPoseSetLocalTime(party->paperAnimGroup, 0.0f);
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)party->tail.vivian.poseTime);
        *(f32*)&mario->field_0x98[0x10] -= 0.4f;
        if (*(f32*)&mario->field_0x98[0x10] <= 2.7f) {
            *(f32*)&mario->field_0x98[0x10] = 2.7f;
        }
        party->dispOffset.y -= 0.4f;
        if (party->dispOffset.y <= 2.7f) {
            party->dispOffset.y = 2.7f;
        }
        {
            s16 t = party->tail.vivian.poseTime;
            t -= 1;
            party->tail.vivian.poseTime = t;
            if (t > 0) {
                party->tail.vivian.poseTime = 0;
            }
        }
        {
            s32 t = *(s32*)&party->field_22[2] - 1;
            *(s32*)&party->field_22[2] = t;
            if (t > 0) {
                *(s32*)&party->field_22[2] = 0;
            }
        }
        if (party->tail.vivian.poseTime != 0) {
            break;
        }
        if (*(s32*)&party->field_22[2] != 0) {
            break;
        }
        party->useMotionId = 0x1B;
        marioPaperOff();
        *(s32*)&mario->field_0x20[0] = 0;
        mario->dispFlags &= ~0x1000;
        mario->hitobjJumpFrom = mario->hitobjStandOn;
        mario->hitobjStandOn = NULL;
        *(Vec*)&mario->field_0x98[0xC] = (Vec){0.0f, 0.0f, 0.0f};
        marioChgSmallJumpMotion();
        mario->baseSpeed = 0.0f;
        break;
    case 7:
        animPoseSetLocalTime(party->paperAnimGroup, 0.0f);
        party->tail.vivian.charge -= 1.0f;
        if (party->tail.vivian.charge > 10.0f) {
            break;
        }
        party->tail.vivian.charge = 10.0f;
        partyPaperOff(party);
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        party->flags2 &= ~0x200000;
        party->tail.vivian.charge = 10.0f;
        party->dispOffset.y = 0.0f;
        partyChgRunMode(party, 2);
        party->field_3B = 0;
        partyChgPoseId(party, 1);
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        break;
    }
}
