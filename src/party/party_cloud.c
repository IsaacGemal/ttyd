#include "party/party_cloud.h"
#include "dolphin/types.h"
#include "party.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mario_party.h"
#include "mario/mario_cam.h"
#include "mario/mario_motion.h"
#include "party/party_move.h"
#include "driver/hitdrv.h"
#include "driver/effdrv.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#pragma pool_data on // the original addresses its .rodata through one base register
#include "math_ext.h"
#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)
#include <string.h>

Vec cloud_at;


void mot_cloud() {
    MarioWork* mario = marioGetPtr();
    
    if((mario->trigFlags & 0x1) != 0) {
        mario->trigFlags &= ~0x1;
        mario->flags &= ~(MARIO_FLAG_HAS_INPUT_JUMP|MARIO_FLAG_IS_STEPPING|MARIO_FLAG_IS_FALLING|MARIO_FLAG_IS_JUMPING);
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
    }
}

void cloudResetAt() {
    MarioWork* mario = marioGetPtr();
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);

    if(party != NULL && party->currentMemberId == PARTY_MEMBER_FLURRIE) {
        cloud_at.x = camGetPtr(CAMERA_3D)->cameraAt.x;
        cloud_at.y = camGetPtr(CAMERA_3D)->cameraAt.y;
        cloud_at.z = camGetPtr(CAMERA_3D)->cameraAt.z;

        marioCamZoomOffReq2(600);
    }

    mario->dispFlags &= ~(1 << 8); //kDisableMakeDispDir
    mario->dispFlags &= ~(1 << 2); //kForceFrontPose
    marioAdjustMoveDir();
    mario->unk150.x = 0.01f;
    mario->unk138 = 0.0f;
}

u8 cloudGetStatus() {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    u8 status = 2;

    if (party == NULL || (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED) == 0)) {
        status = 0;
        return status;
    }
    else if (party->useMotionId == 10 && party->useMotionId < 20) {
        status = 1;
    }

    return status;
}

f32 cloudGetBreathDir() {
    PartyEntry* party = partyGetPtr(PARTY_SLOT_PARTY);
    f32 breathDir;

    if (party == NULL) {
        return 0.0f;
    }

    if ((party->flags & PARTY_FLAG_IS_BEING_USED) != 0 && (party->useMotionId < MARIO_MOTION_FALL)){
        breathDir = -1.0f;
    }
    else if (party->useStruct == NULL) {
        breathDir = 0.0f;
    }
    else {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        breathDir = toMovedir(cloud->unk4);
    }
    
    return breathDir;
}

f32 cloudGetBreathPower(f32 param_1, Vec* param_2) {
    PartyEntry* party = partyGetPtr(PARTY_SLOT_PARTY);
    f32 breathPower;
    f32 dist;
    f32 angle;
    f32 unk8;
    f32 val;

    breathPower = 0.0f;
    if (party == NULL)  {
        return breathPower;
    }

    val = 10.0f;
    if(
        party->currentMemberId == PARTY_MEMBER_FLURRIE 
        && (party->flags & PARTY_FLAG_IS_BEING_USED) != 0 
        && party->useMotionId >= MARIO_MOTION_FALL
        && ((fabs(party->position.y - param_2->y - 18.0f)) <= val)
    ) {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        unk8 = cloud->unk8;
        if (-((0.5f * param_1) - distABf(party->position.x, party->position.z, param_2->x, param_2->z)) <= unk8) {
            PartyCloud* cloud = (PartyCloud*)party->useStruct;
            dist = toMovedir(cloud->unk4);
            angle = angleABf(party->position.x, party->position.z, param_2->x, param_2->z);
            dist = revise360(angle - dist);
            
            if (dist <= 30.0f || dist >= 330.0f) {
                breathPower = cos((3.1415927f * dist) / 180.0f);
                val = 100.0f;
                breathPower = val * fabsf(breathPower);
            }
        }
    }

    return breathPower;
}

