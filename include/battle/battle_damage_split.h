#pragma once

// BattleSetStatusDamageFromWeapon is defined in src/battle/battle_damage.c, but it cannot be declared in
// include/battle/battle_damage.h: battle.c aliases that function symbol as a byte array for its split-owned
// switch tables, so a function prototype there conflicts. Declare it here for callers outside battle_damage.c.
s32 BattleSetStatusDamageFromWeapon(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32 flags);
