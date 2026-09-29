#include "battle/battle.h"
#include "manager/evtmgr_cmd.h"
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
static USER_FUNC(kanbu_get_id);
static u32 regist[6];
static u32 data_table[2];
static u32 parts[38];
static s32 init_event[21];
static s32 damage_event[10];
static s32 attack_event[235];
static s32 fire_bottle_event[56];
static s32 hunyahunya_bottle_event[43];
static s32 yokeyoke_bottle_event[74];
static s32 minimini_bottle_event[314];
static s32 one_recover_event[128];
static s32 all_recover_event[155];
static s32 yokeyoke_bottle_event_with_kanbu[15];
static s32 ziwaziwa_bottle_event[15];
static s32 wait_event[6];

// clang-format off
extend_data(unit_gundan_zako_magician_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_gundan_zako_magician_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x00000077, (u32)"btl_un_gundan_zako_magician", 0x00090000, 0x01011B00,
    0x03460132, 0x270F001E, 0x00240000, 0x00240000,
    0x00000000, 0x41900000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x00000000, 0x00000000, 0x41400000, 0x41D80000,
    0x00000000, 0x00000000, 0x41900000, 0x00000000,
    0x41F00000, 0x42100000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_3RD2_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x02000000, (u32)&parts, (u32)&init_event,
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
    0x50505064, 0x50646446, 0x645A645A, 0x645A465A,
    0x46644664, 0x645F0000,
};

// TODO: type
static u32 pose_table[32] = {
    0x00000001, (u32)"N_1", 0x00000002, (u32)"Y_1",
    0x00000009, (u32)"Y_1", 0x00000005, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000003, (u32)"X_1",
    0x0000001B, (u32)"Z_1", 0x0000001C, (u32)"S_1",
    0x0000001D, (u32)"D_1", 0x0000001E, (u32)"D_1",
    0x0000001F, (u32)"S_1", 0x00000027, (u32)"D_1",
    0x0000002A, (u32)"R_1", 0x00000028, (u32)"W_1",
    0x00000041, (u32)"T_1", 0x00000045, (u32)"S_1",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[38] = {
    0x00000001, (u32)"btl_un_gundan_zako_magician", (u32)"c_zako_m", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41E00000,
    0x00000000, 0x00000000, 0x42180000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_gundan_zako_magician", (u32)"c_zako_m", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13018000,
    0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_gundan_zako_magician_fire_bottle[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x01000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x0000004B,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_hunyahunya_bottle[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000004, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001000, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x004B03FD, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_yokeyoke_bottle[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01004020, 0x20000000, 0x00000302,
    0x00000000, 0x00000108, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00640300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_minimini_bottle[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    0x00000000, 0x00000007, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000004B, 0x03FE0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_one_recover[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x80001500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_all_recover[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100160, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_gundan_zako_magician_ziwaziwa_bottle[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100160, 0x20000000, 0x00000302,
    0x00000000, 0x00000008, 0x000007FF, 0x80001500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000502, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static USER_FUNC(kanbu_get_id) {
    s32* args;
    s32 id;
    s32 i;
    BattleWork* wp;
    BattleUnit* unit;

    id = -1;
    args = event->args;
    wp = _battleWorkPointer;
    for (i = 0; i < 64; i++) {
        unit = BattleGetUnitPtr(wp, i);
        if (unit && unit->initialKind == UNIT_LORD_CRUMP_CHAPTER5) {
            id = i;
            break;
        }
    }
    evtSetValue(event, args[0], id);
    if (id != -1) {
        evtSetValue(event, args[1], BtlUnit_GetBodyPartsId(unit));
    } else {
        evtSetValue(event, args[1], 0);
    }
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(kanbu_get_id, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(10)
    ELSE
        GOTO(20)
    END_IF
    LABEL(10)
    SET_VALUE(LW(0), 0)
    ADD(LW(0), 20)
    ADD(LW(0), 10)
    ADD(LW(0), 10)
    ADD(LW(0), 20)
    ADD(LW(0), 10)
    ADD(LW(0), 5)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(hunyahunya_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL(btlevtcmd_CheckStatus, -2, 7, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(yokeyoke_bottle_event)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(minimini_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(one_recover_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 5)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(all_recover_event)
        GOTO(99)
    END_IF
    GOTO(10)
    LABEL(20)
    SET_VALUE(LW(0), 0)
    ADD(LW(0), 60)
    ADD(LW(0), 20)
    ADD(LW(0), 20)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 60)
    IF_INT_LESS(LW(1), LW(0))
        CALL_CHILD(fire_bottle_event)
        GOTO(99)
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL(kanbu_get_id, LW(3), LW(4))
        CALL(btlevtcmd_CheckStatus, LW(3), 7, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(yokeyoke_bottle_event_with_kanbu)
            GOTO(99)
        END_IF
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        CALL(kanbu_get_id, LW(3), LW(4))
        CALL(btlevtcmd_CheckStatus, LW(3), 23, LW(2))
        IF_INT_EQUAL(LW(2), 0)
            CALL_CHILD(ziwaziwa_bottle_event)
            GOTO(99)
        END_IF
    END_IF
    GOTO(20)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_common_event1)
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_minimini_bottle))
        CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(10))
        IF_INT_EQUAL(LW(10), 0)
            SET_VALUE(LW(10), 125)
        ELSE
            SET_VALUE(LW(10), -125)
        END_IF
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(10), 0, 1, LW(14))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    ELSE
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(14))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(14))
    END_IF
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_minimini_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_one_recover))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_all_recover))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4A"))
    END_IF
    WAIT_FRAMES(80)
    RETURN
EVT_END()

static EVT_BEGIN(attack_common_event2)
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1A"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1B"))
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_fire_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_hunyahunya_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    END_IF
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), LW(1), LW(2), 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
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
        LABEL(90)
        CALL(btlevtcmd_JumpPartsContinue, -2, 2)
        CALL(btlevtcmd_GetPartsPos, -2, 2, LW(0), LW(1), LW(2))
        LOOP(12)
            CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
        END_LOOP
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        GOTO(98)
        LABEL(91)
        IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_fire_bottle))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
            CALL(evt_eff, STRING(""), STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
        END_IF
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        LOOP(12)
            CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), LW(1), LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
        END_LOOP
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        GOTO(98)
        LABEL(98)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(30)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(attack_common_event3)
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1D"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1C"))
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_yokeyoke_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5C"))
    END_IF
    IF_INT_EQUAL(LW(9), PTR(weapon_gundan_zako_magician_ziwaziwa_bottle))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4C"))
    END_IF
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(1.0f), FLOAT(0.5f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(0), 0, LW(2), 0, -1)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    LOOP(12)
        CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(0), 0, LW(2), FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
    END_LOOP
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
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
        LABEL(90)
        GOTO(98)
        LABEL(91)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(98)
        LABEL(98)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
        WAIT_FRAMES(10)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(30)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(fire_bottle_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_fire_bottle))
    CALL(btlevtcmd_check_battleflag, LW(0), 2)
    IF_INT_NOT_EQUAL(LW(0), 0)
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
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(hunyahunya_bottle_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_hunyahunya_bottle))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event2)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(yokeyoke_bottle_event)
    SET_VALUE(LW(3), -2)
    SET_VALUE(LW(4), 1)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_yokeyoke_bottle))
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        LOOP(3)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(20)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_5B"))
    WAIT_FRAMES(120)
    BEGIN_INLINE_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_BOTTLE5"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(55)
        CALL(evt_btl_camera_set_mode, 0, 0)
    END_INLINE_EVENT
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    RETURN
EVT_END()

