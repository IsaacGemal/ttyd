#include "battle/battle_weapon_power.h"
#include "battle/battle_stage.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern ItemDropData battle_item_faker_mario[1];
extern ItemDropData battle_item_hyper_kuriboo[10];
extern ItemDropData battle_item_hyper_patakuri[10];
extern ItemDropData battle_item_hyper_sinemon[9];
extern ItemDropData battle_item_hyper_togekuri[10];
extern ItemDropData battle_item_pansy[10];
extern ItemDropData battle_item_twinkling_pansy[10];
extern BattleUnitKind unit_faker_mario_gra;
extern BattleUnitKind unit_hyper_kuriboo;
extern BattleUnitKind unit_hyper_patakuri;
extern BattleUnitKind unit_hyper_sinemon_11_data_DA78;
extern BattleUnitKind unit_hyper_togekuri;
extern BattleUnitKind unit_pansy;
extern BattleUnitKind unit_twinkling_pansy_11_data_F890;

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData gra_btlobject_data[22] = {
    { "ki_1", 2, 0, { -100.0f, 0.0f, -30.0f }, 0, 60, { 0, 0 } },
    { "isi_A", 2, 0, { 150.0f, 0.0f, -30.0f }, 10, 50, { 0, 0 } },
    { "isi_B", 2, 0, { 50.0f, 0.0f, -34.0f }, 20, 40, { 0, 0 } },
    { "isigaki", 2, 1, { 2.0f, 0.0f, -23.0f }, 0, 60, { 0, 0 } },
    { "kusa_1", 2, 1, { -184.0f, 0.0f, 37.0f }, 10, 50, { 0, 0 } },
    { "kusa_2", 2, 1, { 180.0f, 0.0f, 60.0f }, 20, 40, { 0, 0 } },
    { "kusa_3", 2, 1, { 70.0f, 0.0f, 65.0f }, 30, 30, { 0, 0 } },
    { "kusa_4", 2, 1, { -87.0f, 0.0f, 56.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "kusa_1", 2, 1, { 147.0f, 0.0f, 76.0f }, 0, 60, { 0, 0 } },
    { "kusa_2", 2, 1, { 35.0f, 0.0f, 80.0f }, 10, 50, { 0, 0 } },
    { "kusa_3", 2, 1, { -77.0f, 0.0f, 77.0f }, 20, 40, { 0, 0 } },
    { "kusa_4", 2, 1, { -155.0f, 0.0f, 71.0f }, 30, 30, { 0, 0 } },
    { "kusamura_mae", 2, 1, { 0.0f, 0.0f, 72.0f }, 0, 60, { 0, 0 } },
    { "kusamura_usiro", 2, 1, { 0.0f, 0.0f, -65.0f }, 10, 50, { 0, 0 } },
    { "iwa", 2, 1, { 0.0f, 0.0f, -71.0f }, 20, 40, { 0, 0 } },
    { "ki_1", 2, 0, { 100.0f, 0.0f, -51.0f }, 0, 60, { 0, 0 } },
    { "ki_2", 2, 0, { -123.0f, 0.0f, -48.0f }, 10, 50, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "murasaki", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "ao", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

BattleStageData gra_btlstage_data[2] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_0",                                 // mSpecificStageDataDir
        10,                                         // mNumObjects
        &gra_btlobject_data[0],                     // mObjects
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
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // stageEffect
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
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // stageEffect
        },
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_04_1",                                 // mSpecificStageDataDir
        11,                                         // mNumObjects
        &gra_btlobject_data[10],                    // mObjects
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
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // stageEffect
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
            { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 },   // stageEffect
        },
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 0, 1, 1 },                                // layerDestroyed
    },
};

BattleUnitSetup btlparty_gra_gra_00_01_off_1[1] = {
    { &unit_faker_mario_gra, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_faker_mario },
};

