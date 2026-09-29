#pragma once

#include <dolphin/types.h>
#include "battle/battle_unit.h"
#include "battle/battle_database_common.h"
u32 weaponGetPowerFromMarioHammerLv(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
u32 weaponGetPowerDefault(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
u32 weaponGetPowerFromGulliblePartyAttackLv(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
u32 weaponGetPowerHPHalf2(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part);
