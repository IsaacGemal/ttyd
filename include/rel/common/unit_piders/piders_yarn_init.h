// piders_yarn_init, shared by the Pider units (unit_piders, all three versions) and unit_churantalar. Included where it
// sits in the unit's function order (it sets the .text layout). The unit defines piders_getMatColor, which reads its
// own colour template, before piders_draw_yarn_sub.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "rel/common/unit_piders.h"
#include "system.h"

static USER_FUNC(piders_yarn_init) {
    s32 id = BattleTransID(event, -2);
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, id);

    unit->mExtraWork = __memAlloc(HEAP_BATTLE, 0x1c);
    return EVT_RETURN_DONE;
}
