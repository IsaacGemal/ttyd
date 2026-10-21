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
    u8* sampling = (u8*)&wp->sampling;
    s32 idx = (s8)sampling[0xAB7];
    s16* entry = (s16*)(sampling + (s8)sampling[0xA6D + idx] * 0x24 + 4);

    *outUnit = entry[0];
    *outPart = entry[1];
}

static void piders_get_next_target(s32* outUnit, s32* outPart) {
    BattleWork* wp = _battleWorkPointer;
    u8* sampling = (u8*)&wp->sampling;
    s16* entry;

    sampling[0xAB7]++;
    if ((s8)sampling[0xAB7] < (s8)sampling[0xA6C]) {
        entry = (s16*)(sampling + (s8)sampling[0xA6D + (s8)sampling[0xAB7]] * 0x24 + 4);
        *outUnit = entry[0];
        *outPart = entry[1];
    } else {
        *outUnit = -1;
        *outPart = 0;
    }
}
