#pragma once

typedef struct BattleUnit BattleUnit;

#include <dolphin/types.h>
#include "battle/battle_database_common.h"
#include "battle/battle_pad.h"
#include "driver/dispdrv.h"

typedef struct BattleWork BattleWork;

typedef s32 (*BattleACMainCallback)(BattleWork* work);
typedef s32 (*BattleACResultCallback)(BattleWork* work);
typedef void (*BattleACDeleteCallback)(BattleWork* work);

typedef struct BattleACDispParams {
    u8 field_0x0[0xC - 0x0];    // 0x0
    s32 field_0xC;              // 0xC
    u8 field_0x10[0x14 - 0x10]; // 0x10
    f32 iconX;                  // 0x14, see actionCommandDisp
    f32 iconY;                  // 0x18, see actionCommandDisp
    u8 field_0x1C[0x20 - 0x1C]; // 0x1C
    s32 counter;                // 0x20, counts down (ac_button_down) or up (ac_air_gauge)
    u8 field_0x24[0x28 - 0x24]; // 0x24
    f32 value;                  // 0x28, ac_air_gauge's gauge value
} BattleACDispParams;

typedef struct BattleActions {
    struct BattleUnit* mAcUnit;      // 0x0
    s32 field_0x4;                   // 0x4
    s32 field_0x8;                   // 0x8
    s32 mAcState;                    // 0xC
    BattleACMainCallback maincb;     // 0x10
    BattleACResultCallback resultcb; // 0x14
    DispCallback dispcb;             // 0x18
    BattleACDeleteCallback deletecb; // 0x1C
    s32 mDefenseResult;              // 0x20
    s32 mResultCount;                // 0x24
    s32 result;                      // 0x28
    s8 mBaseAcDifficulty;            // 0x2C
    s8 mAcDifficulty;                // 0x2D
    u8 field_0x2E[2];                // 0x2E
    s32 field_0x30;                  // 0x30
    u8 field_34[0x58 - 0x34];        // 0x34
    s32 outputParams[8];             // 0x58
    u8 field_78[0x8C - 0x78];        // 0x78
    BattleWorkPad pad;               // 0x8C
    s32 field_0x288;                 // 0x288
    u8 field_0x28C[0x290 - 0x28C];   // 0x28C
    BattleACDispParams dispParams;   // 0x290 (BattleWork+0x1F20), used by ac_button_down and ac_air_gauge
    u8 field_0x2BC[0x30C - 0x2BC];   // 0x2BC
} BattleActions;
// NB: 0x308 is as far as I can pin to this struct (0x1F98 in battleAcMain_PendulumCraneTiming, TODO?)

void BattleActionCommandManagerInit(BattleWork* wp);
void BattleActionCommandManager(BattleWork* wp);
s32 BattleActionCommandResult(BattleWork* work);
void BattleActionCommandDeclareACResult(BattleWork* work, BattleWeapon* weapon, s32 result);

void BattleActionCommandStart(BattleWork* work);
void BattleActionCommandStop(BattleWork* work);
s32 BattleActionCommandGetDefenceResult(void);
void BattleActionCommandResetDefenceResult(void);
s8 BattleActionCommandGetDifficulty(BattleWork* work);
u32 BattleACPadCheckRecordTrigger(s32 chan, s32 mask);
void BattleAcDrawGauge(s32, s32, s32, s32, s32, s32, s32, s32, f32);
void BattleAcGaugeSeInit(void);
void BattleAcGaugeSeUpdate(f32);
void BattleAcGaugeSeDelete(void);
void BattleActionCommandSetup(BattleWork* wp, s32 acId, BattleUnit* unit, s32 gauge, s32 diff);
void BattleActionCommandSetDifficulty(BattleWork* work, BattleUnit* unit, s32 difficulty);
s32 BattleActionCommandCheckDefence(BattleUnit* unit, int a2);
s32 BattleActionCommandGetPrizeLv(BattleWork* wp, BattleUnit* unit, s32 index);
