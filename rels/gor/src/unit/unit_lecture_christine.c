#include "unit_lecture_christine.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"
#include "unit/party/unit_party_christine.h"
#include "battle/battle_database_common.h"
#include "battle/battle_unit.h"

#pragma pool_data off

#pragma force_active on
extern BattleUnitKind unit_lecture_frankli;
static USER_FUNC(_krb_get_dir);
static USER_FUNC(_battle_set_lecture_unit_id);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[2];
static BattleUnitKindPart parts[1];
static s32 init_event[78];
static s32 damage_event[10];
s32 lecture_christine_ac_counter_damage_event[117];
static s32 phase_event[2];
static s32 wait_event[6];
static s32 attack_event[19];
s32 lecture_zutsuki_attack_event[570];

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

BattleUnitKind unit_lecture_christine = {
    UNIT_TUTORIAL_GOOMBELLA,    // mUnitType
    "btl_un_christine",         // name
    10,                         // maxHp
    0,                          // maxFp
    1,                          // mDangerHP
    1,                          // mPerilHP
    1,                          // mLevel
    0,                          // mBonusExp
    0,                          // bonusCoinRolls
    0,                          // bonusCoinRate
    0,                          // baseCoin
    100,                        // escapeRate
    9,                          // field_0x14
    28,                         // width
    24,                         // height
    { 8, 24 },                  // statusIconOffset
    { 0.0f, 14.0f, 0.0f },      // centerOffset
    { 0, 0 },                   // hpGaugeOffset
    { 0.0f, 30.0f, 0.0f },      // talkTogeBaseOffset
    { 14.0f, 0.0f, -10.0f },    // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },       // burnFlameOffset
    0.0f,                       // field_0x54
    0.0f,                       // field_0x58
    { 14.0f, 0.0f, 0.0f },      // bintaHitOffset
    { 14.0f, 15.6f, 0.0f },     // kissHitOffset
    { 0.0f, 0.0f, 0.0f },       // cutBaseOffset
    0.0f,                       // cutWidth
    0.0f,                       // cutHeight
    0,                          // turnOrder
    0,                          // turnOrderVariance
    -1,                         // swallowRate
    0,                          // swallowAttribute
    100,                        // field_0x8C
    20,                         // itemStealParam
    { 0.0f, 0.0f, 0.0f },       // starPointDispOffset
    "SFX_PARTY_DAMAGE1",        // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",     // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",      // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1", // electricDamageSfxName
    0x02000000,                 // attributeFlags
    &regist,                    // vulnStatus
    1,                          // numParts
    parts,                      // mParts
    init_event,                 // initEvtScript
    data_table,                 // dataTable
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

static BattleDataEntry data_table[2] = {
    { 0x24, lecture_christine_ac_counter_damage_event },
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                                                 // partNum
        "btl_un_christine",                                // partName
        "c_pkr",                                           // modelName
        { 0.0f, 0.0f, 0.0f },                              // positionOffset
        { 0.0f, 25.0f, 0.0f },                             // hitBaseOffset
        { 0.0f, 30.0f, 0.0f },                             // hitCursorBaseOffset
        20,                                                // shadowSize
        30,                                                // field_0x32
        255,                                               // baseAlpha
        &defence,                                          // defense
        &defence_attr,                                     // defenseAttr
        0x00880009,                                        // attributes
        0,                                                 // counterAttributes
        pose_table_christine_stay, // poseTable
    },
};

BattleUnitSetup entry_lecture_frankli = { &unit_lecture_frankli, 2, 0x04000001, { 0.0f, 0.0f, -45.0f }, 0, { 0, 0, 0, 0 }, NULL };

USER_FUNC(_set_lecture_mode) {
    BattleWork* work = _battleWorkPointer;
    s32 id = BattleTransID(event, -3);
    BattleUnit* unit = BattleGetUnitPtr(work, id);

    unit->mAttributeFlags |= 0x40000000;
    work->statusWinLectureOff = 1;
    return EVT_RETURN_DONE;
}

static USER_FUNC(_battle_set_lecture_unit_id) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;

    work->lectureUnitId = evtGetValue(event, *args++);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 9, 3)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(_set_lecture_mode)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_lecture_frankli), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(3))
    CALL(_battle_set_lecture_unit_id, LW(3))
    CALL(btlevtcmd_ChangeFaceDirection, LW(3), -1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, LW(3), 0, LW(0))
    CALL(btlevtcmd_onoff_battleflag, 1, 4096)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

EVT_BEGIN(lecture_christine_ac_counter_damage_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(2), 25)
    ADD(LW(3), 25)
    CALL(btlevtcmd_DivePosition, -2, LW(2), LW(3), LW(4), 5, 0, 0, 0, -1)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("PKR_D_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("PKR_D_1"))
    CALL(evt_msg_print, 2, STRING("mac_0_b_t2_015"), 0, -2)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_TransStageFloorPosition, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(2), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(2), LW(3), LW(4), 20, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(2), 15)
    CALL(btlevtcmd_JumpPosition, -2, LW(2), LW(3), LW(4), 10, -1)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 7)
        CALL(btlevtcmd_SetUnitWork, -6, 1, 8)
    ELSE
        CALL(btlevtcmd_SetUnitWork, -6, 3, 7)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL_CHILD(lecture_zutsuki_attack_event)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetUnitWork, -6, 1, 4)
    END_SWITCH
    RETURN
