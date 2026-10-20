#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_purple_teresa[10];
extern s32 unit_purple_teresa_22_data_E620[];

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

BattleStageObjectData pik_btlobject_data[2] = {
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A2", 2, 1, { 0.0f, 0.0f, -74.0f }, 0, 60, { 0, 0 } },
};

BattleStageData pik_btlstage_data[1] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_06_4",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &pik_btlobject_data[0],                     // mObjects
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
    },
};

BattleUnitSetup btlparty_pik_pik_06_01_off_1[2] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_01_off_1 = { 2, btlparty_pik_pik_06_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_02_off_1[3] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_02_off_1 = { 3, btlparty_pik_pik_06_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_03_off_1[1] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_03_off_1 = { 1, btlparty_pik_pik_06_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_04_off_1[2] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_04_off_1 = { 2, btlparty_pik_pik_06_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_05_off_1[2] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_05_off_1 = { 2, btlparty_pik_pik_06_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_06_off_1[4] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_06_off_1 = { 4, btlparty_pik_pik_06_06_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_07_off_1[1] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_07_off_1 = { 1, btlparty_pik_pik_06_07_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_08_off_1[3] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_08_off_1 = { 3, btlparty_pik_pik_06_08_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_pik_pik_06_09_off_1[4] = {
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_22_data_E620, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_pik_pik_06_09_off_1 = { 4, btlparty_pik_pik_06_09_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_test1_off[2] = {
    { 50, &btlgrp_pik_pik_06_01_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_01_off[2] = {
    { 20, &btlgrp_pik_pik_06_01_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_02_off[2] = {
    { 20, &btlgrp_pik_pik_06_02_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_03_off[2] = {
    { 20, &btlgrp_pik_pik_06_03_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_04_off[2] = {
    { 20, &btlgrp_pik_pik_06_04_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_05_off[2] = {
    { 20, &btlgrp_pik_pik_06_05_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_06_off[2] = {
    { 20, &btlgrp_pik_pik_06_06_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_07_off[2] = {
    { 20, &btlgrp_pik_pik_06_07_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_08_off[2] = {
    { 20, &btlgrp_pik_pik_06_08_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_pik_btlno_pik_03_09_off[2] = {
    { 20, &btlgrp_pik_pik_06_09_off_1, pik_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_pik_tbl[11] = {
    {
        "マップstg_06_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_pik_btlno_pik_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_05_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_06_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_07_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_08_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "pik_03パープルテレサ",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_pik_btlno_pik_03_09_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_pik_setup_no_tbl[22] = {
    (u32)"BTLNO_PIK_TEST1", 0x00000000, (u32)"BTLNO_PIK_03_01", 0x00000001,
    (u32)"BTLNO_PIK_03_02", 0x00000002, (u32)"BTLNO_PIK_03_03", 0x00000003,
    (u32)"BTLNO_PIK_03_04", 0x00000004, (u32)"BTLNO_PIK_03_05", 0x00000005,
    (u32)"BTLNO_PIK_03_06", 0x00000006, (u32)"BTLNO_PIK_03_07", 0x00000007,
    (u32)"BTLNO_PIK_03_08", 0x00000008, (u32)"BTLNO_PIK_03_09", 0x00000009,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
