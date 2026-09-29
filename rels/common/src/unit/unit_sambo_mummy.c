#include "battle/battle_mario.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "party/party_motion.h"

#pragma force_active on
extern s32 btldefaultevt_Dummy[];
extern s32 marioWeapon_UltraHammerFinish[];
static u32 regist[6];
static u32 data_table[16];
static u32 parts[95];
static s32 init_event[54];
static s32 dmg_avoid_counter_thorn_side_event[51];
static s32 damage_event[34];
static s32 damage_event_bodylost[273];
static s32 subset_dmg_sambo_fall[152];
static s32 damage_event_bodylost_blow[354];
static s32 damage_swallow_body[256];
static s32 attack_event[189];
static s32 attack_head_2_event[613];
static s32 attack_head_1_event[587];
static s32 attack_spike_event[824];
static s32 call_friend_event[304];
static u32 entry_sambo[12];
static s32 wait_event[36];
static s32 sambo_rabbit_move[74];
static s32 _body_param_set[49];

// clang-format off
extend_data(sambo_mummy_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(sambo_mummy_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000068, (u32)"btl_un_sambo_mummy", 0x00080000, 0x01011700,
    0x02460132, 0x270F001A, 0x0050000E, 0x00500000,
    0x00000000, 0x42200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41500000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41500000,
    0x00000000, 0x00000000, 0x41500000, 0x42500000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x41D00000, 0x42A00000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_SAMBO_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x05000000, (u32)&parts, (u32)&init_event,
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
    0x4B3C3C64, 0x46646428, 0x64556455, 0x645A4650,
    0x46644664, 0x645F0000,
};

// TODO: type
static u32 weapon_sambo_mummy_head_attack_4[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21000004, 0x00000302,
    0x00000000, 0x00000087, 0x00000000, 0x80002004,
    0x00000000, 0x00003C0A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_head_attack_3[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21000004, 0x00000302,
    0x00000000, 0x00000087, 0x00000000, 0x80002004,
    0x00000000, 0x00003C0A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_head_attack_2[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21004004, 0x00000302,
    0x00000000, 0x00000087, 0x00000000, 0x80002004,
    0x00000000, 0x00003C0A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_head_attack_1[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21004004, 0x00000302,
    0x00000000, 0x00001087, 0x00000000, 0x80002004,
    0x00000000, 0x00003C0A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_spike_attack_4[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_spike_attack_3[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_sambo_mummy_spike_attack_2[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table_body4[36] = {
    0x00000001, (u32)"SNB_N_1", 0x00000002, (u32)"SNB_Y_1",
    0x00000009, (u32)"SNB_Y_1", 0x00000005, (u32)"SNB_K_1",
    0x00000004, (u32)"SNB_X_1", 0x00000003, (u32)"SNB_X_1",
    0x0000001C, (u32)"SNB_S_1", 0x0000001D, (u32)"SNB_Q_1",
    0x0000001E, (u32)"SNB_Q_1", 0x0000001F, (u32)"SNB_S_1",
    0x00000027, (u32)"SNB_D_1", 0x0000002F, (u32)"SNB_W_1",
    0x0000002A, (u32)"SNB_W_1", 0x00000028, (u32)"SNB_W_1",
    0x00000038, (u32)"SNB_X_1", 0x00000039, (u32)"SNB_X_1",
    0x00000041, (u32)"SNB_T_1", 0x00000045, (u32)"SNB_S_1",
};

// TODO: type
static u32 pose_table_body3[40] = {
    0x00000001, (u32)"SNB_N_2", 0x00000002, (u32)"SNB_Y_2",
    0x00000009, (u32)"SNB_Y_2", 0x00000005, (u32)"SNB_K_2",
    0x00000004, (u32)"SNB_X_2", 0x00000003, (u32)"SNB_X_2",
    0x0000001C, (u32)"SNB_S_2", 0x0000001D, (u32)"SNB_Q_2",
    0x0000001E, (u32)"SNB_Q_2", 0x0000001F, (u32)"SNB_S_2",
    0x00000027, (u32)"SNB_D_2", 0x0000002F, (u32)"SNB_W_2",
    0x00000032, (u32)"SNB_A_1A", 0x00000033, (u32)"SNB_A_1B",
    0x0000002A, (u32)"SNB_W_2", 0x00000028, (u32)"SNB_W_2",
    0x00000038, (u32)"SNB_X_2", 0x00000039, (u32)"SNB_X_2",
    0x00000041, (u32)"SNB_T_2", 0x00000045, (u32)"SNB_S_2",
};

// TODO: type
static u32 pose_table_body2[40] = {
    0x00000001, (u32)"SNB_N_3", 0x00000002, (u32)"SNB_Y_3",
    0x00000009, (u32)"SNB_Y_3", 0x00000005, (u32)"SNB_K_3",
    0x00000004, (u32)"SNB_X_3", 0x00000003, (u32)"SNB_X_3",
    0x0000001C, (u32)"SNB_S_3", 0x0000001D, (u32)"SNB_Q_3",
    0x0000001E, (u32)"SNB_Q_3", 0x0000001F, (u32)"SNB_S_3",
    0x00000027, (u32)"SNB_D_3", 0x0000002F, (u32)"SNB_W_3",
    0x00000032, (u32)"SNB_A_2A", 0x00000033, (u32)"SNB_A_2B",
    0x0000002A, (u32)"SNB_W_3", 0x00000028, (u32)"SNB_W_3",
    0x00000038, (u32)"SNB_X_3", 0x00000039, (u32)"SNB_X_3",
    0x00000041, (u32)"SNB_T_3", 0x00000045, (u32)"SNB_S_3",
};

// TODO: type
static u32 pose_table_body1[40] = {
    0x00000001, (u32)"SNB_N_4", 0x00000002, (u32)"SNB_Y_4",
    0x00000009, (u32)"SNB_Y_4", 0x00000005, (u32)"SNB_K_4",
    0x00000004, (u32)"SNB_X_4", 0x00000003, (u32)"SNB_X_4",
    0x0000001C, (u32)"SNB_S_4", 0x0000001D, (u32)"SNB_Q_4",
    0x0000001E, (u32)"SNB_Q_4", 0x0000001F, (u32)"SNB_S_4",
    0x00000027, (u32)"SNB_D_4", 0x0000002F, (u32)"SNB_W_4",
    0x00000032, (u32)"SNB_A_3A", 0x00000033, (u32)"SNB_A_3B",
    0x0000002A, (u32)"SNB_W_4", 0x00000028, (u32)"SNB_W_4",
    0x00000038, (u32)"SNB_X_4", 0x00000039, (u32)"SNB_X_4",
    0x00000041, (u32)"SNB_T_4", 0x00000045, (u32)"SNB_S_4",
};

// TODO: type
static u32 pose_table_parts[6] = {
    0x0000001C, (u32)"SNB_B_1", 0x00000027, (u32)"SNB_B_1",
    0x00000045, (u32)"SNB_B_1",
};

// TODO: type
static u32 data_table[16] = {
    0x00000025, (u32)&dmg_avoid_counter_thorn_side_event, 0x00000027, (u32)&dmg_avoid_counter_thorn_side_event,
    0x00000013, (u32)&damage_event_bodylost, 0x00000012, (u32)&damage_event_bodylost_blow,
    0x00000010, (u32)&damage_event_bodylost, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000036, (u32)&damage_swallow_body, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[95] = {
    0x00000001, (u32)"btl_un_sambo_mummy", (u32)"c_sanbo_m", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42A80000,
    0x00000000, 0x00000000, 0x42B40000, 0x00000000,
    0x0014005A, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000005, (u32)&pose_table_body4, 0x00000002,
    (u32)"btl_un_sambo_mummy", (u32)"c_sanbo_m", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x42800000, 0x00000000,
    0x00000000, 0x428C0000, 0x00000000, 0x00140046,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x03010000,
    0x00000005, (u32)&pose_table_body3, 0x00000003, (u32)"btl_un_sambo_mummy",
    (u32)"c_sanbo_m", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x42300000, 0x00000000, 0x00000000,
    0x42480000, 0x00000000, 0x00140032, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x03010000, 0x00000005,
    (u32)&pose_table_body2, 0x00000004, (u32)"btl_un_sambo_mummy", (u32)"c_sanbo_m",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x41C00000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x03010000, 0x00000005, (u32)&pose_table_body1,
    0x00000005, (u32)"btl_un_sambo_mummy", (u32)"c_sanbo_m", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x13010000, 0x00000000, (u32)&pose_table_parts,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 4)
    CALL_CHILD(_body_param_set)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 5, 69)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_ENM_SAMBO_MOVE1"), STRING("SFX_ENM_SAMBO_MOVE1"), 0, 6, 6)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_ENM_SAMBO_MOVE1"), STRING("SFX_ENM_SAMBO_MOVE1"), 0, 6, 6)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_side_event)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), 47)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(10), LW(3))
    CALL(btlevtcmd_GetStatusMg, LW(10), LW(4))
    MULTIPLY_FLOAT(LW(3), LW(4))
    ADD(LW(1), LW(3))
    ADD(LW(1), 5)
    ADD(LW(2), 10)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(60)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(11), 4)
        CASE_EQUAL(2)
        SET_VALUE(LW(11), 3)
        CASE_EQUAL(3)
        SET_VALUE(LW(11), 2)
        CASE_DEFAULT
        SET_VALUE(LW(11), 1)
    END_SWITCH
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_bodylost)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetDamage, LW(10), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            WAIT_FRAMES(30)
            IF_INT_NOT_EQUAL(LW(15), -1)
                CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(15))
                WAIT_FRAMES(30)
            END_IF
        ELSE
            CALL_CHILD(subsetevt_nodamage_motion)
        END_IF
    ELSE
        SET_VALUE(LW(12), LW(11))
        CALL(btlevtcmd_GetUnitWork, LW(10), 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, LW(10), 0, LW(0))
        CALL_CHILD(_body_param_set)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            SET_VALUE(LW(11), 4)
            CALL(btlevtcmd_SetSwallowParam, -2, 50)
            CALL(btlevtcmd_SetSwallowAttribute, -2, 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 20)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 10, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 16, 0)
            CASE_EQUAL(2)
            SET_VALUE(LW(11), 3)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 40)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 20, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 26, 0)
            CASE_EQUAL(3)
            SET_VALUE(LW(11), 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 60)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 30, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 36, 0)
            CASE_DEFAULT
            SET_VALUE(LW(11), 1)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 80)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 40, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 52, 0)
        END_SWITCH
        CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(12), 9)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(12), 50397184)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 9)
        CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(11), 16842752)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(4)
            CALL_CHILD(subset_dmg_sambo_fall)
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, LW(10), 5, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, LW(10), 5, 16777216)
        CALL(btlevtcmd_SetPartsFallAccel, LW(10), 5, FLOAT(0.4f))
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(0), 25)
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsPosition, LW(10), 5, LW(0), LW(1), LW(2), 16, -1)
        CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(0), 16)
        CALL(btlevtcmd_JumpPartsPosition, LW(10), 5, LW(0), LW(1), LW(2), 12, -1)
        LOOP(40)
            CALL(btlevtcmd_OnPartsAttribute, LW(10), 5, 16777216)
            WAIT_FRAMES(1)
            CALL(btlevtcmd_OffPartsAttribute, LW(10), 5, 16777216)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_OnPartsAttribute, LW(10), 5, 16777216)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(subset_dmg_sambo_fall)
    CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(1), 18)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    SET_VALUE(LW(3), LW(1))
    WAIT_FRAMES(2)
    SUBTRACT(LW(3), 1)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(3)
    SUBTRACT(LW(3), 2)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(2)
    SUBTRACT(LW(3), 3)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    SUBTRACT(LW(3), 5)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    SUBTRACT(LW(3), 7)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    ADD(LW(3), 2)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    ADD(LW(3), 3)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    ADD(LW(3), 3)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    SUBTRACT(LW(3), 3)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    SUBTRACT(LW(3), 3)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    WAIT_FRAMES(1)
    SUBTRACT(LW(3), 2)
    CALL(btlevtcmd_SetPos, LW(10), LW(0), LW(3), LW(2))
    RETURN
