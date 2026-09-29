#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "driver/effdrv.h"
#include "eff/eff_fire.h"
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
static USER_FUNC(eff_all_fire);
static USER_FUNC(eff_fire);
static EffectEntry* eff[1];
static EffectEntry* eff2[10];
static s32 step[10];
static USER_FUNC(bubble_copy_status);
static u32 regist[6];
static u32 data_table[8];
static u32 parts[38];
static s32 init_event[39];
static s32 damage_event[10];
static s32 avoid_fire_event[16];
static s32 damage_fire_explosion_event[103];
static s32 friend_event[99];
static s32 attack_event[51];
static s32 normal_attack_event[412];
static s32 fire_attack_event[365];
static s32 wait_event[6];

// clang-format off
extend_data(unit_phantom_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_phantom_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x0000007C, (u32)"btl_un_phantom", 0x000A0000, 0x01011E00,
    0x02460232, 0x270F001A, 0x00240008, 0x00180000,
    0x00000000, 0x41900000, 0x00000000, 0x0000FFFD,
    0x00000000, 0x00000000, 0x00000000, 0x41500000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41500000,
    0x00000000, 0x00000000, 0x41500000, 0x41BB3333,
    0x00000000, 0x00000000, 0x41900000, 0x00000000,
    0x41D00000, 0x42100000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_BUBBLE_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000004,
    (u32)&regist, 0x02000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00030101, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x28506464, 0x41640028, 0x64506450, 0x645A4128,
    0x50644164, 0x645A0000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"N_1", 0x00000002, (u32)"Y_1",
    0x00000009, (u32)"Y_1", 0x00000005, (u32)"K_1",
    0x00000004, (u32)"X_1", 0x00000003, (u32)"X_1",
    0x0000001C, (u32)"S_1", 0x0000001D, (u32)"Q_1",
    0x0000001E, (u32)"Q_1", 0x0000001F, (u32)"A_2B",
    0x00000027, (u32)"D_1", 0x00000032, (u32)"A_1",
    0x0000002A, (u32)"R_1", 0x00000028, (u32)"W_1",
    0x00000038, (u32)"X_1", 0x00000039, (u32)"X_1",
    0x00000041, (u32)"T_1", 0x00000045, (u32)"S_1",
};

