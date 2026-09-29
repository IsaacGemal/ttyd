#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "evt/evt_cmd.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 pose_table_nokotarou_stay[];
extern s32 pose_table_nokotarou_turn[];
extern s32 weaponGetPowerFromGulliblePartyAttackLv[];
static u32 regist_nokotarou[6];
static u32 data_table_nokotarou[6];
static u32 parts_nokotarou[19];
static s32 dmg_turn_event[64];
static s32 init_event_nokotarou[62];
static s32 damage_event_nokotarou[24];
static s32 dead_event_nokotarou[177];
static s32 _koura_rotate_stop[13];
static s32 attack_event_nokotarou[33];
static s32 attack_event_nokotarou_koura[393];
static s32 attack_event_nokotarou_shubibin[339];
static s32 wait_event_nokotarou[6];

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_gullible_nokotarou[49] = {
    0x00000051, (u32)"btl_un_gullible_nokotarou", 0x000A0000, 0x01010000,
    0x00640032, 0x0009001C, 0x0028000A, 0x001E0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x42200000, 0x00000000, 0x41600000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41600000,
    0x00000000, 0x00000000, 0x41600000, 0x41D00000,
    0x00000000, 0x00000000, 0x41A00000, 0x00000000,
    0x41E00000, 0x42200000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_PARTY_DAMAGE1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x10000000,
    (u32)&regist_nokotarou, 0x01000000, (u32)&parts_nokotarou, (u32)&init_event_nokotarou,
    (u32)&data_table_nokotarou,
};

// TODO: type
static u32 enemyWeapon_NokotarouKouraAttack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerFromGulliblePartyAttackLv,
    0x00000002, 0x00000003, 0x00000005, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x21004004, 0x00080302,
    0x00000000, 0x00010087, 0x000005FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 enemyWeapon_NokotarouShubibinAttack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerFromGulliblePartyAttackLv,
    0x00000002, 0x00000003, 0x00000005, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20004004, 0x00080302,
    0x00000000, 0x00010087, 0x000005FF, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_nokotarou[2] = {
    0x01010101, 0x01000000,
};