s32 cloudGetHitBreathout(s32 param_1) {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    s32 hitBreathout;

    if (party == NULL) {
        return 0;
    }

    if (
        party->currentMemberId != PARTY_MEMBER_FLURRIE 
        || (party->flags & PARTY_FLAG_IS_BEING_USED) == 0
        || party->useMotionId < MARIO_MOTION_FALL
    ){
            return 0;
    }

    hitBreathout = 0;
    if (param_1 == 0) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk20;
    }
    else if (param_1 == 1) { 
        hitBreathout = ((PartyCloud*)party->useStruct)->unk24;
    }
    else if (param_1 == 2) {
        hitBreathout = ((PartyCloud*)party->useStruct)->unk28;
    }
    else if (param_1 == 3) { 
        hitBreathout = ((PartyCloud*)party->useStruct)->unk2C;
    }
    
    return hitBreathout;
}

f32 cloudGetBreathDist() {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    f32 breathDist;

    if (party == NULL) {
        breathDist = 0.0f;
        return breathDist;
    }

    if ((party->currentMemberId != PARTY_MEMBER_FLURRIE || (party->flags & PARTY_FLAG_IS_BEING_USED) == 0) || party->useMotionId < MARIO_MOTION_FALL) {
        breathDist = 0.0f;
    }
    else {
        PartyCloud* cloud = (PartyCloud*)party->useStruct;
        breathDist = cloud->unk8;
    }

    return breathDist;
}

void unk_801820d0(PartyEntry* party) {
    *(f32*)((u8*)party + 0x17C) = 0.0f;
    *(f32*)((u8*)party + 0x180) = 4.25f;
}

void unk_8018206c(PartyEntry* party) {
    f32 bob;

    *(f32*)((u8*)party + 0x17C) = revise360(*(f32*)((u8*)party + 0x17C) - *(f32*)((u8*)party + 0x180));
    bob = sin((3.1415927f * *(f32*)((u8*)party + 0x17C)) / 180.0f);
    *(f32*)((u8*)party + 0x80) = 3.0f + 3.0f * bob;
}

void cloud_move(PartyEntry* party) {
    f32 bob;

    if ((party->field_8 & 0x8) != 0) {
        party->field_8 &= ~0x8;
        partyMoveFlyInit(party, 1);
        *(f32*)((u8*)party + 0x17C) = 0.0f;
        *(f32*)((u8*)party + 0x180) = 4.25f;
    }

    partyFlyMain(party);

    *(f32*)((u8*)party + 0x17C) = revise360(*(f32*)((u8*)party + 0x17C) - *(f32*)((u8*)party + 0x180));
    bob = sin((3.1415927f * *(f32*)((u8*)party + 0x17C)) / 180.0f);
    *(f32*)((u8*)party + 0x80) = 3.0f + 3.0f * bob;

    if (party->field_3B != 2) {
        party->field_8 &= ~0x40000;
    }
}

void cloud_init(PartyEntry* party) {
    party->field_33 = 5;
    party->field_32 = 1;
    party->height = 38.0f;
    party->radius = 24.0f;
    *(s32*)((u8*)party + 0x174) = 0;
    *(s32*)((u8*)party + 0x178) = 0;
    *(s32*)((u8*)party + 0x17C) = 0;
    *(s32*)((u8*)party + 0x180) = 0;
    *(s32*)((u8*)party + 0x170) = 0;
    *(s32*)((u8*)party + 0x178) = 0;
}

// `opt_common_subs off`: with CSE on MWCC hoists party+0x94 across movePos into a
// callee-saved register (r29 + a 0x20 frame). Writing the flag as `pf = (...)` rather
// than `if (...) pf = 1;` gives the original's r0 allocation for pf.
#pragma opt_common_subs off // without it MWCC CSEs party+0x94 across movePos into a callee-saved register (r29)
static inline void cloudMoveToMario(PartyEntry* party) {
    MarioWork* pm = party->playerPtr;
    f32 pa;
    s32 pf;
    party->targetPos = pm->position;
    pa = *(f32*)pm->unk1AC;
    pf = (pa < 90.0f || pa >= 270.0f);
    pa = toMovedir(pa + (pf != 0 ? -90.0f : 90.0f));
    movePos((f32*)((u8*)party + 0x94), (f32*)((u8*)party + 0x9C), 3.3f, pa);
    party->targetPos.y += 10.0f;
    party->position = party->targetPos;
}

