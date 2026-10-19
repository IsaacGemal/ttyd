#include "party/party_kuribo.h"
#include "evt/evt_cmd.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "party.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "mario/mario_pouch.h"
#include "party/party_hit.h"
#include "party/party_move.h"
#include "pmario_sound.h"
#include "system.h"
#include "evt/evt_msg.h"

// data external to this unit
extern s32 kurio_msg_evt[];

// The original .rodata pads this (the unit's last string) out with two extra NULs; the natural
// string works identically for strcmp, so only the matching build carries the padding.
#ifdef MUST_MATCH
#define KURURIN_FLOOR_NAME "MOBJ_KururinFloor\0\0"
#else
#define KURURIN_FLOOR_NAME "MOBJ_KururinFloor"
#endif

// .sbss
u32 msg_ep; //unknown since it only gets written to once and never used

s32 kurio_msg_evt[19];

// clang-format off

// partial view of PartyEntry from 0x94: the appear position written by
// partyGetAppearPos3 (no named field in include/party.h).
typedef struct PartyAppearPos {
    u8 pad[0x94];
    Vec position;
} PartyAppearPos;

BOOL christineGetStatus(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (!party || party->currentMemberId != PARTY_MEMBER_GOOMBELLA || !(party->flags & 0x100)) {
        return FALSE;
    }
    return TRUE;
}


