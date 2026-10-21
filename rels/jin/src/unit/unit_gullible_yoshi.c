#include "battle/battle_camera.h"
#include "battle/battle_database_common.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "unit/party/unit_party_yoshi.h"
#include "battle/battle_disp.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern BattleDispPose pose_table_yoshi_stay[];
extern s32 weaponGetPowerFromGulliblePartyAttackLv[];
static BattleVulnerableStatus regist_yoshi;
static BattleDataEntry data_table_yoshi[2];
static BattleUnitKindPart parts_yoshi;
static s32 init_event_yoshi[24];
s32 damage_event_yoshi[22];
static s32 dead_event_yoshi[177];
static s32 attack_event_yoshi[748];
static s32 wait_event_yoshi[7];

// clang-format off
extend_data(68,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(69,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_gullible_yoshi = {
    UNIT_YOSHI_CHAPTER4,        // mUnitType
    "btl_un_gullible_yoshi",    // name
    10,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    9,                          // field_0x14
    24,                         // width
    28,                         // height
    { 4, 24 },                  // statusIconOffset
    { 0.0f, 14.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },      // talkTogeBaseOffset
    { 12.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 12.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 18.2f, 0.0f },     // kissHitOffset
    { 0.0f, 14.0f, 0.0f },      // cutBaseOffset
    24.0f,                      // cutWidth
    28.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_PARTY_DAMAGE1",        // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x10000000,                 // attributeFlags
    &regist_yoshi,              // vulnStatus
    1,                          // numParts
    &parts_yoshi,               // mParts
    init_event_yoshi,           // initEvtScript
    data_table_yoshi,           // dataTable
};

static BattleWeapon enemyWeapon_YoshiNormalAttack = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_NULL,                                // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    1,                                        // superguardType
    1.0f,                                     // field_0x14
    1,                                        // mStylishCommandMultiplier
    1,                                        // field_0x19
    1,                                        // mBingoSlotChance
    1,                                        // field_0x1B
    weaponGetPowerFromGulliblePartyAttackLv,  // hpDamageFn
    { 1, 1, 1, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015097,                               // special
    0x0000000C,                               // counterResistanceFlags
    0x80002004,                               // targetWeightingFlags
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

static BattleUnitDefense defence_yoshi = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_yoshi = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_yoshi = {
    60,  // sleepRate
    80,  // stopRate
    70,  // dizzyRate
    100, // poisonRate
    70,  // confuseRate
    100, // electricRate
    100, // burnRate
    70,  // freezeRate
    100, // hugeRate
    70,  // tinyRate
    100, // attackUpRate
    70,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    70,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    70,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDataEntry data_table_yoshi[2] = {
    { 0x31, dead_event_yoshi },
    { 0x00, NULL },
};

static BattleUnitKindPart parts_yoshi = {
    1,                      // partNum
    "btl_un_yoshi",         // partName
    "c_babyyoshi",          // modelName
    { 0.0f, 0.0f, 0.0f },   // positionOffset
    { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
    { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
    20,                     // field_0x30
    30,                     // field_0x32
    255,                    // baseAlpha
    &defence_yoshi,         // defense
    &defence_attr_yoshi,    // defenseAttr
    0x00000009,             // attributes
    0,                      // counterAttributes
    pose_table_yoshi_stay,  // poseTable
};

static EVT_BEGIN(init_event_yoshi)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_yoshi))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_yoshi))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_yoshi))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(yoshi_original_color_anim_set, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(damage_event_yoshi)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(20), 1)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event_yoshi)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    SET_VALUE(LW(6), 1)
    IF_INT_EQUAL(LW(0), LW(3))
        IF_INT_EQUAL(LW(1), LW(4))
            IF_INT_EQUAL(LW(2), LW(5))
                SET_VALUE(LW(6), 0)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    SWITCH(GW(19))
        CASE_EQUAL(0)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 130, 58, 228, 130, 38, -20, 30, 11)
        WAIT_FRAMES(37)
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_03_yos"), 0, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(500)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 90, 58, 228, 90, 38, -20, 30, 11)
        WAIT_FRAMES(37)
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_04"), 0, GW(16))
        WAIT_MS(200)
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(0)
            IF_INT_EQUAL(GW(19), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    CALL(btlevtcmd_KillUnit, -2, 1)
    SET_VALUE(GW(20), 2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_yoshi)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(&enemyWeapon_YoshiNormalAttack))
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    WAIT_FRAMES(20)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    BEGIN_INLINE_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(2)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(3)
        END_LOOP
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_R_5"))
    WAIT_FRAMES(25)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1A"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    ADD(LW(1), 35)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_3"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    ADD(LW(7), 40)
    SET_VALUE(LW(10), FLOAT(1.0f))
    BEGIN_INLINE_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 190, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 381, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 668, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 859, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 1145, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 1241, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 1527, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 1814, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 2100, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 2196, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), EVTDAT_POINTER_MAX, 1742, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(6)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_MOVE1R"), EVTDAT_POINTER_MAX, 1527, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
        END_LOOP
    END_INLINE_EVENT
    CALL(btl_yoshi_yoroyoro_jump_calc_param, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    CALL(btl_yoshi_yoroyoro_jump_move, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    DELETE_EVENT(LW(13))
    CALL(evt_sub_random, 4, LW(11))
    ADD(LW(11), 1)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    IF_INT_GREATER_OR_EQUAL(LW(10), 1)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
    END_SWITCH
    IF_INT_LESS_OR_EQUAL(LW(10), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_A_2B"))
        WAIT_FRAMES(20)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1A"))
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1B"))
        SET_VALUE(LW(0), 1)
        SET_VALUE(LW(1), 9)
        MULTIPLY(LW(15), -40)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        WAIT_FRAMES(15)
        LOOP(LW(0))
            SET_VALUE(LW(2), LW(1))
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_YOSHI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            LOOP(LW(2))
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
                WAIT_FRAMES(1)
            END_LOOP
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
        WAIT_FRAMES(15)
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(15), LW(1))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        SUBTRACT(LW(15), LW(1))
        DIVIDE(LW(15), 10)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 0, 0, 0, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_1"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, -7, 0)
        CALL(evt_btl_camera_shake_h, 0, 4, 0, 10, 13)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, 0, 0)
        WAIT_FRAMES(54)
        GOTO(95)
    END_IF
    IF_INT_LESS(LW(10), LW(11))
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 0, LW(5))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
        ADD(LW(1), 40)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        ADD(LW(10), 1)
        GOTO(10)
    ELSE
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    LABEL(90)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_1"))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(20)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_yoshi)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
