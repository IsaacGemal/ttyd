#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_weapon_power.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern ItemDropData battle_item_monochrome__pakkun[5];
extern ItemDropData battle_item_monochrome_kurokumorn[9];
extern ItemDropData battle_item_monochrome_sinemon[8];
extern ItemDropData battle_item_vivian1[1];
extern BattleUnitKind unit_boss_vivian;
extern BattleUnitKind unit_monochrome_kurokumorn_30_data_17868;
extern BattleUnitKind unit_monochrome_pakkun_30_data_166A8;
extern BattleUnitKind unit_monochrome_sinemon_30_data_15620;

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData win_btlobject_data[5] = {
    { "A1",     0x0002, 0x0000, { 92.0f, 0.0f, -46.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "S_hana", 0x0002, 0x0001, {  2.0f, 0.0f, -46.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "S_kusa", 0x0002, 0x0001, {  2.0f, 0.0f,  64.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "B",      0x0002, 0x0002, {  0.0f, 0.0f,   0.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "C",      0x0002, 0x0003, {  0.0f, 0.0f,   0.0f }, 0, 0x3C, { 0x00, 0x00 } },
};

BattleStageData win_btlstage_data[1] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_02_0",                                 // mSpecificStageDataDir
        5,                                          // mNumObjects
        &win_btlobject_data[0],                     // mObjects
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
            0x0210006F,                                 // targetClassFlags
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
        { 0 },                                      // layerDestroyed
    },
};

BattleUnitSetup btlparty_win_win_00_01_off_1[2] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_01_off_1 = { 2, btlparty_win_win_00_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_01_on_1[2] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_01_on_1 = { 2, btlparty_win_win_00_01_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_01_on_2[3] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_01_on_2 = { 3, btlparty_win_win_00_01_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_01_on_3[4] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_01_on_3 = { 4, btlparty_win_win_00_01_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_02_off_1[2] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_02_off_1 = { 2, btlparty_win_win_00_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_02_on_1[2] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_02_on_1 = { 2, btlparty_win_win_00_02_on_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_02_on_2[3] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_02_on_2 = { 3, btlparty_win_win_00_02_on_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_03_off_1[3] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_03_off_1 = { 3, btlparty_win_win_00_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_03_on_1[2] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_win_win_00_03_on_1 = { 2, btlparty_win_win_00_03_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_03_on_2[2] = {
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_00_03_on_2 = { 2, btlparty_win_win_00_03_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_00_04_off_1[1] = {
    { &unit_boss_vivian, 1, 0x04000004, { 55.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_vivian1 },
};

BattleGroupSetup btlgrp_win_win_00_04_off_1 = { 1, btlparty_win_win_00_04_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_02_01_off_1[2] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_02_01_off_1 = { 2, btlparty_win_win_02_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_02_01_on_1[2] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_02_01_on_1 = { 2, btlparty_win_win_02_01_on_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_02_01_on_2[3] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_02_01_on_2 = { 3, btlparty_win_win_02_01_on_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_02_01_on_3[3] = {
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_02_01_on_3 = { 3, btlparty_win_win_02_01_on_3, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_win_win_02_02_off_1[2] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_02_02_off_1 = { 2, btlparty_win_win_02_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_02_02_on_1[2] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_02_02_on_1 = { 2, btlparty_win_win_02_02_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_02_02_on_2[3] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_02_02_on_2 = { 3, btlparty_win_win_02_02_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_02_02_on_3[3] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_02_02_on_3 = { 3, btlparty_win_win_02_02_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_03_01_off_1[4] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_03_01_off_1 = { 4, btlparty_win_win_03_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_03_01_on_1[2] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_03_01_on_1 = { 2, btlparty_win_win_03_01_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_03_01_on_2[3] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_03_01_on_2 = { 3, btlparty_win_win_03_01_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_03_01_on_3[3] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_kurokumorn_30_data_17868, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_30_data_15620, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_win_win_03_01_on_3 = { 3, btlparty_win_win_03_01_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_win_win_03_01_on_4[4] = {
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_30_data_166A8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_win_win_03_01_on_4 = { 4, btlparty_win_win_03_01_on_4, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

NpcBattleSetupEntry btlgrpsel_win_btlno_win_test1_off[2] = {
    { 50, &btlgrp_win_win_00_01_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_01_on[4] = {
    { 20, &btlgrp_win_win_00_01_on_1, win_btlstage_data },
    { 20, &btlgrp_win_win_00_01_on_2, win_btlstage_data },
    { 10, &btlgrp_win_win_00_01_on_3, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_01_off[2] = {
    { 10, &btlgrp_win_win_00_01_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_02_on[3] = {
    { 20, &btlgrp_win_win_00_02_on_1, win_btlstage_data },
    { 20, &btlgrp_win_win_00_02_on_2, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_02_off[2] = {
    { 10, &btlgrp_win_win_00_02_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_03_on[3] = {
    { 20, &btlgrp_win_win_00_03_on_1, win_btlstage_data },
    { 20, &btlgrp_win_win_00_03_on_2, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_03_off[2] = {
    { 10, &btlgrp_win_win_00_03_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_04_off[2] = {
    { 50, &btlgrp_win_win_00_04_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_01_on[4] = {
    { 20, &btlgrp_win_win_02_01_on_1, win_btlstage_data },
    { 20, &btlgrp_win_win_02_01_on_2, win_btlstage_data },
    { 10, &btlgrp_win_win_02_01_on_3, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_01_off[2] = {
    { 10, &btlgrp_win_win_02_01_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_02_on[4] = {
    { 20, &btlgrp_win_win_02_02_on_1, win_btlstage_data },
    { 20, &btlgrp_win_win_02_02_on_2, win_btlstage_data },
    { 10, &btlgrp_win_win_02_02_on_3, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_02_off[2] = {
    { 10, &btlgrp_win_win_02_02_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_03_01_on[5] = {
    { 10, &btlgrp_win_win_03_01_on_1, win_btlstage_data },
    { 10, &btlgrp_win_win_03_01_on_2, win_btlstage_data },
    { 10, &btlgrp_win_win_03_01_on_3, win_btlstage_data },
    { 10, &btlgrp_win_win_03_01_on_4, win_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_03_01_off[2] = {
    { 10, &btlgrp_win_win_03_01_off_1, win_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_win_tbl[9] = {
    {
        "マップstg_02_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_win_btlno_win_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "win_00ドクガッスン１",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129997318,                                                                                         // flag
        btlgrpsel_win_btlno_win_00_01_on,                                                                   // onLoadouts
        btlgrpsel_win_btlno_win_00_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "win_00モノクロシンエモン",                                                                                  // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129997318,                                                                                         // flag
        btlgrpsel_win_btlno_win_00_02_on,                                                                   // onLoadouts
        btlgrpsel_win_btlno_win_00_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "win_00ドクガッスン３",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129997318,                                                                                         // flag
        btlgrpsel_win_btlno_win_00_03_on,                                                                   // onLoadouts
        btlgrpsel_win_btlno_win_00_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "win00ビビアン３姉妹",                                                                                     // name
        "win_00",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_win_btlno_win_00_04_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_3WOMEN1",                                                                            // music
    },
    {
        "win_02シンエモン１",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129997320,                                                                                         // flag
        btlgrpsel_win_btlno_win_02_01_on,                                                                   // onLoadouts
        btlgrpsel_win_btlno_win_02_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "win_02パックンフラワー",                                                                                     // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -129997320,                                                                                           // flag
        btlgrpsel_win_btlno_win_02_02_on,                                                                     // onLoadouts
        btlgrpsel_win_btlno_win_02_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "win_03パックン１",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -129997319,                                                                                           // flag
        btlgrpsel_win_btlno_win_03_01_on,                                                                     // onLoadouts
        btlgrpsel_win_btlno_win_03_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
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
u32 btl_win_setup_no_tbl[18] = {
    (u32)"BTLNO_WIN_TEST1", 0x00000000, (u32)"BTLNO_WIN_00_01", 0x00000001,
    (u32)"BTLNO_WIN_00_02", 0x00000002, (u32)"BTLNO_WIN_00_03", 0x00000003,
    (u32)"BTLNO_WIN_00_04", 0x00000004, (u32)"BTLNO_WIN_02_01", 0x00000005,
    (u32)"BTLNO_WIN_02_02", 0x00000006, (u32)"BTLNO_WIN_03_01", 0x00000007,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
