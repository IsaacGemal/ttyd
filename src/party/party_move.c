#include "party.h"
#include "mario/mario.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include <string.h>
#include <dolphin/mtx.h>
#include <math.h>
#include "party/party_move.h"
#include "party/party_hit.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "party/party_motion.h"
#include "party/party_cloud.h"
#include "party/party_vivian.h"
#include "party/party_nokonoko.h"
#include "motion/mot_damage.h"
#include "motion/mot_stay.h"
#include "motion/mot_slit.h"
#include "party/party_chuchu.h"
#include "party/party_bomhei.h"

// Local declarations for data that will eventually move to a header.
extern s32 readId[2];
extern s32 writeId[2];
extern u8 footmarkBuf[0xA00];


// Layout of a footmark record (0x10 bytes) inside footmarkBuf.
typedef struct Footmark {
    u8 field_0;
    u8 pad[3];
    f32 x;
    f32 y;
    f32 z;
} Footmark;


void partySetDir(PartyEntry* party, f32 x, f32 z) {
    PartyEntry* work = party;
    work->dir = angleABf(work->position.x, work->position.z, x, z);
}

void partyChgMot(PartyEntry* party, s32 mot) {
    PartyEntry* work = party;
    if (work->field_36 == 3) {
        work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
        work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
        work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    }
    work->field_36 = mot;
    work->field_8 |= 2;
}

// NONMATCHING (86.67%): the original keeps a redundant `cmpwi r0,4; bge` range test for the empty
// case 4/5 group; this compiler (GC/2.6) folds it. Cases above 6 emit `cmpwi r0,7` instead. Non-empty
// case 4/5 bodies make -O4,p duplicate the shared after-switch block instead of merging it.
void partyChgMoveMode(PartyEntry* party, u8 mode) {
    PartyEntry* work = party;
    s32 oldMode = work->field_3B;
    switch (oldMode) {
    case 4:
    case 5:
        break;
    case 6:
        work->flags &= ~0x8000;
        break;
    default:
        break;
    }
    work->field_3B = mode;
    work->field_8 |= 4;
}

// NONMATCHING (95.00%): the inlined partyChgMoveMode switch is missing the original's redundant
// `cmpwi r0,4; bge` range test for the empty case 4/5 group (same blocker as partyChgMoveMode).
void partyMoveBesideMario(PartyEntry* party, s32 mode) {
    if (party->currentMemberId == 4 && party->field_34 == 3) {
        return;
    }
    party->field_3C[1] = mode;
    partyChgMoveMode(party, 3);
    partyChgMot(party, 1);
}

// NONMATCHING (95.00%): same inlined partyChgMoveMode range-test mismatch as partyMoveBesideMario.
void partyMoveBehindMario(PartyEntry* party, s32 mode) {
    if (party->currentMemberId == 4 && party->field_34 == 3) {
        return;
    }
    party->field_3C[1] = mode;
    partyChgMoveMode(party, 2);
    partyChgMot(party, 1);
}

// NONMATCHING (96.67%): the original keeps a redundant `cmpwi r0,4; bge` range test for the empty
// case 4/5 group in the field_3B switch; this compiler folds it (same blocker as partyChgMoveMode).
void partyMoveBehindMario3(PartyEntry* party, s32 mode, f32 dir) {
    if (party->currentMemberId == 4 && party->field_34 == 3) {
        return;
    }
    party->field_8 |= 0x100000;
    party->field_8 |= 0x40000;
    party_set_behind_dir(party, dir);
    party->field_3C[1] = mode;
    switch (party->field_3B) {
    case 4:
    case 5:
        break;
    case 6:
        party->flags &= ~0x8000;
        break;
    }
    party->field_3B = 2;
    party->field_8 |= 0x4;
    if (party->field_36 == 3) {
        party->flags &= ~0x70;
        party->flags &= ~0x3000000;
        party->flags &= ~0x4000000;
    }
    party->field_36 = 1;
    party->field_8 |= 0x2;
}

void party_move_mode_beside_mario(PartyEntry* party) {
    PartyEntry* work = party;

    if (work->field_8 & 0x4) {
        work->field_8 &= ~0x4;
    }
    switch (work->field_36) {
    case 0:
    case 1:
        party_motion_beside_mario(party);
        break;
    case 2:
        party_motion_jump2(party);
        break;
    case 3:
        party_motion_fall(party);
        break;
    case 6:
        party_motion_upstairs(party);
        break;
    case 7:
        party_motion_damage(party);
        break;
    default:
        break;
    }
    work->field_FC = work->dir;
}

void party_move_mode_behind_mario(PartyEntry* party) {
    PartyEntry* work = party;

    if (work->field_8 & 0x4) {
        work->field_8 &= ~0x4;
    }
    switch (work->field_36) {
    case 0:
    case 1:
        if (work->field_32 == 0) {
            party_motion_behind_mario(party);
        } else {
            party_motion_behind_mario_fly(party);
        }
        break;
    case 2:
        party_motion_jump2(party);
        break;
    case 3:
        party_motion_fall(party);
        break;
    case 6:
        party_motion_upstairs(party);
        break;
    case 7:
        party_motion_damage(party);
        break;
    default:
        break;
    }
    work->field_FC = work->dir;
}

void party_move_mode_walk(PartyEntry* party) {
    PartyEntry* work = party;

    if (work->field_8 & 0x4) {
        work->field_8 &= ~0x4;
    }
    switch (work->field_36) {
    case 0:
        party_motion_stay(party);
        break;
    case 1:
        if (work->field_32 == 0) {
            party_motion_homing_walk(party);
        } else {
            party_motion_homing_fly(party);
        }
        break;
    case 2:
        party_motion_jump(party);
        break;
    case 3:
        party_motion_fall(party);
        break;
    case 6:
        party_motion_upstairs(party);
        break;
    case 7:
        party_motion_damage(party);
        break;
    default:
        break;
    }
    work->field_FC = work->dir;
    if (work->field_8 & 0x01000000) {
        work->field_8 &= ~0x01000000;
        {
            Vec pos = party->position;
            marioSetSplash(0, &pos);
        }
        work->field_4C = 0x14;
    }
    if (--work->field_4C < 0) {
        work->field_4C = 0;
    }
    if (work->field_34 != 0x0D) {
        if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        }
    }
}

void party_move_mode_stay(PartyEntry* party) {
    PartyEntry* work = party;

    if (work->field_8 & 0x4) {
        work->field_8 &= ~0x4;
        partyChgPoseId(party, 1);
        if (work->currentMemberId == 5) {
            unk_801820d0(party);
        }
    }
    switch (work->field_36) {
    case 0:
        party_motion_stay(party);
        if (work->currentMemberId == 5) {
            unk_8018206c(party);
        }
        break;
    case 1:
    case 2:
    case 6:
        if (work->field_36 == 3) {
            party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
            party->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
            party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        }
        work->field_36 = 0;
        work->field_8 |= 2;
        break;
    case 3:
        party_motion_fall(party);
        break;
    }
    work->field_FC = work->dir;
    if (work->field_8 & 0x01000000) {
        work->field_8 &= ~0x01000000;
        {
            Vec pos = party->position;
            marioSetSplash(0, &pos);
        }
        work->field_4C = 0x14;
    }
    if (--work->field_4C < 0) {
        work->field_4C = 0;
    }
    if (work->field_34 != 0x0D) {
        if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        }
    }
}

// NONMATCHING (99.29%): same instructions; only register numbers or operands differ.
void unk_800c9398(PartyEntry* party, f32 value) {
    PartyEntry* work = party;
    f32 gravity = work->playerPtr->playerGravity;
    f32 speed = value * gravity;
    f32 scale;
    if (work->flags & 0x80) {
        scale = 0.75f;
    } else {
        scale = 1.0f;
    }
    work->speed2 = speed * scale;
    work->speed = work->speed2;
}

void partyClearJumpPara(PartyEntry* party) {
    PartyEntry* work = party;
    work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
    work->jumpPara1 = 0.0f;
    work->jumpPara2 = 0.0f;
    work->jumpPara3 = 0.0f;
}

void partyMoveNoHosei(PartyEntry* party) {
    partyMoveMain(party, 0);
}

void partyMove(PartyEntry* party) {
    partyMoveMain(party, 1);
}

void N_partyPlaySfxPartyLanding1(PartyEntry* party) {
    psndSFXOn_3D((const char*)0x928, &party->position);
}

void partyGetReadId2(u32 index, Vec* out) {
    Footmark* footmark;
    u32 id = readId[index];
    footmark = (Footmark*)(footmarkBuf + index * 0x500) + id;
    out->x = footmark->x;
    out->y = footmark->y;
    out->z = footmark->z;
}

void partyGetReadId(u32 index, f32* x, f32* y, f32* z) {
    Footmark* footmark;
    u32 id = readId[index];
    footmark = (Footmark*)(footmarkBuf + index * 0x500) + id;
    *x = footmark->x;
    *y = footmark->y;
    *z = footmark->z;
}

void partyNextReadId(u32 index) {
    if (readId[index] != writeId[index]) {
        readId[index]++;
        if (readId[index] >= 0x50) {
            readId[index] = 0;
        }
    }
}

