#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_audience.h"
#include "battle/battle_ac.h"
#include "battle/battle_damage.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_attack_audience.h"
#include "driver/itemdrv.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_hit.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mario_pouch.h"
#include "pmario_sound.h"
#include "data/item_data.h"
#include <stdio.h>


extern ItemData itemDataTable[];

// .data event tables are defined below the functions that use them.
extern s32 btlataudevtDummy[16];
extern s32 btlataudevtPresentItem_Damage[];
extern s32 btlataudevtPresentItem_Get[];

// psndSFX_pit has no shared prototype; the original's callers disagree on the pitch width.
void psndSFX_pit(s32 id, s16 pitch);

// .rodata
const char str_802F3A98[] = "btl_msg_get_item";
const char str_802F3AAC[] = "SFX_BTL_DAMAGED_PLIABLE1";

// .sdata2
s32 dat_8041ccc0 = -1;
f32 float_8_8041ccc4 = 8.0f;
f32 float_900_8041ccc8 = 900.0f;
f32 float_0_8041cccc = 0.0f;
f32 float_10_8041ccd0 = 10.0f;
f32 float_34_8041ccd4 = 34.0f;
f32 float_60_8041ccd8 = 60.0f;
f32 float_32_8041ccdc = 32.0f;
f32 float_1_8041cce0 = 1.0f;

// NONMATCHING (93.75%): MWCC emits the btlataudevtDummy base plus a separate `addi 0x4`; the original
// folds the +4 into the @ha/@l reloc addend (btlataudevtDummy+0x4). Same instruction sequence otherwise.
s32* btlataudGetAttackEventPtr(s32 index) {
    s32* data = BtlUnit_GetData(BattleGetUnitPtr(_battleWorkPointer, index), 6);
    if (data == NULL) {
        return btlataudevtDummy + 1;
    }
    return data;
}

void* btlataudGetMoveEventPtr(void) {
    s32 type = BattleAudience_GetPresentItemType();

    switch (type) {
    case 0:
        break;
    case 1:
        return btlataudevtPresentItem_Damage;
    }
    return btlataudevtPresentItem_Get;
}

s32 _damage(EventEntry* event) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 targetId = BattleAudience_GetPresentTargetUnitId();
    BattleUnit* unit = BattleGetUnitPtr(work, targetId);
    s32 partId = BtlUnit_GetBodyPartsId(unit);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, partId);
    BattleAudience* audience;

    evtGetValue(event, args[0]);
    audience = BattleAudienceBaseGetPtr();
    BattleActionCommandResetDefenceResult();
    BattleCheckDamage(BattleGetSystemPtr(work), unit, part, (BattleWeapon*)((u8*)audience + 0x137FC), 0x100);
    return 2;
}

s32 _get_item_announce(EventEntry* event, s32 isFirstCall) {
    BattleWork* work = _battleWorkPointer;
    s32 itemNo = BattleAudience_GetPresentItemNo();

    if (isFirstCall) {
        s32 targetId = BattleAudience_GetPresentTargetUnitId();
        BattleUnit* unit = BattleGetUnitPtr(work, targetId);
        f32 x;
        f32 y;
        f32 z;

        BtlUnit_GetPos(unit, &x, &y, &z);
        y = y + (unit->mSizeMultiplier * BtlUnit_GetHeight(unit) + float_8_8041ccc4);
        event->userdata[1] = (s32)itemEntry(NULL, itemNo, 0x11, -1, 0, x, y, z);
        event->userdata[2] = 0x5A;
    } else {
        if (event->userdata[2] > 0) {
            event->userdata[2]--;
        }
        if (event->userdata[2] <= 0) {
            itemDelete(((ItemEntry*)event->userdata[1])->name);
            return 2;
        }
    }
    dispEntry(CAMERA_2D, float_900_8041ccc8, 1, _get_item_announce_disp, NULL);
    return 0;
}

// NONMATCHING (99.45%): the 2^52 double is an anonymous @246 pool entry instead of the named
// double_to_int_802f3a90 (known MWCC quirk, cf. mot_damage.c), and r6/r7/r8 in the conversion are
// permuted. Same instruction sequence otherwise.
void _get_item_announce_disp(CameraId camera, void* param) {
    s32 itemNo = BattleAudience_GetPresentItemNo();
    const char* itemName = msgSearch(itemDataTable[itemNo].name);
    const char* fmt = msgSearch(str_802F3A98);
    char buf[128];
    u16 lines;
    s32 width;
    f32 x;
    GXColor color;

    sprintf(buf, fmt, itemName);
    width = FontGetMessageWidthLine(buf, &lines);
    x = float_0_8041cccc - (f32)(width / 2);
    lines++;
#ifdef MUST_MATCH
    color = *(GXColor*)&dat_8041ccc0; // a GXColor object here changes the load (99% -> 80%)
#else
    color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
#endif
    windowDispGX_Waku_col(0, color, x - float_10_8041ccd0, float_34_8041ccd4,
                          (f32)(width + 20), (f32)(lines * 0x1D + 3), float_10_8041ccd0);
    FontDrawStart();
    FontDrawMessage((s32)x, (s32)float_60_8041ccd8 - 0x1D, buf);
}

s32 _get_present_target_id(EventEntry* event) {
    s32* args = event->args;
    s32 target = args[0];

    evtSetValue(event, target, BattleAudience_GetPresentTargetUnitId());
    return 2;
}

