#include "battle_database_rsh.h"
#include "battle/battle.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_stage.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_moamoa[1];
extern BattleUnitKind unit_boss_moamoa;
s32 bgset_rsh_bg_event[30];

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData rsh_btlobject_data[7] = {
    {"r_mae", 2, 1, {170.0f, 0.0f, -50.0f}, 0, 60},
    {"r_oku", 2, 1, {170.0f, -1000.0f, -50.0f}, 10, 50},
    {"l_mae", 2, 1, {-170.0f, 0.0f, -50.0f}, 20, 40},
    {"l_oku", 2, 1, {-170.0f, -1000.0f, -50.0f}, 30, 30},
    {"iwayama_1", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"iwayama_2", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
};

BattleStageData rsh_btlstage_data[1] = {
    {
        "bti_01",                                   // mGlobalStageDataDir
        "stg_06_0",                                 // mSpecificStageDataDir
        7,                                          // mNumObjects
        &rsh_btlobject_data[0],                     // mObjects
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
        { NULL, NULL, &bgset_rsh_bg_event, NULL },  // layerEvents
        0,                                          // field_0x1B0
        { 0 },                                      // layerDestroyed
    },
};

BattleUnitSetup btlparty_rsh_rsh_06_01_off_1[1] = {
    { &unit_boss_moamoa, 1, 0x04000004, { 70.0f, -20.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_moamoa },
};

BattleGroupSetup btlgrp_rsh_rsh_06_01_off_1 = { 1, btlparty_rsh_rsh_06_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_heart_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_rsh_btlno_rsh_test1_off[2] = {
    { 50, &btlgrp_rsh_rsh_06_01_off_1, rsh_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_rsh_btlno_rsh_06_01_off[2] = {
    { 50, &btlgrp_rsh_rsh_06_01_off_1, rsh_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_rsh_tbl[3] = {
    {
        "マップstg_06_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_rsh_btlno_rsh_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "rsh_06 もあもあ",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_rsh_btlno_rsh_06_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG6_GREAT_MOAMOA1",                                                                      // music
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
u32 btl_rsh_setup_no_tbl[6] = {
    (u32)"BTLNO_RSH_TEST1", 0x00000000, (u32)"BTLNO_RSH_06_01", 0x00000001,
    0x00000000, 0x00000000,
};

s32 _set_offset(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 outIdx = args[1];
    s32 a = evtGetValue(event, outIdx);
    s32 b = evtGetValue(event, args[2]);
    s32 c = evtGetValue(event, args[3]);
    s32 d = evtGetValue(event, args[4]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)id);

    obj->mPositionOffset1.x = c + (d * a) / b;
    a = a - 1;
    if (a < 0) {
        a = b;
    }
    evtSetValue(event, outIdx, a);
    return 2;
}

s32 _set_destroy_flag(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;

    wp->field_180C0.stageFlags |= 0x8;
    return 2;
}

EVT_BEGIN(bgset_rsh_bg_event)
    CALL(_set_destroy_flag)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 200)
    LABEL(0)
    CALL(_set_offset, STRING("iwayama_1"), LW(0), 400, -300, 800)
    CALL(_set_offset, STRING("iwayama_2"), LW(1), 400, -400, 800)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
