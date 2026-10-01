#include <dolphin/types.h>
#include "unit/party/unit_party_chuchurina.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "driver/dispdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_8037EC10[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

USER_FUNC(_chuchu_make_extra_work_area) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    *(void**)((u8*)unit + 0x310) = BattleAlloc(0xC0);
    return EVT_RETURN_DONE;
}

extern BattleWeapon partyWeapon_ChuchurinaNormalAttackLeftLast;
extern BattleWeapon partyWeapon_ChuchurinaItemSteal;
extern BattleWeapon partyWeapon_ChuchurinaMadowaseAttack;
extern BattleWeapon partyWeapon_ChuchurinaKiss;

#define TECH_MENU_ENTRY(table)                                                        \
    cmd->mWeaponTable[*count].mIndex = -1;                                            \
    cmd->mWeaponTable[*count].mItem = 0;                                              \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                 \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                          \
    cmd->mWeaponTable[*count].mIcon =                                                 \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                    \
    cmd->mWeaponTable[*count].mName =                                                 \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0)); \
    (*count)++;

// The original addresses the four weapons from one base register (addi r30, r5, lbl_8037EC10@l, then
// +0x6A0/+0x8E0/+0x820/+0x9A0); naming each weapon directly does not reproduce that.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = lbl_8037EC10;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x6A0);  // partyWeapon_ChuchurinaNormalAttackLeftLast
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x8E0); // partyWeapon_ChuchurinaItemSteal
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x820); // partyWeapon_ChuchurinaMadowaseAttack
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0x9A0); // partyWeapon_ChuchurinaKiss
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_ChuchurinaNormalAttackLeftLast);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaItemSteal);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaMadowaseAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_ChuchurinaKiss);
    }
#endif
}

// NONMATCHING (92.29%): same instruction sequence; the callee-saved registers are assigned in a different order
// (target: event r29, args r27, work r28; ours r31, r29, r30).
USER_FUNC(_get_binta_hit_position) {
    s32 id1, id2, outX, outY, outZ;
    BattleWork* work;
    s32* args;
    BattleUnit* unit;
    f32 z, y, x;
    f32 scale;
    s32 dir;

    args = event->args;
    work = _battleWorkPointer;
    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, id1));
    unit = BattleGetUnitPtr(work, BattleTransID(event, id2));
    dir = unit->faceDirection;
    BtlUnit_GetPos(unit, &x, &y, &z);
    scale = unit->mSizeMultiplier;
    x += (f32)dir * (unit->bintaHitOffset.x * scale);
    y += unit->bintaHitOffset.y * scale;
    z += unit->bintaHitOffset.z * scale;
    evtSetFloat(event, outX, x);
    evtSetFloat(event, outY, y);
    evtSetFloat(event, outZ, z);
    return EVT_RETURN_DONE;
}

USER_FUNC(_make_madowase_weapon) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, src, val;
    s32 dst;
    BattleWeapon* area;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    dst = args[0];
    area = *(BattleWeapon**)((u8*)unit + 0x310);
    src = evtGetValue(event, dst);
    val = evtGetValue(event, args[1]);
    memcpy(area, (void*)src, 0xC0);
    area->dizzyChance = val;
    evtSetValue(event, dst, (s32)area);
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_itemsteal_param) {
    BattleWork* work;
    s32* args = event->args;
    BattleUnit* unit;
    s32 id, a, b, chance, steal;

    id = evtGetValue(event, args[0]);
    a = args[1];
    work = _battleWorkPointer;
    b = args[2];
    unit = BattleGetUnitPtr(work, BattleTransID(event, id));
    chance = 0x50 - ((u8)work->actionCommands.mAcDifficulty - 3) * 0x14;
    if (chance <= 5) {
        chance = 5;
    }
    steal = 100 - unit->mKindParams->itemStealParam;
    evtSetValue(event, a, chance);
    evtSetValue(event, b, steal);
    return EVT_RETURN_DONE;
}

// .sbss:0x0 | 0x80418A28
s32 mono_alpha;

void mono_capture(CameraId cameraId, void* param) {
    BattleUnit* unit = param;
    BattleUnitPart* parts = BtlUnit_GetPartsPtr(unit, 1);

    *(smartEntry**)((u8*)unit + 0x318) = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 1, 0, 0), 1);
    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, 1, 0);
    GXCopyTex((*(smartEntry**)((u8*)unit + 0x318))->address, 0);
    GXPixModeSync();
    *(u32*)((u8*)parts + 0x204) &= ~0x100;
}

USER_FUNC(mono_main) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    dispEntry(CAMERA_2D, float_910_8041ddbc, 0, mono_disp, unit);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(mono_capture_event) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    BtlUnit_GetPartsPtr(unit, 1);
    dispEntry(CAMERA_2D, float_910_8041ddbc, 0, mono_capture, unit);
    return EVT_RETURN_DONE;
}

USER_FUNC(mono_on) {
    if (isFirstCall) {
        mono_alpha = 0;
    }
    mono_alpha += 10;
    if (mono_alpha > 0xFF) {
        mono_alpha = 0xFF;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(mono_off) {
    BattleUnit* unit;
    s32 id;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    mono_alpha -= 10;
    if (mono_alpha < 0) {
        mono_alpha = 0;
        BtlUnit_GetPartsPtr(unit, 1);
        if (*(smartEntry**)((u8*)unit + 0x318) != NULL) {
            smartFree(*(smartEntry**)((u8*)unit + 0x318));
        }
        *(smartEntry**)((u8*)unit + 0x318) = NULL;
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}