static EVT_BEGIN(minimini_bottle_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_minimini_bottle))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_1E"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 10)
    ADD(LW(1), 20)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_3C"))
    WAIT_FRAMES(24)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(24)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetEnemyBelong, LW(3), LW(10))
    IF_INT_EQUAL(LW(10), 0)
        SET_VALUE(LW(10), 125)
    ELSE
        SET_VALUE(LW(10), -125)
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 2, 0, FLOAT(6.0f), FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 2, LW(10), 0, 0, 0, -1)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 20, LW(10), 50, 0, FLOAT(2.4f), 0, 0, 0, 0, 0, 0, 0)
    LOOP(12)
        CALL(evt_eff64, STRING(""), STRING("glass_n64"), 0, LW(10), 0, 0, FLOAT(1.0f), 30, 0, 0, 0, 0, 0, 0)
    END_LOOP
    BEGIN_INLINE_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_CRASH1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("B_2"))
        WAIT_FRAMES(8)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    END_INLINE_EVENT
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(0)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(9), 0, LW(5))
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
        LABEL(90)
        GOTO(97)
        LABEL(91)
        IF_INT_EQUAL(LW(10), 0)
            CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
        END_IF
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
        GOTO(97)
        LABEL(97)
        CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
        IF_INT_NOT_EQUAL(LW(3), -1)
            ADD(LW(10), 1)
            GOTO(0)
        END_IF
        LABEL(98)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_FRAMES(60)
        LABEL(99)
        CALL(btlevtcmd_ResetFaceDirection, -2)
        RETURN
EVT_END()

static EVT_BEGIN(one_recover_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_one_recover))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4B"))
    WAIT_FRAMES(80)
    CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 10)
    CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHeight, LW(3), LW(5))
    CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
    MULTIPLY_FLOAT(LW(5), LW(6))
    ADD(LW(1), LW(5))
    CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 10, 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(all_recover_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_all_recover))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL_CHILD(attack_common_event1)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(10)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(30)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_3RD2_SHAKE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_4B"))
    WAIT_FRAMES(80)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    LABEL(0)
    BEGIN_SIBLING_EVENT
        CALL(evt_sub_random, 10, LW(10))
        ADD(LW(10), 5)
        WAIT_FRAMES(LW(10))
        CALL(btlevtcmd_RecoverHp, LW(3), LW(4), 5)
        CALL(btlevtcmd_GetPartsPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_GetHeight, LW(3), LW(5))
        CALL(btlevtcmd_GetStatusMg, LW(3), LW(6))
        MULTIPLY_FLOAT(LW(5), LW(6))
        ADD(LW(1), LW(5))
        CALL(evt_eff, STRING(""), STRING("recovery"), 0, LW(0), LW(1), LW(2), 5, 0, 0, 0, 0, 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        GOTO(0)
    END_IF
    WAIT_FRAMES(90)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(yokeyoke_bottle_event_with_kanbu)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_yokeyoke_bottle))
    CALL(kanbu_get_id, LW(3), LW(4))
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event3)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(ziwaziwa_bottle_event)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_magician_ziwaziwa_bottle))
    CALL(kanbu_get_id, LW(3), LW(4))
    CALL_CHILD(attack_common_event1)
    CALL_CHILD(attack_common_event3)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