// TODO: type
static u32 defence_attr_nokotarou[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_turn_nokotarou[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist_nokotarou[6] = {
    0x3C504664, 0x46646446, 0x64466446, 0x645A4600,
    0x00644664, 0x64000000,
};

// TODO: type
static u32 data_table_nokotarou[6] = {
    0x00000031, (u32)&dead_event_nokotarou, 0x0000000D, (u32)&dmg_turn_event,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts_nokotarou[19] = {
    0x00000001, (u32)"btl_un_nokotarou", (u32)"c_pnk", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence_nokotarou, (u32)&defence_attr_nokotarou,
    0x00001009, 0x00000000, (u32)pose_table_nokotarou_stay,
};

static EVT_BEGIN(dmg_turn_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn_nokotarou))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_nokotarou_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("PNK_D_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 6, -1)
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
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_nokotarou))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_nokotarou_stay))
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("PNK_S_1"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(init_event_nokotarou)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event_nokotarou))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_nokotarou))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event_nokotarou))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 9, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 11, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 6, 12, 16)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 9, 3)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_nokotarou)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(GW(20), 1)
    END_IF
    CALL_CHILD(_koura_rotate_stop)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event_nokotarou)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(4), LW(5))
    SET_VALUE(LW(6), 1)
    IF_INT_EQUAL(LW(0), LW(3))
        IF_INT_EQUAL(LW(1), LW(4))
            IF_INT_EQUAL(LW(2), LW(5))
                SET_VALUE(LW(6), 0)
            END_IF
        END_IF
    END_IF
    IF_INT_EQUAL(LW(6), 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    WAIT_FRAMES(1)
    CALL(btlevtcmd_WaitAttackEnd)
    SWITCH(GW(19))
        CASE_EQUAL(0)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 130, 58, 228, 130, 38, -20, 30, 11)
        WAIT_FRAMES(37)
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_03_nok"), 0, -2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_MS(500)
        CALL(evt_btl_camera_set_mode, 0, 3)
        CALL(evt_btl_camera_set_moveto, 0, 90, 58, 228, 90, 38, -20, 30, 11)
        WAIT_FRAMES(37)
        CALL(evt_msg_print, 2, STRING("stg4_jin_26_04"), 0, GW(16))
        WAIT_MS(200)
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(0)
            IF_INT_EQUAL(GW(19), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_D_3"))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_DIE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SWITCH
    CALL(btlevtcmd_KillUnit, -2, 1)
    SET_VALUE(GW(20), 2)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(18)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(_change_koura_pose_fast)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(10)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 27, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(3)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 30, 0)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(_restore_koura_pose)
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_1"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 45, 0)
    END_LOOP
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PNK_K_2"))
    LOOP(6)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_AddRotate, -2, 0, 15, 0)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(_koura_rotate_start)
    CALL(btlevtcmd_SetUnitWork, -2, 8, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
    BEGIN_INLINE_EVENT
        LABEL(0)
        CALL(btlevtcmd_GetUnitWork, -2, 8, LW(1))
        IF_INT_EQUAL(LW(1), 0)
            GOTO(10)
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 10, LW(11))
        IF_INT_NOT_EQUAL(LW(11), 0)
            CALL(btlevtcmd_SetUnitWork, -2, 10, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 9, LW(10))
            MULTIPLY(LW(10), -1)
            MULTIPLY(LW(0), -1)
            CALL(btlevtcmd_SetUnitWork, -2, 9, LW(10))
        END_IF
        CALL(btlevtcmd_GetUnitWork, -2, 9, LW(10))
        IF_INT_GREATER_OR_EQUAL(LW(10), 0)
            ADD(LW(0), 1)
            IF_INT_GREATER(LW(0), LW(10))
                SET_VALUE(LW(0), LW(10))
            END_IF
        ELSE
            SUBTRACT(LW(0), 1)
            IF_INT_LESS(LW(0), LW(10))
                SET_VALUE(LW(0), LW(10))
            END_IF
        END_IF
        CALL(btlevtcmd_AddRotate, -2, 0, LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(0)
        LABEL(10)
    END_INLINE_EVENT
    RETURN
EVT_END()

static EVT_BEGIN(_koura_rotate_stop)
    CALL(btlevtcmd_SetUnitWork, -2, 8, 0)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_nokotarou)
    CALL(btlevtcmd_GetOverTurnCount, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        CALL_CHILD(wakeup_event)
        RETURN
    END_IF
    CALL(btlevtcmd_DrawLots, LW(0), 2, 80, 20)
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL_CHILD(attack_event_nokotarou_koura)
        CASE_EQUAL(1)
        CALL_CHILD(attack_event_nokotarou_shubibin)
    END_SWITCH
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_nokotarou_koura)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(enemyWeapon_NokotarouKouraAttack))
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
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_ATTACK1"), EVTDAT_POINTER_MAX, 0, LW(13))
    SET_VALUE(LW(0), 15)
    CALL(btlevtcmd_SetUnitWork, -2, 9, 45)
    CALL_CHILD(_koura_rotate_start)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    WAIT_FRAMES(30)
    CALL(evt_snd_sfxoff, LW(13))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_ATTACK2"), EVTDAT_POINTER_MAX, 0, LW(13))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(5), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), 0, LW(13))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(13), -1, 0)
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL_CHILD(_koura_rotate_stop)
        CALL_CHILD(_restore_koura_pose)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    LABEL(90)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 18, -1)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 8)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL_CHILD(_restore_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_nokotarou_shubibin)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(enemyWeapon_NokotarouShubibinAttack))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(12), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 100)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL_CHILD(_change_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_SHUBIBIN1"), EVTDAT_POINTER_MAX, 0, LW(13))
    CALL(btlevtcmd_SetUnitWork, -2, 5, 0)
    SET_VALUE(LW(0), 15)
    CALL(btlevtcmd_SetUnitWork, -2, 9, 45)
    CALL_CHILD(_koura_rotate_start)
    WAIT_MS(500)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    WAIT_MS(500)
    DELETE_EVENT(LW(15))
    CALL(evt_snd_sfxoff, LW(13))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_NOKO_SHUBIBIN2"), EVTDAT_POINTER_MAX, 0, LW(13))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_posoffset, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 50)
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(10.0f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(5), LW(2))
    ADD(LW(2), 5)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(50)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    LABEL(50)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(80)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, -400, LW(1), LW(2), 0, -1, 0)
    LABEL(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    MULTIPLY(LW(0), -1)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL_CHILD(_koura_rotate_stop)
    CALL_CHILD(_restore_koura_pose)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event_nokotarou)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
