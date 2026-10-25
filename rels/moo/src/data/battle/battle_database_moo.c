#include "battle_database_moo.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern ItemDropData battle_item_barriern_z[1];
extern ItemDropData battle_item_default_param_list[1];
extern ItemDropData battle_item_sinemon[8];
extern BattleUnitKind unit_barriern_z_18_data_72F0;
extern BattleUnitKind unit_kuriboo_18_data_4608;
extern BattleUnitKind unit_sinemon_18_data_5D28;
static USER_FUNC(_space_rotate);
s32 bgset_moo_space_event[35];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData moo_btlobject_data[5] = {
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"space", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_01", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_02", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_03", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
};

BattleStageData moo_btlstage_data[1] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_07_4",                                 // mSpecificStageDataDir
        5,                                          // mNumObjects
        &moo_btlobject_data[0],                     // mObjects
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
        { NULL, NULL, NULL, &bgset_moo_space_event }, // layerEvents
        0,                                          // field_0x1B0
        { 0 },                                      // layerDestroyed
    },
};

BattleUnitSetup btlparty_moo_moo_00_01_off_1[3] = {
    { &unit_kuriboo_18_data_4608, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
    { &unit_kuriboo_18_data_4608, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_kuriboo_18_data_4608, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_moo_moo_00_01_off_1 = { 3, btlparty_moo_moo_00_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_01_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_01_off_1 = { 2, btlparty_moo_moo_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_02_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_02_off_1 = { 2, btlparty_moo_moo_01_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_02_off_2[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_02_off_2 = { 3, btlparty_moo_moo_01_02_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_03_off_1[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_03_off_1 = { 3, btlparty_moo_moo_01_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_04_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_04_off_1 = { 2, btlparty_moo_moo_01_04_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_04_off_2[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_04_off_2 = { 3, btlparty_moo_moo_01_04_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_04_off_3[4] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_04_off_3 = { 4, btlparty_moo_moo_01_04_off_3, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_01_05_off_1[4] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_01_05_off_1 = { 4, btlparty_moo_moo_01_05_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_02_01_off_1[2] = {
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_moo_moo_02_01_off_1 = { 2, btlparty_moo_moo_02_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_02_02_off_1[2] = {
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_02_02_off_1 = { 2, btlparty_moo_moo_02_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_02_03_off_1[2] = {
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_moo_moo_02_03_off_1 = { 2, btlparty_moo_moo_02_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_02_04_off_1[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_02_04_off_1 = { 3, btlparty_moo_moo_02_04_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_02_05_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_barriern_z_18_data_72F0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_moo_moo_02_05_off_1 = { 2, btlparty_moo_moo_02_05_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_01_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_01_off_1 = { 2, btlparty_moo_moo_05_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_01_off_2[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_01_off_2 = { 3, btlparty_moo_moo_05_01_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_01_off_3[4] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_01_off_3 = { 4, btlparty_moo_moo_05_01_off_3, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_02_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_02_off_1 = { 2, btlparty_moo_moo_05_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_03_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_03_off_1 = { 2, btlparty_moo_moo_05_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_03_off_2[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_03_off_2 = { 3, btlparty_moo_moo_05_03_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_04_off_1[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_04_off_1 = { 3, btlparty_moo_moo_05_04_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_05_off_1[2] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_05_off_1 = { 2, btlparty_moo_moo_05_05_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_05_off_2[3] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_05_off_2 = { 3, btlparty_moo_moo_05_05_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_moo_moo_05_05_off_3[4] = {
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_18_data_5D28, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_moo_moo_05_05_off_3 = { 4, btlparty_moo_moo_05_05_off_3, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_test1_off[2] = {
    { 50, &btlgrp_moo_moo_00_01_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_01_01_off[2] = {
    { 10, &btlgrp_moo_moo_01_01_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_01_02_off[3] = {
    { 10, &btlgrp_moo_moo_01_02_off_1, moo_btlstage_data },
    { 10, &btlgrp_moo_moo_01_02_off_2, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_01_03_off[2] = {
    { 10, &btlgrp_moo_moo_01_03_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_01_04_off[4] = {
    { 10, &btlgrp_moo_moo_01_04_off_1, moo_btlstage_data },
    { 10, &btlgrp_moo_moo_01_04_off_2, moo_btlstage_data },
    { 5, &btlgrp_moo_moo_01_04_off_3, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_01_05_off[2] = {
    { 5, &btlgrp_moo_moo_01_05_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_02_01_off[2] = {
    { 20, &btlgrp_moo_moo_02_01_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_02_02_off[2] = {
    { 20, &btlgrp_moo_moo_02_02_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_02_03_off[2] = {
    { 20, &btlgrp_moo_moo_02_03_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_02_04_off[2] = {
    { 20, &btlgrp_moo_moo_02_04_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_02_05_off[2] = {
    { 20, &btlgrp_moo_moo_02_05_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_05_01_off[4] = {
    { 10, &btlgrp_moo_moo_05_01_off_1, moo_btlstage_data },
    { 10, &btlgrp_moo_moo_05_01_off_2, moo_btlstage_data },
    { 5, &btlgrp_moo_moo_05_01_off_3, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_05_02_off[2] = {
    { 20, &btlgrp_moo_moo_05_02_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_05_03_off[3] = {
    { 10, &btlgrp_moo_moo_05_03_off_1, moo_btlstage_data },
    { 10, &btlgrp_moo_moo_05_03_off_2, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_05_04_off[2] = {
    { 20, &btlgrp_moo_moo_05_04_off_1, moo_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_moo_btlno_moo_05_05_off[4] = {
    { 10, &btlgrp_moo_moo_05_05_off_1, moo_btlstage_data },
    { 10, &btlgrp_moo_moo_05_05_off_2, moo_btlstage_data },
    { 5, &btlgrp_moo_moo_05_05_off_3, moo_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_moo_tbl[17] = {
    {
        "マップstg_07_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo01シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_01_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo01シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_01_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo01シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_01_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo01シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_01_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo01シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_01_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo02バリアーンZ",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_02_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo02バリアーンZ",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_02_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo02バリアーンZ",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_02_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo02シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_02_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo02シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_02_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo05シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_05_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo05シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_05_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo05シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_05_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo05シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_05_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "moo05シンエモン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_moo_btlno_moo_05_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_moo_setup_no_tbl[34] = {
    (u32)"BTLNO_MOO_TEST1", 0x00000000, (u32)"BTLNO_MOO_01_01", 0x00000001,
    (u32)"BTLNO_MOO_01_02", 0x00000002, (u32)"BTLNO_MOO_01_03", 0x00000003,
    (u32)"BTLNO_MOO_01_04", 0x00000004, (u32)"BTLNO_MOO_01_05", 0x00000005,
    (u32)"BTLNO_MOO_02_01", 0x00000006, (u32)"BTLNO_MOO_02_02", 0x00000007,
    (u32)"BTLNO_MOO_02_03", 0x00000008, (u32)"BTLNO_MOO_02_04", 0x00000009,
    (u32)"BTLNO_MOO_02_05", 0x0000000A, (u32)"BTLNO_MOO_05_01", 0x0000000B,
    (u32)"BTLNO_MOO_05_02", 0x0000000C, (u32)"BTLNO_MOO_05_03", 0x0000000D,
    (u32)"BTLNO_MOO_05_04", 0x0000000E, (u32)"BTLNO_MOO_05_05", 0x0000000F,
    0x00000000, 0x00000000,
};

#include "rel/common/_space_rotate.h"

EVT_BEGIN(bgset_moo_space_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 0)
    LABEL(0)
    CALL(_space_rotate, STRING("star_01"), LW(1), 7500)
    CALL(_space_rotate, STRING("star_02"), LW(2), 6750)
    CALL(_space_rotate, STRING("star_03"), LW(3), 6000)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