// Each clears one party slot's buffer: 0x50 records (0x500 bytes) of footmarkBuf.
#ifdef MUST_MATCH
// NONMATCHING (84.98%): the `u32` copy makes the compiler emit the original's `stw` stores; the
// pointer walk matches the `bdnz`/0x80-stride unroll, but this compiler still emits a redundant
// loop-entry test (`cmpwi`/`bge`) the original lacks.
void partyClearFootmark(void) {
    MarioWork* mario = marioGetPtr();
    Vec pos = mario->position;
    u32* src = (u32*)&pos;
    Footmark* f = (Footmark*)footmarkBuf;
    s32 i;

    for (i = 0; i < 0x50; i++) {
        ((u32*)&f->x)[0] = src[0];
        ((u32*)&f->x)[1] = src[1];
        ((u32*)&f->x)[2] = src[2];
        f->field_0 = 0;
        f++;
    }
    writeId[0] = 0;
    readId[0] = 0;
}
#else
void partyClearFootmark(void) {
    Vec pos = marioGetPtr()->position;
    s32 i;

    for (i = 0; i < 0x50; i++) {
        ((Footmark*)footmarkBuf)[i].field_0 = 0;
        ((Footmark*)footmarkBuf)[i].x = pos.x;
        ((Footmark*)footmarkBuf)[i].y = pos.y;
        ((Footmark*)footmarkBuf)[i].z = pos.z;
    }
    writeId[0] = 0;
    readId[0] = 0;
}
#endif

// dont_inline keeps this a real `bl` from partyRecordFootmark, as in retail.
#pragma dont_inline on
// NONMATCHING (87.37%): shares the clear loop's redundant loop-entry test with partyClearFootmark;
// the mario/target copy is otherwise matched.
void partyClearFootmark2(void) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target = partyGetPtr(marioGetPartyId());
    Vec pos;
#ifdef MUST_MATCH
    u32* src = (u32*)&pos;
    Footmark* f;
#endif
    s32 i;

    if (target == NULL || nokonokoGetStatus(target) == 3
        || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
        pos = mario->position;
    } else {
        pos = target->position;
    }
#ifdef MUST_MATCH
    // the u32 copy and pointer walk, as in partyClearFootmark
    f = (Footmark*)(footmarkBuf + 0x500);
    for (i = 0; i < 0x50; i++) {
        ((u32*)&f->x)[0] = src[0];
        ((u32*)&f->x)[1] = src[1];
        ((u32*)&f->x)[2] = src[2];
        f->field_0 = 0;
        f++;
    }
#else
    for (i = 0; i < 0x50; i++) {
        ((Footmark*)footmarkBuf)[0x50 + i].field_0 = 0;
        ((Footmark*)footmarkBuf)[0x50 + i].x = pos.x;
        ((Footmark*)footmarkBuf)[0x50 + i].y = pos.y;
        ((Footmark*)footmarkBuf)[0x50 + i].z = pos.z;
    }
#endif
    writeId[1] = 0;
    readId[1] = 0;
}
#pragma dont_inline reset

f32 unk_800c92c4(PartyEntry* party) {
    PartyEntry* work = party;
    MarioWork* mario;
    f32 f3;
    f32 f4;
    f32 f2;
    f32 f0;
    f32 f1;

    if (work->field_28 != 0 || work->field_36 == 2 || work->field_36 == 3 || work->field_36 == 6) {
        return work->speed;
    }
    mario = work->playerPtr;
    f3 = work->speed;
    if (mario->flags & 0x8000) {
        f4 = mario->baseSpeed;
    } else {
        f2 = work->speed2 * mario->playerGravity;
        if (work->flags & 0x80) {
            f0 = 0.75f;
        } else {
            f0 = 1.0f;
        }
        f4 = f2 * f0;
    }
    if (f4 > 0.0f) {
        f0 = 0.5f + f4 - f3;
        f1 = f0 * 0.05f + f3;
        if (f1 >= f4) {
            f1 = f4;
        }
        work->speed = f1;
        return f1;
    } else {
        f0 = -1.0f - f3;
        f1 = f0 * 0.1f + f3;
        if (f1 <= 0.0f) {
            f1 = 0.0f;
        }
        work->speed = f1;
        return f1;
    }
}

// NONMATCHING (97.46%): same instructions; only register numbers or operands differ.
void unk_800c93d0(PartyEntry* party, f32 value) {
    PartyEntry* work = party;
    MarioWork* mario;
    f32 f5;
    f32 f4;
    f32 f2;
    f32 f0;

    if (value != 0.0f) {
        work->speed2 = value;
        if (work->field_28 != 0) {
            return;
        }
        if (work->field_36 == 2) {
            return;
        }
        if (work->field_36 == 3) {
            return;
        }
        if (work->field_36 == 6) {
            return;
        }
        mario = work->playerPtr;
        f5 = work->speed;
        if (mario->flags & 0x8000) {
            f4 = mario->baseSpeed;
        } else {
            f2 = work->speed2 * mario->playerGravity;
            if (work->flags & 0x80) {
                f0 = 0.75f;
            } else {
                f0 = 1.0f;
            }
            f4 = f2 * f0;
        }
        if (f4 > 0.0f) {
            f0 = 0.5f + f4 - f5;
            f0 = f0 * 0.05f + f5;
            if (f0 >= f4) {
                f0 = f4;
            }
            work->speed = f0;
            return;
        } else {
            f0 = -1.0f - f5;
            f0 = f0 * 0.1f + f5;
            if (f0 <= 0.0f) {
                f0 = 0.0f;
            }
            work->speed = f0;
            return;
        }
    } else {
        work->speed2 = 0.0f;
    }
}

// Layout of a footmark record (0x10 bytes) inside footmarkBuf.

static const char* boxNames[] = {
    "MOBJ_TreasureBox",
    "MOBJ_BigTreasureBox",
    "MOBJ_GrayTreasureBox",
    "MOBJ_BlackTreasureBox",
    "MOBJ_GoldenTreasureBox",
    "MOBJ_BlueJumpStand",
    "MOBJ_BlueJumpStand2",
    "MOBJ_RedJumpStand",
    "MOBJ_RedJumpStand2",
    "MOBJ_MapJumpStand",
    "MOBJ_SaveBlock",
    "MOBJ_RecoveryBlock",
    "MOBJ_CoinTakenRecoveryBlock",
    "MOBJ_eki_06_su1",
    "MOBJ_eki_06_su2",
    "MOBJ_eki_06_su3",
    "MOBJ_RedSwitch",
    "MOBJ_BlueSwitch",
    "MOBJ_WhiteSwitch",
    "MOBJ_BlackSwitch",
    "MOBJ_green_FlowBlockSwitch",
    "MOBJ_purple_FlowBlockSwitch",
    "MOBJ_orange_FlowBlockSwitch",
    "MOBJ_RedTimerSwitch",
    "MOBJ_BlueTimerSwitch",
    NULL,
};

s32 unk_800c8dd0(HitEntry* hit) {
    s32 found;
    const char** name;
    u8* obj;

    if (hit == NULL) {
        return 0;
    }
    if ((hitGetAttr(hit) & 0x80000000) == 0) {
        return 0;
    }
    name = boxNames;
    obj = (u8*)mobjHitObjPtrToPtr(hit);
    found = 0;
    while (*name != NULL) {
        if (strcmp((const char*)(obj + 0x15), *name) == 0) {
            found = 1;
            break;
        }
        name++;
    }
    return found;
}

void unk_800c90d4(PartyEntry* party, s32 value) {
    PartyEntry* work = party;
    s32 lo;
    s32 hi;

    work->field_2D++;
    if (party->currentMemberId == 9) {
        if (value == 0) {
            lo = 0x14;
            hi = 0x28;
        } else {
            lo = 0x0A;
            hi = 0x14;
        }
        if (work->field_2D >= hi) {
            work->field_2D = 0;
            psndSFXOn_3D((const char*)0x60D, &party->position);
        } else if (work->field_2D == lo) {
            psndSFXOn_3D((const char*)0x60C, &party->position);
        }
    } else if (party->currentMemberId == 8) {
        if (work->field_2D >= 0x0F) {
            work->field_2D = 0;
            psndSFXOn_3D((const char*)0x942, &party->position);
        }
    } else if (party->currentMemberId == 10) {
        if (value == 0) {
            lo = 5;
            hi = 10;
        } else {
            lo = 5;
            hi = 10;
        }
        if (work->field_2D >= hi) {
            work->field_2D = 0;
            psndSFXOn_3D((const char*)0x3FF, &party->position);
        } else if (work->field_2D == lo) {
            psndSFXOn_3D((const char*)0x3FF, &party->position);
        }
    } else if (work->field_32 == 1) {
        if (work->field_2D >= (value == 0 ? 0x28 : 0x14)) {
            work->field_2D = 0;
            psndSFXOn_3D((const char*)0x926, &party->position);
        }
    } else {
        if (value == 0) {
            lo = 0x14;
            hi = 0x28;
        } else {
            lo = 0x0A;
            hi = 0x14;
        }
        if (work->field_2D >= hi) {
            work->field_2D = 0;
            psndSFXOn_3D((const char*)0x925, &party->position);
        } else if (work->field_2D == lo) {
            psndSFXOn_3D((const char*)0x924, &party->position);
        }
    }
}

void unk_800c8f24(PartyEntry* party, f32 f1) {
    PartyEntry* work = party;
    MarioWork* mario;
    f32 f2;
    f32 f3;

    if (party->flags2 & 0x100) {
        partyChgPoseId(party, 3);
        unk_800c90d4(party, 1);
        return;
    }
    mario = work->playerPtr;
    f3 = mario->playerGravity;
    f2 = f3 == 1.0f ? 1.65f : 2.0f;
    f3 = f3 == 1.0f ? 0.8f : 0.0f;
    if (f2 - f3 < f1 && f1 < f1 + f3) {
        if (N_marioGetMovementSpeed() == 1) {
            partyChgPoseId(party, 2);
            unk_800c90d4(party, 0);
        } else {
            partyChgPoseId(party, 3);
            unk_800c90d4(party, 1);
        }
    } else if (f1 >= f2) {
        partyChgPoseId(party, 3);
        unk_800c90d4(party, 1);
    } else {
        partyChgPoseId(party, 2);
        unk_800c90d4(party, 0);
    }
}

