#include "party/party_bomhei.h"
#include "party.h"
#include "party/party_move.h"
#include "party/party_hit.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_sbr.h"
#include "mario/mario_motion.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include "system.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/casedrv.h"
#include "driver/npcdrv.h"
#include "driver/animdrv.h"
#include "manager/evtmgr.h"
#include "evt/evt_cmd.h"
#include "evt/evt_cam.h"
#include "eff/eff_sandars.h"
#include "motion/mot_damage.h"
#include "memory.h"
#include "matching.h"
#include <string.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>

// .rodata / .sdata2 data referenced by this file's functions.
const Vec vec3_802ccf48 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ccf54 = {0.0f, -1.0f, 0.0f};
#ifdef MUST_MATCH
// Layout padding only, nothing references it: fills the original's slot between the two Vecs.
const s32 dat_802ccf60[8] = {0, 45, -45, 90, -90, 135, -135, 180};
#endif

// .data / .sdata objects bomhei_use_main references.  m_w_7_sync is the frame table the
// M_W_7 pose follows; the two evt scripts are the short/long camera shakes.
s32 m_w_7_sync[45] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9, 8, 7, 5, 3, 1, 0,
    0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 9, 8, 6, 4, 3, 2, 1, 0, 0, 0, 0, 0};
s32 longQuake_evt[8] = {0x0005005B, (s32)evt_cam_shake, 4, (s32)0xF24A7A8F, (s32)0xF24A7A8F, 0x258, 2, 1};
static s32 quake_evt[8] = {0x0005005B, (s32)evt_cam_shake, 4, (s32)0xF24A7A8F, (s32)0xF24A7A8F, 0xC8, 2, 1};

// The per-use work block stored in PartyEntry::useStruct (0x170) while Bobbery is active.
typedef struct BomheiWork {
    u32 flags; // 0x0
    s32 timer; // 0x4
} BomheiWork;

// .text:0x2BE4 | 0x800BFBE8 | size: 0xA4
void bomhei_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
        party->playerPtr->flags &= ~0x2000;
        if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.countdownSfxId);
            party->tail.bobbery.countdownSfxId = -1;
        }
        if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.alarmSfxId);
            party->tail.bobbery.alarmSfxId = -1;
        }
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 0xE);
    }
}

// .text:0x2BAC | 0x800BFBB0 | size: 0x38
void bomhei_init_800BFBB0(PartyEntry* party) {
    party->field_33 = 3;
    party->field_32 = 0;
    party->height = 28.0f;
    party->radius = 28.0f;
    party->tail.bobbery.marioMotion = 100;
    party->tail.bobbery.field_17C = -1;
    party->tail.bobbery.countdownSfxId = -1;
    party->tail.bobbery.alarmSfxId = -1;
}


// .text:0x2B48 | 0x800BFB4C | size: 0x64
s32 bomheiGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    s32 status = 2;

    if (party == NULL || (party->currentMemberId == PARTY_MEMBER_BOBBERY && !(party->flags & PARTY_FLAG_IS_BEING_USED))) {
        return 0;
    }
    if (party->useMotionId == 0x3C) {
        status = 1;
    }
    return status;
}

// .text:0x2A3C | 0x800BFA40 | size: 0x10C
void bomhei_exit(PartyEntry* party) {
    party->playerPtr->flags &= ~0x2000;
    party = partyGetPtr(marioGetPartyId());
    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_BOBBERY) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    party->playerPtr->flags &= ~0x2000;
    if (party->field_34 != 9) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    }
    party->flags2 &= ~0x10;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags &= ~PARTY_FLAG_IS_EVT_USE;
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.countdownSfxId);
        party->tail.bobbery.countdownSfxId = -1;
    }
    if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.alarmSfxId);
        party->tail.bobbery.alarmSfxId = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
}

// .text:0x2940 | 0x800BF944 | size: 0xFC
void bomhei_use_cancel(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_BOBBERY) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    party->playerPtr->flags &= ~0x2000;
    if (party->field_34 != 9) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    }
    party->flags2 &= ~0x10;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags &= ~PARTY_FLAG_IS_EVT_USE;
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.countdownSfxId);
        party->tail.bobbery.countdownSfxId = -1;
    }
    if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.alarmSfxId);
        party->tail.bobbery.alarmSfxId = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
}

// .text:0xE20 | 0x800BDE58 | size: 0x1318
// Common tail repeated after several states: if one of the three hit objects has the
// "hit wall" attribute, switch to state 0x3C (or 0x28 for the soft variant).
static inline void bomhei_checkHit(PartyEntry* party) {
    if (party->field_12C != NULL && (hitGetAttr((HitEntry*)party->field_12C) & 0xC00) != 0) {
        party->useMotionId = 0x3C;
    } else if (party->headHit != NULL && (hitGetAttr((HitEntry*)party->headHit) & 0xC00) != 0) {
        party->useMotionId = 0x3C;
    } else if (party->groundHit != NULL) {
        s32 attr = hitGetAttr((HitEntry*)party->groundHit);
        if ((attr & 0xC00) != 0) {
            party->useMotionId = 0x3C;
        } else if ((attr & 0x200) != 0) {
            party->useMotionId = 0x28;
        }
    }
}

