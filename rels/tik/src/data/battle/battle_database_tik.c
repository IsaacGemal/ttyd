#include "battle_database_tik.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off
// the original REL was compiled with fp contraction enabled (fmadds in _wave_offset)
#pragma fp_contract on

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_flower_drop_param_default4[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default3[];
extern s32 battle_heart_drop_param_default4[];
extern ItemDropData battle_item_gesso[1];
extern ItemDropData battle_item_hammer_bros[10];
extern ItemDropData battle_item_hannya[10];
extern ItemDropData battle_item_hennya[10];
extern ItemDropData battle_item_hinnya[10];
extern ItemDropData battle_item_kamec[10];
extern ItemDropData battle_item_kuriboo[10];
extern ItemDropData battle_item_kurikuri[1];
extern ItemDropData battle_item_nokonoko[10];
extern ItemDropData battle_item_patakuri[10];
extern ItemDropData battle_item_patapata[10];
extern ItemDropData battle_item_togekuri[10];
extern ItemDropData battle_item_togenoko[10];
extern BattleUnitKind unit_boss_gesso;
extern BattleUnitKind unit_hammer_bros_26_data_240F0;
extern BattleUnitKind unit_hannya_26_data_20220;
extern BattleUnitKind unit_hennya_26_data_30370;
extern BattleUnitKind unit_hinnya_26_data_20DE0;
extern BattleUnitKind unit_kamec;
extern BattleUnitKind unit_kuriboo_26_data_1CBF8;
extern BattleUnitKind unit_lecture_frankli_kuriboo;
extern BattleUnitKind unit_nokonoko_26_data_2DCC8;
extern BattleUnitKind unit_patakuri_26_data_21998;
extern BattleUnitKind unit_patapata_26_data_29498;
extern BattleUnitKind unit_togekuri_26_data_23050;
extern BattleUnitKind unit_togenoko;
static USER_FUNC(_wave_offset);

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData tik_btlobject_data[20] = {
    { "isi_2", 2, 0, { -150.0f, 0.0f, -23.0f }, 10, 50, { 0, 0 } },
    { "isi_3", 2, 0, { -60.0f, 0.0f, -54.0f }, 20, 40, { 0, 0 } },
    { "isi_4", 2, 0, { 176.0f, 0.0f, -62.0f }, 30, 30, { 0, 0 } },
    { "nami_0", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_1", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_2", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_3", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_0", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_1", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_2", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_3", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_0", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "jon_0", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "isi_1", 2, 1, { -202.0f, 0.0f, -17.0f }, 0, 60, { 0, 0 } },
    { "isi_2", 2, 1, { 200.0f, 0.0f, -30.0f }, 10, 50, { 0, 0 } },
    { "A1", 2, 0, { 0.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "B_mati", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

BattleStageData tik_btlstage_data[4] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_00_2",                                 // mSpecificStageDataDir
        8,                                          // mNumObjects
        &tik_btlobject_data[0],                     // mObjects
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
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 1, 1, 1 },                                // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_00_2",                                 // mSpecificStageDataDir
        5,                                          // mNumObjects
        &tik_btlobject_data[8],                     // mObjects
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
        0,                                          // field_0x1B0
        { 0 },                                      // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_00_3",                                 // mSpecificStageDataDir
        2,                                          // mNumObjects
        &tik_btlobject_data[13],                    // mObjects
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
        NULL,                                       // initEvent
        NULL,                                       // objectTurnEventA1
        NULL,                                       // objectTurnEventA2
        NULL,                                       // objectTurnEventB
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 0 },                                      // layerDestroyed
    },
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_00_1",                                 // mSpecificStageDataDir
        5,                                          // mNumObjects
        &tik_btlobject_data[15],                    // mObjects
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
        { 0 },                                      // layerEvents
        0,                                          // field_0x1B0
        { 0 },                                      // layerDestroyed
    },
};