void unk_800c9044(PartyEntry* party) {
    PartyEntry* work = party;

    if (work->field_34 == 0) {
        return;
    }
    if (marioChkDeepSleep() == 1 || (party->flags & 0x400000)) {
        partyChgPoseId(party, 6);
    } else if (party->currentMemberId != 7) {
        partyChgPoseId(party, 1);
    } else if (chuchu_searchObject() == 0) {
        partyChgPoseId(party, 1);
    }
}

// Match note: without opt_propagation off, MWCC propagates the 270.0f literal and emits
// `fadds f0, f1, f0`; keeping the sum in a named temp makes it emit the original `fadds f0, f0, f1`.
#pragma opt_propagation off // keep the constant load scheduling so fadds takes its operands in the original order
void unk_800c8e74(PartyEntry* party) {
    PartyEntry* work = party;
    MarioWork* mario = work->playerPtr;
    f32 angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
#ifdef MUST_MATCH
    f32 viewYaw = cam->viewYaw;
    f32 sum = viewYaw + 270.0f;
    f32 result = revise360(sum - angle);
#else
    f32 result = revise360(cam->viewYaw + 270.0f - angle);
#endif
    s32 flag = 0;

    if (result >= 90.0f && result <= 270.0f) {
        flag = 1;
    }
    work->field_110 = flag ? 180.0f : 0.0f;
}
#pragma opt_propagation reset

void unk_800c7c98(PartyEntry* party) {
    PartyEntry* work = party;
    s32 mot;

    if (work->field_8 & 0x4) {
        work->field_8 &= ~0x4;
        partyChgPoseId(party, 1);
        party->flags &= ~0x100;
        if (party->currentMemberId == 5) {
            unk_801820d0(party);
        }
    }
    mot = work->field_36;
    switch (mot) {
    case 0:
        unk_8014bbc8(party);
        if (party->currentMemberId == 5) {
            unk_8018206c(party);
        }
        break;
    case 1:
    case 2:
    case 6:
        if (work->field_36 == 3) {
            party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
            party->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
            party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        }
        work->field_36 = 0;
        work->field_8 |= 2;
        break;
    case 3:
        party_motion_fall(party);
        break;
    }
    work->field_FC = work->dir;
    if (work->field_8 & 0x01000000) {
        work->field_8 &= ~0x01000000;
        {
            Vec pos = party->position;
            marioSetSplash(0, &pos);
        }
        work->field_4C = 0x14;
    }
    if (--work->field_4C < 0) {
        work->field_4C = 0;
    }
    if (work->field_34 != 0x0D) {
        if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
            partyChgRunMode(party, 0x0D);
        }
    }
    vivianShadow(party);
}

void partyMoveCheckWall(PartyEntry* party) {
    PartyEntry* work = party;
    Vec pos = party->position;
    f32 angle;
    f32 speed;
    void* wall;
    HitEntry* ground;

    partyGetMoveDirSpd(party, &speed, &angle);
    wall = partySearchFrontWall(party, &pos, angle, speed);
    if (wall != NULL) {
        work->field_12C = wall;
        if (work->speed) {
            work->field_134 = wall;
        }
    }
    party->position.x = pos.x;
    party->position.z = pos.z;
    ground = partySearchGround(party, 10.0f, 10.0f);
    pos.y = work->field_E4;
    if (ground != NULL) {
        if (hitGetAttr(ground) & 0x200) {
            ground = NULL;
        }
    }
    work->field_138 = ground;
    if (ground == NULL) {
        party->position.y = party->position.y + work->jumpPara1;
        if (work->jumpPara2 == 0.0f) {
            work->jumpPara2 = -1.4f;
        }
        work->jumpPara1 = work->jumpPara1 + work->jumpPara2;
    }
}

void partyChkWall(PartyEntry* party) {
    PartyEntry* work = party;
    Vec pos = party->position;
    f32 angle;
    f32 speed;
    void* wall;

    work->speed = 0.0f;
    partyGetMoveDirSpd(party, &speed, &angle);
    wall = partySearchFrontWall(party, &pos, angle, speed);
    if (wall != NULL) {
        work->field_12C = wall;
        if (work->speed) {
            work->field_134 = wall;
        }
    }
    party->position.x = pos.x;
    party->position.z = pos.z;
}

s32 partyMarioHosei(PartyEntry* party) {
    PartyEntry* work = party;
    MarioWork* mario = work->playerPtr;
    f32 f4;
    f32 f5;
    f32 f6;
    f32 f3;
    f32 f1;
    f32 angle;
    f32 f31;
    f32 f30;

    if (!(work->field_8 & 0x40000)) {
        f4 = 0.4f;
        f5 = 0.3f;
        f6 = f4;
    } else {
        f4 = 0.2f;
        f5 = 0.15f;
        f6 = f4;
    }
    if (mario->flags & 0x8000) {
        f4 = 0.2f;
        f5 = 0.15f;
        f6 = f4;
    }
    f3 = *(f32*)((u8*)work + 0xF4) + mario->wPlayerCollisionBox.x;
    f1 = mario->wPlayerCollisionBox.y + *(f32*)((u8*)work + 0xF0);
    *(f32*)((u8*)work + 0xA0) = 0.0f;
    f4 = f4 * f3;
    f5 = f5 * f3;
    *(f32*)((u8*)work + 0xA8) = 0.0f;
    f6 = f6 * f1;
    if (fabsf(work->position.x - mario->position.x) <= f4
        && fabsf(work->position.z - mario->position.z) <= f5
        && fabsf(work->position.y - mario->position.y) <= f6) {
        angle = angleABf(mario->position.x, mario->position.z, work->position.x, work->position.z);
        if (mario->baseSpeed) {
            if (fabsf(angle - mario->directionView) <= 4.0f) {
                if (angle - mario->directionView >= 0.0f) {
                    angle += 90.0f;
                } else {
                    angle -= 90.0f;
                }
            }
            if (++*(s16*)((u8*)work + 0x52) > 100) {
                *(s16*)((u8*)work + 0x52) = 100;
            }
            f30 = 1.0f;
        } else {
            angle = angleABf(mario->position.x, mario->position.z, work->position.x, work->position.z);
            f30 = 4.0f;
        }
        if (mario->baseSpeed > 1.0f) {
            f30 = 0.8f * mario->baseSpeed;
        }
        f31 = 3.1415927f * angle / 180.0f;
        *(f32*)((u8*)work + 0xA0) = f30 * (f32)sin(f31);
        *(f32*)((u8*)work + 0xA8) = f30 * -(f32)cos(f31);
    } else if (*(s16*)((u8*)work + 0x52) > 0) {
        *(s16*)((u8*)work + 0x52) -= 1;
    }
    return *(s16*)((u8*)work + 0x52);
}

// NONMATCHING (95.62%): compiler CSEs `id != 0` into r29 across the marioGetPtr calls, so `id`
// lands in r30 instead of r29 and the second anotherPartyGetPtr reuses the value instead of
// recomputing it (`srwi r3` vs `mr r3,r29`).
f32 unk_800c9f48(s32 id, f32 y) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    PartyEntry* target;
    PartyEntry* target2;
    s32 flag = 0;

    target = anotherPartyGetPtr(id != 0);
    if (id == 0) {
        return fabsf(mario->position.y - y);
    }
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        f32 v;
        mario2 = marioGetPtr();
        flag = 0;
        mario = marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario->position, &target2->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (flag) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            v = target2->position.y;
        } else {
            v = mario2->position.y;
        }
        return fabsf(v - y);
    }
    return fabsf(mario->position.y - y);
}

f32 partyGetTargetDistY(s32 id) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target;
    s32 flag;

    if (id == 0) {
        if (mario->baseSpeed > 0.0f) {
            return mario->baseSpeed;
        }
        return mario->mBaseDashSpeed;
    }
    flag = 0;
    target = anotherPartyGetPtr(id != 0);
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        return (target)->speed;
    }
    mario = marioGetPtr();
    if (mario->baseSpeed > 0.0f) {
        return mario->baseSpeed;
    }
    return mario->mBaseDashSpeed;
}

f32 partyGetTargetMovespd2(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (!(party->flags & 0x8)) {
        if (mario->baseSpeed > 0.0f) {
            return mario->baseSpeed;
        }
        return mario->mBaseDashSpeed;
    }
    if (party->field_8 & 0x20000) {
        if (mario->baseSpeed > 0.0f) {
            return mario->baseSpeed;
        }
        return mario->mBaseDashSpeed;
    }
    {
        PartyEntry* target;
        s32 flag = 0;

        target = partyGetPtr(marioGetPartyId());
        if (target != NULL) {
            if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (target != NULL && flag == 0) {
            return target->speed;
        }
    }
        if (mario->baseSpeed > 0.0f) {
            return mario->baseSpeed;
        }
        return mario->mBaseDashSpeed;
}

// NONMATCHING (95.77%): same `id != 0` CSE/register allocation diff as unk_800c9f48.
f32 partyGetTargetDist3D(s32 id, Vec* pos) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    PartyEntry* target;
    PartyEntry* target2;
    s32 flag = 0;

    target = anotherPartyGetPtr(id != 0);
    if (id == 0) {
        return PSVECDistance(&marioGetPtr()->position, pos);
    }
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        Vec local;
        mario2 = marioGetPtr();
        flag = 0;
        mario = marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario->position, &target2->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (flag) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            local.x = target2->position.x;
            local.y = target2->position.y;
            local.z = target2->position.z;
        } else {
            local.x = mario2->position.x;
            local.y = mario2->position.y;
            local.z = mario2->position.z;
        }
        return PSVECDistance(&local, pos);
    }
    return PSVECDistance(&marioGetPtr()->position, pos);
}

