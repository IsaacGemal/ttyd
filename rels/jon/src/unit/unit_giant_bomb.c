#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "driver/animdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "party/party_motion.h"
#include <string.h>

#pragma pool_data off

#pragma force_active on
static void _fire_point_callback(BattleUnitPart* part, s32 group, void* mtx);
static USER_FUNC(_set_draw_callback);
static u32 regist[6];
static u32 data_table[6];
static u32 parts[38];
static s32 init_event[57];
static s32 damage_event[118];
static s32 phase_event[126];
static s32 attack_event[175];
static s32 wait_event[6];
static s32 dead_event[52];
static s32 explosion_event[247];

// clang-format off
extend_data(200,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(201,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_giant_bomb[49] = {
    0x000000A8, (u32)"btl_un_giant_bomb", 0x000A0000, 0x01012202,
    0x05460032, 0x270F0036, 0x00460007, 0x00460000,
    0x00000000, 0x420C0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41D80000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41D80000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x420C0000, 0x00000000,
    0x42580000, 0x428C0000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_HEAVYB_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x02000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 weapon_heavy_bomb_super_explosion[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101020, 0x20000000, 0x03000302,
    0x00000000, 0x00022087, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00323264, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_heavy_bomb_guard[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02004020, 0x10000000, 0x00000302,
    0x00000000, 0x00020008, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00640303, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_heavy_bomb_guard_2[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02004020, 0x10000000, 0x00000302,
    0x00000000, 0x00020008, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00006403, 0x03000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_heavy_bomb_guard_3[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02004020, 0x10000000, 0x00000302,
    0x00000000, 0x00020008, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000009,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 defence[2] = {
    0x02020202, 0x02000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x46373264, 0x3264645A, 0x64466446, 0x6455323C,
    0x0A643264, 0x64140000,
};

// TODO: type
static u32 pose_table_3[32] = {
    0x00000001, (u32)"N_1", 0x00000009, (u32)"Y_1",
    0x00000002, (u32)"Y_1", 0x00000003, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000005, (u32)"K_1",
    0x00000009, (u32)"Z_1", 0x0000001C, (u32)"S_1",
    0x0000001D, (u32)"D_1", 0x00000023, (u32)"S_1",
    0x0000001F, (u32)"S_1", 0x00000027, (u32)"D_1",
    0x00000038, (u32)"X_1", 0x00000039, (u32)"X_1",
    0x00000041, (u32)"S_1", 0x00000045, (u32)"S_1",
};

// TODO: type
static u32 pose_table_3_fire[32] = {
    0x00000001, (u32)"N_1", 0x00000009, (u32)"Y_1",
    0x00000002, (u32)"Y_1", 0x00000003, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000005, (u32)"K_1",
    0x00000009, (u32)"Z_1", 0x0000001C, (u32)"S_1",
    0x0000001D, (u32)"D_1", 0x00000023, (u32)"S_1",
    0x0000001F, (u32)"S_1", 0x00000027, (u32)"D_1",
    0x00000038, (u32)"X_1", 0x00000039, (u32)"X_1",
    0x00000041, (u32)"S_1", 0x00000045, (u32)"S_1",
};

// TODO: type
static u32 pose_table_2[32] = {
    0x00000001, (u32)"N_2", 0x00000009, (u32)"Y_2",
    0x00000002, (u32)"Y_2", 0x00000003, (u32)"K_2",
    0x00000004, (u32)"X_2", 0x00000005, (u32)"K_2",
    0x00000009, (u32)"Z_2B", 0x0000001C, (u32)"S_2B",
    0x0000001D, (u32)"D_2", 0x00000023, (u32)"S_2B",
    0x0000001F, (u32)"S_2B", 0x00000027, (u32)"D_2",
    0x00000038, (u32)"X_2", 0x00000039, (u32)"X_2",
    0x00000041, (u32)"S_2B", 0x00000045, (u32)"S_2B",
};

// TODO: type
static u32 pose_table_2_fire[32] = {
    0x00000001, (u32)"N_2", 0x00000009, (u32)"Y_2",
    0x00000002, (u32)"Y_2", 0x00000003, (u32)"K_2",
    0x00000004, (u32)"X_2", 0x00000005, (u32)"K_2",
    0x00000009, (u32)"Z_2A", 0x0000001C, (u32)"S_2A",
    0x0000001D, (u32)"D_2", 0x00000023, (u32)"S_2A",
    0x0000001F, (u32)"S_2A", 0x00000027, (u32)"D_2",
    0x00000038, (u32)"X_2", 0x00000039, (u32)"X_2",
    0x00000041, (u32)"S_2A", 0x00000045, (u32)"S_2A",
};

// TODO: type
static u32 pose_table_1[32] = {
    0x00000001, (u32)"N_3", 0x00000009, (u32)"Y_3",
    0x00000002, (u32)"Y_3", 0x00000003, (u32)"K_3",
    0x00000004, (u32)"X_3", 0x00000005, (u32)"K_3",
    0x00000009, (u32)"Z_3B", 0x0000001C, (u32)"S_3B",
    0x0000001D, (u32)"D_3", 0x00000023, (u32)"S_3B",
    0x0000001F, (u32)"S_3B", 0x00000027, (u32)"D_3",
    0x00000038, (u32)"X_3", 0x00000039, (u32)"X_3",
    0x00000041, (u32)"S_3B", 0x00000045, (u32)"S_3B",
};

// TODO: type
static u32 pose_table_1_fire[32] = {
    0x00000001, (u32)"N_3", 0x00000009, (u32)"Y_3",
    0x00000002, (u32)"Y_3", 0x00000003, (u32)"K_3",
    0x00000004, (u32)"X_3", 0x00000005, (u32)"K_3",
    0x00000009, (u32)"Z_3A", 0x0000001C, (u32)"S_3A",
    0x0000001D, (u32)"D_3", 0x00000023, (u32)"S_3A",
    0x0000001F, (u32)"S_3A", 0x00000027, (u32)"D_3",
    0x00000038, (u32)"X_3", 0x00000039, (u32)"X_3",
    0x00000041, (u32)"S_3A", 0x00000045, (u32)"S_3A",
};

// TODO: type
static u32 pose_table_0_fire[32] = {
    0x00000001, (u32)"N_4", 0x00000009, (u32)"Y_4",
    0x00000002, (u32)"Y_4", 0x00000003, (u32)"K_4",
    0x00000004, (u32)"X_4", 0x00000005, (u32)"K_4",
    0x00000009, (u32)"Z_4A", 0x0000001C, (u32)"S_4A",
    0x0000001D, (u32)"D_4", 0x00000023, (u32)"S_4A",
    0x0000001F, (u32)"S_4A", 0x00000027, (u32)"D_4",
    0x00000038, (u32)"X_4", 0x00000039, (u32)"X_4",
    0x00000041, (u32)"S_4A", 0x00000045, (u32)"S_4A",
};

// TODO: type
static u32 pose_table_0[32] = {
    0x00000001, (u32)"N_4", 0x00000009, (u32)"Y_4",
    0x00000002, (u32)"Y_4", 0x00000003, (u32)"K_4",
    0x00000004, (u32)"X_4", 0x00000005, (u32)"K_4",
    0x00000009, (u32)"Z_4B", 0x0000001C, (u32)"S_4B",
    0x0000001D, (u32)"D_4", 0x00000023, (u32)"S_4B",
    0x0000001F, (u32)"S_4B", 0x00000027, (u32)"D_4",
    0x00000038, (u32)"X_4", 0x00000039, (u32)"X_4",
    0x00000041, (u32)"S_4B", 0x00000045, (u32)"S_4B",
};

// TODO: type
static u32 pose_table_3_to_2[26] = {
    0x00000001, (u32)"B_4", 0x00000009, (u32)"B_1",
    0x00000002, (u32)"B_1", 0x00000003, (u32)"B_7",
    0x00000004, (u32)"B_10", 0x00000005, (u32)"B_7",
    0x0000001C, (u32)"B_1", 0x0000001F, (u32)"B_1",
    0x00000027, (u32)"B_1", 0x00000038, (u32)"B_10",
    0x00000039, (u32)"B_10", 0x00000041, (u32)"B_1",
    0x00000045, (u32)"B_1",
};

// TODO: type
static u32 pose_table_2_to_1[26] = {
    0x00000001, (u32)"B_5", 0x00000009, (u32)"B_2",
    0x00000002, (u32)"B_2", 0x00000003, (u32)"B_8",
    0x00000004, (u32)"B_11", 0x00000005, (u32)"B_8",
    0x0000001C, (u32)"B_2", 0x0000001F, (u32)"B_2",
    0x00000027, (u32)"B_2", 0x00000038, (u32)"B_11",
    0x00000039, (u32)"B_11", 0x00000041, (u32)"B_2",
    0x00000045, (u32)"B_2",
};

// TODO: type
static u32 pose_table_1_to_0[26] = {
    0x00000001, (u32)"B_6", 0x00000009, (u32)"B_3",
    0x00000002, (u32)"B_3", 0x00000003, (u32)"B_9",
    0x00000004, (u32)"B_12", 0x00000005, (u32)"B_9",
    0x0000001C, (u32)"B_3", 0x0000001F, (u32)"B_3",
    0x00000027, (u32)"B_3", 0x00000038, (u32)"B_12",
    0x00000039, (u32)"B_12", 0x00000041, (u32)"B_3",
    0x00000045, (u32)"B_3",
};

// TODO: type
static u32 data_table[6] = {
    0x00000031, (u32)&dead_event, 0x00000034, (u32)&explosion_event,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_sound_timing_table[45] = {
    (u32)"S_3A", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3",
    0x00000001, (u32)"S_3A", 0x3E888889, 0x00000000,
    (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_3A", 0x3F088889,
    0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_3A",
    0x3F4CCCCD, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001,
    (u32)"S_4A", 0x3C23D70A, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3",
    0x00000001, (u32)"S_4A", 0x3E2AAAAB, 0x00000000,
    (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_4A", 0x3EAAAAAB,
    0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001, (u32)"S_4A",
    0x3F000000, 0x00000000, (u32)"SFX_ENM_HEAVYB_WAIT3", 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000001,
};

// TODO: type
static u32 parts[38] = {
    0x00000001, (u32)"btl_un_giant_bomb", (u32)"c_giant", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x42480000,
    0x00000000, 0x00000000, 0x42820000, 0x00000000,
    0x00410046, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table_3, 0x00000002,
    (u32)"ジャイアントボム", (u32)"c_giant", 0x00000000, 0x42A00000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x000F000F,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x02010000,
    0x00000000, (u32)&pose_table_3,
};

static USER_FUNC(_set_draw_callback) {
    BattleWork* wp = _battleWorkPointer;
    s32 id = BattleTransID(event, -2);
    BattleUnit* unit = BattleGetUnitPtr(wp, id);
    BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, 1);

    animPoseSetDispCallBack(*(s32*)part->field_0x1C0, (void*)_fire_point_callback, part);
    part = BtlUnit_GetPartsPtr(unit, 2);
    part->attributes |= 0x01000000;
    return 2;
}

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 3)
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("F_1"))
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(pose_sound_timing_table))
    CALL(_set_draw_callback)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
            CALL(evt_snd_sfxoff, LW(0))
            CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
            CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1))
                CASE_EQUAL(0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_0))
            END_SWITCH
        END_IF
    END_IF
    CALL(btlevtcmd_CheckDamageCode, LW(10), 256, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
        SWITCH(LW(0))
            CASE_OR_EQUAL(27)
            CASE_OR_EQUAL(24)
            CASE_OR_EQUAL(25)
            CASE_OR_EQUAL(52)
            CASE_OR_EQUAL(46)
            CASE_OR_EQUAL(50)
            CASE_OR_EQUAL(54)
            GOTO(50)
            CASE_END
        END_SWITCH
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
    LABEL(50)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 29)
    CALL_CHILD(dead_event)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_PhaseEventStartDeclare, -2)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("K_4"))
                CALL(btlevtcmd_AfterReactionEntry, -2, 52)
                RETURN
            END_IF
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_WAIT2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(3)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_3_to_2))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2_to_1))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1_to_0))
            END_SWITCH
            CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
            WAIT_FRAMES(40)
            CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
            SUBTRACT(LW(0), 1)
            CALL(btlevtcmd_SetUnitWork, -2, 1, LW(0))
            SWITCH(LW(0))
                CASE_EQUAL(2)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_2_fire))
                CASE_EQUAL(1)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_1_fire))
                CASE_EQUAL(0)
                CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_0_fire))
            END_SWITCH
            CALL(btlevtcmd_StartWaitEvent, -2)
            RETURN
        END_IF
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_3"))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_4"))
        END_SWITCH
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_WAIT1"), EVTDAT_POINTER_MAX, 0, LW(0))
        CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
        CALL(btlevtcmd_OnUnitFlag, -2, 16)
    ELSE
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(3)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_1"))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_2"))
            CASE_EQUAL(1)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_3"))
            CASE_EQUAL(0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("G_4"))
        END_SWITCH
        WAIT_FRAMES(40)
        CALL(btlevtcmd_GetUnitWork, -2, 3, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(12), PTR(weapon_heavy_bomb_guard))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
            CASE_EQUAL(1)
            SET_VALUE(LW(12), PTR(weapon_heavy_bomb_guard_2))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 2)
            CASE_EQUAL(2)
            SET_VALUE(LW(12), PTR(weapon_heavy_bomb_guard_3))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
        END_SWITCH
        CALL(btlevtcmd_WeaponAftereffect, LW(12))
        CALL(btlevtcmd_CheckDamage, -2, -2, 1, LW(12), 256, LW(5))
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), -1)
        CALL(evt_snd_sfxoff, LW(0))
    END_IF
    CALL(btlevtcmd_GetDamageCode, LW(10), LW(0))
    SWITCH(LW(0))
        CASE_OR_EQUAL(27)
        CASE_OR_EQUAL(24)
        CASE_OR_EQUAL(25)
        CASE_OR_EQUAL(52)
        CASE_OR_EQUAL(46)
        CASE_OR_EQUAL(50)
        CASE_OR_EQUAL(54)
        GOTO(50)
        CASE_END
    END_SWITCH
    LABEL(10)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core)
    RETURN
    LABEL(50)
    CALL(btlevtcmd_AfterReactionEntry, -2, 52)
    RETURN
