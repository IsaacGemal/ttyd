// piders_get_num, piders_get_zyun and piders_get_first_pos_attr, shared by the Pider units (unit_piders, all three
// versions) and unit_churantalar. Included where they sit in the unit's function order (it sets the .text layout). The
// unit defines piders_getMatColor, which reads its own colour template, before piders_draw_yarn_sub.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "rel/common/unit_piders.h"
#include "system.h"

static USER_FUNC(piders_get_num) {
    s32 i;
    s32* args = event->args;
    s32 count = 0;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;

    for (i = 0; i < 64; i++) {
        unit = wp->units[i];
        if (unit != NULL && (unit->currentKind == UNIT_PIDER || unit->currentKind == UNIT_ARANTULA) &&
            BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) == 0) {
            count++;
        }
    }
    evtSetValue(event, args[0], count);
    return EVT_RETURN_DONE;
}

static USER_FUNC(piders_get_zyun) {
    s32 id;
    s32 i;
    s32* args = event->args;
    s32 count = 0;
    BattleWork* wp;
    BattleUnit* unit;

    id = evtGetValue(event, args[0]);
    wp = _battleWorkPointer;
    id = BattleTransID(event, id);
    for (i = 0; i < 64; i++) {
        unit = wp->units[i];
        if (unit != NULL && (unit->currentKind == UNIT_PIDER || unit->currentKind == UNIT_ARANTULA)) {
            if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) == 0) {
                count++;
            }
            if (unit->unitId == id) {
                break;
            }
        }
    }
    evtSetValue(event, args[1], count);
    return EVT_RETURN_DONE;
}

static USER_FUNC(piders_get_first_pos_attr) {
    s32 i;
    s32 val;
    s32* args = event->args;
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;

    for (i = 0; i < 64; i++) {
        unit = wp->units[i];
        if (unit != NULL && (unit->currentKind == UNIT_PIDER || unit->currentKind == UNIT_ARANTULA)) {
            if (BtlUnit_CheckStatus(unit, STATUS_INSTANT_KILL) == 0) {
                break;
            }
        }
    }
    val = unit->work[0];
    if (BtlUnit_CanActStatus(unit)) {
        if (val == 0) {
            val = 1;
        } else {
            val = 0;
        }
    }
    evtSetValue(event, args[0], val);
    return EVT_RETURN_DONE;
}
