#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(krb_get_dir);
static BattleDataEntry data_table[2];
static BattleVulnerableStatus regist;
static BattleUnitKindPart parts[1];
static s32 init_event[46];
static s32 damage_event[10];
static s32 phase_event[122];
static s32 attack_event[492];
static s32 wait_event[6];

// clang-format off
extend_data(84,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(85,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_kurikuri = {
    UNIT_GOOMBA_GLITZVILLE,     // mUnitType
    "btl_un_kuriboo",           // name
    2,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    7,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    0,                          // baseCoin
    50,                         // escapeRate
    9999,                       // field_0x14
    23,                         // width
    25,                         // height
    { 5, 25 },                  // statusIconOffset
    { 0.0f, 12.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 12.0f, 0.0f, -10.0f },    // field_0x3C
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 11.5f, 0.0f, 0.0f },      // bintaHitOffset
    { 12.0f, 18.0f, 0.0f },     // kissHitOffset
    { 0.0f, 12.0f, 0.0f },      // cutBaseOffset
    23.0f,                      // cutWidth
    25.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_KURI_DAMAGED1",    // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x00040000,                 // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleDispPose pose_table[20] = {
    { 0x01, "KUR_N_1" },
    { 0x02, "KUR_Y_1" },
    { 0x09, "KUR_Y_1" },
    { 0x05, "KUR_K_1" },
    { 0x04, "KUR_I_1" },
    { 0x03, "KUR_I_1" },
    { 0x1C, "KUR_S_1" },
    { 0x1D, "KUR_Q_1" },
    { 0x1E, "KUR_Q_1" },
    { 0x1F, "KUR_S_1" },
    { 0x27, "KUR_D_1" },
    { 0x32, "KUR_A_1" },
    { 0x33, "KUR_A_2" },
    { 0x2A, "KUR_R_1" },
    { 0x29, "KUR_R_2" },
    { 0x28, "KUR_W_1" },
    { 0x38, "KUR_I_1" },
    { 0x39, "KUR_I_1" },
    { 0x41, "KUR_T_1" },
    { 0x45, "KUR_S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    100, // sleepRate
    110, // stopRate
    100, // dizzyRate
    100, // poisonRate
    100, // confuseRate
    100, // electricRate
    100, // burnRate
    100, // freezeRate
    100, // hugeRate
    100, // tinyRate
    100, // attackUpRate
    100, // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    100, // allergicRate
    110, // frightRate
    100, // galeForceRate
    100, // fastRate
    100, // slowRate
    100, // dodgyRate
    100, // invisibleRate
    150, // instaKillRate
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_kuriboo",      // partName
        "c_kuribo",            // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 22.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 30.0f, 0.0f }, // hitCursorBaseOffset
        20,                    // field_0x30
        30,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

static BattleWeapon weapon_kuriboo_attack = {
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
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20001000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00005087,                               // special
    0,                                        // counterResistanceFlags
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

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_ENM_KURI_JUMP1"), STRING("SFX_ENM_KURI_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        IF_INT_EQUAL(GSWF(2509), 0)
            CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_get_turn, LW(0))
                IF_INT_EQUAL(LW(0), 1)
                    CALL(btlevtcmd_PhaseEventStartDeclare, -4)
                    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                    SET_VALUE(GSWF(2509), 1)
                    CALL(evt_btl_camera_set_prilimit, 1)
                    CALL(evt_btl_camera_set_mode, 1, 3)
                    CALL(evt_btl_camera_set_moveto, 1, -104, 74, 334, -102, 52, 0, 30, 4)
                    WAIT_FRAMES(30)
                    CALL(btlevtcmd_GetUnitKind, -4, LW(0))
                    SWITCH(LW(0))
                        CASE_EQUAL(224)
                        CALL(evt_msg_print, 2, STRING("stg3_tou_83_kur"), 0, -4)
                        CASE_EQUAL(225)
                        CALL(evt_msg_print, 2, STRING("stg3_tou_83_nok"), 0, -4)
                        CASE_EQUAL(227)
                        CALL(evt_msg_print, 2, STRING("stg3_tou_83_win"), 0, -4)
                    END_SWITCH
                    CALL(evt_btl_camera_set_moveto, 1, 91, 74, 334, 91, 52, 0, 30, 4)
                    WAIT_FRAMES(30)
                    CALL(evt_msg_print, 2, STRING("stg3_tou_84"), 0, -2)
                    CALL(evt_btl_camera_set_prilimit, 0)
                    CALL(evt_btl_camera_set_mode, 0, 0)
                    CALL(btlevtcmd_StartWaitEvent, -4)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_IF
        END_IF
    END_IF
    RETURN
EVT_END()

#include "rel/common/krb_get_dir.h"

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_kuriboo_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_kuriboo_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.6f))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_kuriboo_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_kuriboo_attack))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 100)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        SET_VALUE(LW(8), 0)
        LOOP(32)
            CALL(btlevtcmd_GetPosFloat, -2, LW(9), LW(10), LW(11))
            CALL(krb_get_dir, LW(5), LW(6), LW(9), LW(10), LW(8))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(8))
            SET_FLOAT_VALUE(LW(5), LW(9))
            SET_FLOAT_VALUE(LW(6), LW(10))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        LOOP(12)
            WAIT_FRAMES(1)
        END_LOOP
        SET_VALUE(LW(15), 50)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    END_INLINE_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 32, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_kuriboo_attack), 256, LW(5))
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
        GOTO(91)
        CASE_END
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(15), 69)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_MoveDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    WAIT_MS(500)
    GOTO(95)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KURI_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_kuriboo_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_kuriboo_attack), 256, LW(5))
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(15), 69)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, LW(15))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
    GOTO(95)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_ENM_KURI_MOVE1"), STRING("SFX_ENM_KURI_MOVE2"), 0, 3, 3, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 41)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
