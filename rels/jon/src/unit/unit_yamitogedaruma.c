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

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static u32 regist[6];
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

// TODO: type
u32 unit_yamitogedaruma[49] = {
    0x000000A5, (u32)"btl_un_yamitogedaruma", 0x00080000, 0x01012201,
    0x02460232, 0x270F002E, 0x00280004, 0x00280000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41B80000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41B80000,
    0x00000000, 0x00000000, 0x41A00000, 0x41D00000,
    0x00000000, 0x00000000, 0x41A00000, 0x00000000,
    0x42380000, 0x42200000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_TOGEDA_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

static BattleUnitDefense defence[1] = {
    { 4, 4, 4, 4, 4 },
};

static BattleUnitDefenseAttr defence_attr[1] = {
    { 0, 0, 0, 0, 0 },
};

static BattleUnitDefense defence_turn[1] = {
    { 0, 0, 0, 0, 0 },
};

// TODO: type
static u32 regist[6] = {
    0x14501464, 0x14646428, 0x64326432, 0x6464281E,
    0x14642864, 0x641E0000,
};

static BattleDispPose pose_table[18] = {
    { 0x00000001, "TGD_N_1" },
    { 0x00000002, "TGD_I_1" },
    { 0x00000009, "TGD_I_1" },
    { 0x00000005, "TGD_K_1" },
    { 0x00000004, "TGD_X_1" },
    { 0x00000003, "TGD_X_1" },
    { 0x0000001C, "TGD_S_4" },
    { 0x0000001D, "TGD_Q_1" },
    { 0x0000001E, "TGD_Q_1" },
    { 0x0000001F, "TGD_S_2" },
    { 0x00000027, "TGD_D_1" },
    { 0x00000032, "TGD_A_1" },
    { 0x00000028, "TGD_S_4" },
    { 0x0000002A, "TGD_S_4" },
    { 0x00000038, "TGD_X_1" },
    { 0x00000039, "TGD_X_1" },
    { 0x00000041, "TGD_T_1" },
    { 0x00000045, "TGD_S_4" },
};

static BattleDispPose pose_table_turn[15] = {
    { 0x00000001, "TGD_N_2" },
    { 0x00000002, "TGD_I_2" },
    { 0x00000009, "TGD_Z_4" },
    { 0x00000005, "TGD_K_2" },
    { 0x00000004, "TGD_X_2" },
    { 0x00000003, "TGD_X_2" },
    { 0x0000001C, "TGD_S_5" },
    { 0x0000001D, "TGD_Q_2" },
    { 0x0000001E, "TGD_Q_2" },
    { 0x0000001F, "TGD_S_3" },
    { 0x00000027, "TGD_D_2" },
    { 0x00000038, "TGD_X_2" },
    { 0x00000039, "TGD_X_2" },
    { 0x00000041, "TGD_S_5" },
    { 0x00000045, "TGD_S_5" },
};

static BattleDataEntry data_table[6] = {
    { 0x0000000D, dmg_turn_event },
    { 0x00000025, dmg_avoid_counter_thorn_head_event },
    { 0x00000027, dmg_avoid_counter_thorn_front_event },
    { 0x00000026, dmg_avoid_counter_thorn_front_event },
    { 0x00000030, btldefaultevt_Dummy },
    { 0x00000000, NULL },
};

static BattleUnitKindPart parts = {
    0x00000001,
    "btl_un_yamitogedaruma",
    "c_togedaruma_b",
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 35.0f, 0.0f },
    { 0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF, 0x00, 0x00 },
    (BattleUnitDefense*)defence,
    (BattleUnitDefenseAttr*)defence_attr,
    kMainPart | kUnknown_0x8 | kIsBombFlippable,
    kCounterTopSpiky | kCounterPreemptiveFrontSpiky | kCounterFrontSpiky,
    pose_table,
};

static BattleWeapon weapon_togedaruma_attack[1] = {{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x64, 0x00, 0x00, 0x01, 0x3F, 0x80, 0x00, 0x00 },
    1,
    1,
    1,
    1,
    (void*)weaponGetPowerDefault,
    { 0x00000008, 0, 0, 0, 0, 0, 0, 0 },
    NULL,
    { 0 },
    { 0x01, 0x10, 0x12, 0x60, 0x21, 0x00, 0x20, 0x00,
      0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00 },
    0x00000087,
    { 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x20, 0x04 },
    NULL,
    { 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00 },
}};

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
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn))
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
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence))
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
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_togedaruma_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_togedaruma_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_togedaruma_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_togedaruma_attack))
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
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_togedaruma_attack), 256, LW(5))
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
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_togedaruma_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_togedaruma_attack), 256, LW(5))
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
