#include "unit_boss_rampell.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "driver/animdrv.h"
#include "eff/eff_scanning.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "unit/unit_mario.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
static USER_FUNC(_mario_makkuro_set);
static USER_FUNC(vivian_comeback);
static USER_FUNC(eff_scanning);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[61];
static s32 damage_event[10];
static s32 dead_event[74];
static s32 phase_event[106];
static s32 attack_event[24];
static s32 ogg_attack_event[491];
static s32 wait_event[6];
static s32 init_event_fmario[73];
static s32 dead_event_fmario[93];
static s32 damage_event_fmario[10];
static s32 attack_event_fmario[58];
static s32 marioAttackEvent_NormalJump[344];
static s32 marioAttackEvent_KururinJump[469];
static s32 marioAttackEvent_NormalHammer[299];
static s32 marioAttackEvent_KaitenHammer[299];
static s32 wait_event_fmario[6];

#include "rel/common/eff_scanning.h"
#include "rel/common/vivian_comeback.h"

static USER_FUNC(_mario_makkuro_set) {
    BattleWork* work;
    BattleUnit* unit;

    work = _battleWorkPointer;
    unit = BattleGetUnitPtr(work, BattleTransID(event, -2));
    if (unit != NULL) {
        faker_mario_makkuro_set();
        animPoseSetMaterialFlagOn(BtlUnit_GetPartsPtr(unit, 1)->poseId, 0xB000000);
    }
    return EVT_RETURN_DONE;
}

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

