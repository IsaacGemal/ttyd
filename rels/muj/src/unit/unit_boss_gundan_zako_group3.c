#include "unit_boss_gundan_zako_group3.h"
#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_unit.h"
#include "battle/battle_weapon_power.h"
#include "driver/extdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdlib.h>

#pragma pool_data off

#pragma force_active on
static u32 regist[6];
static u32 data_table[6];
static u32 parts[19];
static s32 init_event[21];
static s32 damage_event[129];
static s32 dead_event[136];
static s32 attack_event[287];
static s32 hit_check_event[119];
static s32 wait_event[6];

// clang-format off
extend_data(88,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(89,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boss_gundan_zako_group3[49] = {
    0x00000066, (u32)"btl_un_gundan_zako_group3", 0x000A0000, 0x01010000,
    0x00640032, 0x270F0104, 0x00340000, 0x00340000,
    0x00000000, 0x00000000, 0x00000000, 0x0000FFF1,
    0x00000000, 0x00000000, 0x00000000, 0x43020000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x43160000,
    0x00000000, 0x428C0000, 0x43020000, 0x42073333,
    0x00000000, 0x41200000, 0x41200000, 0x00000000,
    0x43820000, 0x42500000, 0x0000FF00, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_TEAM_3RD_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00200000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[8] = {
    0x0000001C, (u32)"Z_1", 0x0000001F, (u32)"Z_1",
    0x00000027, (u32)"Z_1", 0x00000045, (u32)"Z_1",
};

// TODO: type
static u32 data_table[6] = {
    0x00000031, (u32)&dead_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_gundan_zako_group3", (u32)"c_zako_n", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x03000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
u32 weapon_gundan_zako_group3_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20004004, 0x00000302,
    0x00000000, 0x00000087, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_GetDamage, -2, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        SET_VALUE(LW(15), 1)
    ELSE
        SET_VALUE(LW(15), 0)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 4, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 8, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 15, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 20, LW(14))
    OR(LW(15), LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 0, LW(14))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(zakogun_damage_pose)
    END_IF
    CALL_CHILD(btldefaultevt_Damage)
    CALL(zakogun_damage_pose_cancel)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    CALL(zakogun_dead3)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(48)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        LOOP(20)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(evt_sub_random, 300, LW(3))
            SUBTRACT(LW(3), 150)
            ADD(LW(0), LW(3))
            CALL(evt_sub_random, 30, LW(3))
            ADD(LW(1), LW(3))
            SET_VALUE(LW(2), 75)
            CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetCoin, -2, LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, -2, LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(3), LW(1), LW(2))
    CALL(_add_star_point_disp_offset, -2, LW(3), LW(1), LW(2))
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CheckDamageCode, -2, 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, -2, 0)
    ELSE
        CALL(btlevtcmd_KillUnit, -2, 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    SET_VALUE(LW(9), PTR(weapon_gundan_zako_group3_attack))
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
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(zakogun_get_kanbu_id_evtcmd, LW(5))
    CALL(btlevtcmd_CheckActStatus, LW(5), LW(12))
    IF_INT_NOT_EQUAL(LW(12), 0)
        CALL(zakogun_get_kanbu_id_evtcmd, LW(5))
        BEGIN_SIBLING_EVENT_GET_ID(LW(15))
            WAIT_FRAMES(10)
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("W_1"))
            CALL(btlevtcmd_GetPos, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionSub, LW(5), LW(0), 50)
            CALL(btlevtcmd_MovePosition, LW(5), LW(0), LW(1), LW(2), 60, -1, 0)
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("S_1"))
        END_SIBLING_EVENT
    END_IF
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(32)
        CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_ADHERE1"), 0)
    END_SIBLING_EVENT
    CALL(zakogun_attack3_1)
    IF_INT_NOT_EQUAL(LW(12), 0)
        DELETE_EVENT(LW(15))
        CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("A_1A"))
        CALL(btlevtcmd_snd_se, LW(5), STRING("SFX_BOSS_KANBU_JUMP1"), EVTDAT_POINTER_MAX, 0, LW(13))
        CALL(btlevtcmd_JumpSetting, LW(5), 0, FLOAT(6.0f), FLOAT(0.3f))
        CALL(btlevtcmd_JumpPosition, LW(5), 0, 100, 0, 0, -1)
        CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("S_1"))
        CALL(evt_snd_sfxoff, LW(13))
    END_IF
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    CALL(zakogun_set_target_id, LW(3))
    IF_INT_NOT_EQUAL(LW(12), 0)
        BEGIN_SIBLING_EVENT_GET_ID(LW(14))
            CALL(zakogun_get_kanbu_id_evtcmd, LW(5))
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("R_2"))
            LOOP(0)
                CALL(zakogun_kanbu_on_ball)
                WAIT_FRAMES(1)
            END_LOOP
        END_SIBLING_EVENT
    END_IF
    BEGIN_SIBLING_EVENT
        CALL_CHILD(hit_check_event)
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_ROUND1"), LW(13))
    CALL(zakogun_attack3_2, LW(13))
    IF_INT_NOT_EQUAL(LW(12), 0)
        DELETE_EVENT(LW(14))
        DELETE_EVENT(LW(15))
    END_IF
    IF_INT_NOT_EQUAL(LW(12), 0)
        BEGIN_INLINE_EVENT
            CALL(zakogun_get_kanbu_id_evtcmd, LW(5))
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("A_1A"))
            CALL(btlevtcmd_GetPos, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpSetting, LW(5), 30, 0, FLOAT(0.3f))
            CALL(btlevtcmd_JumpPosition, LW(5), LW(0), 0, LW(2), 30, -1)
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("S_1"))
            CALL(btlevtcmd_ChangeFaceDirection, LW(5), 1)
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("R_1"))
            CALL(btlevtcmd_GetHomePos, LW(5), LW(0), LW(1), LW(2))
            CALL(btlevtcmd_SetMoveSpeed, LW(5), FLOAT(6.0f))
            CALL(btlevtcmd_MovePosition, LW(5), LW(0), LW(1), LW(2), 0, -1, 0)
            CALL(btlevtcmd_AnimeChangePose, LW(5), 1, STRING("S_1"))
            CALL(btlevtcmd_ResetFaceDirection, LW(5))
        END_INLINE_EVENT
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BOSS_TEAM_3RD_ADHERE2"), 0)
    CALL(zakogun_attack3_3)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(hit_check_event)
    LABEL(10)
    LOOP(0)
        CALL(btlevtcmd_GetUnitWork, -2, 0, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
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
    END_SWITCH
    LABEL(90)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    GOTO(98)
    LABEL(98)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    IF_INT_NOT_EQUAL(LW(3), -1)
        CALL(zakogun_set_target_id, LW(3))
        GOTO(10)
    ELSE
        CALL(zakogun_set_target_id, -1)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()









s32 zakogun_kanbu_on_ball(EventEntry* event, BOOL isFirstCall) {
    BattleUnit* unit = BattleGetUnitPtr(_battleWorkPointer, zakogun_get_kanbu_id());

    BtlUnit_SetPos(unit, work_20_bss_6C->pos.x, 45.0f + work_20_bss_6C->pos.y, work_20_bss_6C->pos.z);
    return 2;
}
s32 zakogun_ext_init3(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    ExtPose* pose = extGetPosePtr();
    BattleUnit* unit;
    s32 a;
    s32 b;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 dist;
    Vec tmp;

    work_20_bss_6C->field_0x0 = 2;
    for (i = 0; i < 500; i++, pose++) {
        zakogun_ext_init123_common(pose, i);
        zakogun[i].target.x = (20.0f * (f32)(i % 12)) + -50.0f + ((5.0f * (f32)rand()) / 32767.0f - 2.5f);
        zakogun[i].target.y = 0.0f;
        zakogun[i].target.z = (4.0f * (f32)(i / 12)) + -80.0f + ((5.0f * (f32)rand()) / 32767.0f - 2.5f);
        unit = BattleGetUnitPtr(_battleWorkPointer, zakogun_get_kanbu_id());
        dx = zakogun[i].target.x - unit->mHomePosition.x;
        dy = zakogun[i].target.y - unit->mHomePosition.y;
        dz = zakogun[i].target.z - unit->mHomePosition.z;
        dist = sqrt(dz * dz + (dx * dx + dy * dy));
        if (dist <= 30.0f) {
            zakogun[i].target.x += 30.0f * (dx / dist);
            zakogun[i].target.y += 30.0f * (dy / dist);
            zakogun[i].target.z += 30.0f * (dz / dist);
        }
    }
    for (i = 0; i < 250; i++) {
        a = irand(500);
        b = irand(500);
        tmp = zakogun[a].target;
        zakogun[a].target = zakogun[b].target;
        zakogun[b].target = tmp;
    }
    return 2;
}
void zakogun_ext_main3(void) {
    BattleUnit* unit = zakogun_get_ptr();
    s32 i;
    ExtPose* pose;
    f32 dx;
    f32 dy;

    pose = extGetPosePtr();

    for (i = 0; i < 500; i++, pose++) {
        ZakogunEntry* entry = &zakogun[i];

        pose->field_0x10 = entry->poseStart + entry->poseIndex / entry->poseDiv;
        entry->poseIndex++;
        if (entry->poseIndex >= entry->poseCount * entry->poseDiv) {
            entry->poseIndex = 0;
        }
        switch (entry->state) {
            case 0:
                entry->state = 3;
                entry->timer = 0;
                entry->rotZ = 0.0f;
                entry->rotY = 0.0f;
                entry->rotX = 0.0f;
                zakogun_set_pose(entry, 0, 0x14, 2);
                break;
            case 3:
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->state = 5;
                    entry->timer = 0;
                }
                break;
            case 5:
                entry->field_0x34 = 300.0f;
                entry->field_0x38 = 0.0f;
                entry->field_0x3c = -75.0f + ((150.0f * (f32)rand()) / 32767.0f);
                entry->timerMax = entry->timer = irand(0x1E) + 0x3C;
                entry->state = 0xA;
                zakogun_set_pose(entry, 0x16, 0xA, 1);
                // fallthrough
            case 0xA:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x34, entry->target.x);
                entry->pos.y = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x38, entry->target.y);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x3c, entry->target.z);
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0xF;
                    zakogun_set_pose(entry, 0, 0x14, 2);
                }
                break;
            case 0xF:
                entry->pos.x = entry->target.x + (unit->mPosition.x - unit->mHomePosition.x);
                entry->pos.y = entry->target.y;
                if (entry->pos.y < unit->mPosition.y) {
                    entry->pos.y = unit->mPosition.y;
                }
                entry->pos.z = entry->target.z;
                break;
            case 0x14:
                if (i >= 0 && i < 0x1E4) {
                    {
                        f32 ytop = work_20_bss_6C->pos.y + work_20_bss_6C->field_0x34;

                        entry->field_0x40 = ((f32)(i % 22) *
                                                 ((2.0f * work_20_bss_6C->field_0x34) / 22.0f)) +
                                            (work_20_bss_6C->pos.x - work_20_bss_6C->field_0x34);
                        entry->field_0x44 = ytop - ((f32)(i / 22) *
                                                    ((2.0f * work_20_bss_6C->field_0x34) / 22.0f));
                    }
                    entry->field_0x48 = (work_20_bss_6C->pos.z + ((f32)rand() / 32767.0f)) - 0.5f;
                    dx = work_20_bss_6C->pos.x - entry->field_0x40;
                    dy = work_20_bss_6C->pos.y - entry->field_0x44;
                    if ((f32)sqrt(dx * dx + dy * dy) > work_20_bss_6C->field_0x34) {
                        entry->field_0x40 = work_20_bss_6C->pos.x;
                        entry->field_0x44 = work_20_bss_6C->pos.y;
                        entry->field_0x48 = work_20_bss_6C->pos.z - 10.0f;
                    }
                } else {
                    entry->field_0x40 = work_20_bss_6C->pos.x;
                    entry->field_0x44 = work_20_bss_6C->pos.y;
                    entry->field_0x48 = work_20_bss_6C->pos.z - 10.0f;
                }
                entry->field_0x34 = entry->pos.x;
                entry->field_0x38 = entry->pos.y;
                entry->field_0x3c = entry->pos.z;
                entry->field_0x50 = ((2.0f * (entry->field_0x44 - entry->field_0x38)) - -200.0f) / 40.0f;
                entry->state = 0x19;
                entry->timer = (s32)entry->field_0x44;
                // fallthrough
            case 0x19:
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->state = 0x1E;
                    entry->timer = 0x1E;
                    entry->timerMax = 0x1E;
                    zakogun_set_pose(entry, 0x22, 1, 1);
                }
                break;
            case 0x1E:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x34, entry->field_0x40);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x3c, entry->field_0x48);
                entry->pos.y += entry->field_0x50;
                entry->field_0x50 += -0.5f;
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x23;
                    entry->pos.y = entry->field_0x44;
                    entry->rotZ = (360.0f * (f32)rand()) / 32767.0f;
                    entry->field_0x58 = entry->pos.x - work_20_bss_6C->pos.x;
                    entry->field_0x5c = entry->pos.y - work_20_bss_6C->pos.y;
                    zakogun_set_pose(entry, 0, 0x14, 2);
                    work_20_bss_6C->field_0x68++;
                }
                break;
            case 0x23:
                if (work_20_bss_6C->field_0x68 == 0x1F4) {
                    entry->state = 0x28;
                }
                break;
            case 0x32:
                entry->field_0x34 = entry->pos.x;
                entry->field_0x38 = entry->pos.y;
                entry->field_0x3c = entry->pos.z;
                entry->field_0x40 = ((200.0f * (f32)rand()) / 32767.0f) - 100.0f;
                entry->field_0x44 = 0.0f;
                entry->field_0x48 = ((150.0f * (f32)rand()) / 32767.0f) - 75.0f;
                entry->field_0x50 = ((2.0f * (entry->field_0x44 - entry->field_0x38)) - -200.0f) / 40.0f;
                entry->rotZ = 0.0f;
                entry->state = 0x37;
                entry->timerMax = 0x14;
                entry->timer = 0x14;
                zakogun_set_pose(entry, 0x22, 1, 1);
                // fallthrough
            case 0x37:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x34, entry->field_0x40);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x3c, entry->field_0x48);
                entry->pos.y += entry->field_0x50;
                entry->field_0x50 += -0.5f;
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0x3C;
                    entry->pos.y = entry->field_0x44;
                    zakogun_set_pose(entry, 0, 0x14, 2);
                }
                break;
            case 0x3C:
                entry->field_0x34 = entry->pos.x;
                entry->field_0x3c = entry->pos.z;
                entry->timerMax = entry->timer = irand(0x1E) + 0x1E;
                entry->state = 0x41;
                zakogun_set_pose(entry, 0x16, 0xA, 1);
                // fallthrough
            case 0x41:
                entry->pos.x = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x34, entry->target.x);
                entry->pos.z = intplGetValue(0, entry->timerMax - entry->timer, entry->timerMax,
                                             entry->field_0x3c, entry->target.z);
                entry->timer--;
                if (entry->timer < 0) {
                    entry->state = 0xF;
                    entry->pos.y = entry->target.y;
                    zakogun_set_pose(entry, 0, 0x14, 2);
                }
                break;
            case 0x50:
                entry->rotY = 0.0f;
                entry->timer = irand(5) + 0x30;
                entry->state = 0x52;
                // fallthrough
            case 0x52:
                if (entry->timer <= 0x30) {
                    entry->rotY -= 15.0f;
                }
                entry->timer--;
                if (entry->timer <= 0) {
                    entry->rotY = 0.0f;
                    entry->state = 0x55;
                }
                break;
            case 0x55:
                entry->rotX += 3.0f;
                if (entry->rotX >= 90.0f) {
                    entry->state = -1;
                    entry->pos = (Vec){0.0f, -1000.0f, 0.0f};
                }
                break;
            case 0x5A:
                entry->rotY = 180.0f;
                entry->pos.x += 5.0f;
                if (entry->pos.x >= 300.0f) {
                    entry->state = -1;
                    entry->rotY = 0.0f;
                }
                break;
        }
        if (entry->state >= 0x23 && entry->state <= 0x28) {
            Mtx m;

            PSMTXIdentity(*pose->matrix);
            PSMTXRotRad(m, 'z', 0.017453292f * entry->rotZ);
            PSMTXConcat(*pose->matrix, m, *pose->matrix);
            (*pose->matrix)[0][3] = entry->field_0x58;
            (*pose->matrix)[1][3] = entry->field_0x5c;
            (*pose->matrix)[2][3] = 0.05f * (f32)i + work_20_bss_6C->pos.z;
            PSMTXRotRad(m, 'z', 0.017453292f * work_20_bss_6C->field_0x38);
            PSMTXConcat(m, *pose->matrix, *pose->matrix);
            (*pose->matrix)[0][3] += work_20_bss_6C->pos.x;
            (*pose->matrix)[1][3] += work_20_bss_6C->pos.y;
            (*pose->matrix)[2][3] += work_20_bss_6C->pos.z;
            entry->pos.x = (*pose->matrix)[0][3];
            entry->pos.y = (*pose->matrix)[1][3];
            entry->pos.z = (*pose->matrix)[2][3];
        } else {
            Mtx m;

            PSMTXIdentity(*pose->matrix);
            PSMTXRotRad(m, 'x', 0.017453292f * entry->rotX);
            PSMTXConcat(*pose->matrix, m, *pose->matrix);
            PSMTXRotRad(m, 'y', 0.017453292f * entry->rotY);
            PSMTXConcat(*pose->matrix, m, *pose->matrix);
            PSMTXRotRad(m, 'z', 0.017453292f * entry->rotZ);
            PSMTXConcat(*pose->matrix, m, *pose->matrix);
            (*pose->matrix)[0][3] = entry->pos.x;
            (*pose->matrix)[1][3] = entry->pos.y;
            (*pose->matrix)[2][3] = 0.03f * (f32)i + entry->pos.z;
        }
    }
}
s32 zakogun_attack3_1(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        Vec pos = {0.0f, 55.0f, 0.0f};

        work_20_bss_6C->pos = pos;
        work_20_bss_6C->field_0x34 = 25.0f;
        work_20_bss_6C->field_0x38 = 0.0f;
        work_20_bss_6C->field_0x68 = 0;
        for (i = 0; i < 500; i++) {
            zakogun[i].state = 0x14;
        }
    }
    i = 0;
    count = 0;
    for (; i < 500; i++) {
        if (zakogun[i].state == 0x28) {
            count++;
        }
    }
    return count == 500 ? 2 : 0;
}
s32 zakogun_attack3_2(EventEntry* event, BOOL isFirstCall) {
    BattleUnit* unit = zakogun_get_ptr();
    s32 val = evtGetValue(event, event->args[0]);

    if (isFirstCall) {
        work_20_bss_6C->field_0x68 = 0;
        work_20_bss_6C->state = 0;
    }
    switch (work_20_bss_6C->state) {
        case 0:
            work_20_bss_6C->state = 5;
            work_20_bss_6C->field_0x3c = 0.0f;
            // fallthrough
        case 5:
            work_20_bss_6C->pos.x += work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x38 -= work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x3c -= 0.06f;
            if (work_20_bss_6C->targetId != -1) {
                BattleUnit* target = BattleGetUnitPtr(_battleWorkPointer, work_20_bss_6C->targetId);

                if (work_20_bss_6C->pos.x <= target->mPosition.x) {
                    unit->work[0] = 1;
                }
            }
            if (work_20_bss_6C->pos.x <= -200.0f) {
                work_20_bss_6C->state = 8;
            }
            break;
        case 8:
            work_20_bss_6C->pos.x += work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x38 -= work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x3c += 5.0f;
            if (work_20_bss_6C->field_0x3c >= 0.0f) {
                work_20_bss_6C->state = 0xA;
            }
            break;
        case 0xA:
            work_20_bss_6C->pos.x += work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x38 -= work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x3c += 0.06f;
            if (work_20_bss_6C->pos.x >= -30.0f) {
                work_20_bss_6C->state = 0xD;
            }
            break;
        case 0xD:
            work_20_bss_6C->pos.x += work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x38 -= work_20_bss_6C->field_0x3c;
            work_20_bss_6C->field_0x3c -= 1.0f;
            if (work_20_bss_6C->field_0x3c <= 0.0f) {
                work_20_bss_6C->state = 0xF;
                psndSFXOff(val);
                work_20_bss_6C->timer = 0x14;
            }
            break;
        case 0xF:
            work_20_bss_6C->timer--;
            if (work_20_bss_6C->timer < 0) {
                return 2;
            }
            break;
    }
    return 0;
}
s32 zakogun_attack3_3(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 500; i++) {
            zakogun[i].state = 0x32;
        }
    }
    i = 0;
    count = 0;
    for (; i < 500; i++) {
        if (zakogun[i].state == 0xF) {
            count++;
        }
    }
    return count == 500 ? 2 : 0;
}
s32 zakogun_dead3(EventEntry* event, BOOL isFirstCall) {
    s32 i;

    for (i = 0; i < 500; i++) {
        zakogun[i].state = 0x50;
    }
    return 2;
}
s32 zakogun_escape3(EventEntry* event, BOOL isFirstCall) {
    s32 i;
    s32 count;

    if (isFirstCall) {
        for (i = 0; i < 500; i++) {
            ZakogunEntry* entry = &zakogun[i];

            entry->state = 0x5A;
            zakogun_set_pose(entry, 0x16, 0xA, 1);
        }
    }
    i = 0;
    count = 0;
    for (; i < 500; i++) {
        if (zakogun[i].state == -1) {
            count++;
        }
    }
    return count == 500 ? 2 : 0;
}

// clang-format on