// The original spells the last two guards as one `mot <= 1 || mot == 2` condition; MWCC then
// keeps the `mot == 2` arm out of line (`bne body; b end`). The braces below just keep the
// cleanup body in its own scope (and unchanged from the base).
void cloud_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    u16 mot;

    N__cloud_use(party);

    mot = *(u16*)((u8*)mario + 0x2E);
    if (mot == 0x1B) {
        return;
    }
    if (mot <= 1 || mot == 2) {
        return;
    }
    {
        party->flags &= ~(1 << 8);

        if (party->useStruct != NULL) {
            if (*(u32*)((u8*)party->useStruct + 0x34) != 0xFFFFFFFFu) {
                psndSFXOff(*(s32*)((u8*)party->useStruct + 0x34));
                *(u32*)((u8*)party->useStruct + 0x34) = 0xFFFFFFFFu;
            }
        }

        if (*(void**)((u8*)party + 0x178) != NULL) {
            effSoftDelete((EffectEntry*)*(void**)((u8*)party + 0x178));
            *(void**)((u8*)party + 0x178) = NULL;
        }

        cloudMoveToMario(party);

        party->flags2 |= 0x10;
        party->flags &= 0x7FFFFFFF;

        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }

        mot = *(u16*)((u8*)mario + 0x2E);
        if (mot == 0x1F || mot == 0x20) {
            partyChgRunMode(party, 0x0D);
        } else {
            partyChgPose(party, partyChgPoseId(party, 1));
            partyChgRunMode(party, 2);
            partyChgMot(party, 0);
        }
    }
}
#pragma opt_common_subs reset

#define cloud ((PartyCloud*)party->useStruct)

#ifdef MUST_MATCH
// Match-only: stops MWCC CSE-ing party+0x94 into a callee-saved register across the inlined movePos.
#pragma opt_common_subs off
static inline void cloudMoveToMarioStep(PartyEntry* party) {
    MarioWork* pm = party->playerPtr;
    f32 pa;
    s32 pf;
    party->targetPos = pm->position;
    pa = *(f32*)pm->unk1AC;
    pf = (pa < 90.0f || pa >= 270.0f);
    pa = toMovedir(pa + (pf != 0 ? -90.0f : 90.0f));
    movePos((f32*)((u8*)party + 0x94), (f32*)((u8*)party + 0x9C), 3.3f, pa);
    party->targetPos.y += 10.0f;
}
#define MOVE_TO_MARIO() cloudMoveToMarioStep(party)
#else
#define MOVE_TO_MARIO() do { \
    MarioWork* pm = party->playerPtr; \
    f32 pa; \
    s32 pf = 0; \
    party->targetPos = pm->position; \
    pa = *(f32*)((u8*)pm + 0x1AC); \
    if (pa < 90.0f || pa >= 270.0f) { pf = 1; } \
    pa = toMovedir(pa + (pf ? -90.0f : 90.0f)); \
    movePos((f32*)((u8*)party + 0x94), (f32*)((u8*)party + 0x9C), 3.3f, pa); \
    party->targetPos.y += 10.0f; \
} while (0)
#endif

#ifdef MUST_MATCH
// Match-only: the original re-reads party->playerPtr at the cloud->unk8 clamp (lwz r30, 0x160(r29));
// the natural single load is under #else.
#define RELOAD_MARIO() (mario = party->playerPtr)
#else
#define RELOAD_MARIO() ((void)0)
#endif

