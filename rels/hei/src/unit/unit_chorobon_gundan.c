#include "unit_chorobon_gundan.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 escape_chorogun_event[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[26];
static s32 damage_event[59];
static s32 attack_event[217];
static s32 wait_event[6];

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_chorobon_gundan = {
    UNIT_FUZZY_HORDE,               // mUnitType
    "btl_un_chorobon_gundan",       // name
    20,                             // maxHp
    0,                              // maxFp
    1,                              // mDangerHP
    1,                              // mPerilHP
    19,                             // mLevel
    0,                              // mBonusExp
    5,                              // bonusCoinRolls
    100,                            // bonusCoinRate
    0,                              // baseCoin
    50,                             // field_0x13
    5,                              // field_0x14
    260,                            // width
    50,                             // height
    { 0, 50 },                      // statusIconOffset
    { 0.0f, 0.0f, 0.0f },           // centerOffset
    { 0, 0 },                       // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },           // talkTogeBaseOffset
    { 130.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },           // burnFlameOffset
    0.0f,                           // field_0x54
    0.0f,                           // field_0x58
    { 130.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 130.0f, 32.5f, 0.0f },        // kissHitOffset
    { 0.0f, 25.0f, 0.0f },          // cutBaseOffset
    260.0f,                         // cutWidth
    50.0f,                          // cutHeight
    0,                              // turnOrder
    0,                              // turnOrderVariance
    -1,                             // swallowRate
    0,                              // swallowAttribute
    100,                            // field_0x8C
    20,                             // itemStealParam
    { 0.0f, 0.0f, 0.0f },           // starPointDispOffset
    "SFX_BOSS_TEAM_CHORO_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",         // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",          // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",     // electricDamageSfxName
    0x00240000,                     // attributeFlags
    &regist,                        // vulnStatus
    1,                              // numParts
    parts,                          // mParts
    init_event,                     // initEvtScript
    data_table,                     // dataTable
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

static BattleDispPose pose_table[15] = {
    { 0x01, "CBN_N_1" },
    { 0x02, "CBN_Y_1" },
    { 0x09, "CBN_Y_1" },
    { 0x05, "CBN_K_1" },
    { 0x04, "CBN_X_1" },
    { 0x03, "CBN_X_1" },
    { 0x1C, "CBN_S_1" },
    { 0x1D, "CBN_Q_1" },
    { 0x1E, "CBN_Q_1" },
    { 0x1F, "CBN_S_1" },
    { 0x27, "CBN_D_1" },
    { 0x32, "CBN_A_1" },
    { 0x2A, "CBN_R_1" },
    { 0x28, "CBN_W_1" },
    { 0x45, "CBN_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                        // partNum
        "btl_un_chorobon_gundan", // partName
        "c_chorobon",             // modelName
        { 0.0f, 0.0f, 0.0f },     // positionOffset
        { 0.0f, 25.0f, 0.0f },    // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },    // hitCursorBaseOffset
        20,                       // field_0x30
        30,                       // field_0x32
        255,                      // baseAlpha
        &defence,                 // defense
        &defence_attr,            // defenseAttr
        0x03000009,               // attributes
        0,                        // counterAttributes
        pose_table,               // poseTable
    },
};

BattleWeapon weapon_chorobon_gundan_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000002,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000007,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80000004,                               // targetWeightingFlags
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

EVT_BEGIN(unk_evt_hei_00019e30)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(_add_star_point_disp_offset, LW(10), LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: write chorogun_escape (global, 0x1ec bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_get_alive_num (global, 0x80 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_GetDamage, -2, LW(15))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        IF_INT_GREATER_OR_EQUAL(LW(0), LW(15))
            BREAK_LOOP
        END_IF
        CALL(chorogun_get_alive_num, LW(1))
        IF_INT_LESS_OR_EQUAL(LW(1), 50)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        ELSE
            CALL(chorogun_escape)
        END_IF
        ADD(LW(0), 1)
    END_LOOP
    CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(chorogun_get_alive_num, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 500)
        CALL_CHILD(escape_chorogun_event)
        GOTO(99)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

// TODO: write chorogun_after_attack (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_counterd (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_bound_target (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_dive_target (global, 0x230 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_approach_target (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_set_target_id (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_chorobon_gundan_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_chorobon_gundan_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_chorobon_gundan_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_chorobon_gundan_attack))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, LW(3), -1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(chorogun_set_target_id, LW(3))
    CALL(chorogun_approach_target)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 256, LW(5))
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
    WAIT_MS(1000)
    GOTO(98)
    LABEL(91)
    CALL(chorogun_get_alive_num, LW(15))
    ADD(LW(15), 100)
    DIVIDE(LW(15), 200)
    IF_INT_LESS_OR_EQUAL(LW(15), 0)
        SET_VALUE(LW(15), 1)
    END_IF
    SET_VALUE(LW(14), 1)
    LOOP(0)
        IF_INT_GREATER(LW(14), LW(15))
            BREAK_LOOP
        END_IF
        CALL(chorogun_dive_target, 30, LW(6))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_chorobon_gundan_attack))
        IF_INT_EQUAL(LW(14), LW(15))
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 256, LW(5))
        ELSE
            CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_chorobon_gundan_attack), 0, LW(5))
        END_IF
        IF_INT_EQUAL(LW(5), 21)
            CALL(chorogun_counterd, LW(6))
        ELSE
            CALL(chorogun_bound_target, LW(6))
        END_IF
        ADD(LW(14), 1)
        WAIT_FRAMES(30)
    END_LOOP
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(chorogun_after_attack)
    WAIT_MS(1000)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
u32 chorogun_ext_entry_data[63] = {
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x00000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x40000000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x40800000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x40C00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41200000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41400000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41600000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41800000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41900000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41A00000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41B00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x41C00000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x41D00000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x41E00000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x41F00000,
    (u32)"c_chorobon", (u32)"CBN_W_1", 0x42000000, (u32)"c_chorobon",
    (u32)"CBN_W_1", 0x42080000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x42100000, (u32)"c_chorobon", (u32)"CBN_W_1", 0x42180000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: write chorogun_ext_main (global, 0xd38 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_ext_init (global, 0x37c bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(chorogun_entry)
    CALL(evt_npc_entry, STRING("ext_chorobon"), STRING("c_chorobon"))
    CALL(evt_npc_set_position, STRING("ext_chorobon"), 0, -1000, 0)
    CALL(evt_ext_entry, 1000, PTR(chorogun_ext_entry_data), PTR(chorogun_ext_init), PTR(chorogun_ext_main), 0)
    RETURN
EVT_END()

// TODO: type
u32 chorogun_targetId[1] = {
    0x00000000,
};

// TODO: type
u32 chorogun[1] = {
    0x00000000,
};

// TODO: write chorogun_escape_all (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_appear (global, 0xcc bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_ext_disp (global, 0x34 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it

// TODO: write chorogun_get_ptr (global, 0x64 bytes) here:
// its asm is in build/G8MJ01/hei/asm/unit/unit_chorobon_gundan.s; this is where the original has it, just before the first script that uses it
// clang-format on
