#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_party.h"
#include "mario/mario_sbr.h"
#include "party.h"
#include "matching.h"
#include "mario/mario_cam.h"
#include "motion/mot_slit.h"
#include "motion/mot_dokan.h"

// Local prototypes for functions whose headers don't declare them yet. Listed in REPORT.md.

// The original copies each Vec with an unscheduled per-component load/store pair; a volatile cast
// stops MWCC's scheduler hoisting the third component's load (same shape as mot_damageToge).
#ifdef MUST_MATCH
#define COPY_VEC(dst, src) (*(volatile Vec*)(dst) = *(volatile Vec*)(src))
#define STORE_F32V(p, v) (*(volatile f32*)(p) = (v))
#else
#define COPY_VEC(dst, src) (*(dst) = *(src))
#define STORE_F32V(p, v) (*(p) = (v))
#endif

// .rodata named constants, in the order they appear in the original.
#ifdef MUST_MATCH
// Non-const arrays placed in .rodata: MWCC then copies them with the original's r3/r0 load split. A const
// scalar pools differently (r0/r3), which _partyEntry's two copies at offset 0x70 are sensitive to.
__declspec(section ".rodata") Vec vec3_802f3d60[] = { { 0.0f, 0.0f, 0.0f } };
__declspec(section ".rodata") Vec vec3_802f3d6c[] = { { 0.0f, 0.0f, 0.0f } };
#else
const Vec vec3_802f3d60[1] = { { 0.0f, 0.0f, 0.0f } };
const Vec vec3_802f3d6c[1] = { { 0.0f, 0.0f, 0.0f } };
#endif
const Vec vec3_802f3d78 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d84 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d90 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d9c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3da8 = { 0.0f, 16.0f, 0.0f };
const Vec vec3_802f3db4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dc0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dcc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dd8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3de4 = { 0.0f, 16.0f, 0.0f };
const Vec vec3_802f3df0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dfc = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f3e08 = { 1.0f, 1.0f, 1.0f };

void _partyEntry(s32 arg) {
    MarioWork* mario;
    PartyEntry* party;
    s32 i;
    s32 count;
    s32 member;
    f32 angle;
    f32 px;
    f32 py;
    f32 pz;

    mario = marioGetPtr();
    if (mario->flags & 0x02000000) {
        return;
    }
    count = 0;
    if (partyGetPtr(mario->currPartySlotId[0]) != NULL) {
        count = 1;
    }
    if (partyGetPtr(mario->currPartySlotId[1]) != NULL) {
        count++;
    }
    for (i = 0; i < 2; i++) {
        if (mario->currPartySlotId[i] < 0) {
            member = mario->prevPartyMemberId[i];
            px = mario->position.x;
            py = mario->position.y;
            pz = mario->position.z;
            if (member != 0) {
                if (i == 0) {
                    member = partyEntryPos(member, px, py, pz);
                } else {
                    member = partyEntry2Pos(member, px, py, pz);
                }
                if (member >= 0) {
                    mario->currPartySlotId[i] = member;
                    party = partyGetPtr(member);
                    partyChgRunMode(party, 0xA);
                    if (arg == 0) {
                        party->field_70 = vec3_802f3d60[0];
                    }
                    party->flags |= PARTY_FLAG_DISABLE_FLOORS;
                    party->flags |= PARTY_FLAG_FORCE_FALL;
                    if (count == 1) {
                        angle = 180.0f;
                    } else {
                        angle = 120.0f;
                    }
                    party->flags2 &= ~1;
                    party->field_110 = revise360(mario->unk1A8 + angle);
                    party->field_10C = mario->unk1A8;
                    party->field_FC = mario->moveDir;
                }
            }
        } else {
            party = partyGetPtr(mario->currPartySlotId[i]);
            partyChgRunMode(party, 0xA);
            if (arg == 0) {
                party->field_70 = vec3_802f3d6c[0];
            }
            party->flags |= PARTY_FLAG_DISABLE_FLOORS;
            party->flags |= PARTY_FLAG_FORCE_FALL;
            if (count == 1) {
                angle = 180.0f;
            } else {
                angle = -120.0f;
            }
            party->flags2 &= ~1;
            party->field_110 = revise360(mario->unk1A8 + angle);
            party->field_10C = mario->unk1A8;
            party->field_FC = mario->moveDir;
        }
    }
}