// NONMATCHING (96.15%): same `id != 0` CSE/register allocation diff as unk_800c9f48.
f32 partyGetTargetDist(s32 id, f32 x, f32 z) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    PartyEntry* target;
    PartyEntry* target2;
    s32 flag = 0;

    target = anotherPartyGetPtr(id != 0);
    if (id == 0) {
        return distABf(x, z, mario->position.x, mario->position.z);
    }
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        f32 vx;
        f32 vz;
        mario2 = marioGetPtr();
        flag = 0;
        mario = marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario->position, &target2->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (flag) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            vx = target2->position.x;
            vz = target2->position.z;
        } else {
            vx = mario2->position.x;
            vz = mario2->position.z;
        }
        return distABf(x, z, vx, vz);
    }
    return distABf(x, z, mario->position.x, mario->position.z);
}

// NONMATCHING (95.82%): same `id != 0` CSE/register allocation diff as unk_800c9f48.
f32 partyGetTargetDir(s32 id, f32 x, f32 z) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    PartyEntry* target;
    PartyEntry* target2;
    s32 flag = 0;

    target = anotherPartyGetPtr(id != 0);
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        f32 vx;
        f32 vz;
        mario2 = marioGetPtr();
        flag = 0;
        mario = marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario->position, &target2->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (flag) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            vx = target2->position.x;
            vz = target2->position.z;
        } else {
            vx = mario2->position.x;
            vz = mario2->position.z;
        }
        return angleABf(x, z, vx, vz);
    }
    return angleABf(x, z, mario->position.x, mario->position.z);
}

void L_getTargetPos(s32 id, Vec* out) {
    MarioWork* mario = marioGetPtr();
    MarioWork* mario2;
    PartyEntry* target;
    s32 flag = 0;

    mario2 = marioGetPtr();
    target = anotherPartyGetPtr(id != 0);
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario2->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (flag) {
        target = NULL;
    }
    if (target != NULL) {
        out->x = target->position.x;
        out->y = target->position.y;
        out->z = target->position.z;
    } else {
        out->x = mario->position.x;
        out->y = mario->position.y;
        out->z = mario->position.z;
    }
}

// Layout of a footmark record (0x10 bytes) inside footmarkBuf.

u32 partyGetFootmarkId(u32* flags) {
    return (*flags >> 2) & 1;
}

s32 partyGetFootmarkWriteId(u32 index) {
    return writeId[index];
}

s32 partyGetFootmarkReadId(u32 index) {
    return readId[index];
}

Footmark* partyGetFootmarkPtr(u32 index) {
    u32 id = readId[index];
    return (Footmark*)(footmarkBuf + index * 0x500) + id;
}

// HEADER PROBLEM: the asm returns BOOL (`li r3,0x1` / `li r3,0x0` before both epilogues,
// e.g. 800C3D18 and 800C407C), but include/party/party_move.h:32 declares this function `void`
// and bulk agents may not change existing header lines. The definition is therefore `void` and
// the eight `li r3` return-value instructions are missing; the header should become
// `BOOL partyGetAppearPos2(PartyEntry* party, Vec* pos);` (the only caller, party.c:1105,
// ignores the result).
// The compiler also pools this function's Vec compound-literal templates into per-literal local
// .rodata symbols (@752/@754/...) instead of the original's str_802CDB20+0x38c... blob, so it
// keeps only three callee-saved GPRs (r29-r31) instead of four (r28-r31). All other opcodes,
// stack offsets and control flow match.
// NONMATCHING (71.95%): wrong header return type + pool-layout blocker.
void partyGetAppearPos2(PartyEntry* party, Vec* position) {
    MarioWork* player = party->playerPtr;
    HitCheckQuery query;
    f32 unk1A8;
    f32 targetDist = 30.0f + party->field_F4;
    f32 moveDist = 30.0f;
    f32 angle;
    f32 sinv;
    f32 cosv;
    HitEntry* hit;

    revise360(180.0f + player->unk1A8);
    unk1A8 = player->unk1A8;
    position->x = player->position.x;
    position->y = player->position.y;
    position->z = player->position.z;

    if (player->currPartySlotId[1] < 0) {
        angle = toMovedir(180.0f + player->unk1A8);
        sincosf(&sinv, &cosv, angle);
        query.targetDistance = targetDist;
        query.targetPos = (Vec){position->x, position->y + 37.0f, position->z};
        query.targetDir = (Vec){sinv, 0.0f, cosv};
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL) {
            return;
        }

        angle = toMovedir(player->unk1A8);
        movePos(&position->x, &position->z, -moveDist, angle);
        query.targetDistance = 38.0f;
        query.targetPos = (Vec){position->x, position->y + 37.0f, position->z};
        query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
        hit = hitCheckVecFilter(&query, NULL);
        if (hit == NULL) {
            return;
        }
        position->y = query.hitPos.y;
        return;
    }

    position->x = player->position.x;
    position->y = player->position.y;
    position->z = player->position.z;
    angle = toMovedir2(unk1A8, -90.0f);
    sincosf(&sinv, &cosv, angle);
    query.targetDistance = targetDist;
    query.targetPos = (Vec){player->position.x, player->position.y + 37.0f, player->position.z};
    query.targetDir = (Vec){sinv, 0.0f, cosv};
    hit = hitCheckVecFilter(&query, NULL);
    if (hit == NULL) {
        position->x = player->position.x;
        position->y = player->position.y;
        position->z = player->position.z;
        angle = toMovedir2(unk1A8, party->currentSlotId == 0 ? 20.0f : -20.0f);
        movePos(&position->x, &position->z, -moveDist, angle);
        query.targetDistance = 38.0f;
        query.targetPos = (Vec){position->x, position->y + 37.0f, position->z};
        query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL) {
            position->y = query.hitPos.y;
            return;
        }
    }

    angle = toMovedir2(unk1A8, 90.0f);
    sincosf(&sinv, &cosv, angle);
    query.targetDistance = targetDist;
    query.targetPos = (Vec){player->position.x, player->position.y + 37.0f, player->position.z};
    query.targetDir = (Vec){sinv, 0.0f, cosv};
    hit = hitCheckVecFilter(&query, NULL);
    if (hit != NULL) {
        return;
    }

    position->x = player->position.x;
    position->y = player->position.y;
    position->z = player->position.z;
    angle = toMovedir2(unk1A8, party->currentSlotId == 0 ? -20.0f : 20.0f);
    movePos(&position->x, &position->z, -moveDist, angle);
    query.targetDistance = 38.0f;
    query.targetPos = (Vec){position->x, position->y + 37.0f, position->z};
    query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    hit = hitCheckVecFilter(&query, NULL);
    if (hit == NULL) {
        return;
    }
    position->y = query.hitPos.y;
    return;
}

// The compiler does not pool the .rodata constants for this function, so it keeps two
// pool-base registers live across the loop instead of the original's one. That shifts
// every callee-saved register by one (stmw r20 vs r21) and leaves the four Vec/angle
// literal offsets (@749/@755/@757/@761/@763) unnamed instead of
// str_802CDB20+0x41c/0x440/0x44c/0x458/0x464. All opcodes and stack offsets otherwise match.
// NONMATCHING (89.61%): one extra callee-saved register and unpooled constant relocations.
BOOL partyGetAppearPos3(PartyEntry* party, Vec* position) {
    MarioWork* player = party->playerPtr;
    f32 unk1AC = *(f32*)((u8*)player + 0x1AC);
    f32 targetDist = 30.0f + *(f32*)((u8*)party + 0xF4);
    HitCheckQuery query;
    s32 angles[9] = {45, 135, -135, 315, -315, 180, -180, 90, -90};
    HitEntry* hit;
    NpcEntry* npc;
    f32 bestX;
    f32 bestY;
    f32 bestZ;
    f32 bestDist = -1.0f;
    f32 sinv;
    f32 cosv;
    Vec hitNormal;
    f32 angle;
    u32 i;

    position->x = player->position.x;
    position->y = player->position.y;
    position->z = player->position.z;

    for (i = 0; i < 9; i++) {
        angle = toMovedir2(unk1AC, (f32)angles[i]);
        sincosf(&sinv, &cosv, angle);
        query.targetDistance = targetDist;
        query.targetPos = (Vec){player->position.x, player->position.y + 37.0f, player->position.z};
        query.targetDir = (Vec){sinv, 0.0f, cosv};
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL) {
            continue;
        }

        position->x = player->position.x;
        position->y = player->position.y;
        position->z = player->position.z;
        movePos(&position->x, &position->z, 30.0f, angle);

        query.targetDistance = 87.0f;
        query.targetPos = (Vec){position->x, position->y + 37.0f, position->z};
        query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
        hit = hitCheckVecFilter(&query, NULL);
        if (hit == NULL) {
            continue;
        }
        if (unk_800c8dd0(hit) != 0) {
            continue;
        }
        {
            hitNormal = query.hitNormal;
            if (partyNrmToAngle(&hitNormal) > 60.0f) {
                continue;
            }
        }
        {
            BOOL proceed;
            if (hit == NULL) {
                proceed = FALSE;
            } else if ((hitGetAttr(hit) & 0xA00) != 0) {
                proceed = FALSE;
            } else if (unk_800c8dd0(hit) != 0) {
                proceed = FALSE;
            } else {
                proceed = TRUE;
            }
            if (!proceed) {
                continue;
            }
        }

        npc = npcNearDistCheck(position->x, position->y, position->z, *(f32*)((u8*)party + 0xF4));
        if (npc != NULL) {
            if (bestDist < PSVECDistance(&npc->position, position)) {
                bestDist = PSVECDistance(&npc->position, position);
                bestX = position->x;
                bestY = position->y;
                bestZ = position->z;
            }
        } else {
            position->y = query.hitPos.y;
            marioChkWallAround(position, 1, 2.0f, 0.0f, *(f32*)((u8*)party + 0xF4),
                               *(f32*)((u8*)party + 0xF0));
            return TRUE;
        }
    }
    if (bestDist >= 0.0f) {
        position->x = bestX;
        position->y = bestY;
        position->z = bestZ;
        return TRUE;
    }
    return FALSE;
}

