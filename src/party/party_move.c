#include "party.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include <string.h>
#include <dolphin/mtx.h>
#include <math.h>

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

void partyMoveMain(PartyEntry* party, s32 hosei);
s32 nokonokoGetStatus(PartyEntry* party);
void* mobjHitObjPtrToPtr(HitEntry* hit);
s32 marioChkDeepSleep(void);
s32 chuchu_searchObject(void);
void unk_800c90d4(PartyEntry* party, s32 value);
s32 N_marioGetMovementSpeed(void);
void partySetDir(PartyEntry* party, f32 x, f32 z);
s32 party_motion_fall(PartyEntry* party);
void party_motion_beside_mario(PartyEntry* party);
void party_motion_jump2(PartyEntry* party);
void party_motion_upstairs(PartyEntry* party);
void party_motion_damage(PartyEntry* party);
void party_motion_behind_mario(PartyEntry* party);
void party_motion_behind_mario_fly(PartyEntry* party);
void unk_8014bbc8(PartyEntry* party);
void unk_801820d0(PartyEntry* party);
void unk_8018206c(PartyEntry* party);
void partyChgRunMode(PartyEntry* party, s32 mode);
void marioSetSplash(s32 a, Vec* pos);
void vivianShadow(PartyEntry* party);
void partyGetMoveDirSpd(PartyEntry* party, f32* angle, f32* speed);
void* partySearchFrontWall(PartyEntry* party, Vec* pos, f32 speed, f32 angle);
void* partySearchGround(PartyEntry* party, f32 a, f32 b);

void partySetDir(PartyEntry* party, f32 x, f32 z) {
    PartyEntry* work = party;
    work->dir = angleABf(work->position.x, work->position.z, x, z);
}

void partyChgMot(PartyEntry* party, u8 mot) {
    PartyEntry* work = party;
    if (work->field_36 == 3) {
        work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
        work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
        work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    }
    work->field_36 = mot;
    work->field_8 |= 2;
}

// NONMATCHING (86.67%): 2 missing.
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

// NONMATCHING (99.29%): same instructions; only register numbers or operands differ.
void unk_800c9398(PartyEntry* party, f32 value) {
    PartyEntry* work = party;
    f32 speed = value * work->playerPtr->playerGravity;
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

// NONMATCHING (63.2%): the `u32` copy makes the compiler emit the original's `stw` stores, but it
// still unrolls 8 records (20 iterations) instead of the original's 16 (10 iterations) and picks a
// different buffer register. The plain field form below only reaches 41.6% (`stfs` vs `stw`).
#ifdef MUST_MATCH
// NONMATCHING (63.22%): 6 missing, 12 extra, plus register differences.
void partyClearFootmark(void) {
    MarioWork* mario = marioGetPtr();
    Vec pos = mario->position;
    u32* src = (u32*)&pos;
    Footmark* f = (Footmark*)footmarkBuf;
    s32 i;

    for (i = 0; i < 0xA0; i++) {
        f[i].field_0 = 0;
        ((u32*)&f[i].x)[0] = src[0];
        ((u32*)&f[i].x)[1] = src[1];
        ((u32*)&f[i].x)[2] = src[2];
    }
    writeId[0] = 0;
    readId[0] = 0;
}
#else
void partyClearFootmark(void) {
    Vec pos = marioGetPtr()->position;
    s32 i;

    for (i = 0; i < 0xA0; i++) {
        ((Footmark*)footmarkBuf)[i].field_0 = 0;
        ((Footmark*)footmarkBuf)[i].x = pos.x;
        ((Footmark*)footmarkBuf)[i].y = pos.y;
        ((Footmark*)footmarkBuf)[i].z = pos.z;
    }
    writeId[0] = 0;
    readId[0] = 0;
}
#endif

// NONMATCHING (65.96%): same word-store/unroll mismatch as partyClearFootmark; the position selection
// (target when close, else Mario) matches, only the clear loop differs.
void partyClearFootmark2(void) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target = partyGetPtr(marioGetPartyId());
    Vec pos;
    s32 i;

    if (target != NULL && nokonokoGetStatus(target) != 3
        && PSVECDistance(&mario->position, &target->position) < 150.0f) {
        pos = target->position;
    } else {
        pos = mario->position;
    }
    for (i = 0; i < 0xA0; i++) {
        ((Footmark*)footmarkBuf)[i].field_0 = 0;
        ((Footmark*)footmarkBuf)[i].x = pos.x;
        ((Footmark*)footmarkBuf)[i].y = pos.y;
        ((Footmark*)footmarkBuf)[i].z = pos.z;
    }
    writeId[1] = 0;
    readId[1] = 0;
}

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
    obj = mobjHitObjPtrToPtr(hit);
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

// NONMATCHING (99.77%): the multiply-free add needs `fadds f0, f0, f1` (constant first);
// this compiler emits the operands in source order, so one operand order stays swapped.
void unk_800c8e74(PartyEntry* party) {
    PartyEntry* work = party;
    MarioWork* mario = work->playerPtr;
    f32 angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
#ifdef MUST_MATCH
    /* The named temp makes the compiler load cam->viewYaw into f1 before 270.0f into f0. */
    f32 viewYaw = cam->viewYaw;
    f32 t = 270.0f;
    f32 result = revise360(viewYaw + t - angle);
#else
    f32 result = revise360(cam->viewYaw + 270.0f - angle);
#endif
    s32 flag = 0;

    if (result >= 90.0f && result <= 270.0f) {
        flag = 1;
    }
    work->field_110 = flag ? 180.0f : 0.0f;
}

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
