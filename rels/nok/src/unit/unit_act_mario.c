#include "unit_act_mario.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_audience.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_disp.h"

#pragma pool_data off

#pragma force_active on
extern BattleUnitKind unit_act_atmic_teresa;
extern BattleUnitKind unit_act_clauda;
extern BattleUnitKind unit_act_kinopiko;
extern BattleUnitKind unit_act_kinopio;
extern BattleUnitKind unit_act_teresa;
extern BattleUnitKind unitdata_Party_Christine;
static USER_FUNC(_clauda_breath_effect_fire);
static USER_FUNC(_clauda_breath_effect_ready);
static BattleVulnerableStatus regist;
static BattleDataEntry data_table[1];
static BattleUnitKindPart parts[1];

// Global .rodata float constants this file's functions use (see the original's
// .rodata: the compiler reuses double_to_int for the int->float conversion).
// Declared extern here and defined at the bottom of the file: the functions are
// compiled before the definitions, so they refer to the named symbols instead of
// folding the literals into the instruction pool.
static s32 init_event[15];
static s32 wait_event[6];
static s32 battle_entry_event[168];
s32 _battle_event[155];
s32 _act_mario_jump_attack[240];
s32 _act_clauda_press_attack[350];
s32 _act_clauda_breath_attack[269];
s32 _act_teresa_attack[190];
s32 _act_atmic_teresa_press_attack[188];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleUnitKind unit_act_mario = {
    UNIT_EPILOGUE_DOOPLISS_MARIO, // mUnitType
    "btl_un_act_mario",           // name
    10,                           // maxHp
    0,                            // maxFp
    1,                            // mDangerHP
    1,                            // mPerilHP
    1,                            // mLevel
    0,                            // mBonusExp
    0,                            // bonusCoinRolls
    0,                            // bonusCoinRate
    0,                            // baseCoin
    100,                          // escapeRate
    9,                            // field_0x14
    28,                           // width
    24,                           // height
    { 5, 25 },                    // statusIconOffset
    { 0.0f, 14.0f, 0.0f },        // centerOffset
    { 0, 0 },                     // hpGaugeOffset
    { 0.0f, 30.0f, 0.0f },        // talkTogeBaseOffset
    { 14.0f, 0.0f, -10.0f },      // heldItemBaseOffset
    { 0.0f, 0.0f, 0.0f },         // burnFlameOffset
    0.0f,                         // field_0x54
    0.0f,                         // field_0x58
    { 14.0f, 0.0f, 0.0f },        // bintaHitOffset
    { 14.0f, 15.6f, 0.0f },       // kissHitOffset
    { 0.0f, 0.0f, 0.0f },         // cutBaseOffset
    0.0f,                         // cutWidth
    0.0f,                         // cutHeight
    0,                            // turnOrder
    0,                            // turnOrderVariance
    -1,                           // swallowRate
    0,                            // swallowAttribute
    100,                          // field_0x8C
    20,                           // itemStealParam
    { 0.0f, 0.0f, 0.0f },         // starPointDispOffset
    "SFX_BTL_DAMAGE1",            // damageSfxName
    "SFX_BTL_DAMAGE_FIRE1",       // fireDamageSfxName
    "SFX_BTL_DAMAGE_ICE1",        // iceDamageSfxName
    "SFX_BTL_DAMAGE_BIRIBIRI1",   // electricDamageSfxName
    0,                            // attributeFlags
    &regist,                      // vulnStatus
    1,                            // numParts
    parts,                        // mParts
    init_event,                   // initEvtScript
    data_table,                   // dataTable
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

static BattleDispPose pose_table[7] = {
    { 0x1C, "EM_S_1" },
    { 0x1F, "KNP_S_1" },
    { 0x27, "EM_D_1" },
    { 0x2A, "EM_R_1" },
    { 0x28, "EM_W_1" },
    { 0x41, "EM_T_1" },
    { 0x45, "EM_S_1" },
};

static BattleDataEntry data_table[1] = {
    { 0x00, NULL },
};

static BattleUnitKindPart parts[1] = {
    {
        1,                      // partNum
        "btl_un_act_mario",     // partName
        "c_mario",              // modelName
        { 0.0f, 0.0f, 0.0f },   // positionOffset
        { -5.0f, 30.0f, 0.0f }, // hitBaseOffset
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

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(_status_window_off)
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
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

BattleUnitSetup entry_unit_data[8] = {
    { &unit_act_clauda, 0, 0x04000004, { -140.0f, 0.0f, 5.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_teresa, 1, 0x04000004, { 45.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_teresa, 1, 0x04000004, { 85.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_atmic_teresa, 1, 0x04000004, { 150.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_kinopio, 2, 0x04000004, { -75.0f, 0.0f, -40.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_kinopio, 2, 0x04000004, { -15.0f, 0.0f, -40.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_act_kinopiko, 2, 0x04000004, { -45.0f, 0.0f, -40.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unitdata_Party_Christine, 2, 0x04000004, { 0.0f, -50.0f, 110.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_ChangeFaceDirection, -2, 1)
    CALL(btlevtcmd_GetStageSize, LW(6), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    MULTIPLY(LW(6), -1)
    CALL(btlevtcmd_GetHomePos, -3, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -3, LW(6), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(0))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_unit_data), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[1]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[2]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[3]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, LW(3))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[4]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, LW(3))
    CALL(btlevtcmd_ChangeFaceDirection, LW(3), 1)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[5]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 6, LW(3))
    CALL(btlevtcmd_ChangeFaceDirection, LW(3), 1)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[6]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 7, LW(3))
    CALL(btlevtcmd_ChangeFaceDirection, LW(3), 1)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(&entry_unit_data[7]), 0)
    CALL(btlevtcmd_SetUnitWork, -2, 8, LW(3))
    CALL(btlevtcmd_ChangeFaceDirection, LW(3), 1)
    SPAWN_EVENT(_battle_event)
    WAIT_MS(2000)
    CALL(evt_msg_print, 2, STRING("ep_39"), 0, -2)
    WAIT_MS(1000)
    CALL(btlevtcmd_onoff_battleflag, 1, 32)
    RETURN
EVT_END()

EVT_BEGIN(_battle_event)
    LABEL(0)
    CALL_CHILD(_act_mario_jump_attack)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(evt_sub_random, 1, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL_CHILD(_act_clauda_press_attack)
    ELSE
        CALL_CHILD(_act_clauda_breath_attack)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_A_7"))
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1000)
    CALL_CHILD(_act_teresa_attack)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1000)
    CALL_CHILD(_act_teresa_attack)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1000)
    CALL_CHILD(_act_atmic_teresa_press_attack)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_GetUnitWork, -2, 5, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1500)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_GetUnitWork, -2, 7, LW(5))
    CALL(btlevtcmd_GetBodyId, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1500)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    GOTO(0)
    RETURN
EVT_END()

EVT_BEGIN(_act_target_enemy)
    CALL(evt_sub_random, 2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(3))
        CASE_DEFAULT
        CALL(btlevtcmd_GetUnitWork, -2, 4, LW(3))
    END_SWITCH
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    RETURN
EVT_END()

EVT_BEGIN(_act_target_mario)
    CALL(evt_sub_random, 1, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
        CASE_DEFAULT
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
    END_SWITCH
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    RETURN
EVT_END()

EVT_BEGIN(_act_mario_jump_attack)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 65)
    WAIT_MS(1000)
    CALL_CHILD(_act_target_enemy)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    SET_VALUE(LW(0), -20)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(6))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(6))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1B"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    SET_VALUE(LW(6), 36)
    CALL(btlevtcmd_MarioJumpParam, -2, LW(0), LW(1), LW(2), LW(6))
    CALL(btlevtcmd_MarioJumpPosition, -2, LW(0), LW(1), LW(2), LW(6), 3, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_J_1D"))
    WAIT_MS(50)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_StopWaitEvent, LW(3))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        WAIT_MS(1000)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_SIBLING_EVENT
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("EM_R_1"))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    RETURN
EVT_END()

