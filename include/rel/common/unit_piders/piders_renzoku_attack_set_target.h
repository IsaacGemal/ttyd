// piders_renzoku_attack_set_target, shared by the Pider units (unit_piders, all three versions) and unit_churantalar.
// Included where it sits in the unit's function order (it sets the .text layout). The unit defines piders_getMatColor,
// which reads its own colour template, before piders_draw_yarn_sub.
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "evt/evt_cmd.h"
#include "memory.h"
#include "rel/common/unit_piders.h"
#include "system.h"

static USER_FUNC(piders_renzoku_attack_set_target) {
    EventEntry* evt = event;
    s32* args = evt->args;
    s32 ids[20];
    s32 parts[20];
    s32 i = 0;
    s32 count = 0;

    ((s8*)_battleWorkPointer)[0xEDF] = 0;
    piders_get_target(&ids[0], &parts[0]);
    while (1) {
        if (ids[i] == -1) {
            count = i;
            break;
        }
        i++;
        piders_get_next_target(&ids[i], &parts[i]);
    }
    for (i = 0; i < 3; i++) {
        s32 idx = irand(count);

        evtSetValue(evt, *(args++), ids[idx]);
        evtSetValue(evt, *(args++), parts[idx]);
    }
    return EVT_RETURN_DONE;
}
