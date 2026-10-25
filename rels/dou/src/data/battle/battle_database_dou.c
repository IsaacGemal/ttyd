#include "battle_database_dou.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "forcedata.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern ItemDropData battle_item_bubble[8];
extern ItemDropData battle_item_heavy_bom[9];
extern ItemDropData battle_item_hermos[8];
extern ItemDropData battle_item_killer[1];
extern ItemDropData battle_item_killer_cannon[5];
extern ItemDropData battle_item_patamet[10];
extern BattleUnitKind unit_bubble_6_data_11C80;
extern BattleUnitKind unit_heavy_bom_6_data_15A08;
extern BattleUnitKind unit_hermos_6_data_17118;
extern BattleUnitKind unit_killer;
extern BattleUnitKind unit_killer_cannon;
extern BattleUnitKind unit_kuriboo_6_data_110E8;
extern BattleUnitKind unit_patamet_6_data_19100;
static USER_FUNC(_wave_offset);
s32 bgset_dou_wave_event[77];

// clang-format off
extend_data(52,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(53,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData dou_btlobject_data[51] = {
    { "taki", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_kabe", 2, 2, { 0.0f, 0.0f, 0.0f }, 10, 50, { 0, 0 } },
    { "zabu_1", 2, 2, { 0.0f, 0.0f, 0.0f }, 20, 40, { 0, 0 } },
    { "zabu_2", 2, 2, { 0.0f, 0.0f, 0.0f }, 30, 30, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_kabe", 2, 2, { 0.0f, 0.0f, 0.0f }, 10, 50, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "takara_01", 2, 1, { 111.0f, 0.0f, -69.0f }, 0, 60, { 0, 0 } },
    { "takara_02", 2, 1, { -87.0f, 0.0f, -72.0f }, 10, 50, { 0, 0 } },
    { "takara_03", 2, 1, { 136.0f, 0.0f, 57.0f }, 20, 40, { 0, 0 } },
    { "takara_04", 2, 1, { -182.0f, 0.0f, 57.0f }, 30, 30, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C_kabe", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_00", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_01", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "fune_L", 2, 0, { -104.0f, 0.0f, -64.0f }, 0, 60, { 0, 0 } },
    { "fune_R", 2, 0, { 120.0f, 0.0f, -64.0f }, 10, 50, { 0, 0 } },
    { "A1_kabe", 2, 0, { 0.0f, 0.0f, -51.0f }, 20, 40, { 0, 0 } },
    { "iwa_01", 2, 1, { 232.0f, 0.0f, -21.0f }, 0, 60, { 0, 0 } },
    { "iwa_02", 2, 1, { -234.0f, 0.0f, -21.0f }, 10, 50, { 0, 0 } },
    { "iwa_03", 2, 1, { 0.0f, 0.0f, -38.0f }, 20, 40, { 0, 0 } },
    { "B_kabe", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_02", 2, 2, { 0.0f, 0.0f, 0.0f }, 10, 50, { 0, 0 } },
    { "nami_03", 2, 2, { 0.0f, 0.0f, 0.0f }, 20, 40, { 0, 0 } },
    { "nami_04", 2, 2, { 0.0f, 0.0f, 0.0f }, 30, 30, { 0, 0 } },
    { "C_kabe", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_00", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_01", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "fune_R", 2, 0, { 120.0f, 0.0f, -64.0f }, 10, 50, { 0, 0 } },
    { "A1_kabe", 2, 0, { 0.0f, 0.0f, -51.0f }, 20, 40, { 0, 0 } },
    { "iwa_01", 2, 1, { 232.0f, 0.0f, -21.0f }, 0, 60, { 0, 0 } },
    { "iwa_02", 2, 1, { -234.0f, 0.0f, -21.0f }, 10, 50, { 0, 0 } },
    { "iwa_03", 2, 1, { 0.0f, 0.0f, -38.0f }, 20, 40, { 0, 0 } },
    { "B_kabe", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_02", 2, 2, { 0.0f, 0.0f, 0.0f }, 10, 50, { 0, 0 } },
    { "nami_03", 2, 2, { 0.0f, 0.0f, 0.0f }, 20, 40, { 0, 0 } },
    { "nami_04", 2, 2, { 0.0f, 0.0f, 0.0f }, 30, 30, { 0, 0 } },
    { "C_kabe", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_00", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_01", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A1_kabe", 2, 0, { 0.0f, 0.0f, -51.0f }, 20, 40, { 0, 0 } },
    { "iwa_01", 2, 1, { 192.0f, 0.0f, 62.0f }, 0, 60, { 0, 0 } },
    { "iwa_02", 2, 1, { -173.0f, 0.0f, 53.0f }, 10, 50, { 0, 0 } },
    { "iwa_03", 2, 1, { 0.0f, 0.0f, -38.0f }, 20, 40, { 0, 0 } },
    { "B_kabe", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_02", 2, 2, { 0.0f, 0.0f, 0.0f }, 10, 50, { 0, 0 } },
    { "nami_03", 2, 2, { 0.0f, 0.0f, 0.0f }, 20, 40, { 0, 0 } },
    { "nami_04", 2, 2, { 0.0f, 0.0f, 0.0f }, 30, 30, { 0, 0 } },
    { "C_kabe", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_00", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_01", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

BattleStageData dou_btlstage_data[6] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_4",                                 // mSpecificStageDataDir
        5,                                          // mNumObjects
        &dou_btlobject_data[0],                     // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_4",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &dou_btlobject_data[5],                     // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_5",                                 // mSpecificStageDataDir
        8,                                          // mNumObjects
        &dou_btlobject_data[7],                     // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_3",                                 // mSpecificStageDataDir
        13,                                         // mNumObjects
        &dou_btlobject_data[15],                    // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { NULL, NULL, &bgset_dou_wave_event, NULL }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_3",                                 // mSpecificStageDataDir
        12,                                         // mNumObjects
        &dou_btlobject_data[28],                    // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { NULL, NULL, &bgset_dou_wave_event, NULL }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_05_3",                                 // mSpecificStageDataDir
        11,                                         // mNumObjects
        &dou_btlobject_data[40],                    // mObjects
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
            26,                                         // damagePattern
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
            26,                                         // damagePattern
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { NULL, NULL, &bgset_dou_wave_event, NULL }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
};

BattleUnitSetup btlparty_dou_dou_00_01_off_1[3] = {
    { &unit_kuriboo_6_data_110E8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_kuriboo_6_data_110E8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_kuriboo_6_data_110E8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_dou_dou_00_01_off_1 = { 3, btlparty_dou_dou_00_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_01_01_off_01[3] = {
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
};

BattleGroupSetup btlgrp_dou_dou_01_01_off_01 = { 3, btlparty_dou_dou_01_01_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_02_01_off_01[2] = {
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
};

BattleGroupSetup btlgrp_dou_dou_02_01_off_01 = { 2, btlparty_dou_dou_02_01_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_02_02_off_01[4] = {
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
};

BattleGroupSetup btlgrp_dou_dou_02_02_off_01 = { 4, btlparty_dou_dou_02_02_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_03_01_off_01[2] = {
    { &unit_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer_cannon },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_03_01_off_01 = { 2, btlparty_dou_dou_03_01_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_03_99_off_01[2] = {
    { &unit_killer, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
    { &unit_killer, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
};

BattleGroupSetup btlgrp_dou_dou_03_99_off_01 = { 2, btlparty_dou_dou_03_99_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_03_02_off_01[2] = {
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_03_02_off_01 = { 2, btlparty_dou_dou_03_02_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_03_03_off_01[3] = {
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_03_03_off_01 = { 3, btlparty_dou_dou_03_03_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_05_01_off_01[2] = {
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
};

BattleGroupSetup btlgrp_dou_dou_05_01_off_01 = { 2, btlparty_dou_dou_05_01_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_05_02_off_01[2] = {
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_dou_dou_05_02_off_01 = { 2, btlparty_dou_dou_05_02_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_06_01_off_01[3] = {
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_dou_dou_06_01_off_01 = { 3, btlparty_dou_dou_06_01_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_06_02_off_01[4] = {
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_dou_dou_06_02_off_01 = { 4, btlparty_dou_dou_06_02_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_06_03_off_01[1] = {
    { &unit_patamet_6_data_19100, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_dou_dou_06_03_off_01 = { 1, btlparty_dou_dou_06_03_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_07_01_off_01[3] = {
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
};

BattleGroupSetup btlgrp_dou_dou_07_01_off_01 = { 3, btlparty_dou_dou_07_01_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_08_01_off_01[4] = {
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_6_data_15A08, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_08_01_off_01 = { 4, btlparty_dou_dou_08_01_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_09_01_off_01[3] = {
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
};

BattleGroupSetup btlgrp_dou_dou_09_01_off_01 = { 3, btlparty_dou_dou_09_01_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_09_02_off_01[3] = {
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_hermos_6_data_17118, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hermos },
    { &unit_bubble_6_data_11C80, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
};

BattleGroupSetup btlgrp_dou_dou_09_02_off_01 = { 3, btlparty_dou_dou_09_02_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_dou_dou_12_01_off_01[2] = {
    { &unit_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer_cannon },
    { &unit_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_12_01_off_01 = { 2, btlparty_dou_dou_12_01_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_12_99_off_01[2] = {
    { &unit_killer, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
    { &unit_killer, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
};

BattleGroupSetup btlgrp_dou_dou_12_99_off_01 = { 2, btlparty_dou_dou_12_99_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_13_01_off_01[2] = {
    { &unit_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer_cannon },
    { &unit_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_dou_dou_13_01_off_01 = { 2, btlparty_dou_dou_13_01_off_01, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_dou_dou_13_99_off_01[2] = {
    { &unit_killer, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
    { &unit_killer, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_killer },
};

BattleGroupSetup btlgrp_dou_dou_13_99_off_01 = { 2, btlparty_dou_dou_13_99_off_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_test1_off[2] = {
    { 50, &btlgrp_dou_dou_00_01_off_1, &dou_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_test2_off[2] = {
    { 50, &btlgrp_dou_dou_00_01_off_1, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_test3_off[2] = {
    { 50, &btlgrp_dou_dou_00_01_off_1, &dou_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_01_01_off[2] = {
    { 20, &btlgrp_dou_dou_01_01_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_02_01_off[2] = {
    { 20, &btlgrp_dou_dou_02_01_off_01, &dou_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_02_02_off[2] = {
    { 20, &btlgrp_dou_dou_02_02_off_01, &dou_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_03_01_off[2] = {
    { 20, &btlgrp_dou_dou_03_01_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_03_99_off[2] = {
    { 20, &btlgrp_dou_dou_03_99_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_03_02_off[2] = {
    { 20, &btlgrp_dou_dou_03_02_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_03_03_off[2] = {
    { 20, &btlgrp_dou_dou_03_03_off_01, &dou_btlstage_data[4] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_05_01_off[2] = {
    { 20, &btlgrp_dou_dou_05_01_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_05_02_off[2] = {
    { 20, &btlgrp_dou_dou_05_02_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_06_01_off[2] = {
    { 20, &btlgrp_dou_dou_06_01_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_06_02_off[2] = {
    { 20, &btlgrp_dou_dou_06_02_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_06_03_off[2] = {
    { 20, &btlgrp_dou_dou_06_03_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_07_01_off[2] = {
    { 20, &btlgrp_dou_dou_07_01_off_01, dou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_08_01_off[2] = {
    { 20, &btlgrp_dou_dou_08_01_off_01, &dou_btlstage_data[4] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_09_01_off[2] = {
    { 20, &btlgrp_dou_dou_09_01_off_01, &dou_btlstage_data[4] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_09_02_off[2] = {
    { 20, &btlgrp_dou_dou_09_02_off_01, &dou_btlstage_data[4] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_12_01_off[2] = {
    { 20, &btlgrp_dou_dou_12_01_off_01, &dou_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_12_02_off[2] = {
    { 20, &btlgrp_dou_dou_12_01_off_01, &dou_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_12_99_off[2] = {
    { 20, &btlgrp_dou_dou_12_99_off_01, &dou_btlstage_data[5] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_13_01_off[2] = {
    { 20, &btlgrp_dou_dou_13_01_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_13_02_off[2] = {
    { 20, &btlgrp_dou_dou_13_01_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_13_03_off[2] = {
    { 20, &btlgrp_dou_dou_13_01_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_13_04_off[2] = {
    { 20, &btlgrp_dou_dou_13_01_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_dou_btlno_dou_13_99_off[2] = {
    { 20, &btlgrp_dou_dou_13_99_off_01, &dou_btlstage_data[1] },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_dou_tbl[28] = {
    {
        "マップstg_05_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_05_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_05_5",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU01エルモス",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_01_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU02バブル",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_02_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU02バブル",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_02_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU03キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_03_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU03キラー",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_03_99_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU03ヘビーボム",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_dou_btlno_dou_03_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "DOU03ヘビーボム",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_dou_btlno_dou_03_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "DOU05エルモス",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_05_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU05パタメット",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_05_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU06パタメット",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_06_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU06パタメット",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_06_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU06パタメット",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_06_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU07エルモス",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_07_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU08ヘビーボム",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_dou_btlno_dou_08_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "DOU09バブル",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_09_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU09エルモス",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_09_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU12キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_12_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU12キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_12_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU12キラー",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_12_99_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU13キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_13_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU13キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_13_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU13キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_13_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU13キラー大砲",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_13_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "DOU13キラー",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_dou_btlno_dou_13_99_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_dou_setup_no_tbl[56] = {
    (u32)"BTLNO_DOU_TEST1", 0x00000000, (u32)"BTLNO_DOU_TEST2", 0x00000001,
    (u32)"BTLNO_DOU_TEST3", 0x00000002, (u32)"BTLNO_DOU_01_01", 0x00000003,
    (u32)"BTLNO_DOU_02_01", 0x00000004, (u32)"BTLNO_DOU_02_02", 0x00000005,
    (u32)"BTLNO_DOU_03_01", 0x00000006, (u32)"BTLNO_DOU_03_99", 0x00000007,
    (u32)"BTLNO_DOU_03_02", 0x00000008, (u32)"BTLNO_DOU_03_03", 0x00000009,
    (u32)"BTLNO_DOU_05_01", 0x0000000A, (u32)"BTLNO_DOU_05_02", 0x0000000B,
    (u32)"BTLNO_DOU_06_01", 0x0000000C, (u32)"BTLNO_DOU_06_02", 0x0000000D,
    (u32)"BTLNO_DOU_06_03", 0x0000000E, (u32)"BTLNO_DOU_07_01", 0x0000000F,
    (u32)"BTLNO_DOU_08_01", 0x00000010, (u32)"BTLNO_DOU_09_01", 0x00000011,
    (u32)"BTLNO_DOU_09_02", 0x00000012, (u32)"BTLNO_DOU_12_01", 0x00000013,
    (u32)"BTLNO_DOU_12_02", 0x00000014, (u32)"BTLNO_DOU_12_99", 0x00000015,
    (u32)"BTLNO_DOU_13_01", 0x00000016, (u32)"BTLNO_DOU_13_02", 0x00000017,
    (u32)"BTLNO_DOU_13_03", 0x00000018, (u32)"BTLNO_DOU_13_04", 0x00000019,
    (u32)"BTLNO_DOU_13_99", 0x0000001A, 0x00000000, 0x00000000,
};

#include "rel/common/_wave_offset.h"

EVT_BEGIN(bgset_dou_wave_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 200)
    SET_VALUE(LW(2), 200)
    SET_VALUE(LW(3), 100)
    SET_VALUE(LW(4), 300)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 400)
    SET_VALUE(LW(7), 2500)
    LABEL(0)
    CALL(_wave_offset, STRING("nami_00"), LW(0), 800, 30, LW(1), 300, 10)
    CALL(_wave_offset, STRING("nami_01"), LW(2), 600, 20, LW(3), 250, 6)
    CALL(_wave_offset, STRING("nami_02"), LW(4), 1100, 40, LW(5), 360, 7)
    CALL(_wave_offset, STRING("nami_03"), LW(6), 1250, 50, LW(7), 450, 8)
    CALL(_wave_offset, STRING("nami_04"), LW(8), 1100, 50, LW(9), 400, 6)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
