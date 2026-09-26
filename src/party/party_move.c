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
    u32 field_0;
    f32 x;
    f32 y;
    f32 z;
} Footmark;

// Local view of the party movement work area. Fields not yet declared in
// PartyEntry live here and should move into a header later.
typedef struct PartyMoveWork {
    u32 flags;                    // 0x0
    u8 field_4[0x8 - 0x4];
    u32 field_8;                  // 0x8
    u8 field_C[0x28 - 0xC];
    s16 field_28;                 // 0x28
    u8 field_2A[0x2D - 0x2A];
    s8 field_2D;                  // 0x2D
    u8 field_2E[0x32 - 0x2E];
    u8 field_32;                  // 0x32
    u8 field_33;                  // 0x33
    u8 field_34;                  // 0x34
    u8 field_35;                  // 0x35
    u8 field_36;                  // 0x36
    u8 field_37[0x3B - 0x37];
    u8 field_3B;                  // 0x3B
    u8 field_3C[0x4C - 0x3C];
    s8 field_4C;                  // 0x4C
    u8 field_4D[0x58 - 0x4D];
    Vec position;                 // 0x58
    u8 field_64[0xE4 - 0x64];
    f32 field_E4;                 // 0xE4
    u8 field_E8[0xFC - 0xE8];
    f32 field_FC;                 // 0xFC
    f32 dir;                      // 0x100
    f32 speed;                    // 0x104
    f32 speed2;                   // 0x108
    f32 field_10C;                // 0x10C
    f32 field_110;                // 0x110
    f32 jumpPara1;                // 0x114
    f32 jumpPara2;                // 0x118
    f32 jumpPara3;                // 0x11C
    u8 field_120[0x12C - 0x120];
    void* field_12C;              // 0x12C
    u8 field_130[0x134 - 0x130];
    void* field_134;              // 0x134
    void* field_138;              // 0x138
    u8 field_13C[0x140 - 0x13C];
    void* field_140;              // 0x140
    u8 field_144[0x160 - 0x144];
    MarioWork* playerPtr;         // 0x160
} PartyMoveWork;

void partyMoveMain(PartyEntry* party, s32 hosei);
s32 nokonokoGetStatus(PartyEntry* party);
void* mobjHitObjPtrToPtr(HitEntry* hit);
s32 marioChkDeepSleep(void);
s32 chuchu_searchObject(void);
char* partyChgPoseId(PartyEntry* party, s32 index);
void unk_800c90d4(PartyEntry* party, s32 value);
s32 N_marioGetMovementSpeed(void);
s32 party_motion_fall(PartyEntry* party);
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
    PartyMoveWork* work = (PartyMoveWork*)party;
    work->dir = angleABf(work->position.x, work->position.z, x, z);
}

void partyChgMot(PartyEntry* party, u8 mot) {
    PartyMoveWork* work = (PartyMoveWork*)party;
    if (work->field_36 == 3) {
        work->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
        work->flags &= ~(PARTY_FLAG_FORCE_FALL | PARTY_FLAG_DISABLE_FRONT_WALL_CHK);
        work->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
    }
    work->field_36 = mot;
    work->field_8 |= 2;
}

