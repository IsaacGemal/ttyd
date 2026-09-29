#include "battle/battle_camera.h"
#include "battle/battle_damage.h"
#include "battle/battle_database_common.h"
#include "battle/battle_disp.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 btldefaultevt_Confuse[];
extern s32 gesso_arm_damage_pose_event[];
extern s32 gesso_arm_dead_event[];
static BattleVulnerableStatus regist;
BattleDataEntry unk_tik_00029f1c[13];
static BattleUnitKindPart parts;
static s32 init_event[25];
static s32 damage_event[31];
s32 unk_evt_tik_0002a0c8[93];
static s32 attack_event[171];
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

// BattleUnitKind: size 0xC4 matches, but offset 0x4 is the unit-name string pointer
// and battle_unit.h models it as opaque bytes (u8 field_0x4[8]), so it cannot be
// initialized through the struct yet.
// TODO: type
u32 unit_boss_gesso_left_arm[49] = {
    0x00000009, (u32)"btl_un_gesso_left_arm", 0x00030000, 0x01010000,
    0x00640032, 0x00090096, 0x0028FF10, 0x00240000,
    0x00000000, 0x00000000, 0x00000000, 0xFF01FFFB,
    0x00000000, 0x00000000, 0x00000000, 0x42960000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42960000,
    0x00000000, 0x00000000, 0x42960000, 0x41D00000,
    0x00000000, 0xC3570000, 0x41A00000, 0x00000000,
    0x43160000, 0x42200000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_GESSO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00200000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&unk_tik_00029f1c,
};

static BattleUnitDefense defence = {
    0, 0, 0, 0, 0,
};

static BattleUnitDefenseAttr defence_attr = {
    0, 0, 0, 0, 0,
};

static BattleVulnerableStatus regist = {
    0, 0, 0, 0, 0, 0, 100,
};

static BattleDispPose pose_table[4] = {
    { 0x1C, "GSO_S1_1" },
    { 0x1F, "GSO_S1_1" },
    { 0x27, "GSO_D1_1" },
    { 0x45, "GSO_S1_1" },
};

// No terminator: BtlUnit_GetData reads on into data_table below.
BattleDataEntry unk_tik_00029f1c[13] = {
    { 0x18, unk_evt_tik_0002a0c8 },
    { 0x19, unk_evt_tik_0002a0c8 },
    { 0x1A, unk_evt_tik_0002a0c8 },
    { 0x1B, unk_evt_tik_0002a0c8 },
    { 0x1C, unk_evt_tik_0002a0c8 },
    { 0x1D, unk_evt_tik_0002a0c8 },
    { 0x1E, unk_evt_tik_0002a0c8 },
    { 0x1F, unk_evt_tik_0002a0c8 },
    { 0x20, unk_evt_tik_0002a0c8 },
    { 0x21, unk_evt_tik_0002a0c8 },
    { 0x22, unk_evt_tik_0002a0c8 },
    { 0x23, unk_evt_tik_0002a0c8 },
    { 0x24, unk_evt_tik_0002a0c8 },
};

static BattleDataEntry data_table[3] = {
    { 0x0A, gesso_arm_damage_pose_event },
    { 0x31, gesso_arm_dead_event },
    { 0x00, 0 },
};

static BattleUnitKindPart parts = {
    1,
    "btl_un_gesso_left_arm",
    "c_gesso3",
    { 0.0f, 0.0f, 0.0f },          // positionOffset
    { 270.0f, 20.0f, 0.0f },       // hitBaseOffset
    // opaque in battle_unit.h: Vec {270.0f, 20.0f, 0.0f}, u16 {0x0014, 0x001E}, u16 {0x00FF, 0x0000}
    { 0x43, 0x87, 0x00, 0x00, 0x41, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x14, 0x00, 0x1E, 0x00, 0xFF, 0x00, 0x00 },
    &defence,
    &defence_attr,
    0x00080009,
    0,
    &pose_table,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_GetUnitWork, LW(3), 4, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StartWaitEvent, LW(3))
        END_IF
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_tik_0002a0c8)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_OnUnitFlag, LW(10), 1)
    SET_VALUE(LW(14), 39)
    SET_VALUE(LW(15), -1)
    CALL(btlevtcmd_CheckDamagePattern, LW(10), 1, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(14), 38)
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_RunDataEventChild, LW(10), 10)
    END_SIBLING_EVENT
    CALL_CHILD(subsetevt_damage)
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(1))
    IF_INT_NOT_EQUAL(LW(1), 0)
        WAIT_FRAMES(30)
        CALL_CHILD(subsetevt_dead)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_WaitAttackEnd)
    END_IF
    CALL(btlevtcmd_ResetFaceDirection, LW(10))
    CALL(btlevtcmd_StartWaitEvent, LW(10))
    LABEL(99)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(3))
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_GetUnitWork, LW(3), 4, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StartWaitEvent, LW(3))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    RETURN
EVT_END()

static BattleWeapon weapon_gesso_left_arm_attack[1] = {
  {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x64, 0x00, 0x00, 0x01, 0x3F, 0x80, 0x00, 0x00 },
    1, 1, 1, 1,
    weaponGetPowerDefault,
    { 1, 0, 0, 0, 0, 0, 0, 0 },
    0,
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0x01, 0x10, 0x12, 0x60, 0x20, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00 },
    0x87,
    { 0x00, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00, 0x04,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    0,
    { 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
  },
};

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(12), PTR(weapon_gesso_left_arm_attack))
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
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("GSO_A1_1"))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ftof, 16, LW(0))
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_ARM_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_ftof, 44, LW(0))
        WAIT_FRAMES(LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_GESSO_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_ftof, 64, LW(0))
    WAIT_FRAMES(LW(0))
    LABEL(50)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 1)
        SWITCH(LW(5))
            CASE_EQUAL(3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
            CASE_EQUAL(6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
            CASE_EQUAL(2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_SWITCH
    ELSE
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    END_IF
    CALL(btlevtcmd_CheckWeaponPluralTarget, LW(12), LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_NOT_EQUAL(LW(3), -1)
            GOTO(50)
        END_IF
    END_IF
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    LABEL(99)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
