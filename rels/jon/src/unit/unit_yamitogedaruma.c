#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_disp.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[6];
static BattleUnitKindPart parts;
static s32 init_event[21];
static s32 dmg_avoid_counter_thorn_head_event[63];
static s32 dmg_avoid_counter_thorn_front_event[121];
static s32 damage_event[10];
static s32 dmg_turn_event[131];
static s32 attack_event[358];
static s32 wait_event[102];

// clang-format off
extend_data(140,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(141,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_yamitogedaruma = {
    UNIT_DARK_BRISTLE,          // mUnitType
    "btl_un_yamitogedaruma",    // name
    8,                          // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    34,                         // mLevel
    1,                          // mBonusExp
    2,                          // bonusCoinRolls
    70,                         // bonusCoinRate
    2,                          // baseCoin
    50,                         // escapeRate
    9999,                       // field_0x14
    46,                         // width
    40,                         // height
    { 4, 40 },                  // statusIconOffset
    { 0.0f, 20.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },       // talkTogeBaseOffset
    { 23.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 23.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 20.0f, 26.0f, 0.0f },     // kissHitOffset
    { 0.0f, 20.0f, 0.0f },      // cutBaseOffset
    46.0f,                      // cutWidth
    40.0f,                      // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    0,                          // swallowRate
    2,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_ENM_TOGEDA_DAMAGED1",  // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0,                          // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    &parts,                     // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
};

static BattleUnitDefense defence = { { 4, 4, 4, 4, 4 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefense defence_turn = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    20,  // sleepRate
    80,  // stopRate
    20,  // dizzyRate
    100, // poisonRate
    20,  // confuseRate
    100, // electricRate
    100, // burnRate
    40,  // freezeRate
    100, // hugeRate
    50,  // tinyRate
    100, // attackUpRate
    50,  // attackDownRate
    100, // defenseUpRate
    100, // defenseDownRate
    40,  // allergicRate
    30,  // frightRate
    20,  // galeForceRate
    100, // fastRate
    40,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    30,  // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "TGD_N_1" },
    { 0x02, "TGD_I_1" },
    { 0x09, "TGD_I_1" },
    { 0x05, "TGD_K_1" },
    { 0x04, "TGD_X_1" },
    { 0x03, "TGD_X_1" },
    { 0x1C, "TGD_S_4" },
    { 0x1D, "TGD_Q_1" },
    { 0x1E, "TGD_Q_1" },
    { 0x1F, "TGD_S_2" },
    { 0x27, "TGD_D_1" },
    { 0x32, "TGD_A_1" },
    { 0x28, "TGD_S_4" },
    { 0x2A, "TGD_S_4" },
    { 0x38, "TGD_X_1" },
    { 0x39, "TGD_X_1" },
    { 0x41, "TGD_T_1" },
    { 0x45, "TGD_S_4" },
};

static BattleDispPose pose_table_turn[15] = {
    { 0x01, "TGD_N_2" },
    { 0x02, "TGD_I_2" },
    { 0x09, "TGD_Z_4" },
    { 0x05, "TGD_K_2" },
    { 0x04, "TGD_X_2" },
    { 0x03, "TGD_X_2" },
    { 0x1C, "TGD_S_5" },
    { 0x1D, "TGD_Q_2" },
    { 0x1E, "TGD_Q_2" },
    { 0x1F, "TGD_S_3" },
    { 0x27, "TGD_D_2" },
    { 0x38, "TGD_X_2" },
    { 0x39, "TGD_X_2" },
    { 0x41, "TGD_S_5" },
    { 0x45, "TGD_S_5" },
};

static BattleDataEntry data_table[6] = {
    { 0x0D, dmg_turn_event },
    { 0x25, dmg_avoid_counter_thorn_head_event },
    { 0x27, dmg_avoid_counter_thorn_front_event },
    { 0x26, dmg_avoid_counter_thorn_front_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts = {
    1,                       // partNum
    "btl_un_yamitogedaruma", // partName
    "c_togedaruma_b",        // modelName
    { 0.0f, 0.0f, 0.0f },    // positionOffset
    { 0.0f, 35.0f, 0.0f },   // hitBaseOffset
    { 0.0f, 45.0f, 0.0f },   // hitCursorBaseOffset
    20,                      // shadowSize
    30,                      // field_0x32
    255,                     // baseAlpha
    &defence,                // defense
    &defence_attr,           // defenseAttr
    0x00002009,              // attributes
    0x00000007,              // counterAttributes
    pose_table,              // poseTable
};

static BattleWeapon weapon_togedaruma_attack = {
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
    { 8, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000087,                               // special
    0x0000001E,                               // counterResistanceFlags
    0x00002004,                               // targetWeightingFlags
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
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 58)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 32)
        GOTO(90)
    END_IF
    ADD(LW(1), 45)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_front_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_CheckStatus, -2, 17, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_3"))
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_4"))
        ELSE
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_1"))
            WAIT_FRAMES(60)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_2"))
        END_IF
    END_SIBLING_EVENT
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 53)
        ADD(LW(1), 26)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 33)
        ADD(LW(1), 16)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 43)
    ADD(LW(1), 21)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(12))
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("TGD_S_1"))
    IF_INT_LESS_OR_EQUAL(LW(12), 0)
        SET_VALUE(LW(12), 0)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 10, 0)
        BEGIN_SIBLING_EVENT
            LOOP(0)
                IF_INT_GREATER_OR_EQUAL(LW(12), 10)
                    BREAK_LOOP
                END_IF
                ADD(LW(12), 1)
                WAIT_FRAMES(1)
                CALL(btlevtcmd_AddRotate, -2, 0, 0, -18)
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("TGD_S_5"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 10, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 10, LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(&defence))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    SET_VALUE(LW(12), 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 10, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(12), 10)
                BREAK_LOOP
            END_IF
            ADD(LW(12), 1)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_AddRotate, -2, 0, 0, -18)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_ENM_TOGEDA_MOVE6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("TGD_S_4"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_togedaruma_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_togedaruma_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_togedaruma_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_togedaruma_attack))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_1"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE2"), EVTDAT_POINTER_MAX, 0, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_A_1"))
    WAIT_MS(400)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(7.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_togedaruma_attack), 256, LW(5))
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_A_1"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 0, 0, FLOAT(0.1f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), FLOAT(60.0f), -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_N_1"))
    WAIT_MS(1500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_togedaruma_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_togedaruma_attack), 256, LW(5))
    CALL(evt_snd_sfxoff, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_TOGEDA_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(5.0f), FLOAT(0.2f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_J_2"))
    WAIT_FRAMES(15)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("TGD_S_4"))
    WAIT_MS(400)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    LABEL(0)
    SET_VALUE(LW(15), 0)
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 4, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        ADD(LW(15), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_OffPartsCounterAttribute, -2, 1, 2)
    ELSE
        CALL(btlevtcmd_OnPartsCounterAttribute, -2, 1, 2)
    END_IF
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
