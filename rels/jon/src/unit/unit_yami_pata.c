#include "battle/battle_camera.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
static BattleVulnerableStatus regist_patapata;
static BattleUnitKindPart parts_patapata;
static BattleDispPose pose_table_patapata[18];
static BattleDataEntry data_table_patapata[3];
static s32 init_event_patapata[26];
static s32 dmg_fall_event[189];
static s32 patapata_dmg_first_attack_pos[19];
static s32 attack_event_patapata[541];
static BattleDispPose pose_table[18];
static BattleDataEntry data_table[3];
static BattleUnitKindPart parts[1];
static s32 damage_event[10];
static s32 dmg_turn_event[78];
static s32 attack_event[548];
static s32 wait_event[6];

// clang-format off
extend_data(120,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(121,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// BattleUnitKind (0xC4). Left raw: the header's field_0x4 and field_0xBC are u8[] regions that in the
// original hold a const char* (0x4) and the init event pointer (0xBC), which cannot be written as bytes at
// compile time. Everything else (unit type 0x9D = UNIT_DARK_PARATROOPA, parts/data table at 0xB8/0xC0) fits.
u32 unit_yami_pata[49] = {
    0x0000009D, (u32)"btl_un_yami_pata", 0x00080000, 0x01011100,
    0x01460132, 0x270F0024, 0x0028FFFC, 0x00280000,
    0x00000000, 0x41C00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41100000,
    0x40C00000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41900000,
    0x00000000, 0x00000000, 0x41300000, 0x41C00000,
    0x00000000, 0x00000000, 0x41C00000, 0x00000000,
    0x42100000, 0x42200000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_PATA_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00040004,
    (u32)&regist_patapata, 0x01000000, (u32)&parts_patapata, (u32)&init_event_patapata,
    (u32)&data_table_patapata,
};

static BattleWeapon weapon_patapata_attack[1] = {
    {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x01, 0x3F, 0x80, 0x00, 0x00 },
        0x01, 0x01, 0x01, 0x01,
        (void*)weaponGetPowerDefault,
        { 0x00000004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        0,
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x01, 0x10, 0x12, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00 },
        0x00005087,
        { 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        0,
        { 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    },
};

// BattleUnitDefense* tables: 8 bytes each, but the header struct is u8 defenses[5] (5 bytes); typing them
// would shift every following symbol. Values are 5 defense entries + 3 bytes of tail padding.
static u32 defence_patapata[2] = {
    0x02020202, 0x02000000,
};

// BattleUnitDefenseAttr* table (5 attrs + 3 pad); header struct is u8 defenseAttrs[5].
static u32 defence_patapata_attr[2] = {
    0x00000000, 0x00000000,
};

static BattleVulnerableStatus regist_patapata = {
    75, 80, 105, 100, 70, 100, 100, 70, 100, 85,
    100, 85, 100, 85, 80, 75, 95, 100, 70, 100,
    100, 70,
};

static BattleUnitKindPart parts_patapata = {
    0x01,
    "btl_un_yami_pata",
    "c_yaminoko",
    { 0.0f, 0.0f, 0.0f },
    { 0.0f, 30.0f, 0.0f },
    // field_0x24, byte-identical to the original data
    { 0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF, 0x00, 0x00 },
    (BattleUnitDefense*)defence_patapata,
    (BattleUnitDefenseAttr*)defence_patapata_attr,
    0x00600809,
    0x00000000,
    &pose_table_patapata,
};

static BattleDispPose pose_table_patapata[18] = {
    { 0x01, "PTP_N_1" }, { 0x02, "PTP_Y_1" },
    { 0x09, "PTP_Y_1" }, { 0x05, "PTP_K_1" },
    { 0x04, "PTP_X_1" }, { 0x03, "PTP_X_1" },
    { 0x1C, "PTP_S_1" }, { 0x1D, "PTP_Q_1" },
    { 0x1E, "PTP_Q_1" }, { 0x1F, "PTP_S_1" },
    { 0x27, "PTP_D_1" }, { 0x32, "PTP_A_1" },
    { 0x2A, "PTP_R_1" }, { 0x28, "PTP_W_1" },
    { 0x38, "PTP_X_1" }, { 0x39, "PTP_X_1" },
    { 0x41, "PTP_T_1" }, { 0x45, "PTP_S_1" },
};

static BattleDataEntry data_table_patapata[3] = {
    { 0x0E, &dmg_fall_event },
    { 0x30, &patapata_dmg_first_attack_pos },
    { 0x00, NULL },
};

// Pose-sound table: 8 entries of {const char* poseName; f32 frame; u32 field8; const char* seName; s32 flag}.
// No matching struct exists; consumed by btlevtcmd_AnimeSetPoseSoundTable.
static u32 pose_sound_timing_table[40] = {
    (u32)"PTP_S_1", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_PATA_WAIT1",
    0x00000001, (u32)"PTP_W_1", 0x3C23D70A, 0x00000000,
    (u32)"SFX_ENM_PATA_MOVE1", 0x00000001, (u32)"PTP_R_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_ENM_PATA_MOVE1", 0x00000001, (u32)"PTP_N_1",
    0x3C23D70A, 0x00000000, (u32)"SFX_ENM_PATA_WAIT1", 0x00000001,
    (u32)"PTP_K_1", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_PATA_WAIT1",
    0x00000001, (u32)"PTP_A_3", 0x3C23D70A, 0x00000000,
    (u32)"SFX_ENM_PATA_WAIT1", 0x00000001, (u32)"PTP_X_1", 0x3C23D70A,
    0x00000000, (u32)"SFX_ENM_PATA_WAIT1", 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
};

static EVT_BEGIN(init_event_patapata)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event_patapata))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event_patapata))
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_fall_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_OffAttribute, LW(10), 4)
    CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(11), 2048)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_ChangeDataTable, LW(10), PTR(data_table))
    CALL(btlevtcmd_ReplaceParts, LW(10), LW(11), PTR(parts), 1)
    CALL(btlevtcmd_AnimeChangePoseDirect, LW(10), LW(11), STRING("NKT_D_1"))
    CALL(btlevtcmd_ChangeKind, LW(10), 156)
    CALL(btlevtcmd_OnUnitFlag, LW(10), 4)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_FALL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 0)
    CALL(btlevtcmd_SetFallAccel, LW(10), FLOAT(1.0f))
    CALL(btlevtcmd_JumpPosition, LW(10), LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_SetHomePos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHpDamageCount, LW(10), LW(0))
    IF_INT_LESS_OR_EQUAL(LW(0), 1)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
        CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
        CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
        CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
        CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3R"), 0, 3, 3)
        CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3L"), 0, 6, 6)
        CALL(btlevtcmd_SetOverTurnCount, -2, 0)
        CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
        RETURN
    END_IF
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3R"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3L"), 0, 6, 6)
    CALL(btlevtcmd_SetOverTurnCount, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 39)
    CALL_CHILD(dmg_turn_event)
    RETURN
