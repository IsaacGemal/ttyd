#include "unit_boss_gundan_zako_group1.h"
#include "unit_boss_gundan_zako_group2.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"
#include "driver/extdrv.h"

#pragma pool_data off

#pragma force_active on
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[4];
static BattleUnitKindPart parts[1];
static s32 init_event[44];
static s32 damage_event[134];
s32 damage_event_kaiten_hammer[30];
static s32 dead_event[138];
static s32 attack_event[85];
static s32 hit_check_event[95];
static s32 wait_event[6];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_boss_gundan_zako_group1 = {
    UNIT_X_NAUTS_CRUMP_FORMATION1, // mUnitType
    "btl_un_gundan_zako_group1",   // name
    6,                             // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    0,                             // mLevel
    0,                             // mBonusExp
    0,                             // bonusCoinRolls
    100,                           // bonusCoinRate
    0,                             // baseCoin
    50,                            // escapeRate
    9999,                          // renzokuSoftCap
    50,                            // width
    50,                            // height
    { 0, 50 },                     // statusIconOffset
    { 0.0f, 0.0f, 0.0f },          // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },          // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },       // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // kagenukeOffsetX
    0.0f,                          // kagenukeOffsetZ
    { 25.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 25.0f, 32.5f, 0.0f },        // kissHitOffset
    { -4.0f, 25.0f, 0.0f },        // cutBaseOffset
    50.0f,                         // cutWidth
    50.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    -1,                            // swallowRate
    0,                             // swallowAttribute
    100,                           // shotDamageRate
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_BOSS_TEAM_3RD_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0x00200000,                    // attributeFlags
    &regist,                       // vulnStatus
    1,                             // numParts
    parts,                         // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0, // sleepRate
    0, // stopRate
    0, // dizzyRate
    0, // poisonRate
    0, // confuseRate
    0, // electricRate
    0, // burnRate
    0, // freezeRate
    0, // hugeRate
    0, // tinyRate
    0, // attackUpRate
    0, // attackDownRate
    0, // defenseUpRate
    0, // defenseDownRate
    0, // allergicRate
    0, // frightRate
    0, // galeForceRate
    0, // fastRate
    0, // slowRate
    0, // dodgyRate
    0, // invisibleRate
    0, // instaKillRate
};

static BattleDispPose pose_table[4] = {
    { 0x1C, "Z_1" },
    { 0x1F, "Z_1" },
    { 0x27, "Z_1" },
    { 0x45, "Z_1" },
};

static BattleDataEntry data_table[4] = {
    { 0x12, damage_event_kaiten_hammer },
    { 0x31, dead_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                           // partNum
        "btl_un_gundan_zako_group1", // partName
        "c_zako_n",                  // modelName
        { 0.0f, 0.0f, 0.0f },        // positionOffset
        { 0.0f, 35.0f, 0.0f },       // hitBaseOffset
        { 0.0f, 40.0f, 0.0f },       // hitCursorBaseOffset
        20,                          // shadowSize
        30,                          // field_0x32
        255,                         // baseAlpha
        &defence,                    // defense
        &defence_attr,               // defenseAttr
        0x03000009,                  // attributes
        0,                           // counterAttributes
        pose_table,                  // poseTable
    },
};

BattleWeapon weapon_gundan_zako_group1_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    0,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 5, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00001087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x00000004,                               // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_B4
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), 15)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), 15)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamage, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        SET_VALUE(LW(15), 1)
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 4, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 8, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 15, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 20, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 0, LW(14))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(zakogun_damage_pose)
        CALL(zakogun_start_sukima, 0)
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(zakogun_damage_pose_cancel)
    CALL(zakogun_end_sukima)
    RETURN
EVT_END()

