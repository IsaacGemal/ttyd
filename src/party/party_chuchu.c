#include "party/party_chuchu.h"
#include "party.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "mario/mario_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "driver/itemdrv.h"
#include "driver/mobjdrv.h"
#include "driver/effdrv.h"
#include <string.h>
#include <dolphin/mtx.h>
#include "party/party_move.h"
#include "evt/evt_msg.h"
#include "party/party_hit.h"
#include "eff/eff_fukidashi.h"

// Local declarations: not yet in a header.

extern char* mobj_list[];

typedef struct ChuchuSearchEntry {
    const char* name; // 0x0
    s32 evtId;        // 0x4
    s32 x;            // 0x8
    s32 y;            // 0xC
    s32 z;            // 0x10
} ChuchuSearchEntry;

extern ChuchuSearchEntry unk_8038bb34[];
extern u32 msg_evt[];

USER_FUNC(search_result) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (isFirstCall) {
        if ((u32)event->lwData[0] == 0 || event->lwData[1] == 0) {
            return EVT_RETURN_DONE;
        }
        if (event->lwData[1] == 1) {
            if (party != NULL) {
                effFukidashiEntry(6, 0, -2, sysMsec2Frame(0x5DC), 0.0f, 18.0f, 0.0f, 0.0f, 0.0f);
            }
            return EVT_RETURN_DONE;
        }
        psndSFXOn_3D((const char*)0x945, &party->position);
        event->field_84 = 0x2A;
        partyChgPose(party, "PCH_Y_1");
    }
    partyChkGnd(party);
    if (--*(s32*)&event->field_84 > 0) {
        return 0;
    }
    if (party != NULL) {
        effFukidashiEntry(6, 0, -2, sysMsec2Frame(0x5DC), 0.0f, 18.0f, 0.0f, 0.0f, 0.0f);
    }
    return EVT_RETURN_DONE;
}

