#include "battle/battle.h"
#include "battle/battle_ac_help.h"
#include "battle/battle_break_slot.h"
#include "battle/battle_disp.h"
#include "battle/battle_icon.h"
#include "battle/battle_information.h"
#include "battle/battle_menu_disp.h"
#include "battle/battle_seq.h"
#include "battle/battle_unit.h"
#include "mario/mario_pouch.h"
#include "memory.h"
#include <string.h>
#include "mario/mariost.h"
#include "driver/animdrv.h"
#include "system.h"
#include "statuswindow.h"



// forward declarations




extern BattleUnitSetup entryunit_party[7];

// The original object defines these; other translation units reference them.
extern BattleUnitKind unitdata_Mario;
extern BattleUnitKind unit_system;

// clang-format off
BattleUnitSetup entryunit_mario = { &unitdata_Mario, 0, 0x04000002, { -100.0f, 0.0f, -5.0f }, 0, { 0, 0, 0, 0 }, NULL };

BattleUnitSetup entryunit_system = { &unit_system, 0, 0x04000002, { 0.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL };
// clang-format on

u8 gap_08_80418974_sbss[4];
BattleWork* _battleWorkPointer;

void BattleConsumeReserveItem(void) {
    BattleWork* wp = _battleWorkPointer;
    s32 i;

    for (i = 0; i < 4; i++) {
        if ((s32)wp->reserveItems[i] != 0) {
            pouchRemoveItem((ItemType)wp->reserveItems[i]);
        }
        wp->reserveItems[i] = 0;
    }
}

BOOL BattleStatusWindowCheck(void) {
    BattleWork* wp = _battleWorkPointer;
    BOOL result;

    if (wp->statusWinField0) {
        result = FALSE;
    } else if (wp->statusWinField1) {
        result = FALSE;
    } else if (wp->statusWinAPRecovery) {
        result = TRUE;
    } else if (wp->statusWinEvent && wp->statusWinSystem) {
        result = TRUE;
    } else {
        result = FALSE;
    }

    if (result) {
        if (statusWinCheck() == 0) {
            statusWinOpen();
        }
    } else {
        if (statusWinCheck() != 0) {
            statusWinClose();
        }
    }

    return result;
}

void BattleStatusWindowSystemOff(void) {
    _battleWorkPointer->statusWinSystem = 0;
    BattleStatusWindowCheck();
}

void BattleStatusWindowEventOff(void) {
    _battleWorkPointer->statusWinEvent = 0;
    BattleStatusWindowCheck();
}

void BattleStatusWindowSystemOn(void) {
    _battleWorkPointer->statusWinSystem = 1;
    BattleStatusWindowCheck();
}

void BattleStatusWindowEventOn(void) {
    _battleWorkPointer->statusWinEvent = 1;
    BattleStatusWindowCheck();
}

void BattleStatusWindowAPRecoveryOff(void) {
    _battleWorkPointer->statusWinAPRecovery = 0;
    BattleStatusWindowCheck();
}

void BattleStatusWindowAPRecoveryOn(void) {
    _battleWorkPointer->statusWinAPRecovery = 1;
    BattleStatusWindowCheck();
}

void BattleMajinaiEndCheck(void) {
    BattleWork* wp = _battleWorkPointer;

    if (pouchGetPtr()->mMerleeCurseUsesRemaining == 0) {
        wp->flags |= 0x400000;
    }
}

void BattleMajinaiDone(void) {
    PouchData* pouch = pouchGetPtr();

    pouch->mMerleeCurseUsesRemaining--;
    if (pouch->mMerleeCurseUsesRemaining != 0) {
        pouch->mTurnsUntilMerleeActivation = -1;
        pouch->mMerleeNextCurseType = 0;
    } else {
        pouch->mTurnsUntilMerleeActivation = -1;
        pouch->mMerleeNextCurseType = 0;
    }
}

s32 BattleMajinaiCheck(void) {
    PouchData* pouch = pouchGetPtr();
    BattleWork* wp = _battleWorkPointer;
    s32 roll;

    if (pouch->mMerleeCurseUsesRemaining == 0) {
        return 0;
    }

    if (pouch->mTurnsUntilMerleeActivation < 0) {
        pouch->mMerleeNextCurseType = 0;
        pouch->mTurnsUntilMerleeActivation = irand(5) + 6;
        pouch->mTurnsUntilMerleeActivation--;
        return 0;
    }

    if (pouch->mMerleeNextCurseType != 0) {
        return pouch->mMerleeNextCurseType;
    }

    pouch->mTurnsUntilMerleeActivation--;
    if (pouch->mTurnsUntilMerleeActivation > 0) {
        return 0;
    }

    pouch->mTurnsUntilMerleeActivation = 0;

    if (wp->fbatInfo->setup->field_4 & 0x20) {
        roll = irand(100);
        if (roll < 45) {
            pouch->mMerleeNextCurseType = 1;
        } else if (roll < 90) {
            pouch->mMerleeNextCurseType = 2;
        } else {
            pouch->mMerleeNextCurseType = 3;
        }
    } else {
        roll = irand(100);
        if (roll < 30) {
            pouch->mMerleeNextCurseType = 1;
        } else if (roll < 60) {
            pouch->mMerleeNextCurseType = 2;
        } else if (roll < 80) {
            pouch->mMerleeNextCurseType = 3;
        } else {
            pouch->mMerleeNextCurseType = 4;
        }
    }

    return pouch->mMerleeNextCurseType;
}

BOOL battleDisableHResetCheck(void) {
    BattleWork* wp = _battleWorkPointer;

    if (gp->inBattle == 0) {
        return FALSE;
    }
    if (wp == NULL) {
        return FALSE;
    }
    return (wp->flags >> 29) & 1;
}

BOOL BattleAfterReactionMain(void) {
    s32 i;
    s32 id;
    BattleWork* wp;
    s32 data;
    BattleUnit* unit;
    BattleAfterReactionWork* q;

    wp = _battleWorkPointer;
    q = wp->afterReaction;

    for (i = 0; i < 64; i++) {
        id = q[i].id;

        if (id != -1) {
            data = q[i].field_4;
            unit = BattleGetUnitPtr(wp, id);

            if (unit == NULL) {
                BattleAfterReactionRelease(id, data);
                return BattleAfterReactionMain();
            }

            BattleConsumeReserveItem();
            if (BattleCheckConcluded(wp)) {
                return FALSE;
            }

            BattleAfterReactionRelease(id, data);
            {
                EventEntry* event = evtEntry(BtlUnit_GetData(unit, data), 0xA, 0);
                if (event != NULL) {
                    unit->attackEventId = event->eventId;
                    event->unitId = unit->unitId;
                }
            }
            return TRUE;
        }
    }

    return FALSE;
}

void BattleAfterReactionRelease(s32 id, s32 data) {
    s32 i, j;
    BattleAfterReactionWork* q = _battleWorkPointer->afterReaction;

    for (i = 0; i < 64; i++) {
        if (q[i].id != -1 && q[i].id == id && q[i].field_4 == data) {
            for (j = i; j < 63; j++) {
                q[j].id = q[j + 1].id;
                q[j].field_4 = q[j + 1].field_4;
            }
            q[63].id = -1;
            q[63].field_4 = 0;
            i--;
        }
    }
}

void BattleAfterReactionEntry(s32 id, s32 data) {
    BattleAfterReactionWork* q = _battleWorkPointer->afterReaction;
    s32 i;

    for (i = 0; i < 64; i++) {
        if (q[i].id != -1 && q[i].id == id) {
            return;
        }
    }

    for (i = 0; i < 64; i++) {
        if (q[i].id == -1) {
            break;
        }
    }

    if (i < 64) {
        q[i].id = id;
        q[i].field_4 = data;
    }
}

void BattleAfterReactionQueueInit(void) {
    BattleAfterReactionWork* q = _battleWorkPointer->afterReaction;
    s32 i;

    for (i = 0; i < 64; i++) {
        q[i].id = -1;
        q[i].field_4 = 0;
    }
}

void BattleCheckUnitBroken(BattleWork* wp) {
    s32 i;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->flags & 0x80000000) {
            BtlUnit_Delete(unit);
        }
    }
}

