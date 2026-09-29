#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern BattleUnitKind UNIT_REF_TOGEZO;
static USER_FUNC(togezo_copy_status);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[2];
static s32 init_event[40];
static s32 damage_event[322];
static s32 unison_phase_event[139];
static s32 avoid_counter_thorn_head_event[61];
static s32 dmg_first_attack_pos_event[19];
static s32 attack_event[185];
static s32 normal_attack_event[585];
static s32 friend_event[263];
static s32 wait_event[6];

// clang-format off
extend_data(unit_jyugem_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_jyugem_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind UNIT_DATA_SYM = {
    UNIT_LAKITU,                // mUnitType
    "btl_un_jyugem",            // name
    5,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    11,                         // mLevel
    0,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    1,                          // baseCoin
    50,                         // field_0x13
    9999,                       // field_0x14
    32,                         // width
    42,                         // height
    { 4, 42 },                  // statusIconOffset
    { 0.0f, 21.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 16.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 16.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 10.0f, 38.0f, 0.0f },     // kissHitOffset
    { 0.0f, 21.0f, 0.0f },      // cutBaseOffset
    32.0f,                      // cutWidth
    42.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_JUGEMU_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00000004,                 // attributeFlags
    &regist,                    // vulnStatus
    2,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    80,  // sleepRate
    90,  // stopRate
    105, // dizzyRate
    100, // poisonRate
    80,  // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    90,  // tinyRate
    100, // attackUpRate
    90,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    80,  // allergicRate
    90,  // frightRate
    100, // galeForceRate
    100, // fastRate
    80,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    95,  // instaKillRate
};

static BattleDispPose pose_table[17] = {
    { 0x01, "JGM_N_1" },
    { 0x02, "JGM_Y_1" },
    { 0x09, "JGM_Y_1" },
    { 0x05, "JGM_K_1" },
    { 0x04, "JGM_X_1" },
    { 0x03, "JGM_X_1" },
    { 0x1C, "JGM_S_1" },
    { 0x1D, "JGM_Q_1" },
    { 0x1E, "JGM_Q_1" },
    { 0x1F, "JGM_S_1" },
    { 0x27, "JGM_D_1" },
    { 0x2A, "JGM_R_1" },
    { 0x28, "JGM_W_1" },
    { 0x38, "JGM_I_1" },
    { 0x39, "JGM_I_1" },
    { 0x41, "JGM_T_1" },
    { 0x45, "JGM_S_1" },
};

static BattleDispPose pose_table_hold[17] = {
    { 0x01, "JGM_S_2" },
    { 0x02, "JGM_S_2" },
    { 0x09, "JGM_S_2" },
    { 0x05, "JGM_S_2" },
    { 0x04, "JGM_S_2" },
    { 0x03, "JGM_S_2" },
    { 0x1C, "JGM_S_2" },
    { 0x1D, "JGM_Q_1" },
    { 0x1E, "JGM_Q_1" },
    { 0x1F, "JGM_S_2" },
    { 0x27, "JGM_D_1" },
    { 0x2A, "JGM_S_2" },
    { 0x28, "JGM_S_2" },
    { 0x38, "JGM_S_2" },
    { 0x39, "JGM_S_2" },
    { 0x41, "JGM_S_2" },
    { 0x45, "JGM_S_2" },
};

static BattleDataEntry data_table[3] = {
    { 0x25, avoid_counter_thorn_head_event },
    { 0x30, dmg_first_attack_pos_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[2] = {
    {
        1,                     // partNum
        "btl_un_jyugem",       // partName
        "c_jugemu",            // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 35.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 50.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00600009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
    {
        2,                    // partNum
        "パイポ",                // partName
        "c_jugemu",           // modelName
        { 0.0f, 0.0f, 0.0f }, // positionOffset
        { 0.0f, 0.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 0.0f, 0.0f }, // hitCursorBaseOffset
        20,                   // field_0x30
        30,                   // field_0x32
        255,                  // baseAlpha
        &defence,             // defense
        &defence_attr,        // defenseAttr
        0x13010000,           // attributes
        0,                    // counterAttributes
        pose_table,           // poseTable
    },
};

static BattleWeapon weapon_jyugem_attack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardsAllowed
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x000007FF,                               // counterResistanceFlags
    0x80002000,                               // targetWeightingFlags
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

static BattleUnitSetup entry_togezo[1] = {
    { &UNIT_REF_TOGEZO, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventUnisonPhase, -2, PTR(unison_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("JGM_P_1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL_CHILD(btldefaultevt_Damage)
    ELSE
        CALL(btlevtcmd_GetDamage, LW(10), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            SET_VALUE(LW(15), 1)
        ELSE
            SET_VALUE(LW(15), 0)
        END_IF
        CALL(btlevtcmd_CheckStatus, LW(10), 27, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 1, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 3, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 5, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 6, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 9, LW(14))
        OR(LW(15), LW(14))
        CALL(btlevtcmd_CheckStatus, LW(10), 2, LW(14))
        OR(LW(15), LW(14))
        IF_INT_NOT_EQUAL(LW(15), 0)
            BEGIN_INLINE_EVENT
                CALL(btlevtcmd_OffPartsAttribute, -2, 2, 50331648)
                CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                ADD(LW(1), 35)
                CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(0), 200)
                CALL(btlevtcmd_JumpPartsSetting, LW(10), 2, 0, FLOAT(5.0f), FLOAT(0.3f))
                CALL(btlevtcmd_JumpPartsPosition, LW(10), 2, LW(0), 0, LW(2), 0, -1)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 2, 16777216)
                CALL(btlevtcmd_OnPartsAttribute, LW(10), 2, 33554432)
            END_INLINE_EVENT
            CALL(btlevtcmd_SetUnitWork, LW(10), 0, 0)
            CALL(btlevtcmd_OffPartsCounterAttribute, LW(10), 1, 1)
            CALL(btlevtcmd_AnimeSetPoseTable, LW(10), 1, PTR(pose_table))
            CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
            CALL_CHILD(btldefaultevt_Damage)
        ELSE
            CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
            IF_INT_EQUAL(LW(0), 22)
                GOTO(90)
            END_IF
            IF_INT_EQUAL(LW(0), 30)
                GOTO(90)
            END_IF
            IF_INT_EQUAL(LW(0), 31)
                GOTO(90)
            END_IF
            IF_INT_EQUAL(LW(0), 38)
                GOTO(90)
            END_IF
            IF_INT_EQUAL(LW(0), 39)
                GOTO(90)
            END_IF
            IF_INT_EQUAL(LW(0), 41)
                GOTO(90)
            END_IF
            CALL(btlevtcmd_CheckStatus, LW(10), 4, LW(15))
            CALL(btlevtcmd_CheckStatus, LW(10), 8, LW(14))
            OR(LW(15), LW(14))
            CALL(btlevtcmd_CheckStatus, LW(10), 11, LW(14))
            OR(LW(15), LW(14))
            CALL(btlevtcmd_CheckStatus, LW(10), 15, LW(14))
            OR(LW(15), LW(14))
            CALL(btlevtcmd_CheckStatus, LW(10), 20, LW(14))
            OR(LW(15), LW(14))
            CALL(btlevtcmd_CheckStatus, LW(10), 0, LW(14))
            OR(LW(15), LW(14))
            IF_INT_NOT_EQUAL(LW(15), 0)
                BEGIN_INLINE_EVENT
                    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 50331648)
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    ADD(LW(1), 35)
                    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_JumpPartsSetting, LW(10), 2, 30, 0, FLOAT(0.1f))
                    CALL(btlevtcmd_JumpPartsPosition, LW(10), 2, LW(0), LW(1), LW(2), 30, -1)
                    CALL(btlevtcmd_OnPartsAttribute, LW(10), 2, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, LW(10), 2, 33554432)
                END_INLINE_EVENT
            END_IF
            LABEL(90)
            CALL_CHILD(btldefaultevt_Damage)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(unison_phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_get_turn, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckActStatus, -2, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 6, LW(14))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        GOTO(99)
    END_IF
    CALL(evt_sub_random, 99, LW(0))
    IF_INT_LESS(LW(0), 50)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_1"))
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_HOLD1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_2"))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL(btlevtcmd_OnPartsCounterAttribute, -2, 1, 1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_hold))
        CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 20, 0)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(avoid_counter_thorn_head_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 45)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 25)
        GOTO(90)
    END_IF
    ADD(LW(1), 35)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_first_attack_pos_event)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 10)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        GOTO(99)
    END_IF
    SET_VALUE(LW(11), 20)
    ADD(LW(11), 65)
    ADD(LW(11), 15)
    SUBTRACT(LW(11), 1)
    CALL(evt_sub_random, LW(11), LW(10))
    SET_VALUE(LW(11), 20)
    IF_INT_LESS(LW(10), LW(11))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(1), 0)
        LOOP(0)
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), 0)
                SUBTRACT(LW(2), 10)
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                IF_INT_EQUAL(LW(3), -1)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            ELSE
                GOTO(9)
            END_IF
        END_LOOP
        LABEL(9)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(1), 0)
        LOOP(0)
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
            IF_INT_LESS_OR_EQUAL(LW(0), 150)
                ADD(LW(2), 10)
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                IF_INT_EQUAL(LW(3), -1)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            ELSE
                GOTO(10)
            END_IF
        END_LOOP
    END_IF
    LABEL(10)
    SET_VALUE(LW(11), 65)
    ADD(LW(11), 15)
    SUBTRACT(LW(11), 1)
    CALL(evt_sub_random, LW(11), LW(10))
    SET_VALUE(LW(11), 65)
    IF_INT_LESS(LW(10), LW(11))
        CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    ELSE
        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static USER_FUNC(togezo_copy_status) {
    s32* args;
    s32 srcId;
    s32 dstId;
    BattleWork* wp;
    BattleUnit* src;
    BattleUnit* dst;

    args = event->args;
    srcId = BattleTransID(event, evtGetValue(event, args[0]));
    dstId = BattleTransID(event, evtGetValue(event, args[1]));
    wp = _battleWorkPointer;
    src = BattleGetUnitPtr(wp, srcId);
    dst = BattleGetUnitPtr(wp, dstId);
    dst->mSizeChangeTurns = src->mSizeChangeTurns;
    dst->mSizeChangeStrength = src->mSizeChangeStrength;
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_jyugem_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_jyugem_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_jyugem_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_jyugem_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_1"))
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_2"))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
        CALL(btlevtcmd_OffPartsCounterAttribute, -2, 1, 1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_3"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 50331648)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 35)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_THROW1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 45, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 45, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_jyugem_attack), 256, LW(5))
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
    CALL(btlevtcmd_JumpPartsContinue, -2, 2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_jyugem_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_jyugem_attack), 256, LW(5))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_CheckDamageCode, LW(3), 262144, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            SET_VALUE(LW(1), 0)
            LOOP(0)
                CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
                SET_VALUE(LW(4), LW(0))
                CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
                MULTIPLY(LW(4), LW(5))
                IF_INT_LESS(LW(4), 0)
                    SUBTRACT(LW(2), 10)
                    CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                    IF_INT_EQUAL(LW(3), -1)
                        GOTO(97)
                    END_IF
                ELSE
                    GOTO(96)
                END_IF
            END_LOOP
            LABEL(96)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            SET_VALUE(LW(1), 0)
            LOOP(0)
                CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
                IF_INT_LESS_OR_EQUAL(LW(0), 150)
                    ADD(LW(2), 10)
                    CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                    IF_INT_EQUAL(LW(3), -1)
                        GOTO(97)
                    END_IF
                ELSE
                    GOTO(98)
                END_IF
            END_LOOP
        END_IF
    END_IF
    GOTO(98)
    LABEL(97)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_REBOUND1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 45, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 45, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 15, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_togezo), 0)
    CALL(togezo_copy_status, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 50331648)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, LW(3), LW(4), STRING("TGZ_P_1"))
    CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_ENM_TOGEZO_TURN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, LW(3), 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 15, 0)
    LOOP(5)
        CALL(btlevtcmd_AddRotate, LW(3), 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, LW(3), LW(4), STRING("TGZ_S_1"))
    LOOP(5)
        CALL(btlevtcmd_AddRotate, LW(3), 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 0, 0)
    GOTO(99)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 150)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(5.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 50331648)
    WAIT_FRAMES(10)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(friend_event)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_1"))
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_2"))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
        CALL(btlevtcmd_OffPartsCounterAttribute, -2, 1, 1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("JGM_A_3"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_THROW2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 50331648)
    CALL(btlevtcmd_GetPos, -2, LW(10), LW(11), LW(12))
    ADD(LW(11), 35)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(10), LW(11), LW(12))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 45, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 45, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 15, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_JUGEM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_togezo), 0)
    CALL(togezo_copy_status, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 50331648)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AnimeChangePose, LW(3), LW(4), STRING("TGZ_P_1"))
    CALL(btlevtcmd_snd_se, LW(3), STRING("SFX_ENM_TOGEZO_TURN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, LW(3), 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 15, 0)
    LOOP(5)
        CALL(btlevtcmd_AddRotate, LW(3), 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, LW(3), LW(4), STRING("TGZ_S_1"))
    LOOP(5)
        CALL(btlevtcmd_AddRotate, LW(3), 0, 0, -36)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 0, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