BattleUnitSetup btlparty_tik_tik_gesso[1] = {
    { &unit_boss_gesso, 1, 0x04000004, { 122.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gesso },
};

BattleGroupSetup btlgrp_tik_tik_gesso = { 1, btlparty_tik_tik_gesso, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_01_01_off_1[4] = {
    { &unit_kuriboo_26_data_1CBF8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kurikuri },
    { &unit_togekuri_26_data_23050, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_patakuri_26_data_21998, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_lecture_frankli_kuriboo, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_tik_tik_01_01_off_1 = { 4, btlparty_tik_tik_01_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_01_02_off_1[2] = {
    { &unit_togekuri_26_data_23050, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_togekuri_26_data_23050, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_tik_tik_01_02_off_1 = { 2, btlparty_tik_tik_01_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_01_off_1[1] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_01_off_1 = { 1, btlparty_tik_tik_04_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_01_on_1[1] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_01_on_1 = { 1, btlparty_tik_tik_04_01_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_01_on_2[2] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_01_on_2 = { 2, btlparty_tik_tik_04_01_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_02_off_1[2] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_02_off_1 = { 2, btlparty_tik_tik_04_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_02_on_1[2] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_02_on_1 = { 2, btlparty_tik_tik_04_02_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_02_on_2[3] = {
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_04_02_on_2 = { 3, btlparty_tik_tik_04_02_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_03_off_1[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_patapata_26_data_29498, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_tik_tik_04_03_off_1 = { 2, btlparty_tik_tik_04_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_04_03_off_2[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_04_03_off_2 = { 2, btlparty_tik_tik_04_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_04_03_off_3[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_04_03_off_3 = { 2, btlparty_tik_tik_04_03_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_04_04_off_1[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_togekuri_26_data_23050, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_tik_tik_04_04_off_1 = { 2, btlparty_tik_tik_04_04_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_04_off_2[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_04_04_off_2 = { 2, btlparty_tik_tik_04_04_off_2, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_04_04_off_3[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_04_04_off_3 = { 2, btlparty_tik_tik_04_04_off_3, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_01_off_1[3] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_07_01_off_1 = { 3, btlparty_tik_tik_07_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_01_off_2[2] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_07_01_off_2 = { 2, btlparty_tik_tik_07_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_02_off_1[2] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_07_02_off_1 = { 2, btlparty_tik_tik_07_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_02_off_2[3] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_07_02_off_2 = { 3, btlparty_tik_tik_07_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_03_off_1[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_nokonoko_26_data_2DCC8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_tik_tik_07_03_off_1 = { 2, btlparty_tik_tik_07_03_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_03_off_2[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_07_03_off_2 = { 2, btlparty_tik_tik_07_03_off_2, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_03_off_3[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
};

BattleGroupSetup btlgrp_tik_tik_07_03_off_3 = { 2, btlparty_tik_tik_07_03_off_3, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_07_04_off_1[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_07_04_off_1 = { 2, btlparty_tik_tik_07_04_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_07_04_off_2[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
};

BattleGroupSetup btlgrp_tik_tik_07_04_off_2 = { 2, btlparty_tik_tik_07_04_off_2, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_07_04_off_3[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_07_04_off_3 = { 2, btlparty_tik_tik_07_04_off_3, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_15_01_off_1[1] = {
    { &unit_kuriboo_26_data_1CBF8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_tik_tik_15_01_off_1 = { 1, btlparty_tik_tik_15_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_15_02_off_1[1] = {
    { &unit_patakuri_26_data_21998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_tik_tik_15_02_off_1 = { 1, btlparty_tik_tik_15_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_15_03_off_1[1] = {
    { &unit_togekuri_26_data_23050, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_tik_tik_15_03_off_1 = { 1, btlparty_tik_tik_15_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_15_04_off_1[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_kuriboo_26_data_1CBF8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_tik_tik_15_04_off_1 = { 2, btlparty_tik_tik_15_04_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_15_04_off_2[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
};

BattleGroupSetup btlgrp_tik_tik_15_04_off_2 = { 2, btlparty_tik_tik_15_04_off_2, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_15_04_off_3[2] = {
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_15_04_off_3 = { 2, btlparty_tik_tik_15_04_off_3, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_tik_tik_15_05_off_1[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_patakuri_26_data_21998, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_tik_tik_15_05_off_1 = { 2, btlparty_tik_tik_15_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_15_05_off_2[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_15_05_off_2 = { 2, btlparty_tik_tik_15_05_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_15_05_off_3[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_hammer_bros_26_data_240F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hammer_bros },
};

BattleGroupSetup btlgrp_tik_tik_15_05_off_3 = { 2, btlparty_tik_tik_15_05_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_18_01_off_1[2] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_18_01_off_1 = { 2, btlparty_tik_tik_18_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_01_off_2[3] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_18_01_off_2 = { 3, btlparty_tik_tik_18_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_02_off_1[4] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_18_02_off_1 = { 4, btlparty_tik_tik_18_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_02_off_2[2] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_26_data_20DE0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_tik_tik_18_02_off_2 = { 2, btlparty_tik_tik_18_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_03_off_1[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_patakuri_26_data_21998, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_tik_tik_18_03_off_1 = { 2, btlparty_tik_tik_18_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_18_03_off_2[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_18_03_off_2 = { 2, btlparty_tik_tik_18_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_18_03_off_3[2] = {
    { &unit_kamec, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_18_03_off_3 = { 2, btlparty_tik_tik_18_03_off_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_tik_tik_18_04_off_1[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_18_04_off_1 = { 2, btlparty_tik_tik_18_04_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_04_off_2[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_togenoko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
};

BattleGroupSetup btlgrp_tik_tik_18_04_off_2 = { 2, btlparty_tik_tik_18_04_off_2, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_18_04_off_3[2] = {
    { &unit_togenoko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko },
    { &unit_kamec, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kamec },
};

BattleGroupSetup btlgrp_tik_tik_18_04_off_3 = { 2, btlparty_tik_tik_18_04_off_3, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_01_off_1[5] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_01_off_1 = { 5, btlparty_tik_tik_20_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_02_off_1[4] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_02_off_1 = { 4, btlparty_tik_tik_20_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_03_off_1[3] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_03_off_1 = { 3, btlparty_tik_tik_20_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_04_off_1[4] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_04_off_1 = { 4, btlparty_tik_tik_20_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_05_off_1[5] = {
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_05_off_1 = { 5, btlparty_tik_tik_20_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_tik_tik_20_06_off_1[5] = {
    { &unit_hennya_26_data_30370, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_26_data_20220, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_tik_tik_20_06_off_1 = { 5, btlparty_tik_tik_20_06_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_test1_off[2] = {
    { 50, &btlgrp_tik_tik_01_01_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_test2_off[2] = {
    { 50, &btlgrp_tik_tik_01_01_off_1, tik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_test3_off[2] = {
    { 50, &btlgrp_tik_tik_01_01_off_1, &tik_btlstage_data[2] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_gesso_off[2] = {
    { 50, &btlgrp_tik_tik_gesso, tik_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_01_01_off[2] = {
    { 50, &btlgrp_tik_tik_01_01_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_01_02_off[2] = {
    { 50, &btlgrp_tik_tik_01_02_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_01_on[3] = {
    { 20, &btlgrp_tik_tik_04_01_on_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_01_on_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_01_off[2] = {
    { 20, &btlgrp_tik_tik_04_01_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_02_on[3] = {
    { 20, &btlgrp_tik_tik_04_02_on_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_02_on_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_02_off[2] = {
    { 20, &btlgrp_tik_tik_04_02_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_03_off[4] = {
    { 20, &btlgrp_tik_tik_04_03_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_03_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_03_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_04_04_off[4] = {
    { 20, &btlgrp_tik_tik_04_04_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_04_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_04_04_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_07_01_off[3] = {
    { 20, &btlgrp_tik_tik_07_01_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_01_off_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_07_02_off[3] = {
    { 20, &btlgrp_tik_tik_07_02_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_02_off_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_07_03_off[4] = {
    { 20, &btlgrp_tik_tik_07_03_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_03_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_03_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_07_04_off[4] = {
    { 20, &btlgrp_tik_tik_07_04_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_04_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_07_04_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_15_01_off[2] = {
    { 20, &btlgrp_tik_tik_15_01_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_15_02_off[2] = {
    { 20, &btlgrp_tik_tik_15_02_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_15_03_off[2] = {
    { 20, &btlgrp_tik_tik_15_03_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_15_04_off[4] = {
    { 20, &btlgrp_tik_tik_15_04_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_15_04_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_15_04_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_15_05_off[4] = {
    { 20, &btlgrp_tik_tik_15_05_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_15_05_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_15_05_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_18_01_off[3] = {
    { 20, &btlgrp_tik_tik_18_01_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_01_off_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_18_02_off[3] = {
    { 20, &btlgrp_tik_tik_18_02_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_02_off_2, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_18_03_off[4] = {
    { 20, &btlgrp_tik_tik_18_03_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_03_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_03_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_18_04_off[4] = {
    { 20, &btlgrp_tik_tik_18_04_off_1, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_04_off_2, &tik_btlstage_data[3] },
    { 20, &btlgrp_tik_tik_18_04_off_3, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_01_off[2] = {
    { 20, &btlgrp_tik_tik_20_01_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_02_off[2] = {
    { 20, &btlgrp_tik_tik_20_02_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_03_off[2] = {
    { 20, &btlgrp_tik_tik_20_03_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_04_off[2] = {
    { 20, &btlgrp_tik_tik_20_04_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_05_off[2] = {
    { 20, &btlgrp_tik_tik_20_05_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_tik_btlno_tik_20_06_off[2] = {
    { 20, &btlgrp_tik_tik_20_06_off_1, &tik_btlstage_data[3] },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_tik_tbl[30] = {
    {
        "マップstg_00_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_00_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_00_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik12ゲッソー",                                                                                        // name
        "tik_02",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_gesso_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "tik01クリボー３人衆",                                                                                     // name
        "tik_01",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_01_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik01トゲクリボー",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_01_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik04ヒンニャー1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129998754,                                                                                         // flag
        btlgrpsel_tik_btlno_tik_04_01_on,                                                                   // onLoadouts
        btlgrpsel_tik_btlno_tik_04_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik04ヒンニャー2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -129998754,                                                                                         // flag
        btlgrpsel_tik_btlno_tik_04_02_on,                                                                   // onLoadouts
        btlgrpsel_tik_btlno_tik_04_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik04カメック",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_04_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik04トゲノコ",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_04_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik07ハンニャー1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_07_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik07ハンニャー2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_07_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik07トゲノコ",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_07_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik07ハンマーブロス",                                                                                       // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_07_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik15クリボー",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_15_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik15パタクリボー",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_15_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik15トゲクリボー",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_15_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik15ハンマーブロス",                                                                                       // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_tik_btlno_tik_15_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "tik15カメック",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_15_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik18ハンニャー1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_18_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik18ハンニャー2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_18_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik18カメック",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_18_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik18トゲノコ",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_18_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ハンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ハンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ハンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ハンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ハンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "tik20ヘンニャー",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_tik_btlno_tik_20_06_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_tik_setup_no_tbl[60] = {
    (u32)"BTLNO_TIK_TEST1", 0x00000000, (u32)"BTLNO_TIK_TEST2", 0x00000001,
    (u32)"BTLNO_TIK_TEST3", 0x00000002, (u32)"BTLNO_TIK_GESSO", 0x00000003,
    (u32)"BTLNO_TIK_01_01", 0x00000004, (u32)"BTLNO_TIK_01_02", 0x00000005,
    (u32)"BTLNO_TIK_04_01", 0x00000006, (u32)"BTLNO_TIK_04_02", 0x00000007,
    (u32)"BTLNO_TIK_04_03", 0x00000008, (u32)"BTLNO_TIK_04_04", 0x00000009,
    (u32)"BTLNO_TIK_07_01", 0x0000000A, (u32)"BTLNO_TIK_07_02", 0x0000000B,
    (u32)"BTLNO_TIK_07_03", 0x0000000C, (u32)"BTLNO_TIK_07_04", 0x0000000D,
    (u32)"BTLNO_TIK_15_01", 0x0000000E, (u32)"BTLNO_TIK_15_02", 0x0000000F,
    (u32)"BTLNO_TIK_15_03", 0x00000010, (u32)"BTLNO_TIK_15_04", 0x00000011,
    (u32)"BTLNO_TIK_15_05", 0x00000012, (u32)"BTLNO_TIK_18_01", 0x00000013,
    (u32)"BTLNO_TIK_18_02", 0x00000014, (u32)"BTLNO_TIK_18_03", 0x00000015,
    (u32)"BTLNO_TIK_18_04", 0x00000016, (u32)"BTLNO_TIK_20_01", 0x00000017,
    (u32)"BTLNO_TIK_20_02", 0x00000018, (u32)"BTLNO_TIK_20_03", 0x00000019,
    (u32)"BTLNO_TIK_20_04", 0x0000001A, (u32)"BTLNO_TIK_20_05", 0x0000001B,
    (u32)"BTLNO_TIK_20_06", 0x0000001C, 0x00000000, 0x00000000,
};

static USER_FUNC(_wave_offset) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 a1 = args[1];
    s32 v1 = evtGetValue(event, a1);
    s32 v2 = evtGetValue(event, args[2]);
    s32 v3 = evtGetValue(event, args[3]);
    s32 a4 = args[4];
    s32 v4 = evtGetValue(event, a4);
    s32 v5 = evtGetValue(event, args[5]);
    s32 v6 = evtGetValue(event, args[6]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)name);
    s32 next1;
    s32 next4;

    obj->mPositionOffset1.x = v3 * sinfd((f32)(v1 * 360 / v2));
    obj->mPositionOffset1.y = v6 * sinfd((f32)(v4 * 360 / v5 + 270)) + v6;

    next1 = v1 + 1;
    if (next1 >= v2) {
        next1 = 0;
    }
    evtSetValue(event, a1, next1);

    next4 = v4 + 1;
    if (next4 >= v5) {
        next4 = 0;
    }
    evtSetValue(event, a4, next4);

    return EVT_RETURN_DONE;
}

EVT_BEGIN(bgset_tik_wave_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    LABEL(0)
    CALL(_wave_offset, STRING("nami_1"), LW(2), 600, 20, LW(3), 250, 6)
    CALL(_wave_offset, STRING("nami_2"), LW(4), 1100, 40, LW(5), 360, 7)
    CALL(_wave_offset, STRING("nami_3"), LW(6), 1250, 50, LW(7), 450, 8)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
