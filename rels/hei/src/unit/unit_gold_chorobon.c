#include "unit_gold_chorobon.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
extern s32 chorogun_appear[];
extern s32 chorogun_entry[];
extern s32 chorogun_escape_all[];
extern BattleUnitKind unit_chorobon_gundan;
extern s32 unk_evt_hei_00019e30[];
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[15];
static BattleUnitKindPart parts[1];
static s32 init_event[43];
static s32 damage_event[131];
static s32 counter_damage_event[30];
static s32 return_event[24];
static s32 attack_event[500];
static s32 wait_event[6];

// clang-format off
extend_data(76,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(77,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_gold_chorobon = {
    UNIT_GOLD_FUZZY,             // mUnitType
    "btl_un_gold_chorobon",      // name
    10,                          // maxHp
    0,                           // maxFp
    1,                           // mDangerHP
    1,                           // mPerilHP
    27,                          // mLevel
    0,                           // mBonusExp
    10,                          // bonusCoinRolls
    100,                         // bonusCoinRate
    0,                           // baseCoin
    50,                          // escapeRate
    5,                           // renzokuSoftCap
    28,                          // width
    32,                          // height
    { 5, 32 },                   // statusIconOffset
    { 0.0f, 16.0f, 0.0f },       // centerOffset
    { 0, 0 },                    // hpGaugeOffset
    { 0.0f, 0.0f, 0.0f },        // talkTogeBaseOffset
    { 14.0f, 0.0f, -10.0f },     // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },        // burnFlameOffset
    0.0f,                        // field_0x54
    0.0f,                        // field_0x58
    { 14.0f, 0.0f, 0.0f },       // bintaHitOffset
    { 14.0f, 20.8f, 0.0f },      // kissHitOffset
    { 0.0f, 16.0f, 0.0f },       // cutBaseOffset
    28.0f,                       // cutWidth
    32.0f,                       // cutHeight
    0,                           // turnOrder
    0,                           // turnOrderVariance
    0,                           // swallowRate
    2,                           // swallowAttribute
    100,                         // field_0x8C
    20,                          // itemStealParam
    { 0.0f, 0.0f, 0.0f },        // starPointDispOffset
    "SFX_BOSS_G_CHORO_DAMAGED1", // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",      // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",       // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",  // electricDamageSfxName
    0x00040000,                  // attributeFlags
    &regist,                     // vulnStatus
    1,                           // numParts
    parts,                       // mParts
    init_event,                  // initEvtScript
    data_table,                  // dataTable
};

static BattleUnitDefense defence = { { 0, 0, 0, 0, 0 } };

static BattleUnitDefenseAttr defence_attr = { { 0, 0, 0, 0, 0 } };

static BattleVulnerableStatus regist = {
    0,   // sleepRate
    0,   // stopRate
    0,   // dizzyRate
    100, // poisonRate
    0,   // confuseRate
    100, // electricRate
    100, // burnRate
    0,   // freezeRate
    100, // hugeRate
    0,   // tinyRate
    100, // attackUpRate
    0,   // attackDownRate
    100, // defenseUpRate
    0,   // defenseDownRate
    0,   // allergicRate
    0,   // frightRate
    0,   // galeForceRate
    100, // fastRate
    0,   // slowRate
    100, // dodgyRate
    100, // invisibleRate
    0,   // instaKillRate
};

static BattleDispPose pose_table[18] = {
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
    { 0x38, "CBN_X_1" },
    { 0x39, "CBN_X_1" },
    { 0x41, "CBN_T_1" },
    { 0x45, "CBN_S_1" },
};

static BattleDataEntry data_table[15] = {
    { 0x18, counter_damage_event },
    { 0x1A, counter_damage_event },
    { 0x19, counter_damage_event },
    { 0x1B, counter_damage_event },
    { 0x1C, counter_damage_event },
    { 0x1D, counter_damage_event },
    { 0x1E, counter_damage_event },
    { 0x1F, counter_damage_event },
    { 0x20, counter_damage_event },
    { 0x21, counter_damage_event },
    { 0x22, counter_damage_event },
    { 0x23, counter_damage_event },
    { 0x24, counter_damage_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_gold_chorobon", // partName
        "c_chorobon_k",         // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { 0.0f, 25.0f, 0.0f },  // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },  // hitCursorBaseOffset
        20,                     // shadowSize
        30,                     // field_0x32
        255,                    // baseAlpha
        &defence,               // defense
        &defence_attr,          // defenseAttr
        0x00000009,             // attributes
        0,                      // counterAttributes
        pose_table,             // poseTable
    },
};

BattleWeapon weapon_gold_chorobon_attack = {
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
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00005007,                               // special
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

static BtlUnitPoseSound pose_sound_timing_table[4] = {
    { "CBN_S_1", 0.5833333f, NULL, "SFX_BOSS_G_CHORO_MOVE1", 0, 1 },
    { "CBN_S_1", 0.8333333f, NULL, "SFX_BOSS_G_CHORO_WAIT2", 0, 1 },
    { "CBN_S_1", 1.5833334f, NULL, "SFX_BOSS_G_CHORO_WAIT1", 0, 1 },
    { NULL, 0.0f, NULL, NULL, 0, 1 },
};

EVT_BEGIN(gold_chorobon_rabbit_move)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LABEL(0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(11), LW(5))
    SUBTRACT(LW(11), LW(0))
    SET_VALUE(LW(12), LW(11))
    IF_INT_LESS(LW(12), 0)
        MULTIPLY(LW(12), -1)
    END_IF
    IF_INT_GREATER(LW(12), LW(8))
        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
            ADD(LW(0), LW(8))
        ELSE
            SUBTRACT(LW(0), LW(8))
        END_IF
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(6), LW(2), 0, -1)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
        GOTO(0)
    ELSE
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(5), LW(6), LW(7), 0, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_G_CHORO_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(2)
            CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_IF
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

BattleUnitSetup entry_chorogun[1] = {
    { &unit_chorobon_gundan, 1, 0x04000004, { 75.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

EVT_BEGIN(call_chorogun_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_S_1"))
    WAIT_MS(1000)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 120, 40, 200, 120, -10, -500, 30, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_reset_move_color_lv_all)
    CALL(evt_msg_print, 2, STRING("stg1_hei_43"), 0, -2)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 1, 0)
    CALL(evt_btl_camera_set_prilimit, 0)
    WAIT_MS(1000)
    BEGIN_SIBLING_EVENT
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePose, -3, 1, STRING("M_N_7"))
    END_SIBLING_EVENT
    CALL(chorogun_appear)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -3, 1)
    RETURN
EVT_END()

USER_FUNC(get_gold_chorobon_id) {
    s32* args = event->args;
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    s32 id = 0;

    while (id < 0x40) {
        unit = BattleGetUnitPtr(work, id);
        if (unit != NULL && unit->initialKind == UNIT_GOLD_FUZZY) {
            break;
        }
        id++;
    }
    if (id != 0x40) {
        evtSetValue(event, args[0], id);
    } else {
        evtSetValue(event, args[0], -1);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(escape_chorogun_event)
    WAIT_MS(2000)
    CALL(get_gold_chorobon_id, LW(4))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, LW(4), 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_SetUnitWork, LW(4), 1, 1)
    CALL(btlevtcmd_GetUnitId, -2, LW(10))
    CALL_CHILD(unk_evt_hei_00019e30)
    CALL(chorogun_escape_all)
    WAIT_MS(1000)
    CALL(btlevtcmd_GetUnitWork, LW(4), 2, LW(3))
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL_CHILD(chorogun_entry)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_BOSS_G_CHORO_MOVE1"), STRING("SFX_BOSS_G_CHORO_LANDING1"))
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    CALL(btlevtcmd_GetUnitId, LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(1), -1)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
            IF_INT_NOT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
                IF_INT_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
                        CALL(btlevtcmd_WaitAttackEnd)
                        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(10))
                        CALL_CHILD(unk_evt_hei_00019e30)
                        SET_VALUE(LW(10), -2)
                        CALL(chorogun_escape_all)
                        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
                        CALL(btlevtcmd_KillUnit, LW(3), 0)
                    END_IF
                END_IF
            END_IF
        END_IF
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 7)
            IF_INT_GREATER_OR_EQUAL(LW(0), 1)
                CALL(btlevtcmd_WaitAttackEnd)
                CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_chorogun), 0)
                CALL(btlevtcmd_SetUnitWork, -2, 2, LW(3))
                CALL_CHILD(call_chorogun_event)
                CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
                GOTO(99)
            END_IF
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    CALL_CHILD(subsetevt_counter_damage)
    CALL(btlevtcmd_GetHp, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(return_event)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(return_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetHomePos, -2, LW(5), LW(6), LW(7))
    SET_VALUE(LW(8), 40)
    CALL_CHILD(gold_chorobon_rabbit_move)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(&weapon_gold_chorobon_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(&weapon_gold_chorobon_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(&weapon_gold_chorobon_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(&weapon_gold_chorobon_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(4.0f), FLOAT(0.6f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(5), LW(6), LW(7))
    SET_VALUE(LW(6), 0)
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(5), 50)
    SET_VALUE(LW(8), 50)
    CALL_CHILD(gold_chorobon_rabbit_move)
    WAIT_FRAMES(20)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_JumpSetting, -2, 60, 0, FLOAT(0.7f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 2)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(&weapon_gold_chorobon_attack), 256, LW(5))
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
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 5)
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_JumpSetting, -2, 0, FLOAT(1.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 30)
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_N_1"))
    WAIT_MS(1000)
    GOTO(98)
    LABEL(91)
    LOOP(4)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.08f), FLOAT(-0.12f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(&weapon_gold_chorobon_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(&weapon_gold_chorobon_attack), 256, LW(5))
    LOOP(4)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.08f), FLOAT(0.12f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 10, -1)
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_A_1"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        ADD(LW(1), 30)
        CALL(btlevtcmd_SetScale, -2, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
        WAIT_FRAMES(16)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 6, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("CBN_S_1"))
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddScale, -2, FLOAT(0.16f), FLOAT(-0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    LOOP(2)
        CALL(btlevtcmd_AddScale, -2, FLOAT(-0.16f), FLOAT(0.24f), 0)
        WAIT_FRAMES(1)
    END_LOOP
    WAIT_FRAMES(10)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_nomove_y_onoff, 0, 0)
    CALL_CHILD(return_event)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
