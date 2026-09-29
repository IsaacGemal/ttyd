#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(yoshi_exist);
static USER_FUNC(get_id);
static u32 regist[6];
static u32 parts[19];
static u32 pose_table[36];
static u32 data_table[6];
static s32 init_event2[18];
static s32 init_event_sub[47];
static s32 dmg_avoid_counter_thorn_head_event[76];
static s32 damage_event[102];
static s32 phase_event[188];
static s32 attack_event[385];
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

// TODO: type
u32 unit_iron_sinemon2[49] = {
    0x0000003E, (u32)"btl_un_iron_sinemon2", 0x00060000, 0x01011200,
    0x02640146, 0x00090034, 0x00360004, 0x00360000,
    0x00000000, 0x41D80000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41D00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41D00000,
    0x00000000, 0x00000000, 0x41800000, 0x42100000,
    0x00000000, 0x00000000, 0x41D80000, 0x00000000,
    0x42500000, 0x42580000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_ISIN_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event2,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x04040404, 0x04000000,
};

// TODO: type
static u32 defence_turn[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_iron_sinemon2", (u32)"c_sinemon_m", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42140000,
    0x00000000, 0x00000000, 0x42340000, 0x00000000,
    0x001E001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000001, (u32)&pose_table,
};

// TODO: type
static u32 weapon_iron_sinemon_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x000000C7, 0x000007FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"SIN_N_1", 0x00000002, (u32)"SIN_Y_1",
    0x00000009, (u32)"SIN_Y_1", 0x00000005, (u32)"SIN_K_1",
    0x00000004, (u32)"SIN_I_1", 0x00000003, (u32)"SIS_I_1",
    0x0000001C, (u32)"SIN_S_1", 0x0000001D, (u32)"SIN_Q_1",
    0x0000001E, (u32)"SIN_Q_1", 0x0000001F, (u32)"SIN_A_1a",
    0x00000027, (u32)"SIN_D_1", 0x00000032, (u32)"SIN_A_1a",
    0x00000028, (u32)"SIN_W_1", 0x0000002A, (u32)"SIN_R_1",
    0x00000041, (u32)"SIN_T_1", 0x00000038, (u32)"SIN_I_1",
    0x00000039, (u32)"SIN_I_1", 0x00000045, (u32)"SIN_S_1",
};

// TODO: type
static u32 data_table[6] = {
    0x00000025, (u32)&dmg_avoid_counter_thorn_head_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// The original object's functions are emitted in reverse source order
// (objdiff metadata `reverse_fn_order`): get_id is defined first here, so it
// lands second in .text, after yoshi_exist.
USER_FUNC(get_id) {
    s32* args = event->args;
    BattleUnit* unit;
    s32 i;
    s32 idRed;
    s32 idGreen;

    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(_battleWorkPointer, i);
        if (unit != NULL) {
            if (unit->currentKind == UNIT_IRON_CLEFT_RED) {
                idRed = i;
            }
            if (unit->currentKind == UNIT_IRON_CLEFT_GREEN) {
                idGreen = i;
            }
        }
    }
    evtSetValue(event, args[0], idRed);
    evtSetValue(event, args[1], idGreen);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(init_event1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(get_id, LW(0), LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(1))
    CALL_CHILD(init_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(init_event2)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(get_id, LW(0), LW(1))
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
    CALL_CHILD(init_event_sub)
    RETURN
EVT_END()

static EVT_BEGIN(init_event_sub)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetBaseScale, -2, FLOAT(1.5f), FLOAT(1.5f), FLOAT(1.0f))
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_ISIN_MOVE1"), STRING("SFX_BOSS_ISIN_MOVE2"), 0, 10, 10)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_ISIN_MOVE1"), STRING("SFX_BOSS_ISIN_MOVE2"), 0, 7, 7)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
        ADD(LW(1), 67)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 14)
        ADD(LW(1), 37)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 20)
    ADD(LW(1), 52)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2511), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_GetTotalDamage, -2, LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_btl_camera_set_prilimit, 1)
    CALL(evt_btl_camera_set_mode, 1, 3)
    CALL(evt_btl_camera_set_moveto, 1, 91, 74, 334, 91, 52, 0, 30, 4)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL(evt_msg_print, 2, STRING("stg3_tou_272"), 0, -2)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
    CALL(evt_msg_print, 2, STRING("stg3_tou_273"), 0, LW(0))
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    SET_VALUE(GSWF(2511), 1)
    CALL(evt_btl_camera_set_prilimit, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -4)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