f32 BattleGetFloorHeight(BattleWork* wp, f32 x, f32 y, f32 z) {
    BattleStage* stage = BattleStageGetPtr();

    if (z <= *(f32*)&stage->field_0x0[0xB2C]) {
        return 0.0f;
    }
    return *(f32*)&stage->field_0x0[0xB34];
}

s32 BattleGetStockExp(BattleWork* wp) {
    s32 exp = wp->stockExp;

    if (exp <= 0) {
        return 1;
    }
    if (exp >= 100) {
        return 100;
    }
    return exp;
}

void BattleStoreExp(BattleWork* wp, s32 exp) {
    wp->stockExp += exp;
}

void BattleStoreCoin(BattleWork* wp, s32 coins) {
    *(s32*)(wp->fbatInfo->field_0x14) += coins;
}

void BattlePartyInfoWorkInit(BattleWork* wp) {
    BattleWorkPartyInfo* partyInfo;
    int i;

    // TODO: add macros for Party IDs
    for (i = 0xE0; i < 0xE7; i++) {
        partyInfo = &wp->partyInfo[i - 0xE0];
        partyInfo->field_0 = 1.0;
        partyInfo->field_24 = 0;
        memset(partyInfo->field_4, 0, sizeof(partyInfo->field_4));
    }
}