EVT_END()

static EVT_BEGIN(damage_event_bodylost_blow)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetDamage, LW(10), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL_CHILD(subsetevt_shot_damage)
        ELSE
            CALL_CHILD(subsetevt_nodamage_motion)
        END_IF
    ELSE
        SET_VALUE(LW(12), LW(11))
        CALL(btlevtcmd_GetUnitWork, LW(10), 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, LW(10), 0, LW(0))
        CALL_CHILD(_body_param_set)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            SET_VALUE(LW(11), 4)
            CALL(btlevtcmd_SetSwallowParam, -2, 50)
            CALL(btlevtcmd_SetSwallowAttribute, -2, 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 20)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 10, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 16, 0)
            CASE_EQUAL(2)
            SET_VALUE(LW(11), 3)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 40)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 20, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 26, 0)
            CASE_EQUAL(3)
            SET_VALUE(LW(11), 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 60)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 30, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 36, 0)
            CASE_DEFAULT
            SET_VALUE(LW(11), 1)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 80)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 40, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 52, 0)
        END_SWITCH
        CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(12), 9)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(12), 50397184)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 9)
        CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(11), 16842752)
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(4)
            CALL_CHILD(subset_dmg_sambo_fall)
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, LW(10), 5, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, LW(10), 5, 16777216)
        CALL(btlevtcmd_CheckDamagePattern, LW(10), 15, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            SET_VALUE(LW(13), PTR(marioWeapon_KaitenHammerInvolved))
        ELSE
            SET_VALUE(LW(13), PTR(marioWeapon_UltraHammerInvolved))
        END_IF
        CALL(btlevtcmd_GetFriendBelong, LW(10), LW(0))
        CALL(btlevtcmd_SamplingEnemy, LW(10), LW(0), LW(13))
        CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
        LABEL(0)
        IF_INT_EQUAL(LW(3), -1)
            GOTO(50)
        END_IF
        CALL(btlevtcmd_GetUnitId, -2, LW(10))
        IF_INT_NOT_EQUAL(LW(10), LW(3))
            CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
            GOTO(0)
        END_IF
        LABEL(10)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_EQUAL(LW(3), -1)
            GOTO(50)
        END_IF
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHomePos, LW(10), EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        ADD(LW(1), 15)
        CALL(btlevtcmd_JumpPartsSetting, LW(10), 5, 0, FLOAT(5.0f), FLOAT(0.01f))
        CALL(btlevtcmd_JumpPartsPosition, LW(10), 5, LW(0), LW(1), LW(2), 0, -1)
        CALL(btlevtcmd_CheckDamage, LW(10), LW(3), LW(4), LW(13), 256, LW(5))
        GOTO(10)
        LABEL(50)
        CALL(btlevtcmd_GetStageSize, LW(5), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(0), 0)
        CALL(btlevtcmd_FaceDirectionSub, LW(10), LW(0), LW(5))
        CALL(btlevtcmd_GetPos, LW(10), EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpPartsSetting, LW(10), 5, 0, FLOAT(5.0f), FLOAT(0.01f))
        CALL(btlevtcmd_JumpPartsPosition, LW(10), 5, LW(0), LW(1), LW(2), 0, -1)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), 5, 16777216)
        CALL(btlevtcmd_CheckDamagePattern, LW(10), 16, LW(5))
        IF_INT_NOT_EQUAL(LW(5), 0)
            CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(marioWeapon_UltraHammerFinish), 131328, LW(5))
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(damage_swallow_body)
    SET_VALUE(LW(14), 39)
    SET_VALUE(LW(15), -1)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(btlevtcmd_GetDamage, LW(10), LW(0))
        IF_INT_GREATER_OR_EQUAL(LW(0), 1)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            WAIT_FRAMES(30)
            IF_INT_NOT_EQUAL(LW(15), -1)
                CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(15))
                WAIT_FRAMES(30)
            END_IF
        ELSE
            CALL_CHILD(subsetevt_nodamage_motion)
        END_IF
    ELSE
        WAIT_FRAMES(3)
        SET_VALUE(LW(12), LW(11))
        CALL(btlevtcmd_GetUnitWork, LW(10), 0, LW(0))
        SUBTRACT(LW(0), 1)
        CALL(btlevtcmd_SetUnitWork, LW(10), 0, LW(0))
        CALL_CHILD(_body_param_set)
        SWITCH(LW(0))
            CASE_EQUAL(1)
            SET_VALUE(LW(11), 4)
            CALL(btlevtcmd_SetSwallowParam, -2, 50)
            CALL(btlevtcmd_SetSwallowAttribute, -2, 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 20)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 10, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 16, 0)
            CASE_EQUAL(2)
            SET_VALUE(LW(11), 3)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 40)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 20, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 26, 0)
            CASE_EQUAL(3)
            SET_VALUE(LW(11), 2)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 60)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 30, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 36, 0)
            CASE_DEFAULT
            SET_VALUE(LW(11), 1)
            CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 80)
            CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 40, 0)
            CALL(btlevtcmd_SetKissHitOffset, -2, 10, 52, 0)
        END_SWITCH
        BEGIN_SIBLING_EVENT
            WAIT_FRAMES(3)
            CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(12), 9)
            CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(12), 50397184)
            CALL(btlevtcmd_OnPartsAttribute, LW(10), LW(11), 9)
            CALL(btlevtcmd_OffPartsAttribute, LW(10), LW(11), 16842752)
            CALL(btlevtcmd_AnimeChangePoseType, LW(10), LW(11), LW(14))
            WAIT_FRAMES(1)
            CALL_CHILD(subset_dmg_sambo_fall)
        END_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsPos, LW(10), 5, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, LW(10), 5, 16777216)
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 5, 0, 0, 0, -1)
        CALL(btlevtcmd_OnPartsAttribute, LW(10), 5, 16777216)
        WAIT_FRAMES(20)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    SWITCH(LW(10))
        CASE_EQUAL(1)
        SET_VALUE(LW(11), 4)
        CASE_EQUAL(2)
        SET_VALUE(LW(11), 3)
        CASE_EQUAL(3)
        SET_VALUE(LW(11), 2)
        CASE_DEFAULT
        SET_VALUE(LW(11), 1)
    END_SWITCH
    IF_INT_EQUAL(LW(10), 4)
        CALL(btlevtcmd_GetUnitWork, -2, 1, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 1)
            CALL(evt_sub_random, 99, LW(0))
            IF_INT_LESS(LW(0), 20)
                CALL(btlevtcmd_GetPos, -2, LW(6), LW(7), LW(8))
                CALL(btlevtcmd_FaceDirectionAdd, -2, LW(6), 40)
                SUBTRACT(LW(8), 10)
                CALL(btlevtcmd_CheckSpace, LW(3), LW(6), LW(7), LW(8), 40, 0, 0, 0)
                IF_INT_EQUAL(LW(3), -1)
                    IF_INT_GREATER(LW(6), 0)
                        CALL_CHILD(call_friend_event)
                        GOTO(99)
                    END_IF
                END_IF
            END_IF
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(10), 2)
        CALL(evt_sub_random, 69, LW(0))
        IF_INT_LESS_OR_EQUAL(LW(0), 30)
            SWITCH(LW(10))
                CASE_EQUAL(4)
                CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
                CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_4))
                CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_4), LW(3), LW(4))
                CASE_EQUAL(3)
                CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
                CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_3))
                CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_3), LW(3), LW(4))
                CASE_EQUAL(2)
                CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
                CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_2))
                CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_2), LW(3), LW(4))
            END_SWITCH
            IF_INT_NOT_EQUAL(LW(3), -1)
                CALL_CHILD(attack_head_2_event)
                RETURN
            END_IF
        END_IF
        CALL_CHILD(attack_spike_event)
        RETURN
    END_IF
    CALL_CHILD(attack_head_1_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_head_2_event)
    SET_VALUE(LW(6), LW(11))
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_4))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_4), LW(3), LW(4))
        CASE_EQUAL(3)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_3))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_3), LW(3), LW(4))
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_2))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_2), LW(3), LW(4))
    END_SWITCH
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_head_attack_4))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_head_attack_4))
        CASE_EQUAL(3)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_head_attack_3))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_head_attack_3))
        CASE_EQUAL(2)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_head_attack_2))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_head_attack_2))
    END_SWITCH
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SWITCH(LW(10))
        CASE_EQUAL(4)
        IF_INT_EQUAL(LW(5), 0)
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 60)
        ELSE
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 42)
        END_IF
        CASE_EQUAL(3)
        IF_INT_EQUAL(LW(5), 0)
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 40)
        ELSE
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 28)
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(LW(5), 0)
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 20)
        ELSE
            CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 14)
        END_IF
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 43)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    SET_VALUE(LW(14), 40)
    IF_INT_EQUAL(LW(15), -1)
        SET_VALUE(LW(14), -40)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    CALL(evt_sub_intpl_init, 11, 0, LW(14), 40)
    LABEL(0)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    SET_VALUE(LW(13), 40)
    SET_VALUE(LW(14), -90)
    IF_INT_EQUAL(LW(15), -1)
        SET_VALUE(LW(13), -40)
        SET_VALUE(LW(14), 90)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 1, LW(13), LW(14), 10)
    LABEL(3)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_4), 256, LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_3), 256, LW(5))
        CASE_EQUAL(2)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_2), 256, LW(5))
    END_SWITCH
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
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    SET_VALUE(LW(14), -90)
    IF_INT_EQUAL(LW(15), -1)
        SET_VALUE(LW(14), 90)
    END_IF
    CALL(evt_sub_intpl_init, 0, LW(14), 0, 10)
    LABEL(4)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(4)
    END_IF
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_sambo_mummy_head_attack_4))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_4), 256, LW(5))
        CASE_EQUAL(3)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_sambo_mummy_head_attack_3))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_3), 256, LW(5))
        CASE_EQUAL(2)
        CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_sambo_mummy_head_attack_2))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_2), 256, LW(5))
    END_SWITCH
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 90)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 43)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    SET_VALUE(LW(14), -90)
    IF_INT_EQUAL(LW(15), -1)
        SET_VALUE(LW(14), 90)
    END_IF
    CALL(evt_sub_intpl_init, 0, LW(14), 0, 20)
    LABEL(5)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(5)
    END_IF
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 41)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_head_1_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_head_attack_1))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_head_attack_1), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_head_attack_1))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_head_attack_1))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.6f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(5), LW(6), LW(7))
    CALL(btlevtcmd_TransStageFloorPosition, LW(5), LW(6), LW(7))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(5), 50)
    SET_VALUE(LW(8), 35)
    CALL_CHILD(sambo_rabbit_move)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_1), 256, LW(5))
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
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 5)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(2)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        SET_VALUE(LW(14), -10)
        IF_INT_EQUAL(LW(15), -1)
            SET_VALUE(LW(14), 10)
        END_IF
        SET_VALUE(LW(0), 0)
        LOOP(15)
            ADD(LW(0), LW(14))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 44, -1)
    CALL(btlevtcmd_SetRotateOffset, -2, 0, 10, 0)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(2)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        SET_VALUE(LW(0), 180)
        LOOP(12)
            IF_INT_EQUAL(LW(15), -1)
                ADD(LW(0), 15)
                IF_INT_EQUAL(LW(0), 360)
                    SET_VALUE(LW(0), 0)
                END_IF
            ELSE
                SUBTRACT(LW(0), 15)
                IF_INT_EQUAL(LW(0), 0)
                    SET_VALUE(LW(0), 360)
                END_IF
            END_IF
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(1.0f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 30)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(2)
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
        SET_VALUE(LW(14), -10)
        IF_INT_EQUAL(LW(15), -1)
            SET_VALUE(LW(14), 10)
        END_IF
        SET_VALUE(LW(0), 0)
        LOOP(15)
            ADD(LW(0), LW(14))
            CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 36, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK6"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_sambo_mummy_head_attack_1))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_head_attack_1), 256, LW(5))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.9f))
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 30)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 12, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 10)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 8, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 4, 43)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 4, 41)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.5f))
    CALL(btlevtcmd_GetHomePos, -2, LW(5), LW(6), LW(7))
    SET_VALUE(LW(8), 35)
    CALL_CHILD(sambo_rabbit_move)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_spike_event)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_spike_attack_4))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_spike_attack_4), LW(3), LW(4))
        CASE_EQUAL(3)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_spike_attack_3))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_spike_attack_3), LW(3), LW(4))
        CASE_EQUAL(2)
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_sambo_mummy_spike_attack_2))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_sambo_mummy_spike_attack_2), LW(3), LW(4))
    END_SWITCH
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    SWITCH(LW(10))
        CASE_EQUAL(4)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_spike_attack_4))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_spike_attack_4))
        CASE_EQUAL(3)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_spike_attack_3))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_spike_attack_3))
        CASE_EQUAL(2)
        CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_sambo_mummy_spike_attack_2))
        CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_sambo_mummy_spike_attack_2))
    END_SWITCH
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_SetPartsScale, -2, 5, FLOAT(0.7f), FLOAT(0.7f), FLOAT(1.0f))
    ELSE
        CALL(btlevtcmd_SetPartsScale, -2, 5, FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(10))
    SUBTRACT(LW(10), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, LW(10))
    CALL_CHILD(_body_param_set)
    SWITCH(LW(10))
        CASE_EQUAL(3)
        SET_VALUE(LW(7), 1)
        SET_VALUE(LW(6), 2)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 60)
        CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 30, 0)
        CALL(btlevtcmd_SetKissHitOffset, -2, 10, 36, 0)
        CASE_EQUAL(2)
        SET_VALUE(LW(7), 2)
        SET_VALUE(LW(6), 3)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 40)
        CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 20, 0)
        CALL(btlevtcmd_SetKissHitOffset, -2, 10, 26, 0)
        CASE_EQUAL(1)
        SET_VALUE(LW(7), 3)
        SET_VALUE(LW(6), 4)
        CALL(btlevtcmd_SetSwallowParam, -2, 50)
        CALL(btlevtcmd_SetSwallowAttribute, -2, 2)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 14, 20)
        CALL(btlevtcmd_SetCutBaseOffset, -2, 0, 10, 0)
        CALL(btlevtcmd_SetKissHitOffset, -2, 10, 16, 0)
    END_SWITCH
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(7), 9)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(7), 50397184)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(6), 9)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(6), 16842752)
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        IF_INT_EQUAL(LW(5), 0)
            SET_VALUE(LW(9), 20)
            SET_VALUE(LW(11), 20)
        ELSE
            SET_VALUE(LW(9), 14)
            SET_VALUE(LW(11), 14)
        END_IF
        MULTIPLY(LW(9), LW(10))
        SUBTRACT(LW(9), LW(11))
        ADD(LW(1), LW(9))
        CALL(btlevtcmd_SetPartsPos, -2, 5, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_OffPartsAttribute, -2, 5, 16777216)
        CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.6f))
        CALL(btlevtcmd_AnimeChangePose, -2, 5, STRING("SNB_B_1"))
        CALL(btlevtcmd_JumpPartsPosition, -2, 5, LW(0), LW(1), LW(2), 30, -1)
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 50)
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_ATTACK5"), EVTDAT_POINTER_MAX, 0, LW(14))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 51)
    WAIT_FRAMES(2)
    WAIT_FRAMES(5)
    SWITCH(LW(10))
        CASE_EQUAL(3)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_spike_attack_4), 256, LW(5))
        CASE_EQUAL(2)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_spike_attack_3), 256, LW(5))
        CASE_EQUAL(1)
        CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_sambo_mummy_spike_attack_2), 256, LW(5))
    END_SWITCH
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
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStageSize, LW(0), EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetFaceDirection, -2, LW(9))
    MULTIPLY(LW(0), LW(9))
    SUBTRACT(LW(1), 5)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(10.0f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.1f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, -1)
    CALL(evt_snd_sfxoff, LW(14))
    CALL(btlevtcmd_OnPartsAttribute, -2, 5, 16777216)
    GOTO(99)
    LABEL(91)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(10.0f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.1f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, -1)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    SWITCH(LW(10))
        CASE_EQUAL(3)
        SET_VALUE(LW(12), PTR(weapon_sambo_mummy_spike_attack_4))
        CASE_EQUAL(2)
        SET_VALUE(LW(12), PTR(weapon_sambo_mummy_spike_attack_3))
        CASE_EQUAL(1)
        SET_VALUE(LW(12), PTR(weapon_sambo_mummy_spike_attack_2))
    END_SWITCH
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(12))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(6), 43)
    IF_INT_EQUAL(LW(5), 21)
        CALL(evt_snd_sfxoff, LW(14))
        CALL(btlevtcmd_GetBodyId, -2, LW(11))
        CALL(btlevtcmd_GetHitPos, -2, LW(11), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(4.5f))
        CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.4f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 5, LW(0), LW(1), LW(2), 40, -1)
        BEGIN_INLINE_EVENT
            CALL(btlevtcmd_GetPartsPos, -2, 5, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 55)
            CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(3.0f))
            CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.1f))
            CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
            CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(2.0f))
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 24)
            CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 21, 0, 0, -1)
            CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(1.5f))
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
            CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 20, 0, 0, -1)
            CALL(btlevtcmd_OnPartsAttribute, -2, 5, 16777216)
        END_INLINE_EVENT
        CALL(btlevtcmd_CheckDamage, -2, -2, LW(11), LW(12), 256, LW(5))
        RETURN
    END_IF
    CALL(evt_snd_sfxoff, LW(14))
    CALL(btlevtcmd_GetPartsPos, -2, 5, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 55)
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(3.0f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, 5, FLOAT(0.1f))
    CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(2.0f))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 24)
    CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 21, 0, 0, -1)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, 5, FLOAT(1.5f))
    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(0), 20)
    CALL(btlevtcmd_DivePartsPosition, -2, 5, LW(0), LW(1), LW(2), 0, 20, 0, 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, 5, 16777216)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(call_friend_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("SNB_S_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_SEND_GROUP1_3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 11, 0, -20, 40)
    LABEL(0)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_SEND_GROUP1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 11, -20, 30, 40)
    LABEL(1)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_SEND_GROUP1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 11, 30, -40, 40)
    LABEL(2)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_SEND_GROUP2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(evt_sub_intpl_init, 11, -40, 90, 20)
    LABEL(3)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(3)
    END_IF
    WAIT_FRAMES(40)
    BEGIN_INLINE_EVENT
        CALL(evt_btl_camera_shake_h, 0, 2, 0, 10, 13)
    END_INLINE_EVENT
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_sambo), 0)
    SET_VALUE(LW(0), LW(6))
    SET_VALUE(LW(1), LW(7))
    SET_VALUE(LW(2), LW(8))
    SUBTRACT(LW(1), 80)
    CALL(btlevtcmd_SetPos, LW(3), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(evt_eff, 0, STRING("kemuri_test"), 14, LW(0), LW(1), LW(2), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_SEND_GROUP3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, LW(3), 1, STRING("SNB_D_1"))
        CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, LW(3), FLOAT(0.7f))
        CALL(btlevtcmd_JumpPosition, LW(3), LW(6), LW(7), LW(8), 20, -1)
        CALL(btlevtcmd_SetHomePos, LW(3), LW(6), LW(7), LW(8))
        WAIT_FRAMES(40)
        CALL(btlevtcmd_StartWaitEvent, LW(3))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(4))
    ADD(LW(4), 1)
    CALL(btlevtcmd_SetUnitWork, LW(3), 1, LW(4))
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_sub_intpl_init, 4, 90, -30, 14)
    LABEL(4)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(4)
    END_IF
    WAIT_FRAMES(40)
    CALL(evt_sub_intpl_init, 11, -30, 0, 20)
    LABEL(5)
    CALL(evt_sub_intpl_get_value)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, LW(0))
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(5)
    END_IF
    WAIT_FRAMES(40)
    RETURN
