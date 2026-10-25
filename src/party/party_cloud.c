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
    mario->camShiftRate.x = 0.01f;
    mario->camShiftSideTarget = 0.0f;
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
        PartyCloud* cloud = party->partyCloud;
        breathDir = toMovedir(cloud->breathDispDir);
    }
    
    return breathDir;
}

f32 cloudGetBreathPower(f32 param_1, Vec* param_2) {
    PartyEntry* party = partyGetPtr(PARTY_SLOT_PARTY);
    f32 breathPower;
    f32 dist;
    f32 angle;
    f32 breathDist;
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
        PartyCloud* cloud = party->partyCloud;
        breathDist = cloud->breathDist;
        if (-((0.5f * param_1) - distABf(party->position.x, party->position.z, param_2->x, param_2->z)) <= breathDist) {
            PartyCloud* cloud = party->partyCloud;
            dist = toMovedir(cloud->breathDispDir);
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

HitEntry* cloudGetHitBreathout(s32 param_1) {
    u32 partyId = marioGetPartyId();
    PartyEntry* party = partyGetPtr(partyId);
    HitEntry* hitBreathout;

    if (party == NULL) {
        return NULL;
    }

    if (
        party->currentMemberId != PARTY_MEMBER_FLURRIE 
        || (party->flags & PARTY_FLAG_IS_BEING_USED) == 0
        || party->useMotionId < MARIO_MOTION_FALL
    ){
            return NULL;
    }

    hitBreathout = NULL;
    if (param_1 == 0) {
        hitBreathout = party->partyCloud->hitBreathout[0];
    }
    else if (param_1 == 1) { 
        hitBreathout = party->partyCloud->hitBreathout[1];
    }
    else if (param_1 == 2) {
        hitBreathout = party->partyCloud->hitBreathout[2];
    }
    else if (param_1 == 3) { 
        hitBreathout = party->partyCloud->hitBreathout[3];
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
        PartyCloud* cloud = party->partyCloud;
        breathDist = cloud->breathDist;
    }

    return breathDist;
}

void cloudBobInit(PartyEntry* party) {
    party->tail.flurrie.bobAngle = 0.0f;
    party->tail.flurrie.bobStep = 4.25f;
}

void cloudBobUpdate(PartyEntry* party) {
    f32 bob;

    party->tail.flurrie.bobAngle = revise360(party->tail.flurrie.bobAngle - party->tail.flurrie.bobStep);
    bob = sin((3.1415927f * party->tail.flurrie.bobAngle) / 180.0f);
    party->dispOffset.y = 3.0f + 3.0f * bob;
}

void cloud_move(PartyEntry* party) {
    f32 bob;

    if ((party->trigFlags & PARTY_TRIG_FLAG_CHG_RUN_MODE) != 0) {
        party->trigFlags &= ~PARTY_TRIG_FLAG_CHG_RUN_MODE;
        partyMoveFlyInit(party, 1);
        party->tail.flurrie.bobAngle = 0.0f;
        party->tail.flurrie.bobStep = 4.25f;
    }

    partyFlyMain(party);

    party->tail.flurrie.bobAngle = revise360(party->tail.flurrie.bobAngle - party->tail.flurrie.bobStep);
    bob = sin((3.1415927f * party->tail.flurrie.bobAngle) / 180.0f);
    party->dispOffset.y = 3.0f + 3.0f * bob;

    if (party->moveMode != 2) {
        party->trigFlags &= ~0x40000;
    }
}

void cloud_init(PartyEntry* party) {
    party->partnerType = PARTY_MEMBER_FLURRIE;
    party->isFlyer = 1;
    party->height = 38.0f;
    party->radius = 24.0f;
    party->tail.flurrie.field_0x174 = 0;
    party->tail.flurrie.breathFx = NULL;
    *(s32*)&party->tail.flurrie.bobAngle = 0;
    *(s32*)&party->tail.flurrie.bobStep = 0;
    party->useStruct = NULL;
    party->tail.flurrie.breathFx = NULL;
}

// Writing the flag as `pf = (...)` rather than `if (...) pf = 1;` gives the original's r0
// allocation for pf.
static inline void cloudMoveToMario(PartyEntry* party) {
    MarioWork* pm = party->playerPtr;
    f32 pa;
    s32 pf;
    party->targetPos = pm->position;
    pa = pm->dispReqDir;
    pf = (pa < 90.0f || pa >= 270.0f);
    pa = toMovedir(pa + (pf != 0 ? -90.0f : 90.0f));
    movePos(&party->targetPos.x, &party->targetPos.z, 3.3f, pa);
    party->targetPos.y += 10.0f;
    party->position = party->targetPos;
}

// The original spells the last two guards as one `mot <= 1 || mot == 2` condition; MWCC then
// keeps the `mot == 2` arm out of line (`bne body; b end`). The braces below just keep the
// cleanup body in its own scope (and unchanged from the base).
void cloud_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    MarioMotion mot;

    N__cloud_use(party);

    mot = mario->currMotionId;
    if (mot == MARIO_MOTION_CLOUD) {
        return;
    }
    if (mot <= 1 || mot == 2) {
        return;
    }
    {
        party->flags &= ~(1 << 8);

        if (party->useStruct != NULL) {
            if (party->partyCloud->sfxId != 0xFFFFFFFFu) {
                psndSFXOff(party->partyCloud->sfxId);
                party->partyCloud->sfxId = 0xFFFFFFFFu;
            }
        }

        if (party->tail.flurrie.breathFx != NULL) {
            effSoftDelete(party->tail.flurrie.breathFx);
            party->tail.flurrie.breathFx = NULL;
        }

        cloudMoveToMario(party);

        party->flags2 |= 0x10;
        party->flags &= 0x7FFFFFFF;

        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }

        mot = mario->currMotionId;
        if (mot == MARIO_MOTION_DAMAGE || mot == MARIO_MOTION_DAMAGE_TOGE) {
            partyChgRunMode(party, 0x0D);
        } else {
            partyChgPose(party, partyChgPoseId(party, 1));
            partyChgRunMode(party, 2);
            partyChgMot(party, 0);
        }
    }
}