// The angle table and the two Vec templates land in this file's .rodata pool, but the original
// pools them at str_802CDB20+0x5a4 / +0x5d4 / +0x5f0 and addresses angles, templates and the
// 2^52 int-to-float double off that one base register. Because the rest of party_move.c is still
// unwritten, our pool offsets differ and the compiler keeps the angle base and the template base
// in separate registers, so every callee-saved GPR is assigned differently (same stmw r25 set,
// different roles). The stack frame, opcodes and control flow match exactly: `u32 i` for the
// `cmplwi` loop test, component-wise `position->x/y/z = pos.x/y/z` for the lfs/stfs copy, and
// `if (i >= 12)` for the post-loop `cmplwi; blt`.
// NONMATCHING (85.22%): pool-layout blocker only; opcodes and stack shape match.
BOOL partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* position) {
    s32 angles[12] = {0, 30, -30, 60, -60, 90, -90, 120, -120, 150, -150, 180};
    MapObjectEntry* obj;
    Vec pos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    f32 dist;
    f32 sinv;
    f32 cosv;
    HitEntry* h;
    u32 i;

    if (hit == NULL) {
        return FALSE;
    }
    if ((hitGetAttr(hit) & 0x80000000) == 0) {
        return FALSE;
    }
    obj = mobjHitObjPtrToPtr(hit);
    pos.x = obj->position.x;
    pos.z = obj->position.z;
    pos.y = 50.0f + obj->position.y;
    for (i = 0; i < 12; i++) {
        sincosf(&sinv, &cosv, party->dir + (f32)angles[i]);
        dir = (Vec){sinv, 0.0f, cosv};
        dist = 50.0f;
        h = partyHitCheck(party, &pos, &dir, &hitPos, &hitNormal, &dist);
        if (h == NULL) {
            dist = 150.0f;
            pos.x = obj->position.x + sinv * 50.0f;
            pos.z = obj->position.z + cosv * 50.0f;
            dir = (Vec){0.0f, -1.0f, 0.0f};
            h = partyHitCheck(party, &pos, &dir, &hitPos, &hitNormal, &dist);
            if (h == NULL || !(hitGetAttr(h) & 0x200)) {
                break;
            }
        }
    }
    if (i >= 12) {
        return FALSE;
    }
    position->x = pos.x;
    position->y = pos.y;
    position->z = pos.z;
    return TRUE;
}

// NONMATCHING (91.17%): footmark clear inherits partyClearFootmark's redundant loop-entry test;
// the inlined partyChgMoveMode switch also drops the original's `cmpwi 4; bge`.
void partyWalkInit(PartyEntry* party, s32 mode) {
    PartyEntry* work = party;
    MarioWork* mario = work->playerPtr;
    s32 i;

    work->flags |= 2;
    if (!(work->flags & 0x8)) {
        Vec pos = marioGetPtr()->position;
        u32* src = (u32*)&pos;
        Footmark* f = (Footmark*)footmarkBuf;
        for (i = 0; i < 0x50; i++) {
            ((u32*)&f->x)[0] = src[0];
            ((u32*)&f->x)[1] = src[1];
            ((u32*)&f->x)[2] = src[2];
            f->field_0 = 0;
            f++;
        }
        writeId[0] = 0;
        readId[0] = 0;
    } else {
        PartyEntry* target;
        Vec pos;
        u32* src = (u32*)&pos;
        Footmark* f;
        MarioWork* player = marioGetPtr();
        target = partyGetPtr(marioGetPartyId());
        if (target != NULL) {
            if (nokonokoGetStatus(target) == 3
                || PSVECDistance(&player->position, &target->position) >= 150.0f) {
                pos = player->position;
            } else {
                pos = target->position;
            }
        } else {
            pos = player->position;
        }
        f = (Footmark*)(footmarkBuf + 0x500);
        for (i = 0; i < 0x50; i++) {
            ((u32*)&f->x)[0] = src[0];
            ((u32*)&f->x)[1] = src[1];
            ((u32*)&f->x)[2] = src[2];
            f->field_0 = 0;
            f++;
        }
        writeId[1] = 0;
        readId[1] = 0;
    }
    *(u8*)((u8*)work + 0x3D) = 0;
    *(u32*)((u8*)work + 0x44) = 0x10;
    *(f32*)((u8*)work + 0x40) = 50.0f;
    *(u8*)((u8*)work + 0x3E) = 0;
    work->jumpPara2 = 1.8f;
    work->jumpPara1 = 0.0f;
    if (!(mario->flags & 0x400000)) {
        partyChgPoseId(party, 1);
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
    }
}

// NONMATCHING (99.74%): all opcodes match; the inlined unk_800c8f24 body allocates the
// gravity value to f1 instead of f3, so 17 operands show as fp-register `arg_mismatch`.
void partyForceWalk(PartyEntry* party) {
    PartyEntry* work = party;
    Vec pos;
    HitEntry* ground;
    HitEntry* wall;
    s32 attr;

    if (work->field_8 & 0x8) {
        work->field_8 &= ~0x8;
        if (work->flags & 0x100) {
            if (work->currentMemberId == 3) {
                bomhei_use_cancel();
            }
            if (work->currentMemberId == 2) {
                nokonoko_finish(work);
            }
            work->flags &= ~0x100;
        }
    }
    switch (work->field_36) {
    case 0:
    case 1:
        if (work->field_28 <= 0) {
            work->field_28 = 0;
            unk_800ba2dc(work);
        } else {
            {
                f32 spd = work->speed;
                unk_800c8f24(work, spd);
            }
            pos = work->position;
            wall = partySearchFrontWall(work, &pos, work->speed, work->dir);
            if (wall != NULL) {
                if (work->field_36 == 3) {
                    work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                    work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
                    work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
                }
                work->field_36 = 2;
                work->field_8 |= 2;
            } else {
                work->position.x = pos.x;
                work->position.z = pos.z;
            }
            if (--work->field_28 <= 0) {
                work->field_28 = 0;
                unk_800ba2dc(work);
            } else {
                work->flags &= ~0x80;
                ground = partySearchGround(work, 10.0f, 10.0f);
                attr = 0;
                if (ground != NULL) {
                    attr = hitGetAttr(ground);
                    if (attr & 0x200) {
                        ground = NULL;
                    }
                }
                work->field_138 = ground;
                if (ground == NULL) {
                    if (work->field_36 == 3) {
                        work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                        work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
                        work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
                    }
                    work->field_36 = 3;
                    work->field_8 |= 2;
                    work->jumpPara2 = -1.0f;
                } else {
                    if (attr & 0x100) {
                        work->flags |= 0x80;
                    } else {
                        work->flags &= ~0x80;
                    }
                }
                work->position.y = work->field_E4;
            }
        }
        break;
    case 2:
        if (--work->field_28 <= 0) {
            work->field_28 = 0;
        }
        party_motion_jump(work);
        break;
    case 3:
        if (--work->field_28 <= 0) {
            work->field_28 = 0;
        }
        party_motion_fall(work);
        break;
    default:
        break;
    }
    work->field_FC = work->dir;
    if (work->currentMemberId == 6) {
        vivianShadow(work);
    }
}

extern int __float_nan[];

static inline s32 partyFpClassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
    case 0x7F800000:
        if (*(s32*)&x & 0x007FFFFF) {
            return 1;
        } else {
            return 2;
        }
        break;
    case 0:
        if (*(s32*)&x & 0x007FFFFF) {
            return 5;
        } else {
            return 3;
        }
        break;
    default:
        return 4;
    }
}

static inline f32 partySqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return *(f32*)__float_nan;
    } else if (partyFpClassifyf(x) == 1) {
        return *(f32*)__float_nan;
    }
    return x;
}