// Code and instruction schedule match the original except: (a) the loop setup emits `li r23, 0` for
// `i = 0` where the original reuses found's zero (`mr r23, r24`), which also moves the `li r24, 0`/
// `mr r30, r3` pair by one slot, and (b) the item/tmp zero vectors are compound literals
// (`(Vec){0,0,0}`) emitted as local .rodata at `...rodata.0+0` while the original uses the named
// globals `vec3_802f8288` (0x130) and `vec3_802f839c` (0x244); the latter is shared with
// chuchu_searchObject, so the pool offsets only settle when the unit's .rodata matches.
// NONMATCHING (98.64%): see the two differences above.
USER_FUNC(set_msg) {
    MapObjectEntry* mobj;
    ItemEntry* item;
    ChuchuSearchEntry* entry;
    MarioWork* loopPlayer;
    BOOL found;
    u32 i;
    MarioWork* player;
    s32 pass;
    s32 result;
    PartyEntry* party;
    Vec closestPos;
    Vec itemPos;
    Vec itemTmp;
    Vec tmp;
    Vec pos;
    f32 threshold;
    f32 minDist;
    f32 direction;

    party = partyGetPtr(marioGetPartyId());
    player = party->playerPtr;
    event->lwData[0] = 0;
    event->lwData[1] = 0;
    if (party == NULL) {
        return EVT_RETURN_DONE;
    }

    pass = 0;
    do {
        if (pass == 0) {
            threshold = 150.0f;
        } else {
            threshold = 10000.0f;
        }
        result = -1;
        item = itemNearDistCheck(player->position.x, player->position.y, player->position.z, threshold);
        mobj = mobjNearDistCheck2(player->position.x, player->position.y, player->position.z, threshold,
                                  mobj_list);
        found = FALSE;
        loopPlayer = party->playerPtr;
        minDist = 10000.0f;
        entry = unk_8038bb34;
        for (i = 0; i < 0x28; i++, entry++) {
            f32 dist;
            if (strcmp(gp->currentMapName, entry->name) != 0) {
                continue;
            }
            if (evtGetValue(NULL, entry->evtId) != 0) {
                continue;
            }
            tmp = (Vec){ 0.0f, 0.0f, 0.0f };
            tmp.x = (f32)entry->x;
            tmp.y = (f32)entry->y;
            tmp.z = (f32)entry->z;
            pos = tmp;
            dist = PSVECDistance(&loopPlayer->position, &pos);
            if (threshold < dist) {
                continue;
            }
            if (minDist > dist) {
                minDist = dist;
                found = TRUE;
                closestPos.x = pos.x;
                closestPos.y = pos.y;
                closestPos.z = pos.z;
            }
        }

        minDist = 10000.0f;
        if (item != NULL) {
            itemTmp = (Vec){ 0.0f, 0.0f, 0.0f };
            itemTmp.x = item->position.x;
            itemTmp.y = item->position.y;
            itemTmp.z = item->position.z;
            itemPos = itemTmp;
            if (10000.0f > PSVECDistance(&itemPos, &player->position)) {
                f32 d = PSVECDistance(&itemPos, &player->position);
                minDist = d;
                direction = revise360(angleABf(party->position.x, party->position.z, item->position.x, item->position.z)
                                      - *(f32*)((u8*)party + 0xF8));
                result = 0;
            }
        }
        if (mobj != NULL) {
            if (minDist > PSVECDistance(&mobj->position, &player->position)) {
                f32 d = PSVECDistance(&mobj->position, &player->position);
                minDist = d;
                direction = revise360(angleABf(party->position.x, party->position.z, mobj->position.x,
                                               mobj->position.z)
                                      - *(f32*)((u8*)party + 0xF8));
                result = 1;
            }
        }
        if (found) {
            if (minDist > PSVECDistance(&closestPos, &player->position)) {
                minDist = PSVECDistance(&closestPos, &player->position);
                direction = revise360(angleABf(party->position.x, party->position.z, closestPos.x,
                                               closestPos.z)
                                      - *(f32*)((u8*)party + 0xF8));
                result = 2;
            }
        }
        if (result >= 0) {
            break;
        }
        pass++;
    } while (pass < 2);

    if (result >= 0) {
        if (threshold == 150.0f) {
            event->lwData[0] = (s32)"map_exist_near";
            event->lwData[1] = 2;
        } else {
            event->lwData[0] = (s32)"map_exist_far";
            event->lwData[1] = 1;
        }
        party->field_110 = (direction <= 180.0f) ? 180.0f : 0.0f;
        party->field_FC = partyToMovedir(party, party->field_110);
        party->dir = party->field_FC;
    } else {
        event->lwData[0] = (s32)"map_no_exist";
        event->lwData[1] = 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(post_msg) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    event->lwData[0] = 0;
    if (party != NULL) {
        party->useMotionId++;
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (97.26%): stack frame and instruction sequence match; the loop prologue only differs in
// scheduling (`addi r0;mr r31` vs `addi r31`) and `li r29,0;li r28,0` vs `mr r28,r29`.
// `Vec pos = tmp;` (a separate zero-init temp plus a whole-struct copy) is what reproduces the
// original's 0x14 scratch and the `lwz/stw` (not `lfs/stfs`) copy of pos to 0x8.
BOOL chuchu_searchObject(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* player = party->playerPtr;
    ChuchuSearchEntry* entry;
    MarioWork* loopPlayer;
    BOOL found;
    u32 i;
    f32 minDist;

    if (itemNearDistCheck(player->position.x, player->position.y, player->position.z, 150.0f) != NULL) {
        return TRUE;
    }
    if (mobjNearDistCheck2(player->position.x, player->position.y, player->position.z, 150.0f, mobj_list) != NULL) {
        return TRUE;
    }

    found = FALSE;
    loopPlayer = party->playerPtr;
    entry = unk_8038bb34;
    minDist = 10000.0f;
    for (i = 0; i < 0x28; i++, entry++) {
        if (strcmp(gp->currentMapName, entry->name) != 0) {
            continue;
        }
        if (evtGetValue(NULL, entry->evtId) != 0) {
            continue;
        }
        {
            Vec tmp = { 0.0f, 0.0f, 0.0f };
            Vec pos;
            f32 dist;
            tmp.x = (f32)entry->x;
            tmp.y = (f32)entry->y;
            tmp.z = (f32)entry->z;
            pos = tmp;
            dist = PSVECDistance(&loopPlayer->position, &pos);
            if (150.0f < dist) {
                continue;
            }
            if (minDist > dist) {
                minDist = dist;
                found = TRUE;
            }
        }
    }
    return found != 0;
}

void chuchu_init(PartyEntry* party) {
    party->field_33 = 7;
    party->field_32 = 0;
    *(f32*)((u8*)party + 0xF0) = 20.0f;
    *(f32*)((u8*)party + 0xF4) = 24.0f;
}

void chuchu_reinit(PartyEntry* party) {
}

void chuchu_exit(PartyEntry* party) {
}

void chuchu_move(PartyEntry* party) {
    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        partyWalkInit(party, 1);
    }
    unk_800c92c4(party);
    partyWalkMain(party);
}

void chuchu_use(PartyEntry* party) {
    MarioWork* player = party->playerPtr;

    if (party->field_8 & 0x8) {
        party->field_8 &= ~0x8;
        party->useMotionId = 0;
        marioStSystemLevel(1);
    }

    switch (party->useMotionId) {
    case 0: {
        Vec appear;
        partyGetAppearPos3(party, &appear);
        *(Vec*)((u8*)party + 0x94) = appear;
        *(s32*)((u8*)party + 0x24) = 4;
        party->field_FC = angleABf(party->position.x, party->position.z,
                                   *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
        party->dir = party->field_FC;
        party->field_10C = player->unk1A8;
        party->field_110 = player->unk1A8;
        party->speed = distABf(party->position.x, party->position.z,
                               *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C))
                       / (f32)*(s32*)((u8*)party + 0x24);
        party->jumpPara1 = 0.0f;
        party->useMotionId = 1;
        break;
    }
    case 1: {
        Vec appear;
        partyGetAppearPos3(party, &appear);
        *(Vec*)((u8*)party + 0x94) = appear;
        party->position.x += (((Vec*)((u8*)party + 0x94))->x - party->position.x) / (f32)*(s32*)((u8*)party + 0x24);
        party->position.y += (((Vec*)((u8*)party + 0x94))->y - party->position.y) / (f32)*(s32*)((u8*)party + 0x24);
        party->position.z += (((Vec*)((u8*)party + 0x94))->z - party->position.z) / (f32)*(s32*)((u8*)party + 0x24);
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            return;
        }
        partyChgPoseId(party, 1);
        party->speed = 0.0f;
        party->useMotionId = 10;
        break;
    }
    case 10:
        party->field_110 = *(f32*)((u8*)player + 0x1AC);
        party->field_FC = toMovedir(party->field_110);
        party->dir = party->field_FC;
        party->useMotionId = 20;
        *(s32*)((u8*)party + 0x24) = 25;
        break;
    case 20:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        psndSFXOn_3D((const char*)0x944, &party->position);
        partyChgPose(party, "PCH_A2_1");
        party->useMotionId = 21;
        *(s32*)((u8*)party + 0x24) = 30;
        // fallthrough
    case 21:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        partyChgPoseId(party, 1);
        evtEntryType(msg_evt, 0, 0, 0x1A);
        party->useMotionId = 22;
        break;
    case 23:
        partyChgPoseId(party, 1);
        party->useMotionId = 0x50;
        break;
    default:
        break;
    }

    switch ((s8)party->useMotionId) {
    case 0x50:
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 2);
        party->field_3B = 0;
        marioChgMot(0);
        marioStSystemLevel(0);
        break;
    }
    partyChkGnd(party);
}
char* mobj_list[] = {
    "MOBJ_TreasureBox",
    "MOBJ_BigTreasureBox",
    "MOBJ_GrayTreasureBox",
    "MOBJ_BlackTreasureBox",
    "MOBJ_KururinFloor",
    "MOBJ_HatenaBlock",
    "MOBJ_BadgeBlock",
    "MOBJ_10CountBlock",
    "MOBJ_HiddenHatenaBlock",
    "MOBJ_HiddenBadgeBlock",
    "MOBJ_Hidden10CountBlock",
    "MOBJ_BrickHatenaBlock",
    "MOBJ_BrickBadgeBlock",
    "MOBJ_PowerUpBlock",
    NULL,
};