void _EquipItem(BattleUnit* unit, EquipFlags flags, ItemType type) {
    if (type == ITEM_NULL) {
        return;
    }

    if (type == ITEM_REFUND) {
        unit->badgesEquipped.Refund++;
        return;
    } else if (type == ITEM_SIMPLIFIER) {
        unit->badgesEquipped.Simplifier++;
        return;
    } else if (type == ITEM_UNSIMPLIFIER) {
        unit->badgesEquipped.Unsimplifier++;
        return;
    }

    if ((flags & EQUIP_PARTNER) == 0) {
        if (type == ITEM_SPIKE_SHIELD) {
            unit->badgesEquipped.SpikeShield++;
            return;
        } else if (type == ITEM_ICE_POWER) {
            unit->badgesEquipped.IcePower++;
            return;
        } else if (type == ITEM_CLOSE_CALL) {
            unit->badgesEquipped.CloseCall++;
            return;
        } else if (type == ITEM_PRETTY_LUCKY) {
            unit->badgesEquipped.PrettyLucky++;
            return;
        } else if (type == ITEM_LUCKY_DAY) {
            unit->badgesEquipped.LuckyDay++;
            return;
        } else if (type == ITEM_DAMAGE_DODGE) {
            unit->badgesEquipped.DamageDodge++;
            return;
        } else if (type == ITEM_POWER_PLUS) {
            unit->badgesEquipped.PowerPlus++;
            return;
        } else if (type == ITEM_P_UP_D_DOWN) {
            unit->badgesEquipped.PUpDDown++;
            return;
        } else if (type == ITEM_ALL_OR_NOTHING) {
            unit->badgesEquipped.AllOrNothing++;
            return;
        } else if (type == ITEM_MEGA_RUSH) {
            unit->badgesEquipped.MegaRush++;
            return;
        } else if (type == ITEM_POWER_RUSH) {
            unit->badgesEquipped.PowerRush++;
            return;
        } else if (type == ITEM_P_DOWN_D_UP) {
            unit->badgesEquipped.PDownDUp++;
            return;
        } else if (type == ITEM_DOUBLE_PAIN) {
            unit->badgesEquipped.DoublePain++;
            return;
        } else if (type == ITEM_LAST_STAND) {
            unit->badgesEquipped.LastStand++;
            return;
        } else if (type == ITEM_DEFEND_PLUS) {
            unit->badgesEquipped.DefendPlus++;
            return;
        } else if (type == ITEM_HAPPY_HEART) {
            unit->badgesEquipped.HappyHeart++;
            return;
        } else if (type == ITEM_HAPPY_FLOWER) {
            unit->badgesEquipped.HappyFlower++;
            return;
        } else if (type == ITEM_RETURN_POSTAGE) {
            unit->badgesEquipped.ReturnPostage++;
            return;
        } else if (type == ITEM_HP_PLUS) {
            unit->badgesEquipped.HpPlus++;
            return;
        } else if (type == ITEM_FP_PLUS) {
            unit->badgesEquipped.FpPlus++;
            return;
        } else if (type == ITEM_FEELING_FINE) {
            unit->badgesEquipped.FeelingFine++;
            return;
        } else if (type == ITEM_ZAP_TAP) {
            unit->badgesEquipped.ZapTap++;
            return;
        } else if (type == ITEM_PITY_FLOWER) {
            unit->badgesEquipped.PityFlower++;
            return;
        } else if (type == ITEM_HP_DRAIN_BADGE) {
            unit->badgesEquipped.HpDrain++;
            return;
        } else if (type == ITEM_FP_DRAIN) {
            unit->badgesEquipped.FpDrain++;
            return;
        } else if (type == ITEM_UNUSED_DEFEND_BADGE) {
            unit->badgesEquipped.DefendUnused++;
            return;
        } else if (type == ITEM_CHARGE) {
            unit->badgesEquipped.Charge++;
            return;
        } else if (type == ITEM_SUPER_CHARGE) {
            unit->badgesEquipped.SuperCharge++;
            return;
        } else if (type == ITEM_FLOWER_SAVER) {
            unit->badgesEquipped.FlowerSaver++;
            return;
        } else if (type == ITEM_SUPER_APPEAL) {
            unit->badgesEquipped.SuperAppeal++;
            return;
        } else if (type == ITEM_LUCKY_START) {
            unit->badgesEquipped.LuckyStart++;
            return;
        }
    }

    if ((flags & EQUIP_MARIO) == 0) {
        if (type == ITEM_CLOSE_CALL_P) {
            unit->badgesEquipped.CloseCall++;
            return;
        } else if (type == ITEM_PRETTY_LUCKY_P) {
            unit->badgesEquipped.PrettyLucky++;
            return;
        } else if (type == ITEM_LUCKY_DAY_P) {
            unit->badgesEquipped.LuckyDay++;
            return;
        } else if (type == ITEM_DAMAGE_DODGE_P) {
            unit->badgesEquipped.DamageDodge++;
            return;
        } else if (type == ITEM_ALL_OR_NOTHING_P) {
            unit->badgesEquipped.AllOrNothing++;
            return;
        } else if (type == ITEM_POWER_PLUS_P) {
            unit->badgesEquipped.PowerPlus++;
            return;
        } else if (type == ITEM_POWER_RUSH_P) {
            unit->badgesEquipped.PowerRush++;
            return;
        } else if (type == ITEM_DEFEND_PLUS_P) {
            unit->badgesEquipped.DefendPlus++;
            return;
        } else if (type == ITEM_UNUSED_DEFEND_BADGE_P) {
            unit->badgesEquipped.DefendUnused++;
            return;
        } else if (type == ITEM_P_DOWN_D_UP_P) {
            unit->badgesEquipped.PDownDUp++;
            return;
        } else if (type == ITEM_PITY_FLOWER_P) {
            unit->badgesEquipped.PityFlower++;
            return;
        } else if (type == ITEM_HP_DRAIN_P) {
            unit->badgesEquipped.HpDrain++;
            return;
        } else if (type == ITEM_FP_DRAIN_P) {
            unit->badgesEquipped.FpDrain++;
            return;
        } else if (type == ITEM_CHARGE_P) {
            unit->badgesEquipped.Charge++;
            return;
        } else if (type == ITEM_SUPER_CHARGE_P) {
            unit->badgesEquipped.SuperCharge++;
            return;
        } else if (type == ITEM_HAPPY_HEART_P) {
            unit->badgesEquipped.HappyHeart++;
            return;
        } else if (type == ITEM_HP_PLUS_P) {
            unit->badgesEquipped.HpPlus++;
            return;
        } else if (type == ITEM_LAST_STAND_P) {
            unit->badgesEquipped.LastStand++;
            return;
        } else if (type == ITEM_MEGA_RUSH_P) {
            unit->badgesEquipped.MegaRush++;
            return;
        } else if (type == ITEM_P_UP_D_DOWN_P) {
            unit->badgesEquipped.PUpDDown++;
            return;
        } else if (type == ITEM_FLOWER_SAVER_P) {
            unit->badgesEquipped.FlowerSaver++;
            return;
        } else if (type == ITEM_FEELING_FINE_P) {
            unit->badgesEquipped.FeelingFine++;
            return;
        } else if (type == ITEM_SUPER_APPEAL_P) {
            unit->badgesEquipped.SuperAppeal++;
            return;
        } else if (type == ITEM_LUCKY_START_P) {
            unit->badgesEquipped.LuckyStart++;
            return;
        }
    }

    if ((flags & EQUIP_MARIO) != 0) {
        if (type == ITEM_DOUBLE_DIP) {
            unit->badgesEquipped.DoubleDip++;
            if (unit->badgesEquipped.DoubleDip >= 2 && !unit->badgesEquipped.TripleDip) {
                unit->badgesEquipped.TripleDip++;
            }
            return;
        } else if (type == ITEM_TRIPLE_DIP) {
            unit->badgesEquipped.TripleDip++;
            return;
        } else if (type == ITEM_JUMPMAN) {
            unit->badgesEquipped.Jumpman++;
            return;
        } else if (type == ITEM_HAMMERMAN) {
            unit->badgesEquipped.Hammerman++;
            return;
        }
    }

    if (flags & EQUIP_PARTNER && type == ITEM_DOUBLE_DIP_P) {
        unit->badgesEquipped.DoubleDip++;
        if (unit->badgesEquipped.DoubleDip >= 2 && !unit->badgesEquipped.TripleDip) {
            unit->badgesEquipped.TripleDip++;
        }
        return;
    }

    if ((flags & EQUIP_MARIO || flags & EQUIP_PARTNER) && type == ITEM_AUTO_COMMAND_BADGE) {
        unit->badgesEquipped.AutoCommand++;
        return;
    }
}