USER_FUNC(yoshi_exist) {
    s32* args = event->args;
    PouchData* pouch;
    s32 yoshi;

    pouch = pouchGetPtr();
    yoshi = 0;
    if (pouch->partyData[PARTNER_YOSHI].flags & 1) {
        yoshi = 1;
    }
    evtSetValue(event, args[0], yoshi);
    return EVT_RETURN_DONE;
}

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(2512), 1)
        RETURN
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108865)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_get_turn, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            CALL(btlevtcmd_PhaseEventStartDeclare, -4)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(evt_btl_camera_set_prilimit, 1)
            CALL(evt_btl_camera_set_mode, 1, 3)
            CALL(evt_btl_camera_set_moveto, 1, -104, 74, 334, -102, 52, 0, 30, 4)
            WAIT_FRAMES(30)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            IF_INT_EQUAL(GSWF(2510), 0)
                CALL(evt_msg_print_battle_party, STRING("stg3_tou_266"))
            ELSE
                CALL(evt_msg_print_battle_party, STRING("stg3_tou_267"))
            END_IF
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_moveto, 1, 91, 74, 334, 91, 52, 0, 30, 4)
            WAIT_FRAMES(30)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL(yoshi_exist, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_SetTalkPoseType, -2, 65)
                CALL(btlevtcmd_SetStayPoseType, -2, 69)
                CALL(evt_msg_print, 2, STRING("stg3_tou_268"), 0, -2)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
                CALL(btlevtcmd_SetTalkPoseType, LW(0), 65)
                CALL(btlevtcmd_SetStayPoseType, LW(0), 69)
                CALL(evt_msg_print, 2, STRING("stg3_tou_269"), 0, LW(0))
            ELSE
                CALL(btlevtcmd_SetTalkPoseType, -2, 65)
                CALL(btlevtcmd_SetStayPoseType, -2, 69)
                CALL(evt_msg_print, 2, STRING("stg3_tou_270"), 0, -2)
                CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
                CALL(btlevtcmd_SetTalkPoseType, LW(0), 65)
                CALL(btlevtcmd_SetStayPoseType, LW(0), 69)
                CALL(evt_msg_print, 2, STRING("stg3_tou_271"), 0, LW(0))
            END_IF
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            IF_INT_EQUAL(GSWF(2510), 0)
                SET_VALUE(GSWF(2510), 1)
            END_IF
            CALL(evt_btl_camera_set_prilimit, 0)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_StartWaitEvent, -4)
            CALL(btlevtcmd_StartWaitEvent, -2)
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
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_iron_sinemon_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_iron_sinemon_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_iron_sinemon_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_iron_sinemon_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    BEGIN_SIBLING_EVENT_GET_ID(LW(14))
        LOOP(0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ISIN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ISIN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(7)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ISIN_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    WAIT_MS(750)
    DELETE_EVENT(LW(14))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), FLOAT(30.0f))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 25)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_iron_sinemon_attack), 256, LW(5))
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
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 75)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    WAIT_MS(500)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_noshake, 0)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_iron_sinemon_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_iron_sinemon_attack), 256, LW(5))
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(1.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ISIN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 100)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(evt_btl_camera_shake_h, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_ISIN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(evt_btl_camera_noshake, 0)
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
