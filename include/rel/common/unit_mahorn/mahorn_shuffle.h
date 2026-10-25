// mahorn_shuffle and mahorn_copy_status, shared by the Wizzerd units (unit_mahorn, unit_mahorn_custom,
// unit_super_mahorn). Included where they sit in the unit's function order (it sets the .text layout).
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "system.h"

static USER_FUNC(mahorn_shuffle) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* units[6];
    s32 count = 0;
    s32 i;

    for (i = 0; i < 64; i++) {
        units[count] = work->units[i];
        if (units[count] != NULL && (units[count]->currentKind == 0x80 ||
                                     units[count]->currentKind == 0x81 ||
                                     units[count]->currentKind == 0xA9 ||
                                     units[count]->currentKind == 0xAA)) {
            count++;
        }
    }
    for (i = 0; i < 10; i++) {
        s32 a = irand(count);
        s32 b = irand(count);
        BattleUnit* unitA = units[a];
        BattleUnit* unitB = units[b];
        f32 tmp;

        tmp = unitA->mPosition.x;
        unitA->mPosition.x = unitB->mPosition.x;
        unitB->mPosition.x = tmp;
        tmp = unitA->mHomePosition.x;
        unitA->mHomePosition.x = unitB->mHomePosition.x;
        unitB->mHomePosition.x = tmp;
        tmp = unitA->mPosition.z;
        unitA->mPosition.z = unitB->mPosition.z;
        unitB->mPosition.z = tmp;
        tmp = unitA->mHomePosition.z;
        unitA->mHomePosition.z = unitB->mHomePosition.z;
        unitB->mHomePosition.z = tmp;
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(mahorn_copy_status) {
    s32* args = event->args;
    BattleUnit* unit1;
    BattleUnit* unit2;
    s32 id1;
    s32 id2;
    BattleWork* wp;
    f32 ratio;

    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    wp = _battleWorkPointer;
    unit1 = BattleGetUnitPtr(wp, id1);
    unit2 = BattleGetUnitPtr(wp, id2);
    unit2->currentHp = unit1->currentHp;
    memcpy(&unit2->mSleepTurns, &unit1->mSleepTurns, 0x1E);
    unit2->hpGauge.fromHp = unit2->currentHp;
    unit2->hpGauge.toHp = unit2->currentHp;
    ratio = (f32)unit2->currentHp / (f32)unit2->maxHp;
    unit2->hpGauge.drainRatio = ratio;
    unit2->hpGauge.fillRatio = ratio;
    return EVT_RETURN_DONE;
}