void partyChgMoveMode(PartyEntry* party, u8 mode) {
    PartyMoveWork* work = (PartyMoveWork*)party;
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

void unk_800c9398(PartyEntry* party, f32 value) {
    PartyMoveWork* work = (PartyMoveWork*)party;
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
    PartyMoveWork* work = (PartyMoveWork*)party;
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

f32 unk_800c92c4(PartyEntry* party) {
    PartyMoveWork* work = (PartyMoveWork*)party;
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

void unk_800c93d0(PartyEntry* party, f32 value) {
    PartyMoveWork* work = (PartyMoveWork*)party;
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
    PartyMoveWork* work = (PartyMoveWork*)party;
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
    PartyMoveWork* work = (PartyMoveWork*)party;
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
    if (f3 == 1.0f) {
        f2 = 1.65f;
    } else {
        f2 = 2.0f;
    }
    if (f3 == 1.0f) {
        f3 = 0.8f;
    } else {
        f3 = 0.0f;
    }
    if (f1 > f2 - f3 && f1 < f1 + f3) {
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
    PartyMoveWork* work = (PartyMoveWork*)party;

    if (work->field_34 == 0) {
        return;
    }
    if (marioChkDeepSleep() == 1 || (party->flags & 0x400000)) {
        partyChgPoseId(party, 6);
    } else if (party->currentMemberId == 7) {
        if (chuchu_searchObject() == 0) {
            partyChgPoseId(party, 1);
        }
    } else {
        partyChgPoseId(party, 1);
    }
}

void unk_800c8e74(PartyEntry* party) {
    PartyMoveWork* work = (PartyMoveWork*)party;
    MarioWork* mario = work->playerPtr;
    f32 angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 viewYaw = cam->viewYaw;
    f32 result = revise360(viewYaw + 270.0f - angle);
    s32 flag = 0;

    if (result >= 90.0f && result <= 270.0f) {
        flag = 1;
    }
    work->field_110 = flag ? 180.0f : 0.0f;
}

void unk_800c7c98(PartyEntry* party) {
    PartyMoveWork* work = (PartyMoveWork*)party;
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
    {
        s8 timer = work->field_4C - 1;
        work->field_4C = timer;
        if (timer < 0) {
            work->field_4C = 0;
        }
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
    PartyMoveWork* work = (PartyMoveWork*)party;
    Vec pos = party->position;
    f32 angle;
    f32 speed;
    void* wall;
    HitEntry* ground;

    partyGetMoveDirSpd(party, &speed, &angle);
    wall = partySearchFrontWall(party, &pos, angle, speed);
    if (wall != NULL) {
        work->field_12C = wall;
        if (work->speed != 0.0f) {
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
    PartyMoveWork* work = (PartyMoveWork*)party;
    Vec pos = party->position;
    f32 angle;
    f32 speed;
    void* wall;

    work->speed = 0.0f;
    partyGetMoveDirSpd(party, &speed, &angle);
    wall = partySearchFrontWall(party, &pos, angle, speed);
    if (wall != NULL) {
        work->field_12C = wall;
        if (work->speed != 0.0f) {
            work->field_134 = wall;
        }
    }
    party->position.x = pos.x;
    party->position.z = pos.z;
}

f32 unk_800c9f48(s32 id, f32 y) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target;
    s32 flag = 0;
    s32 isTarget = id != 0;

    target = anotherPartyGetPtr(isTarget);
    if (id == 0) {
        return fabsf(mario->position.y - y);
    }
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        MarioWork* mario2 = marioGetPtr();
        PartyEntry* target2;
        s32 flag2 = 0;

        marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario2->position, &target2->position) >= 150.0f) {
                flag2 = 1;
            }
        }
        if (flag2) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            return fabsf(target2->position.y - y);
        }
        return fabsf(mario2->position.y - y);
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
        return ((PartyMoveWork*)target)->speed;
    }
    mario = marioGetPtr();
    if (mario->baseSpeed > 0.0f) {
        return mario->baseSpeed;
    }
    return mario->mBaseDashSpeed;
}

f32 partyGetTargetDist3D(s32 id, Vec* pos) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target;
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
        MarioWork* mario2 = marioGetPtr();
        PartyEntry* target2;
        Vec local;
        s32 flag2 = 0;

        marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario2->position, &target2->position) >= 150.0f) {
                flag2 = 1;
            }
        }
        if (flag2) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            local = target2->position;
        } else {
            local = mario2->position;
        }
        return PSVECDistance(&local, pos);
    }
    return PSVECDistance(&marioGetPtr()->position, pos);
}

f32 partyGetTargetDist(s32 id, f32 x, f32 z) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target;
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
        MarioWork* mario2 = marioGetPtr();
        PartyEntry* target2;
        s32 flag2 = 0;

        marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario2->position, &target2->position) >= 150.0f) {
                flag2 = 1;
            }
        }
        if (flag2) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            return distABf(x, z, target2->position.x, target2->position.z);
        }
        return distABf(x, z, mario2->position.x, mario2->position.z);
    }
    return distABf(x, z, mario->position.x, mario->position.z);
}

f32 partyGetTargetDir(s32 id, f32 x, f32 z) {
    MarioWork* mario = marioGetPtr();
    PartyEntry* target;
    s32 flag = 0;

    target = anotherPartyGetPtr(id != 0);
    if (target != NULL) {
        if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
            flag = 1;
        }
    }
    if (target != NULL && flag == 0) {
        MarioWork* mario2 = marioGetPtr();
        PartyEntry* target2;
        s32 flag2 = 0;

        marioGetPtr();
        target2 = anotherPartyGetPtr(id != 0);
        if (target2 != NULL) {
            if (nokonokoGetStatus(target2) == 3 || PSVECDistance(&mario2->position, &target2->position) >= 150.0f) {
                flag2 = 1;
            }
        }
        if (flag2) {
            target2 = NULL;
        }
        if (target2 != NULL) {
            return angleABf(x, z, target2->position.x, target2->position.z);
        }
        return angleABf(x, z, mario2->position.x, mario2->position.z);
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
