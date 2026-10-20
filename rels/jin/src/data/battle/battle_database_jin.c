#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern ItemDropData battle_item_atmic_teresa[1];
extern ItemDropData battle_item_basabasa[10];
extern ItemDropData battle_item_faker_mario[1];
extern ItemDropData battle_item_met[10];
extern ItemDropData battle_item_rampell[1];
extern ItemDropData battle_item_teresa[10];
extern ItemDropData battle_item_togemet[10];
extern BattleUnitKind unit_atmic_teresa;
extern BattleUnitKind unit_basabasa_14_data_1DE40;
extern BattleUnitKind unit_boss_rampell;
extern BattleUnitKind unit_faker_mario;
extern BattleUnitKind unit_met;
extern BattleUnitKind unit_teresa_14_data_1EF28;
extern BattleUnitKind unit_togemet;

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData jin_btlobject_data[18] = {
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "if_wire", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
    { "if_body", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "ranpu", 2, 0, { -85.0f, 0.0f, -36.0f }, 0, 60, { 0, 0 } },
    { "tukue", 2, 0, { -27.0f, 0.0f, -46.0f }, 20, 40, { 0, 0 } },
    { "reizouko", 2, 0, { -124.0f, 0.0f, -60.0f }, 20, 40, { 0, 0 } },
    { "furo", 2, 0, { 92.0f, 0.0f, -43.0f }, 30, 30, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 90, { 0, 0 } },
};

BattleStageData jin_btlstage_data[6] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_3",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &jin_btlobject_data[0],                     // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            NULL,                                       // hpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_3",                                 // mSpecificStageDataDir
        4,                                          // mNumObjects
        &jin_btlobject_data[2],                     // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            NULL,                                       // hpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_2",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &jin_btlobject_data[6],                     // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            NULL,                                       // hpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_4",                                 // mSpecificStageDataDir
        6,                                          // mNumObjects
        &jin_btlobject_data[8],                     // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_5",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &jin_btlobject_data[14],                    // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            NULL,                                       // hpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_6",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &jin_btlobject_data[16],                    // mObjects
        {  // bgsetAttackA1
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            NULL,                                       // hpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        {  // bgsetAttackB
            NULL,                                       // name
            0,                                          // icon
            ITEM_NULL,                                  // item
            NULL,                                       // description
            100,                                        // baseAccuracy
            0,                                          // baseFpCost
            0,                                          // baseSpCost
            2,                                          // superguardType
            0.0f,                                       // field_0x14
            0,                                          // mStylishCommandMultiplier
            0,                                          // field_0x19
            0,                                          // mBingoSlotChance
            0,                                          // field_0x1B
            weaponGetPowerDefault,                      // hpDamageFn
            { 1, 0, 0, 0, 0, 0, 0, 0 },                 // hpDamageWork
            NULL,                                       // fpDamageFn
            { 0, 0, 0, 0, 0, 0, 0, 0 },                 // fpDamageWork
            0x02100060,                                 // targetClassFlags
            0x20000000,                                 // targetPropertyFlags
            0,                                          // element
            26,                                         // field_0x6D
            3,                                          // actionLevel
            2,                                          // field_0x6F
            NULL,                                       // acHelpMsg
            0,                                          // special
            0x000007FF,                                 // counterResistanceFlags
            0,                                          // targetWeightingFlags
            0,                                          // sleepChance
            0,                                          // sleepTime
            0,                                          // stopChance
            0,                                          // stopTime
            0,                                          // dizzyChance
            0,                                          // dizzyTime
            0,                                          // poisonChance
            0,                                          // poisonTime
            0,                                          // poisonStrength
            0,                                          // confuseChance
            0,                                          // confuseTime
            0,                                          // electricChance
            0,                                          // electricTime
            0,                                          // dodgyChance
            0,                                          // dodgyTime
            0,                                          // burnChance
            0,                                          // burnTime
            0,                                          // freezeChance
            0,                                          // freezeTime
            0,                                          // sizeChangeChance
            0,                                          // sizeChangeTime
            0,                                          // sizeChangeStrength
            0,                                          // atkChangeChance
            0,                                          // atkChangeTime
            0,                                          // atkChangeStrength
            0,                                          // defChangeChance
            0,                                          // defChangeTime
            0,                                          // defChangeStrength
            0,                                          // allergicChance
            0,                                          // allergicTime
            0,                                          // ohkoChance
            0,                                          // chargeStrength
            0,                                          // fastChance
            0,                                          // fastTime
            0,                                          // slowChance
            0,                                          // slowTime
            0,                                          // frightChance
            0,                                          // galeForceChance
            0,                                          // paybackTime
            0,                                          // holdFastTime
            0,                                          // invisibleChance
            0,                                          // invisibleTime
            0,                                          // hpRegenTime
            0,                                          // hpRegenStrength
            0,                                          // fpRegenTime
            0,                                          // fpRegenStrength
            NULL,                                       // mAttackEvtScript
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // field_B4
        },
        NULL,                                       // field_0x190
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // field_0x1A0
        0,                                          // field_0x1B0
    },
};