static inline void cloudMoveToMarioStep(PartyEntry* party) {
    MarioWork* pm = party->playerPtr;
    f32 pa;
    s32 pf;
    party->targetPos = pm->position;
    pa = pm->dispReqDir;
    pf = (pa < 90.0f || pa >= 270.0f);
    pa = toMovedir(pa + (pf != 0 ? -90.0f : 90.0f));
    movePos(&party->targetPos.x, &party->targetPos.z, 3.3f, pa);
    party->targetPos.y += 10.0f;
}
#define MOVE_TO_MARIO() cloudMoveToMarioStep(party)

#ifdef MUST_MATCH
// Match-only: the original re-reads party->playerPtr at the party->partyCloud->breathDist clamp (lwz r30, 0x160(r29));
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

    if ((party->trigFlags & PARTY_TRIG_FLAG_CHG_RUN_MODE) != 0) {
        party->trigFlags &= ~PARTY_TRIG_FLAG_CHG_RUN_MODE;
        *(s32*)&party->tail.flurrie.bobAngle = 0;
        *(s32*)&party->tail.flurrie.bobStep = 0;
        marioAdjustMoveDir();
        party->useMotionId = 0;
        party->useStruct = __memAlloc(HEAP_DEFAULT, 0x48);
        memset(party->useStruct, 0, 0x48);
        party->partyCloud->field_0x0 = 100.0f;
        party->partyCloud->maxDist = 120.0f;
        party->partyCloud->sweepOffset1 = 0.0f;
        party->partyCloud->sweepDown1 = 0;
        party->partyCloud->sweepOffset2 = 0.0f;
        party->partyCloud->sweepDown2 = 1;
        party->tail.flurrie.breathFx = NULL;
        if (party->useStruct != NULL) {
            party->partyCloud->sfxId = 0xFFFFFFFFu;
        }
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) != 0) {
            party->partyCloud->useTimer = sysMsec2Frame(party->evtUseMsec);
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
        party->dispDir = mario->dispDir;
        party->dispReqDir = mario->dispReqDir;
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
        if (party->tail.flurrie.breathFx == NULL) {
            party->tail.flurrie.breathFx = effKumokumoN64Entry(0, 0, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, 0.0f);
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->camera = party->camId;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color1R = 0x4C;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color1G = 0xA7;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color1B = 0xCA;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color2R = 0x7D;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color2G = 0x1B;
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->color2B = 0xF5;
        }
        marioChgMot(0x1B);
        marioChgPose("M_A_2A");
        partyChgPose(party, "PWD_A_5");
        party->timer = 0x10;
        party->useMotionId = 0xB;
        mario->dispFlags |= 0x4;
        mario->dispFlags |= 0x100;
        party->flags2 |= 0x10;
        party->partyCloud->breathDist = 0.0f;
        party->partyCloud->breathDispDir = mario->dispDir;
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            mario->camShiftRate.x = 0.03f;
            mario->camShiftSideTarget = 70.0f * ((revise360(270.0f - mario->dispReqDir) <= 180.0f) ? -1.0f : 1.0f);
        }
        if (party->useStruct != NULL && party->partyCloud->sfxId == 0xFFFFFFFFu) {
            party->partyCloud->sfxId = psndSFXOn_3D((char*)0x930, &party->playerPtr->position);
        }
        /* fallthrough */
    case 0xB:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        if (--party->timer > 0) {
            break;
        }
        partyChgPose(party, "PWD_A_6");
        marioChgPose("M_A_2B");
        party->useMotionId = 0xC;
        /* fallthrough */
    case 0xC:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        partyUpdateKeyData(party);
        mario->dispDir = party->partyCloud->breathDispDir;
        mario->dispReqDir = party->partyCloud->breathDispDir;
        party->dispDir = party->partyCloud->breathDispDir;
        party->dispReqDir = party->partyCloud->breathDispDir;
        sx = mario->position.x;
        sz = mario->position.z;
        f31 = mario->dispDir;
        flag = 0;
        if (f31 < 90.0f || f31 >= 270.0f) { flag = 1; }
        f31 = toMovedir(f31 + (flag ? -90.0f : 90.0f));
        movePos(&sx, &sz, 3.3f, f31);
        party->position.x = sx;
        party->position.z = sz;
        party->position.y = mario->position.y + 10.0f;
        party->dispOffset = (Vec){0.0f, 0.0f, 0.0f};
        party->partyCloud->breathDist += 2.0f;
        RELOAD_MARIO();
        if (party->partyCloud->breathDist >= party->partyCloud->maxDist) {
            party->partyCloud->breathDist = party->partyCloud->maxDist;
        }
        angle = toMovedir(party->partyCloud->breathDispDir);
        sincosf(&sin1, &cos1, angle);
        sincosf(&sin2, &cos2, -45.0f);
        dist = party->partyCloud->breathDist;
        hit = hitCheckFilter(NULL, party->position.x, 18.0f + mario->position.y, party->position.z, sin1, sin2, cos1, &hitX,
                             &hitY, &hitZ, &dist,
                             &hitNX, &hitNY, &hitNZ);
        if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
            hit = NULL;
        }
        party->partyCloud->hitBreathout[1] = hit;
        getHitBreatheout2(party, 0.0f);
        if (++party->partyCloud->mode > 2) {
            party->partyCloud->mode = 0;
        }
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            if ((party->button & 0x400) != 0 && marioKeyOffChk() == 0) {
                break;
            }
        } else {
            if (--party->partyCloud->useTimer > 0) {
                break;
            }
        }
        party->partyCloud->hitBreathout[0] = 0;
        party->partyCloud->hitBreathout[1] = 0;
        party->partyCloud->hitBreathout[2] = 0;
        party->partyCloud->hitBreathout[3] = 0;
        party->timer = sysMsec2Frame(200);
        party->useMotionId = 0xD;
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) != 0) {
            party->useMotionId = 0x14;
        }
        if (party->useStruct != NULL && party->partyCloud->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(party->partyCloud->sfxId);
            party->partyCloud->sfxId = 0xFFFFFFFFu;
        }
        break;
    case 0xD:
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        partyUpdateKeyData(party);
        if ((party->buttonNew & 0x400) != 0) {
            party->useMotionId = 0xA;
        }
        if (--party->timer <= 0) {
            party->useMotionId = 0x14;
            break;
        }
        if (party->stickX != 0 || party->stickY != 0) {
            party->useMotionId = 0x14;
        } else {
            break;
        }
        /* fallthrough */
    case 0x14:
        party->flags &= ~(1 << 8);
        if (party->useStruct != NULL && party->partyCloud->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(party->partyCloud->sfxId);
            party->partyCloud->sfxId = 0xFFFFFFFFu;
        }
        if (party->tail.flurrie.breathFx != NULL) {
            effSoftDelete(party->tail.flurrie.breathFx);
            party->tail.flurrie.breathFx = NULL;
        }
        MOVE_TO_MARIO();
        party->position = party->targetPos;
        party->flags2 &= ~0x10;
        if (mario->currMotionId == 0x1B) {
            mario->dispFlags &= ~(1 << 8);
            mario->dispFlags &= ~(1 << 2);
            marioAdjustMoveDir();
            party->trigFlags |= 0x40000;
            mario->dispFlags &= ~(1 << 8);
            marioChgMot(0);
        }
        party->flags2 |= 0x10;
        partyChgRunMode(party, 2);
        party->field_0x3D = 0;
        partyChgMoveMode(party, 2);
        partyChgMot(party, 1);
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0) {
            mario->camShiftRate.x = 0.01f;
            mario->camShiftSideTarget = 0.0f;
        }
        party->flags &= 0x7FFFFFFF;
        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }
        break;
    }
}
#undef MOVE_TO_MARIO

