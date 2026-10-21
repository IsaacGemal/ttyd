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
    u8 mAcDifficulty;                // 0x2D
    u8 field_0x2E[2];                // 0x2E
    s32 field_0x30;                  // 0x30
    s32 acFlag;                      // 0x34 (BattleWork 0x1CC4), set by btlevtcmd_AcSetFlag, bit 0 read by ac_power_gauge
    s32 acParams[8];                 // 0x38 (BattleWork 0x1CC8), set by btlevtcmd_AcSetParam(All)/AcGet
    s32 outputParams[8];             // 0x58
    s32 acGaugeParams[4];            // 0x78 (BattleWork 0x1D08), set by btlevtcmd_AcSetGaugeParam
    s16 guardFrames;                 // 0x88 (BattleWork 0x1D18), counted down by ac_button_down/ac_stick_rotate
    u8 field_0x8A[0x8C - 0x8A];      // 0x8A
    BattleWorkPad pad;               // 0x8C
    s32 field_0x288;                 // 0x288
    u8 field_0x28C[0x290 - 0x28C];   // 0x28C
    BattleACDispParams dispParams;   // 0x290 (BattleWork+0x1F20), used by ac_button_down and ac_air_gauge
    u8 field_0x2BC[0x30C - 0x2BC];   // 0x2BC
} BattleActions;
// NB: 0x308 is as far as I can pin to this struct (0x1F98 in battleAcMain_PendulumCraneTiming, TODO?)

// TimingA's action-command work area in BattleActions.field_0x2BC (BattleWork+0x1F4C); battle_event_cmd's
// btlevtcmd_ac_timing_flag_onoff sets and clears its flags.
typedef struct AcTimingADecWork {
    s32 pad_0x00;   // 0x0, never accessed
    u32 flags;      // 0x4
    s32 endTimer;   // 0x8, frames the result display waits (0x3C)
} AcTimingADecWork;

void BattleActionCommandManagerInit(BattleWork* wp);
void BattleActionCommandManager(BattleWork* wp);
s32 BattleActionCommandResult(BattleWork* work);
void BattleActionCommandDeclareACResult(BattleWork* work, BattleWeapon* weapon, s32 result);

void BattleActionCommandStart(BattleWork* work);
void BattleActionCommandStop(BattleWork* work);
s32 BattleActionCommandGetDefenceResult(void);
void BattleActionCommandResetDefenceResult(void);
s32 BattleActionCommandGetDifficulty(BattleWork* work);
u32 BattleACPadCheckRecordTrigger(s32 chan, s32 mask);
void BattleAcDrawGauge(s32 x, s32 y, f32 ratio, s32 type, s32 count, s32 t0, s32 t1, s32 t2, s32 flags);
void BattleAcGaugeSeInit(void);
void BattleAcGaugeSeUpdate(f32);
void BattleAcGaugeSeDelete(void);
void BattleActionCommandSetup(BattleWork* wp, s32 acId, BattleUnit* unit, s32 gauge, s32 diff);
void BattleActionCommandSetDifficulty(BattleWork* work, BattleUnit* unit, s32 difficulty);
s32 BattleActionCommandCheckDefence(BattleUnit* unit, BattleWeapon* weapon);
s32 BattleActionCommandGetPrizeLv(BattleWork* wp, BattleUnit* unit, s32 index);
s32 BattleACGetButtonIcon(s32 button, s32 status);