// NONMATCHING (89.86%): footmark clear inherits partyClearFootmark's redundant loop-entry test;
// the inlined partyChgMoveMode switch drops `cmpwi 4; bge`; the inline sqrt double pool lands
// in a fresh @991 instead of str_802CDB20+0x600 and the length/scale fp regs are swapped.
void partyGetMoveDirSpd(PartyEntry* party, f32* angle, f32* speed) {
    PartyEntry* work = party;
    MarioWork* mario = work->playerPtr;
    s32 hosei = partyMarioHosei(work);
    f32 sinv;
    f32 cosv;
    f32 f31;
    f32 f30;
    f32 scale;

    if (hosei != 0 && work->field_3B != 2) {
        if (!(work->flags & 0x8)) {
            partyClearFootmark();
        } else {
            s32 flag = 0;
            PartyEntry* partner = anotherPartyGetPtr(1);

            if (partner != NULL) {
                if (nokonokoGetStatus(partner) == 3
                    || PSVECDistance(&mario->position, &partner->position) >= 150.0f) {
                    flag = 1;
                }
            }
            if (flag != 0) {
                MarioWork* m2 = marioGetPtr();
                PartyEntry* target = partyGetPtr(marioGetPartyId());
                Vec pos;
                u32* src;
                Footmark* f;
                s32 i;

                if (target == NULL || nokonokoGetStatus(target) == 3
                    || PSVECDistance(&m2->position, &target->position) >= 150.0f) {
                    pos = m2->position;
                } else {
                    pos = target->position;
                }
                src = (u32*)&pos;
                f = (Footmark*)(footmarkBuf + 0x500);
                for (i = 0; i < 0x50; i++) {
                    ((u32*)&f->x)[0] = src[0];
                    ((u32*)&f->x)[1] = src[1];
                    ((u32*)&f->x)[2] = src[2];
                    f->field_0 = 0;
                    f++;
                }
                writeId[1] = 0;
                readId[1] = 0;
            }
        }
    }
    if (work->field_8 & 0x200000) {
        hosei = 0;
    }
    if (hosei >= 0x1E) {
        *(s16*)((u8*)work + 0x52) = 0;
        if (!(work->currentMemberId == 4 && work->field_34 == 3)) {
            work->field_3C[1] = 0;
            {
                s32 oldMode = work->field_3B;
                switch (oldMode) {
                case 4:
                case 5:
                    break;
                case 6:
                    work->flags &= ~0x8000;
                    break;
                default:
                    break;
                }
            }
            work->field_3B = 2;
            work->field_8 |= 4;
            if (work->field_36 == 3) {
                work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
                work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            }
            work->field_36 = 1;
            work->field_8 |= 2;
        }
    }
    sincosf(&sinv, &cosv, work->dir);
    f31 = work->speed * sinv + (*(f32*)((u8*)work + 0xAC) + *(f32*)((u8*)work + 0xA0));
    f30 = work->speed * cosv + (*(f32*)((u8*)work + 0xB4) + *(f32*)((u8*)work + 0xA8));
    *angle = angleABf(0.0f, 0.0f, f31, f30);
    if (work->flags & 0x80) {
        scale = 0.75f;
    } else {
        scale = 1.0f;
    }
    *speed = scale * partySqrtf(f31 * f31 + f30 * f30);
}

// partyMoveMain returns a state code in r3.
#ifdef MUST_MATCH
// Match shape: for these zero compares the original loads the field into f1 and 0.0f into f0;
// comparing against a named zero makes this compiler allocate the same registers.
#define PARTY_MOVE_ZERO zero
#else
#define PARTY_MOVE_ZERO 0.0f
#endif
s32 partyMoveMain(PartyEntry* party, s32 hosei) {
    PartyEntry* work = party;
    Vec pos;
    f32 speed;
    f32 angle;
#ifdef MUST_MATCH
    f32 zero;
#endif
    HitEntry* hit;

    work->field_12C = NULL;
    work->field_134 = NULL;
    pos = party->position;
    speed = 0.0f;
#ifdef MUST_MATCH
    zero = 0.0f;
#endif

    if (hosei != 0) {
        partyGetMoveDirSpd(party, &angle, &speed);
    } else {
        angle = work->dir;
        speed = work->speed * ((party->flags & PARTY_FLAG_IN_SHALLOW_WATER) ? 0.75f : 1.0f);
    }
    if (!(party->flags & 0x01000000)) {
        hit = partySearchFrontWall(party, &pos, speed, angle);
        if (hit != NULL) {
            work->field_12C = hit;
            if (work->speed != PARTY_MOVE_ZERO) {
                work->field_134 = hit;
            }
        }
        if (hit != NULL) {
            party->position.x = pos.x;
            party->position.z = pos.z;
        } else {
            marioChkWallAround(&pos, 1, speed, angle, *(f32*)((u8*)work + 0xF4), *(f32*)((u8*)work + 0xF0));
            party->position.x = pos.x;
            party->position.z = pos.z;
        }
    } else {
        movePos(&party->position.x, &party->position.z, speed, angle);
    }
    if (!(party->flags & 0x02000000)) {
        Vec headPos = party->position;
        f32 headOut = *(f32*)((u8*)work + 0xF0);
        hit = partySearchHead(party, &headPos, &headOut, work->field_10C);
        if (hit != NULL) {
            work->field_140 = hit;
        }
    }
    if (party->flags & PARTY_FLAG_IS_JUMPING) {
        angle = toMovedirSimple(work->field_110);
        if (work->field_140 == NULL) {
            work->position.y += work->jumpPara1;
        }
        work->jumpPara1 += work->jumpPara2;
        party->flags &= ~PARTY_FLAG_IS_GROUNDED;
        return 1;
    }
    hit = NULL;
    if (!(party->flags & 0x02000000) && work->jumpPara1 <= 0.0f) {
        f32 d = fabsf(work->jumpPara1);
        hit = partySearchGround(party, d, d);
        pos.y = work->field_E4;
        if (hit != NULL) {
            s32 attr = hitGetAttr(hit);
            if (attr & 0x200) {
                work->field_138 = hit;
                party->flags &= ~PARTY_FLAG_IS_GROUNDED;
                if (!(party->flags & PARTY_FLAG_IS_FALLING)) {
                    party->flags |= PARTY_FLAG_IS_FALLING;
                    if (work->jumpPara2 == PARTY_MOVE_ZERO) {
                        work->jumpPara2 = -1.4f;
                    }
                }
                party->position.y += work->jumpPara1;
                work->jumpPara1 += work->jumpPara2;
                return 2;
            }
            if (*(f32*)((u8*)work + 0xE8) != 0.0f) {
                hit = NULL;
            } else {
                party->position.y = pos.y;
            }
        }
    }
    work->field_138 = hit;
    if (hit == NULL) {
        if (!(party->flags & 0x04000000)) {
            party->flags &= ~PARTY_FLAG_IS_GROUNDED;
            if (!(party->flags & PARTY_FLAG_IS_FALLING)) {
                party->flags |= PARTY_FLAG_IS_FALLING;
                if (work->jumpPara2 == PARTY_MOVE_ZERO) {
                    work->jumpPara2 = -1.4f;
                }
            }
            party->position.y += work->jumpPara1;
            work->jumpPara1 += work->jumpPara2;
            return 2;
        }
    } else {
        if (party->flags & PARTY_FLAG_IS_FALLING) {
            party->flags &= ~PARTY_FLAG_IS_FALLING;
            party->flags |= PARTY_FLAG_IS_GROUNDED;
            work->jumpPara1 = 0.0f;
            work->jumpPara2 = 0.0f;
            party->position.y = pos.y;
            return 3;
        }
        party->position.y = pos.y;
    }
    return 0;
}
#undef PARTY_MOVE_ZERO

// Inlined footmark writer used by partyRecordFootmark. The extra marioGetPtr() is present in the
// original bytes (its result is discarded).
static inline void partyRecordFootmarkSub(PartyEntry* party, Vec* pos, s32 flags) {
    s32 slot = (party->flags >> 2) & 1;
    Footmark* f;
    s32 id;

    marioGetPtr();
    f = (Footmark*)(footmarkBuf + slot * 0x500);
    id = writeId[slot];
    f = &f[id];
    if ((f->field_0 & 1) == 0 || (flags & 1) == 0) {
        if (f->x != pos->x || f->y != pos->y || f->z != pos->z) {
            if (readId[slot] != writeId[slot] + 1) {
                writeId[slot]++;
                if (writeId[slot] >= 0x50) {
                    writeId[slot] = 0;
                }
                f = (Footmark*)(footmarkBuf + slot * 0x500 + writeId[slot] * 0x10);
                *(Vec*)&f->x = *pos;
                if (flags & 1) {
                    f->field_0 |= 1;
                } else {
                    f->field_0 &= ~1;
                }
                if (flags & 4) {
                    f->field_0 |= 4;
                } else {
                    f->field_0 &= ~4;
                }
            }
        }
    }
}

void partyRecordFootmark(PartyEntry* party) {
    Vec pos;
    Vec pos2;
    Vec pos3;
    Vec mp;
    PartyEntry* target;
    s32 flag;
    MarioWork* player = party->playerPtr;
    s32 flags = 0;

    if (party->currentSlotId == 0) {
        MarioWork* m = marioGetPtr();
        s32 jump = 0;
        if (m->flags & MARIO_FLAG_IS_JUMPING) {
            jump = 1;
        }
        if (jump) {
            flags |= 1;
        }
        if (marioChkSlitThrouh() != 0) {
            flags |= 4;
        }
        pos = player->position;
        partyRecordFootmarkSub(party, &pos, flags);
    } else {
        flag = 0;
        target = partyGetPtr(marioGetPartyId());
        if (target != NULL) {
            if (nokonokoGetStatus(target) == 3
                || PSVECDistance(&player->position, &target->position) >= 150.0f) {
                flag = 1;
            }
        }
        if (target != NULL && flag == 0) {
            if (*(s8*)(party->field_4D + 4) == 0) {
                u32 i;
                Footmark* f;
                *(s8*)(party->field_4D + 4) = 1;
                mp = marioGetPtr()->position;
                f = (Footmark*)footmarkBuf;
                for (i = 0; i < 0x50; i++) {
                    *(Vec*)&f->x = mp;
                    f->field_0 = 0;
                    f++;
                }
                writeId[0] = 0;
                readId[0] = 0;
            }
            {
                s32 jump = 0;
                if (target->flags & PARTY_FLAG_IS_JUMPING) {
                    jump = 1;
                }
                if (jump) {
                    flags |= 1;
                }
            }
            pos2 = target->position;
            partyRecordFootmarkSub(party, &pos2, flags);
        } else {
            if (*(s8*)(party->field_4D + 4) == 1) {
                *(s8*)(party->field_4D + 4) = 0;
                partyClearFootmark2();
            }
            {
                MarioWork* m = marioGetPtr();
                s32 jump = 0;
                if (m->flags & MARIO_FLAG_IS_JUMPING) {
                    jump = 1;
                }
                if (jump) {
                    flags |= 1;
                }
            }
            if (marioChkSlitThrouh() != 0) {
                flags |= 4;
            }
            pos3 = player->position;
            partyRecordFootmarkSub(party, &pos3, flags);
        }
    }
}