void N__cloud_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    f32 sx, sz, hitNZ, hitNY, hitNX, dist, hitZ, hitY, hitX, cos2, sin2, cos1, sin1;
    f32 angle;
    f32 f31;
    s32 flag;
    s16 seq;

    if ((party->field_8 & 0x8) != 0) {
        party->field_8 &= ~0x8;
        *(s32*)((u8*)party + 0x17C) = 0;
        *(s32*)((u8*)party + 0x180) = 0;
        marioAdjustMoveDir();
        party->useMotionId = 0;
        party->useStruct = __memAlloc(HEAP_DEFAULT, 0x48);
        memset(party->useStruct, 0, 0x48);
        cloud->unk0 = 100.0f;
        *(f32*)((u8*)cloud + 0xC) = 120.0f;
        *(f32*)((u8*)cloud + 0x10) = 0.0f;
        *(s32*)((u8*)cloud + 0x14) = 0;
        *(f32*)((u8*)cloud + 0x18) = 0.0f;
        *(s32*)((u8*)cloud + 0x1C) = 1;
        *(void**)((u8*)party + 0x178) = NULL;
        if (party->useStruct != NULL) {
            *(u32*)((u8*)party->useStruct + 0x34) = 0xFFFFFFFFu;
        }
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) != 0) {
            *(s32*)((u8*)party->useStruct + 0x30) = sysMsec2Frame(party->unk16C);
        }
    }

    switch (party->useMotionId) {
    case 0:
        mario->dispFlags |= 0x100;
        party->timer = 4;
        party->useMotionId = 1;
        /* fallthrough */
    case 1:
        MOVE_TO_MARIO();
        party->field_10C = mario->unk1A8;
        party->field_110 = *(f32*)((u8*)mario + 0x1AC);
        party->position.x += (party->targetPos.x - party->position.x) / (f32)party->timer;
        party->position.y += (party->targetPos.y - party->position.y) / (f32)party->timer;
        party->position.z += (party->targetPos.z - party->position.z) / (f32)party->timer;
        if (--party->timer > 0) {
            break;
        }
        party->useMotionId = 0xA;
        /* fallthrough */
    case 0xA:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        if (*(void**)((u8*)party + 0x178) == NULL) {
            *(void**)((u8*)party + 0x178) = effKumokumoN64Entry(0, 0, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, 0.0f);
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x60) = party->camId;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x18) = 0x4C;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x1C) = 0xA7;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x20) = 0xCA;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x28) = 0x7D;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x2C) = 0x1B;
            *(s32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x30) = 0xF5;
        }
        marioChgMot(0x1B);
        marioChgPose("M_A_2A");
        partyChgPose(party, (char*)"PWD_A_5");
        party->timer = 0x10;
        party->useMotionId = 0xB;
        mario->dispFlags |= 0x4;
        mario->dispFlags |= 0x100;
        party->flags2 |= 0x10;
        *(f32*)((u8*)cloud + 0x8) = 0.0f;
        *(f32*)((u8*)cloud + 0x4) = mario->unk1A8;
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            mario->unk150.x = 0.03f;
            mario->unk138 = 70.0f * ((revise360(270.0f - *(f32*)((u8*)mario + 0x1AC)) <= 180.0f) ? -1.0f : 1.0f);
        }
        if (party->useStruct != NULL && *(u32*)((u8*)party->useStruct + 0x34) == 0xFFFFFFFFu) {
            *(s32*)((u8*)party->useStruct + 0x34) = psndSFXOn_3D((char*)0x930, &party->playerPtr->position);
        }
        /* fallthrough */
    case 0xB:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        if (--party->timer > 0) {
            break;
        }
        partyChgPose(party, (char*)"PWD_A_6");
        marioChgPose("M_A_2B");
        party->useMotionId = 0xC;
        /* fallthrough */
    case 0xC:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        partyUpdateKeyData(party);
        *(f32*)((u8*)mario + 0x1A8) = *(f32*)((u8*)cloud + 0x4);
        *(f32*)((u8*)mario + 0x1AC) = *(f32*)((u8*)cloud + 0x4);
        party->field_10C = *(f32*)((u8*)cloud + 0x4);
        party->field_110 = *(f32*)((u8*)cloud + 0x4);
        sx = mario->position.x;
        sz = mario->position.z;
        f31 = *(f32*)((u8*)mario + 0x1A8);
        flag = 0;
        if (f31 < 90.0f || f31 >= 270.0f) { flag = 1; }
        f31 = toMovedir(f31 + (flag ? -90.0f : 90.0f));
        movePos(&sx, &sz, 3.3f, f31);
        party->position.x = sx;
        party->position.z = sz;
        party->position.y = mario->position.y + 10.0f;
        *(Vec*)party->field_7C = (Vec){0.0f, 0.0f, 0.0f};
        *(f32*)((u8*)cloud + 0x8) += 2.0f;
        RELOAD_MARIO();
