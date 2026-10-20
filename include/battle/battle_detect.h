#pragma once

#include "battle/battle_database_common.h"

typedef struct BattleSamplingEntry {
    s16 unitId;      // 0x0
    s16 partId;      // 0x2
    s16 x;           // 0x4
    s16 y;           // 0x6
    s16 z;           // 0x8
    u8 pad_0xA[2];   // 0xA
    s32 field_0xC;   // 0xC
    s32 field_0x10;  // 0x10
    s32 field_0x14;  // 0x14
    s32 field_0x18;  // 0x18
    u8 field_0x1C;   // 0x1C
    s8 field_0x1D;   // 0x1D
    u8 pad_0x1E[2];  // 0x1E
    s32 field_0x20;  // 0x20
} BattleSamplingEntry;

typedef struct BattleSampling {
    BattleWeapon* weapon;          // 0x0
    BattleSamplingEntry entries[74]; // 0x4
    s8 count;                      // 0xA6C
    s8 order[0xAB7 - 0xA6D];       // 0xA6D  (retail: lbz + extsb, so s8)
    s8 selectIdx;                  // 0xAB7
    s32 unitId;                    // 0xAB8
    s8 belong;                     // 0xABC
    u8 pad_0xABD[0xAC0 - 0xABD];  // 0xABD
    u32 field_0xAC0;               // 0xAC0
    u32 field_0xAC4;               // 0xAC4
    s8 attackDirection;            // 0xAC8
} BattleSampling;

// The original stores `belong` and `attackDirection` with stb: both are bytes.
s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4,
                        s32 arg5, s8 attackDirection);
typedef struct BattleUnit BattleUnit;
typedef struct BattleWork BattleWork;
void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** outUnit1, BattleUnit** outUnit2);
void BattleChoiceSamplingEnemy(u8* sampling, u32 flags, s32* outId, s32* outPartId);
void BattleSamplingEnemyUpdate(u8* sampling);
void _btlSamplingEnemy(u8* sampling);
