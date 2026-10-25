#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_sub.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "math_trig.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern BattleUnitKind unit_super_killer;
static USER_FUNC(killer_get_angle);
static USER_FUNC(killer_copy_status);
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[3];
static BattleDispPose pose_table[16];
static BattleDataEntry data_table[1];
static s32 init_event[63];
static s32 damage_event[10];
static s32 attack_event[738];
static s32 friend_event[244];
static s32 friend_event_sky[318];
static s32 wait_event[6];

// clang-format off
extend_data(172,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(173,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_super_killer_cannon = {
    UNIT_BOMBSHELL_BILL_BLASTER,     // mUnitType
    "btl_un_super_killer_cannon",    // name
    10,                              // maxHp
    0,                               // maxFp
    1,                               // mDangerHP
    1,                               // mPerilHP
    31,                              // mLevel
    0,                               // mBonusExp
    1,                               // bonusCoinRolls
    70,                              // bonusCoinRate
    3,                               // baseCoin
    50,                              // escapeRate
    9999,                            // renzokuSoftCap
    52,                              // width
    38,                              // height
    { 5, 38 },                       // statusIconOffset
    { 0.0f, 19.0f, 0.0f },           // centerOffset
    { 0, 0 },                        // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },            // talkTogeBaseOffset
    { 26.0f, 0.0f, -10.0f },         // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },            // burnFlameOffset
    20.0f,                           // kagenukeOffsetX
    10.0f,                           // kagenukeOffsetZ
    { 26.0f, 0.0f, 0.0f },           // bintaHitOffset
    { 26.0f, 24.7f, 0.0f },          // kissHitOffset
    { -10.0f, 19.0f, 0.0f },         // cutBaseOffset
    52.0f,                           // cutWidth
    38.0f,                           // cutHeight
    0,                               // turnOrder
    0,                               // turnOrderVariance
    0,                               // swallowRate
    2,                               // swallowAttribute
    100,                             // shotDamageRate
    20,                              // itemStealParam
    { 0.0f, 0.0f, 0.0f },            // starPointDispOffset
    "SFX_ENM_KILLER_CANON_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",          // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",           // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",      // electricDamageSfxName
    0,                               // attributeFlags
    &regist,                         // vulnStatus
    3,                               // numParts
    parts,                           // mParts
    init_event,                      // initEvtScript
    data_table,                      // dataTable
};

static BattleUnitDefense defence = { { 4, 4, 4, 4, 4 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    0,   // stopRate
    0,   // dizzyRate
    0,   // poisonRate
    0,   // confuseRate
    150, // electricRate
    100, // burnRate
    90,  // freezeRate
    100, // hugeRate
    80,  // tinyRate
    100, // attackUpRate
    80,  // attackDownRate
    100, // defenseUpRate
    95,  // defenseDownRate
    40,  // allergicRate
    0,   // frightRate
    5,   // galeForceRate
    100, // fastRate
    40,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    90,  // instaKillRate
};

static BattleUnitKindPart parts[3] = {
    {
        1,                            // partNum
        "btl_un_super_killer_cannon", // partName
        "c_killtai_g",                // modelName
        { 0.0f, 0.0f, 0.0f },         // positionOffset
        { 0.0f, 30.0f, 0.0f },        // hitBaseOffset
        { 0.0f, 35.0f, 0.0f },        // hitCursorBaseOffset
        35,                           // shadowSize
        30,                           // field_0x32
        255,                          // baseAlpha
        &defence,                     // defense
        &defence_attr,                // defenseAttr
        0x00000009,                   // attributes
        0,                            // counterAttributes
        pose_table,                   // poseTable
    },
    {
        2,                            // partNum
        "btl_un_super_killer_cannon", // partName
        "c_killtai_g",                // modelName
        { 0.0f, 0.0f, 0.0f },         // positionOffset
        { 0.0f, 0.0f, 0.0f },         // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },         // hitCursorBaseOffset
        0,                            // shadowSize
        0,                            // field_0x32
        255,                          // baseAlpha
        &defence,                     // defense
        &defence_attr,                // defenseAttr
        0x00010000,                   // attributes
        0,                            // counterAttributes
        pose_table,                   // poseTable
    },
    {
        3,                            // partNum
        "btl_un_super_killer_cannon", // partName
        "c_killtai_g",                // modelName
        { 0.0f, 0.0f, 0.0f },         // positionOffset
        { 0.0f, 0.0f, 0.0f },         // hitBaseOffset
        { 0.0f, 0.0f, 0.0f },         // hitCursorBaseOffset
        0,                            // shadowSize
        0,                            // field_0x32
        255,                          // baseAlpha
        &defence,                     // defense
        &defence_attr,                // defenseAttr
        0x00010000,                   // attributes
        0,                            // counterAttributes
        pose_table,                   // poseTable
    },
};

static BattleUnitSetup entry_killer[1] = {
    { &unit_super_killer, 1, 0x04000004, { -1000.0f, -1000.0f, -1000.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static BattleDispPose pose_table[16] = {
    { 0x01, "KTA_N_1" },
    { 0x02, "KTA_Y_1" },
    { 0x09, "KTA_Y_1" },
    { 0x05, "KTA_K_1" },
    { 0x04, "KTA_X_1" },
    { 0x03, "KTA_X_1" },
    { 0x1C, "KTA_S_1" },
    { 0x1D, "KTA_Q_1" },
    { 0x1E, "KTA_Q_1" },
    { 0x1F, "KTA_A_1" },
    { 0x27, "KTA_D_1" },
    { 0x38, "KTA_X_1" },
    { 0x39, "KTA_X_1" },
    { 0x32, "KTA_A_1" },
    { 0x41, "KTA_T_1" },
    { 0x45, "KTA_S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_AddPos, -2, 20, 0, 0)
    CALL(btlevtcmd_AddHomePos, -2, 20, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KTA_Z_2"))
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(0), 15)
    ADD(LW(0), 0)
    ADD(LW(0), 10)
    ADD(LW(0), 20)
    ADD(LW(0), 0)
    ADD(LW(0), 10)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 15)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 0)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 1)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event_sky)
                    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
                    LOOP(0)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 2)
                        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 0)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 1)
                    CALL_CHILD(friend_event_sky)
                    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
                    LOOP(0)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 2)
                        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event_sky)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                BREAK_LOOP
            END_IF
            ADD(LW(11), 2)
            CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

