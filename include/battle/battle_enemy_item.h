#pragma once

#include "battle/battle_unit.h"

// Returns the attack event script of the item the enemy should use, or NULL.
void* BattleEnemyUseItemCheck(BattleUnit* unit);
void* _check_status_recover_item(BattleUnit* unit);
void* _check_status_support_item(BattleUnit* unit);
void* _check_status_attack_item(BattleUnit* unit);
void* _check_attack_item(BattleUnit* unit);
void* _check_fp_recover_item(BattleUnit* unit);
void* _check_hp_recover_item(BattleUnit* unit);