// Matching. The register-allocation miss was fixed by declaring `party` after `hit`/`msg` (assignment
// split from the declaration) and by loading `mario = party->playerPtr` before the `event->lwData[0]`
// check, which reproduces the original's `lwz r27, 0x160(r31)` scheduling. Hits use the named
// `hitobjPush` (0x1EC) / `hitobjStandOn` (0x1E4); the middle one is the unnamed `MarioWork+0x1E0`.
USER_FUNC(pre_kurio_use) {
    HitEntry* hit;
    const char* msg;
    PartyEntry* party;
    MarioWork* mario;
    MapObjectEntry* obj;

    party = partyGetPtr(marioGetPartyId());

    if (!party) {
        return EVT_RETURN_DONE;
    }

    if (party->flags2 & 0x4000000) {
        return EVT_RETURN_DONE;
    }

    psndSetFlag(0x80);
    party->flags |= 0x04000000;
    party->flags |= 0x02000000;

    mario = party->playerPtr;

    if (event->lwData[0]) {
        event->lwData[0] += 8;
        return EVT_RETURN_DONE;
    }

    hit = (HitEntry*)mario->hitobjPush;
    if (hit != NULL && (hitGetAttr(hit) & 0x80000000)) {
        goto found;
    }
    hit = *(HitEntry**)((u8*)mario + 0x1E0);
    if (hit != NULL && (hitGetAttr(hit) & 0x80000000)) {
        goto found;
    }
    hit = (HitEntry*)mario->hitobjStandOn;
    if (hit != NULL && (hitGetAttr(hit) & 0x80000000)) {
        goto found;
    }
    event->lwData[0] = (s32) "msg_kuri_map";
    return EVT_RETURN_DONE;

found:
    obj = mobjHitObjPtrToPtr(hit);
    if (strcmp(obj->animName, "MOBJ_Lv1Block") == 0) {
        if (pouchGetHammerLv() < 1) {
            msg = "obj_hlp_block_y_0";
        } else {
            msg = "obj_hlp_block_y_1";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lv1BigBlock") == 0) {
        if (pouchGetHammerLv() < 1) {
            msg = "obj_hlp_block_y_0";
        } else {
            msg = "obj_hlp_block_y_1";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lv1BigBigBlock") == 0) {
        if (pouchGetHammerLv() < 1) {
            msg = "obj_hlp_block_y_0";
        } else if (pouchGetHammerLv() < 2) {
            msg = "obj_hlp_block_y_2";
        } else {
            msg = "obj_hlp_block_y_3";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lv2Block") == 0 || strcmp(obj->animName, "MOBJ_Lv2BigBlock") == 0) {
        if (pouchGetHammerLv() < 1) {
            msg = "obj_hlp_block_s_0";
        } else if (pouchGetHammerLv() < 3) {
            msg = "obj_hlp_block_s_1";
        } else {
            msg = "obj_hlp_block_s_2";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lv2BigBigBlock") == 0) {
        if (pouchGetHammerLv() < 1) {
            msg = "obj_hlp_block_s_0";
        } else if (pouchGetHammerLv() < 3) {
            msg = "obj_hlp_block_s_1";
        } else {
            msg = "obj_hlp_block_s_3";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lv3BigBigBlock") == 0 || strcmp(obj->animName, "MOBJ_Lv3BigBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_Lv3Block") == 0) {
        msg = "obj_hlp_redyellow_block";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Block") == 0 || strcmp(obj->animName, "MOBJ_PinkBlock") == 0) {
        msg = "obj_hlp_su_block";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_PowerUpBlock") == 0) {
        msg = "obj_hlp_shine";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_HatenaBlock") == 0) {
        msg = "obj_hlp_hatena";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_BadgeBlock") == 0) {
        msg = "obj_hlp_badge_hatena";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_green_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_green_big_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_green_big02_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_purple_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_purple_big_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_purple_big02_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_orange_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_orange_big_FlowBlock") == 0 ||
        strcmp(obj->animName, "MOBJ_orange_big02_FlowBlock") == 0) {
        msg = "obj_hlp_huyuu";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_green_FlowBlockSwitch") == 0 ||
        strcmp(obj->animName, "MOBJ_purple_FlowBlockSwitch") == 0 ||
        strcmp(obj->animName, "MOBJ_orange_FlowBlockSwitch") == 0) {
        msg = "obj_hlp_huyuu_switch";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_BlueTornadoSwitch") == 0 ||
        strcmp(obj->animName, "MOBJ_RedTornadoSwitch") == 0) {
        msg = (pouchGetJumpLv() < 2) ? "obj_hlp_big_switch_1" : "obj_hlp_big_switch_2";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_TreasureBox") == 0 || strcmp(obj->animName, "MOBJ_GrayTreasureBox") == 0 ||
        strcmp(obj->animName, "MOBJ_BigTreasureBox") == 0) {
        msg = (mobjCheckItemboxOpen(obj) == 2) ? "obj_hlp_takara_1" : "obj_hlp_takara_0";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_BlackTreasureBox") == 0) {
        msg = (mobjCheckItemboxOpen(obj) == 2) ? "obj_hlp_blackbox_1" : "obj_hlp_blackbox_0";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_RedJumpStand") == 0 || strcmp(obj->animName, "MOBJ_RedJumpStand2") == 0 ||
        strcmp(obj->animName, "MOBJ_BlueJumpStand") == 0 || strcmp(obj->animName, "MOBJ_BlueJumpStand2") == 0 ||
        strcmp(obj->animName, "MOBJ_MapJumpStand") == 0) {
        msg = "obj_hlp_jump";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_RedTimerSwitch") == 0 || strcmp(obj->animName, "MOBJ_BlueTimerSwitch") == 0) {
        msg = "obj_hlp_timer_switch";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_RedSwitch") == 0 || strcmp(obj->animName, "MOBJ_BlueSwitch") == 0 ||
        strcmp(obj->animName, "MOBJ_WhiteSwitch") == 0 || strcmp(obj->animName, "MOBJ_BlackSwitch") == 0) {
        msg = "obj_hlp_switch";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_RideSwitchA") == 0 || strcmp(obj->animName, "MOBJ_RideSwitchB") == 0 ||
        strcmp(obj->animName, "MOBJ_RideSwitchC") == 0) {
        msg = "obj_hlp_yuka_switch";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_BreakingFloor") == 0 ||
        strcmp(obj->animName, "MOBJ_BlackBreakingFloor") == 0) {
        msg = "obj_hlp_hip_attack";
        if (pouchGetJumpLv() >= 2) {
            msg = "obj_hlp_hip_attack_1";
        }
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_BombRock") == 0) {
        msg = "obj_hlp_bomb_rock";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Signboard") == 0) {
        msg = "obj_hlp_kanban";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Arrow") == 0) {
        msg = "obj_hlp_yajirushi";
        goto done;
    }
    if (strcmp(obj->animName, "MOBJ_Lock") == 0) {
        msg = "obj_hlp_lock";
        goto done;
    }
    if (strcmp(obj->animName, KURURIN_FLOOR_NAME) == 0) {
        msg = "msg_kuri_map";
        goto done;
    }
    msg = "msg_kuri_map";

done:
    if (msg != NULL) {
        event->lwData[0] = (s32)msg;
        partyChgPoseId(party, 7);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(post_kurio_use) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (!party) {
        return EVT_RETURN_DONE;
    }

    party->useMotionId = 0x14;
    marioStSystemLevel(0);
    psndClearFlag(0x80);
    party->flags &= ~0x04000000;
    party->flags &= ~0x02000000;
    return EVT_RETURN_DONE;
}


// .sdata2 constants

void kuribo_init_800C23B4(PartyEntry* party) {
    party->field_33 = 1;
    party->field_32 = 0;
    party->height = 24.0f;
    party->radius = 26.0f;
    msg_ep = 0;
}


void kuribo_use_post(PartyEntry* party) {
    party->flags &= ~0x04000000;
    party->flags &= ~0x02000000;
}


// Raw party offsets used (no named field): 0x24 frame counter, 0xEC distance threshold.
void kuribo_use(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;

        if (PSVECDistance(&mario->position, &party->position) > 125.0f) {
            party->useMotionId = 0;
        } else {
            Vec pos = party->position;

            hit = partySearchFrontWall(party, &pos, 150.f,
                                       angleABf(party->position.x, party->position.z, mario->position.x,
                                                mario->position.z));
            if (hit != NULL) {
                if (party->field_EC < distABf(party->position.x, party->position.z, mario->position.x,
                                                         mario->position.z)) {
                    party->useMotionId = 0;
                } else {
                    hit = NULL;
                }
            }
            if (hit == NULL) {
                partyChgPoseId(party, 1);
                party->speed = 0.0f;
                party->timer = 100;
                party->useMotionId = 0xA;
            }
        }
    }

    switch (party->useMotionId) {
    case 0:
        if (!partyGetAppearPos3(party, &party->targetPos)) {
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
        }
        party->timer = 4;
        party->field_FC = angleABf(party->position.x, party->position.z, ((PartyAppearPos*)party)->position.x,
                                   ((PartyAppearPos*)party)->position.z);
        party->dir = party->field_FC;
        party->field_10C = mario->unk1A8;
        party->field_110 = mario->unk1A8;
        party->speed = distABf(party->position.x, party->position.z, ((PartyAppearPos*)party)->position.x,
                               ((PartyAppearPos*)party)->position.z) /
                       (f32)party->timer;
        party->jumpPara1 = 0.0f;
        party->useMotionId = 1;
        break;
    case 1: {
        if (!partyGetAppearPos3(party, &party->targetPos)) {
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
        }
        party->position.x += (((PartyAppearPos*)party)->position.x - party->position.x) /
                                 (f32)party->timer;
        party->position.y += (((PartyAppearPos*)party)->position.y - party->position.y) /
                                 (f32)party->timer;
        party->position.z += (((PartyAppearPos*)party)->position.z - party->position.z) /
                                 (f32)party->timer;
        if (--party->timer > 0) {
            return;
        }
        partyChgPoseId(party, 1);
        party->speed = 0.0f;
        party->timer = 100;
        party->useMotionId = 0xA;
        break;
    }
    case 10: {
        EventEntry* evt = evtEntryType(kurio_msg_evt, 0, 0, 0x1A);

        marioStSystemLevel(1);
        evt->lwData[0] = (s32)fbatNpcTalkCheck();
        party->useMotionId = 0xF;
        break;
    }
    case 0x14:
        party->timer = 5;
        party->useMotionId = 0x15;
        // fallthrough
    case 0x15:
        if (--party->timer <= 0) {
            partyChgRunMode(party, 2);
            partyChgMoveMode(party, 0);
            partyChgMot(party, 0);
            partyChgPoseId(party, 1);
            party->flags &= ~0x100;
            party->flags &= ~0x04000000;
            party->flags &= ~0x02000000;
        }
        break;
    }
    partyChkGnd(party);
}


void kuribo_bye(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        partyGoodbyeInit(party);
    }
    if (partyGoodbyeMain(party)) {
        partyChgRunMode(party, 0xE);
    }
}

EVT_BEGIN(kurio_msg_evt)
    CALL(pre_kurio_use)
    IF_INT_EQUAL(LW(0), 0)
        CALL(post_kurio_use)
        RETURN
    END_IF
    CALL(evt_msg_print, 0, LW(0), 0, STRING("party"))
    CALL(post_kurio_use)
    RETURN
EVT_END()
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
