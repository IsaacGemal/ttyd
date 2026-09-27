#pragma once

#include "battle/battle_database_common.h"

// The original stores `belong` and `attackDirection` with stb: both are bytes.
s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4,
                        s32 arg5, s8 attackDirection);
typedef struct BattleUnit BattleUnit;
typedef struct BattleWork BattleWork;
void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** outUnit1, BattleUnit** outUnit2);
void BattleChoiceSamplingEnemy(u8* sampling, u32 flags, s32* outId, s32* outPartId);