// TODO: type
static u32 data_table[8] = {
    0x0000000B, (u32)&damage_fire_explosion_event, 0x00000028, (u32)&avoid_fire_event,
    0x00000030, (u32)btldefaultevt_Dummy, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[38] = {
    0x00000001, (u32)"btl_un_phantom", (u32)"c_phantom", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x420C0000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000109, 0x00000010, (u32)&pose_table, 0x00000002,
    0x00000000, (u32)"c_phantom", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x000F000F,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_bubble_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x01000302,
    0x00000000, 0x00000087, 0x00000070, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_bubble_fire_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20000000, 0x01000302,
    0x00000000, 0x000000C7, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x0000004B,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 entry_bubble[12] = {
    (u32)&UNIT_DATA_SYM, 0x01000000, 0x04000004, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPos, -2, LW(0), 10, LW(2))
    CALL(btlevtcmd_SetHomePos, -2, LW(0), 10, LW(2))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(avoid_fire_event)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_FIRE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(damage_fire_explosion_event)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
        IF_INT_LESS_OR_EQUAL(LW(0), 170)
            ADD(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(friend_event)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    LOOP(0)
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
        IF_INT_GREATER_OR_EQUAL(LW(0), 10)
            SUBTRACT(LW(2), 10)
            CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
            IF_INT_EQUAL(LW(3), -1)
                CALL_CHILD(friend_event)
                WAIT_MS(500)
                GOTO(99)
            END_IF
        ELSE
            BREAK_LOOP
        END_IF
    END_LOOP
    LABEL(99)
    RETURN
EVT_END()

static USER_FUNC(bubble_copy_status) {
    s32* args;
    s32 srcId;
    s32 dstId;
    BattleWork* wp;
    BattleUnit* src;
    BattleUnit* dst;

    args = event->args;
    srcId = BattleTransID(event, evtGetValue(event, args[0]));
    dstId = BattleTransID(event, evtGetValue(event, args[1]));
    wp = _battleWorkPointer;
    src = BattleGetUnitPtr(wp, srcId);
    dst = BattleGetUnitPtr(wp, dstId);
    dst->currentHp = src->currentHp;
    if (dst->currentHp > dst->maxHp) {
        dst->currentHp = dst->maxHp;
    }
    dst->mSizeChangeTurns = src->mSizeChangeTurns;
    dst->mSizeChangeStrength = src->mSizeChangeStrength;
    dst->hpGauge.field_0xC = dst->currentHp;
    dst->hpGauge.field_0x10 = dst->currentHp;
    dst->hpGauge.field_0x14 = dst->hpGauge.field_0x18 = (f32)dst->currentHp / (f32)dst->maxHp;
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(friend_event)
    CALL(btlevtcmd_AnimeChangePose, LW(10), LW(11), STRING("S_1"))
    CALL(btlevtcmd_GetPos, -2, LW(12), LW(13), LW(14))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_bubble), 0)
    CALL(bubble_copy_status, -2, LW(3))
    CALL(btlevtcmd_SetPos, LW(3), LW(12), LW(13), LW(14))
    CALL(btlevtcmd_JumpSetting, LW(3), 0, 10, FLOAT(0.5f))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_SPLIT1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 10, 0)
        SET_VALUE(LW(12), 0)
        LOOP(0)
            WAIT_FRAMES(1)
            ADD(LW(12), 1)
            IF_INT_GREATER_OR_EQUAL(LW(12), 30)
                BREAK_LOOP
            END_IF
            CALL(btlevtcmd_AddRotate, LW(3), 0, 0, 40)
        END_LOOP
        CALL(btlevtcmd_SetRotate, LW(3), 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, LW(3), 0, 0, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), LW(1), LW(2), FLOAT(30.0f), -1)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), LW(1), LW(2))
    WAIT_MS(1000)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        GOTO(99)
        RETURN
    END_IF
    SET_VALUE(LW(0), 70)
    ADD(LW(0), 30)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    IF_INT_LESS(LW(1), 70)
        SET_VALUE(LW(9), PTR(weapon_bubble_attack))
        CALL_CHILD(normal_attack_event)
        GOTO(99)
    END_IF
    SET_VALUE(LW(9), PTR(weapon_bubble_fire_attack))
    CALL_CHILD(fire_attack_event)
    GOTO(99)
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(normal_attack_event)
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        WAIT_FRAMES(120)
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 13)
    SUBTRACT(LW(1), 20)
    IF_INT_LESS(LW(1), 0)
        SET_VALUE(LW(1), 0)
    END_IF
    ADD(LW(2), 5)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 0, 0, -1)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 5)
    SUBTRACT(LW(1), 20)
    IF_INT_LESS(LW(1), 0)
        SET_VALUE(LW(1), 0)
    END_IF
    ADD(LW(2), 5)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 4, 0, -1)
    DELETE_EVENT(LW(15))
    WAIT_FRAMES(30)
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
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_FIRE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LOOP(4)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
        WAIT_FRAMES(10)
    END_LOOP
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_FIRE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    WAIT_FRAMES(10)
    LOOP(3)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1B"))
        WAIT_FRAMES(10)
    END_LOOP
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_MOVE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(5)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    ADD(LW(1), 20)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 20, 0, 0, -1)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 100)
    ADD(LW(1), 10)
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, -30, 0, 0, -1)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 40, 25, 4, 0, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    WAIT_MS(400)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static USER_FUNC(eff_fire) {
    s32 id;
    BattleUnit* unit;
    BattleUnitPart* part;
    effFireData* data;
    f32 x;
    f32 y;
    f32 z;

    id = BattleTransID(event, evtGetValue(event, event->args[0]));
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    part = BtlUnit_GetPartsPtr(unit, 2);
    if (isFirstCall) {
        eff[0] = effFireEntry(7, 106, 0.0f, -1000.0f, 0.0f, 1.0f);
        ((effFireData*)eff[0]->userdata)->field_0x78 = 0.0f;
        ((effFireData*)eff[0]->userdata)->field_0x7C = 0.0f;
    }
    data = eff[0]->userdata;
    if (data != NULL) {
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        {
        Vec pos = { 0.0f, 0.0f, 0.0f };
        pos.x = x;
        pos.y = y;
        pos.z = z;
        data->position = pos;
        data->field_0x78 += 0.1f * unit->mSizeMultiplier;
        if (data->field_0x78 >= unit->mSizeMultiplier) {
            data->field_0x78 = unit->mSizeMultiplier;
        }
        data->field_0x7C += 0.1f * unit->mSizeMultiplier;
        if (data->field_0x7C >= unit->mSizeMultiplier) {
            data->field_0x7C = unit->mSizeMultiplier;
        }
        }
    }
    return EVT_RETURN_BLOCK;
}