#ifdef MUST_MATCH
        // Match-only: the original loads cloud->unk8 (0x8) before cloud->field_0xC; assigning mx
        // inside the comparison forces that emission order. The natural form is under #else.
        {
            f32 a = cloud->unk8;
            f32 mx;
            if (a >= (mx = *(f32*)cloud->field_0xC)) {
                cloud->unk8 = mx;
            }
        }
#else
        if (cloud->unk8 >= *(f32*)cloud->field_0xC) {
            cloud->unk8 = *(f32*)cloud->field_0xC;
        }
#endif
        angle = toMovedir(cloud->unk4);
        sincosf(&sin1, &cos1, angle);
        sincosf(&sin2, &cos2, -45.0f);
        dist = cloud->unk8;
        hit = hitCheckFilter(NULL, party->position.x, 18.0f + mario->position.y, party->position.z, sin1, sin2, cos1, &hitX,
                             &hitY, &hitZ, &dist,
                             &hitNX, &hitNY, &hitNZ);
        if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
            hit = NULL;
        }
        cloud->unk24 = (s32)hit;
        getHitBreatheout2(party, 0.0f);
        if (++*(s16*)(cloud->field_0x30 + 8) > 2) {
            *(s16*)(cloud->field_0x30 + 8) = 0;
        }
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            if ((*(u16*)((u8*)party + 0x150) & 0x400) != 0 && marioKeyOffChk() == 0) {
                break;
            }
        } else {
            if (--*(s32*)((u8*)cloud + 0x30) > 0) {
                break;
            }
        }
        cloud->unk20 = 0;
        cloud->unk24 = 0;
        cloud->unk28 = 0;
        cloud->unk2C = 0;
        party->timer = sysMsec2Frame(200);
        party->useMotionId = 0xD;
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) != 0) {
            party->useMotionId = 0x14;
        }
        if (party->useStruct != NULL && *(u32*)((u8*)party->useStruct + 0x34) != 0xFFFFFFFFu) {
            psndSFXOff(*(s32*)((u8*)party->useStruct + 0x34));
            *(u32*)((u8*)party->useStruct + 0x34) = 0xFFFFFFFFu;
        }
        break;
    case 0xD:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        partyUpdateKeyData(party);
        if ((*(u16*)((u8*)party + 0x152) & 0x400) != 0) {
            party->useMotionId = 0xA;
        }
        if (--party->timer <= 0) {
            party->useMotionId = 0x14;
            break;
        }
        if (*(s8*)((u8*)party + 0x158) != 0 || *(s8*)((u8*)party + 0x159) != 0) {
            party->useMotionId = 0x14;
        } else {
            break;
        }
        /* fallthrough */
    case 0x14:
        party->flags &= ~(1 << 8);
        if (party->useStruct != NULL && *(u32*)((u8*)party->useStruct + 0x34) != 0xFFFFFFFFu) {
            psndSFXOff(*(s32*)((u8*)party->useStruct + 0x34));
            *(u32*)((u8*)party->useStruct + 0x34) = 0xFFFFFFFFu;
        }
        if (*(void**)((u8*)party + 0x178) != NULL) {
            effSoftDelete((EffectEntry*)*(void**)((u8*)party + 0x178));
            *(void**)((u8*)party + 0x178) = NULL;
        }
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        party->flags2 &= ~0x10;
        if (*(u16*)((u8*)mario + 0x2E) == 0x1B) {
            mario->dispFlags &= ~(1 << 8);
            mario->dispFlags &= ~(1 << 2);
            marioAdjustMoveDir();
            party->field_8 |= 0x40000;
            mario->dispFlags &= ~(1 << 8);
            marioChgMot(0);
        }
        party->flags2 |= 0x10;
        partyChgRunMode(party, 2);
        *(u8*)((u8*)party + 0x3D) = 0;
        partyChgMoveMode(party, 2);
        partyChgMot(party, 1);
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            mario->unk150.x = 0.01f;
            mario->unk138 = 0.0f;
        }
        party->flags &= 0x7FFFFFFF;
        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }
        break;
    }
}
#undef cloud
#ifdef MUST_MATCH
#pragma opt_common_subs reset
#endif
#undef MOVE_TO_MARIO