s32 _get_present_item(EventEntry* event) {
    BattleWork* work = _battleWorkPointer;
    s32 itemNo = BattleAudience_GetPresentItemNo();
    s32 targetId = BattleAudience_GetPresentTargetUnitId();
    BattleUnit* unit = BattleGetUnitPtr(work, targetId);

    if (unit->mAlliance == 0) {
        pouchGetItem(itemNo);
    } else if (unit->heldItem == 0) {
        unit->heldItem = itemNo;
    }
    return 2;
}

s32 _delete_present_item(EventEntry* event) {
    s32* args = event->args;
    s32 target = args[0];
    s32 itemNo = BattleAudience_GetPresentItemNo();

    if (target != 0xF1194D80) {
        evtSetValue(event, target, itemNo);
    }
    BattleAudience_SetPresentItemNo(0);
    return 2;
}

s32 _check_aud_item_type(EventEntry* event) {
    s32* args = event->args;
    s32 target = args[0];
    ItemType itemType;
    s32 flag;

    evtSetValue(event, target, 1);
    BattleAudience_GetItemOn2(NULL, NULL, NULL, NULL, &itemType);
    if (itemType == 0) {
        return 2;
    }
    if (itemType >= 0xEC && itemType < 0xF0) {
        flag = 1;
    } else {
        flag = 0;
    }
    evtSetValue(event, target, flag);
    return 2;
}

// NONMATCHING (99.51%): identical except the address-taken locals are allocated in reverse stack order
// (ours memberId@0x14..z@0x8, original memberId@0x8..z@0x14).
// NONMATCHING (99.74%): `args` is r31 here (r29 in the original, which reuses r29 for the &z pointer afterwards).
s32 _attack_aud(EventEntry* event) {
    f32 z;
    f32 y;
    f32 x;
    s32* args = event->args;
    s32 memberId = evtGetValue(event, args[0]);
    s32 type = evtGetValue(event, args[1]);
#ifdef MUST_MATCH
    // these pointers give the original's frame (0x30) and r28-r31 save set
    f32* py = &y;
    f32* pz = &z;
#else
#define py (&y)
#define pz (&z)
#endif
    char* sound = NULL;

    BattleAudience_GetItemOn2(&memberId, &x, py, pz, NULL);
    y += float_32_8041ccdc;
    if (type != 1) {
        effHitEntry(0, 3, x, y, z, float_1_8041cce0);
        sound = (char*)str_802F3AAC;
    }
    BattleAudience_GetPosition(memberId, &x, py, pz);
    if (sound != NULL) {
        psndSFX_pit(psndSFXOn_3D(sound, (Vec*)&x), 0x3FF);
    }
    BattleAudience_Attack(memberId);
    return 2;
}

#ifndef MUST_MATCH
#undef py
#undef pz
#endif

s32 _get_attack_aud_no(EventEntry* event) {
    s32* args = event->args;
    s32 arg0 = args[0];
    s32 arg1 = args[1];
    s32 arg2 = args[2];
    s32 arg3 = args[3];
    s32 memberId;
    f32 x;
    f32 y;
    f32 z;

    BattleAudience_GetItemOn2(&memberId, &x, &y, &z, NULL);
    if (arg0 != 0xF1194D80) {
        evtSetValue(event, arg0, memberId);
    }
    if (arg1 != 0xF1194D80) {
        evtSetValue(event, arg1, (s32)x);
    }
    if (arg2 != 0xF1194D80) {
        evtSetValue(event, arg2, (s32)y);
    }
    if (arg3 != 0xF1194D80) {
        evtSetValue(event, arg3, (s32)z);
    }
    return 2;
}

// .data
s32 btlataudevtDummy[] = {
    0x00000000,
    0x0005005B, (s32)_get_attack_aud_no, 0xFE363C83, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0003005B, (s32)_attack_aud, 0xFE363C83,
    0x00000000,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE,
    0x00000002, 0x00000001,
};

s32 btlataudevtPresentItem_Damage[] = {
    0x0002005B, (s32)_delete_present_item, 0xFE363C80,
    0x0001006C, 0xFE363C8F,
    0x0002005B, (s32)_damage, 0xFE363C80,
    0x0000006D,
    0x0002005B, (s32)btlevtcmd_WaitEventEnd, 0xFE363C8F,
    0x00000002, 0x00000001,
};

s32 btlataudevtPresentItem_Get[] = {
    0x0002005B, (s32)_get_present_target_id, 0xFE363C83,
    0x0002005B, (s32)btlevtcmd_StopWaitEvent, 0xFFFFFFFE,
    0x0003005B, (s32)btlevtcmd_GetUnitId, 0xFFFFFFFE, 0xFE363C80,
    0x00020019, 0xFE363C80, 0xFE363C83,
    0x0002005B, (s32)btlevtcmd_StopWaitEvent, 0xFE363C83,
    0x00000021,
    0x0001005B, (s32)_get_present_item,
    0x0003005B, (s32)btlevtcmd_GetBodyId, 0xFE363C83, 0xFE363C84,
    0x0004005B, (s32)btlevtcmd_AnimeChangePoseType, 0xFE363C83, 0xFE363C84,
    0x00000034,
    0x0001005B, (s32)_get_item_announce,
    0x0002005B, (s32)_delete_present_item, 0xF1194D80,
    0x0003005B, (s32)btlevtcmd_GetUnitId, 0xFFFFFFFE, 0xFE363C80,
    0x00020019, 0xFE363C80, 0xFE363C83,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFE363C83,
    0x00000021,
    0x0002005B, (s32)btlevtcmd_StartWaitEvent, 0xFFFFFFFE,
    0x00000002, 0x00000001,
};

__declspec(section ".data") f32 lbl_80371490[] = { -1.0f, 1.0f };
