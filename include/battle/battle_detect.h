#pragma once

#include "battle/battle_database_common.h"

// The original stores `belong` and `attackDirection` with stb: both are bytes.
s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4,
                        s32 arg5, s8 attackDirection);