// .text:0x2830 | 0x800BF834 | size: 0x110
void bomhei_use_cancel2(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party == NULL) {
        return;
    }
    if (party->currentMemberId != PARTY_MEMBER_BOBBERY) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return;
    }
    if (!(party->playerPtr->flags & 0x2000)) {
        return;
    }
    party->playerPtr->flags &= ~0x2000;
    marioChgPose("M_S_1");
    if (party->field_34 != 9) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    }
    party->flags2 &= ~0x10;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags &= ~PARTY_FLAG_IS_EVT_USE;
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.countdownSfxId);
        party->tail.bobbery.countdownSfxId = -1;
    }
    if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.alarmSfxId);
        party->tail.bobbery.alarmSfxId = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
}
// .text:0x273C | 0x800BF6D4 | size: 0xF4
void bomhei_use_post(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->useMotionId >= 0x46 && party->useMotionId < 0x64) {
        partyGetAppearPos(party, &party->position);
    }
    if (mario->flags & 0x2000) {
        const char* pose;
        mario->flags &= ~0x2000;
        if (mario->currMotionId == 1) {
            pose = "M_W_1";
        } else if (mario->currMotionId == 2) {
            pose = "M_R_1";
        } else if (mario->flags & 0x10000) {
#ifdef MUST_MATCH
            // Natural form is "M_J_1B"; the extra \0 gives the target an 8-byte object.
            pose = "M_J_1B\0";
#else
            pose = "M_J_1B";
#endif
        } else {
            pose = "M_S_1";
        }
        marioChgPose(pose);
    }
    party->flags2 &= ~0x10;
    party->flags &= ~PARTY_FLAG_FORCE_FALL;
    party->flags &= ~PARTY_FLAG_IS_EVT_USE;
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    if (party->field_34 != 9) {
        party->field_3B = 0;
        party->field_8 |= 8;
    }
    party->playerPtr->flags &= ~0x2000;
    if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.countdownSfxId);
        party->tail.bobbery.countdownSfxId = -1;
    }
    if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
        psndSFXOff(party->tail.bobbery.alarmSfxId);
        party->tail.bobbery.alarmSfxId = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
}

// .text:0x2138 | 0x800BF170 | size: 0x604
// Matches: the six jump-stand string references pool off vec3_802ccf48 (+0x68)
// once dat_802ccf60 and the int->float doubles below occupy their original
// .rodata slots.
void bomhei_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    PartyEntry* p2;

    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        bomhei_use_main(party);
        if ((party->useMotionId >= 0x1E && party->useMotionId <= 0x20) ||
            (party->useMotionId >= 0x32 && party->useMotionId <= 0x33) || party->useMotionId == 0x3C) {
            partyChkGnd(party);
            if (party->flags & PARTY_FLAG_IS_GROUNDED) {
                partyChgMot(party, 0);
                party->jumpPara1 = 0.0f;
            }
            if (strcmp(gp->currentMapName, "muj_05") == 0) {
                if (party->groundHit != NULL && strcmp(hitGetName(party->groundHit), "A_dokuro_bero") == 0) {
                    party->dir = angleABf(party->position.x, party->position.z, 638.0f, 12.5f);
                    party->field_FC = party->dir;
                }
            }
        }
        if (party->tail.bobbery.marioMotion != 0x66) {
            if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
                party->tail.bobbery.marioMotion = 0x66;
                return;
            }
            mario_controll(party);
        }
        if (party->useMotionId <= 0xA && mario->hitobjStandOn != NULL &&
            (hitGetAttr(mario->hitobjStandOn) & 0x80000000)) {
            MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjStandOn);
            if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0 || strcmp(obj->animName, "MOBJ_RedJumpStand2") == 0 ||
                strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0 || strcmp(obj->animName, "MOBJ_BlueJumpStand2") == 0) {
                p2 = partyGetPtr(marioGetPartyId());
                if (p2 == NULL) {
                    return;
                }
                if (p2->currentMemberId != PARTY_MEMBER_BOBBERY) {
                    return;
                }
                if (!(p2->flags & PARTY_FLAG_IS_BEING_USED)) {
                    return;
                }
                p2->playerPtr->flags &= ~0x2000;
                if (p2->field_34 != 9) {
                    partyChgRunMode(p2, 2);
                    p2->field_3B = 0;
                }
                p2->flags2 &= ~0x10;
                p2->flags &= ~PARTY_FLAG_FORCE_FALL;
                p2->flags &= ~PARTY_FLAG_IS_EVT_USE;
                p2->flags &= ~PARTY_FLAG_IS_BEING_USED;
                if ((u32)p2->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
                    psndSFXOff(p2->tail.bobbery.countdownSfxId);
                    p2->tail.bobbery.countdownSfxId = -1;
                }
                if ((u32)p2->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
                    psndSFXOff(p2->tail.bobbery.alarmSfxId);
                    p2->tail.bobbery.alarmSfxId = -1;
                }
                if (p2->useStruct != NULL) {
                    __memFree(0, p2->useStruct);
                    p2->useStruct = NULL;
                }
                return;
            }
        }
        if (!(party->flags & PARTY_FLAG_IS_EVT_USE) && marioKeyOffChk()) {
            p2 = partyGetPtr(marioGetPartyId());
            if (p2 == NULL) {
                return;
            }
            if (p2->currentMemberId != PARTY_MEMBER_BOBBERY) {
                return;
            }
            if (!(p2->flags & PARTY_FLAG_IS_BEING_USED)) {
                return;
            }
            p2->playerPtr->flags &= ~0x2000;
            if (p2->field_34 != 9) {
                partyChgRunMode(p2, 2);
                p2->field_3B = 0;
            }
            p2->flags2 &= ~0x10;
            p2->flags &= ~PARTY_FLAG_FORCE_FALL;
            p2->flags &= ~PARTY_FLAG_IS_EVT_USE;
            p2->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if ((u32)p2->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.countdownSfxId);
                p2->tail.bobbery.countdownSfxId = -1;
            }
            if ((u32)p2->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.alarmSfxId);
                p2->tail.bobbery.alarmSfxId = -1;
            }
            if (p2->useStruct != NULL) {
                __memFree(0, p2->useStruct);
                p2->useStruct = NULL;
            }
            return;
        }
        if (mario->forceMoveTimer != 0) {
            p2 = partyGetPtr(marioGetPartyId());
            if (p2 == NULL) {
                return;
            }
            if (p2->currentMemberId != PARTY_MEMBER_BOBBERY) {
                return;
            }
            if (!(p2->flags & PARTY_FLAG_IS_BEING_USED)) {
                return;
            }
            p2->playerPtr->flags &= ~0x2000;
            if (p2->field_34 != 9) {
                partyChgRunMode(p2, 2);
                p2->field_3B = 0;
            }
            p2->flags2 &= ~0x10;
            p2->flags &= ~PARTY_FLAG_FORCE_FALL;
            p2->flags &= ~PARTY_FLAG_IS_EVT_USE;
            p2->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if ((u32)p2->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.countdownSfxId);
                p2->tail.bobbery.countdownSfxId = -1;
            }
            if ((u32)p2->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.alarmSfxId);
                p2->tail.bobbery.alarmSfxId = -1;
            }
            if (p2->useStruct != NULL) {
                __memFree(0, p2->useStruct);
                p2->useStruct = NULL;
            }
            return;
        }
        if (party->useMotionId <= 0xA && (mario->dispFlags & 0x01000000)) {
            p2 = partyGetPtr(marioGetPartyId());
            if (p2 == NULL) {
                return;
            }
            if (p2->currentMemberId != PARTY_MEMBER_BOBBERY) {
                return;
            }
            if (!(p2->flags & PARTY_FLAG_IS_BEING_USED)) {
                return;
            }
            p2->playerPtr->flags &= ~0x2000;
            if (p2->field_34 != 9) {
                partyChgRunMode(p2, 2);
                p2->field_3B = 0;
            }
            p2->flags2 &= ~0x10;
            p2->flags &= ~PARTY_FLAG_FORCE_FALL;
            p2->flags &= ~PARTY_FLAG_IS_EVT_USE;
            p2->flags &= ~PARTY_FLAG_IS_BEING_USED;
            if ((u32)p2->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.countdownSfxId);
                p2->tail.bobbery.countdownSfxId = -1;
            }
            if ((u32)p2->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
                psndSFXOff(p2->tail.bobbery.alarmSfxId);
                p2->tail.bobbery.alarmSfxId = -1;
            }
            if (p2->useStruct != NULL) {
                __memFree(0, p2->useStruct);
                p2->useStruct = NULL;
            }
        }
    }
}