void BtlUnit_EquipItem(BattleUnit* unit, EquipFlags flags, ItemType type) {
    PouchData* pouch = pouchGetPtr();
    int i;

    if (unit) {
        if (flags & EQUIP_RESET) {
            for (i = 0; i < 40; i++) {
                ((u8*)unit)[i + offsetof(BattleUnit, badgesEquipped)] = FALSE;
            }
        }

        if (unit->currentKind != UNIT_SYSTEM) {
            if (flags & (EQUIP_MARIO | EQUIP_PARTNER)) {
                for (i = 0; i < 200; i++) {
                    _EquipItem(unit, flags & ~EQUIP_RESET, pouch->equippedBadges[i]);
                }
            } else {
                _EquipItem(unit, flags, type);
            }
            BtlUnit_ReviseHpFp(unit);
        }
    }
}

BattleUnitType BattleTransPartyIdToUnitKind(s32 partyId) {
    switch (partyId) {
        case 1:
            return 0xE0;
        case 2:
            return 0xE1;
        case 4:
            return 0xE2;
        case 5:
            return 0xE3;
        case 6:
            return 0xE4;
        case 3:
            return 0xE5;
        case 7:
            return 0xE6;
        case 0:
        default:
            return 0x0;
    }
}

s32 BattleTransPartyId(BattleUnitType kind) {
    switch (kind) {
        case 0x50:
        case 0x89:
        case 0xE0:
            return 1;
        case 0x51:
        case 0x8A:
        case 0xE1:
            return 2;
        case 0x52:
        case 0x8B:
        case 0xE2:
            return 4;
        case 0x53:
        case 0x8C:
        case 0xE3:
            return 5;
        case 0x8D:
        case 0xE4:
            return 6;
        case 0x8E:
        case 0xE5:
            return 3;
        case 0x8F:
        case 0xE6:
            return 7;
        case 0x00:
            return 0;
        default:
            return 0;
    }
}