BattleUnitKind unit_boss_rampell = {
    UNIT_DOOPLISS_CHAPTER4_FIGHT1, // mUnitType
    "btl_un_hatena",               // name
    40,                            // maxHp
    0,                             // maxFp
    1,                             // mDangerHP
    1,                             // mPerilHP
    52,                            // mLevel
    0,                             // mBonusExp
    0,                             // bonusCoinRolls
    100,                           // bonusCoinRate
    0,                             // baseCoin
    70,                            // field_0x13
    7,                             // field_0x14
    50,                            // width
    48,                            // height
    { 4, 48 },                     // statusIconOffset
    { 0.0f, 22.0f, 0.0f },         // centerOffset
    { 0, -3 },                     // hpGaugeOffset
    { 0.0f, 40.0f, 0.0f },         // talkTogeBaseOffset
    { 25.0f, 0.0f, -10.0f },       // field_0x3C
    { 0.0f, 0.0f, 0.0f },          // burnFlameOffset
    0.0f,                          // field_0x54
    0.0f,                          // field_0x58
    { 18.0f, 0.0f, 0.0f },         // bintaHitOffset
    { 12.0f, 32.0f, 0.0f },        // kissHitOffset
    { 7.0f, 22.0f, 0.0f },         // cutBaseOffset
    50.0f,                         // cutWidth
    48.0f,                         // cutHeight
    0,                             // turnOrder
    0,                             // turnOrderVariance
    0,                             // swallowRate
    2,                             // swallowAttribute
    100,                           // field_0x8C
    20,                            // itemStealParam
    { 0.0f, 0.0f, 0.0f },          // starPointDispOffset
    "SFX_BOSS_RNPL_DAMAGED1",      // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",        // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",         // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",    // electricDamageSfxName
    0,                             // attributeFlags
    &regist,                       // vulnStatus
    1,                             // numParts
    parts,                         // mParts
    init_event,                    // initEvtScript
    data_table,                    // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    60,  // sleepRate
    60,  // stopRate
    70,  // dizzyRate
    100, // poisonRate
    60,  // confuseRate
    100, // electricRate
    100, // burnRate
    50,  // freezeRate
    100, // hugeRate
    80,  // tinyRate
    100, // attackUpRate
    80,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    60,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    60,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[18] = {
    { 0x01, "N_1" },
    { 0x02, "Y_1" },
    { 0x09, "Y_1" },
    { 0x05, "K_1" },
    { 0x04, "X_1" },
    { 0x03, "K_1" },
    { 0x1C, "S_1" },
    { 0x1D, "Q_1" },
    { 0x1E, "Q_1" },
    { 0x1F, "S_1" },
    { 0x27, "D_1" },
    { 0x32, "A_2" },
    { 0x2A, "R_1" },
    { 0x28, "W_1" },
    { 0x38, "X_1" },
    { 0x39, "X_1" },
    { 0x41, "T_1" },
    { 0x45, "S_1" },
};

static BattleDataEntry data_table[2] = {
    { 0x31, dead_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                     // partNum
        "btl_un_hatena",       // partName
        "c_ranpel",            // modelName
        { 0.0f, 0.0f, 0.0f },  // positionOffset
        { 0.0f, 35.0f, 0.0f }, // hitBaseOffset
        { 0.0f, 40.0f, 0.0f }, // hitCursorBaseOffset
        30,                    // field_0x30
        40,                    // field_0x32
        255,                   // baseAlpha
        &defence,              // defense
        &defence_attr,         // defenseAttr
        0x00000009,            // attributes
        0,                     // counterAttributes
        pose_table,            // poseTable
    },
};

static BattleWeapon weapon_rampell_attack = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01101260,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00011007,                               // special
    0x00000006,                               // counterResistanceFlags
    0x80000000,                               // targetWeightingFlags
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

EVT_BEGIN(rampell_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 90, 65, 332, 90, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()


EVT_BEGIN(to_faker_mario)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 2)
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL_CHILD(rampell_zoom)
    CALL(evt_msg_print, 2, STRING("stg4_jin_08_02"), 0, -2)
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_ARM_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    WAIT_MS(600)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_ARM_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    WAIT_MS(600)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_ARM_UP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_LAUGH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3B"))
    WAIT_MS(800)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_EYE_SHINE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_FLOAT_VALUE(LW(7), FLOAT(-6.0f))
    MULTIPLY_FLOAT(LW(7), LW(6))
    ADD_FLOAT(LW(0), LW(7))
    SET_FLOAT_VALUE(LW(7), FLOAT(33.0f))
    MULTIPLY_FLOAT(LW(7), LW(6))
    ADD_FLOAT(LW(1), LW(7))
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 3, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    SET_FLOAT_VALUE(LW(7), FLOAT(7.0f))
    MULTIPLY_FLOAT(LW(7), LW(6))
    ADD_FLOAT(LW(0), LW(7))
    SET_FLOAT_VALUE(LW(7), FLOAT(-1.0f))
    MULTIPLY_FLOAT(LW(7), LW(6))
    ADD_FLOAT(LW(1), LW(7))
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 3, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(1000)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_TRANSFORM4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 7, LW(0), LW(1), LW(2), FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(200)
    CALL(btlevtcmd_SetPos, -2, -1000, -1000, 0)
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -110, 65, 332, -110, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    CALL(btlevtcmd_GetPos, -3, LW(0), LW(1), LW(2))
    ADD(LW(0), 3)
    ADD(LW(1), -5)
    ADD(LW(2), 5)
    CALL(eff_scanning, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -3, STRING("SFX_BOSS_RNPL_TRANSFORM2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -3, STRING("SFX_BOSS_RNPL_TRANSFORM1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(1300)
    CALL(btlevtcmd_snd_se, -3, STRING("SFX_BOSS_RNPL_TRANSFORM3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(500)
    CALL_CHILD(rampell_zoom)
    CALL_CHILD(init_event_fmario)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_LESS(LW(0), 10)
        CALL(btlevtcmd_SetHp, -2, 10)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_RNPLTRANSFORM5"), 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(5), LW(6), LW(7))
    CALL(evt_sub_intpl_msec_init, 11, -50, 0, 1000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(btlevtcmd_SetPos, -2, LW(5), LW(0), LW(7))
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_msg_print, 2, STRING("stg4_jin_08_03"), 0, -2)
    WAIT_MS(200)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    MULTIPLY(LW(0), 50)
    DIVIDE(LW(0), 100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_RNPL_MOVE1L"), STRING("SFX_BOSS_RNPL_MOVE1R"), 0, 15, 15)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_RNPL_MOVE1L"), STRING("SFX_BOSS_RNPL_MOVE1R"), 0, 8, 8)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL_CHILD(to_faker_mario)
        RETURN
    END_IF
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_wait_move_end)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 90, 65, 332, 90, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    CALL(evt_msg_print, 2, STRING("stg4_jin_08_04"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_K_1"))
    WAIT_FRAMES(192)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_MS(300)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckActStatus, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 2)
                CALL(btlevtcmd_get_turn, LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 5)
                    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                    CALL_CHILD(to_faker_mario)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_IF
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(0)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
                CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                CALL(btlevtcmd_StatusWindowOnOff, 0)
                CALL_CHILD(rampell_zoom)
                CALL(evt_msg_print, 2, STRING("stg4_jin_08_01"), 0, -2)
                WAIT_MS(200)
                CALL(btlevtcmd_StatusWindowOnOff, 1)
                CALL(evt_btl_camera_set_mode, 0, 0)
                CALL(btlevtcmd_StartWaitEvent, -2)
                CASE_EQUAL(1)
                CALL(btlevtcmd_GetHp, -2, LW(0))
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(1))
                IF_INT_LESS(LW(0), LW(1))
                    CALL(btlevtcmd_PhaseEventStartDeclare, -2)
                    CALL_CHILD(to_faker_mario)
                    CALL(btlevtcmd_StartWaitEvent, -2)
                END_IF
            END_SWITCH
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(9), PTR(&weapon_rampell_attack))
    CALL_CHILD(ogg_attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(ogg_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(0), 120)
    ADD(LW(1), 70)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(2.0f))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.5f))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_FLY1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(15)
        CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.0f))
    END_SIBLING_EVENT
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 50, 7, 4, 0, -1)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(0.75f))
    WAIT_MS(200)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
        CALL(evt_sub_intpl_msec_init, 1, 0, 135, 200)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(80)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(2.0f))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.01f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(2.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("D_2"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("O_1"))
    WAIT_MS(800)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_2"))
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.0f))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(2.0f))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 15, 0)
        CALL(evt_sub_intpl_init, 4, 135, 0, 20)
        LOOP(0)
            CALL(evt_sub_intpl_get_value_para, LW(0), LW(1))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("J_1B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(0), 60)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.75f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_RNPL_LANDING1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(2.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("R_1"))
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeSetMotionSpeed, -2, 1, FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static BattleUnitDefense defence_fmario = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr_fmario = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist_fmario = {
    50,  // sleepRate
    50,  // stopRate
    70,  // dizzyRate
    50,  // poisonRate
    50,  // confuseRate
    50,  // electricRate
    100, // burnRate
    40,  // freezeRate
    100, // hugeRate
    70,  // tinyRate
    100, // attackUpRate
    70,  // attackDownRate
    100, // defenseUpRate
    90,  // defenseDownRate
    50,  // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    50,  // slowRate
    100, // dodgyRate
    100, // invisibleRate
    3,   // instaKillRate
};

static BattleDispPose pose_table_fmario_stay[15] = {
    { 0x01, "EM_N_1" },
    { 0x02, "EM_Y_2" },
    { 0x09, "EM_Y_2" },
    { 0x05, "EM_K_1" },
    { 0x04, "EM_K_1" },
    { 0x03, "EM_K_1" },
    { 0x1C, "EM_S_1" },
    { 0x27, "EM_D_1" },
    { 0x32, "EM_A_1" },
    { 0x2A, "EM_R_1" },
    { 0x28, "EM_W_1" },
    { 0x38, "EM_D_3" },
    { 0x39, "EM_D_3" },
    { 0x41, "EM_T_1" },
    { 0x45, "EM_S_1" },
};

static BattleDataEntry data_table_fmario[2] = {
    { 0x31, dead_event_fmario },
    { 0x00, NULL },
};

static BattleUnitKindPart parts_fmario[1] = {
    {
        1,                      // partNum
        "btl_un_hatena",        // partName
        "c_mario",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 3.0f, 27.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // field_0x30
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence_fmario,        // defense
        &defence_attr_fmario,   // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table_fmario_stay, // poseTable
    },
};

static BattleWeapon weapon_masao_jump = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015807,                               // special
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

static BattleWeapon weapon_masao_hammer = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00010807,                               // special
    0x0000020C,                               // counterResistanceFlags
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

static BattleWeapon weapon_masao_kururin_jump = {
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
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015807,                               // special
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

static BattleWeapon weapon_masao_kaiten_hammer = {
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
    { 4, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x21002000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // field_0x6D
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00010807,                               // special
    0x0000020C,                               // counterResistanceFlags
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

static EVT_BEGIN(mario_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, -110, 65, 332, -110, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(enemy_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 115, 65, 332, 115, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(fmario_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 90, 65, 332, 90, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(gparty_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(evt_btl_camera_set_moveto, 0, 130, 65, 332, 130, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()


static EVT_BEGIN(init_event_fmario)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(11), PTR(pose_table_fmario_stay))
    CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table_fmario))
    CALL(btlevtcmd_ReplaceParts, -2, LW(11), PTR(parts_fmario), 1)
    CALL(btlevtcmd_SetRegistStatus, -2, PTR(&regist_fmario))
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_fmario))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_fmario))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_fmario))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_OnUnitFlag, -2, 4)
    CALL(btlevtcmd_ClearAllStatus, -2)
    CALL(_mario_makkuro_set)
    SET_VALUE(GW(17), -1)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_NM_MOVE1L"), STRING("SFX_BOSS_NM_MOVE1R"), 0, 20, 20)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_NM_MOVE1L"), STRING("SFX_BOSS_NM_MOVE1R"), 0, 10, 10)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_BOSS_NM_JUMP1"), STRING("SFX_BOSS_NM_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event_fmario)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_wait_move_end)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 90, 65, 332, 90, 30, -340, 30, 11)
    WAIT_FRAMES(37)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(300)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("EM_T_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("EM_S_1"))
    CALL(evt_msg_print, 2, STRING("stg4_jin_08_04"), 0, -2)
    WAIT_MS(300)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_K_1"))
    WAIT_FRAMES(152)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_NM_LAND_DAMAGE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_MS(1500)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_fmario)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_fmario)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 2, 50, 50)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(9), PTR(&weapon_masao_jump))
        CALL_CHILD(marioAttackEvent_NormalJump)
        CASE_EQUAL(1)
        SET_VALUE(LW(9), PTR(&weapon_masao_hammer))
        CALL_CHILD(marioAttackEvent_NormalHammer)
        CASE_EQUAL(2)
        SET_VALUE(LW(9), PTR(&weapon_masao_kururin_jump))
        CALL_CHILD(marioAttackEvent_KururinJump)
        CASE_EQUAL(3)
        SET_VALUE(LW(9), PTR(&weapon_masao_kaiten_hammer))
        CALL_CHILD(marioAttackEvent_KaitenHammer)
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(jump_attack_voice)
    CALL(evt_sub_random, 2, LW(15))
    SWITCH(LW(15))
        CASE_EQUAL(0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CASE_EQUAL(2)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_VOICE_NM_AC1_5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_NormalJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL_CHILD(jump_attack_voice)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_FRAMES(40)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KururinJump)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL_CHILD(jump_attack_voice)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_DOWN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_D_2"))
    WAIT_FRAMES(40)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
        CALL_CHILD(jump_attack_voice)
        CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(15)
    DELETE_EVENT(LW(14))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
    MULTIPLY(LW(15), -30)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_Z_2"))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_JUMP_KURURIN1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(12)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    WAIT_FRAMES(15)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 27)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_NormalHammer)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 24)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1A"))
    WAIT_MS(66)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_FURU2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(233)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1B"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1C"))
    WAIT_MS(50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_DON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(100)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(200)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(marioAttackEvent_KaitenHammer)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 24)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    ADD(LW(2), 5)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1A"))
    WAIT_MS(66)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_FURU2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(233)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1B"))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_A_1C"))
    WAIT_MS(50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_NM_HAMMER_DON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_MS(100)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(98)
    WAIT_MS(700)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_S_1"))
    WAIT_MS(200)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_W_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_fmario)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

