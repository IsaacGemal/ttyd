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

#pragma pool_data off

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 btldefaultevt_Dummy[];
extern s32 chorogun_appear[];
extern s32 chorogun_entry[];
extern s32 chorogun_escape_all[];
extern s32 unit_chorobon_gundan[];
extern s32 unk_evt_hei_00019e30[];
static u32 regist[6];
static u32 data_table[30];
static u32 parts[19];
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

// TODO: type
u32 unit_gold_chorobon[49] = {
    0x00000014, (u32)"btl_un_gold_chorobon", 0x000A0000, 0x01011B00,
    0x0A640032, 0x0005001C, 0x00200005, 0x00200000,
    0x00000000, 0x41800000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41600000, 0x41A66666,
    0x00000000, 0x00000000, 0x41800000, 0x00000000,
    0x41E00000, 0x42000000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_G_CHORO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040000,
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
    0x00000064, 0x00646400, 0x64006400, 0x64000000,
    0x00640064, 0x64000000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"CBN_N_1", 0x00000002, (u32)"CBN_Y_1",
    0x00000009, (u32)"CBN_Y_1", 0x00000005, (u32)"CBN_K_1",
    0x00000004, (u32)"CBN_X_1", 0x00000003, (u32)"CBN_X_1",
    0x0000001C, (u32)"CBN_S_1", 0x0000001D, (u32)"CBN_Q_1",
    0x0000001E, (u32)"CBN_Q_1", 0x0000001F, (u32)"CBN_S_1",
    0x00000027, (u32)"CBN_D_1", 0x00000032, (u32)"CBN_A_1",
    0x0000002A, (u32)"CBN_R_1", 0x00000028, (u32)"CBN_W_1",
    0x00000038, (u32)"CBN_X_1", 0x00000039, (u32)"CBN_X_1",
    0x00000041, (u32)"CBN_T_1", 0x00000045, (u32)"CBN_S_1",
};

// TODO: type
static u32 data_table[30] = {
    0x00000018, (u32)&counter_damage_event, 0x0000001A, (u32)&counter_damage_event,
    0x00000019, (u32)&counter_damage_event, 0x0000001B, (u32)&counter_damage_event,
    0x0000001C, (u32)&counter_damage_event, 0x0000001D, (u32)&counter_damage_event,
    0x0000001E, (u32)&counter_damage_event, 0x0000001F, (u32)&counter_damage_event,
    0x00000020, (u32)&counter_damage_event, 0x00000021, (u32)&counter_damage_event,
    0x00000022, (u32)&counter_damage_event, 0x00000023, (u32)&counter_damage_event,
    0x00000024, (u32)&counter_damage_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_gold_chorobon", (u32)"c_chorobon_k", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_gold_chorobon_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00005007, 0x00000000, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_sound_timing_table[20] = {
    (u32)"CBN_S_1", 0x3F155555, 0x00000000, (u32)"SFX_BOSS_G_CHORO_MOVE1",
    0x00000001, (u32)"CBN_S_1", 0x3F555555, 0x00000000,
    (u32)"SFX_BOSS_G_CHORO_WAIT2", 0x00000001, (u32)"CBN_S_1", 0x3FCAAAAB,
    0x00000000, (u32)"SFX_BOSS_G_CHORO_WAIT1", 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
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

// TODO: type
u32 entry_chorogun[12] = {
    (u32)unit_chorobon_gundan, 0x01000000, 0x04000004, 0x42960000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_gold_chorobon_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_gold_chorobon_attack), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_gold_chorobon_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_gold_chorobon_attack))
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
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_gold_chorobon_attack), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_gold_chorobon_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_gold_chorobon_attack), 256, LW(5))
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
