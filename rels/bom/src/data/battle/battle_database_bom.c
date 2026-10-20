#include "battle_database_bom.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_stage.h"
#include "eff/n64/eff_snowfall_n64.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_bllizard[9];
extern ItemDropData battle_item_default_param_list[1];
extern ItemDropData battle_item_ice_pakkun[5];
extern BattleUnitKind unit_bllizard_3_data_B7F0;
extern BattleUnitKind unit_ice_pakkun_3_data_CDB0;
extern BattleUnitKind unit_kuriboo_3_data_AC58;
s32 bgset_bom_snow_event[4];

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

// TODO: type
u32 bom_btlobject_data[66] = {
    (u32)"yuki_front", 0x00020001, 0x00000000, 0x00000000,
    0x42980000, 0x003C0000, (u32)"yuki_back", 0x00020001,
    0x00000000, 0x00000000, 0xC2200000, 0x0A320000,
    (u32)"kusa_01", 0x00020001, 0xC3270000, 0x00000000,
    0xC1A80000, 0x14280000, (u32)"kusa_02", 0x00020001,
    0x43220000, 0x00000000, 0xC1300000, 0x1E1E0000,
    (u32)"kabe_01", 0x00020001, 0x43420000, 0x00000000,
    0x42840000, 0x003C0000, (u32)"kabe_02", 0x00020001,
    0x42DE0000, 0x00000000, 0xC1C80000, 0x0A320000,
    (u32)"kabe_03", 0x00020000, 0xC28E0000, 0x00000000,
    0xC2700000, 0x003C0000, (u32)"ki_01", 0x00020000,
    0x43390000, 0x00000000, 0xC25C0000, 0x0A320000,
    (u32)"ki_02", 0x00020000, 0x42980000, 0x00000000,
    0xC2480000, 0x14280000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000,
};

BattleStageData bom_btlstage_data[1] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_07_0",                                 // mSpecificStageDataDir
        11,                                         // mNumObjects
        (BattleStageObjectData*)&bom_btlobject_data[0], // mObjects
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { NULL, NULL, NULL, &bgset_bom_snow_event }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
};

BattleUnitSetup btlparty_bom_bom_00_01_off_1[1] = {
    { &unit_kuriboo_3_data_AC58, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_bom_bom_00_01_off_1 = { 1, btlparty_bom_bom_00_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_bom_bom_03_01_off_1[2] = {
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_bom_bom_03_01_off_1 = { 2, btlparty_bom_bom_03_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_bom_bom_03_01_off_2[3] = {
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_bom_bom_03_01_off_2 = { 3, btlparty_bom_bom_03_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_bom_bom_03_03_off_1[2] = {
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_bom_bom_03_03_off_1 = { 2, btlparty_bom_bom_03_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_bom_bom_03_03_off_2[3] = {
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_bom_bom_03_03_off_2 = { 3, btlparty_bom_bom_03_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_bom_bom_04_01_off_1[2] = {
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_bom_bom_04_01_off_1 = { 2, btlparty_bom_bom_04_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_bom_bom_04_01_off_2[3] = {
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_bom_bom_04_01_off_2 = { 3, btlparty_bom_bom_04_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_bom_bom_04_02_off_1[2] = {
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_bom_bom_04_02_off_1 = { 2, btlparty_bom_bom_04_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_bom_bom_04_02_off_2[3] = {
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_bllizard_3_data_B7F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_bom_bom_04_02_off_2 = { 3, btlparty_bom_bom_04_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_bom_bom_04_03_off_1[4] = {
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_3_data_CDB0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_bom_bom_04_03_off_1 = { 4, btlparty_bom_bom_04_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_test1_off[2] = {
    { 50, &btlgrp_bom_bom_00_01_off_1, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_03_01_off[3] = {
    { 20, &btlgrp_bom_bom_03_01_off_1, bom_btlstage_data },
    { 10, &btlgrp_bom_bom_03_01_off_2, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_03_02_off[3] = {
    { 10, &btlgrp_bom_bom_03_01_off_1, bom_btlstage_data },
    { 20, &btlgrp_bom_bom_03_01_off_2, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_03_03_off[3] = {
    { 10, &btlgrp_bom_bom_03_03_off_1, bom_btlstage_data },
    { 10, &btlgrp_bom_bom_03_03_off_2, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_04_01_off[3] = {
    { 10, &btlgrp_bom_bom_04_01_off_1, bom_btlstage_data },
    { 10, &btlgrp_bom_bom_04_01_off_2, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_04_02_off[3] = {
    { 10, &btlgrp_bom_bom_04_02_off_1, bom_btlstage_data },
    { 10, &btlgrp_bom_bom_04_02_off_2, bom_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_bom_btlno_bom_04_03_off[2] = {
    { 10, &btlgrp_bom_bom_04_03_off_1, bom_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_bom_tbl[8] = {
    {
        "マップstg_07_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_bom_btlno_bom_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "bom03ブリザード",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_bom_btlno_bom_03_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "bom03ブリザード",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_bom_btlno_bom_03_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "bom03アイスパックン",                                                                                       // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_bom_btlno_bom_03_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "bom04アイスパックン",                                                                                       // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_bom_btlno_bom_04_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "bom04ブリザード",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_bom_btlno_bom_04_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "bom04アイスパックン",                                                                                       // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_bom_btlno_bom_04_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_bom_setup_no_tbl[16] = {
    (u32)"BTLNO_BOM_TEST1", 0x00000000, (u32)"BTLNO_BOM_03_01", 0x00000001,
    (u32)"BTLNO_BOM_03_02", 0x00000002, (u32)"BTLNO_BOM_03_03", 0x00000003,
    (u32)"BTLNO_BOM_04_01", 0x00000004, (u32)"BTLNO_BOM_04_02", 0x00000005,
    (u32)"BTLNO_BOM_04_03", 0x00000006, 0x00000000, 0x00000000,
};

USER_FUNC(_snow_control) {
    if (isFirstCall) {
        effSnowfallN64Entry(0, 0x40);
    }
    return 0;
}

EVT_BEGIN(bgset_bom_snow_event)
    CALL(_snow_control)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