#pragma opt_common_subs off // without it MWCC CSE-hoists party+0x94 across movePos into r28/r29 (party_cloud does the same)
void bomhei_use_main(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    f32 f31;
    Vec pos50;
    f32 targetX, targetZ;
    Vec splash;
    Vec pos47;
    Vec pos51;
    f32 sx47, sy47, sz47;
    f32 sx51, sy51, sz51;

    if ((party->field_8 & 0x8) != 0) {
        party->field_8 &= ~0x8;
        party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
        party->flags &= ~0x03000000;
        party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        party->useStruct = __memAlloc(HEAP_DEFAULT, 8);
        memset(party->useStruct, 0, 8);
        party->useMotionId = 0;
    }

    switch (party->useMotionId) {
    case 0:
        party->useMotionId = 1;
        party->tail.bobbery.marioMotion = 100;
        party->tail.bobbery.motionTimer = 0;
        party->timer = 4;
        ((BomheiWork*)party->useStruct)->timer = 0;
        ((BomheiWork*)party->useStruct)->flags = 0;
        partyChgPose(party, (char*)"R_1");
        /* fallthrough */
    case 1: {
        party->targetPos = mario->position;
        movePos(&party->targetPos.x, &party->targetPos.z, -6.0f, toMovedirSimple(mario->dispReqDir));
        party->position.x += (party->targetPos.x - party->position.x) / (f32)party->timer;
        party->position.y += (party->targetPos.y - party->position.y) / (f32)party->timer;
        party->position.z += (party->targetPos.z - party->position.z) / (f32)party->timer;
        if (--party->timer > 0) {
            break;
        }
        party->timer = 9;
        targetX = mario->position.x;
        targetZ = mario->position.z;
        movePos(&targetX, &targetZ, -6.0f, toMovedirSimple(mario->dispReqDir));
        party->speed = distABf(party->position.x, party->position.z, targetX, targetZ) / 18.0f;
        party->dir = angleABf(party->position.x, party->position.z, targetX, targetZ);
        party->field_FC = party->dir;
        party->jumpPara1 = 7.5f;
        // The temp stops -O4,p folding the fneg into the pool word.
        {
            f32 gravity = 0.8333333f;
            party->jumpPara2 = -gravity;
        }
        partyChgPose(party, (char*)"J_1A");
        party->useMotionId = 2;
        party->tail.bobbery.marioMotion = 0xA;
        break;
    }
    case 2:
        party->position.y += party->jumpPara1;
        party->jumpPara1 += party->jumpPara2;
        if (--party->timer <= 0) {
            party->flags2 |= 0x10;
            partyChgPose(party, (char*)"L_1");
            ((BomheiWork*)party->useStruct)->timer = (s16)sysMsec2Frame(3000);
            party->useMotionId = 0xA;
            party->tail.bobbery.marioMotion = 0x14;
        }
        break;
    case 0xA:
        if ((((BomheiWork*)party->useStruct)->flags & 2) != 0) {
            partyChgPose(party, (char*)"L_2");
        }
        {
            MarioWork* pm = party->playerPtr;

            f31 = 36.0f;
            if (strcmp(pm->animName, (char*)"M_W_7") == 0) {
                f32 val = (f32)m_w_7_sync[party->tail.bobbery.syncFrame];
                // The temp keeps 0.25 in f0 instead of f2.
                f32 quarter = 0.25f;
                f31 += val * quarter;
            }
            party->position.x = pm->position.x;
            party->position.y = pm->position.y + f31;
            party->position.z = pm->position.z;
            movePos(&party->position.x, &party->position.z, -6.0f, toMovedirSimple(pm->dispReqDir));
            party->position.z += 2.0f;
            party->field_110 = pm->dispReqDir;
            party->field_10C = pm->unk1A8;
        }
        bomhei_checkHit(party);
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) != 0 ||
            (mario->hitobjStandOn != NULL && mario->airTimer == 0 &&
             (mario->buttonHeld & 0x400) == 0)) {
            party->useMotionId = 0x14;
        }
        break;
    default:
        break;
    }

    switch (party->useMotionId) {
    case 0x14: {
        psndSFXOn_3D((char*)0x939, &party->playerPtr->position);
        party->tail.bobbery.marioMotion = 0x1E;
        party->useMotionId = 0x15;
        targetX = party->position.x;
        targetZ = party->position.z;
        movePos(&targetX, &targetZ, 38.0f, toMovedirSimple(mario->dispReqDir));
        party->speed = distABf(party->position.x, party->position.z, targetX, targetZ) / 20.0f;
        party->dir = angleABf(party->position.x, party->position.z, targetX, targetZ);
        party->field_FC = party->dir;
        party->jumpPara1 = 3.5555556f;
        // The temp stops -O4,p folding the fneg into the pool word.
        {
            f32 gravity = 0.35555556f;
            party->jumpPara2 = -gravity;
        }
#ifdef MUST_MATCH
        // Natural form is (s16)10.5f; the temp stops the conversion folding to an integer.
        {
            f32 frames = 10.5f;
            *(s32*)&party->field_22[2] = (s16)frames;
        }
#else
        *(s32*)&party->field_22[2] = (s16)10.5f;
#endif
        party->flags2 &= ~0x10;
        party->flags |= PARTY_FLAG_IS_JUMPING;
        bomhei_checkHit(party);
        break;
    }
    case 0x15:
        partyMoveNoHosei(party);
        if (--party->timer > 0) {
            bomhei_checkHit(party);
            break;
        }
        party->jumpPara1 = 0.0f;
        party->jumpPara2 = -0.33333334f;
#ifdef MUST_MATCH
        // Natural form is (s16)10.5f; the temp stops the conversion folding to an integer.
        {
            f32 frames = 10.5f;
            *(s32*)&party->field_22[2] = (s16)frames;
        }
#else
        *(s32*)&party->field_22[2] = (s16)10.5f;
#endif
        party->flags &= ~PARTY_FLAG_IS_JUMPING;
        if (party->groundHit != NULL) {
            party->useMotionId = 0x1E;
        } else {
            party->useMotionId = 0x16;
        }
        bomhei_checkHit(party);
        break;
    case 0x16: {
        s32 found;

        partyMoveNoHosei(party);
        if (party->groundHit != NULL) {
            if (party->field_12C != NULL && (hitGetAttr((HitEntry*)party->field_12C) & 0xC00) != 0) {
                party->useMotionId = 0x3C;
                found = TRUE;
            } else if (party->headHit != NULL && (hitGetAttr((HitEntry*)party->headHit) & 0xC00) != 0) {
                party->useMotionId = 0x3C;
                found = TRUE;
            } else if (party->groundHit != NULL) {
                s32 attr = hitGetAttr((HitEntry*)party->groundHit);

                if ((attr & 0xC00) != 0) {
                    party->useMotionId = 0x3C;
                    found = TRUE;
                } else if ((attr & 0x200) != 0) {
                    party->useMotionId = 0x28;
                    found = TRUE;
                } else {
                    goto bomhei16_false;
                }
            } else {
                goto bomhei16_false;
            }
            goto bomhei16_done;
        bomhei16_false:
            found = FALSE;
        bomhei16_done:
            if (found) {
                break;
            }
            party->useMotionId = ((((BomheiWork*)party->useStruct)->flags & 4) == 0) ? 0x1E : 0x32;
        }
        break;
    }
    case 0x1E:
        party->timer = sysMsec2Frame(200);
        party->useMotionId = 0x1F;
        /* fallthrough */
    case 0x1F:
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0 && (gp->buttonNew[0] & 0x400) != 0) {
            ((BomheiWork*)party->useStruct)->timer = (s16)(sysMsec2Frame(500) + 1);
            party->useMotionId = 0x32;
        }
        partyChgPose(party, (char*)"S_1");
        if (--party->timer > 0) {
            bomhei_checkHit(party);
            break;
        }
        party->useMotionId = 0x20;
        /* fallthrough */
    case 0x20:
        if ((party->flags & PARTY_FLAG_IS_EVT_USE) == 0 && (gp->buttonNew[0] & 0x400) != 0) {
            ((BomheiWork*)party->useStruct)->timer = (s16)(sysMsec2Frame(500) + 1);
        }
        partyChgPose(party, (!(((BomheiWork*)party->useStruct)->flags & 2)) ? (char*)"W_1" : (char*)"W_2");
        party->speed = 0.6f;
        partyMove(party);
        if (((BomheiWork*)party->useStruct)->timer <= 0) {
            party->useMotionId = 0x32;
        }
        bomhei_checkHit(party);
        break;
    default:
        break;
    }

    if (party->useMotionId < 0x32) {
        if (((BomheiWork*)party->useStruct)->timer > 0) {
            ((BomheiWork*)party->useStruct)->timer--;
            if (((BomheiWork*)party->useStruct)->timer == (s16)sysMsec2Frame(2500)) {
                ((BomheiWork*)party->useStruct)->flags |= 2;
                if ((u32)party->tail.bobbery.alarmSfxId == 0xFFFFFFFF) {
                    party->tail.bobbery.countdownSfxId = psndSFXOn_3D((char*)0x93C, &party->playerPtr->position);
                }
            } else if (((BomheiWork*)party->useStruct)->timer == (s16)sysMsec2Frame(500)) {
                party->useMotionId = 0x32;
                if ((mario->flags & 0x2000) != 0) {
                    party->tail.bobbery.marioMotion = 0x28;
                } else {
                    party->useMotionId = 0x32;
                }
            }
        }
    }

    switch (party->useMotionId) {
    case 0x28: {
        if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.countdownSfxId);
            party->tail.bobbery.countdownSfxId = -1;
        }
        splash = party->position;
        marioSetSplash(0, &splash);
        ((BomheiWork*)party->useStruct)->flags |= 8;
        ((BomheiWork*)party->useStruct)->timer = (s16)(sysMsec2Frame(500) + 0x1E);
        ((BomheiWork*)party->useStruct)->flags |= 2;
        party->flags |= PARTY_FLAG_FORCE_FALL;
        party->useMotionId = 0x29;
        party->timer = 6;
        break;
    }
    case 0x29:
        party->position.y += party->jumpPara1;
        party->jumpPara1 += party->jumpPara2;
        if (--party->timer <= 0) {
            party->useMotionId = 0x2A;
            party->jumpPara1 = 0.0f;
            party->jumpPara2 = 0.0f;
        }
        /* fallthrough */
    case 0x2A:
        {
            f32 tmp = 0.1f * (-1.0f - party->speed);
            party->speed += tmp;
        }
        if (party->speed <= 0.0f) {
            party->speed = 0.0f;
        }
        break;
    default:
        break;
    }

    switch (party->useMotionId) {
    case 0x32:
        if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.countdownSfxId);
            party->tail.bobbery.countdownSfxId = -1;
        }
        if ((u32)party->tail.bobbery.alarmSfxId == 0xFFFFFFFF) {
            party->tail.bobbery.alarmSfxId = psndSFXOn_3D((char*)0x93D, &party->playerPtr->position);
        }
        partyChgPose(party, (char*)"A_3");
        party->useMotionId = 0x33;
        party->timer = 0x28;
        /* fallthrough */
    case 0x33:
        if ((mario->flags & 0x2000) == 0) {
            if ((party->flags & PARTY_FLAG_IS_GROUNDED) != 0) {
                party->speed = 0.0f;
            }
            partyMove(party);
        } else {
            MarioWork* pm = party->playerPtr;

            f31 = 36.0f;
            if (strcmp(pm->animName, (char*)"M_W_7") == 0) {
                f32 val = (f32)m_w_7_sync[party->tail.bobbery.syncFrame];
                // The temp keeps 0.25 in f0 instead of f2.
                f32 quarter = 0.25f;
                f31 += val * quarter;
            }
            party->position.x = pm->position.x;
            party->position.y = pm->position.y + f31;
            party->position.z = pm->position.z;
            movePos(&party->position.x, &party->position.z, -6.0f, toMovedirSimple(pm->dispReqDir));
            party->position.z += 2.0f;
            party->field_110 = pm->dispReqDir;
            party->field_10C = pm->unk1A8;
        }
        if (--party->timer > 0) {
            bomhei_checkHit(party);
            break;
        }
        if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.alarmSfxId);
            party->tail.bobbery.alarmSfxId = -1;
        }
        ((BomheiWork*)party->useStruct)->flags &= ~0x2;
        party->frontHit = check_front(party);
        if ((mario->flags & 0x2000) != 0) {
            mario->flags &= ~0x2000;
        }
        party->flags &= ~PARTY_FLAG_IS_FALLING;
        ((BomheiWork*)party->useStruct)->flags &= ~0x8;
        party->useMotionId = 0x3C;
        break;
    default:
        break;
    }

    switch (party->useMotionId) {
    case 0x3C: {
        s32 found;

        if ((u32)party->tail.bobbery.countdownSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.countdownSfxId);
            party->tail.bobbery.countdownSfxId = -1;
        }
        if ((u32)party->tail.bobbery.alarmSfxId != 0xFFFFFFFF) {
            psndSFXOff(party->tail.bobbery.alarmSfxId);
            party->tail.bobbery.alarmSfxId = -1;
        }
        psndSFXOn_3D((char*)0x93B, &party->position);
        ((BomheiWork*)party->useStruct)->timer = 0;
        party->speed = 0.0f;
        partyMove(party);
        hit = party->frontHit;
        if (hit == NULL) {
            found = FALSE;
        } else if ((hitGetAttr(hit) & 0x80000000) != 0) {
            if (strcmp(mobjHitObjPtrToPtr(hit)->animName, "MOBJ_BombRock") != 0) {
                goto bomhei3c_false;
            }
            found = TRUE;
        } else {
            CaseEntry* cs = caseCheckHitObj2(hit);
            if (cs == NULL || cs->activeConditionId != 0xF) {
                goto bomhei3c_false;
            }
            found = TRUE;
        }
        goto bomhei3c_done;
    bomhei3c_false:
        found = FALSE;
    bomhei3c_done:
        if (found) {
            evtEntryType(longQuake_evt, 0, 0, 0);
        } else {
            evtEntryType(quake_evt, 0, 0, 0);
        }
        {
            EffectEntry* eff = effSandarsEntry(pouchGetPartyAttackLv(3), party->position.x,
                                               party->position.y + party->height,
                                               party->position.z, 0.0f);
            ((effSandarsData*)eff->userdata)->camId = party->camId;
        }
        fbatSandersBombTriggerOn();
        party->useMotionId = 0x46;
        break;
    }
    case 0x46:
        party->speed = 0.0f;
        party->jumpPara1 = 8.0f;
        party->jumpPara2 = 0.0f;
        party->timer = 0x3C;
        party->useMotionId = 0x47;
        /* fallthrough */
    case 0x47: {
        party->position.y += party->jumpPara1;
        pos47 = party->position;
        marioGetScreenPos(pos47, &sx47, &sy47, &sz47);
        if (sy47 <= -300.0f) {
            party->timer = sysMsec2Frame(800);
            party->useMotionId = 0x50;
        }
        break;
    }
    case 0x50:
        if (--party->timer <= 0) {
            partyGetAppearPos(party, &pos50);
            party->position.x = pos50.x;
            party->position.y = 200.0f + pos50.y;
            party->position.z = pos50.z;
            party->jumpPara1 = 0.0f;
            party->jumpPara2 = -1.0f;
            party->speed = 0.0f;
            party->flags &= ~PARTY_FLAG_IS_JUMPING;
            party->flags |= PARTY_FLAG_IS_FALLING;
            party->flags |= PARTY_FLAG_FORCE_FALL;
            partyChgPose(party, (char*)"J_1B");
            party->useMotionId = 0x51;
        }
        break;
    case 0x51: {
        if (mario->position.y + 37.0f > party->position.y) {
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
        }
        hit = partySearchGround(party, 11.0f, party->jumpPara1);
        f31 = (f32)mario->unk16C - 25.0f;
        pos51 = party->position;
        marioGetScreenPos(pos51, &sx51, &sy51, &sz51);
        if (sy51 > f31) {
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
        } else if (hit != NULL) {
            party->groundHit = hit;
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
        }
        partyMove(party);
        if (party->jumpPara1 <= -6.0f) {
            party->jumpPara2 = 0.0f;
            party->jumpPara1 = -6.0f;
        }
        if ((party->flags & PARTY_FLAG_FORCE_FALL) != 0) {
            break;
        }
        if (party->position.y < -2000.0f) {
            party->position = mario->position;
            party->position.y += 200.0f;
            party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
        }
        if (party->groundHit != NULL) {
            if ((((u32)hitGetAttr((HitEntry*)party->groundHit) >> 9) & 1) == 0) {
                partyChgPose(party, (char*)"F_1");
                party->useMotionId = 0x52;
                party->timer = 0x1E;
                psndSFXOn_3D((char*)0x93A, &party->position);
            }
        }
        break;
    }
    case 0x52:
        if (--party->timer <= 0) {
            party->useMotionId = 0x64;
        }
        partyMove(party);
        break;
    case 0x64:
        partyChgRunMode(party, 2);
        party->field_3B = 0;
        party->flags2 &= ~0x10;
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        if (party->useStruct != NULL) {
            __memFree(HEAP_DEFAULT, party->useStruct);
            party->useStruct = NULL;
        }
        break;
    default:
        break;
    }
}
#pragma opt_common_subs reset