#include "rel/common/killer_copy_status.h"

static EVT_BEGIN(friend_event)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_2A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_Z_1"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_2A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_Z_1"))
        WAIT_FRAMES(10)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_1"))
    WAIT_FRAMES(40)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 70)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 50, LW(9), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 28)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 15, LW(9), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 40)
    ADD(LW(9), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 30, LW(9), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    GOTO(10)
    LABEL(10)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_killer), 0)
    CALL(killer_copy_status, -2, LW(3))
    CALL(btlevtcmd_SetPos, LW(3), LW(7), 10, LW(2))
    CALL(btlevtcmd_SetFallAccel, LW(3), 0)
    CALL(btlevtcmd_SetJumpSpeed, LW(3), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), 10, LW(2), FLOAT(7.0f), -1)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), 10, LW(2))
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_OnOffStatus, LW(3), 16, 1, 1, 1)
    END_IF
    RETURN
EVT_END()

#include "rel/common/killer_get_angle.h"

static EVT_BEGIN(friend_event_sky)
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(evt_sub_get_dir, LW(7), LW(8), LW(0), 40, LW(10))
    CALL(killer_get_angle, LW(7), LW(8), LW(0), 40, LW(10))
    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
    LOOP(0)
        IF_INT_GREATER(LW(11), 0)
            BREAK_LOOP
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(11), LW(10))
            BREAK_LOOP
        END_IF
        SUBTRACT(LW(11), 2)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
        WAIT_FRAMES(10)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3"))
    WAIT_FRAMES(40)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 60)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 75, LW(9), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 28)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 20, LW(9), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 40)
    ADD(LW(9), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 38, LW(9), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    GOTO(10)
    LABEL(10)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_killer), 0)
    CALL(killer_copy_status, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_SetPos, LW(3), LW(7), 10, LW(2))
    CALL(btlevtcmd_SetPartsRotate, LW(3), LW(4), 0, 0, LW(10))
    CALL(btlevtcmd_SetFallAccel, LW(3), 0)
    CALL(btlevtcmd_SetJumpSpeed, LW(3), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), 40, LW(2), FLOAT(7.0f), -1)
    CALL(btlevtcmd_SetPartsRotate, LW(3), LW(4), 0, 0, 0)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), 40, LW(2))
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_OnOffStatus, LW(3), 16, 1, 1, 1)
    END_IF
    CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 4194304)
    CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 2097152)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