BattleUnit* BattleChangeParty(BattleWork* wp) {
    BattleUnit* oldParty;
    BattleUnit* newParty;
    s32 oldId;
    s32 newKind;
    s32 i;
    f32 y2, x, y, z;

    newKind = *(s32*)((u8*)wp + 0x1C68); // 0x1C68 is inside commandMenu, not yet named
    oldParty = BattleGetPartyPtr(wp);
    oldId = oldParty->unitId;

    if (newKind < 0xE0 || newKind >= 0xE7) {
        return NULL;
    }

    newParty = BtlUnit_Spawn(&entryunit_party[newKind - 0xE0], 1);

    for (i = 0; i < 64; i++) {
        s32 id = wp->unitIdTable[i];

        if (id == -1) {
            continue;
        }
        if (id == oldId) {
            wp->unitIdTable[i] = newParty->unitId;
        } else if (id == newParty->unitId) {
            wp->unitIdTable[i] = oldId;
        }
    }

    BtlUnit_GetHomePos(newParty, &x, &y, &z);
    BtlUnit_GetHomePos(oldParty, &x, &y2, &z);
    BtlUnit_EquipItem(newParty, EQUIP_RESET | EQUIP_PARTNER, ITEM_NULL);
    y += BattleGetFloorHeight(_battleWorkPointer, x, y, z);
    BtlUnit_SetHomePos(newParty, x, y, z);

    if (wp->activeUnitId == oldId) {
        wp->activeUnitId = newParty->unitId;
    }
    newParty->moveState = oldParty->moveState;
    newParty->movesRemaining = oldParty->movesRemaining;
    newParty->maxMovesThisTurn = oldParty->maxMovesThisTurn;
    newParty->activeTurns = oldParty->activeTurns;
    BtlUnit_SetParamToPouch(oldParty);
    oldParty->flags |= 8;
    newParty->mMoveColorLv = oldParty->mMoveColorLv;
    newParty->field_0x30E[0] = oldParty->field_0x30E[0];

    return newParty;
}

