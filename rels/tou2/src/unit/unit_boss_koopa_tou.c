#include "battle/battle.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_message.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_msg.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "party/party_motion.h"
#include "system.h"
#include "unit_boss_koopa_tou.h"

#pragma pool_data off

#pragma force_active on
static void _disp_icon(CameraId camera, void* param);
static USER_FUNC(_icon_jump_icon);
static USER_FUNC(party_cannot_use_command);
static USER_FUNC(mario_cannot_use_command);
static u32 cmd_disable_string[5];
static u32 regist[6];
static u32 data_table[4];
static u32 parts[19];
static s32 init_event[72];
static s32 entry_event[45];
static s32 damage_event[10];
static s32 dead_event[111];
static s32 phase_event[31];
static s32 harf_hp_event[62];
static s32 attack_event[108];
static s32 hip_attack_event[419];
static s32 bite_attack_event[267];
static s32 critical_bite_attack_event[250];
static s32 fire_attack_event[345];
static s32 wait_event[6];

// clang-format off
extend_data(152,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(153,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_boss_koopa_tou[49] = {
    0x0000003F, (u32)"btl_un_koopa", 0x001E0000, 0x01012700,
    0x00640032, 0x00090056, 0x00500000, 0x00460000,
    0x00000000, 0x42200000, 0x00000000, 0x00000000,
    0x42200000, 0x42200000, 0x00000000, 0x422C0000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41A80000,
    0x00000000, 0x00000000, 0x42100000, 0x42900000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x42AC0000, 0x42A00000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_BOSS_KOOPA_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x01000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x01010101, 0x01000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x463C3C64, 0x46646428, 0x644B644B, 0x645A3C00,
    0x00643C64, 0x64000000,
};

// TODO: type
static u32 pose_table[36] = {
    0x00000001, (u32)"KPA_N_3", 0x00000002, (u32)"KPA_Z_6",
    0x00000009, (u32)"KPA_Z_6", 0x00000005, (u32)"KPA_K_2",
    0x00000004, (u32)"KPA_S_2", 0x00000003, (u32)"KPA_S_2",
    0x0000001C, (u32)"KPA_S_1", 0x0000001D, (u32)"KPA_Q_2",
    0x0000001E, (u32)"KPA_Q_2", 0x0000001F, (u32)"KPA_D_1",
    0x00000027, (u32)"KPA_D_1", 0x00000032, (u32)"KPA_S_1",
    0x0000002A, (u32)"KPA_R_1", 0x00000028, (u32)"KPA_W_1",
    0x00000038, (u32)"KPA_S_1", 0x00000039, (u32)"KPA_S_1",
    0x00000041, (u32)"KPA_T_1", 0x00000045, (u32)"KPA_S_1",
};

// TODO: type
static u32 pose_table_forward_leaning[36] = {
    0x00000001, (u32)"KPA_N_2", 0x00000002, (u32)"KPA_Z_5",
    0x00000009, (u32)"KPA_Z_5", 0x00000005, (u32)"KPA_K_1",
    0x00000004, (u32)"KPA_X_1", 0x00000003, (u32)"KPA_X_1",
    0x0000001C, (u32)"KPA_S_8", 0x0000001D, (u32)"KPA_D_4",
    0x0000001E, (u32)"KPA_D_4", 0x0000001F, (u32)"KPA_D_4",
    0x00000027, (u32)"KPA_D_4", 0x00000032, (u32)"KPA_S_8",
    0x0000002A, (u32)"KPA_R_1", 0x00000028, (u32)"KPA_W_1",
    0x00000038, (u32)"KPA_S_8", 0x00000039, (u32)"KPA_S_8",
    0x00000041, (u32)"KPA_T_9", 0x00000045, (u32)"KPA_S_8",
};

// TODO: type
static u32 data_table[4] = {
    0x00000031, (u32)&dead_event, 0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[19] = {
    0x00000001, (u32)"btl_un_koopa", (u32)"c_koopa", 0x00000000,
    0x00000000, 0x00000000, 0x42080000, 0x42780000,
    0x00000000, 0x41B00000, 0x429C0000, 0x00000000,
    0x00460050, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_koopa_hip_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x20001004, 0x000A0302,
    0x00000000, 0x00001087, 0x0000000C, 0x80000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00191964, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_koopa_bite_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00000087, 0x00000002, 0x00000004,
    0x00000000, 0x00001E0A, 0x01000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_koopa_critical_bite_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00000087, 0x00000002, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
static u32 weapon_koopa_fire_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x01000302,
    0x00000000, 0x00000047, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

static EVT_BEGIN(koopa_zoom)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_btl_camera_set_moveto, 0, LW(0), 95, 350, LW(0), 45, -300, 30, 11)
    WAIT_FRAMES(37)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventPhase, -2, PTR(phase_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetEventEntry, -2, PTR(entry_event))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_GetMaxHp, -2, LW(0))
    DIVIDE(LW(0), 2)
    CALL(btlevtcmd_SetUnitWork, -2, 2, LW(0))
    CALL(btlevtcmd_SetUnitWork, -2, 3, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_BOSS_KOOPA_MOVE1L"), STRING("SFX_BOSS_KOOPA_MOVE1R"), 0, 15, 15)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_BOSS_KOOPA_MOVE1L"), STRING("SFX_BOSS_KOOPA_MOVE1R"), 0, 8, 8)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(entry_event)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    WAIT_FRAMES(60)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    WAIT_FRAMES(60)
    CALL_CHILD(koopa_zoom)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_LAUGH2_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_Y_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 2, STRING("stg3_tou_403_06_01"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
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
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        CALL(btlevtcmd_ResetFaceDirection, -2)
    END_IF
    CALL(btlevtcmd_WaitAttackEnd)
    CALL(btlevtcmd_StatusWindowOnOff, 0)
    CALL_CHILD(koopa_zoom)
    CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_D_1"))
    CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_D_1"))
    CALL(evt_msg_print, 2, STRING("stg3_tou_403_06_03"), 0, -2)
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_SATIATED2_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_D_2"))
    WAIT_MS(700)
    SET_VALUE(LW(10), -2)
    CALL_CHILD(subsetevt_dead_core_nospin_norotate)
    CALL(btlevtcmd_StatusWindowOnOff, 1)
    CALL(evt_btl_camera_set_mode, 0, 0)
    RETURN
EVT_END()

static EVT_BEGIN(phase_event)
    CALL(btlevtcmd_CheckStatus, -2, 27, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CheckPhase, LW(0), 67108867)
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_PhaseEventStartDeclare, -2)
        CALL_CHILD(harf_hp_event)
        CALL(btlevtcmd_StartWaitEvent, -2)
    END_IF
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(harf_hp_event)
    CALL(btlevtcmd_GetUnitWork, -2, 3, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_GetHp, -2, LW(0))
        CALL(btlevtcmd_GetUnitWork, -2, 2, LW(1))
        IF_INT_LESS(LW(0), LW(1))
            CALL(btlevtcmd_SetUnitWork, -2, 3, 1)
            CALL(btlevtcmd_StatusWindowOnOff, 0)
            CALL_CHILD(koopa_zoom)
            CALL(btlevtcmd_SetTalkPose, -2, STRING("KPA_T_3"))
            CALL(btlevtcmd_SetStayPose, -2, STRING("KPA_S_4"))
            CALL(evt_msg_print, 2, STRING("stg3_tou_403_06_02"), 0, -2)
            WAIT_MS(300)
            CALL(btlevtcmd_StatusWindowOnOff, 1)
            CALL(evt_btl_camera_set_mode, 0, 0)
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        END_IF
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_GetUnitWork, -2, 4, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        SET_VALUE(LW(9), PTR(weapon_koopa_fire_attack))
        CALL_CHILD(fire_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 1)
        CASE_EQUAL(1)
        SET_VALUE(LW(9), PTR(weapon_koopa_bite_attack))
        CALL_CHILD(bite_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 2)
        CASE_EQUAL(2)
        SET_VALUE(LW(9), PTR(weapon_koopa_hip_attack))
        CALL_CHILD(hip_attack_event)
        CALL(btlevtcmd_SetUnitWork, -2, 4, 3)
        CASE_DEFAULT
        CALL(btlevtcmd_DrawLots, LW(0), 4, 30, 30, 0, 30)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            SET_VALUE(LW(9), PTR(weapon_koopa_hip_attack))
            CALL_CHILD(hip_attack_event)
            CASE_EQUAL(1)
            SET_VALUE(LW(9), PTR(weapon_koopa_bite_attack))
            CALL_CHILD(bite_attack_event)
            CASE_EQUAL(2)
            SET_VALUE(LW(9), PTR(weapon_koopa_critical_bite_attack))
            CALL_CHILD(critical_bite_attack_event)
            CASE_EQUAL(3)
            SET_VALUE(LW(9), PTR(weapon_koopa_fire_attack))
            CALL_CHILD(fire_attack_event)
        END_SWITCH
    END_SWITCH
    LABEL(90)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static USER_FUNC(mario_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 rnd = irand(2);
    s32 index = *event->args;
    s32 value = 0;

    switch (rnd) {
    case 0:
        ((s32*)work->field_18FF0)[18] = 3;
        value = 0x146;
        break;
    case 1:
        ((s32*)work->field_18FF0)[19] = 3;
        value = 0x149;
        break;
    case 2:
        ((s32*)work->field_18FF0)[20] = 3;
        value = 0x14C;
        break;
    }
    event->lwData[0] = cmd_disable_string[rnd];
    evtSetValue(event, index, value);
    return 2;
}

static USER_FUNC(party_cannot_use_command) {
    BattleWork* work = _battleWorkPointer;
    s32 rnd = irand(1);
    s32 index = *event->args;
    s32 value = 0;

    switch (rnd) {
    case 0:
        ((s32*)work->field_18FF0)[21] = 3;
        value = 0x150;
        break;
    case 1:
        ((s32*)work->field_18FF0)[22] = 3;
        value = 0x14C;
        break;
    }
    event->lwData[0] = cmd_disable_string[rnd + 3];
    evtSetValue(event, index, value);
    return 2;
}

static USER_FUNC(_icon_jump_icon) {
    s32* args = event->args;
    BossKoopaIconData* data;
    s32 iconId;
    f32 x;
    f32 y;
    f32 z;
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    f32 gravity;
    s32 count;

    iconId = evtGetValue(event, args[0]);
    x = evtGetFloat(event, args[1]);
    y = evtGetFloat(event, args[2]);
    z = evtGetFloat(event, args[3]);
    targetX = evtGetFloat(event, args[4]);
    targetY = evtGetFloat(event, args[5]);
    targetZ = evtGetFloat(event, args[6]);
    gravity = evtGetFloat(event, args[7]);
    count = evtGetValue(event, args[8]);

    if (isFirstCall) {
        Vec pos;
        event->userdata[0] = (s32)BattleAlloc(0x7C);
        data = (BossKoopaIconData*)event->userdata[0];
        data->targetPos.x = targetX;
        data->targetPos.y = targetY;
        data->targetPos.z = targetZ;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        data->curPos = pos;
        data->position = pos;
        data->gravity = gravity;
        data->count = count;
        data->velocity = (data->targetPos.y - data->position.y) / (f32)data->count + data->gravity * (f32)data->count / 2.0f;
        data->speed = distABf(data->position.x, data->position.z, data->targetPos.x, data->targetPos.z) / (f32)data->count;
        event->userdata[1] = iconId;
        event->userdata[2] = 0;
    }

    data = (BossKoopaIconData*)event->userdata[0];
    if (event->userdata[2] != 0) {
        BattleFree(data);
        return 2;
    }

    data->curPos.y += data->velocity;
    data->velocity -= data->gravity;
    if (data->velocity < 0.0f && data->curPos.y < data->targetPos.y) {
        data->curPos.y = data->targetPos.y;
    }
    btlMovePos(&data->curPos.x, &data->curPos.z, data->speed, angleABf(data->position.x, data->position.z, data->targetPos.x, data->targetPos.z));
    data->count--;
    if (data->count <= 0) {
        event->userdata[2] = 1;
        data->curPos = data->targetPos;
    }
    dispEntry(4, 900.0f, 1, _disp_icon, event);
    return 0;
}

static EVT_BEGIN(hip_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(2.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2A"))
    WAIT_MS(300)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2B"))
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(25)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2C"))
        WAIT_FRAMES(15)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2D"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.25f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(2), 20)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 50, -1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_HIP_ATTACK1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    CALL(btlevtcmd_JumpContinue, -2)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2E"))
    WAIT_MS(500)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(evt_btl_camera_shake_h, 0, 3, 0, 50, 13)
    CALL(btlevtcmd_JumpContinue, -2)
    WAIT_MS(500)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_2E"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetResultACDefence, LW(7))
    IF_INT_LESS_OR_EQUAL(LW(7), 3)
        SET_VALUE(LW(15), 0)
        CALL(btlevtcmd_GetUnitKind, LW(3), LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(222)
            CALL(mario_cannot_use_command, LW(15))
            CASE_BETWEEN(224, 230)
            CALL(party_cannot_use_command, LW(15))
            CASE_DEFAULT
            SET_VALUE(LW(0), -1)
        END_SWITCH
        IF_INT_NOT_EQUAL(LW(0), -1)
            IF_INT_NOT_EQUAL(LW(15), 0)
                BEGIN_INLINE_EVENT
                    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_GetPos, -2, LW(5), LW(6), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 60)
                    CALL(btlevtcmd_TransStageFloorPosition, LW(5), LW(6), LW(7))
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 30)
                    SET_VALUE(LW(0), LW(5))
                    SET_VALUE(LW(1), LW(6))
                    SET_VALUE(LW(2), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 40)
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 20)
                    SET_VALUE(LW(0), LW(5))
                    SET_VALUE(LW(1), LW(6))
                    SET_VALUE(LW(2), LW(7))
                    CALL(btlevtcmd_FaceDirectionSub, LW(3), LW(5), 25)
                    CALL(_icon_jump_icon, LW(15), LW(0), LW(1), LW(2), LW(5), LW(6), LW(7), FLOAT(0.5f), 12)
                END_INLINE_EVENT
            END_IF
            BEGIN_SIBLING_EVENT
                CALL(btlevtcmd_AnnounceMessage, 0, 0, 0, LW(0), 120)
            END_SIBLING_EVENT
        END_IF
    END_IF
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(bite_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 65)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3B"))
    WAIT_MS(150)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    WAIT_MS(100)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_S_1"))
    RETURN
