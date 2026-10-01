#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
USER_FUNC(_clauda_make_extra_work_area);
USER_FUNC(_check_blow_rate);
USER_FUNC(_make_breath_weapon);
USER_FUNC(_make_kiss_weapon);
USER_FUNC(_get_clauda_kiss_hit_position);
USER_FUNC(_make_kumoguard_weapon);
extern u32 partyWeapon_ClaudaBreathAttack[48];
extern u32 partyWeapon_ClaudaKumogakureAttack[48];
extern u32 partyWeapon_ClaudaNormalAttack[48];
extern u32 partyWeapon_ClaudaPredationAttack[48];