BOOL BattlePartyAnimeLoad(BattleUnitType partyId) {
    BattleUnitSetup* setup = &entryunit_party[partyId - 0xE0];
    return animEffectAsync(setup->mUnitKindParams->mParts->modelName, 1);
}

BattleUnit* BattleGetPartnerPtr(BattleWork* work, BattleUnit* unit) {
    if (unit->currentKind == 0xDE) {
        return BattleGetPartyPtr(work);
    }
    return BattleGetMarioPtr(work);
}

BattleUnit* BattleGetPartyPtr(BattleWork* wp) {
    s32 i;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit && !(unit->flags & 8) && unit->currentKind >= 0xE0 && unit->currentKind < 0xE7 &&
            unit->mAlliance == 0) {
            return unit;
        }
    }

    return NULL;
}

BattleUnit* BattleGetMarioPtr(BattleWork* wp) {
    s32 i;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->currentKind == 0xDE && unit->mAlliance == 0) {
            return unit;
        }
    }

    return NULL;
}

BattleUnit* BattleGetSystemPtr(BattleWork* wp) {
    s32 i;

    for (i = 0; i < 64; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->currentKind == 0xDC) {
            return unit;
        }
    }

    return NULL;
}

BattleUnitPart* BattleGetUnitPartsPtr(s32 index, s32 partNum) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, index);
    return BtlUnit_GetPartsPtr(unit, partNum);
}

void BattleSetUnitPtr(BattleWork* wp, s32 index, BattleUnit* unit) {
    if (index < 64) {
        wp->units[index] = unit;
    }
}

BattleUnit* BattleGetUnitPtr(BattleWork* wp, s32 index) {
    if (index == -1) {
        return NULL;
    }
    if (index < -1) {
        return NULL;
    }
    if (index < 64) {
        return wp->units[index];
    }

    return NULL;
}

void BattleFree(void* ptr) {
    if (ptr)
        __memFree(HEAP_BATTLE, ptr);
}

void* BattleAlloc(u32 size) {
    return __memAlloc(HEAP_BATTLE, size);
}

void BattleIncSeq(BattleWork* wp, BattleSequence seq) {
    s32 value = BattleGetSeq(wp, seq);
    BattleSetSeq(wp, seq, value + 1);
}

s32 BattleGetSeq(BattleWork* wp, BattleSequence seq) {
    switch (seq) {
        case SEQ_UNKNOWN:
            return wp->seqUnknownValue;
        case SEQ_INIT:
            return wp->seqInitValue;
        case SEQ_FIRST_ACT:
            return wp->seqFirstActValue;
        case SEQ_TURN:
            return wp->seqTurnValue;
        case SEQ_PHASE:
            return wp->seqPhaseValue;
        case SEQ_MOVE:
            return wp->seqMoveValue;
        case SEQ_ACT:
            return wp->seqActValue;
        case SEQ_END:
            return wp->seqEndValue;
        default:
            return 0;
    }
}

void BattleSetSeq(BattleWork* wp, BattleSequence seq, u32 value) {
    switch (seq) {
        case SEQ_UNKNOWN:
            wp->seqUnknownValue = value;
            break;
        case SEQ_INIT:
            wp->seqInitValue = value;
            break;
        case SEQ_FIRST_ACT:
            wp->seqFirstActValue = value;
            break;
        case SEQ_TURN:
            wp->seqTurnValue = value;
            break;
        case SEQ_PHASE:
            wp->seqPhaseValue = value;
            break;
        case SEQ_MOVE:
            wp->seqMoveValue = value;
            break;
        case SEQ_ACT:
            wp->seqActValue = value;
            break;
        case SEQ_END:
            wp->seqEndValue = value;
            break;
    }
}