void cloud_exit(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->useStruct != NULL) {
        if (party->partyCloud->sfxId != 0xFFFFFFFFu) {
            psndSFXOff(party->partyCloud->sfxId);
            party->partyCloud->sfxId = 0xFFFFFFFFu;
        }
    }

    if (party->tail.flurrie.breathFx != NULL) {
        effSoftDelete(party->tail.flurrie.breathFx);
        party->tail.flurrie.breathFx = NULL;
    }

    if (party->useStruct != NULL) {
        __memFree(HEAP_DEFAULT, party->useStruct);
        party->useStruct = NULL;
    }

    mario->dispFlags &= ~(1 << 8);
    mario->dispFlags &= ~(1 << 2);
    marioAdjustMoveDir();
    marioChgMot(0);
    mario->camShiftRate.x = 0.01f;
    mario->camShiftSideTarget = 0.0f;
}

// Reading the mode through an inline gives the original's register allocation
// (cloud in r4, mode in r3); a plain dereference swaps them.
static inline s16 getHitBreatheout2_mode(PartyEntry* party) {
    return party->partyCloud->mode;
}

void getHitBreatheout2(PartyEntry* party, f32 angle) {
    MarioWork* mario;
    HitEntry* hit;
    f32 sin2, cos2, sin1, cos1;
    f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;
    f32 dz, dx; // reversed declaration order puts dx/dz in the original's stack slots
    f32 f31;
    f32 dir;
    f32 f30;
    s16 mode;

    mode = getHitBreatheout2_mode(party);
    mario = party->playerPtr;
    if (mode == 0) {
        f30 = 8.0f;
    } else if (mode == 1) {
        f30 = 20.0f;
    } else if (mode == 2) {
        f30 = 40.0f;
    }

    if (party->partyCloud->breathDist >= party->partyCloud->maxDist) {
        party->partyCloud->breathDist = party->partyCloud->maxDist;
    }

    sincosf(&sin1, &cos1, angle);
    f31 = toMovedir(party->partyCloud->breathDispDir);
    sincosf(&sin2, &cos2, f31);

    distance = party->partyCloud->breathDist;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    party->partyCloud->hitBreathout[0] = hit;

    dx = 0.0f;
    dz = 0.0f;
    if (party->tail.flurrie.breathFx != NULL) {
        movePos(&dx, &dz, 24.0f, f31);
        ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->x = party->position.x + dx;
        ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->y = 18.0f + party->position.y;
        ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->z = party->position.z + dz;
        ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->scale =
            0.6f * (party->partyCloud->breathDist / party->partyCloud->maxDist);

        if (party->partyCloud->breathDispDir >= 90.0f && party->partyCloud->breathDispDir <= 270.0f) {
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->rotation = -25.0f;
        } else {
            ((effKumokumoData*)party->tail.flurrie.breathFx->userdata)->rotation = 205.0f;
        }
    }

    if (party->partyCloud->breathDispDir >= 90.0f && party->partyCloud->breathDispDir <= 270.0f) {
        f31 = 1.0f;
    } else {
        f31 = -1.0f;
    }

    if (party->partyCloud->sweepDown1 == 0) {
        party->partyCloud->sweepOffset1 += 1.0f;
        if (party->partyCloud->sweepOffset1 >= 20.0f) {
            party->partyCloud->sweepOffset1 = 20.0f;
            party->partyCloud->sweepDown1 = 1;
        }
    } else {
        party->partyCloud->sweepOffset1 -= 1.0f;
        if (party->partyCloud->sweepOffset1 <= -20.0f) {
            party->partyCloud->sweepOffset1 = -20.0f;
            party->partyCloud->sweepDown1 = 0;
        }
    }

    dir = toMovedir(party->partyCloud->breathDispDir + party->partyCloud->sweepOffset1 * f31);
    sincosf(&sin2, &cos2, dir);
    distance = party->partyCloud->breathDist;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    party->partyCloud->hitBreathout[2] = hit;

    if (party->partyCloud->sweepDown2 == 0) {
        party->partyCloud->sweepOffset2 += 1.0f;
        if (party->partyCloud->sweepOffset2 >= 20.0f) {
            party->partyCloud->sweepOffset2 = 20.0f;
            party->partyCloud->sweepDown2 = 1;
        }
    } else {
        party->partyCloud->sweepOffset2 -= 1.0f;
        if (party->partyCloud->sweepOffset2 <= -20.0f) {
            party->partyCloud->sweepOffset2 = -20.0f;
            party->partyCloud->sweepDown2 = 0;
        }
    }

    dir = toMovedir(party->partyCloud->breathDispDir + party->partyCloud->sweepOffset2 * f31);
    sincosf(&sin2, &cos2, dir);
    distance = party->partyCloud->breathDist;
    hit = hitCheckFilter(NULL, party->position.x, mario->position.y + f30, party->position.z, sin2, sin1, cos2, &hitX,
                         &hitY, &hitZ, &distance,
                         &hitNX, &hitNY, &hitNZ);
    if (hit != NULL && (hitGetAttr(hit) & 4) != 0) {
        hit = NULL;
    }
    party->partyCloud->hitBreathout[3] = hit;
}

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

    mario->camShiftSideTarget = 0.0f;
    if (dir >= 0) {
        angle = revise360((f32)dir);
        mario->camShiftSideTarget = 70.0f;
        if (angle <= 180.0f) {
            mario->camShiftSideTarget = -mario->camShiftSideTarget;
        }
    }
}
#endif
