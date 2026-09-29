#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "system.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(shadow_scale_return);
static USER_FUNC(shadow_scale_small);
static USER_FUNC(shadow_scale_memory);
static USER_FUNC(pakkun_ceil_exist);
static u32 regist[6];
static u32 parts[19];
static u32 pose_table[36];
static u32 data_table[30];
static s32 init_event[31];
static s32 dmg_avoid_counter_thorn_head_event[71];
static s32 damage_event[10];
static s32 counter_damage_event[30];
static s32 return_event[256];
static s32 normal_attack_event[311];
static s32 wait_event[6];

// clang-format off
extend_data(unit_ice_pakkun_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_ice_pakkun_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000071, (u32)"btl_un_ice_pakkun", 0x000A0000, 0x01011A00,
    0x03460132, 0x270F0028, 0x00300004, 0x00300000,
    0x00000000, 0x41C00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41A00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41A00000,
    0x00000000, 0x00000000, 0x41800000, 0x42000000,
    0x00000000, 0x00000000, 0x41C00000, 0x00000000,
    0x42200000, 0x42400000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_PAKKUN_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00010300, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64462864, 0x50646400, 0x64556455, 0x6455460A,
    0x28644664, 0x645F0000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_ice_pakkun", (u32)"c_pakflwr_a", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x42200000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000001, (u32)&pose_table,
};

// TODO: type
static u32 weapon_ice_pakkun_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x21002000, 0x00000302,
    0x00000000, 0x00000087, 0x00000002, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x001E0200, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(9), PTR(weapon_ice_pakkun_attack))
        CALL_CHILD(normal_attack_event)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(9), PTR(weapon_ice_pakkun_attack))
    CALL_CHILD(normal_attack_event)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"PKF_N_1", 0x00000002, (u32)"PKF_Y_1",
    0x00000009, (u32)"PKF_Y_1", 0x00000005, (u32)"PKF_K_1",
    0x00000004, (u32)"PKF_X_1", 0x00000003, (u32)"PKF_X_1",
    0x0000001C, (u32)"PKF_S_1", 0x0000001D, (u32)"PKF_Q_1",
    0x0000001E, (u32)"PKF_Q_1", 0x0000001F, (u32)"PKF_S_2",
    0x00000027, (u32)"PKF_D_1", 0x00000032, (u32)"PKF_A_1B",
    0x00000028, (u32)"PKF_S_1", 0x0000002A, (u32)"PKF_S_1",
    0x00000038, (u32)"PKF_X_1", 0x00000039, (u32)"PKF_X_1",
    0x00000041, (u32)"PKF_T_1", 0x00000045, (u32)"PKF_S_1",
};

// TODO: type
static u32 pose_table_ceil[36] = {
    0x00000001, (u32)"PKF_N_2", 0x00000002, (u32)"PKF_Y_2",
    0x00000009, (u32)"PKF_Y_2", 0x00000005, (u32)"PKF_K_2",
    0x00000004, (u32)"PKF_X_2", 0x00000003, (u32)"PKF_X_2",
    0x0000001C, (u32)"PKF_S_3", 0x0000001D, (u32)"PKF_Q_2",
    0x0000001E, (u32)"PKF_Q_2", 0x0000001F, (u32)"PKF_S_4",
    0x00000027, (u32)"PKF_D_2", 0x00000032, (u32)"PKF_A_5",
    0x00000028, (u32)"PKF_S_3", 0x0000002A, (u32)"PKF_S_3",
    0x00000038, (u32)"PKF_X_2", 0x00000039, (u32)"PKF_X_2",
    0x00000041, (u32)"PKF_S_3", 0x00000045, (u32)"PKF_S_3",
};

// TODO: type
static u32 data_table[30] = {
    0x00000025, (u32)&dmg_avoid_counter_thorn_head_event, 0x0000001A, (u32)&counter_damage_event,
    0x00000019, (u32)&counter_damage_event, 0x0000001B, (u32)&counter_damage_event,
    0x0000001C, (u32)&counter_damage_event, 0x0000001D, (u32)&counter_damage_event,
    0x0000001E, (u32)&counter_damage_event, 0x0000001F, (u32)&counter_damage_event,
    0x00000020, (u32)&counter_damage_event, 0x00000021, (u32)&counter_damage_event,
    0x00000022, (u32)&counter_damage_event, 0x00000023, (u32)&counter_damage_event,
    0x00000024, (u32)&counter_damage_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

static USER_FUNC(pakkun_ceil_exist) {
    s32* args;

    args = event->args;
    if (*(u8*)((u8*)_battleWorkPointer + 0x180DC) & 0x20) {
        evtSetValue(event, args[0], 0);
    } else {
        evtSetValue(event, args[0], 1);
    }
    return EVT_RETURN_DONE;
}

static USER_FUNC(shadow_scale_memory) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    unit->work[0] = (s32)part->field_0x78;
    return EVT_RETURN_DONE;
}