BattleGroupSetup btlgrp_gra_gra_00_01_off_1 = { 1, btlparty_gra_gra_00_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_01_off_1[3] = {
    { &unit_hyper_kuriboo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_kuriboo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_kuriboo, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
};

BattleGroupSetup btlgrp_gra_gra_01_01_off_1 = { 3, btlparty_gra_gra_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_01_on_1[3] = {
    { &unit_hyper_kuriboo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_kuriboo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_kuriboo, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
};

BattleGroupSetup btlgrp_gra_gra_01_01_on_1 = { 3, btlparty_gra_gra_01_01_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_01_on_2[3] = {
    { &unit_hyper_kuriboo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_patakuri, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_pansy, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_01_01_on_2 = { 3, btlparty_gra_gra_01_01_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_01_on_3[3] = {
    { &unit_hyper_kuriboo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_hyper_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_pansy, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_01_01_on_3 = { 3, btlparty_gra_gra_01_01_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_03_off_1[3] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_01_03_off_1 = { 3, btlparty_gra_gra_01_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_03_on_1[3] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_01_03_on_1 = { 3, btlparty_gra_gra_01_03_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_03_on_2[4] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_01_03_on_2 = { 4, btlparty_gra_gra_01_03_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_03_on_3[4] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_pansy, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_01_03_on_3 = { 4, btlparty_gra_gra_01_03_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_04_off_1[3] = {
    { &unit_hyper_togekuri, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
};

BattleGroupSetup btlgrp_gra_gra_01_04_off_1 = { 3, btlparty_gra_gra_01_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_04_on_1[3] = {
    { &unit_hyper_togekuri, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
};

BattleGroupSetup btlgrp_gra_gra_01_04_on_1 = { 3, btlparty_gra_gra_01_04_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_04_on_2[4] = {
    { &unit_hyper_togekuri, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_01_04_on_2 = { 4, btlparty_gra_gra_01_04_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_01_04_on_3[3] = {
    { &unit_hyper_togekuri, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_kuriboo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_kuriboo },
    { &unit_pansy, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_01_04_on_3 = { 3, btlparty_gra_gra_01_04_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_03_01_off_1[2] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_03_01_off_1 = { 2, btlparty_gra_gra_03_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_03_01_off_2[3] = {
    { &unit_hyper_patakuri, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_pansy, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_03_01_off_2 = { 3, btlparty_gra_gra_03_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_03_04_off_1[2] = {
    { &unit_pansy, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_03_04_off_1 = { 2, btlparty_gra_gra_03_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_03_04_off_2[3] = {
    { &unit_pansy, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_twinkling_pansy_11_data_F890, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_gra_gra_03_04_off_2 = { 3, btlparty_gra_gra_03_04_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_03_04_off_3[3] = {
    { &unit_pansy, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_hyper_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
    { &unit_hyper_patakuri, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
};

BattleGroupSetup btlgrp_gra_gra_03_04_off_3 = { 3, btlparty_gra_gra_03_04_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_03_05_off_1[4] = {
    { &unit_pansy, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_03_05_off_1 = { 4, btlparty_gra_gra_03_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_03_05_off_2[3] = {
    { &unit_pansy, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_twinkling_pansy_11_data_F890, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_gra_gra_03_05_off_2 = { 3, btlparty_gra_gra_03_05_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_03_05_off_3[4] = {
    { &unit_pansy, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_hyper_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_patakuri },
    { &unit_hyper_togekuri, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togekuri },
};

BattleGroupSetup btlgrp_gra_gra_03_05_off_3 = { 4, btlparty_gra_gra_03_05_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_04_02_off_1[3] = {
    { &unit_pansy, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_pansy, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
};

BattleGroupSetup btlgrp_gra_gra_04_02_off_1 = { 3, btlparty_gra_gra_04_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_04_02_off_2[2] = {
    { &unit_pansy, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_gra_gra_04_02_off_2 = { 2, btlparty_gra_gra_04_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_04_02_off_3[3] = {
    { &unit_pansy, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_pansy, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pansy },
    { &unit_twinkling_pansy_11_data_F890, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_gra_gra_04_02_off_3 = { 3, btlparty_gra_gra_04_02_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_gra_gra_05_01_off_1[2] = {
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_gra_gra_05_01_off_1 = { 2, btlparty_gra_gra_05_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_05_01_off_2[3] = {
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_twinkling_pansy_11_data_F890, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_gra_gra_05_01_off_2 = { 3, btlparty_gra_gra_05_01_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_05_03_off_1[3] = {
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_gra_gra_05_03_off_1 = { 3, btlparty_gra_gra_05_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_05_03_off_2[3] = {
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_twinkling_pansy_11_data_F890, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_gra_gra_05_03_off_2 = { 3, btlparty_gra_gra_05_03_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gra_gra_05_03_off_3[4] = {
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_pansy, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_pansy, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_hyper_sinemon_11_data_DA78, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_gra_gra_05_03_off_3 = { 4, btlparty_gra_gra_05_03_off_3, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_test1_off[2] = {
    { 50, &btlgrp_gra_gra_01_01_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_test2_off[2] = {
    { 50, &btlgrp_gra_gra_01_01_off_1, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_00_01_off[2] = {
    { 50, &btlgrp_gra_gra_00_01_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_00_02_off[2] = {
    { 50, &btlgrp_gra_gra_00_01_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_01_on[4] = {
    { 10, &btlgrp_gra_gra_01_01_on_1, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_01_on_2, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_01_on_3, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_01_off[2] = {
    { 50, &btlgrp_gra_gra_01_01_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_03_on[4] = {
    { 10, &btlgrp_gra_gra_01_03_on_1, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_03_on_2, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_03_on_3, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_03_off[2] = {
    { 50, &btlgrp_gra_gra_01_03_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_04_on[4] = {
    { 10, &btlgrp_gra_gra_01_04_on_1, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_04_on_2, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_01_04_on_3, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_01_04_off[2] = {
    { 50, &btlgrp_gra_gra_01_04_off_1, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_03_01_off[3] = {
    { 10, &btlgrp_gra_gra_03_01_off_1, gra_btlstage_data },
    { 10, &btlgrp_gra_gra_03_01_off_2, gra_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_03_04_off[4] = {
    { 50, &btlgrp_gra_gra_03_04_off_1, &gra_btlstage_data[1] },
    { 10, &btlgrp_gra_gra_03_04_off_2, &gra_btlstage_data[1] },
    { 40, &btlgrp_gra_gra_03_04_off_3, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_03_05_off[4] = {
    { 30, &btlgrp_gra_gra_03_05_off_1, &gra_btlstage_data[1] },
    { 10, &btlgrp_gra_gra_03_05_off_2, &gra_btlstage_data[1] },
    { 30, &btlgrp_gra_gra_03_05_off_3, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_04_02_off[4] = {
    { 60, &btlgrp_gra_gra_04_02_off_1, &gra_btlstage_data[1] },
    { 30, &btlgrp_gra_gra_04_02_off_2, &gra_btlstage_data[1] },
    { 10, &btlgrp_gra_gra_04_02_off_3, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_05_01_off[3] = {
    { 100, &btlgrp_gra_gra_05_01_off_1, &gra_btlstage_data[1] },
    { 10, &btlgrp_gra_gra_05_01_off_2, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gra_btlno_gra_05_03_off[4] = {
    { 50, &btlgrp_gra_gra_05_03_off_1, &gra_btlstage_data[1] },
    { 10, &btlgrp_gra_gra_05_03_off_2, &gra_btlstage_data[1] },
    { 40, &btlgrp_gra_gra_05_03_off_3, &gra_btlstage_data[1] },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_gra_tbl[14] = {
    {
        "マップstg_04_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_04_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "GRA00ランペルその１",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_00_01_off,                                                                  // offLoadouts
        536870912,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG4_RUNPELL1",                                                                           // music
    },
    {
        "GRA00ランペルその２",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_00_02_off,                                                                  // offLoadouts
        536870912,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG4_RUNPELL1",                                                                           // music
    },
    {
        "GRA01ハイパークリボー",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -129997919,                                                                                           // flag
        btlgrpsel_gra_btlno_gra_01_01_on,                                                                     // onLoadouts
        btlgrpsel_gra_btlno_gra_01_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA01ハイパーパタクリ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -129997919,                                                                                           // flag
        btlgrpsel_gra_btlno_gra_01_03_on,                                                                     // onLoadouts
        btlgrpsel_gra_btlno_gra_01_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA01ハイパートゲクリ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -129997919,                                                                                           // flag
        btlgrpsel_gra_btlno_gra_01_04_on,                                                                     // onLoadouts
        btlgrpsel_gra_btlno_gra_01_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA03ハイパークリボー",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gra_btlno_gra_03_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA03パンジーさん",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gra_btlno_gra_03_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 10, 10, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA03パンジーさん",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gra_btlno_gra_03_05_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 10, 10, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA04パンジーさん",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gra_btlno_gra_04_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 10, 10, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "GRA05ハイパーシンエモン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_05_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "GRA05ハイパーシンエモン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gra_btlno_gra_05_03_off,                                                                  // offLoadouts
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
u32 btl_gra_setup_no_tbl[28] = {
    (u32)"BTLNO_GRA_TEST1", 0x00000000, (u32)"BTLNO_GRA_TEST2", 0x00000001,
    (u32)"BTLNO_GRA_00_01", 0x00000002, (u32)"BTLNO_GRA_00_02", 0x00000003,
    (u32)"BTLNO_GRA_01_01", 0x00000004, (u32)"BTLNO_GRA_01_03", 0x00000005,
    (u32)"BTLNO_GRA_01_04", 0x00000006, (u32)"BTLNO_GRA_03_01", 0x00000007,
    (u32)"BTLNO_GRA_03_04", 0x00000008, (u32)"BTLNO_GRA_03_05", 0x00000009,
    (u32)"BTLNO_GRA_04_02", 0x0000000A, (u32)"BTLNO_GRA_05_01", 0x0000000B,
    (u32)"BTLNO_GRA_05_03", 0x0000000C, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
