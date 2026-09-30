#include "unit/party/unit_party_nokotarou.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "battle/battle_sub.h"
#include "eff/eff_nokotarou.h"
#include "driver/msgdrv.h"
#include "mario/mario_party.h"

const f64 double_to_int_802f8800 = 4503601774854144.0;

extern BattleWeapon partyWeapon_NokotarouKouraAttack;
extern BattleWeapon partyWeapon_NokotarouSyubibinKoura;
extern BattleWeapon partyWeapon_NokotarouKouraGuard;
extern BattleWeapon partyWeapon_NokotarouTsuranukiKoura;
extern u8 lbl_8038BEB8[]; // this unit's .data weapon tables

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

#define TECH_MENU_ENTRY_ICON(table, icon)                                             \
    cmd->mWeaponTable[*count].mIndex = -1;                                            \
    cmd->mWeaponTable[*count].mItem = 0;                                              \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                 \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                          \
    cmd->mWeaponTable[*count].mIcon = (icon);                                         \
    cmd->mWeaponTable[*count].mName =                                                 \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0)); \
    (*count)++;

// The original addresses the four weapons from one base register (addi r30, r5, lbl_8038BEB8@l, then
// +0x844/+0x904/+0x9C4/+0xA84); naming each weapon directly does not reproduce that.
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
#ifdef MUST_MATCH
    u8* const weaponBase = lbl_8038BEB8;
#endif
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

#ifdef MUST_MATCH
    TECH_MENU_ENTRY_ICON(weaponBase + 0x844, 0x150);  // partyWeapon_NokotarouKouraAttack
    if (techLv >= 0) {
        TECH_MENU_ENTRY_ICON(weaponBase + 0x904, 0x151); // partyWeapon_NokotarouSyubibinKoura
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(weaponBase + 0x9C4); // partyWeapon_NokotarouKouraGuard
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(weaponBase + 0xA84); // partyWeapon_NokotarouTsuranukiKoura
    }
#else
    TECH_MENU_ENTRY(&partyWeapon_NokotarouKouraAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY(&partyWeapon_NokotarouSyubibinKoura);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY(&partyWeapon_NokotarouKouraGuard);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY(&partyWeapon_NokotarouTsuranukiKoura);
    }
#endif
}

USER_FUNC(_check_mario_move_count) {
    s32 index = event->args[0];
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);

    evtSetValue(event, index, mario->movesRemaining);
    return EVT_RETURN_DONE;
}

USER_FUNC(_color_lv_set) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 mode = evtGetValue(event, args[1]);
    s32 target = args[2];
    s32 colorLv = evtGetValue(event, target);
    BattleUnit* unit = BattleGetUnitPtr(work, BattleTransID(event, id));

    if (mode != 0) {
        unit->mMoveColorLv = colorLv;
    } else {
        evtSetValue(event, target, unit->mMoveColorLv);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(_tsuranuki_effect_control) {
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    EffectEntry* effect;
    Vec pos;
    s32 target;
    s32 id;
    f32 t;

    id = evtGetValue(event, event->args[0]);
    wp = _battleWorkPointer;
    unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    part = BtlUnit_GetPartsPtr(unit, 1);

    if (isFirstCall) {
        effect = effNokotarouEntry(0, 1000, pos.x, pos.y, pos.z, unit->mSizeMultiplier);
        event->userdata[0] = (s32)effect;
        part->work[2] = 0;
        part->work[1] = 0;
        event->userdata[1] = 0;
    }

    target = part->work[1] * 100;
    if (event->userdata[1] != target) {
        if (event->userdata[1] < target) {
            event->userdata[1] += 10;
            if (event->userdata[1] > target) {
                event->userdata[1] = target;
            }
        } else {
            event->userdata[1] -= 10;
            if (event->userdata[1] < target) {
                event->userdata[1] = target;
            }
        }
    }

    t = float_0p006_8041e03c * (f32)event->userdata[1] + float_2_8041e038;
    t *= unit->mSizeMultiplier;
    effect = (EffectEntry*)event->userdata[0];
    ((NokotarouWork*)effect->userdata)->x2 = pos.x;
    ((NokotarouWork*)effect->userdata)->y2 = pos.y;
    ((NokotarouWork*)effect->userdata)->z2 = pos.z;
    ((NokotarouWork*)effect->userdata)->scale = t;

    if (part->work[2] != 0) {
        part->work[2] = 0;
        effSoftDelete(effect);
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

static USER_FUNC(_check_guard_koura) {
    BattleWork* wp = _battleWorkPointer;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 index = args[1];
    BattleUnit* unit = BattleGetUnitPtr(wp, BattleTransID(event, id));
    BattleUnit* koura;

    if (!(unit->mAttributeFlags & kShellShielded)) {
        evtSetValue(event, index, -1);
        return EVT_RETURN_DONE;
    }

    koura = BtlUnit_GetGuardKouraPtr(unit);
    if (koura == NULL) {
        evtSetValue(event, index, -1);
        return EVT_RETURN_DONE;
    }

    evtSetValue(event, index, koura->unitId);
    return EVT_RETURN_DONE;
}