// NONMATCHING (85.57%): MWCC schedules the intplGetValue argument setup differently (the 0.0f start is loaded after the
// int->float conversion of work[0]; same as battle_seq_end.c).
static USER_FUNC(shadow_scale_small) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    if (isFirstCall) {
        unit->work[1] = 0;
    }
    part->field_0x78 = intplGetValue(0, unit->work[1], 15, (f32)unit->work[0], 0.0f);
    if (unit->work[1] >= 15) {
        return EVT_RETURN_DONE;
    }
    unit->work[1]++;
    return EVT_RETURN_BLOCK;
}

// NONMATCHING (78.09%): MWCC schedules the intplGetValue argument setup differently (the 0.0f start is loaded after the
// int->float conversion of work[0]; same as battle_seq_end.c).
static USER_FUNC(shadow_scale_return) {
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 1);
    if (isFirstCall) {
        unit->work[1] = 0;
    }
    part->field_0x78 = intplGetValue(0, unit->work[1], 15, 0.0f, (f32)unit->work[0]);
    if (unit->work[1] >= 15) {
        return EVT_RETURN_DONE;
    }
    unit->work[1]++;
    return EVT_RETURN_BLOCK;
}


static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
    CALL(shadow_scale_memory, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dmg_avoid_counter_thorn_head_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 31)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 63)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(3))
    IF_INT_NOT_EQUAL(LW(3), 0)
        ADD(LW(1), 37)
        GOTO(90)
    END_IF
    ADD(LW(1), 50)
    GOTO(90)
    LABEL(90)
    CALL(evt_eff, STRING(""), STRING("toge_flush"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_D_1"))
    WAIT_MS(1000)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL_CHILD(return_event)
    RETURN
EVT_END()



static EVT_BEGIN(ceil_fall_ready_event)
    CALL(btlevtcmd_SetEventCeilFall, -2, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_4"))
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
    WAIT_MS(500)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_return, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffAttribute, -2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
    CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 48)
    CALL(btlevtcmd_SetKissHitOffset, -2, 16, 32, 0)
    CALL(btlevtcmd_OffAttribute, -2, 2)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
    CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    RETURN
EVT_END()


static EVT_BEGIN(return_event)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(pakkun_ceil_exist, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_sub_random, 1, LW(15))
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_2"))
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetEventCeilFall, -2, 0)
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), 0, LW(2))
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        BEGIN_SIBLING_EVENT
            CALL(shadow_scale_return, -2)
        END_SIBLING_EVENT
        CALL(btlevtcmd_OffAttribute, -2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, 50, 0)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 48)
        CALL(btlevtcmd_SetKissHitOffset, -2, 16, 32, 0)
        CALL(btlevtcmd_OffAttribute, -2, 2)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table))
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_1"))
    ELSE
        CALL(btlevtcmd_SetEventCeilFall, -2, PTR(ceil_fall_ready_event))
        CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPos, -2, LW(0), 130, LW(2))
        CALL(btlevtcmd_SetHomePos, -2, LW(0), 130, LW(2))
        WAIT_MS(500)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        BEGIN_SIBLING_EVENT
            CALL(shadow_scale_return, -2)
        END_SIBLING_EVENT
        CALL(btlevtcmd_OffAttribute, -2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_3"))
        CALL(btlevtcmd_SetHitOffset, -2, 1, 0, -80, 0)
        CALL(btlevtcmd_SetHitCursorOffset, -2, 1, 0, -50, 0)
        CALL(btlevtcmd_SetStatusIconOffset, -2, 4, 10)
        CALL(btlevtcmd_SetKissHitOffset, -2, 16, -32, 0)
        CALL(btlevtcmd_OnAttribute, -2, 2)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 4194304)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 2097152)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, 1, PTR(pose_table_ceil))
        WAIT_MS(500)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_S_3"))
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        WAIT_MS(750)
        CALL(btlevtcmd_GetFirstAttackTarget, LW(3), LW(4))
    ELSE
        CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
        CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_small, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_CheckAttribute, -2, 2, LW(15))
    IF_INT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_2"))
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_4"))
    END_IF
    WAIT_MS(300)
    CALL(btlevtcmd_OnAttribute, -2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 45)
    CALL(btlevtcmd_SetPos, -2, LW(0), 0, LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(500)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_SetHitOffset, -2, 1, 0, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(shadow_scale_return, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffAttribute, -2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 33554432)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_E_1"))
    WAIT_MS(150)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1A"))
    WAIT_MS(200)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_PAKKUN_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1B"))
    WAIT_MS(200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PKF_A_1C"))
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
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
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(98)
    LABEL(98)
    WAIT_MS(500)
    CALL_CHILD(return_event)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
