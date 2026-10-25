#include "battle_database_tou.h"
#include "battle/battle_stage.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_basabasa[10];
extern ItemDropData battle_item_bllizard[9];
extern ItemDropData battle_item_bomhei[10];
extern ItemDropData battle_item_boomerang_bros[1];
extern ItemDropData battle_item_borodo[10];
extern ItemDropData battle_item_borodo_king[1];
extern ItemDropData battle_item_burst_wanwan[7];
extern ItemDropData battle_item_champion[1];
extern ItemDropData battle_item_dokugassun[9];
extern ItemDropData battle_item_fire_bros[1];
extern ItemDropData battle_item_hammer_bros[10];
extern ItemDropData battle_item_hyper_sinnosuke[7];
extern ItemDropData battle_item_hyper_togemet[7];
extern ItemDropData battle_item_iron_sinemon[1];
extern ItemDropData battle_item_iron_sinemon2[1];
extern ItemDropData battle_item_kamec[10];
extern ItemDropData battle_item_kurikuri[1];
extern ItemDropData battle_item_kurokumorn[9];
extern ItemDropData battle_item_macho_gance[1];
extern ItemDropData battle_item_monban[1];
extern ItemDropData battle_item_nokonoko_fighter[1];
extern ItemDropData battle_item_patapata_fighter[1];
extern ItemDropData battle_item_sambo[7];
extern ItemDropData battle_item_togezo[8];
extern ItemDropData battle_item_ura_noko[10];
extern ItemDropData battle_item_ura_pata[10];
extern s32 tou_gamen_screen_tev_init[];
extern BattleUnitKind unit_basabasa_28_data_16B60;
extern BattleUnitKind unit_bomhei_28_data_17C48;
extern BattleUnitKind unit_boomerang_bros;
extern BattleUnitKind unit_borodo_28_data_1AEF0;
extern BattleUnitKind unit_borodo_king;
extern BattleUnitKind unit_boss_champion;
extern BattleUnitKind unit_boss_koopa_tou;
extern BattleUnitKind unit_boss_macho_gance;
extern BattleUnitKind unit_burst_wanwan_28_data_4AED8;
extern BattleUnitKind unit_chorobon_28_data_1C808;
extern BattleUnitKind unit_crimson_togemet;
extern BattleUnitKind unit_dark_keeper_28_data_1DE40;
extern BattleUnitKind unit_fire_bros;
extern BattleUnitKind unit_flower_chorobon_28_data_21138;
extern BattleUnitKind unit_green_chorobon_28_data_22E58;
extern BattleUnitKind unit_hammer_bros_28_data_24A10;
extern BattleUnitKind unit_hannya_28_data_3BF68;
extern BattleUnitKind unit_hinnya_28_data_27050;
extern BattleUnitKind unit_honenoko_28_data_27C08;
extern BattleUnitKind unit_hyper_sinnosuke;
extern BattleUnitKind unit_iron_sinemon;
extern BattleUnitKind unit_iron_sinemon2;
extern BattleUnitKind unit_jyugem_28_data_2B678;
extern BattleUnitKind unit_kamec_green;
extern BattleUnitKind unit_kamec_red;
extern BattleUnitKind unit_kamec_white;
extern BattleUnitKind unit_kurikuri;
extern BattleUnitKind unit_monochrome_kurokumorn_28_data_59A08;
extern BattleUnitKind unit_monochrome_pakkun_28_data_5B1C8;
extern BattleUnitKind unit_nokonoko_fighter;
extern BattleUnitKind unit_patapata_fighter;
extern BattleUnitKind unit_piders_28_data_2E140;
extern BattleUnitKind unit_sambo_28_data_30280;
extern BattleUnitKind unit_togedaruma_28_data_34998;
extern BattleUnitKind unit_togenoko_ace_28_data_48988;
extern BattleUnitKind unit_togezo_28_data_35BD0;
extern BattleUnitKind unit_ura_noko_28_data_37580;
extern BattleUnitKind unit_ura_pata;
s32 bgset_tou_screen_evt[22];

