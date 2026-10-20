#pragma once

#include "battle/battle_database_common.h"

typedef struct BattleSamplingEntry {
    s16 unitId;      // 0x0
    s16 partId;      // 0x2
    s16 x;           // 0x4, hit-cursor position (part world position plus the cursor offset)
    s16 y;           // 0x6
    s16 z;           // 0x8
    u8 pad_0xA[2];   // 0xA
    s32 baseX;       // 0xC, part world x (no cursor offset) plus xBias; BtlCompForwardLv input, dedup distance
    s32 baseY;       // 0x10, part world y
    s32 baseZ;       // 0x14, part world z plus 5
    s32 xBias;       // 0x18, attack-direction and part offset; weighted x10 in the sort
    u8 forwardLv;    // 0x1C, BtlCompForwardLv(attackDirection, baseX)
    s8 zSide;        // 0x1D, 0 when baseZ < -30, else 1
    u8 pad_0x1E[2];  // 0x1E
    s32 field_0x20;  // 0x20, set to -1 when the entry is built
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
    u32 targetClassFlags;          // 0xAC0, BattleWeapon::targetClassFlags
    u32 targetPropertyFlags;       // 0xAC4, BattleWeapon::targetPropertyFlags
    s8 attackDirection;            // 0xAC8
} BattleSampling;

// The original stores `belong` and `attackDirection` with stb: both are bytes.
s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong,
                        s32 targetClassFlags, s32 targetPropertyFlags, s8 attackDirection);
typedef struct BattleUnit BattleUnit;
typedef struct BattleWork BattleWork;
void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** outUnit1, BattleUnit** outUnit2);
void BattleChoiceSamplingEnemy(u8* sampling, u32 flags, s32* outId, s32* outPartId);
void BattleSamplingEnemyUpdate(u8* sampling);
void _btlSamplingEnemy(u8* sampling);