static EVT_BEGIN(fire_attack_event)
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(evt_btl_camera_set_mode, 0, 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("T_1"))
        WAIT_FRAMES(30)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("S_1"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OffPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
    CALL(btlevtcmd_SetPartsPos, -2, 2, LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(eff_fire, -2)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 0)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    SUBTRACT_FLOAT(LW(10), LW(0))
    SUBTRACT_FLOAT(LW(12), LW(2))
    MULTIPLY_FLOAT(LW(10), FLOAT(0.7f))
    MULTIPLY_FLOAT(LW(12), FLOAT(0.6f))
    ADD_FLOAT(LW(0), LW(10))
    ADD_FLOAT(LW(2), LW(12))
    CALL(btlevtcmd_DivePartsPosition, -2, 2, LW(0), LW(1), LW(2), 40, 0, 4, 0, -1)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(10), LW(11), LW(12))
    SUBTRACT_FLOAT(LW(10), LW(0))
    SUBTRACT_FLOAT(LW(12), LW(2))
    MULTIPLY_FLOAT(LW(10), FLOAT(0.4f))
    MULTIPLY_FLOAT(LW(12), FLOAT(0.4f))
    ADD_FLOAT(LW(0), LW(10))
    ADD_FLOAT(LW(2), LW(12))
    CALL(btlevtcmd_DivePartsPosition, -2, 2, LW(0), LW(1), LW(2), 20, 0, 4, 0, -1)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetPos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_DivePartsPosition, -2, 2, LW(0), LW(1), LW(2), 30, 0, 1, 0, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_ATTACK3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
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
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 16, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(400)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static USER_FUNC(eff_all_fire) {
    s32* args;
    BattleUnit* unit;
    effFireData* data;
    s32 id;
    s32 index;
    f32 x;
    f32 y;
    f32 z;

    args = event->args;
    id = BattleTransID(event, evtGetValue(event, args[0]));
    index = evtGetValue(event, args[1]);
    unit = BattleGetUnitPtr(_battleWorkPointer, id);
    {
    f32 scale[10] = { 1.0f, 1.25f, 1.5f, 1.75f, 2.0f, 2.25f, 2.0f, 1.75f, 1.5f, 1.25f };
    if (isFirstCall) {
        eff2[index] = effFireEntry(7, 120, 0.0f, -1000.0f, 0.0f, 1.0f);
        ((effFireData*)eff2[index]->userdata)->field_0x78 = 2.0f;
        ((effFireData*)eff2[index]->userdata)->field_0x7C = unit->mSizeMultiplier * scale[index];
    }
    data = eff2[index]->userdata;
    if (data != NULL) {
        BtlUnit_GetPos(unit, &x, &y, &z);
        {
        Vec pos = { 0.0f, 0.0f, 0.0f };
        pos.x = x + 24.0f * unit->faceDirection * (index + 1);
        pos.y = y - 15.0f - 2.0f * scale[index];
        pos.z = z - 10.0f;
        data->position = pos;
        }
    }
    }
    return EVT_RETURN_BLOCK;
}


static EVT_BEGIN(all_fire_attackEvent_sub)
    BEGIN_SIBLING_EVENT
        CALL(eff_all_fire, -2, LW(0))
    END_SIBLING_EVENT
    WAIT_FRAMES(120)
    RETURN
EVT_END()

static EVT_BEGIN(all_fire_attack_event)
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, -40, LW(1), LW(2), 30, 0, 4, 0, -1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_ATTACK4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2A"))
    LOOP(20)
        CALL(btlevtcmd_AddPos, -2, 2, 0, 0)
        WAIT_FRAMES(2)
        CALL(btlevtcmd_AddPos, -2, -2, 0, 0)
        WAIT_FRAMES(2)
    END_LOOP
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(10)
            SPAWN_EVENT(all_fire_attackEvent_sub)
            WAIT_FRAMES(2)
            ADD(LW(0), 1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_ATTACK2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_2B"))
    CALL(evt_btl_camera_set_mode, 0, 0)
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
    END_SWITCH
    LABEL(90)
    GOTO(98)
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
    WAIT_FRAMES(180)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("A_1A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_BUBBLE_MOVE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 30, 0, 4, 0, -1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(400)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// clang-format on
