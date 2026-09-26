#pragma once

#include "battle/battle_unit.h"

// Returns the attack event script of the item the enemy should use, or NULL.
void* BattleEnemyUseItemCheck(BattleUnit* unit);