EVT_BEGIN(_act_clauda_press_attack)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1000)
    CALL_CHILD(_act_target_enemy)
    CALL(btlevtcmd_CalculateFaceDirection, LW(5), -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, LW(5), LW(15))
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_A1_1"))
    CALL(btlevtcmd_SetFallAccel, LW(5), FLOAT(0.2f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_JumpPosition, LW(5), LW(0), LW(1), LW(2), 60, -1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        CALL(evt_sub_intpl_init_float, 5, FLOAT(1.0f), FLOAT(0.15f), 10)
        LOOP(11)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(3), LW(0), LW(7), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(1500)
        CALL(evt_sub_intpl_init_float, 5, FLOAT(0.15f), FLOAT(1.25f), 10)
        LOOP(11)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(3), LW(0), LW(7), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 7, FLOAT(1.25f), FLOAT(1.0f), 30)
        LOOP(31)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(3), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(3), LW(0), LW(7), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 28)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpContinue, LW(5))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(0.4f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(5), LW(0), LW(7), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(0.4f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(5), LW(0), LW(7), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.0f), FLOAT(1.5f), 10)
        LOOP(10)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(5), LW(7), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_sub_intpl_init_float, 11, FLOAT(1.5f), FLOAT(1.0f), 6)
        LOOP(6)
            CALL(evt_sub_intpl_get_float, LW(7), EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetScale, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetScale, LW(5), LW(7), LW(1), LW(2))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, LW(5), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(5), LW(0), 20)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetRotate, LW(5), 0, 0, 0)
    CALL(btlevtcmd_SetFallAccel, LW(5), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPosition, LW(5), LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_Y_1"))
    CALL(evt_audience_joy_ending)
    WAIT_MS(500)
    CALL(btlevtcmd_WaitEventEnd, LW(14))
    CALL(btlevtcmd_SetMoveSpeed, LW(5), FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 40)
    CALL(btlevtcmd_GetHomePos, LW(5), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetMoveFrame, LW(5), LW(0), LW(1), LW(2), FLOAT(6.0f), LW(9))
    CALL(btlevtcmd_DivePosition, LW(5), LW(0), LW(1), LW(2), LW(9), 0, 4, 0, -1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    RETURN
EVT_END()

s32 _status_window_off(EventEntry* event, BOOL isFirstCall) {
    _battleWorkPointer->statusWinScriptOff = 1;
    return EVT_RETURN_DONE;
}

static s32 _clauda_breath_effect_ready(EventEntry* event, BOOL isFirstCall) {
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit;
    EffectEntry* effect;
    s8 dir;
    s32 id;
    Vec pos;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    dir = unit->faceDirection;
    pos.x += unit->mSizeMultiplier * (dir * 20);
    pos.y += 25.0f * unit->mSizeMultiplier;
    effect = effKumokumoN64Entry(0, 0x3C, pos.x, pos.y, pos.z, 0.0f, 2.0f,
                                 1.0f);
    ((effKumokumoData*)effect->userdata)->color1R = 0x4C;
    ((effKumokumoData*)effect->userdata)->color1G = 0xA7;
    ((effKumokumoData*)effect->userdata)->color1B = 0xCA;
    ((effKumokumoData*)effect->userdata)->color2R = 0x7D;
    ((effKumokumoData*)effect->userdata)->color2G = 0x1B;
    ((effKumokumoData*)effect->userdata)->color2B = 0xF5;
    return EVT_RETURN_DONE;
}

static s32 _clauda_breath_effect_fire(EventEntry* event, BOOL isFirstCall) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    EffectEntry* effect;
    s32 arg0;
    s8 dir;
    s32 id;
    Vec pos;

    id = BattleTransID(event, -2);
    unit = BattleGetUnitPtr(work, id);
    arg0 = args[0];
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    dir = unit->faceDirection;
    pos.x += unit->mSizeMultiplier * (dir * 25);
    pos.y += 15.0f * unit->mSizeMultiplier;
    effect = effKumokumoN64Entry(0, 0, pos.x, pos.y, pos.z, 0.0f, -2.0f,
                                 1.0f);
    ((effKumokumoData*)effect->userdata)->color1R = 0x4C;
    ((effKumokumoData*)effect->userdata)->color1G = 0xA7;
    ((effKumokumoData*)effect->userdata)->color1B = 0xCA;
    ((effKumokumoData*)effect->userdata)->color2R = 0x7D;
    ((effKumokumoData*)effect->userdata)->color2G = 0x1B;
    ((effKumokumoData*)effect->userdata)->color2B = 0xF5;
    ((effKumokumoData*)effect->userdata)->rotation = -10.0f;
    evtSetValue(event, arg0, (s32)effect);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(_act_clauda_breath_attack)
    CALL(btlevtcmd_JumpSetting, LW(5), 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 65)
    WAIT_MS(1000)
    CALL(_clauda_breath_effect_ready)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_A2_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_A2_2"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, LW(5), LW(6))
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_A2_3"))
    CALL(_clauda_breath_effect_fire, LW(12))
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        LOOP(30)
            CALL(btlevtcmd_SetDispOffset, LW(3), 2, 0, 0)
            WAIT_FRAMES(2)
            CALL(btlevtcmd_SetDispOffset, LW(3), 0, 0, 0)
            WAIT_FRAMES(2)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 28)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        LOOP(30)
            CALL(btlevtcmd_SetDispOffset, LW(3), 2, 0, 0)
            WAIT_FRAMES(2)
            CALL(btlevtcmd_SetDispOffset, LW(3), 0, 0, 0)
            WAIT_FRAMES(2)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 28)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        LOOP(30)
            CALL(btlevtcmd_SetDispOffset, LW(3), 2, 0, 0)
            WAIT_FRAMES(2)
            CALL(btlevtcmd_SetDispOffset, LW(3), 0, 0, 0)
            WAIT_FRAMES(2)
        END_LOOP
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 28)
    END_SIBLING_EVENT
    WAIT_FRAMES(120)
    CALL(evt_eff_delete_ptr, LW(12))
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("PWD_Y_1"))
    CALL(evt_audience_joy_ending)
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 28)
    RETURN
