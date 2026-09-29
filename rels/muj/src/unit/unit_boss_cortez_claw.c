#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 cortez_weapon_blow_dead_event[];
extern s32 cortez_weapon_break_event[];
extern s32 cortez_weapon_counter_damage_event[];
extern s32 cortez_weapon_dead_event[];
extern s32 cortez_weapon_phase_event[];
extern s32 cortez_weapon_unison_phase_event[];
static u32 regist[6];
static u32 data_table[36];
static u32 parts[19];
static s32 init_event[47];
static s32 damage_event[10];
static s32 attack_event[353];
static s32 wait_event[10];
static s32 cortez_damage_sub_event[25];

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

// TODO: type
u32 unit_boss_cortez_claw[49] = {
    0x0000005F, (u32)"btl_un_cortez_claw", 0x00040000, 0x01010000,
    0x00640032, 0x00140041, 0x00280000, 0x001E0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42040000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42020000,
    0x00000000, 0x00000000, 0x42020000, 0x41D00000,
    0x00000000, 0x00000000, 0x41A00000, 0x00000000,
    0x42820000, 0x42200000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_KOR_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x02000004,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x00505F64, 0x5A646455, 0x00006400, 0x645A3200,
    0x5A643200, 0x00000000,
};

// TODO: type
u32 weapon_cortez_claw_float_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80000000,
    0x00000000, 0x0000320A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[12] = {
    0x00000002, (u32)"KRT_Z_1", 0x00000009, (u32)"KRT_Z_1",
    0x0000001C, (u32)"KRT_S_1", 0x0000001F, (u32)"KRT_S_1",
    0x00000027, (u32)"KRT_S_1", 0x00000045, (u32)"KRT_S_1",
};

// TODO: type
static u32 data_table[36] = {
    0x00000009, (u32)&cortez_damage_sub_event, 0x00000031, (u32)cortez_weapon_dead_event,
    0x00000047, (u32)cortez_weapon_break_event, 0x0000003A, (u32)cortez_weapon_blow_dead_event,
    0x00000024, (u32)cortez_weapon_counter_damage_event, 0x00000018, (u32)cortez_weapon_counter_damage_event,
    0x00000019, (u32)cortez_weapon_counter_damage_event, 0x0000001A, (u32)cortez_weapon_counter_damage_event,
    0x0000001B, (u32)cortez_weapon_counter_damage_event, 0x0000001C, (u32)cortez_weapon_counter_damage_event,
    0x0000001D, (u32)cortez_weapon_counter_damage_event, 0x0000001E, (u32)cortez_weapon_counter_damage_event,
    0x0000001F, (u32)cortez_weapon_counter_damage_event, 0x00000020, (u32)cortez_weapon_counter_damage_event,
    0x00000021, (u32)cortez_weapon_counter_damage_event, 0x00000022, (u32)cortez_weapon_counter_damage_event,
    0x00000023, (u32)cortez_weapon_counter_damage_event, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_cortez_claw", (u32)"c_buki4", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x03010009, 0x00000000, (u32)&pose_table,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_OnUnitFlag, -2, 16777216)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(cortez_weapon_phase_event))
    CALL(btlevtcmd_SetEventUnisonPhase, -2, PTR(cortez_weapon_unison_phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 2)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_SetMaxMoveCount, -2, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(weapon_cortez_claw_float_attack))
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
    CALL(btlevtcmd_SetRotateOffset, -2, 25, 12, 0)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(20)
        CALL(evt_sub_intpl_init, 2, 0, -135, 40)
        LOOP(41)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 20)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -10, 4, 0, -1)
    WAIT_MS(200)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(evt_sub_intpl_init, 0, -135, 45, 10)
        LOOP(11)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOR_SWORD_DANCE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(7)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    SWITCH(LW(5))
        CASE_OR_EQUAL(4)
        GOTO(90)
        CASE_END
        CASE_EQUAL(3)
        CALL(btlevtcmd_StartAvoid, LW(3), 38)
        GOTO(90)
        CASE_EQUAL(6)
        CALL(btlevtcmd_StartAvoid, LW(3), 39)
        GOTO(90)
        CASE_EQUAL(2)
        CALL(btlevtcmd_StartAvoid, LW(3), 40)
        GOTO(90)
        CASE_DEFAULT
        GOTO(91)
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    GOTO(95)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_EQUAL(LW(5), 21)
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
        ADD(LW(1), 15)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_init, 5, 0, -30, 20)
            CALL(evt_sub_intpl_get_value)
            LOOP(21)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 21, 0, 5, 0, -1)
        BEGIN_SIBLING_EVENT
            CALL(evt_sub_intpl_init, 4, -30, 0, 20)
            CALL(evt_sub_intpl_get_value)
            LOOP(21)
                CALL(evt_sub_intpl_get_value)
                CALL(btlevtcmd_SetRotate, -2, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, LW(0))
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 0, 4, 0, -1)
        RETURN
    END_IF
    LABEL(95)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    WAIT_MS(500)
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_intpl_init, 11, 45, 0, 15)
        LOOP(15)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 60, -10, 4, 0, -1)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeWaitPlayLoopTiming, -2, 1)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(cortez_damage_sub_event)
    CALL(btlevtcmd_GetDamage, LW(10), LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetDamagePartsId, -2, LW(11))
    CALL(btlevtcmd_AnimeChangePose, LW(10), 1, STRING("KRT_Z_1"))
    WAIT_FRAMES(30)
    LABEL(90)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