void BattleSetMarioParamToFieldBattle(BattleWork* wp) {
    BattleUnit* mario;
    BattleUnit* party;

    mario = BattleGetMarioPtr(wp);
    BtlUnit_SetParamToPouch(mario);
    party = BattleGetPartyPtr(wp);
    if (party) {
        BtlUnit_SetParamToPouch(party);
    }
}

BOOL Btl_UnitSetup(BattleWork* wp) {
    int i;
    BattleUnitSetup* enemyData;
    NpcBattleInfo* setup;
    BattleGroupSetup* groupSetup;
    PouchData* pouch;
    BattleUnit* system;
    BattleUnit* mario;
    BattleUnitType partyId;
    BattleUnit* party;
    BattleUnit* enemy;
    ItemType heldItem;
    int j;
    f32 x, y, z;

    setup = wp->fbatInfo->setup;
    groupSetup = setup->groupSetup;
    pouch = pouchGetPtr();
    wp->alliances[0].allianceId = 2;
    wp->alliances[1].allianceId = 1;
    wp->alliances[2].allianceId = 0;
    wp->alliances[0].attackDirection = 1;
    wp->alliances[1].attackDirection = -1;
    wp->alliances[2].attackDirection = 1;
    wp->alliances[0].clearConditionMet = 0;
    wp->alliances[1].clearConditionMet = 0;
    wp->alliances[2].clearConditionMet = 0;

    system = BtlUnit_Entry(&entryunit_system);
    BtlUnit_EquipItem(system, EQUIP_MARIO | EQUIP_RESET, ITEM_NULL);
    mario = BtlUnit_Entry(&entryunit_mario);
    BtlUnit_EquipItem(mario, EQUIP_MARIO | EQUIP_RESET, ITEM_NULL);
    BtlUnit_SetParamFromPouch(mario);

    partyId = wp->fbatInfo->party;
    if (partyId) {
        party = BtlUnit_Entry(&entryunit_party[partyId - 0xE0]);
        BtlUnit_GetHomePos(party, &x, &y, &z);
        BtlUnit_SetHomePos(party, -140.0, y, 5.0);
        BtlUnit_SetPos(party, -140.0, y, 5.0);
        BtlUnit_EquipItem(party, EQUIP_PARTNER | EQUIP_RESET, ITEM_NULL);
        BtlUnit_SetParamFromPouch(party);
    }

    enemyData = groupSetup->enemyData;
    for (i = 0; i < groupSetup->numEnemies; enemyData++, i++) {
        enemy = BtlUnit_Entry(enemyData);
        enemy->groupSetupId = i;
        enemy->heldItem = ITEM_NULL;
        heldItem = setup->stolenItems[enemy->groupSetupId];
        if (heldItem) {
            enemy->heldItem = heldItem;
            enemy->flags |= 0x40000000u;
        } else {
            enemy->heldItem = setup->heldItems[i];
            BtlUnit_EquipItem(enemy, EQUIP_RESET, setup->heldItems[i]);
        }
        enemy->dropTable = setup->itemDropLists[i];
        enemy->currentHp = enemy->maxHp;
        enemy->currentFp = enemy->maxFp;
    }

    wp->activeUnitId = -1;
    wp->badgeEquippedFlags = 0;
    for (j = 0; j < 200; j++) {
        switch (pouch->equippedBadges[j]) {
            case ITEM_QUICK_CHANGE:
                wp->badgeEquippedFlags |= 1 << 0;
                break;
            case ITEM_PEEKABOO:
                wp->badgeEquippedFlags |= 1 << 1;
                break;
            case ITEM_TIMING_TUTOR:
                wp->badgeEquippedFlags |= 1 << 2;
                break;
            case ITEM_HEART_FINDER:
                wp->badgeEquippedFlags |= 1 << 3;
                break;
            case ITEM_FLOWER_FINDER:
                wp->badgeEquippedFlags |= 1 << 4;
                break;
            case ITEM_MONEY_MONEY:
                wp->badgeEquippedFlags |= 1 << 7;
                break;
            case ITEM_ATTACK_FX_R:
                wp->badgeEquippedFlags |= 1 << 8;
                break;
            case ITEM_ATTACK_FX_B:
                wp->badgeEquippedFlags |= 1 << 10;
                break;
            case ITEM_ATTACK_FX_G:
                wp->badgeEquippedFlags |= 1 << 11;
                break;
            case ITEM_ATTACK_FX_Y:
                wp->badgeEquippedFlags |= 1 << 9;
                break;
            case ITEM_ATTACK_FX_P:
                wp->badgeEquippedFlags |= 1 << 12;
                break;
            default:
                break;
        }
    }

    return TRUE;
}

