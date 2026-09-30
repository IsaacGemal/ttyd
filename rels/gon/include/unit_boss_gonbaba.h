#pragma once

#include "evt/evt_cmd.h"
#include "battle/battle_weapon_power.h"

u32 weaponGetPower_Gonbaba(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
USER_FUNC(_btlgon_get_escape_param);
USER_FUNC(_set_gonbaba_light);
USER_FUNC(_wait_damage_event_end);
EVT_DEF(negotiation_event);
EVT_DEF(gon_extra_eaten_event);
EVT_DEF(gonbaba_damage_sub_event);
EVT_DEF(firebreath_attack_event);