EVT_END()

static EVT_BEGIN(patapata_dmg_first_attack_pos)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    SET_VALUE(LW(1), 10)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_WaitAttackEnd)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event_patapata)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        WAIT_FRAMES(50)
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_patapata_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_patapata_attack), LW(3), LW(4))
    END_IF
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
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_patapata_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_patapata_attack))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 45)
    ADD(LW(1), 30)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.5f))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -16, 0, 0, -1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_nomove_y_onoff, 0, 1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PATA_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PTP_A_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 6)
    ADD(LW(1), 6)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0)
    WAIT_FRAMES(11)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PATA_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PTP_A_4"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_patapata_attack), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_patapata_attack), 256, LW(5))
    END_IF
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
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(5.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0)
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    SET_VALUE(LW(1), 20)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_patapata_attack))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_patapata_attack), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_patapata_attack), 256, LW(5))
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PATA_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    ADD(LW(1), 30)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(24)
            SUBTRACT(LW(0), 45)
            IF_INT_LESS(LW(0), 0)
                ADD(LW(0), 360)
            END_IF
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        WAIT_FRAMES(1)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SUBTRACT(LW(1), 20)
        CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    END_INLINE_EVENT
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, 30, 0, 0, 0)
    WAIT_FRAMES(10)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 0, -10, 0, 0, -1)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static BattleDispPose pose_table[18] = {
    { 0x01, "NKT_N_1" }, { 0x02, "NKT_Y_1" },
    { 0x09, "NKT_Y_1" }, { 0x05, "NKT_K_1" },
    { 0x04, "NKT_X_1" }, { 0x03, "NKT_X_1" },
    { 0x1C, "NKT_S_1" }, { 0x1D, "NKT_Q_1" },
    { 0x1E, "NKT_Q_1" }, { 0x1F, "NKT_S_1" },
    { 0x27, "NKT_D_1" }, { 0x32, "NKT_A_1" },
    { 0x2A, "NKT_R_1" }, { 0x28, "NKT_W_1" },
    { 0x38, "NKT_X_1" }, { 0x39, "NKT_X_1" },
    { 0x41, "NKT_T_1" }, { 0x45, "NKT_S_1" },
};