// .text:0xD3C | 0x800BDD74 | size: 0xE4
BOOL mario_bomhei_keychk(void) {
    PartyEntry* party;
    u8 motion;
    u8 pose;

    marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (party == NULL) {
        return TRUE;
    }
    if (party->currentMemberId != PARTY_MEMBER_BOBBERY) {
        return TRUE;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return TRUE;
    }
    motion = party->tail.bobbery.marioMotion;
    if (motion == 0xA || (motion >= 0x14 && motion <= 0x15) || motion == 0x65 ||
        (motion >= 0x28 && motion <= 0x29) || (motion >= 0x1E && motion <= 0x24)) {
        return FALSE;
    }
    pose = party->useMotionId;
    if ((pose >= 0x32 && pose <= 0x33) || pose == 0x3C || (pose >= 0x46 && pose <= 0x47)) {
        return FALSE;
    }
    return TRUE;
}

// .text:0xD10 | 0x800BDCE4 | size: 0x2C
BOOL mario_bomhei_keychk2(void) {
    PartyEntry* party;

    marioGetPtr();
    party = partyGetPtr(marioGetPartyId());
    if (party == NULL) {
        return TRUE;
    }
    if (party->currentMemberId != PARTY_MEMBER_BOBBERY) {
        return TRUE;
    }
    if (!(party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return TRUE;
    }
    if (party->tail.bobbery.marioMotion == 0xA || party->tail.bobbery.marioMotion == 0x65 ||
        (party->tail.bobbery.marioMotion >= 0x1E && party->tail.bobbery.marioMotion <= 0x24)) {
        return FALSE;
    }
    return TRUE;
}

// .text:0xC7C | 0x800BDACC | size: 0x94
void mario_controll(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    u8 motion = party->tail.bobbery.marioMotion;

    switch (motion) {
    case 0xA:
        marioAdjustMoveDir();
        marioChgPose("M_U_2");
        mario->flags |= 0x2000;
        party->tail.bobbery.marioMotion = 0x65;
        break;
    case 0x14:
        mario->flags |= 0x2000;
        party->tail.bobbery.marioMotion = 0x15;
        break;
    case 0x15:
        if (strcmp(mario->animName, "M_W_7") == 0) {
            int t;
            animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()],
                                 (f32)party->tail.bobbery.syncFrame);
            t = party->tail.bobbery.syncFrame + 1;
            party->tail.bobbery.syncFrame = t;
            if ((u8)t >= 0x28) {
                party->tail.bobbery.syncFrame = 0;
            }
        } else {
            party->tail.bobbery.syncFrame = 0;
        }
        break;
    case 0x28:
        marioChgMot(0);
        party->tail.bobbery.marioMotion = 0x29;
        party->tail.bobbery.motionTimer = 0x28;
        break;
    case 0x29: {
        s16 t = --party->tail.bobbery.motionTimer;
        if (t <= 0) {
            marioChgMot(0);
            party->tail.bobbery.marioMotion = 0x64;
        }
        break;
    }
    case 0x1E:
        marioChgMot(0);
        party->tail.bobbery.marioMotion = 0x1F;
        break;
    case 0x1F:
        marioChgPose("M_C_1");
        mario->flags &= ~0x2000;
        party->tail.bobbery.motionTimer = 0x32;
        party->tail.bobbery.marioMotion = 0x20;
        break;
    case 0x20: {
        s16 t = --party->tail.bobbery.motionTimer;
        if (t <= 0) {
            party->tail.bobbery.marioMotion = 0x24;
            party->tail.bobbery.motionTimer = 0xA;
        }
        break;
    }
    case 0x24: {
        s16 t;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)party->tail.bobbery.motionTimer);
        t = --party->tail.bobbery.motionTimer;
        if (t < 0) {
            marioChgMot(0);
            party->tail.bobbery.marioMotion = 0x64;
        }
        break;
    }
    default:
        break;
    }
}