EVT_END()

// TODO: type
static u32 entry_sambo[12] = {
    (u32)&UNIT_DATA_SYM, 0x01000000, 0x04000004, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(wait_event)
    SET_VALUE(LW(10), -2)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(11), 4)
        CASE_EQUAL(2)
        SET_VALUE(LW(11), 3)
        CASE_EQUAL(3)
        SET_VALUE(LW(11), 2)
        CASE_DEFAULT
        SET_VALUE(LW(11), 1)
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseFromTable, LW(10), LW(11))
    RETURN
EVT_END()

static EVT_BEGIN(sambo_rabbit_move)
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
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(6), LW(2), 0, -1)
        GOTO(0)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_SAMBO_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_JumpPosition, -2, LW(5), LW(6), LW(7), 0, -1)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(_body_param_set)
    CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_SetHeight, -2, 20)
        CALL(btlevtcmd_SetCutHeight, -2, 20)
        CASE_EQUAL(2)
        CALL(btlevtcmd_SetHeight, -2, 40)
        CALL(btlevtcmd_SetCutHeight, -2, 40)
        CASE_EQUAL(3)
        CALL(btlevtcmd_SetHeight, -2, 60)
        CALL(btlevtcmd_SetCutHeight, -2, 60)
        CASE_DEFAULT
        CALL(btlevtcmd_SetHeight, -2, 80)
        CALL(btlevtcmd_SetCutHeight, -2, 80)
    END_SWITCH
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