EVT_END()

static EVT_BEGIN(explosion_event)
    CALL(btlevtcmd_GetUnitWork, -2, 2, LW(0))
    CALL(evt_snd_sfxoff, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 2, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_HEAVYB_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, -2, LW(5))
    DIVIDE(LW(5), 2)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    CALL(evt_eff, 0, STRING("bomb"), 5, LW(0), LW(1), LW(2), FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_StageDispellFog)
    WAIT_MS(300)
    CALL(evt_eff, 0, STRING("bomb"), 4, -40, 30, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 40, 90, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 40, 30, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, -40, 90, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, -100, 60, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    WAIT_MS(125)
    CALL(evt_eff, 0, STRING("bomb"), 4, 100, 60, 20, FLOAT(2.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_heavy_bomb_super_explosion))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_heavy_bomb_super_explosion), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_heavy_bomb_super_explosion))
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_heavy_bomb_super_explosion))
    SET_VALUE(LW(10), 0)
    LABEL(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_heavy_bomb_super_explosion), 256, LW(5))
    IF_INT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_heavy_bomb_super_explosion), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(99)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core)
    CALL(btlevtcmd_KillUnit, -2, 0)
    RETURN
EVT_END()

static void _fire_point_callback(BattleUnitPart* part, s32 group, void* mtx) {
    BattleUnit* unit = part->mOwner;
    s32 pose;
    const char* name;

    if (unit->work[0] == 0) {
        return;
    }
    pose = *(s32*)part->field_0x1C0;
    switch (unit->work[1]) {
        case 0:
            name = "locator11";
            break;
        case 1:
            name = "locator10";
            break;
        case 2:
            name = "locator9";
            break;
        case 3:
        default:
            name = "locator8";
            break;
    }
    if (strcmp(name, animPoseGetGroupName(pose, group)) != 0) {
        return;
    }
    {
        BattleUnitPart* part2 = BtlUnit_GetPartsPtr(unit, 2);
        s32 pose2 = *(s32*)part2->field_0x1C0;
        s32 shapeId = animPoseGetShapeIdx(pose2, "pPlaneShape3");
        animPoseDrawShape(pose2, shapeId);
    }
}
// clang-format on
#pragma force_active off