EVT_BEGIN(damage_event_kaiten_hammer)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamage, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL(zakogun_damage_pose)
        CALL(zakogun_start_sukima, 1)
        WAIT_FRAMES(80)
        CALL(zakogun_damage_pose_cancel)
        CALL(zakogun_end_sukima)
    ELSE
        WAIT_FRAMES(80)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(zakogun_dead1)
    WAIT_FRAMES(36)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(48)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(10)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(evt_sub_random, 100, LW(3))
            SUBTRACT(LW(3), 50)
            ADD(LW(0), LW(3))
            CALL(evt_sub_random, 30, LW(3))
            ADD(LW(1), LW(3))
            SET_VALUE(LW(2), 75)
            CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetCoin, -2, LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, -2, LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(1), LW(2))
    CALL(_add_star_point_disp_offset, -2, LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, -2, 0)
    ELSE
        CALL(btlevtcmd_KillUnit, -2, 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    SET_VALUE(LW(9), PTR(&weapon_gundan_zako_group1_attack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(zakogun_set_target_id, LW(3))
    BEGIN_SIBLING_EVENT
        CALL_CHILD(hit_check_event)
    END_SIBLING_EVENT
    CALL(zakogun_attack1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(hit_check_event)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_END
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_EQUAL(1)
        GOTO(91)
        CASE_END
        CASE_DEFAULT
        GOTO(98)
        CASE_END
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(98)
    LABEL(98)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

ExtMakeTextureEntry zakogun_ext_entry_data[49] = {
    { "c_zako_n", "S_1", 0.0f },
    { "c_zako_n", "S_1", 2.0f },
    { "c_zako_n", "S_1", 4.0f },
    { "c_zako_n", "S_1", 6.0f },
    { "c_zako_n", "S_1", 8.0f },
    { "c_zako_n", "S_1", 10.0f },
    { "c_zako_n", "S_1", 12.0f },
    { "c_zako_n", "S_1", 14.0f },
    { "c_zako_n", "S_1", 16.0f },
    { "c_zako_n", "S_1", 18.0f },
    { "c_zako_n", "S_1", 20.0f },
    { "c_zako_n", "S_1", 22.0f },
    { "c_zako_n", "S_1", 24.0f },
    { "c_zako_n", "S_1", 26.0f },
    { "c_zako_n", "S_1", 28.0f },
    { "c_zako_n", "S_1", 30.0f },
    { "c_zako_n", "S_1", 32.0f },
    { "c_zako_n", "S_1", 34.0f },
    { "c_zako_n", "S_1", 36.0f },
    { "c_zako_n", "S_1", 38.0f },
    { "c_zako_n", "Z_2", 0.0f },
    { "c_zako_n", "Z_3", 0.0f },
    { "c_zako_n", "R_1", 0.0f },
    { "c_zako_n", "R_1", 1.0f },
    { "c_zako_n", "R_1", 2.0f },
    { "c_zako_n", "R_1", 3.0f },
    { "c_zako_n", "R_1", 4.0f },
    { "c_zako_n", "R_1", 5.0f },
    { "c_zako_n", "R_1", 6.0f },
    { "c_zako_n", "R_1", 7.0f },
    { "c_zako_n", "R_1", 8.0f },
    { "c_zako_n", "R_1", 9.0f },
    { "c_zako_n", "A_3", 0.0f },
    { "c_zako_n", "D_1", 0.0f },
    { "c_zako_n", "D_1", 3.0f },
    { "c_zako_n", "D_1", 6.0f },
    { "c_zako_n", "J_1A", 0.0f },
    { "c_zako_n", "S_2", 0.0f },
    { "c_zako_n", "S_2", 10.0f },
    { "c_zako_n", "S_2", 20.0f },
    { "c_zako_n", "S_2", 30.0f },
    { "c_zako_n", "A_5", 0.0f },
    { "c_zako_n", "A_5", 8.0f },
    { "c_zako_n", "A_5", 12.0f },
    { "c_zako_n", "A_2", 0.0f },
    { "c_zako_n", "D_5", 0.0f },
    { "c_zako_n", "D_5", 2.0f },
    { "c_zako_n", "D_5", 4.0f },
    { NULL, NULL, 0.0f },
};

EVT_BEGIN(zakogun_entry)
    CALL(evt_npc_entry, STRING("ext_zakogun"), STRING("c_zako_n"))
    CALL(evt_npc_set_position, STRING("ext_zakogun"), 0, -1000, 0)
    CALL(evt_ext_entry, 500, PTR(zakogun_ext_entry_data), PTR(zakogun_ext_init), PTR(zakogun_ext_main), 0)
    RETURN
EVT_END()


// TODO: type
ZakogunWork* work_20_bss_6C;

// TODO: type
ZakogunEntry* zakogun;

// clang-format on

// clang-format off

s32 zakogun_set_target_id(EventEntry* event, BOOL isFirstCall) {
    work_20_bss_6C->targetId = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

void zakogun_set_pose(ZakogunEntry* entry, s32 poseStart, s32 poseCount, s32 poseDiv) {
    entry->poseStart = poseStart;
    entry->poseCount = poseCount;
    entry->poseDiv = poseDiv;
    entry->poseIndex = 0;
}

s32 zakogun_damage_pose(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    if (isFirstCall) {
        for (i = 0; i < 500; i++) {
            ZakogunEntry* entry = &zakogun[i];

            if (entry != NULL) {
                switch (work_20_bss_6C->phase) {
                    case 0:
                    case 2:
                        entry->poseStart = 0x21;
                        entry->poseCount = 3;
                        entry->poseDiv = 3;
                        entry->poseIndex = 0;
                        break;
                    case 1:
                        entry->poseStart = 0x2D;
                        entry->poseCount = 3;
                        entry->poseDiv = 2;
                        entry->poseIndex = 0;
                        break;
                }
            }
        }
    }
    return EVT_RETURN_DONE;
}

s32 zakogun_damage_pose_cancel(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    if (isFirstCall) {
        for (i = 0; i < 500; i++) {
            ZakogunEntry* entry = &zakogun[i];

            if (entry != NULL) {
                switch (work_20_bss_6C->phase) {
                    case 0:
                        entry->poseStart = 0x15;
                        entry->poseCount = 1;
                        entry->poseDiv = 1;
                        entry->poseIndex = 0;
                        break;
                    case 1:
                        entry->poseStart = 0x25;
                        entry->poseCount = 4;
                        entry->poseDiv = 6;
                        entry->poseIndex = 0;
                        break;
                    case 2:
                        entry->poseStart = 0;
                        entry->poseCount = 0x14;
                        entry->poseDiv = 2;
                        entry->poseIndex = 0;
                        break;
                }
            }
        }
    }
    return EVT_RETURN_DONE;
}

BattleUnit* zakogun_get_ptr(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            if (unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION1 ||
                unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION2 ||
                unit->initialKind == UNIT_X_NAUTS_CRUMP_FORMATION3) {
                return unit;
            }
        }
    }
    return NULL;
}

void zakogun_ext_init(void) {
    u8* flag = &_battleWorkPointer->field_180C0.stageFlags;
    BattleStageObject* obj;
    BattleStageObject* obj2;

    work_20_bss_6C = BattleAlloc(0x74);
    work_20_bss_6C->entries = BattleAlloc(0xDAC0);
    zakogun = work_20_bss_6C->entries;
    work_20_bss_6C->phase = 0;
    obj = BattleSearchObjectPtr("if_body");
    obj2 = BattleSearchObjectPtr("if_wire");
    obj2->mPosition = (Vec){0.0f, 250.0f, 0.0f};
    obj->mPosition = obj2->mPosition;
    flag[0] |= 0x60;
}

void zakogun_ext_main(void) {
    switch (work_20_bss_6C->phase) {
        case 0:
            zakogun_ext_main1();
            break;
        case 1:
            zakogun_ext_main2();
            break;
        case 2:
            zakogun_ext_main3();
            break;
    }
    dispEntry(4, 0.0f, 1, zakogun_ext_disp, NULL);
}

void zakogun_ext_disp(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

s32 zakogun_get_kanbu_id_evtcmd(EventEntry* event, BOOL isFirstCall) {
    s32 id = -1;
    BattleWork* wp = _battleWorkPointer;
    s32 i;
    s32* args = event->args;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            if (unit->initialKind == UNIT_LORD_CRUMP_CHAPTER5) {
                id = i;
            }
        }
    }
    evtSetValue(event, args[0], id);
    return EVT_RETURN_DONE;
}

s32 zakogun_get_kanbu_id(void) {
    s32 i;
    BattleWork* wp = _battleWorkPointer;
    s32 id = -1;

    for (i = 0; i < 0x40; i++) {
        BattleUnit* unit = BattleGetUnitPtr(wp, i);

        if (unit != NULL) {
            if (unit->initialKind == UNIT_LORD_CRUMP_CHAPTER5) {
                id = i;
            }
        }
    }
    return id;
}

s32 zakogun_ext_init1(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    ExtPose* pose = extGetPosePtr();

    work_20_bss_6C->phase = 0;
    for (i = 0; i < 6; i++, pose++) {
        zakogun_ext_init123_common(pose, i);
    }
    return EVT_RETURN_DONE;
}

void zakogun_ext_init123_common(ExtPose* pose, s32 index) {
    pose->flags = EXT_POSE_VISIBLE;
    PSMTXIdentity(*pose->matrix);
    (*pose->matrix)[0][3] = 0.0f;
    (*pose->matrix)[1][3] = -1000.0f;
    (*pose->matrix)[2][3] = 0.0f;
    pose->heading = 90.0f;
    zakogun[index].field_0x0 = 0;
    zakogun[index].state = 0;
    zakogun[index].timer = 0;
    zakogun[index].pos = (Vec){0.0f, -1000.0f, 0.0f};
    zakogun[index].target = (Vec){0.0f, 0.0f, 0.0f};
    zakogun[index].rotY = 0.0f;
}

const ZakoOffsetTable dat_muj_00012fdc = {
    { -20.0f, -10.0f, 0.0f, 10.0f, 20.0f, 30.0f }
};

void zakogun_ext_main1(void) {
    BattleWork* wp = _battleWorkPointer;
    BattleUnit* unit;
    s32 i;
    s32 j;
    s32 n;
    ExtPose* pose;
    f32 x;
    f32 y;
    f32 z;
    ZakoOffsetTable arr;
    f32 f31;
    Mtx mtx;

    unit = zakogun_get_ptr();
    pose = extGetPosePtr();
    for (i = 0; i < 6; i++, pose++) {
        ZakogunEntry* entry = &zakogun[i];

        pose->texId = entry->poseStart + entry->poseIndex / entry->poseDiv;
        entry->poseIndex++;
        if (entry->poseIndex >= entry->poseCount * entry->poseDiv) {
            entry->poseIndex = 0;
        }
        if (i == 1 && entry->state >= 0x23 && entry->state < 0x50) {
            unit->mPosition.x = entry->pos.x;
        }
        switch (entry->state) {
            case 0:
                entry->state = 5;
                entry->timer = 0;
                entry->rotZ = 0.0f;
                entry->rotY = 0.0f;
                entry->rotX = 0.0f;
                entry->poseStart = 0;
                entry->poseCount = 0x14;
                entry->poseDiv = 2;
                entry->poseIndex = 0;
                break;
            case 5:
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->timer = 0;
                    entry->state = 0xA;
                }
                break;
            case 0xA:
                entry->pos.x = entry->from.x = 300.0f + (f32)irand(100);
                entry->from.y = 0.0f;
                entry->pos.y = 0.0f;
                entry->pos.z = entry->from.z = (f32)irand(100) - 50.0f;
                entry->state = 0xC;
                entry->rotY = 0.0f;
                // fallthrough
            case 0xC:
                x = unit->mHomePosition.x;
                y = unit->mHomePosition.y;
                z = unit->mHomePosition.z;
                switch (i) {
                    case 0:
                        entry->to.x = x - 10.0f;
                        entry->to.y = y;
                        entry->to.z = z - 10.0f;
                        break;
                    case 1:
                        entry->to.x = x;
                        entry->to.y = y;
                        entry->to.z = z;
                        break;
                    case 2:
                        entry->to.x = x + 10.0f;
                        entry->to.y = y;
                        entry->to.z = z + 10.0f;
                        break;
                    case 3:
                        entry->to.x = 45.0f + x;
                        entry->to.y = y;
                        entry->to.z = z - 5.0f;
                        break;
                    case 4:
                        entry->to.x = 55.0f + x;
                        entry->to.y = y;
                        entry->to.z = z + 5.0f;
                        break;
                    case 5:
                        entry->to.x = 75.0f + x;
                        entry->to.y = y;
                        entry->to.z = z;
                        break;
                }
                entry->state = 0xF;
                entry->timer = entry->timerMax = irand(0x28) + 0x28;
                entry->poseStart = 0x16;
                entry->poseCount = 0xA;
                entry->poseDiv = 1;
                entry->poseIndex = 0;
                break;
            case 0xF:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.x, entry->to.x);
                entry->pos.y = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.y, entry->to.y);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.z, entry->to.z);
                if (i == 5) {
                    if (entry->timer % 12 == 0) {
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1L", 0x7F, 0, &entry->pos);
                    }
                    if (entry->timer % 12 == 6) {
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1R", 0x7F, 0, &entry->pos);
                    }
                }
                entry->timer--;
                if (entry->timer >= 0) {
                    break;
                }
                entry->timer = -1;
                entry->rotY = 0.0f;
                if (i >= 3 && i <= 5) {
                    switch (i) {
                        case 3:
                        case 4: {
                            for (j = 0, n = 0; j < 3; j++) {
                                if (zakogun[j].state == 0x14) {
                                    n++;
                                }
                            }
                            if (n == 3) {
                                entry->state = 0x10;
                            }
                            break;
                        }
                        case 5: {
                            for (j = 3, n = 0; j < 5; j++) {
                                if (zakogun[j].state == 0x14) {
                                    n++;
                                }
                            }
                            if (n == 2) {
                                entry->state = 0x10;
                            }
                            break;
                        }
                    }
                } else {
                    entry->state = 0x14;
                    entry->timer = 0;
                    entry->poseStart = 0x15;
                    entry->poseCount = 1;
                    entry->poseDiv = 1;
                    entry->poseIndex = 0;
                }
                break;
            case 0x10:
                x = unit->mHomePosition.x;
                y = unit->mHomePosition.y;
                z = unit->mHomePosition.z;
                entry->from.x = entry->pos.x;
                entry->from.y = entry->pos.y;
                entry->from.z = entry->pos.z;
                switch (i) {
                    case 3:
                        entry->to.x = x - 5.0f;
                        entry->to.y = y + 10.0f;
                        entry->to.z = z - 5.0f;
                        break;
                    case 4:
                        entry->to.x = x + 5.0f;
                        entry->to.y = y + 10.0f;
                        entry->to.z = z + 5.0f;
                        break;
                    case 5:
                        entry->to.x = x;
                        entry->to.y = y + 20.0f;
                        entry->to.z = z;
                        break;
                }
                entry->state = 0x11;
                entry->timerMax = 0x14;
                entry->timer = 0x14;
                entry->velY = ((2.0f * (entry->to.y - entry->from.y)) - -200.0f) / 40.0f;
                entry->poseStart = 0x22;
                entry->poseCount = 1;
                entry->poseDiv = 1;
                entry->poseIndex = 0;
                break;
            case 0x11:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.x, entry->to.x);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.z, entry->to.z);
                entry->pos.y += entry->velY;
                entry->velY += -0.5f;
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x14;
                    entry->timer = 0;
                    entry->pos.y = entry->to.y;
                    entry->poseStart = 0x15;
                    entry->poseCount = 1;
                    entry->poseDiv = 1;
                    entry->poseIndex = 0;
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_WAKE1", 0x7F, 0, &entry->pos);
                }
                break;
            case 0x14:
                if (unit == NULL) {
                    break;
                }
                BtlUnit_GetPos(unit, &x, &y, &z);
                entry->timer++;
                entry->timer = zakogun[0].timer;
                if (entry->timer >= 0x1E) {
                    entry->timer -= 0x1E;
                }
                switch (i) {
                    case 0:
                        entry->pos.x = entry->target.x = x - 10.0f;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z - 10.0f;
                        break;
                    case 1:
                        entry->pos.x = entry->target.x = x;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z;
                        break;
                    case 2:
                        entry->pos.x = entry->target.x = x + 10.0f;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z + 10.0f;
                        break;
                    case 3:
                        entry->pos.x = entry->target.x = x - 5.0f;
                        if (entry->timer < 0xA) {
                            entry->pos.y = entry->target.y = (y + 10.0f) - 0.1f * (f32)entry->timer;
                        } else if (entry->timer < 0x14) {
                            entry->pos.y = entry->target.y = (y + 10.0f) - 1.0f;
                        } else {
                            entry->pos.y = entry->target.y =
                                ((y + 10.0f) - 1.0f) + 0.1f * ((f32)entry->timer - 20.0f);
                        }
                        entry->pos.z = entry->target.z = z - 5.0f;
                        break;
                    case 4:
                        entry->pos.x = entry->target.x = x + 5.0f;
                        if (entry->timer < 0xA) {
                            entry->pos.y = entry->target.y = (y + 10.0f) - 0.1f * (f32)entry->timer;
                        } else if (entry->timer < 0x14) {
                            entry->pos.y = entry->target.y = (y + 10.0f) - 1.0f;
                        } else {
                            entry->pos.y = entry->target.y =
                                ((y + 10.0f) - 1.0f) + 0.1f * ((f32)entry->timer - 20.0f);
                        }
                        entry->pos.z = entry->target.z = z + 5.0f;
                        break;
                    case 5:
                        entry->pos.x = entry->target.x = x;
                        if (entry->timer < 0xF) {
                            entry->pos.y = entry->target.y = (y + 20.0f) - 0.15f * (f32)entry->timer;
                        } else {
                            entry->pos.y = entry->target.y =
                                ((y + 20.0f) - 2.25f) + 0.15f * ((f32)entry->timer - 15.0f);
                        }
                        entry->pos.z = entry->target.z = z;
                        break;
                }
                break;
            case 0x1E:
                if (i >= 0 && i <= 2) {
                    entry->poseStart = 0x14;
                    entry->poseCount = 1;
                    entry->poseDiv = 1;
                    entry->poseIndex = 0;
                }
                entry->state = 0x23;
                entry->timer = 5;
                // fallthrough
            case 0x23:
                if (i >= 3 && i <= 5) {
                    entry->pos.y += 3.0f;
                }
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x24;
                    entry->timer = 0x1E;
                }
                break;
            case 0x24:
                entry->timer--;
                if (i >= 0 && i <= 2) {
                    if (entry->timer == 0xF) {
                        entry->poseStart = 0x16;
                        entry->poseCount = 0xA;
                        entry->poseDiv = 1;
                        entry->poseIndex = 0;
                    }
                    if (entry->timer <= 0xF) {
                        if (entry->timer % 12 == 0) {
                            BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1L", 0x7F, 0, &entry->pos);
                        }
                        if (entry->timer % 12 == 6) {
                            BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1R", 0x7F, 0, &entry->pos);
                        }
                    }
                }
                if (entry->timer > 0) {
                    break;
                }
                entry->state = 0x28;
                break;
            case 0x28: {
                BattleUnit* target = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

                BtlUnit_GetPos(target, &x, &y, &z);
                x += 100.0f;
                entry->from.x = entry->pos.x;
                entry->from.y = entry->pos.y;
                entry->from.z = entry->pos.z;
                entry->to.x = x;
                entry->to.y = entry->pos.y;
                entry->to.z = entry->pos.z;
                entry->state = 0x2D;
                entry->timerMax = 0x28;
                entry->timer = 0x28;
            }
                // fallthrough
            case 0x2D:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.x, entry->to.x);
                if (entry->timer % 12 == 0) {
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1L", 0x7F, 0, &entry->pos);
                }
                if (entry->timer % 12 == 6) {
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1R", 0x7F, 0, &entry->pos);
                }
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x32;
                    entry->timer = 0;
                }
                break;
            case 0x32: {
                BattleUnit* target;

                arr = dat_muj_00012fdc;
                target = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);
                BtlUnit_GetPos(target, &x, &y, &z);
                entry->from.x = entry->pos.x;
                entry->from.y = entry->pos.y;
                entry->from.z = entry->pos.z;
                entry->to.x = x - arr.v[i];
                entry->to.y = 0.0f;
                entry->to.z = entry->from.z;
                entry->state = 0x37;
                entry->timerMax = 0x28;
                entry->timer = 0x28;
                entry->velY = ((2.0f * (entry->to.y - entry->from.y)) - -320.0f) / 80.0f;
                entry->poseStart = 0x20;
                entry->poseCount = 1;
                entry->poseDiv = 1;
                entry->poseIndex = 0;
                if (i == 0) {
                    BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_ATTACK1", 0x7F, 0, &entry->pos);
                }
                break;
            }
            case 0x37: {
                BattleUnit* target = BattleGetUnitPtr(wp, work_20_bss_6C->targetId);

                BtlUnit_GetPos(target, &x, &y, &z);
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.x, entry->to.x);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.z, entry->to.z);
                entry->pos.y += entry->velY;
                entry->velY += -0.2f;
                entry->timer--;
                if (entry->timer <= 0xA) {
                    entry->state = 0x3A;
                    if (i == 0) {
                        unit->work[0]++;
                        entry->poseStart = 0;
                        entry->poseCount = 0x14;
                        entry->poseDiv = 2;
                        entry->poseIndex = 0;
                    }
                }
                break;
            }
            case 0x3A:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.x, entry->to.x);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->from.z, entry->to.z);
                entry->pos.y += entry->velY;
                entry->velY += -0.2f;
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x3C;
                    entry->timer = 0x1E;
                    entry->pos.y = entry->to.y;
                    entry->poseStart = 0x15;
                    entry->poseCount = 1;
                    entry->poseDiv = 1;
                    entry->poseIndex = 0;
                    if (i == 0) {
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_ATTACK1_2", 0x7F, 0, &entry->pos);
                    }
                }
                break;
            case 0x3C:
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->state = 0x3E;
                    entry->poseStart = 0x16;
                    entry->poseCount = 0xA;
                    entry->poseDiv = 1;
                    entry->poseIndex = 0;
                }
                break;
            case 0x3E:
                entry->state = 0xC;
                entry->rotY = 180.0f;
                entry->from = entry->pos;
                unit->mPosition = unit->mHomePosition;
                btl_camera_set_mode(0, 0);
                break;
            case 0x50:
                entry->rotZ = 0.0f;
                entry->state = 0x51;
                entry->timer = 6;
                // fallthrough
            case 0x51:
                if (i >= 0 && i <= 2) {
                    entry->rotZ += 15.0f;
                    if (entry->rotZ >= 45.0f) {
                        entry->rotZ = 45.0f;
                    }
                }
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->state = 0x52;
                    entry->timer = 0xA;
                    entry->velY = 0.0f;
                    if (i >= 0 && i <= 2) {
                        entry->rotZ = 45.0f;
                    }
                }
                break;
            case 0x52:
                if (i >= 3 && i <= 4) {
                    entry->rotZ += 15.0f;
                    if (entry->rotZ >= 45.0f) {
                        entry->rotZ = 45.0f;
                    }
                    entry->pos.y += entry->velY;
                    entry->velY += -0.5f;
                    if (entry->pos.y < 0.0f) {
                        entry->pos.y = 0.0f;
                    }
                }
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->state = 0x53;
                    entry->timer = 0x14;
                    entry->velY = 0.0f;
                    if (i >= 3 && i <= 4) {
                        entry->rotZ = 45.0f;
                        entry->pos.y = 0.0f;
                    }
                }
                break;
            case 0x53:
                if (i == 5) {
                    entry->rotZ += 15.0f;
                    if (entry->rotZ >= 45.0f) {
                        entry->rotZ = 45.0f;
                    }
                    entry->pos.y += entry->velY;
                    entry->velY += -0.5f;
                    if (entry->pos.y < 0.0f) {
                        entry->pos.y = 0.0f;
                    }
                }
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->rotY = 0.0f;
                    entry->state = 0x54;
                    entry->timer = 0x30;
                    if (i == 5) {
                        entry->rotZ = 45.0f;
                        entry->pos.y = 0.0f;
                    }
                }
                break;
            case 0x54:
                if (entry->timer <= 0x30) {
                    entry->rotY -= 15.0f;
                }
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->rotY = 0.0f;
                    entry->state = 0x55;
                }
                break;
            case 0x55:
                entry->rotX += 3.0f;
                if (entry->rotX >= 90.0f) {
                    entry->state = -1;
                    entry->pos = (Vec){0.0f, -1000.0f, 0.0f};
                }
                break;
            case 0x5A:
                entry->pos.x += 5.0f;
                if (i == 0) {
                    if (gp->retraceCount % 12 == 0) {
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1L", 0x7F, 0, &entry->pos);
                    }
                    if (gp->retraceCount % 12 == 6) {
                        BtlUnit_snd_se_pos(unit, "SFX_BOSS_TEAM_3RD_MOVE1R", 0x7F, 0, &entry->pos);
                    }
                }
                if (entry->pos.x >= 300.0f) {
                    entry->state = -1;
                }
                break;
            case 0x5F:
            case 0x60:
                f31 = 0.0f;
                BtlUnit_GetPos(unit, &x, &y, &z);
                if (entry->state == 0x60) {
                    entry->timer++;
                    entry->timer = zakogun[0].timer;
                    if ((entry->timer & 3) == 0 || (entry->timer & 3) == 1) {
                        f31 = 1.0f;
                    } else {
                        f31 = -1.0f;
                    }
                }
                switch (i) {
                    case 0:
                        entry->pos.x = entry->target.x = x - 10.0f + f31;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z - 10.0f;
                        break;
                    case 1:
                        entry->pos.x = entry->target.x = x + f31;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z;
                        break;
                    case 2:
                        entry->pos.x = entry->target.x = x + 10.0f + f31;
                        entry->pos.y = entry->target.y = y;
                        entry->pos.z = entry->target.z = z + 10.0f;
                        break;
                    case 3:
                        entry->pos.x = entry->target.x = x - 5.0f + f31;
                        entry->pos.y = entry->target.y = y + 20.0f;
                        entry->pos.z = entry->target.z = z - 5.0f;
                        break;
                    case 4:
                        entry->pos.x = entry->target.x = x + 5.0f + f31;
                        entry->pos.y = entry->target.y = y + 20.0f;
                        entry->pos.z = entry->target.z = z + 5.0f;
                        break;
                    case 5:
                        entry->pos.x = entry->target.x = x + f31;
                        entry->pos.y = entry->target.y = y + 40.0f;
                        entry->pos.z = entry->target.z = z;
                        break;
                }
                break;
        }
        PSMTXIdentity(*pose->matrix);
        PSMTXRotRad(mtx, 'x', 0.017453292f * entry->rotX);
        PSMTXConcat(*pose->matrix, mtx, *pose->matrix);
        PSMTXRotRad(mtx, 'y', 0.017453292f * entry->rotY);
        PSMTXConcat(*pose->matrix, mtx, *pose->matrix);
        PSMTXRotRad(mtx, 'z', 0.017453292f * entry->rotZ);
        PSMTXConcat(*pose->matrix, mtx, *pose->matrix);
        (*pose->matrix)[0][3] = entry->pos.x;
        (*pose->matrix)[1][3] = entry->pos.y;
        (*pose->matrix)[2][3] = 0.25f * (f32)i + entry->pos.z;
    }
}