void cloud_exit(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->useStruct != NULL) {
        if (*(u32*)((u8*)party->useStruct + 0x34) != 0xFFFFFFFFu) {
            psndSFXOff(*(s32*)((u8*)party->useStruct + 0x34));
            *(u32*)((u8*)party->useStruct + 0x34) = 0xFFFFFFFFu;
        }
    }

    if (*(void**)((u8*)party + 0x178) != NULL) {
        effSoftDelete((EffectEntry*)*(void**)((u8*)party + 0x178));
        *(void**)((u8*)party + 0x178) = NULL;
    }

    if (party->useStruct != NULL) {
        __memFree(HEAP_DEFAULT, party->useStruct);
        party->useStruct = NULL;
    }

    mario->dispFlags &= ~(1 << 8);
    mario->dispFlags &= ~(1 << 2);
    marioAdjustMoveDir();
    marioChgMot(0);
    mario->unk150.x = 0.01f;
    mario->unk138 = 0.0f;
}

#define cloud ((PartyCloud*)party->useStruct)
#ifdef MUST_MATCH
// Match-only: reading the mode through an inline gives the original's register allocation
// (cloud in r4, mode in r3); the plain dereference in the #else compiles the same but swaps them.
static inline s16 getHitBreatheout2_mode(PartyEntry* party) {
    return *(s16*)((u8*)((PartyCloud*)party->useStruct) + 0x38);
}
#endif