// clang-format off
extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData tou_btlobject_data[14] = {
    { "gamen", 2, 0, { 0.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "etc", 2, 0, { 0.0f, 0.0f, -54.0f }, 10, 50, { 0, 0 } },
    { "side__L", 2, 0, { -137.0f, 0.0f, -40.0f }, 20, 40, { 0, 0 } },
    { "side__R", 2, 0, { 137.0f, 0.0f, -40.0f }, 30, 30, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "gamen", 2, 0, { 0.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "etc", 2, 0, { 0.0f, 0.0f, -54.0f }, 10, 50, { 0, 0 } },
    { "side__L", 2, 0, { -137.0f, 0.0f, -40.0f }, 20, 40, { 0, 0 } },
    { "side__R", 2, 0, { 137.0f, 0.0f, -40.0f }, 30, 30, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "if_wire", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
    { "if_body", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
};

BattleStageData tou_btlstage_data[2] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_03_0",                                 // mSpecificStageDataDir
        6,                                          // mNumObjects
        &tou_btlobject_data[0],                     // mObjects
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
        { NULL, NULL, NULL, &bgset_tou_screen_evt }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_03_0",                                 // mSpecificStageDataDir
        8,                                          // mNumObjects
        &tou_btlobject_data[6],                     // mObjects
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
        { NULL, NULL, NULL, &bgset_tou_screen_evt }, // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
};

BattleUnitSetup btlparty_tou_tou_rank_19[5] = {
    { &unit_kurikuri, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurikuri },
    { &unit_kurikuri, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurikuri },
    { &unit_kurikuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_kurikuri },
    { &unit_kurikuri, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurikuri },
    { &unit_kurikuri, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurikuri },
};

BattleGroupSetup btlgrp_tou_tou_rank_19 = { 5, btlparty_tou_tou_rank_19, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_18[3] = {
    { &unit_nokonoko_fighter, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko_fighter },
    { &unit_nokonoko_fighter, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko_fighter },
    { &unit_patapata_fighter, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_patapata_fighter },
};

BattleGroupSetup btlgrp_tou_tou_rank_18 = { 3, btlparty_tou_tou_rank_18, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_17[3] = {
    { &unit_sambo_28_data_30280, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_28_data_30280, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_28_data_30280, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_sambo },
};

BattleGroupSetup btlgrp_tou_tou_rank_17 = { 3, btlparty_tou_tou_rank_17, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_16[3] = {
    { &unit_honenoko_28_data_27C08, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_honenoko_28_data_27C08, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_honenoko_28_data_27C08, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_bomhei },
};

BattleGroupSetup btlgrp_tou_tou_rank_16 = { 3, btlparty_tou_tou_rank_16, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_15[3] = {
    { &unit_togezo_28_data_35BD0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurokumorn },
    { &unit_togezo_28_data_35BD0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_jyugem_28_data_2B678, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_tou_tou_rank_15 = { 3, btlparty_tou_tou_rank_15, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_14[3] = {
    { &unit_monochrome_pakkun_28_data_5B1C8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togezo },
    { &unit_monochrome_kurokumorn_28_data_59A08, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togezo },
    { &unit_piders_28_data_2E140, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_togezo },
};

BattleGroupSetup btlgrp_tou_tou_rank_14 = { 3, btlparty_tou_tou_rank_14, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_13[3] = {
    { &unit_hyper_sinnosuke, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinnosuke },
    { &unit_hyper_sinnosuke, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinnosuke },
    { &unit_hyper_sinnosuke, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_hyper_sinnosuke },
};

BattleGroupSetup btlgrp_tou_tou_rank_13 = { 3, btlparty_tou_tou_rank_13, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_12[4] = {
    { &unit_bomhei_28_data_17C48, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_28_data_17C48, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_28_data_17C48, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_28_data_17C48, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
};

BattleGroupSetup btlgrp_tou_tou_rank_12 = { 4, btlparty_tou_tou_rank_12, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_11[4] = {
    { &unit_borodo_28_data_1AEF0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_28_data_1AEF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_king, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo_king },
    { &unit_borodo_king, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo_king },
};

BattleGroupSetup btlgrp_tou_tou_rank_11 = { 4, btlparty_tou_tou_rank_11, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_10[2] = {
    { &unit_iron_sinemon, 1, 0x04000004, { 80.0f, 0.0f, 0.0f }, 0, { 50, 0, 0, 0 }, battle_item_iron_sinemon },
    { &unit_iron_sinemon2, 1, 0x04000004, { 150.0f, 0.0f, 10.0f }, 0, { 130, 0, 0, 0 }, battle_item_iron_sinemon2 },
};

BattleGroupSetup btlgrp_tou_tou_rank_10 = { 2, btlparty_tou_tou_rank_10, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_9[2] = {
    { &unit_crimson_togemet, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togemet },
    { &unit_crimson_togemet, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_togemet },
};

BattleGroupSetup btlgrp_tou_tou_rank_9 = { 2, btlparty_tou_tou_rank_9, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_8[2] = {
    { &unit_togedaruma_28_data_34998, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_togedaruma_28_data_34998, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tou_tou_rank_8 = { 2, btlparty_tou_tou_rank_8, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_7[3] = {
    { &unit_ura_noko_28_data_37580, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_pata, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_noko_28_data_37580, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_ura_pata },
};

BattleGroupSetup btlgrp_tou_tou_rank_7 = { 3, btlparty_tou_tou_rank_7, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_6[3] = {
    { &unit_chorobon_28_data_1C808, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_green_chorobon_28_data_22E58, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_flower_chorobon_28_data_21138, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tou_tou_rank_6 = { 3, btlparty_tou_tou_rank_6, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_5[3] = {
    { &unit_kamec_red, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_kamec_white, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_fire_bros },
    { &unit_kamec_green, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_boomerang_bros },
};

BattleGroupSetup btlgrp_tou_tou_rank_5 = { 3, btlparty_tou_tou_rank_5, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_4[1] = {
    { &unit_dark_keeper_28_data_1DE40, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tou_tou_rank_4 = { 1, btlparty_tou_tou_rank_4, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_3[3] = {
    { &unit_hammer_bros_28_data_24A10, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_fire_bros, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_boomerang_bros, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
};

BattleGroupSetup btlgrp_tou_tou_rank_3 = { 3, btlparty_tou_tou_rank_3, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_2[2] = {
    { &unit_burst_wanwan_28_data_4AED8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
    { &unit_burst_wanwan_28_data_4AED8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
};

BattleGroupSetup btlgrp_tou_tou_rank_2 = { 2, btlparty_tou_tou_rank_2, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_rank_1[1] = {
    { &unit_togenoko_ace_28_data_48988, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tou_tou_rank_1 = { 1, btlparty_tou_tou_rank_1, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_add_1[4] = {
    { &unit_basabasa_28_data_16B60, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_28_data_16B60, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_28_data_16B60, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
    { &unit_basabasa_28_data_16B60, 1, 0x04000004, { 150.0f, 130.0f, 20.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa },
};

BattleGroupSetup btlgrp_tou_tou_add_1 = { 4, btlparty_tou_tou_add_1, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_add_2[4] = {
    { &unit_hinnya_28_data_27050, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monban },
    { &unit_hannya_28_data_3BF68, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monban },
    { &unit_hinnya_28_data_27050, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monban },
    { &unit_hannya_28_data_3BF68, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tou_tou_add_2 = { 4, btlparty_tou_tou_add_2, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_boss[1] = {
    { &unit_boss_macho_gance, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_macho_gance },
};

BattleGroupSetup btlgrp_tou_tou_boss = { 1, btlparty_tou_tou_boss, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_champ[1] = {
    { &unit_boss_champion, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_champion },
};

BattleGroupSetup btlgrp_tou_tou_champ = { 1, btlparty_tou_tou_champ, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

BattleUnitSetup btlparty_tou_tou_koopa[1] = {
    { &unit_boss_koopa_tou, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tou_tou_koopa = { 1, btlparty_tou_tou_koopa, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_test1_off[2] = {
    { 50, &btlgrp_tou_tou_rank_19, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_01_off[2] = {
    { 50, &btlgrp_tou_tou_rank_19, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_02_off[2] = {
    { 50, &btlgrp_tou_tou_rank_18, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_03_off[2] = {
    { 50, &btlgrp_tou_tou_rank_17, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_04_off[2] = {
    { 50, &btlgrp_tou_tou_rank_16, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_05_off[2] = {
    { 50, &btlgrp_tou_tou_rank_15, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_06_off[2] = {
    { 50, &btlgrp_tou_tou_rank_14, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_07_off[2] = {
    { 50, &btlgrp_tou_tou_rank_13, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_08_off[2] = {
    { 50, &btlgrp_tou_tou_rank_12, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_09_off[2] = {
    { 50, &btlgrp_tou_tou_rank_11, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_10_off[2] = {
    { 50, &btlgrp_tou_tou_rank_10, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_11_off[2] = {
    { 50, &btlgrp_tou_tou_rank_9, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_12_off[2] = {
    { 50, &btlgrp_tou_tou_rank_8, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_13_off[2] = {
    { 50, &btlgrp_tou_tou_rank_7, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_14_off[2] = {
    { 50, &btlgrp_tou_tou_rank_6, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_15_off[2] = {
    { 50, &btlgrp_tou_tou_rank_5, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_16_off[2] = {
    { 50, &btlgrp_tou_tou_rank_4, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_17_off[2] = {
    { 50, &btlgrp_tou_tou_rank_3, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_18_off[2] = {
    { 50, &btlgrp_tou_tou_rank_2, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_19_off[2] = {
    { 50, &btlgrp_tou_tou_rank_1, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_20_off[2] = {
    { 50, &btlgrp_tou_tou_add_1, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_21_off[2] = {
    { 50, &btlgrp_tou_tou_add_2, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_22_off[2] = {
    { 50, &btlgrp_tou_tou_champ, &tou_btlstage_data[1] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_23_off[2] = {
    { 50, &btlgrp_tou_tou_boss, tou_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tou_btlno_tou_01_24_off[2] = {
    { 50, &btlgrp_tou_tou_koopa, tou_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_tou_tbl[26] = {
    {
        "マップstg_03_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "クリクリブラザーズ",                                                                                          // name
        "tou_03",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tou_btlno_tou_01_01_off,                                                                    // offLoadouts
        64,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "コウラーズ",                                                                                              // name
        "tou_03",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tou_btlno_tou_01_02_off,                                                                    // offLoadouts
        64,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "サンボ三兄弟",                                                                                           // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_03_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ボンバーズ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_04_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "クモーンズ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_05_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ジュゲムとイタい仲間たち",                                                                                     // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_06_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ハイパーズ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_07_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ボロドーズ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_08_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "クッパぐんだんズ",                                                                                         // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_09_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "こうてつウォリアーズ",                                                                                       // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_10_off,                                                                  // offLoadouts
        536870976,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "ザ・トゲトゲン",                                                                                          // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_11_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミトゲダルマー",                                                                                         // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_12_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ザ・ウラウラン",                                                                                            // name
        "tou_03",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tou_btlno_tou_01_13_off,                                                                    // offLoadouts
        64,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "レッドデビルズ",                                                                                          // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_14_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "トリプルブロス",                                                                                            // name
        "tou_03",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tou_btlno_tou_01_15_off,                                                                    // offLoadouts
        64,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ザ・マホーンズ",                                                                                          // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_16_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "キラーボムヘイズ",                                                                                         // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_17_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "バーストワンワン",                                                                                         // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_18_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "トゲノコエース",                                                                                            // name
        "tou_03",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tou_btlno_tou_01_19_off,                                                                    // offLoadouts
        64,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ナイトウィングス",                                                                                         // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_20_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "スネイルズ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_21_off,                                                                  // offLoadouts
        64,                                                                                                 // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ブレッドハート",                                                                                          // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_22_off,                                                                  // offLoadouts
        536870976,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_CHAMP1",                                                                             // music
    },
    {
        "マッチョガンス",                                                                                          // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_23_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_GANCE1",                                                                             // music
    },
    {
        "乱入クッパ",                                                                                            // name
        "tou_03",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tou_btlno_tou_01_24_off,                                                                  // offLoadouts
        536870912,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG8_KOOPA",                                                                              // music
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
u32 btl_tou_setup_no_tbl[52] = {
    (u32)"BTLNO_TOU_TEST1", 0x00000000, (u32)"BTLNO_TOU_01_01", 0x00000001,
    (u32)"BTLNO_TOU_01_02", 0x00000002, (u32)"BTLNO_TOU_01_03", 0x00000003,
    (u32)"BTLNO_TOU_01_04", 0x00000004, (u32)"BTLNO_TOU_01_05", 0x00000005,
    (u32)"BTLNO_TOU_01_06", 0x00000006, (u32)"BTLNO_TOU_01_07", 0x00000007,
    (u32)"BTLNO_TOU_01_08", 0x00000008, (u32)"BTLNO_TOU_01_09", 0x00000009,
    (u32)"BTLNO_TOU_01_10", 0x0000000A, (u32)"BTLNO_TOU_01_11", 0x0000000B,
    (u32)"BTLNO_TOU_01_12", 0x0000000C, (u32)"BTLNO_TOU_01_13", 0x0000000D,
    (u32)"BTLNO_TOU_01_14", 0x0000000E, (u32)"BTLNO_TOU_01_15", 0x0000000F,
    (u32)"BTLNO_TOU_01_16", 0x00000010, (u32)"BTLNO_TOU_01_17", 0x00000011,
    (u32)"BTLNO_TOU_01_18", 0x00000012, (u32)"BTLNO_TOU_01_19", 0x00000013,
    (u32)"BTLNO_TOU_01_20", 0x00000014, (u32)"BTLNO_TOU_01_21", 0x00000015,
    (u32)"BTLNO_TOU_01_22", 0x00000016, (u32)"BTLNO_TOU_01_23", 0x00000017,
    (u32)"BTLNO_TOU_01_24", 0x00000018, 0x00000000, 0x00000000,
};

EVT_BEGIN(bgset_tou_screen_evt)
    CALL(tou_gamen_screen_tev_init)
    WAIT_FRAMES(1)
    CALL(evt_map_set_tevcallback, 8, PTR(tou_gamen_screen_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gamen"), 134217728)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gamen"), 256)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