// Two scheduling shapes the original's compiler emitted that MWCC only reproduces with help:
//  - Every Vec copy is an unscheduled load x/load y/store x/store y/load z/store z, so COPY_VEC reads both
//    sides volatile to stop the scheduler hoisting the third component's load (same shape as mot_damageToge).
//  - In the sub-motion-1 branch the second spinSpeed store is volatile (STORE_F32V): it keeps that branch's scale
//    copy below both spinSpeed stores, which also settles the f1/f2/f0 assignment.
// The float constants still pool into anonymous @NNN .sdata2 slots (MWCC folds consts defined above the code);
// objdiff matches those by value, so they do not affect the score.
void mot_dokan(void) {
    MarioWork* mario;
    PartyEntry* party;

    mario = marioGetPtr();
    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->dispFlags |= 0x1000;
        mario->dispFlags |= 8;
        mario->dispFlags |= 0x100;
        mario->field_0x2A4 = 0.0f;
        mario->wCamVal1 = mario->position.z;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->baseSpeed = 0.0f;
        if (mario->field_0x3E == 2) {
            mario->currSubMotionId = 0xA;
        } else if (mario->field_0x3E == 3) {
            mario->currSubMotionId = 0xA;
        } else if (mario->field_0x3E == 1) {
            marioPaperOn("p_dokan_y");
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            marioPaperLightOff();
            mario->dispFlags |= 8;
            mario->dispFlags |= 0x100;
            mario->unk1A8 = mario->dispReqDir;
            mario->currSubMotionId = 0x14;
            mario->spinSpeed = 0.0f;
            STORE_F32V(&mario->spinSpeed, 30.0f);
            COPY_VEC(&mario->scale, &vec3_802f3d78);
            _partyEntry(1);
        } else if (mario->field_0x3E == 0) {
            _partyEntry(0);
            mario->currSubMotionId = 0x1E;
        } else if (mario->field_0x3E == 5) {
            mario->dispFlags |= 0x1000;
            if ((party = partyGetPtr(0)) != NULL) {
                partyShadowOff(party);
            }
            if ((party = partyGetPtr(1)) != NULL) {
                partyShadowOff(party);
            }
            COPY_VEC(&mario->dispOfs, &vec3_802f3d84);
            COPY_VEC(&mario->dispOfs2, &vec3_802f3d90);
            COPY_VEC(&mario->dispRot, &vec3_802f3d9c);
            COPY_VEC(&mario->dispPivot, &vec3_802f3da8);
            marioSlitForceCancel();
            marioPaperOn("p_dokan_x");
            marioPaperLightOff();
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            mario->unk1A8 = mario->dispReqDir;
            mario->spinSpeed = 30.0f;
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0;
        } else if (mario->field_0x3E == 4) {
            COPY_VEC(&mario->scale, &vec3_802f3db4);
            COPY_VEC(&mario->dispOfs, &vec3_802f3dc0);
            COPY_VEC(&mario->dispOfs2, &vec3_802f3dcc);
            COPY_VEC(&mario->dispRot, &vec3_802f3dd8);
            COPY_VEC(&mario->dispPivot, &vec3_802f3de4);
            mario->directionView = toMovedir(mario->unk1A8);
            mario->moveDir = mario->directionView;
            marioPaperOn("p_dokan_x");
            marioPaperLightOff();
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            mario->unk1A8 = mario->dispReqDir;
            mario->spinSpeed = 30.0f;
            mario->currSubMotionId = 0x28;
            _partyEntry(1);
            mario->dispFlags |= 0x1000;
            partyShadowOff(partyGetPtr(marioGetPartyId()));
            partyShadowOff(partyGetPtr(marioGetExtraPartyId()));
        }
    }

    switch (mario->currSubMotionId) {
    case 0xA:
        mario->unk1A8 = revise360(mario->unk1A8 + mario->spinSpeed);
        mario->scale.x += -0.005f;
        if (mario->scale.x < 0.0f) {
            mario->scale.x = 0.0f;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        break;
    case 0x14:
        COPY_VEC(&mario->scale, &vec3_802f3df0);
        mario->currSubMotionId = 0x15;
        if (mario->flags & 0x02000000) {
            mario->dispOfs2.y = -2.0f;
        }
        // fallthrough
    case 0x15:
        mario->unk1A8 = revise360(mario->unk1A8 + mario->spinSpeed);
        if (mario->flags & 0x02000000) {
            mario->scale.x += 0.03f;
            if (mario->scale.x >= 1.0f) {
                mario->scale.x = 1.0f;
            }
            mario->scale.y = mario->scale.x;
            mario->scale.z = mario->scale.x;
        } else {
            mario->scale.x += 0.05f;
            if (mario->scale.x >= 1.0f) {
                mario->scale.x = 1.0f;
            }
            mario->scale.y = mario->scale.x;
            mario->scale.z = mario->scale.x;
        }
        break;
    case 0x1E:
        mario->unk1A8 = revise360(mario->unk1A8 + mario->spinSpeed);
        break;
    case 0x0:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        mario->dispRot.x += mario->spinSpeed;
        mario->scale.x += -0.02f;
        if (mario->scale.x < 0.0f) {
            mario->scale.x = 0.0f;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        mario->dispPivot.y = 16.0f * mario->scale.x;
        mario->dispOfs.y = 16.0f - mario->dispPivot.y;
        mario->dispOfs.z = -3.0f;
        break;
    case 0x28:
        mario->dispRot.x += mario->spinSpeed;
        mario->scale.x += 0.03f;
        if (mario->scale.x >= 1.0f) {
            mario->scale.x = 1.0f;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        mario->dispPivot.y = 16.0f * mario->scale.x;
        mario->dispOfs.y = 16.0f - mario->dispPivot.y;
        mario->dispOfs.z = -3.0f;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            party->position = mario->position;
            party->field_70.x = mario->scale.x;
            party->field_70.y = mario->scale.y;
            party->field_70.z = mario->scale.z;
            party->spinPivot.y = mario->dispPivot.y;
            party->dispOffset.y = mario->dispOfs.y;
            party->dispOffset.z = mario->dispOfs.z;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            party->position = mario->position;
            party->field_70.x = mario->scale.x;
            party->field_70.y = mario->scale.y;
            party->field_70.z = mario->scale.z;
            party->spinPivot.y = mario->dispPivot.y;
            party->dispOffset.y = mario->dispOfs.y;
            party->dispOffset.z = mario->dispOfs.z;
        }
        mario->field_EC.y = mario->position.y;
        break;
    case 0x64:
        if (mario->field_0x3E == 1) {
            mario->currSubMotionId = 0x6E;
        } else if (mario->field_0x3E == 4) {
            marioUpdateCamPos();
            mario->dispRot.x = 0.0f;
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            partyDokanEnd(marioGetPartyId(), mario->field_0x3E);
            partyDokanEnd(marioGetExtraPartyId(), mario->field_0x3E);
        } else if (mario->field_0x3E == 1) {
            mario->currSubMotionId = 0x65;
        } else if (mario->field_0x3E == 0) {
            marioUpdateCamPos();
            mario->currSubMotionId = 0x65;
        } else {
            mario->currSubMotionId = 0x66;
        }
        break;
    case 0x65:
        mario->unk1A8 += mario->spinSpeed;
        if (mario->unk1A8 >= 540.0f) {
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            mario->dispRot.x = 0.0f;
            mario->dispReqDir = revise360(180.0f + mario->dispReqDir);
            mario->unk1A8 = mario->dispReqDir;
            mario->directionView = toMovedirSimple(mario->unk1A8);
            mario->moveDir = mario->directionView;
            partyDokanEnd(marioGetPartyId(), mario->field_0x3E);
            partyDokanEnd(marioGetExtraPartyId(), mario->field_0x3E);
        }
        break;
    case 0x66:
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x67;
        }
        break;
    case 0x67:
        COPY_VEC(&mario->field_0xE0, &mario->position);
        COPY_VEC(&mario->scale, &vec3_802f3dfc);
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->unk1A8 = revise360(mario->unk1A8);
        mario->dispReqDir = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        mario->moveDir = mario->directionView;
        if (mario->field_0x3E == 4) {
            camFollowYOn();
            mario->field_EC.y = mario->position.y;
        }
        mario->flags &= ~0x100;
        break;
    default:
        break;
    }

    switch (mario->currSubMotionId) {
    case 0x6E:
        if (!(mario->flags & 0x02000000)) {
            mario->field_EC.y = mario->position.y;
        }
        marioUpdateCamPos();
        camFollowYOff();
        mario->currSubMotionId = 0x6F;
        // fallthrough
    case 0x6F:
        mario->unk1A8 += mario->spinSpeed;
        if (mario->unk1A8 >= 540.0f) {
            if (!(mario->flags & 0x02000000)) {
                camFollowYOn();
            }
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x70;
            mario->dispRot.x = 0.0f;
            mario->dispReqDir = revise360(mario->unk1A8);
            mario->unk1A8 = mario->dispReqDir;
            mario->directionView = toMovedirSimple(mario->unk1A8);
            mario->moveDir = mario->directionView;
            partyDokanEnd(marioGetPartyId(), mario->field_0x3E);
            partyDokanEnd(marioGetExtraPartyId(), mario->field_0x3E);
        }
        break;
    case 0x70:
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x71;
        }
        break;
    case 0x71:
        COPY_VEC(&mario->field_0xE0, &mario->position);
        COPY_VEC(&mario->scale, &vec3_802f3e08);
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->unk1A8 = revise360(mario->unk1A8);
        mario->dispReqDir = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        mario->moveDir = mario->directionView;
        if (!(mario->flags & 0x02000000)) {
            camFollowYOn();
        }
        mario->flags &= ~0x100;
        break;
    default:
        break;
    }
}