void getHitBreatheout2(PartyEntry* party, f32 angle) {
    MarioWork* mario;
    HitEntry* hit;
    f32 sin2, cos2, sin1, cos1;
    f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;
#ifdef MUST_MATCH
    f32 dz, dx; // reversed declaration order puts dx/dz in the original's stack slots
#else
    f32 dx, dz;
#endif
    f32 f31;
    f32 dir;
    f32 f30;
    s16 mode;

#ifdef MUST_MATCH
    mode = getHitBreatheout2_mode(party);
#else
    mode = *(s16*)((u8*)cloud + 0x38);
#endif
    mario = party->playerPtr;
    if (mode == 0) {
        f30 = 8.0f;
    } else if (mode == 1) {
        f30 = 20.0f;
    } else if (mode == 2) {
        f30 = 40.0f;
    }

    if (cloud->unk8 >= *(f32*)((u8*)cloud + 0xC)) {
        cloud->unk8 = *(f32*)((u8*)cloud + 0xC);
    }

    sincosf(&sin1, &cos1, angle);
    f31 = toMovedir(cloud->unk4);
    sincosf(&sin2, &cos2, f31);

    distance = cloud->unk8;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    cloud->unk20 = (s32)hit;

    dx = 0.0f;
    dz = 0.0f;
    if (*(void**)((u8*)party + 0x178) != NULL) {
        movePos(&dx, &dz, 24.0f, f31);
        *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x4) = party->position.x + dx;
        *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x8) = 18.0f + party->position.y;
        *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0xC) = party->position.z + dz;
        *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x5C) =
            0.6f * (cloud->unk8 / *(f32*)((u8*)cloud + 0xC));

        if (cloud->unk4 >= 90.0f && cloud->unk4 <= 270.0f) {
            *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x58) = -25.0f;
        } else {
            *(f32*)((u8*)((EffectEntry*)*(void**)((u8*)party + 0x178))->userdata + 0x58) = 205.0f;
        }
    }

    if (cloud->unk4 >= 90.0f && cloud->unk4 <= 270.0f) {
        f31 = 1.0f;
    } else {
        f31 = -1.0f;
    }

    if (*(s32*)((u8*)cloud + 0x14) == 0) {
        *(f32*)((u8*)cloud + 0x10) += 1.0f;
        if (*(f32*)((u8*)cloud + 0x10) >= 20.0f) {
            *(f32*)((u8*)cloud + 0x10) = 20.0f;
            *(s32*)((u8*)cloud + 0x14) = 1;
        }
    } else {
        *(f32*)((u8*)cloud + 0x10) -= 1.0f;
        if (*(f32*)((u8*)cloud + 0x10) <= -20.0f) {
            *(f32*)((u8*)cloud + 0x10) = -20.0f;
            *(s32*)((u8*)cloud + 0x14) = 0;
        }
    }

    dir = toMovedir(cloud->unk4 + *(f32*)((u8*)cloud + 0x10) * f31);
    sincosf(&sin2, &cos2, dir);
    distance = cloud->unk8;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    cloud->unk28 = (s32)hit;

    if (*(s32*)((u8*)cloud + 0x1C) == 0) {
        *(f32*)((u8*)cloud + 0x18) += 1.0f;
        if (*(f32*)((u8*)cloud + 0x18) >= 20.0f) {
            *(f32*)((u8*)cloud + 0x18) = 20.0f;
            *(s32*)((u8*)cloud + 0x1C) = 1;
        }
    } else {
        *(f32*)((u8*)cloud + 0x18) -= 1.0f;
        if (*(f32*)((u8*)cloud + 0x18) <= -20.0f) {
            *(f32*)((u8*)cloud + 0x18) = -20.0f;
            *(s32*)((u8*)cloud + 0x1C) = 0;
        }
    }

    dir = toMovedir(cloud->unk4 + *(f32*)((u8*)cloud + 0x18) * f31);
    sincosf(&sin2, &cos2, dir);
    distance = cloud->unk8;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    cloud->unk2C = (s32)hit;
}
#undef cloud

void cloudGetAt(Vec* position) {
    MarioWork* mario = marioGetPtr();
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);

    if (party == NULL || (party->currentMemberId == PARTY_MEMBER_FLURRIE && (party->flags & PARTY_FLAG_IS_BEING_USED) == 0)) {
        return;
    }

    if ((mario->dispFlags & (1 << 20)) == 0) {
        CameraEntry* camera = camGetPtr(CAMERA_3D);

        if(camera->stateA == 0) {
            cloud_at = *position;
        }
        else {
            *position = cloud_at;
        }
    }
}


/// @todo reconstructed: retail's party_cloud .sdata2 starts 0.0f, 70.0f, 180.0f and its int->float conversion
/// double pools first, which needs a function compiled first (last in source) that uses them in that order.
/// Invented: the name and the body (Mario's speed from a stick angle).
#ifdef MUST_MATCH
void cloudSetMarioSpeed(s32 dir) {
    MarioWork* mario = marioGetPtr();
    f32 angle;

    mario->unk138 = 0.0f;
    if (dir >= 0) {
        angle = revise360((f32)dir);
        mario->unk138 = 70.0f;
        if (angle <= 180.0f) {
            mario->unk138 = -mario->unk138;
        }
    }
}
#endif