EVT_END()

BattleWeapon lec_weapon_zutsuki_1 = {
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
    { 1, 1, 1, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015007,                               // special
    0x0000000C,                               // counterResistanceFlags
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

BattleWeapon lec_weapon_zutsuki_2 = {
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
    { 1, 1, 1, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01001260,                               // targetClassFlags
    0x20001004,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00015007,                               // special
    0x0000000C,                               // counterResistanceFlags
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

static USER_FUNC(_krb_get_dir) {
    s32* args = event->args;
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
    s32 out;
    f32 dx;
    f32 dy;

    x0 = evtGetFloat(event, args[0]);
    y0 = evtGetFloat(event, args[1]);
    x1 = evtGetFloat(event, args[2]);
    y1 = evtGetFloat(event, args[3]);
    out = evtGetValue(event, args[4]);
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx == 0.0f && dy == 0.0f) {
        evtSetValue(event, args[4], out);
        return EVT_RETURN_DONE;
    }
    evtSetValue(event, args[4], (s32)angleABf(dx, dy, 0.0f, 0.0f));
    return EVT_RETURN_DONE;
}

USER_FUNC(_config_key_record) {
    s32* args = event->args;
    s32 val = args[0];
    BattleWork* work = _battleWorkPointer;
    BattleWorkPad* pad = &work->actionCommands.pad;

    work->actionCommands.mDefenseResult = 4;
    BtlPad_WorkInit(pad);
    pad->mButtonsPressedHistory[0] = val;
    return EVT_RETURN_DONE;
}

USER_FUNC(_get_defence_ac_result) {
    s32* args = event->args;
    s32 idx = args[0];
    s32 result = 0;

    switch (_battleWorkPointer->actionCommands.mDefenseResult) {
    case 1:
        result = 2;
        break;
    case 2:
        result = 3;
        break;
    case 4:
        result = 1;
        break;
    case 5:
        result = 0;
        break;
    }
    evtSetValue(event, idx, result);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(lecture_zutsuki_attack_event)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    IF_INT_EQUAL(LW(0), 3)
        SET_VALUE(LW(12), PTR(&lec_weapon_zutsuki_1))
    ELSE
        SET_VALUE(LW(12), PTR(&lec_weapon_zutsuki_2))
    END_IF
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(14))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(14), -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_A_2B"))
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKR_J_1A"))
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(15)
        CALL(evt_snd_sfxon, STRING("SFX_BTL_KURI_ATTACK1"), 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 14, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPosFloat, -2, LW(5), LW(6), LW(7))
        SET_VALUE(LW(8), 0)
        LOOP(36)
            CALL(btlevtcmd_GetPosFloat, -2, LW(12), LW(13), LW(14))
            CALL(_krb_get_dir, LW(5), LW(6), LW(12), LW(13), LW(8))
            CALL(btlevtcmd_GetRotate, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetRotate, -2, LW(0), LW(1), LW(8))
            SET_FLOAT_VALUE(LW(5), LW(12))
            SET_FLOAT_VALUE(LW(6), LW(13))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), 36, 0, -1)
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_006"), 0, -6)
        WAIT_FRAMES(1)
        LABEL(11)
        CALL(btlevtcmd_PadCheckTrigger, 0, 256, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(11)
        END_IF
        WAIT_MS(50)
        BEGIN_INLINE_EVENT
            WAIT_MS(100)
            CALL(evt_msg_continue)
        END_INLINE_EVENT
        WAIT_MS(100)
        CASE_EQUAL(7)
        CALL(btlevtcmd_SetTalkPose, -6, STRING("T_1"))
        CALL(btlevtcmd_SetStayPose, -6, STRING("S_1"))
        CALL(evt_msg_print, 2, STRING("mac_0_b_t2_014"), 0, -6)
        WAIT_FRAMES(1)
        LABEL(12)
        CALL(btlevtcmd_PadCheckTrigger, 0, 512, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            WAIT_FRAMES(1)
            GOTO(12)
        END_IF
        WAIT_MS(50)
        BEGIN_INLINE_EVENT
            WAIT_MS(100)
            CALL(evt_msg_continue)
        END_INLINE_EVENT
        WAIT_MS(100)
    END_SWITCH
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
        CALL(evt_btl_camera_set_zoom, 0, FLOAT(162.5f))
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetUnitWork, -6, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(3)
        CALL(_config_key_record, 256)
        CASE_EQUAL(7)
        CALL(_config_key_record, 512)
    END_SWITCH
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(_get_defence_ac_result, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetUnitWork, -6, 3, 4)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetUnitWork, -6, 3, 5)
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetUnitWork, -6, 3, 6)
    END_SWITCH
    ADD(LW(10), 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(6))
        IF_INT_LESS(LW(6), 0)
            MULTIPLY(LW(6), -1)
        END_IF
        CALL(btlevtcmd_GetFaceDirection, -2, LW(0))
        MULTIPLY(LW(6), LW(0))
        DIVIDE(LW(6), 15)
        SET_VALUE(LW(7), 15)
        LABEL(20)
        CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(6))
        SUBTRACT(LW(7), 1)
        WAIT_FRAMES(1)
        IF_INT_GREATER_OR_EQUAL(LW(7), 1)
            GOTO(20)
        END_IF
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    END_SIBLING_EVENT
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
// clang-format on
