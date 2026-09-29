#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static u32 data_table[4];
static u32 parts[19];
static u32 regist[6];
static s32 init_event[42];
static s32 damage_event[58];
static s32 change_spark_event[70];
static s32 attack_event[440];
static s32 wait_event[6];
static s32 damage_event_spark[25];
static s32 attack_event_spark[288];
static s32 wait_event_spark[27];

// clang-format off
extend_data(bomhei_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(bomhei_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x0000002B, (u32)"btl_un_bomhei", 0x00040000, 0x01010C00,
    0x01460132, 0x270F0020, 0x001E0004, 0x001E0000,
    0x00000000, 0x41700000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41800000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41800000,
    0x00000000, 0x00000000, 0x41300000, 0x41800000,
    0x00000000, 0x40000000, 0x41700000, 0x00000000,
    0x42000000, 0x41F00000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_BOMB_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x01010101, 0x01000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"BOM_N_1", 0x00000002, (u32)"BOM_Y_1",
    0x00000009, (u32)"BOM_Y_1", 0x00000005, (u32)"BOM_K_1",
    0x00000004, (u32)"BOM_X_1", 0x00000003, (u32)"BOM_X_1",
    0x0000001C, (u32)"BOM_S_1", 0x0000001D, (u32)"BOM_Q_1",
    0x0000001E, (u32)"BOM_Q_1", 0x0000001F, (u32)"BOM_S_1",
    0x00000027, (u32)"BOM_D_1", 0x00000032, (u32)"BOM_A_1",
    0x0000002A, (u32)"BOM_R_1", 0x00000028, (u32)"BOM_W_1",
    0x00000038, (u32)"BOM_X_1", 0x00000039, (u32)"BOM_X_1",
    0x00000041, (u32)"BOM_T_1", 0x00000045, (u32)"BOM_S_1",
};

// TODO: type
static u32 pose_table_spark[36] = {
    0x00000001, (u32)"BOM_N_2", 0x00000002, (u32)"BOM_Y_2",
    0x00000009, (u32)"BOM_Y_2", 0x00000005, (u32)"BOM_K_2",
    0x00000004, (u32)"BOM_X_2", 0x00000003, (u32)"BOM_X_2",
    0x0000001C, (u32)"BOM_S_2", 0x0000001D, (u32)"BOM_Q_2",
    0x0000001E, (u32)"BOM_Q_2", 0x0000001F, (u32)"BOM_S_2",
    0x00000027, (u32)"BOM_D_2", 0x00000032, (u32)"BOM_A_2",
    0x0000002A, (u32)"BOM_R_2", 0x00000028, (u32)"BOM_W_2",
    0x00000038, (u32)"BOM_X_2", 0x00000039, (u32)"BOM_X_2",
    0x00000041, (u32)"BOM_T_2", 0x00000045, (u32)"BOM_S_2",
};

// TODO: type
static u32 data_table[4] = {
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

// TODO: type
static u32 data_table_spark[4] = {
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_bomhei", (u32)"c_bomhey", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00001000, (u32)&pose_table,
};

// TODO: type
static u32 regist[6] = {
    0x46644600, 0x46966464, 0x645A645A, 0x645F505A,
    0x50645064, 0x645F0000,
};

// TODO: type
static u32 weapon_bomhei_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21004004, 0x00000302,
    0x00000000, 0x00000087, 0x00000000, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_bomhei_bomb[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21004004, 0x00000302,
    0x00000000, 0x00002087, 0x000007FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00191964, 0x00000000, 0x00000000,
};

static EVT_BEGIN(bomb_event)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 15, 0)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_snd_sfxoff, LW(0))
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENM_BOMB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("sandars"), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_StageDispellFog)
    CALL(btlevtcmd_GetCoin, -2, LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, -2, LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        LOOP(LW(0))
        END_LOOP
    END_IF
    WAIT_MS(1000)
    IF_INT_EQUAL(LW(15), 1)
        CALL(btlevtcmd_WaitAttackEnd)
    END_IF
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, -1)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_BOMB_MOVE2"), STRING("SFX_ENM_BOMB_MOVE2"), 0, 4, 4)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_BOMB_MOVE2"), STRING("SFX_ENM_BOMB_MOVE2"), 0, 10, 10)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
        SWITCH(LW(0))
            CASE_OR_EQUAL(24)
            CASE_OR_EQUAL(27)
            CASE_OR_EQUAL(25)
            SET_VALUE(LW(15), 1)
            CALL_CHILD(bomb_event)
            RETURN
            CASE_END
        END_SWITCH
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckActStatus, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(change_spark_event)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(change_spark_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    CALL(btlevtcmd_CheckDamageCode, -2, 256, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetHpDamage, -2, LW(15))
    IF_INT_GREATER_OR_EQUAL(LW(15), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_SPARK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_spark))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_spark))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_spark))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event_spark))
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_spark))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
        CALL(btlevtcmd_OnPartsCounterAttribute, -2, 1, 8192)
        CALL(btlevtcmd_ChangeDataTable, -2, PTR(data_table_spark))
    END_IF
    LABEL(99)
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
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_bomhei_attack))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_bomhei_attack), LW(3), LW(4))
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
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_bomhei_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_bomhei_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 10)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_bomhei_attack), 256, LW(5))
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
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 30)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 60)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 0)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 90)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("BOM_N_1"))
    WAIT_MS(500)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_bomhei_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_bomhei_attack), 256, LW(5))
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(2.0f), FLOAT(0.5f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(1.0f), FLOAT(0.5f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 100)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_ENM_BOMB_MOVE4"), STRING("SFX_ENM_BOMB_MOVE4"), 0, 10, 10, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 1)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_spark)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LW(15), 1)
        CALL_CHILD(bomb_event)
    ELSE
        CALL_CHILD(btldefaultevt_Damage)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_spark)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_bomhei_bomb))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_bomhei_bomb), LW(3), LW(4))
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
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_bomhei_bomb))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_bomhei_bomb))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 10)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_bomhei_bomb), 256, LW(5))
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
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 500)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    SET_VALUE(LW(15), 0)
    CALL_CHILD(bomb_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_bomhei_bomb))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_bomhei_bomb), 256, LW(5))
    SET_VALUE(LW(15), 0)
    CALL_CHILD(bomb_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(1000)
    RETURN
    LABEL(98)
    WAIT_MS(1000)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_spark)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
    IF_INT_EQUAL(LW(15), -1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BOMB_WAIT1"), EVTDAT_POINTER_MAX, 0, LW(15))
        CALL(btlevtcmd_SetUnitWork, -2, 0, LW(15))
    END_IF
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
