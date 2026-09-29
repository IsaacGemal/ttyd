#include "unit_boss_gundan_zako_group1.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

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
    50,                            // field_0x13
    9999,                          // field_0x14
    50,                            // width
    50,                            // height
    { 0, 50 },                     // statusIconOffset
    { 0.0f, 0.0f, 0.0f },          // centerOffset
    { 0, 0 },                      // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },          // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // field_0x54
    0.0f,                          // field_0x58
    { 25.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 25.0f, 32.5f, 0.0f },        // kissHitOffset
    { -4.0f, 25.0f, 0.0f },        // cutBaseOffset
    50.0f,                         // cutWidth
    50.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    -1,                            // swallowRate
    0,                             // swallowAttribute
    100,                           // field_0x8C
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
        20,                          // field_0x30
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
    0,                                        // field_0x6D
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

// TODO: write zakogun_end_sukima (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_start_sukima (global, 0x8c bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_damage_pose_cancel (global, 0xcc bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_damage_pose (global, 0xa8 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

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

// TODO: write zakogun_dead1 (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

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

// TODO: write zakogun_attack1 (global, 0x88 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_set_target_id (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

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

// TODO: type
u32 zakogun_ext_entry_data[147] = {
    (u32)"c_zako_n", (u32)"S_1", 0x00000000, (u32)"c_zako_n",
    (u32)"S_1", 0x40000000, (u32)"c_zako_n", (u32)"S_1",
    0x40800000, (u32)"c_zako_n", (u32)"S_1", 0x40C00000,
    (u32)"c_zako_n", (u32)"S_1", 0x41000000, (u32)"c_zako_n",
    (u32)"S_1", 0x41200000, (u32)"c_zako_n", (u32)"S_1",
    0x41400000, (u32)"c_zako_n", (u32)"S_1", 0x41600000,
    (u32)"c_zako_n", (u32)"S_1", 0x41800000, (u32)"c_zako_n",
    (u32)"S_1", 0x41900000, (u32)"c_zako_n", (u32)"S_1",
    0x41A00000, (u32)"c_zako_n", (u32)"S_1", 0x41B00000,
    (u32)"c_zako_n", (u32)"S_1", 0x41C00000, (u32)"c_zako_n",
    (u32)"S_1", 0x41D00000, (u32)"c_zako_n", (u32)"S_1",
    0x41E00000, (u32)"c_zako_n", (u32)"S_1", 0x41F00000,
    (u32)"c_zako_n", (u32)"S_1", 0x42000000, (u32)"c_zako_n",
    (u32)"S_1", 0x42080000, (u32)"c_zako_n", (u32)"S_1",
    0x42100000, (u32)"c_zako_n", (u32)"S_1", 0x42180000,
    (u32)"c_zako_n", (u32)"Z_2", 0x00000000, (u32)"c_zako_n",
    (u32)"Z_3", 0x00000000, (u32)"c_zako_n", (u32)"R_1",
    0x00000000, (u32)"c_zako_n", (u32)"R_1", 0x3F800000,
    (u32)"c_zako_n", (u32)"R_1", 0x40000000, (u32)"c_zako_n",
    (u32)"R_1", 0x40400000, (u32)"c_zako_n", (u32)"R_1",
    0x40800000, (u32)"c_zako_n", (u32)"R_1", 0x40A00000,
    (u32)"c_zako_n", (u32)"R_1", 0x40C00000, (u32)"c_zako_n",
    (u32)"R_1", 0x40E00000, (u32)"c_zako_n", (u32)"R_1",
    0x41000000, (u32)"c_zako_n", (u32)"R_1", 0x41100000,
    (u32)"c_zako_n", (u32)"A_3", 0x00000000, (u32)"c_zako_n",
    (u32)"D_1", 0x00000000, (u32)"c_zako_n", (u32)"D_1",
    0x40400000, (u32)"c_zako_n", (u32)"D_1", 0x40C00000,
    (u32)"c_zako_n", (u32)"J_1A", 0x00000000, (u32)"c_zako_n",
    (u32)"S_2", 0x00000000, (u32)"c_zako_n", (u32)"S_2",
    0x41200000, (u32)"c_zako_n", (u32)"S_2", 0x41A00000,
    (u32)"c_zako_n", (u32)"S_2", 0x41F00000, (u32)"c_zako_n",
    (u32)"A_5", 0x00000000, (u32)"c_zako_n", (u32)"A_5",
    0x41000000, (u32)"c_zako_n", (u32)"A_5", 0x41400000,
    (u32)"c_zako_n", (u32)"A_2", 0x00000000, (u32)"c_zako_n",
    (u32)"D_5", 0x00000000, (u32)"c_zako_n", (u32)"D_5",
    0x40000000, (u32)"c_zako_n", (u32)"D_5", 0x40800000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: write zakogun_ext_main (global, 0x88 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_ext_init (global, 0xe0 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(zakogun_entry)
    CALL(evt_npc_entry, STRING("ext_zakogun"), STRING("c_zako_n"))
    CALL(evt_npc_set_position, STRING("ext_zakogun"), 0, -1000, 0)
    CALL(evt_ext_entry, 500, PTR(zakogun_ext_entry_data), PTR(zakogun_ext_init), PTR(zakogun_ext_main), 0)
    RETURN
EVT_END()

// TODO: write zakogun_ext_main1 (global, 0x1904 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: type
static u32 jumptable_00030120[97] = {
    (u32)((u8*)&zakogun_ext_main1 + 0x15c), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x19c), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1c8), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x274), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x3e8),
    (u32)((u8*)&zakogun_ext_main1 + 0x5d8), (u32)((u8*)&zakogun_ext_main1 + 0x730), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x7fc), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0xc24), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0xc60),
    (u32)((u8*)&zakogun_ext_main1 + 0xcb0), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0xd84), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0xe08), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0xec0), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0xfec),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x10cc), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x11a4), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x11e8), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x123c), (u32)((u8*)&zakogun_ext_main1 + 0x1258), (u32)((u8*)&zakogun_ext_main1 + 0x12ec), (u32)((u8*)&zakogun_ext_main1 + 0x13c4),
    (u32)((u8*)&zakogun_ext_main1 + 0x148c), (u32)((u8*)&zakogun_ext_main1 + 0x14dc), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1534), (u32)((u8*)&zakogun_ext_main1 + 0x1808),
    (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x1808), (u32)((u8*)&zakogun_ext_main1 + 0x15f8),
    (u32)((u8*)&zakogun_ext_main1 + 0x15f8),
};

// TODO: type
u32 zakogun[1] = {
    0x00000000,
};

// TODO: type
u32 work_20_bss_6C[1] = {
    0x00000000,
};

// TODO: write zakogun_escape1 (global, 0xa8 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_ext_init123_common (global, 0x108 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_ext_init1 (global, 0x16c bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_get_kanbu_id (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_get_kanbu_id_evtcmd (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_ext_disp (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_get_ptr (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it

// TODO: write zakogun_set_pose (global, 0x18 bytes) here:
// its asm is in build/G8MJ01/muj/asm/unit/unit_boss_gundan_zako_group1.s; this is where the original has it, just before the first script that uses it
// clang-format on