// NONMATCHING (91.31%): the two footmark-clear loops carry the known redundant loop-entry test
// (`cmpwi`/`bge`) this compiler emits where the original uses ctr/bdnz; the speed ramp's running
// sum is allocated to f1 instead of f0.
void partyMoveWalk(PartyEntry* party) {
    PartyEntry* work = party;
    MarioWork* mario;
    s32 i;

    if (work->field_8 & 0x8) {
        work->field_8 &= ~0x8;
        party->flags &= ~0x100;
        mario = work->playerPtr;
        party->flags |= 0x2;
        if (!(party->flags & 0x8)) {
            Vec mp = marioGetPtr()->position;
            u32* src = (u32*)&mp;
            Footmark* f = (Footmark*)footmarkBuf;
            for (i = 0; i < 0x50; i++) {
                ((u32*)&f->x)[0] = src[0];
                ((u32*)&f->x)[1] = src[1];
                ((u32*)&f->x)[2] = src[2];
                f->field_0 = 0;
                f++;
            }
            writeId[0] = 0;
            readId[0] = 0;
        } else {
            PartyEntry* target;
            Vec pos;
            u32* src = (u32*)&pos;
            Footmark* f;
            MarioWork* m = marioGetPtr();
            target = partyGetPtr(marioGetPartyId());
            if (target == NULL || nokonokoGetStatus(target) == 3
                || PSVECDistance(&m->position, &target->position) >= 150.0f) {
                pos = m->position;
            } else {
                pos = target->position;
            }
            f = (Footmark*)(footmarkBuf + 0x500);
            for (i = 0; i < 0x50; i++) {
                ((u32*)&f->x)[0] = src[0];
                ((u32*)&f->x)[1] = src[1];
                ((u32*)&f->x)[2] = src[2];
                f->field_0 = 0;
                f++;
            }
            writeId[1] = 0;
            readId[1] = 0;
        }
        *(u8*)((u8*)work + 0x3D) = 0;
        *(u32*)((u8*)work + 0x44) = 0x10;
        *(f32*)((u8*)work + 0x40) = 50.0f;
        *(u8*)((u8*)work + 0x3E) = 0;
        work->jumpPara2 = 1.8f;
        work->jumpPara1 = 0.0f;
        if (!(mario->flags & 0x400000)) {
            partyChgPoseId(party, 1);
            switch (work->field_3B) {
            case 4:
            case 5:
                break;
            case 6:
                party->flags &= ~0x8000;
                break;
            }
            work->field_3B = 1;
            work->field_8 |= 4;
            if (work->field_36 == 3) {
                party->flags &= ~0x70;
                party->flags &= ~0x3000000;
                party->flags &= ~0x4000000;
            }
            work->field_36 = 0;
            work->field_8 |= 2;
        }
    }
    if (work->field_28 == 0 && work->field_36 != 2 && work->field_36 != 3 && work->field_36 != 6) {
        MarioWork* m = work->playerPtr;
        f32 target;
        f32 speed = work->speed;
        if (m->flags & 0x8000) {
            target = m->baseSpeed;
        } else {
            f32 v = work->speed2 * m->playerGravity;
            f32 scale = (party->flags & 0x80) ? 0.75f : 1.0f;
            target = v * scale;
        }
        if (target > 0.0f) {
            f32 f = 0.5f + target - speed;
            f = f * 0.05f + speed;
            if (f >= target) {
                f = target;
            }
            work->speed = f;
        } else {
            f32 f = -1.0f - speed;
            f = f * 0.1f + speed;
            if (f <= 0.0f) {
                f = 0.0f;
            }
            work->speed = f;
        }
    }
    partyWalkMain(party);
}

// Inlined footmark writer for one party slot's ring, used by partyWalkMain (same shape as partyRecordFootmarkSub).
static inline void partyWalkFootmarkSub(Vec* pos, s32 flags, s32 slot) {
    Footmark* f;
    s32 id;

    marioGetPtr();
    id = writeId[slot];
    f = (Footmark*)(footmarkBuf + slot * 0x500 + id * 0x10);
    if ((f->field_0 & 1) == 0 || (flags & 1) == 0) {
        if (f->x != pos->x || f->y != pos->y || f->z != pos->z) {
            if (readId[slot] != id + 1) {
                writeId[slot] = id + 1;
                if (writeId[slot] >= 0x50) {
                    writeId[slot] = 0;
                }
                f = (Footmark*)(footmarkBuf + slot * 0x500 + writeId[slot] * 0x10);
                f->x = pos->x;
                f->y = pos->y;
                f->z = pos->z;
                if (flags & 1) {
                    f->field_0 |= 1;
                } else {
                    f->field_0 &= ~1;
                }
                if (flags & 4) {
                    f->field_0 |= 4;
                } else {
                    f->field_0 &= ~4;
                }
            }
        }
    }
}

// NONMATCHING (87.35%): `flags |= (cond)` and the inlined writer codegen differ (extrwi/mr
// instead of rlwinm./li r3, plus an early `li r31`), same class as partyRecordFootmark.
void partyWalkMain(PartyEntry* party) {
    MarioWork* player = party->playerPtr;
    PartyEntry* target;
    s32 flags = 0;
    Vec pos;

    if (party->currentSlotId == 0) {
        flags |= (marioGetPtr()->flags & 0x10000) != 0;
        if (marioChkSlitThrouh() != 0) {
            flags |= 4;
        }
        pos = player->position;
        partyWalkFootmarkSub(&pos, flags, 0);
    } else {
        target = anotherPartyGetPtr(party->currentSlotId);
        if (target != NULL) {
            if ((target->currentMemberId == 3 && target->field_34 == 3)
                || nokonokoGetStatus(target) == 3
                || PSVECDistance(&player->position, &target->position) >= 150.0f) {
                flags = 0;
                flags |= (marioGetPtr()->flags & 0x10000) != 0;
                pos = marioGetPtr()->position;
                partyWalkFootmarkSub(&pos, flags, 1);
            } else {
                flags = 0;
                if (target->flags & PARTY_FLAG_IS_JUMPING) {
                    flags |= 1;
                }
                pos = target->position;
                partyWalkFootmarkSub(&pos, flags, 1);
            }
        } else {
            flags = 0;
            flags |= (marioGetPtr()->flags & 0x10000) != 0;
            pos = marioGetPtr()->position;
            partyWalkFootmarkSub(&pos, flags, 1);
        }
    }
    walkMain(party);
}

// NONMATCHING (88.61%): both footmark clears inherit partyClearFootmark's redundant loop-entry
// test/register allocation, and the three inlined partyChgMoveMode switches miss the `cmpwi r0,4;
// bge` range test. `(mode == 0) ? 0 : 2` makes MWCC emit the original cntlzw/extrwi/neg/andc idiom.
void partyMoveFlyInit(PartyEntry* party, s32 mode) {
    PartyEntry* work = party;
    s32 i;

    marioGetPtr();
    party->flags |= 0x2;
    if (!(party->flags & 0x8)) {
        Vec mp = marioGetPtr()->position;
        u32* src = (u32*)&mp;
        Footmark* f = (Footmark*)footmarkBuf;
        for (i = 0; i < 0x50; i++) {
            ((u32*)&f->x)[0] = src[0];
            ((u32*)&f->x)[1] = src[1];
            ((u32*)&f->x)[2] = src[2];
            f->field_0 = 0;
            f++;
        }
        writeId[0] = 0;
        readId[0] = 0;
    } else {
        PartyEntry* target;
        Vec pos;
        u32* src = (u32*)&pos;
        Footmark* f;
        MarioWork* m = marioGetPtr();
        target = partyGetPtr(marioGetPartyId());
        if (target == NULL || nokonokoGetStatus(target) == 3
            || PSVECDistance(&m->position, &target->position) >= 150.0f) {
            pos = m->position;
        } else {
            pos = target->position;
        }
        f = (Footmark*)(footmarkBuf + 0x500);
        for (i = 0; i < 0x50; i++) {
            ((u32*)&f->x)[0] = src[0];
            ((u32*)&f->x)[1] = src[1];
            ((u32*)&f->x)[2] = src[2];
            f->field_0 = 0;
            f++;
        }
        writeId[1] = 0;
        readId[1] = 0;
    }
    if (work->field_3B == 2) {
        *(f32*)((u8*)work + 0x40) = 50.0f;
    } else {
        *(u8*)((u8*)work + 0x3D) = (mode == 0) ? 0 : 2;
        switch (work->field_3B) {
        case 4:
        case 5:
            break;
        case 6:
            party->flags &= ~0x8000;
            break;
        }
        work->field_3B = 0;
        work->field_8 |= 4;
        if (*(s8*)((u8*)work + 0x3F) == 1 || *(s8*)((u8*)work + 0x3F) == 3) {
            *(u8*)((u8*)work + 0x3F) = 0;
            *(u8*)((u8*)work + 0x3D) = 0;
            switch (work->field_3B) {
            case 4:
            case 5:
                break;
            case 6:
                party->flags &= ~0x8000;
                break;
            }
            work->field_3B = 1;
            work->field_8 |= 4;
        } else if (*(s8*)((u8*)work + 0x3F) == 2) {
            *(u8*)((u8*)work + 0x3D) = 0;
            switch (work->field_3B) {
            case 4:
            case 5:
                break;
            case 6:
                party->flags &= ~0x8000;
                break;
            }
            work->field_3B = 0x3C;
            work->field_8 |= 4;
        }
        *(u32*)((u8*)work + 0x44) = 0x10;
        *(f32*)((u8*)work + 0x40) = 50.0f;
        *(u8*)((u8*)work + 0x3E) = 0;
        partyChgPoseId(party, 1);
        switch (work->field_3B) {
        case 4:
        case 5:
            break;
        case 6:
            party->flags &= ~0x8000;
            break;
        }
        work->field_3B = 1;
        work->field_8 |= 4;
        if (work->field_36 == 3) {
            party->flags &= ~0x70;
            party->flags &= ~0x3000000;
            party->flags &= ~0x4000000;
        }
        work->field_36 = 0;
        work->field_8 |= 2;
    }
}