EVT_END()

EVT_BEGIN(_act_teresa_attack)
    CALL_CHILD(_act_target_mario)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 42)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetAlpha, LW(5), LW(6), 255)
        SET_VALUE(LW(7), 256)
        LOOP(16)
            WAIT_FRAMES(1)
            SUBTRACT(LW(7), 16)
            CALL(btlevtcmd_SetAlpha, LW(5), LW(6), LW(7))
        END_LOOP
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("A_1"))
        CALL(btlevtcmd_SetAlpha, LW(5), LW(6), 0)
        SET_VALUE(LW(7), -1)
        LOOP(8)
            WAIT_FRAMES(1)
            ADD(LW(7), 32)
            CALL(btlevtcmd_SetAlpha, LW(5), LW(6), LW(7))
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(5), LW(0), 20)
    SUBTRACT(LW(1), 10)
    CALL(btlevtcmd_DivePosition, LW(5), LW(0), LW(1), LW(2), 60, -5, 4, 0, -1)
    CALL(btlevtcmd_SetAlpha, LW(5), LW(6), 255)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("A_2"))
    WAIT_MS(250)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_StopWaitEvent, LW(3))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        WAIT_MS(1000)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_SIBLING_EVENT
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePoseType, LW(5), LW(6), 42)
    CALL(btlevtcmd_AddPartsRotate, LW(5), LW(6), 0, 180, 0)
    CALL(btlevtcmd_GetHomePos, LW(5), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, LW(5), LW(0), LW(1), LW(2), 40, 0, 4, 0, -1)
    CALL(btlevtcmd_SetPartsRotate, LW(5), LW(6), 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 28)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    RETURN
EVT_END()

EVT_BEGIN(_act_atmic_teresa_press_attack)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("R_1"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 10)
    CALL(btlevtcmd_DivePosition, LW(5), -125, 100, LW(2), 120, 10, 4, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("Z_1"))
    CALL(btlevtcmd_DivePosition, LW(5), -125, 0, LW(2), 30, 0, 1, 0, -1)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
        CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
        CALL(btlevtcmd_StopWaitEvent, LW(3))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        WAIT_MS(1000)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(3))
        CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
        CALL(btlevtcmd_StopWaitEvent, LW(3))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        ADD(LW(2), 5)
        CALL(evt_eff, 0, STRING("hit"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 3, 0, 0, 0, 0, 0, 0)
        CALL(evt_audience_joy_ending)
        CALL(btlevtcmd_AnimeChangePoseType, LW(3), LW(4), 39)
        WAIT_MS(1000)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_SIBLING_EVENT
    WAIT_MS(1000)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("W_1"))
    CALL(btlevtcmd_GetHomePos, LW(5), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, LW(5), LW(0), LW(1), LW(2), 60, 50, 6, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, LW(5), LW(6), STRING("S_1"))
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
