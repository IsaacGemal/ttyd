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
const Vec vec3_802ccf80 = {0.0f, -1.0f, 0.0f};
const char str_802CCF8C[] = "MOBJ_BombRock";
const char str_802CCFB0[] = "muj_05";
const char str_802CCFB8[] = "A_dokuro_bero";
const char str_802CCFC8[] = "MOBJ_RedJumpStand";
const char str_802CCFDC[] = "MOBJ_RedJumpStand2";
const char str_802CCFF0[] = "MOBJ_BlueJumpStand";
const char str_802CD004[] = "MOBJ_BlueJumpStand2";
const char str_802CD018[] = "M_J_1B";
const char str_8041B200[] = "M_U_2";
const char str_8041B208[] = "M_W_7";
const char str_8041B210[] = "M_C_1";
const char str_8041B2A8[] = "M_W_1";
const char str_8041B2B0[] = "M_R_1";
const char str_8041B2B8[] = "M_S_1";

// .text:0x0 | 0x800BCE50 | size: 0x3FC
// NONMATCHING (97.09%): each of the three `check hit` blocks emits its own
// `li r0,0; b` for the strcmp else where the original branches to the shared
// false block shared with the caseCheck else. Logic and all other instructions match.
HitEntry* check_front(PartyEntry* party) {
    s32 angles[8] = {0, 45, -45, 90, -90, 135, -135, 180};
    HitCheckQuery query;
    HitEntry* hit;
    HitEntry* hit2;
    HitEntry* ret = NULL;
    HitEntry* ret2 = NULL;
    HitEntry* field138;
    CameraEntry* cam;
    u32 i;
    f32 base;
    f32 f29;
    f32 limit;

    f29 = 0.8f * *(f32*)((u8*)party + 0xF0);
    base = partyToMovedir(party, party->field_10C);
    for (i = 0; i < 8; i++) {
        sincosf(&query.targetDir.x, &query.targetDir.z, base + (f32)angles[i]);
        query.targetDir.y = 0.0f;
        query.targetPos = party->position;
        query.targetPos.y += f29;
        query.targetDistance = 100.0f;
        hit = hitCheckVecFilter(&query, NULL);
        if (hit != NULL) {
            cam = camGetPtr(4);
            limit = revise360(base + (f32)angles[i] - cam->viewYaw);
            if ((limit >= 60.0f && limit <= 120.0f) || (limit >= 240.0f && limit <= 300.0f)) {
                limit = 50.0f;
            } else {
                limit = 30.0f;
            }
            if (query.targetDistance <= limit) {
                s32 accepted;
                if (hit == NULL) {
                    accepted = FALSE;
                } else {
                    s32 attr = hitGetAttr(hit);
                    if (attr & 0x80000000) {
                        if (strcmp(mobjHitObjPtrToPtr(hit)->animName, str_802CCF8C) == 0) {
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
                if (accepted) {
                    return hit;
                }
            }
            ret = NULL;
        }
    }
    query.targetPos = party->position;
    query.targetDir = vec3_802ccf80;
    query.targetDistance = 2.0f + f29;
    hit2 = hitCheckVecFilter(&query, NULL);
    if (hit2 != NULL) {
        s32 accepted;
        if (hit2 == NULL) {
            accepted = FALSE;
        } else {
            s32 attr = hitGetAttr(hit2);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(hit2)->animName, str_802CCF8C) == 0) {
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
        if (accepted) {
            return hit2;
        }
        ret2 = hit2;
    }
    field138 = party->field_138;
    if (field138 != NULL) {
        s32 accepted;
        if (field138 == NULL) {
            accepted = FALSE;
        } else {
            s32 attr = hitGetAttr(field138);
            if (attr & 0x80000000) {
                if (strcmp(mobjHitObjPtrToPtr(field138)->animName, str_802CCF8C) == 0) {
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

// .text:0x3FC | 0x800BD24C | size: 0x880
// partyGetReturnPos (TODO)

// .text:0xC7C | 0x800BDACC | size: 0x94
void mario_controll(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    u8 motion = *(u8*)((u8*)party + 0x17A);

    switch (motion) {
    case 0xA:
        marioAdjustMoveDir();
        marioChgPose(str_8041B200);
        mario->flags |= 0x2000;
        *(u8*)((u8*)party + 0x17A) = 0x65;
        break;
    case 0x14:
        mario->flags |= 0x2000;
        *(u8*)((u8*)party + 0x17A) = 0x15;
        break;
    case 0x15:
        if (strcmp(mario->animName, str_8041B208) == 0) {
            int t;
            animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()],
                                 (f32)*(u8*)((u8*)party + 0x17B));
            t = *(u8*)((u8*)party + 0x17B) + 1;
            *(u8*)((u8*)party + 0x17B) = t;
            if ((u8)t >= 0x28) {
                *(u8*)((u8*)party + 0x17B) = 0;
            }
        } else {
            *(u8*)((u8*)party + 0x17B) = 0;
        }
        break;
    case 0x28:
        marioChgMot(0);
        *(u8*)((u8*)party + 0x17A) = 0x29;
        *(s16*)((u8*)party + 0x178) = 0x28;
        break;
    case 0x29: {
        s16 t = --*(s16*)((u8*)party + 0x178);
        if (t <= 0) {
            marioChgMot(0);
            *(u8*)((u8*)party + 0x17A) = 0x64;
        }
        break;
    }
    case 0x1E:
        marioChgMot(0);
        *(u8*)((u8*)party + 0x17A) = 0x1F;
        break;
    case 0x1F:
        marioChgPose(str_8041B210);
        mario->flags &= ~0x2000;
        *(s16*)((u8*)party + 0x178) = 0x32;
        *(u8*)((u8*)party + 0x17A) = 0x20;
        break;
    case 0x20: {
        s16 t = --*(s16*)((u8*)party + 0x178);
        if (t <= 0) {
            *(u8*)((u8*)party + 0x17A) = 0x24;
            *(s16*)((u8*)party + 0x178) = 0xA;
        }
        break;
    }
    case 0x24: {
        s16 t;
        animPoseSetLocalTime(mario->paperAnimGroupId[marioAnimeId()], (f32)*(s16*)((u8*)party + 0x178));
        t = --*(s16*)((u8*)party + 0x178);
        if (t < 0) {
            marioChgMot(0);
            *(u8*)((u8*)party + 0x17A) = 0x64;
        }
        break;
    }
    default:
        break;
    }
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
    if (*(u8*)((u8*)party + 0x17A) == 0xA || *(u8*)((u8*)party + 0x17A) == 0x65 ||
        (*(u8*)((u8*)party + 0x17A) >= 0x1E && *(u8*)((u8*)party + 0x17A) <= 0x24)) {
        return FALSE;
    }
    return TRUE;
}

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
    motion = *(u8*)((u8*)party + 0x17A);
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

// .text:0xE20 | 0x800BDE58 | size: 0x1318
// bomhei_use_main (TODO)

// .text:0x2138 | 0x800BF170 | size: 0x604
// NONMATCHING (99.98%): the six jump-stand string references use base
// `...rodata.0` (+0x34) where the original pools them off vec3_802ccf48 (+0x68);
// the object puts the string/const-pool data in a different .rodata region. Logic matches.
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
                *(f32*)((u8*)party + 0x114) = 0.0f;
            }
            if (strcmp(gp->currentMapName, str_802CCFB0) == 0) {
                if (party->field_138 != NULL && strcmp(hitGetName(party->field_138), str_802CCFB8) == 0) {
                    party->dir = angleABf(party->position.x, party->position.z, 638.0f, 12.5f);
                    party->field_FC = party->dir;
                }
            }
        }
        if (*(u8*)((u8*)party + 0x17A) != 0x66) {
            if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
                *(u8*)((u8*)party + 0x17A) = 0x66;
                return;
            }
            mario_controll(party);
        }
        if (party->useMotionId <= 0xA && mario->hitobjStandOn != NULL &&
            (hitGetAttr(mario->hitobjStandOn) & 0x80000000)) {
            MapObjectEntry* obj = mobjHitObjPtrToPtr(mario->hitobjStandOn);
            if (strcmp(obj->animName, str_802CCFC8) == 0 || strcmp(obj->animName, str_802CCFDC) == 0 ||
                strcmp(obj->animName, str_802CCFF0) == 0 || strcmp(obj->animName, str_802CD004) == 0) {
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
                p2->flags &= ~0x02000000;
                p2->flags &= ~0x80000000;
                p2->flags &= ~0x100;
                if (*(u32*)((u8*)p2 + 0x180) != 0xFFFFFFFF) {
                    psndSFXOff(*(s32*)((u8*)p2 + 0x180));
                    *(s32*)((u8*)p2 + 0x180) = -1;
                }
                if (*(u32*)((u8*)p2 + 0x184) != 0xFFFFFFFF) {
                    psndSFXOff(*(s32*)((u8*)p2 + 0x184));
                    *(s32*)((u8*)p2 + 0x184) = -1;
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
            p2->flags &= ~0x02000000;
            p2->flags &= ~0x80000000;
            p2->flags &= ~0x100;
            if (*(u32*)((u8*)p2 + 0x180) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x180));
                *(s32*)((u8*)p2 + 0x180) = -1;
            }
            if (*(u32*)((u8*)p2 + 0x184) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x184));
                *(s32*)((u8*)p2 + 0x184) = -1;
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
            p2->flags &= ~0x02000000;
            p2->flags &= ~0x80000000;
            p2->flags &= ~0x100;
            if (*(u32*)((u8*)p2 + 0x180) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x180));
                *(s32*)((u8*)p2 + 0x180) = -1;
            }
            if (*(u32*)((u8*)p2 + 0x184) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x184));
                *(s32*)((u8*)p2 + 0x184) = -1;
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
            p2->flags &= ~0x02000000;
            p2->flags &= ~0x80000000;
            p2->flags &= ~0x100;
            if (*(u32*)((u8*)p2 + 0x180) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x180));
                *(s32*)((u8*)p2 + 0x180) = -1;
            }
            if (*(u32*)((u8*)p2 + 0x184) != 0xFFFFFFFF) {
                psndSFXOff(*(s32*)((u8*)p2 + 0x184));
                *(s32*)((u8*)p2 + 0x184) = -1;
            }
            if (p2->useStruct != NULL) {
                __memFree(0, p2->useStruct);
                p2->useStruct = NULL;
            }
        }
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
            pose = str_8041B2A8;
        } else if (mario->currMotionId == 2) {
            pose = str_8041B2B0;
        } else if (mario->flags & 0x10000) {
            pose = str_802CD018;
        } else {
            pose = str_8041B2B8;
        }
        marioChgPose(pose);
    }
    party->flags2 &= ~0x10;
    party->flags &= ~0x02000000;
    party->flags &= ~0x80000000;
    party->flags &= ~0x100;
    if (party->field_34 != 9) {
        party->field_3B = 0;
        party->field_8 |= 8;
    }
    party->playerPtr->flags &= ~0x2000;
    if (*(u32*)((u8*)party + 0x180) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x180));
        *(s32*)((u8*)party + 0x180) = -1;
    }
    if (*(u32*)((u8*)party + 0x184) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x184));
        *(s32*)((u8*)party + 0x184) = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
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
    marioChgPose(str_8041B2B8);
    if (party->field_34 != 9) {
        partyChgRunMode(party, 2);
        party->field_3B = 0;
    }
    party->flags2 &= ~0x10;
    party->flags &= ~0x02000000;
    party->flags &= ~0x80000000;
    party->flags &= ~0x100;
    if (*(u32*)((u8*)party + 0x180) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x180));
        *(s32*)((u8*)party + 0x180) = -1;
    }
    if (*(u32*)((u8*)party + 0x184) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x184));
        *(s32*)((u8*)party + 0x184) = -1;
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
    party->flags &= ~0x02000000;
    party->flags &= ~0x80000000;
    party->flags &= ~0x100;
    if (*(u32*)((u8*)party + 0x180) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x180));
        *(s32*)((u8*)party + 0x180) = -1;
    }
    if (*(u32*)((u8*)party + 0x184) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x184));
        *(s32*)((u8*)party + 0x184) = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
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
    party->flags &= ~0x02000000;
    party->flags &= ~0x80000000;
    party->flags &= ~0x100;
    if (*(u32*)((u8*)party + 0x180) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x180));
        *(s32*)((u8*)party + 0x180) = -1;
    }
    if (*(u32*)((u8*)party + 0x184) != 0xFFFFFFFF) {
        psndSFXOff(*(s32*)((u8*)party + 0x184));
        *(s32*)((u8*)party + 0x184) = -1;
    }
    if (party->useStruct != NULL) {
        __memFree(0, party->useStruct);
        party->useStruct = NULL;
    }
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

// .text:0x2BAC | 0x800BFBB0 | size: 0x38
void bomhei_init_800BFBB0(PartyEntry* party) {
    party->field_33 = 3;
    party->field_32 = 0;
    *(f32*)((u8*)party + 0xF0) = 28.0f;
    *(f32*)((u8*)party + 0xF4) = 28.0f;
    *(u8*)((u8*)party + 0x17A) = 100;
    *(s32*)((u8*)party + 0x17C) = -1;
    *(s32*)((u8*)party + 0x180) = -1;
    *(s32*)((u8*)party + 0x184) = -1;
}

// .text:0x2BE4 | 0x800BFBE8 | size: 0xA4
void bomhei_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
        party->playerPtr->flags &= ~0x2000;
        if (*(u32*)((u8*)party + 0x180) != 0xFFFFFFFF) {
            psndSFXOff(*(s32*)((u8*)party + 0x180));
            *(s32*)((u8*)party + 0x180) = -1;
        }
        if (*(u32*)((u8*)party + 0x184) != 0xFFFFFFFF) {
            psndSFXOff(*(s32*)((u8*)party + 0x184));
            *(s32*)((u8*)party + 0x184) = -1;
        }
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 0xE);
    }
}