static void flyMain(PartyEntry* party);

// Inlined footmark writer for partyFlyMain (same ring logic as partyWalkFootmarkSub).
static inline void partyFlyFootmarkSub(Vec* pos, s32 flags, s32 slot) {
    Footmark* f;
    s32 id;

    marioGetPtr();
    id = writeId[slot];
    f = (Footmark*)(footmarkBuf + slot * 0x500 + id * 0x10);
    if ((f->field_0 & 1) == 0 || (flags & 1) == 0) {
        if (f->x != pos->x || f->y != pos->y || f->z != pos->z) {
#ifdef MUST_MATCH
            /* the named temp makes the compiler keep readId in r3 and id + 1 in r0 */
            s32 readIdx = readId[slot];
            if (readIdx != id + 1) {
#else
            if (readId[slot] != id + 1) {
#endif
                writeId[slot]++;
                if (writeId[slot] >= 0x50) {
                    writeId[slot] = 0;
                }
                f = (Footmark*)(footmarkBuf + slot * 0x500 + writeId[slot] * 0x10);
#ifdef MUST_MATCH
                /* the original copies the position a word at a time; float assignments give stfs */
                {
                    u32* src = (u32*)pos;

                    ((u32*)&f->x)[0] = src[0];
                    ((u32*)&f->x)[1] = src[1];
                    ((u32*)&f->x)[2] = src[2];
                }
#else
                f->x = pos->x;
                f->y = pos->y;
                f->z = pos->z;
#endif
                if (flags & 1) {
                    f->field_0 |= 1;
                } else {
                    f->field_0 &= ~1;
                }
                if (flags & 4) {
                    f->field_0 |= 4;
                } else {
                    f->field_0 &= ~4;
                }
            }
        }
    }
}

void partyFlyMain(PartyEntry* party) {
    PartyEntry* target;
    s32 flags;

    if (party->currentSlotId == 0) {
        Vec pos;
#ifdef MUST_MATCH
        /* the named temp makes the compiler load mario's flags before clearing flags */
        s32 marioFlags = marioGetPtr()->flags;

        flags = 0;
        if (marioFlags & 0x10000) {
            flags = 1;
        }
#else
        flags = (marioGetPtr()->flags & 0x10000) != 0;
#endif
        pos = marioGetPtr()->position;
        partyFlyFootmarkSub(&pos, flags, 0);
    } else {
#ifdef MUST_MATCH
        /* the original calls anotherPartyGetPtr and discards its result */
        target = anotherPartyGetPtr(party->currentSlotId);
#endif
        target = partyGetPtr(0);
        if (target != NULL) {
            Vec pos;

            flags = 0;
            if (target->flags & PARTY_FLAG_IS_JUMPING) {
                flags = 1;
            }
            pos = target->position;
            partyFlyFootmarkSub(&pos, flags, 1);
        } else {
            Vec pos;
#ifdef MUST_MATCH
            s32 marioFlags = marioGetPtr()->flags;

            flags = 0;
            if (marioFlags & 0x10000) {
                flags = 1;
            }
#else
            flags = (marioGetPtr()->flags & 0x10000) != 0;
#endif
            pos = marioGetPtr()->position;
            partyFlyFootmarkSub(&pos, flags, 0);
        }
    }
    flyMain(party);
}

void walkMain(PartyEntry* party) {
    PartyEntry* work = party;

    switch (work->field_3B) {
    case 0:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
            partyChgPoseId(party, 1);
            if (work->currentMemberId == 5) {
                unk_801820d0(party);
            }
        }
        switch (work->field_36) {
        case 0:
            party_motion_stay(party);
            if (work->currentMemberId == 5) {
                unk_8018206c(party);
            }
            break;
        case 1:
        case 2:
        case 6:
            if (work->field_36 == 3) {
                party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                party->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
                party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            }
            work->field_36 = 0;
            work->field_8 |= 2;
            break;
        case 3:
            party_motion_fall(party);
            break;
        }
        work->field_FC = work->dir;
        if (work->field_8 & 0x01000000) {
            work->field_8 &= ~0x01000000;
            {
                Vec pos = party->position;
                marioSetSplash(0, &pos);
            }
            work->field_4C = 0x14;
        }
        if (--work->field_4C < 0) {
            work->field_4C = 0;
        }
        if (work->field_34 != 0x0D) {
            if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            }
        }
        break;
    case 1:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
            party_motion_stay(party);
            break;
        case 1:
            if (work->field_32 == 0) {
                party_motion_homing_walk(party);
            } else {
                party_motion_homing_fly(party);
            }
            break;
        case 2:
            party_motion_jump(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        if (work->field_8 & 0x01000000) {
            work->field_8 &= ~0x01000000;
            {
                Vec pos = party->position;
                marioSetSplash(0, &pos);
            }
            work->field_4C = 0x14;
        }
        if (--work->field_4C < 0) {
            work->field_4C = 0;
        }
        if (work->field_34 != 0x0D) {
            if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            }
        }
        break;
    case 2:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            if (work->field_32 == 0) {
                party_motion_behind_mario(party);
            } else {
                party_motion_behind_mario_fly(party);
            }
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    case 3:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            party_motion_beside_mario(party);
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    case 6:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            if (work->field_32 == 0) {
                party_motion_behind_mario_continue(party);
            } else {
                party_motion_behind_mario_fly(party);
            }
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    default:
        break;
    }
}

static void flyMain(PartyEntry* party) {
    PartyEntry* work = party;

    switch (work->field_3B) {
    case 0:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
            partyChgPoseId(party, 1);
            if (work->currentMemberId == 5) {
                unk_801820d0(party);
            }
        }
        switch (work->field_36) {
        case 0:
            party_motion_stay(party);
            if (work->currentMemberId == 5) {
                unk_8018206c(party);
            }
            break;
        case 1:
        case 2:
        case 6:
            if (work->field_36 == 3) {
                party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                party->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
                party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            }
            work->field_36 = 0;
            work->field_8 |= 2;
            break;
        case 3:
            party_motion_fall(party);
            break;
        }
        work->field_FC = work->dir;
        if (work->field_8 & 0x01000000) {
            work->field_8 &= ~0x01000000;
            {
                Vec pos = party->position;
                marioSetSplash(0, &pos);
            }
            work->field_4C = 0x14;
        }
        if (--work->field_4C < 0) {
            work->field_4C = 0;
        }
        if (work->field_34 != 0x0D) {
            if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            }
        }
        break;
    case 1:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
            party_motion_stay(party);
            break;
        case 1:
            if (work->field_32 == 0) {
                party_motion_homing_walk(party);
            } else {
                party_motion_homing_fly(party);
            }
            break;
        case 2:
            party_motion_jump(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        if (work->field_8 & 0x01000000) {
            work->field_8 &= ~0x01000000;
            {
                Vec pos = party->position;
                marioSetSplash(0, &pos);
            }
            work->field_4C = 0x14;
        }
        if (--work->field_4C < 0) {
            work->field_4C = 0;
        }
        if (work->field_34 != 0x0D) {
            if (work->field_138 != NULL && (hitGetAttr((HitEntry*)work->field_138) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_12C != NULL && (hitGetAttr((HitEntry*)work->field_12C) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            } else if (work->field_140 != NULL && (hitGetAttr((HitEntry*)work->field_140) & 0x800)) {
                partyChgRunMode(party, 0x0D);
            }
        }
        break;
    case 2:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            if (work->field_32 == 0) {
                party_motion_behind_mario(party);
            } else {
                party_motion_behind_mario_fly(party);
            }
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    case 3:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            party_motion_beside_mario(party);
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    case 6:
        if (work->field_8 & 0x4) {
            work->field_8 &= ~0x4;
        }
        switch (work->field_36) {
        case 0:
        case 1:
            if (work->field_32 == 0) {
                party_motion_behind_mario_continue(party);
            } else {
                party_motion_behind_mario_fly(party);
            }
            break;
        case 2:
            party_motion_jump2(party);
            break;
        case 3:
            party_motion_fall(party);
            break;
        case 6:
            party_motion_upstairs(party);
            break;
        case 7:
            party_motion_damage(party);
            break;
        default:
            break;
        }
        work->field_FC = work->dir;
        break;
    default:
        break;
    }
}
