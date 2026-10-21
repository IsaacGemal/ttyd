// piders_get_target and piders_get_next_target, shared by the Pider units (unit_piders, all three versions) and
// unit_churantalar. Included where they sit in the unit's function order (it sets the .text layout). The unit defines
// piders_getMatColor, which reads its own colour template, before piders_draw_yarn_sub.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "rel/common/unit_piders.h"
#include "system.h"

static void piders_get_target(s32* outUnit, s32* outPart) {
    BattleWork* wp = _battleWorkPointer;
    BattleSampling* sampling = &wp->sampling;
    s32 idx = sampling->selectIdx;
    BattleSamplingEntry* entry = &sampling->entries[sampling->order[idx]];

    *outUnit = entry->unitId;
    *outPart = entry->partId;
}

static void piders_get_next_target(s32* outUnit, s32* outPart) {
    BattleWork* wp = _battleWorkPointer;
    BattleSampling* sampling = &wp->sampling;
    BattleSamplingEntry* entry;

    sampling->selectIdx++;
    if (sampling->selectIdx < sampling->count) {
        entry = &sampling->entries[sampling->order[sampling->selectIdx]];
        *outUnit = entry->unitId;
        *outPart = entry->partId;
    } else {
        *outUnit = -1;
        *outPart = 0;
    }
}