ChuchuSearchEntry unk_8038bb34[0x28] = {
    { "hei_00", (s32)0xF840715E, (s32)0xFFFFFE2C, (s32)0x0000008B, (s32)0xFFFFFF81 },
    { "hei_00", (s32)0xF8406272, (s32)0xFFFFFE68, (s32)0x0000008B, (s32)0xFFFFFF81 },
    { "hei_01", (s32)0xF840628A, (s32)0xFFFFFF38, (s32)0x00000000, (s32)0x00000078 },
    { "hei_01", (s32)0xF8406278, (s32)0x000000BE, (s32)0x00000000, (s32)0xFFFFFFBA },
    { "hei_01", (s32)0xF8406288, (s32)0xFFFFFEE3, (s32)0x00000000, (s32)0x00000055 },
    { "hei_02", (s32)0xF8407160, (s32)0xFFFFFD7B, (s32)0x00000032, (s32)0xFFFFFFBA },
    { "hei_02", (s32)0xF840627A, (s32)0x000000C0, (s32)0x0000005C, (s32)0xFFFFFF88 },
    { "hei_04", (s32)0xF840627E, (s32)0x0000016C, (s32)0x0000004B, (s32)0xFFFFFFA1 },
    { "hei_04", (s32)0xF8406289, (s32)0x00000140, (s32)0x0000004B, (s32)0xFFFFFFC9 },
    { "hei_04", (s32)0xF840627F, (s32)0x00000011, (s32)0x0000005A, (s32)0xFFFFFF86 },
    { "hei_06", (s32)0xF8406282, (s32)0x000000C8, (s32)0x0000004B, (s32)0x0000003C },
    { "nok_01", (s32)0xF9718884, (s32)0xFFFFFE69, (s32)0x00000046, (s32)0xFFFFFF6C },
    { "win_00", (s32)0xF84065FD, (s32)0xFFFFFD4A, (s32)0x0000006F, (s32)0xFFFFFF7E },
    { "win_01", (s32)0xF84065FE, (s32)0x00000138, (s32)0x0000006A, (s32)0xFFFFFF86 },
    { "win_03", (s32)0xF8407169, (s32)0xFFFFFEB3, (s32)0x00000048, (s32)0xFFFFFFE8 },
    { "mri_01", (s32)0xF840668C, (s32)0xFFFFFF10, (s32)0x00000000, (s32)0xFFFFFFEC },
    { "mri_04", (s32)0xF8406696, (s32)0x00000055, (s32)0x00000000, (s32)0x00000055 },
    { "mri_05", (s32)0xF840669C, (s32)0x000000E1, (s32)0xFFFFFECB, (s32)0xFFFFFFD8 },
    { "mri_06", (s32)0xF840716E, (s32)0xFFFFFFA1, (s32)0xFFFFFECA, (s32)0xFFFFFFCC },
    { "mri_12", (s32)0xF84066AC, (s32)0xFFFFFF42, (s32)0xFFFFFECB, (s32)0x00000041 },
    { "mri_13", (s32)0xF84066AE, (s32)0xFFFFFF42, (s32)0xFFFFFECB, (s32)0x00000073 },
    { "mri_15", (s32)0xF8407170, (s32)0xFFFFFFDB, (s32)0x00000000, (s32)0x00000060 },
    { "mri_18", (s32)0xF84066B9, (s32)0x00000032, (s32)0xFFFFFEC0, (s32)0xFFFFFFDD },
    { "mri_20", (s32)0xF84066BB, (s32)0x000000FA, (s32)0xFFFFFEC0, (s32)0x00000082 },
    { "usu_00", (s32)0xF840717C, (s32)0x00000082, (s32)0x00000000, (s32)0x000001C7 },
    { "usu_00", (s32)0xF840630D, (s32)0x0000014A, (s32)0x00000000, (s32)0x00000113 },
    { "jin_01", (s32)0xF9718882, (s32)0x0000031E, (s32)0x00000032, (s32)0xFFFFFFD8 },
    { "muj_02", (s32)0xF84067D2, (s32)0x00000143, (s32)0x00000000, (s32)0x00000032 },
    { "muj_02", (s32)0xF8407189, (s32)0xFFFFFE0C, (s32)0x00000000, (s32)0x0000006E },
    { "muj_02", (s32)0xF84067D0, (s32)0xFFFFFEA2, (s32)0x0000005A, (s32)0xFFFFFFBA },
    { "muj_03", (s32)0xF84067D3, (s32)0xFFFFFF06, (s32)0x0000000A, (s32)0x0000005F },
    { "muj_03", (s32)0xF84067D4, (s32)0x000000AF, (s32)0x000000D2, (s32)0xFFFFFFCE },
    { "muj_05", (s32)0xF84067D5, (s32)0x000000EB, (s32)0x00000000, (s32)0x0000005A },
    { "rsh_02_a", (s32)0xF8406906, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "rsh_02_b", (s32)0xF8406906, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "rsh_02_c", (s32)0xF8406906, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "rsh_04_a", (s32)0xF8407194, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "rsh_04_b", (s32)0xF8407194, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "rsh_04_c", (s32)0xF8407194, (s32)0xFFFFFEF7, (s32)0x00000000, (s32)0xFFFFFF06 },
    { "tou_05", (s32)0xF8407179, (s32)0x00000064, (s32)0x00000000, (s32)0xFFFFFFE4 },
};

u32 msg_evt[] = {
    0x0001005B,
    (u32)set_msg,
    0x0001005B,
    (u32)search_result,
    0x00020019,
    0xFE363C80,
    0x00000000,
    0x0005005B,
    (u32)evt_msg_print,
    0x00000000,
    0xFE363C80,
    0x00000000,
    (u32)"party",
    0x00000021,
    0x0001005B,
    (u32)post_msg,
    0x00000002,
    0x00000001,
};
