#include "party/party_vivian.h"
#include "party.h"
#include "party/party_move.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_motion.h"
#include "mario/mariost.h"
#include "driver/animdrv.h"
#include "driver/hitdrv.h"
#include "system.h"
#include <string.h>

// .sbss
s32 vivian_cancel_req;

// .rodata (three contiguous zero vectors used for paper/effect positioning)
const Vec vec3_802f80a8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f80b4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802f80c0 = {0.0f, 0.0f, 0.0f};
const char str_802F80CC[] = "eki_02";
const char str_802F80D4[] = "PTR_A2_1";
const char str_802F80E0[] = "PTR_A2_3";
const char str_802F80EC[] = "p_bibi";
const char str_802F80F4[] = "PM_B_1";
const char str_802F80FC[] = "PTR_A2_4";
const f64 double_to_int_802f8108 = 4503601774854144.0;
const char str_802F8110[] = "bom_01";

// .sdata2
const f32 float_10_8041df00 = 10.0f;
const char str_8041DF34[] = "M_B_3";
const f32 float_0p1_8041df5c = 0.1f;
const f32 float_22_8041df60 = 22.0f;
const f32 float_neg1000_8041df64 = -1000.0f;

// opt_common_subs off: needed for the target FPR colouring (field_B8.x in f1, the 0.1 constant in f6).
// Without it the first loads and the constant land one register off.
#pragma opt_common_subs off
// NONMATCHING (99.71%): the two fmuls results differ: target my -> f3 and mz -> f1, ours my -> f1, mz -> f2.
// Temporaries avoid the `-fp_contract` fmadds fusion.
// 280 B
void vivian_move(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyMoveFlyInit(party, 1);
    }
    partyFlyMain(party);
    if (party != NULL && party->currentMemberId == 6) {
        f32 dx = party->position.x - party->field_B8.x;
        f32 dy = party->position.y - party->field_B8.y;
        f32 dz = party->position.z - party->field_B8.z;
        f32 mx = float_0p1_8041df5c * dx;
        f32 my = float_0p1_8041df5c * dy;
        f32 mz = float_0p1_8041df5c * dz;
        party->field_B8.x = party->field_B8.x + mx;
        party->field_B8.y = party->field_B8.y + my;
        party->field_B8.z = party->field_B8.z + mz;
        if (distABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z) >= float_22_8041df60) {
            party->field_B8 = party->position;
            movePos(&party->field_B8.x, &party->field_B8.z, float_22_8041df60, angleABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z));
        }
    }
}

// opt_common_subs off: needed for the target FPR colouring (field_B8.x in f1, the 0.1 constant in f6).
// 240 B
void vivianShadow(PartyEntry* party) {
    if (party != NULL && party->currentMemberId == 6) {
        f32 dx = party->position.x - party->field_B8.x;
        f32 dy = party->position.y - party->field_B8.y;
        f32 dz = party->position.z - party->field_B8.z;
        f32 mx = float_0p1_8041df5c * dx;
        f32 my = float_0p1_8041df5c * dy;
        f32 mz = float_0p1_8041df5c * dz;
        party->field_B8.x = party->field_B8.x + mx;
        party->field_B8.y = party->field_B8.y + my;
        party->field_B8.z = party->field_B8.z + mz;
        if (distABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z) >= float_22_8041df60) {
            party->field_B8 = party->position;
            movePos(&party->field_B8.x, &party->field_B8.z, float_22_8041df60, angleABf(party->position.x, party->position.z, party->field_B8.x, party->field_B8.z));
        }
    }
}
#pragma opt_common_subs reset

// 156 B
s32 N_vivianDisplayTailCheck(PartyEntry* party) {
    if (party->currentMemberId != 6) {
        return 0;
    }
    if (*(u8*)((u8*)party + 0x4F) & 1) {
        return 0;
    }
    if (party->flags2 & 0x800000) {
        return 0;
    }
    if (strcmp(gp->currentMapName, str_802F8110) == 0 && party->position.y == float_neg1000_8041df64) {
        return 0;
    }
    return 1;
}

// 28 B
void L_partyVivianTailStart(PartyEntry* party) {
    if (party->currentMemberId == 6) {
        *(s8*)((u8*)party + 0x4F) &= ~1;
    }
}

// 28 B
void L_partyVivianTailStop(PartyEntry* party) {
    if (party->currentMemberId == 6) {
        *(u8*)((u8*)party + 0x4F) |= 1;
    }
}

// NONMATCHING (93.00%): the flags&0x100 test emits `beqlr`; the target branches forward (`bne body; blr`).
// 60 B
void vivianUnhold(PartyEntry* party) {
    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != 6) {
        return;
    }
    if (party->flags & 0x100) {
        if (party->useMotionId == 0xa) {
            party->useMotionId = 0x14;
        }
    }
}

s32 vivianGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return 0;
    }
    if (party->currentMemberId != 6 || !(party->flags & 0x100)) {
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

// 84 B
void vivian_init_801809C0(PartyEntry* party) {
    party->field_33 = 6;
    party->field_32 = 1;
    *(f32*)((u8*)party + 0xF0) = 35.0f;
    *(f32*)((u8*)party + 0xF4) = 24.0f;
    *(f32*)((u8*)party + 0x174) = 10.0f;
    *(u8*)((u8*)party + 0x4F) = 0;
    *(Vec*)((u8*)party + 0xB8) = party->position;
    *(u32*)((u8*)party + 0x184) = 0;
    vivian_cancel_req = 0;
}

// NONMATCHING (85.41%): the Vec copies at 0xA4/0x7C hoist their first loads; the final `if` emits `beq` vs the
// target's `bne body; b`. A `makeZeroVec` literal helper reaches 98.71% but drops unit .rodata to 93.75% (withheld).
// 340 B
void mot_vivian_post(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == 0x20 || mario->currMotionId == 0x1f) {
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        *(Vec*)((u8*)mario + 0xA4) = vec3_802f80a8;
        mario->flags &= ~0x400;
        if (party != NULL) {
            partyPaperOff(party);
            party->flags &= ~0x100;
            party->flags &= ~0x80000000;
            party->flags2 &= ~0x200000;
            *(f32*)((u8*)party + 0x174) = float_10_8041df00;
            *(Vec*)((u8*)party + 0x7C) = vec3_802f80b4;
            partyChgRunMode(party, 2);
            party->field_3B = 0;
            partyChgPoseId(party, 1);
            if (party->currentMemberId == 6) {
                *(s8*)((u8*)party + 0x4F) &= ~1;
            }
        }
    } else if (party != NULL && party->currentMemberId == 6) {
        if (party->flags & 0x100) {
            vivian_cancel_req = 1;
        }
    }
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
        *(u32*)((u8*)mario + 0x1E0) = 0;
        *(u32*)((u8*)mario + 0x1DC) = 0;
        mario->hitobjPush = NULL;
        *(u32*)((u8*)mario + 0x1F0) = 0;
    }
}

// 152 B
void N_marioForceVivianAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(str_802F80EC);
    marioChgPose(str_8041DF34);
    marioChgPaper(str_802F80F4);
    mario->poseTime = 0x18;
    animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)*(u16*)((u8*)mario + 0x28));
}