// 100%. Register colouring matches: attempt-0 `radius0` is its own variable, attempt 1 reuses a
// function-scope `ang` while attempt 2 keeps its own block-scoped `ang`, and the declarations are
// ordered `radius, md, angle, ang, px, pz, radius0` -- that is what puts attempt-0 radius/radius0 in
// f29, angle in f30, attempt-1/2 radius in f31 and md in f30. Behaviour is identical to one variable.
// .text:0x3FC | 0x800BD24C | size: 0x880
void partyGetReturnPos(Vec* pos) {
    MarioWork* mario = marioGetPtr();
    HitCheckQuery query1;
    HitCheckQuery query2;
    HitCheckQuery query3;
    HitEntry* hit;
    Vec v, dir0, dir1, dir2;
    f32 radius, md, angle, ang, px, pz, radius0;
    f32 sin, cos;
    f32 hx, hy, hz, dist, nx, ny, nz;
    f32 z0, x0, z1, x1, z2, x2;
    s32 ok;

    // attempt 0: along Mario's target yaw with a 6-unit probe
    px = mario->position.x;
    radius0 = 2.0f + mario->position.y;
    pz = mario->position.z;
    dist = mario->wPlayerCollisionBox.x;
    angle = mario->targetYaw;
    sincosf(&sin, &cos, angle);
    hit = hitCheckFilter(NULL, px, radius0, pz, sin, 0.0f, cos, &hx, &hy, &hz, &dist, &nx, &ny, &nz);
    if (hit == NULL) {
        v = mario->position;
        marioGetPtr();
        x0 = v.x;
        z0 = v.z;
        movePos(&x0, &z0, 6.0f, angle);
        query1.targetDistance = 4.0f;
        dir0 = vec3_802ccf48;
        dir0.x = x0;
        dir0.y = 2.0f + v.y;
        dir0.z = z0;
        query1.targetPos = dir0;
        query1.targetDir = vec3_802ccf54;
        hit = hitCheckVecFilter(&query1, NULL);
        if (hit != NULL && !(hitGetAttr(hit) & 0xE00)) {
            v.x = (f32)(s32)(1000.0f * query1.hitPos.x + (query1.hitPos.x >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.y = (f32)(s32)(1000.0f * query1.hitPos.y + (query1.hitPos.y >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.z = (f32)(s32)(1000.0f * query1.hitPos.z + (query1.hitPos.z >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            ok = 1;
        } else {
            ok = 0;
        }
        if (ok) {
            pos->x = v.x;
            pos->y = v.y;
            pos->z = v.z;
            return;
        }
    }

    // attempt 1: opposite of Mario's movement direction
    px = mario->position.x;
    radius = 2.0f + mario->position.y;
    pz = mario->position.z;
    dist = mario->wPlayerCollisionBox.x;
    {
    ang = revise360(toMovedirSimple(mario->dispReqDir) - 180.0f);
    sincosf(&sin, &cos, ang);
    hit = hitCheckFilter(NULL, px, radius, pz, sin, 0.0f, cos, &hx, &hy, &hz, &dist, &nx, &ny, &nz);
    if (hit == NULL) {
        md = mario->wPlayerCollisionBox.x;
        v = mario->position;
        marioGetPtr();
        x1 = v.x;
        z1 = v.z;
        movePos(&x1, &z1, md, ang);
        query2.targetDistance = 4.0f;
        dir1 = vec3_802ccf48;
        dir1.x = x1;
        dir1.y = 2.0f + v.y;
        dir1.z = z1;
        query2.targetPos = dir1;
        query2.targetDir = vec3_802ccf54;
        hit = hitCheckVecFilter(&query2, NULL);
        if (hit != NULL && !(hitGetAttr(hit) & 0xE00)) {
            v.x = (f32)(s32)(1000.0f * query2.hitPos.x + (query2.hitPos.x >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.y = (f32)(s32)(1000.0f * query2.hitPos.y + (query2.hitPos.y >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.z = (f32)(s32)(1000.0f * query2.hitPos.z + (query2.hitPos.z >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            ok = 1;
        } else {
            ok = 0;
        }
        if (ok) {
            pos->x = v.x;
            pos->y = v.y;
            pos->z = v.z;
            return;
        }
    }
    }

    // attempt 2: along Mario's movement direction
    { f32 ang;
    ang = toMovedirSimple(mario->dispReqDir);
    sincosf(&sin, &cos, ang);
    hit = hitCheckFilter(NULL, px, radius, pz, sin, 0.0f, cos, &hx, &hy, &hz, &dist, &nx, &ny, &nz);
    if (hit == NULL) {
        { f32 md2 = mario->wPlayerCollisionBox.x;
        v = mario->position;
        marioGetPtr();
        x2 = v.x;
        z2 = v.z;
        movePos(&x2, &z2, md2, ang); }
        query3.targetDistance = 4.0f;
        dir2 = vec3_802ccf48;
        dir2.x = x2;
        dir2.y = 2.0f + v.y;
        dir2.z = z2;
        query3.targetPos = dir2;
        query3.targetDir = vec3_802ccf54;
        hit = hitCheckVecFilter(&query3, NULL);
        if (hit != NULL && !(hitGetAttr(hit) & 0xE00)) {
            v.x = (f32)(s32)(1000.0f * query3.hitPos.x + (query3.hitPos.x >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.y = (f32)(s32)(1000.0f * query3.hitPos.y + (query3.hitPos.y >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            v.z = (f32)(s32)(1000.0f * query3.hitPos.z + (query3.hitPos.z >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
            ok = 1;
        } else {
            ok = 0;
        }
        if (ok) {
            pos->x = v.x;
            pos->y = v.y;
            pos->z = v.z;
            return;
        }
    }
    }

    // fallback: 180 degrees from Mario's target yaw
    px = mario->position.x;
    pz = mario->position.z;
    angle = revise360(180.0f + mario->targetYaw);
    movePos(&px, &pz, mario->wPlayerCollisionBox.x, angle);
    pos->x = px;
    pos->y = mario->position.y;
    pos->z = pz;
}

// field_12C is a HitEntry* stored in a void* field.

// 100%. Requires the unit's data described above: the jump-stand strings are inline literals (pooled
// after the compiler's own int->float magic double at .rodata+0x58), and the functions are listed in
// the reverse of the target .text order. Both are needed for the two `lfd` of the magic double to be
// at 0x58(r30) instead of an anonymous copy at 0xd8.
// .text:0x0 | 0x800BCE50 | size: 0x3FC
HitEntry* check_front(PartyEntry* party) {
#ifdef MUST_MATCH
    s32 angles[8];
#else
    s32 angles[8] = {0, 45, -45, 90, -90, 135, -135, 180};
#endif
    HitCheckQuery query;
    HitEntry* hit2;
    HitEntry* field138;
    HitEntry* hit;
    HitEntry* ret = NULL;
    HitEntry* ret2 = NULL;
    u32 i;
    f32 base;
    f32 f29;
    f32 limit;

#ifdef MUST_MATCH
    // Match-only: a 32-byte struct copy makes MWCC emit the stack copy straight
    // from the named dat_802ccf60 array; the plain array initializer emits a
    // second anonymous template at .rodata 0xd8.
    {
        typedef struct { s32 v[8]; } S32x8;
        *(S32x8*)angles = *(const S32x8*)dat_802ccf60;
    }
#endif
    f29 = 0.8f * *(f32*)&party->field_E8[0x8];
    base = partyToMovedir(party, party->field_10C);
    for (i = 0; i < 8; i++) {
        sincosf(&query.targetDir.x, &query.targetDir.z, base + (f32)angles[i]);
        query.targetDir.y = 0.0f;
        query.targetPos = party->position;
        query.targetPos.y += f29;
        query.targetDistance = 100.0f;
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL) {
            limit = revise360(base + (f32)angles[i] - camGetPtr(4)->viewYaw);
            if ((limit >= 60.0f && limit <= 120.0f) || (limit >= 240.0f && limit <= 300.0f)) {
                limit = 50.0f;
            } else {
                limit = 30.0f;
            }
            if (query.targetDistance <= limit) {
#ifdef MUST_MATCH
                // Match-only: the do-while's breaks make MWCC share one FALSE
                // block between the strcmp else and the caseCheck else; the
                // plain if/else duplicates the `li r0,0; b`.
                s32 accepted;
                do {
                    s32 attr;
                    CaseEntry* cs;
                    if (hit == NULL) {
                        accepted = FALSE;
                        break;
                    }
                    attr = hitGetAttr(hit);
                    if (attr & 0x80000000) {
                        if (strcmp(mobjHitObjPtrToPtr(hit)->animName, "MOBJ_BombRock") == 0) {
                            accepted = TRUE;
                            break;
                        }
                    } else {
                        cs = caseCheckHitObj2(hit);
                        if (cs != NULL && cs->activeConditionId == 0xF) {
                            accepted = TRUE;
                            break;
                        }
                    }
                    accepted = FALSE;
                } while (0);
#else
                s32 accepted;
                if (hit == NULL) {
                    accepted = FALSE;
                } else {
                    s32 attr = hitGetAttr(hit);
                    if (attr & 0x80000000) {
                        if (strcmp(mobjHitObjPtrToPtr(hit)->animName, "MOBJ_BombRock") == 0) {
                            accepted = TRUE;
                        } else {
                            accepted = FALSE;
                        }
                    } else {
                        CaseEntry* cs = caseCheckHitObj2(hit);
                        if (cs != NULL && cs->activeConditionId == 0xF) {
                            accepted = TRUE;
                        } else {
                            accepted = FALSE;
                        }
                    }
                }
#endif
                if (accepted) {
                    return hit;
                }
            }
            ret = NULL;
        }
    }
    query.targetPos = party->position;
    query.targetDir = (Vec){0.0f, -1.0f, 0.0f};
    query.targetDistance = 2.0f + f29;
    hit2 = hitCheckVecFilter(&query, NULL);
    if (hit2 != NULL) {
#ifdef MUST_MATCH
        s32 accepted;
        do {
            s32 attr;
            CaseEntry* cs;
            if (hit2 == NULL) {
                accepted = FALSE;
                break;
            }
            attr = hitGetAttr(hit2);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(hit2)->animName, "MOBJ_BombRock") == 0) {
                    accepted = TRUE;
                    break;
                }
            } else {
                cs = caseCheckHitObj2(hit2);
                if (cs != NULL && cs->activeConditionId == 0xF) {
                    accepted = TRUE;
                    break;
                }
            }
            accepted = FALSE;
        } while (0);
#else
        s32 accepted;
        if (hit2 == NULL) {
            accepted = FALSE;
        } else {
            s32 attr = hitGetAttr(hit2);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(hit2)->animName, "MOBJ_BombRock") == 0) {
                    accepted = TRUE;
                } else {
                    accepted = FALSE;
                }
            } else {
                CaseEntry* cs = caseCheckHitObj2(hit2);
                if (cs != NULL && cs->activeConditionId == 0xF) {
                    accepted = TRUE;
                } else {
                    accepted = FALSE;
                }
            }
        }
#endif
        if (accepted) {
            return hit2;
        }
        ret2 = hit2;
    }
    field138 = party->groundHit;
    if (field138 != NULL) {
#ifdef MUST_MATCH
        s32 accepted;
        do {
            s32 attr;
            CaseEntry* cs;
            if (field138 == NULL) {
                accepted = FALSE;
                break;
            }
            attr = hitGetAttr(field138);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(field138)->animName, "MOBJ_BombRock") == 0) {
                    accepted = TRUE;
                    break;
                }
            } else {
                cs = caseCheckHitObj2(field138);
                if (cs != NULL && cs->activeConditionId == 0xF) {
                    accepted = TRUE;
                    break;
                }
            }
            accepted = FALSE;
        } while (0);
#else
        s32 accepted;
        if (field138 == NULL) {
            accepted = FALSE;
        } else {
            s32 attr = hitGetAttr(field138);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(field138)->animName, "MOBJ_BombRock") == 0) {
                    accepted = TRUE;
                } else {
                    accepted = FALSE;
                }
            } else {
                CaseEntry* cs = caseCheckHitObj2(field138);
                if (cs != NULL && cs->activeConditionId == 0xF) {
                    accepted = TRUE;
                } else {
                    accepted = FALSE;
                }
            }
        }
#endif
        if (accepted) {
            return field138;
        }
    }
    if (ret != NULL) {
        return ret;
    }
    if (ret2 != NULL) {
        return ret2;
    }
    return field138;
}
