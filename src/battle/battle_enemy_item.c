#include "battle/battle.h"
#include "battle/battle_unit.h"
#include "data/item_data.h"
#include "system.h"

extern ItemData itemDataTable[];

s32 BattleSamplingEnemy(u8* out, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4, s32 arg5,
                        s32 arg6);
BOOL BtlUnit_CanActStatus(BattleUnit* unit);

void* _check_status_recover_item(BattleUnit* unit);
void* _check_status_support_item(BattleUnit* unit);
void* _check_status_attack_item(BattleUnit* unit);
void* _check_attack_item(BattleUnit* unit);
void* _check_fp_recover_item(BattleUnit* unit);
void* _check_hp_recover_item(BattleUnit* unit);
void* BattleEnemyUseItemCheck(BattleUnit* unit);

// Keep the local _check_* helpers out of line: the original calls them with `bl` instead of
// inlining their bodies into BattleEnemyUseItemCheck.
#pragma dont_inline on
// The original saves r30/r31 with stmw/lmw (size-oriented prologue) and lays the switch out the same way.
#pragma optimize_for_size on
void* BattleEnemyUseItemCheck(BattleUnit* unit) {
    s32 item = unit->heldItem;
    BattleWeapon* weapon;
    void* result;

    if (item == 0) {
        return 0;
    }
    weapon = itemDataTable[item].battle_params;
    if (weapon == NULL) {
        return 0;
    }
    if ((unit->mTokenFlags & kConfusedAction) && !(weapon->special & 0x10000)) {
        return 0;
    }
    switch (item) {
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x8b:
    case 0x90:
        return _check_attack_item(unit);
    case 0x89:
    case 0x8a:
    case 0x8c:
    case 0x8d:
    case 0x92:
    case 0x93:
        return _check_status_attack_item(unit);
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x8e:
    case 0x8f:
        return _check_status_support_item(unit);
    case 0x99:
        return _check_status_recover_item(unit);
    case 0x9d:
        return _check_hp_recover_item(unit);
    case 0x9e:
        return _check_fp_recover_item(unit);
    default:
        if ((u8)itemDataTable[item].hp_restored != 0) {
            result = _check_hp_recover_item(unit);
            if (result != NULL) {
                return result;
            }
        }
        if ((u8)itemDataTable[item].fp_restored != 0) {
            result = _check_fp_recover_item(unit);
            if (result != NULL) {
                return result;
            }
        }
        return 0;
    }
}
#pragma optimize_for_size off

void* _check_hp_recover_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        BattleUnit* target =
            BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));

        if (unit->mAttributeFlags & kInactive) {
            continue;
        }
        if (target->currentHp >= target->maxHp) {
            continue;
        }
        return itemDataTable[item].battle_params->mAttackEvtScript;
    }
    return 0;
}

void* _check_fp_recover_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        BattleUnit* target =
            BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));
        s32 maxFp;

        if (unit->mAttributeFlags & kInactive) {
            continue;
        }
        maxFp = BtlUnit_GetMaxFp(target);
        if (BtlUnit_GetFp(target) >= maxFp) {
            continue;
        }
        return itemDataTable[item].battle_params->mAttackEvtScript;
    }
    return 0;
}

void* _check_attack_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    if ((s8)sampling[0xa6c] <= 0) {
        return 0;
    }

    if (item == 0x8b || item == 0x85) {
        for (i = 0; i < (s8)sampling[0xa6c]; i++) {
            BattleUnit* target =
                BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));
            if (target->currentKind != 0xe1) {
                continue;
            }
            if (unit->mAttributeFlags & kInactive) {
                continue;
            }
            if (!BtlUnit_CanActStatus(target)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
        }
    }

    if (irand(100) >= wp->turnCount * 0x14) {
        return 0;
    }
    return itemDataTable[item].battle_params->mAttackEvtScript;
}

void* _check_status_attack_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    if ((s8)sampling[0xa6c] <= 0) {
        return 0;
    }

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        BattleUnit* target =
            BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));
        if (unit->mAttributeFlags & kInactive) {
            continue;
        }
        switch (item) {
        case 0x8a:
        case 0x8c:
            if (BtlUnit_CanActStatus(target)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x92:
            if (BtlUnit_CanActStatus(target) && !BtlUnit_CheckStatus(target, 0xb)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x93:
            if (BtlUnit_CanActStatus(target) && !BtlUnit_CheckStatus(target, 0xf)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x89:
            if (BtlUnit_CanActStatus(target) && !BtlUnit_CheckStatus(target, 0x5)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x8d:
            if (BtlUnit_CanActStatus(target) && !BtlUnit_CheckStatus(target, 0x3)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        }
    }
    return 0;
}

void* _check_status_support_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    if ((s8)sampling[0xa6c] <= 0) {
        return 0;
    }

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        BattleUnit* target =
            BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));
        if (unit->mAttributeFlags & kInactive) {
            continue;
        }
        switch (item) {
        case 0x8e:
            if (!BtlUnit_CanActStatus(target)) {
                break;
            }
            if (!BtlUnit_CheckStatus(target, 0xa)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x8f:
            if (!BtlUnit_CheckStatus(target, 0xe)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x86:
            if (!BtlUnit_CheckStatus(target, 0x12)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x87:
            if (!BtlUnit_CheckStatus(target, 0x6)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        case 0x88:
            if (!BtlUnit_CheckStatus(target, 0x7)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        }
    }
    return 0;
}

void* _check_status_recover_item(BattleUnit* unit) {
    BattleWork* wp = _battleWorkPointer;
    s32 item;
    u8* sampling;
    BattleWeapon* weapon;
    s32 i;

    item = unit->heldItem;
    sampling = wp->field_428;
    weapon = itemDataTable[item].battle_params;

    BattleSamplingEnemy(sampling, weapon, unit->unitId, BtlUnit_GetEnemyBelong(unit),
                        *(s32*)&weapon->field_64[0], *(s32*)&weapon->field_64[4],
                        (s32)(s8)wp->alliances[unit->mAlliance].attackDirection);

    if ((s8)sampling[0xa6c] <= 0) {
        return 0;
    }

    for (i = 0; i < (s8)sampling[0xa6c]; i++) {
        BattleUnit* target =
            BattleGetUnitPtr(wp, *(s16*)(sampling + (s8)sampling[0xa6d + i] * 0x24 + 4));
        if (unit->mAttributeFlags & kInactive) {
            continue;
        }
        switch (item) {
        case 0x99:
            if (BtlUnit_CheckStatus(target, 1) || BtlUnit_CheckStatus(target, 2) ||
                BtlUnit_CheckStatus(target, 3) || BtlUnit_CheckStatus(target, 4) ||
                BtlUnit_CheckStatus(target, 5) || BtlUnit_CheckStatus(target, 6) ||
                BtlUnit_CheckStatus(target, 8) || BtlUnit_CheckStatus(target, 9) ||
                BtlUnit_CheckStatus(target, 0xb) || BtlUnit_CheckStatus(target, 0xf) ||
                BtlUnit_CheckStatus(target, 0x14)) {
                return itemDataTable[item].battle_params->mAttackEvtScript;
            }
            break;
        }
    }
    return 0;
}