EVT_END()

static EVT_BEGIN(critical_bite_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 65)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3A"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3B"))
    WAIT_MS(150)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_BITE3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
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
    WAIT_MS(100)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    GOTO(98)
    LABEL(91)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_4"))
    WAIT_MS(1500)
    CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_3C"))
    WAIT_MS(150)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    RETURN
EVT_END()

static EVT_BEGIN(fire_attack_event)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(9))
    CALL(btlevtcmd_ChoiceSamplingEnemy, LW(9), LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
        END_IF
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(9))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(9))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetPos, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), 120)
    CALL(evt_btl_camera_set_mode, 0, 8)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, LW(3))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 200)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), 0, LW(2), 0, -1, 1)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    WAIT_MS(170)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1A"))
    WAIT_MS(300)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_VOICE_KOOPA_FIRE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_BOSS_KOOPA_FIRE_BREATH1"), EVTDAT_POINTER_MAX, 0, LW(15))
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_snd_sfxoff, LW(15))
    END_INLINE_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1B"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    SET_FLOAT_VALUE(LW(6), FLOAT(60.0f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(6))
    SET_FLOAT_VALUE(LW(6), FLOAT(22.5f))
    MULTIPLY_FLOAT(LW(6), LW(5))
    ADD_FLOAT(LW(1), LW(6))
    SUBTRACT(LW(2), 5)
    MULTIPLY_FLOAT(LW(5), FLOAT(3.5f))
    CALL(evt_eff, STRING(""), STRING("gonbaba_breath"), 7, LW(0), LW(1), LW(2), LW(5), 90, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1C"))
    WAIT_MS(300)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    SET_VALUE(LW(10), 0)
    LABEL(10)
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
    GOTO(97)
    LABEL(91)
    IF_INT_EQUAL(LW(10), 0)
        CALL(btlevtcmd_ResultACDefence, LW(3), LW(9))
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(9), 256, LW(5))
    LABEL(97)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    IF_INT_NOT_EQUAL(LW(3), -1)
        ADD(LW(10), 1)
        GOTO(10)
    END_IF
    LABEL(98)
    WAIT_MS(1200)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_A_1D"))
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KPA_R_1"))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
static u32 cmd_disable_string[5] = {
    (u32)"btl_jump_cmd_disable", (u32)"btl_hammer_cmd_disable", (u32)"btl_item_cmd_disable", (u32)"btl_party_tech_cmd_disable",
    (u32)"btl_party_item_cmd_disable",
};

static void _disp_icon(CameraId camera, void* param) {
    EventEntry* event = param;
    Vec* position = (Vec*)(event->userdata[0] + 0xC);

#ifdef MUST_MATCH
    // The retail prototype takes a u16 iconId; the decomp header says s16, which makes MWCC emit
    // an extra extsh. Call through a matching function-pointer type to stay byte-exact.
    ((void (*)(Vec*, s16, u16, f32))iconDispGx)(position, 0, (u16)event->userdata[1], 1.0f);
#else
    iconDispGx(position, 0, (u16)event->userdata[1], 1.0f);
#endif
}
// clang-format on
