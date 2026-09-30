#pragma once

#include "battle/battle_database_common.h"

// The original stores `belong` and `attackDirection` with stb: both are bytes.
s32 BattleSamplingEnemy(u8* sampling, BattleWeapon* weapon, s32 unitId, s8 belong, s32 arg4,
                        s32 arg5, s8 attackDirection);
typedef struct BattleUnit BattleUnit;
typedef struct BattleWork BattleWork;
void BattleGetFirstAttackUnit(BattleWork* wp, BattleUnit** outUnit1, BattleUnit** outUnit2);
void BattleChoiceSamplingEnemy(u8* sampling, u32 flags, s32* outId, s32* outPartId);
void BattleSamplingEnemyUpdate(u8* sampling);
void _btlSamplingEnemy(u8* sampling);

// A sampling entry; the array starts at sampling+4 and each entry is 0x24 bytes.
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
    u8 field_0x1D;   // 0x1D
    u8 pad_0x1E[2];  // 0x1E
    s32 field_0x20;  // 0x20
} BattleSamplingEntry;

// View with the weapon pointer prepended, so unitId is at 0x4 and the base stays `sampling + n*0x24`.
typedef struct BattleSamplingEntryBase {
    BattleWeapon* weapon;   // 0x0
    BattleSamplingEntry e;  // 0x4
} BattleSamplingEntryBase;