static BattleDispPose pose_table_turn[17] = {
    { 0x01, "NKT_N_2" }, { 0x02, "NKT_B_1" },
    { 0x09, "NKT_B_1" }, { 0x05, "NKT_K_2" },
    { 0x04, "NKT_K_2" }, { 0x03, "NKT_K_2" },
    { 0x1C, "NKT_S_2" }, { 0x1D, "NKT_Q_1" },
    { 0x1E, "NKT_Q_1" }, { 0x1F, "NKT_D_1" },
    { 0x27, "NKT_D_1" }, { 0x2A, "NKT_R_1" },
    { 0x28, "NKT_W_1" }, { 0x38, "NKT_K_2" },
    { 0x39, "NKT_K_2" }, { 0x41, "NKT_S_2" },
    { 0x45, "NKT_S_2" },
};

static BattleDataEntry data_table[3] = {
    { 0x0D, &dmg_turn_event },
    { 0x30, btldefaultevt_Dummy },
    { 0x00, NULL },
};

// BattleUnitDefense* tables: 8 bytes each, but the header struct is u8 defenses[5] (5 bytes).
static u32 defence[2] = {
    0x02020202, 0x02000000,
};

// BattleUnitDefenseAttr* table (5 attrs + 3 pad); header struct is u8 defenseAttrs[5].
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// BattleUnitDefense* table (5 defense entries + 3 pad); header struct is u8 defenses[5].
static u32 defence_turn[2] = {
    0x00000000, 0x00000000,
};

static BattleVulnerableStatus regist = {
    75, 80, 70, 100, 70, 100, 100, 70, 100, 85,
    100, 85, 100, 85, 80, 75, 70, 100, 70, 100,
    100, 70,
};

static BattleUnitKindPart parts[1] = {
    {
        0x01,
        "btl_un_yami_noko",
        "c_yaminoko",
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 30.0f, 0.0f },
        // field_0x24, byte-identical to the original data
        { 0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF, 0x00, 0x00 },
        (BattleUnitDefense*)defence,
        (BattleUnitDefenseAttr*)defence_attr,
        0x00001009,
        0x00000000,
        &pose_table,
    },
};

static BattleWeapon weapon_nokonoko_attack[1] = {
    {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x01, 0x3F, 0x80, 0x00, 0x00 },
        0x01, 0x01, 0x01, 0x01,
        (void*)weaponGetPowerDefault,
        { 0x00000004, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        0,
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x01, 0x10, 0x12, 0x60, 0x21, 0x00, 0x20, 0x00, 0x00, 0x08, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00 },
        0x00000087,
        { 0x00, 0x00, 0x05, 0xFF, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        0,
        { 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    },
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetOverTurnCount, -2, 0)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3R"), 0, 3, 3)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_NOKO_MOVE3L"), STRING("SFX_ENM_NOKO_MOVE3L"), 0, 6, 6)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_turn_event)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -15, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence_turn))
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table_turn))
    CALL(btlevtcmd_OnStatusFlag, LW(10), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), 2)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_D_1"))
    WAIT_FRAMES(2)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 15, -1)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 10, -1)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_S_2"))
    RETURN
EVT_END()

static EVT_BEGIN(wakeup_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetOverTurnCount, LW(10), LW(0))
    SUBTRACT(LW(0), 1)
    CALL(btlevtcmd_SetOverTurnCount, LW(10), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_S_3"))
        WAIT_MS(400)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_SetPartsDefenceTable, LW(10), LW(11), PTR(defence))
    CALL(btlevtcmd_OffStatusFlag, LW(10), 1)
    CALL(btlevtcmd_AnimeSetPoseTable, LW(10), LW(11), PTR(pose_table))
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("NKT_S_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_INSIDE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(first_attack_event)
    CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_nokonoko_attack))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 20)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 40)
    WAIT_MS(100)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
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
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
        WAIT_FRAMES(30)
    ELSE
        CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(LW(0))
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_nokonoko_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_nokonoko_attack), LW(3), LW(4))
    END_IF
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
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(100)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.0f), FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_SHELL1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("NKT_A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    BEGIN_SIBLING_EVENT
        LOOP(0)
            CALL(btlevtcmd_GetUnitWork, -2, 0, LW(15))
            IF_INT_EQUAL(LW(15), 1)
                CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
            CALL(btlevtcmd_AddRotate, -2, 0, LW(14), 0)
        END_LOOP
    END_SIBLING_EVENT
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 20)
    WAIT_MS(100)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 40)
    WAIT_MS(100)
    BEGIN_SIBLING_EVENT_GET_ID(LW(13))
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            ADD(LW(1), 10)
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 0, LW(0), LW(1), LW(2), FLOAT(0.3f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(3)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(8.0f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(5), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(5), LW(2), 0, 0, 1)
    DELETE_EVENT(LW(13))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
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
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 100)
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, 0, 1)
    WAIT_MS(250)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    WAIT_MS(250)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(10.0f), FLOAT(0.4f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_nokonoko_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_nokonoko_attack), 256, LW(5))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 10)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(3.0f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 50)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpSetting, -2, 20, FLOAT(0.5f), FLOAT(0.2f))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 75)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), 0, LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_NOKO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
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
