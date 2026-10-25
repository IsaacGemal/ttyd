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
                        party->scale = (Vec){ 0.0f, 0.0f, 0.0f };
                    }
                    party->flags |= PARTY_FLAG_DISABLE_FLOORS;
                    party->flags |= PARTY_FLAG_FORCE_FALL;
                    if (count == 1) {
                        angle = 180.0f;
                    } else {
                        angle = 120.0f;
                    }
                    party->flags2 &= ~1;
                    party->field_110 = revise360(mario->dispDir + angle);
                    party->dispDir = mario->dispDir;
                    party->field_FC = mario->moveDir;
                }
            }
        } else {
            party = partyGetPtr(mario->currPartySlotId[i]);
            partyChgRunMode(party, 0xA);
            if (arg == 0) {
                party->scale = (Vec){ 0.0f, 0.0f, 0.0f };
            }
            party->flags |= PARTY_FLAG_DISABLE_FLOORS;
            party->flags |= PARTY_FLAG_FORCE_FALL;
            if (count == 1) {
                angle = 180.0f;
            } else {
                angle = -120.0f;
            }
            party->flags2 &= ~1;
            party->field_110 = revise360(mario->dispDir + angle);
            party->dispDir = mario->dispDir;
            party->field_FC = mario->moveDir;
        }
    }
}

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
        if (mario->dokanType == 2) {
            mario->currSubMotionId = 0xA;
        } else if (mario->dokanType == 3) {
            mario->currSubMotionId = 0xA;
        } else if (mario->dokanType == 1) {
            marioPaperOn("p_dokan_y");
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            marioPaperLightOff();
            mario->dispFlags |= 8;
            mario->dispFlags |= 0x100;
            mario->dispDir = mario->dispReqDir;
            mario->currSubMotionId = 0x14;
            mario->spinSpeed = 0.0f;
            mario->spinSpeed = 30.0f;
            mario->scale = (Vec){ 0.0f, 0.0f, 0.0f };
            _partyEntry(1);
        } else if (mario->dokanType == 0) {
            _partyEntry(0);
            mario->currSubMotionId = 0x1E;
        } else if (mario->dokanType == 5) {
            mario->dispFlags |= 0x1000;
            if ((party = partyGetPtr(0)) != NULL) {
                partyShadowOff(party);
            }
            if ((party = partyGetPtr(1)) != NULL) {
                partyShadowOff(party);
            }
            mario->dispOfs = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispOfs2 = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispRot = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispPivot = (Vec){ 0.0f, 16.0f, 0.0f };
            marioSlitForceCancel();
            marioPaperOn("p_dokan_x");
            marioPaperLightOff();
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            mario->dispDir = mario->dispReqDir;
            mario->spinSpeed = 30.0f;
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0;
        } else if (mario->dokanType == 4) {
            mario->scale = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispOfs = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispOfs2 = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispRot = (Vec){ 0.0f, 0.0f, 0.0f };
            mario->dispPivot = (Vec){ 0.0f, 16.0f, 0.0f };
            mario->directionView = toMovedir(mario->dispDir);
            mario->moveDir = mario->directionView;
            marioPaperOn("p_dokan_x");
            marioPaperLightOff();
            marioChgPaper("PM_D_1A");
            marioChgPose("M_Z_1");
            mario->dispDir = mario->dispReqDir;
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
        mario->dispDir = revise360(mario->dispDir + mario->spinSpeed);
        mario->scale.x += -0.005f;
        if (mario->scale.x < 0.0f) {
            mario->scale.x = 0.0f;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        break;
    case 0x14:
        mario->scale = (Vec){ 0.0f, 0.0f, 0.0f };
        mario->currSubMotionId = 0x15;
        if (mario->flags & 0x02000000) {
            mario->dispOfs2.y = -2.0f;
        }
        // fallthrough
    case 0x15:
        mario->dispDir = revise360(mario->dispDir + mario->spinSpeed);
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
        mario->dispDir = revise360(mario->dispDir + mario->spinSpeed);
        break;
    case 0x0:
        if (--mario->multiTimer > 0) {
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
            party->scale.x = mario->scale.x;
            party->scale.y = mario->scale.y;
            party->scale.z = mario->scale.z;
            party->spinPivot.y = mario->dispPivot.y;
            party->dispOffset.y = mario->dispOfs.y;
            party->dispOffset.z = mario->dispOfs.z;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            party->position = mario->position;
            party->scale.x = mario->scale.x;
            party->scale.y = mario->scale.y;
            party->scale.z = mario->scale.z;
            party->spinPivot.y = mario->dispPivot.y;
            party->dispOffset.y = mario->dispOfs.y;
            party->dispOffset.z = mario->dispOfs.z;
        }
        mario->camFollowPos.y = mario->position.y;
        break;
    case 0x64:
        if (mario->dokanType == 1) {
            mario->currSubMotionId = 0x6E;
        } else if (mario->dokanType == 4) {
            marioUpdateCamPos();
            mario->dispRot.x = 0.0f;
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            partyDokanEnd(marioGetPartyId(), mario->dokanType);
            partyDokanEnd(marioGetExtraPartyId(), mario->dokanType);
        } else if (mario->dokanType == 1) {
            mario->currSubMotionId = 0x65;
        } else if (mario->dokanType == 0) {
            marioUpdateCamPos();
            mario->currSubMotionId = 0x65;
        } else {
            mario->currSubMotionId = 0x66;
        }
        break;
    case 0x65:
        mario->dispDir += mario->spinSpeed;
        if (mario->dispDir >= 540.0f) {
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            mario->dispRot.x = 0.0f;
            mario->dispReqDir = revise360(180.0f + mario->dispReqDir);
            mario->dispDir = mario->dispReqDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
            mario->moveDir = mario->directionView;
            partyDokanEnd(marioGetPartyId(), mario->dokanType);
            partyDokanEnd(marioGetExtraPartyId(), mario->dokanType);
        }
        break;
    case 0x66:
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x67;
        }
        break;
    case 0x67:
<<<<<<< HEAD
        mario->fallReturnPos = mario->position;
=======
        mario->fallReturnPos = mario->position;
>>>>>>> 95daa1a14 (Event and Mario fields: name urouro, one-message, badge shop and MarioWork words from their readers)
        mario->scale = (Vec){ 1.0f, 1.0f, 1.0f };
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->dispDir = revise360(mario->dispDir);
        mario->dispReqDir = mario->dispDir;
        mario->directionView = toMovedirSimple(mario->dispDir);
        mario->moveDir = mario->directionView;
        if (mario->dokanType == 4) {
            camFollowYOn();
            mario->camFollowPos.y = mario->position.y;
        }
        mario->flags &= ~0x100;
        break;
    default:
        break;
    }

    switch (mario->currSubMotionId) {
    case 0x6E:
        if (!(mario->flags & 0x02000000)) {
            mario->camFollowPos.y = mario->position.y;
        }
        marioUpdateCamPos();
        camFollowYOff();
        mario->currSubMotionId = 0x6F;
        // fallthrough
    case 0x6F:
        mario->dispDir += mario->spinSpeed;
        if (mario->dispDir >= 540.0f) {
            if (!(mario->flags & 0x02000000)) {
                camFollowYOn();
            }
            marioChgPaper("PM_D_1B");
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x70;
            mario->dispRot.x = 0.0f;
            mario->dispReqDir = revise360(mario->dispDir);
            mario->dispDir = mario->dispReqDir;
            mario->directionView = toMovedirSimple(mario->dispDir);
            mario->moveDir = mario->directionView;
            partyDokanEnd(marioGetPartyId(), mario->dokanType);
            partyDokanEnd(marioGetExtraPartyId(), mario->dokanType);
        }
        break;
    case 0x70:
        if (--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x71;
        }
        break;
    case 0x71:
<<<<<<< HEAD
        mario->fallReturnPos = mario->position;
=======
        mario->fallReturnPos = mario->position;
>>>>>>> 95daa1a14 (Event and Mario fields: name urouro, one-message, badge shop and MarioWork words from their readers)
        mario->scale = (Vec){ 1.0f, 1.0f, 1.0f };
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->dispDir = revise360(mario->dispDir);
        mario->dispReqDir = mario->dispDir;
        mario->directionView = toMovedirSimple(mario->dispDir);
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