BattleUnitSetup btlparty_jin_jin_04_ramper[1] = {
    { &unit_boss_rampell, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_rampell },
};

BattleGroupSetup btlgrp_jin_jin_04_ramper = { 1, btlparty_jin_jin_04_ramper, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_faker_mario[1] = {
    { &unit_faker_mario, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_faker_mario },
};

BattleGroupSetup btlgrp_jin_jin_01_faker_mario = { 1, btlparty_jin_jin_01_faker_mario, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_00_atmic_teresa[1] = {
    { &unit_atmic_teresa, 1, 0x04000004, { 70.0f, 20.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_atmic_teresa },
};

BattleGroupSetup btlgrp_jin_jin_00_atmic_teresa = { 1, btlparty_jin_jin_00_atmic_teresa, 0, 10, 0, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_00_01_off_1[1] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_00_01_off_1 = { 1, btlparty_jin_jin_00_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_00_01_off_2[2] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_00_01_off_2 = { 2, btlparty_jin_jin_00_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_00_01_off_3[3] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_00_01_off_3 = { 3, btlparty_jin_jin_00_01_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_00_01_off_4[4] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_00_01_off_4 = { 4, btlparty_jin_jin_00_01_off_4, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_01_01_off_1[2] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_01_01_off_1 = { 2, btlparty_jin_jin_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_01_off_2[3] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_01_01_off_2 = { 3, btlparty_jin_jin_01_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_01_off_3[3] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_01_01_off_3 = { 3, btlparty_jin_jin_01_01_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_03_off_1[2] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_01_03_off_1 = { 2, btlparty_jin_jin_01_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_03_off_2[4] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 150.0f, 130.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_01_03_off_2 = { 4, btlparty_jin_jin_01_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_01_03_off_3[2] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_01_03_off_3 = { 2, btlparty_jin_jin_01_03_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_03_01_off_1[2] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_03_01_off_1 = { 2, btlparty_jin_jin_03_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_03_02_off_1[2] = {
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_14_data_1DE40, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_jin_jin_03_02_off_1 = { 2, btlparty_jin_jin_03_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_07_01_off_1[2] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_07_01_off_1 = { 2, btlparty_jin_jin_07_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_07_01_off_2[3] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_07_01_off_2 = { 3, btlparty_jin_jin_07_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_07_02_off_1[2] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_07_02_off_1 = { 2, btlparty_jin_jin_07_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_07_02_off_2[3] = {
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_14_data_1EF28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jin_jin_07_02_off_2 = { 3, btlparty_jin_jin_07_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jin_jin_09_01_off_1[1] = {
    { &unit_met, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
};

BattleGroupSetup btlgrp_jin_jin_09_01_off_1 = { 1, btlparty_jin_jin_09_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_01_off_1[2] = {
    { &unit_met, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_met, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
};

BattleGroupSetup btlgrp_jin_jin_10_01_off_1 = { 2, btlparty_jin_jin_10_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_02_off_1[1] = {
    { &unit_togemet, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togemet },
};

BattleGroupSetup btlgrp_jin_jin_10_02_off_1 = { 1, btlparty_jin_jin_10_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_03_off_1[1] = {
    { &unit_met, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
};

BattleGroupSetup btlgrp_jin_jin_10_03_off_1 = { 1, btlparty_jin_jin_10_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_04_off_1[4] = {
    { &unit_met, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_met, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_met, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_met, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
};

BattleGroupSetup btlgrp_jin_jin_10_04_off_1 = { 4, btlparty_jin_jin_10_04_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_05_off_1[2] = {
    { &unit_togemet, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togemet },
    { &unit_togemet, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togemet },
};

BattleGroupSetup btlgrp_jin_jin_10_05_off_1 = { 2, btlparty_jin_jin_10_05_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jin_jin_10_06_off_1[3] = {
    { &unit_met, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_met, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_met },
    { &unit_togemet, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togemet },
};

BattleGroupSetup btlgrp_jin_jin_10_06_off_1 = { 3, btlparty_jin_jin_10_06_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_test1_off[2] = {
    { 50, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_test2_off[2] = {
    { 50, &btlgrp_jin_jin_00_01_off_1, jin_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_test3_off[2] = {
    { 50, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_test4_off[2] = {
    { 50, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[4] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_test5_off[2] = {
    { 50, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_01_01_off[2] = {
    { 50, &btlgrp_jin_jin_04_ramper, &jin_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_01_02_off[2] = {
    { 50, &btlgrp_jin_jin_01_faker_mario, &jin_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_01_off[2] = {
    { 50, &btlgrp_jin_jin_00_atmic_teresa, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_02_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_03_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_04_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_05_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_06_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_00_09_off[5] = {
    { 10, &btlgrp_jin_jin_00_01_off_1, &jin_btlstage_data[2] },
    { 20, &btlgrp_jin_jin_00_01_off_2, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_3, &jin_btlstage_data[2] },
    { 10, &btlgrp_jin_jin_00_01_off_4, &jin_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_01_03_off[4] = {
    { 20, &btlgrp_jin_jin_01_01_off_1, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_01_off_2, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_01_off_3, &jin_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_01_05_off[4] = {
    { 10, &btlgrp_jin_jin_01_03_off_1, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_03_off_2, &jin_btlstage_data[1] },
    { 20, &btlgrp_jin_jin_01_03_off_3, &jin_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_02_01_off[4] = {
    { 50, &btlgrp_jin_jin_01_03_off_1, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_03_off_2, &jin_btlstage_data[1] },
    { 20, &btlgrp_jin_jin_01_03_off_3, &jin_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_02_03_off[4] = {
    { 50, &btlgrp_jin_jin_01_01_off_1, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_01_off_2, &jin_btlstage_data[1] },
    { 10, &btlgrp_jin_jin_01_01_off_3, &jin_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_09_01_off[2] = {
    { 50, &btlgrp_jin_jin_09_01_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_01_off[2] = {
    { 50, &btlgrp_jin_jin_10_01_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_02_off[2] = {
    { 50, &btlgrp_jin_jin_10_02_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_03_off[2] = {
    { 50, &btlgrp_jin_jin_10_03_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_04_off[2] = {
    { 50, &btlgrp_jin_jin_10_04_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_05_off[2] = {
    { 50, &btlgrp_jin_jin_10_05_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jin_btlno_jin_10_06_off[2] = {
    { 50, &btlgrp_jin_jin_10_06_off_1, &jin_btlstage_data[5] },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_jin_tbl[26] = {
    {
        "マップstg_04_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_04_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_04_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_04_5",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_04_6",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_test5_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_04ランペルその１",                                                                                    // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_01_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG4_RUNPELL1",                                                                           // music
    },
    {
        "JIN_04ランペルその２",                                                                                    // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_01_02_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG4_FAKEMARIO1",                                                                         // music
    },
    {
        "JIN_00アトミックテレサ",                                                                                     // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_01_off,                                                                    // offLoadouts
        536870928,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 0, 0, 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                                // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_05_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_06_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_00テレサ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jin_btlno_jin_00_09_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "JIN_01バサバサ",                                                                                       // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_01_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_01バサバサ",                                                                                       // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_01_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_02バサバサ",                                                                                       // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_02_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_02バサバサ",                                                                                       // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_02_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_09メット",                                                                                        // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_09_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10メット",                                                                                        // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10トゲメット",                                                                                      // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10メット",                                                                                        // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10メット",                                                                                        // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10トゲメット",                                                                                      // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "JIN_10メット",                                                                                        // name
        "jin_04",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jin_btlno_jin_10_06_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        NULL,                                                                                               // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        0,                                                                                                  // flag
        NULL,                                                                                               // onLoadouts
        NULL,                                                                                               // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_0x20
        NULL,                                                                                               // music
    },
};

// TODO: type
u32 btl_jin_setup_no_tbl[52] = {
    (u32)"BTLNO_JIN_TEST1", 0x00000000, (u32)"BTLNO_JIN_TEST2", 0x00000001,
    (u32)"BTLNO_JIN_TEST3", 0x00000002, (u32)"BTLNO_JIN_TEST4", 0x00000003,
    (u32)"BTLNO_JIN_TEST5", 0x00000004, (u32)"BTLNO_JIN_01_01", 0x00000005,
    (u32)"BTLNO_JIN_01_02", 0x00000006, (u32)"BTLNO_JIN_00_01", 0x00000007,
    (u32)"BTLNO_JIN_00_02", 0x00000008, (u32)"BTLNO_JIN_00_03", 0x00000009,
    (u32)"BTLNO_JIN_00_04", 0x0000000A, (u32)"BTLNO_JIN_00_05", 0x0000000B,
    (u32)"BTLNO_JIN_00_06", 0x0000000C, (u32)"BTLNO_JIN_00_09", 0x0000000D,
    (u32)"BTLNO_JIN_01_03", 0x0000000E, (u32)"BTLNO_JIN_01_05", 0x0000000F,
    (u32)"BTLNO_JIN_02_01", 0x00000010, (u32)"BTLNO_JIN_02_03", 0x00000011,
    (u32)"BTLNO_JIN_09_01", 0x00000012, (u32)"BTLNO_JIN_10_01", 0x00000013,
    (u32)"BTLNO_JIN_10_02", 0x00000014, (u32)"BTLNO_JIN_10_03", 0x00000015,
    (u32)"BTLNO_JIN_10_04", 0x00000016, (u32)"BTLNO_JIN_10_05", 0x00000017,
    (u32)"BTLNO_JIN_10_06", 0x00000018, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