void BattleEnd(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    int i;
    void* field_55C;

    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(wp, i);
        if (unit) {
            BtlUnit_Delete(unit);
        }
    }
    BattleIconEnd();
    battleMenuDispEnd();
    BattleStageEnd();
    BattleAudience_End();
    BattleBreakSlot_End();
    field_55C = wp->commandMenu.field_55C;
    if (field_55C) {
        BattleFree(field_55C);
    }
    _mapFree(wp);
    _battleWorkPointer = NULL;
}

BOOL BattleMain(void) {
    BattleWork* wp = _battleWorkPointer;

    BattleCheckUnitBroken(wp);
    BattlePadManager();
    BattleSequenceManager();
    BattleActionCommandManager(wp);
    BattleStageObjectMain();
    BattleIconMain();
    BattleStageMain();
    BattleAcHelpMain();
    BattleAudience_Main();
    BattleBreakSlot_Main();
    btlDispMain();

    if (BattleGetSeq(wp, SEQ_UNKNOWN) == 4) {
        return FALSE;
    }

    if ((gp->flags & 0x1000) && (gp->flags & 0x2000) && BattleGetSeq(wp, SEQ_UNKNOWN) == 2) {
        wp->flags |= 0x60;
        BattleSetSeq(wp, SEQ_UNKNOWN, 3);
        BattleSetSeq(wp, SEQ_END, 0x7000000);
        return TRUE;
    }

    return TRUE;
}

void BattleInit(FieldBattleInfo* info) {
    BattleWork* wp;

    wp = _battleWorkPointer = (BattleWork*)_mapAlloc(sizeof(BattleWork));
    memset(_battleWorkPointer, 0, sizeof(BattleWork));
    wp->field_19060 = -1;
    wp->field_424 = -1;
    wp->fbatInfo = info;
    BattlePadInit();
    BattleAcHelpInit();
    BattleIconInit();
    BattleActionCommandManagerInit(wp);
    BattleAfterReactionQueueInit();
    BattleStatusChangeMsgWorkInit();
    btlDispInit();
    BattleSetSeq(wp, SEQ_UNKNOWN, 0);
    BattleSetSeq(wp, SEQ_INIT, 0x1000000);
    wp->seqEndWork = NULL;
    wp->commandMenu.field_4 = 0;
    wp->commandMenu.cursor[0].field_0 = 0;
    wp->commandMenu.cursor[0].field_4 = 0;
    wp->commandMenu.cursor[1].field_0 = 0;
    wp->commandMenu.cursor[1].field_4 = 0;
    wp->commandMenu.cursor[2].field_0 = 0;
    wp->commandMenu.cursor[2].field_4 = 0;
    wp->commandMenu.cursor[3].field_0 = 0;
    wp->commandMenu.cursor[3].field_4 = 0;
    wp->commandMenu.cursor[4].field_0 = 0;
    wp->commandMenu.cursor[4].field_4 = 0;
    wp->commandMenu.cursor[5].field_0 = 0;
    wp->commandMenu.cursor[5].field_4 = 0;
    wp->commandMenu.cursor[6].field_0 = 0;
    wp->commandMenu.cursor[6].field_4 = 0;
    wp->commandMenu.cursor[7].field_0 = 0;
    wp->commandMenu.cursor[7].field_4 = 0;
    wp->commandMenu.cursor[8].field_0 = 0;
    wp->commandMenu.cursor[8].field_4 = 0;
    wp->commandMenu.cursor[9].field_0 = 0;
    wp->commandMenu.cursor[9].field_4 = 0;
    wp->commandMenu.field_55C = 0;
    wp->turnCount = 0;
    memset(wp->reserveItems, 0, sizeof(wp->reserveItems));
    wp->flags = 0;
    BtlUnit_Init();
    BattlePartyInfoWorkInit(wp);
    BattleStageInit();
    BattleAudience_Init();
    BattleBreakSlot_Init();
}

BOOL battleSeqEndCheck() {
    return _battleWorkPointer->flags >> 31;
}

void battleMain(void) {
    BattleWork* wp = _battleWorkPointer;

    if (wp && BattleMain() == 0) {
        wp->flags |= 0x80000000;
    }
}