s32 zakogun_attack1(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 6; i++) {
            zakogun[i].state = 0x1E;
        }
    }
    i = 0;
    count = 0;
    for (; i < 6; i++) {
        if (zakogun[i].state == 0x14) {
            count++;
        }
    }
    return count == 6 ? 2 : 0;
}

s32 zakogun_dead1(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    for (i = 0; i < 6; i++) {
        zakogun[i].state = 0x50;
    }
    return EVT_RETURN_DONE;
}

s32 zakogun_escape1(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 6; i++) {
            ZakogunEntry* entry = &zakogun[i];

            entry->state = 0x5A;
            entry->poseStart = 0x16;
            entry->poseCount = 0xA;
            entry->poseDiv = 1;
            entry->poseIndex = 0;
        }
    }
    i = 0;
    count = 0;
    for (; i < 6; i++) {
        if (zakogun[i].state == -1) {
            count++;
        }
    }
    return count == 6 ? 2 : 0;
}

s32 zakogun_start_sukima(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 mode = evtGetValue(event, event->args[0]);

    for (i = 0; i < 6; i++) {
        ZakogunEntry* entry = &zakogun[i];

        if (!(entry->state >= 0x50 && entry->state < 0x5A)) {
            if (mode == 1) {
                entry->state = 0x60;
                entry->timer = 0;
            } else {
                entry->state = 0x5F;
            }
        }
    }
    return EVT_RETURN_DONE;
}

s32 zakogun_end_sukima(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    for (i = 0; i < 6; i++) {
        ZakogunEntry* entry = &zakogun[i];

        if (!(entry->state >= 0x50 && entry->state < 0x5A)) {
            entry->state = 0x14;
        }
    }
    return EVT_RETURN_DONE;
}

// clang-format on
