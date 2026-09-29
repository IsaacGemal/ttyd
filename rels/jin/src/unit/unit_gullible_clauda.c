#include "battle/battle_camera.h"
#include "battle/battle_database_common.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_unit.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle_disp.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern BattleDispPose pose_table_clauda_stay[];
extern s32 weaponGetPowerFromGulliblePartyAttackLv[];
static BattleVulnerableStatus regist_clauda;
static BattleDataEntry data_table_clauda[2];
static BattleUnitKindPart parts_clauda;
static s32 init_event_clauda[50];
s32 damage_event_clauda[22];
static s32 dead_event_clauda[177];
static s32 attack_event_clauda[393];
static s32 wait_event_clauda[6];

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_gullible_clauda = {
    UNIT_FLURRIE_CHAPTER4,      // mUnitType
    "btl_un_gullible_clauda",   // name
    10,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    0,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    100,                        // bonusCoinRate
    0,                          // baseCoin
    50,                         // field_0x13
    9,                          // field_0x14
    42,                         // width
    46,                         // height
    { 12, 40 },                 // statusIconOffset
    { 0.0f, 23.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },      // talkTogeBaseOffset
    { 21.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 21.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 21.0f, 29.9f, 0.0f },     // kissHitOffset
    { 0.0f, 23.0f, 0.0f },      // cutBaseOffset
    42.0f,                      // cutWidth
    46.0f,                      // cutHeight
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
    &regist_clauda,             // vulnStatus
    1,                          // numParts
    &parts_clauda,              // mParts
    init_event_clauda,          // initEvtScript
    data_table_clauda,          // dataTable
};

static BattleWeapon enemyWeapon_ClaudaNormalAttack = {
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
    { 2, 4, 6, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    10,                                       // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00014087,                               // special
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

static BattleUnitDefense defence_clauda = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_clauda = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_clauda = {
    60,  // sleepRate
    80,  // stopRate
    105, // dizzyRate
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

static BattleDataEntry data_table_clauda[2] = {
    { 0x31, dead_event_clauda },
    { 0x00, NULL },
};

static BattleUnitKindPart parts_clauda = {
    1,                      // partNum
    "btl_un_clauda",        // partName
    "c_windy",              // modelName
    { 0.0f, 0.0f, 0.0f },   // positionOffset
    { 15.0f, 25.0f, 0.0f }, // hitBaseOffset
    { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
    20,                     // field_0x30
    30,                     // field_0x32
    255,                    // baseAlpha
    &defence_clauda,        // defense
    &defence_attr_clauda,   // defenseAttr
    0x00000009,             // attributes
    0,                      // counterAttributes
    pose_table_clauda_stay, // poseTable
};

static EVT_BEGIN(init_event_clauda)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_clauda))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_clauda))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_clauda))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 0, 9, 3)
    CALL(btlevtcmd_SetDiveSound, -2, STRING("SFX_PARTY_BATTLE_HANG1"), STRING("SFX_PARTY_BATTLE_HANG1"), 0, 9, 3)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(damage_event_clauda)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(20), 1)
    END_IF
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event_clauda)
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
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_03_win"), 0, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(500)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 90, 58, 228, 90, 38, -20, 30, 11)
        WAIT_FRAMES(37)
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_04"), 0, GW(16))
        WAIT_MS(200)
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(0)
            IF_INT_EQUAL(GW(19), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    CALL(btlevtcmd_KillUnit, -2, 1)
    SET_VALUE(GW(20), 2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_clauda)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(&enemyWeapon_ClaudaNormalAttack))
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
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(400)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_GetResultAC, LW(5))
    IF_BITS_CLEAR(LW(5), 2)
        CALL(evt_sub_random, 40, LW(5))
        SUBTRACT(LW(5), 20)
        ADD(LW(0), LW(5))
        CALL(evt_sub_random, 20, LW(5))
        SUBTRACT(LW(5), 10)
        ADD(LW(1), LW(5))
    END_IF
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 60, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_A1_2"))
        CALL(btlevtcmd_JumpContinue, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PWD_D_3"))
        WAIT_FRAMES(30)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_CLAUD_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(evt_btl_camera_shake_h, 0, 8, 0, 30, 13)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.4f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(0.4f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(0), LW(5), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(5), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, -2, LW(5), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    WAIT_MS(700)
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

static EVT_BEGIN(wait_event_clauda)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
