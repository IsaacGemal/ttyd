#include "unit/party/unit_party_christine.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "battle/battle_monosiri.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"
#include "window/win_log.h"
#include "system.h"
#include <dolphin/types.h>

extern const f32 float_neg50_8041e054;
extern const f32 float_50_8041e058;
extern const f32 float_0_8041e05c;

USER_FUNC(_set_hustle) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(work, BattleTransID(event, value));
    if (unit != NULL) {
        if (unit->movesRemaining <= 0) {
            unit->movesRemaining++;
            unit->moveState = 0;
            unit->mMoveColorLv = 0xFF;
            if (unit->maxMovesThisTurn <= 0) {
                unit->maxMovesThisTurn = 1;
                if (BtlUnit_CheckStatus(unit, STATUS_SLOW)) {
                    unit->mTokenFlags |= 0x2000;
                }
            }
        } else {
            BtlUnit_OnStatusFlag(unit, 0x2000000);
        }
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (99.34%): instruction sequence is identical; only the callee-saved register split differs
// (target: work=r28, args=r30, id1=r31, id2=r30; ours: work=r30, args=r31, id1=r29, id2=r28).
USER_FUNC(_dictionary) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 id1;
    s32 id2;
    s32 value;
    BattleUnit* unit;
    s32 state;
    s32 ptr;

    id1 = BattleTransID(event, -2);
    value = evtGetValue(event, args[0]);
    id2 = BattleTransID(event, value);
    evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(work, id1);
    state = unit->work[0];
    ptr = unit->work[1];

    switch (state) {
    case 0:
        break;
    case 1:
        ptr = (s32)monosiriInit(BattleGetUnitPtr(work, id2)->currentKind, float_neg50_8041e054,
                                float_50_8041e058);
        state = 2;
        break;
    case 2:
        monosiriMain((void*)ptr);
        state = 3;
        break;
    case 3:
        monosiriMain((void*)ptr);
        break;
    case 4:
        monosiriClose((void*)ptr);
        monosiriMain((void*)ptr);
        state = 5;
        break;
    case 5:
        if (monosiriCloseWait((void*)ptr)) {
            monosiriExit((void*)ptr);
            state = 6;
        } else {
            monosiriMain((void*)ptr);
        }
        break;
    case 6:
        break;
    }

    unit->work[0] = state;
    unit->work[1] = ptr;
    return EVT_RETURN_DONE;
}

USER_FUNC(_monosiri_flag_on) {
    BattleUnit* unit;
    s32 value;

    value = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, value);
    battleSetUnitMonosiriFlag(unit->currentKind);
    return EVT_RETURN_DONE;
}

USER_FUNC(btlevtcmd_get_monosiri_msg_no) {
    s32* args = event->args;
    s32 value;
    s32 out;

    value = evtGetValue(event, args[0]);
    evtGetValue(event, args[1]);
    out = args[2];
    evtSetValue(event, out,
                (s32)battleGetUnitMonosiriPtr(
                          BattleGetUnitPtr(_battleWorkPointer, value)->currentKind)
                    ->tattleBtlTag);
    return EVT_RETURN_DONE;
}

// The original computes one base (unk_evt_80390b30 + 0x40, just below this file's .data) and indexes the four
// Christine weapons off it; naming each weapon symbol makes MWCC emit four separate lis/addi pairs.
extern u8 unk_evt_80390b30[];

#define TECH_MENU_ENTRY(table)                                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                                 \
    cmd->mWeaponTable[*count].mItem = 0;                                                   \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                      \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                               \
    cmd->mWeaponTable[*count].mIcon =                                                      \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                         \
    cmd->mWeaponTable[*count].mName =                                                      \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0));     \
    (*count)++;

// Every instruction but the base address is identical. The target keeps the +0x40 in the symbol addend
// (`lis r5, unk_evt_80390b30+0x40@ha` / `addi r30, r5, unk_evt_80390b30+0x40@l`); MWCC 2.6 folds it into the
// first weapon offset. Tried every opt level, compiler version 1.3-3.0, and pointer/struct/u32/array base forms.
// NONMATCHING (99.98%): the +0x40 sits in the later immediate instead of the symbol addend (same final address).
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = unk_evt_80390b30 + 0x40;
#endif
    s32 techLv = partyGetTechLv(BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY(weaponBase + 0x450); // partyWeapon_ChristineNormalAttack
    if (techLv >= 0) {
        TECH_MENU_ENTRY(weaponBase + 0x510); // partyWeapon_ChristineMonosiri
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x5D0); // partyWeapon_ChristineRenzokuAttack
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0x690); // partyWeapon_ChristineKiss
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_ChristineNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_ChristineMonosiri);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_ChristineRenzokuAttack);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_ChristineKiss);
    }
#endif
}

static s32 krb_get_dir(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    s32 out;
    f32 dx;
    f32 dy;

    x0 = evtGetFloat(event, args[0]);
    y0 = evtGetFloat(event, args[1]);
    x1 = evtGetFloat(event, args[2]);
    y1 = evtGetFloat(event, args[3]);
    out = evtGetValue(event, args[4]);
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx == 0.0f && dy == 0.0f) {
        evtSetValue(event, args[4], out);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[4], (s32)angleABf(dx, dy, float_0_8041e05c, float_0_8041e05c));
    return EVT_RETURN_DONE;
}
