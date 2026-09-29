#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "manager/evtmgr_cmd.h"
#include "math_ext.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_event_subset.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
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
static USER_FUNC(chain_main);
static USER_FUNC(chain_init);
static u32 regist[6];
static u32 data_table[30];
static u32 parts[171];
static s32 init_event[41];
static s32 chain_event[23];
static s32 damage_event[33];
static s32 counter_damage_event[68];
static s32 dead_event[200];
static s32 attack_event[452];
static s32 wait_event[476];
static s32 sound_gensui_event[22];
static s32 wait_sound_event1[25];
static s32 wait_sound_event2[12];

// clang-format off
extend_data(unit_burst_wanwan_0,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(unit_burst_wanwan_1,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 UNIT_DATA_SYM[49] = {
    0x0000003B, (u32)"btl_un_burst_wanwan", 0x00060000, 0x01011800,
    0x01460332, 0x270F0022, 0x00240006, 0x002C0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41880000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41880000,
    0x00000000, 0x00000000, 0x41880000, 0x41BB3333,
    0x00000000, 0x00000000, 0x41900000, 0x00000000,
    0x42080000, 0x42100000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_WAN_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x09000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
};

// TODO: type
static u32 defence[2] = {
    0x03636303, 0x03000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00020200, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x00464164, 0x41640046, 0x645A645A, 0x645F412D,
    0x1E644164, 0x645F0000,
};

// TODO: type
static u32 pose_table[38] = {
    0x00000001, (u32)"WAN_N_1", 0x00000002, (u32)"WAN_Y_1",
    0x00000009, (u32)"WAN_Y_1", 0x00000005, (u32)"WAN_K_1",
    0x00000004, (u32)"WAN_X_1", 0x00000003, (u32)"WAN_X_1",
    0x0000001B, (u32)"WAN_Z_1", 0x0000001C, (u32)"WAN_S_1A",
    0x0000001D, (u32)"WAN_D_1", 0x0000001E, (u32)"WAN_D_1",
    0x0000001F, (u32)"WAN_A_1", 0x00000027, (u32)"WAN_D_1",
    0x00000032, (u32)"WAN_A_1", 0x0000002A, (u32)"WAN_R_1",
    0x00000028, (u32)"WAN_W_1", 0x00000038, (u32)"WAN_X_1",
    0x00000039, (u32)"WAN_X_1", 0x00000041, (u32)"WAN_S_1A",
    0x00000045, (u32)"WAN_S_1A",
};

// TODO: type
static u32 data_table[30] = {
    0x00000031, (u32)&dead_event, 0x0000001A, (u32)&counter_damage_event,
    0x00000019, (u32)&counter_damage_event, 0x0000001B, (u32)&counter_damage_event,
    0x0000001C, (u32)&counter_damage_event, 0x0000001D, (u32)&counter_damage_event,
    0x0000001E, (u32)&counter_damage_event, 0x0000001F, (u32)&counter_damage_event,
    0x00000020, (u32)&counter_damage_event, 0x00000021, (u32)&counter_damage_event,
    0x00000022, (u32)&counter_damage_event, 0x00000023, (u32)&counter_damage_event,
    0x00000024, (u32)&counter_damage_event, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000000, 0x00000000,
};

// TODO: type
static u32 parts[171] = {
    0x00000001, (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a", 0x00000000,
    0x00000000, 0x00000000, 0x40A00000, 0x41F00000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x10000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x12010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_burst_wanwan",
    (u32)"c_wanwan_a", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x12010000, 0x00000000,
    (u32)&pose_table, 0x00000004, (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x12010000, 0x00000000, (u32)&pose_table,
    0x00000005, (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x12010000, 0x00000000, (u32)&pose_table, 0x00000006,
    (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a", 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000,
    0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x12010000,
    0x00000000, (u32)&pose_table, 0x00000007, (u32)"btl_un_burst_wanwan",
    (u32)"c_wanwan_a", 0x00000000, 0x00000000, 0x00000000,
    0x41700000, 0x41C80000, 0x00000000, 0x00000000,
    0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x12010000, 0x00000000,
    (u32)&pose_table, 0x00000008, (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a",
    0x00000000, 0x00000000, 0x00000000, 0x41700000,
    0x41C80000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x0014001E, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x12010000, 0x00000000, (u32)&pose_table,
    0x00000009, (u32)"btl_un_burst_wanwan", (u32)"c_wanwan_a", 0x00000000,
    0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000,
    0x0014001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x12010000, 0x00000000, (u32)&pose_table,
};

// TODO: type
static u32 weapon_wanwan_attack[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000001, 0x3F800000, 0x01010101, (u32)weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    0x00000000, 0x00000087, 0x00000072, 0x00002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// NONMATCHING (99.79%): only the temporaries r4/r5 of the first two loads (args, _battleWorkPointer@ha) are swapped.
static USER_FUNC(chain_init) {
    s32* args;
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    s32 id;
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    for (i = 2; i <= 7; i++) {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_SetPartsPos(part, x, y, z);
        BtlUnit_SetAnim(part, "WAN_S_1B");
    }
    part = BtlUnit_GetPartsPtr(unit, 8);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    BtlUnit_SetAnim(part, "WAN_S_1B");
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    BtlUnit_SetAnim(part, "WAN_Z_2");
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(chain_init, -2)
    SPAWN_EVENT(chain_event)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("0"), STRING("SFX_ENM_WAN_LANDING1"))
    CALL(btlevtcmd_SetUnitWork, -2, 1, 127)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

// NONMATCHING (99.71%): same r4/r5 swap at the start, and i/part/next take r30/r29 instead of r29/r30.
static USER_FUNC(chain_main) {
    s32* args;
    BattleWork* wp;
    BattleUnit* unit;
    BattleUnitPart* part;
    BattleUnitPart* next;
    s32 id;
    s32 i;
    f32 x;
    f32 y;
    f32 z;
    f32 ax;
    f32 ay;
    f32 az;
    f32 bx;
    f32 by;
    f32 bz;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 len;
    f32 scale;
    f32 pull;

    args = event->args;
    wp = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    unit = BattleGetUnitPtr(wp, id);
    if (unit == NULL) {
        return EVT_RETURN_BLOCK;
    }
    BtlUnit_GetPartsPtr(unit, 1);
    unit = BattleGetUnitPtr(wp, id);
    BtlUnit_GetPos(unit, &x, &y, &z);
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_SetPartsPos(part, x - 20.0f * unit->faceDirection, y, z);
    if (unit->work[2] == 2) {
        part = BtlUnit_GetPartsPtr(unit, 9);
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        for (i = 8; i >= 1; i--) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_GetPartsPos(part, &bx, &by, &bz);
            part->work[0] = (s32)(bx - x);
            part->work[1] = (s32)(by - y);
        }
        unit->work[2] = 1;
    }
    if (unit->work[2] == 1) {
        part = BtlUnit_GetPartsPtr(unit, 9);
        BtlUnit_GetPartsPos(part, &x, &y, &z);
        for (i = 8; i >= 1; i--) {
            part = BtlUnit_GetPartsPtr(unit, i);
            BtlUnit_SetPartsPos(part, x + part->work[0], y + part->work[1], z);
        }
        return EVT_RETURN_DONE;
    }
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &ax, &ay, &az);
    part = BtlUnit_GetPartsPtr(unit, 9);
    BtlUnit_GetPartsPos(part, &bx, &by, &bz);
    dx = ax - bx;
    dy = ay - by;
    dz = az - bz;
    scale = (f32)sqrt(dx * dx + dy * dy + dz * dz) * 0.125f;
    if (scale < 10.0f) {
        scale = 10.0f;
    }
    pull = 300.0f / (scale * scale);
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &x, &y, &z);
    if (y > 110.0f) {
        y = 110.0f;
    }
    BtlUnit_SetPartsPos(part, x, y, z);
    part = BtlUnit_GetPartsPtr(unit, 1);
    BtlUnit_GetPartsPos(part, &x, &y, &z);
    x += (f32)(-(unit->faceDirection << 4));
    y += 16.0f;
    part = BtlUnit_GetPartsPtr(unit, 2);
    BtlUnit_SetPartsPos(part, x, y, z);
    i = 2;
    do {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_GetPartsPos(part, &ax, &ay, &az);
        next = BtlUnit_GetPartsPtr(unit, i + 1);
        BtlUnit_GetPartsPos(next, &bx, &by, &bz);
        by -= pull;
        dx = ax - bx;
        dy = ay - by;
        dz = az - bz;
        len = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        len = 1.0f - scale / len;
        bx += dx * len;
        by += dy * len;
        bz += dz * len;
        if (by < 0.0f) {
            by = 0.0f;
        }
        BtlUnit_SetPartsPos(next, bx, by, bz);
        i++;
    } while (i <= 7);
    i = 9;
    do {
        part = BtlUnit_GetPartsPtr(unit, i);
        BtlUnit_GetPartsPos(part, &ax, &ay, &az);
        next = BtlUnit_GetPartsPtr(unit, i - 1);
        BtlUnit_GetPartsPos(next, &bx, &by, &bz);
        by -= pull;
        dx = ax - bx;
        dy = ay - by;
        dz = az - bz;
        len = (f32)sqrt(dx * dx + dy * dy + dz * dz);
        len = 1.0f - scale / len;
        bx += dx * len;
        by += dy * len;
        bz += dz * len;
        if (by < 0.0f) {
            by = 0.0f;
        }
        BtlUnit_SetPartsPos(next, bx, by, bz);
        i--;
    } while (i >= 3);
    return EVT_RETURN_DONE;
}


static EVT_BEGIN(chain_event)
    LABEL(0)
    WAIT_FRAMES(1)
    CALL(btlevtcmd_GetUnitId, -2, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        GOTO(99)
    END_IF
    CALL(chain_main, -2)
    GOTO(0)
    LABEL(99)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 2)
    CALL(btlevtcmd_OnAttribute, LW(10), 4)
    CALL_CHILD(btldefaultevt_Damage)
    CALL(btlevtcmd_OffAttribute, LW(10), 4)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    RETURN
EVT_END()

static EVT_BEGIN(counter_damage_event)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 39)
    CALL(btlevtcmd_SetPartsFallAccel, LW(10), 1, FLOAT(1.0f))
    CALL(btlevtcmd_GetPartsPos, LW(10), 1, LW(2), LW(3), LW(4))
    CALL(btlevtcmd_PartsFaceDirectionSub, LW(10), 1, LW(2), 25)
    ADD(LW(3), 25)
    CALL(btlevtcmd_DivePartsPosition, LW(10), 1, LW(2), LW(3), LW(4), 5, 0, 0, 0, -1)
    WAIT_FRAMES(30)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(btlevtcmd_CheckDamageCode, -2, 512, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_CheckDamageCode, -2, 2048, LW(0))
        IF_INT_EQUAL(LW(0), 0)
            RETURN
        END_IF
    END_IF
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(dead_event)
    SET_VALUE(LW(0), 48)
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    LABEL(10)
    WAIT_FRAMES(1)
    SET_VALUE(LW(11), 1)
    LOOP(9)
        CALL(btlevtcmd_AddPartsRotate, LW(10), LW(11), 0, -8, 0)
        ADD(LW(11), 1)
    END_LOOP
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(10)
    END_IF
    SET_VALUE(LW(11), 1)
    LOOP(9)
        CALL(btlevtcmd_SetPartsRotate, LW(10), LW(11), 0, 0, 0)
        ADD(LW(11), 1)
    END_LOOP
    CALL(btlevtcmd_snd_se, LW(10), STRING("SFX_BTL_ENEMY_DIE1_2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPos, LW(10), LW(0), LW(1), LW(2))
    ADD(LW(2), 10)
    CALL(evt_eff, 0, STRING("kemuri_test"), 4, LW(0), LW(1), LW(2), FLOAT(0.8f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetCoin, LW(10), LW(0))
    CALL(btlevtcmd_StoreCoin, LW(0))
    CALL(btlevtcmd_GetExp, LW(10), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_GetPos, LW(10), LW(3), LW(1), LW(2))
    ADD(LW(2), 10)
    ADD(LW(1), 30)
    CALL(evt_eff, STRING(""), STRING("star_point"), 0, LW(3), LW(1), LW(2), LW(0), 0, 0, 0, 0, 0, 0, 0)
    SET_VALUE(LW(11), 1)
    LOOP(9)
        CALL(btlevtcmd_SetPartsRotateOffset, LW(10), LW(11), 0, 0, 0)
        ADD(LW(11), 1)
    END_LOOP
    SET_VALUE(LW(0), 30)
    LABEL(20)
    WAIT_FRAMES(1)
    SET_VALUE(LW(11), 1)
    LOOP(9)
        CALL(btlevtcmd_AddPartsRotate, LW(10), LW(11), 3, 0, 0)
        ADD(LW(11), 1)
    END_LOOP
    SUBTRACT(LW(0), 1)
    IF_INT_GREATER(LW(0), 0)
        GOTO(20)
    END_IF
    CALL(btlevtcmd_GetBackItem, LW(10))
    CALL(btlevtcmd_CheckDamageCode, LW(10), 1024, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_KillUnit, LW(10), 0)
    ELSE
        CALL(btlevtcmd_KillUnit, LW(10), 2)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
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
        CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(weapon_wanwan_attack))
        CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(weapon_wanwan_attack), LW(3), LW(4))
    END_IF
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_CheckToken, -2, 16, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL_CHILD(subsetevt_confuse_flustered)
            RETURN
        END_IF
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 127)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(14))
    CALL(btlevtcmd_WeaponAftereffect, PTR(weapon_wanwan_attack))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, PTR(weapon_wanwan_attack))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangePartsFaceDirection, -2, 1, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 0, 0)
    CALL(btlevtcmd_StopWaitEvent, -2)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_A_1"))
    LOOP(2)
        CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, 1, 20, 0, FLOAT(0.7f))
        BEGIN_SIBLING_EVENT
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT1"), LW(14), 0, EVTDAT_POINTER_MAX)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT3"), LW(14), 0, EVTDAT_POINTER_MAX)
            WAIT_FRAMES(10)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT2"), LW(14), 0, EVTDAT_POINTER_MAX)
        END_SIBLING_EVENT
        CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 20, -1)
    END_LOOP
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_S_1A"))
    END_SIBLING_EVENT
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_ATTACK1"), LW(14), 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetPartsPos, -2, 1, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 40)
    CALL(btlevtcmd_JumpPartsSetting, -2, 1, 30, 0, FLOAT(0.3f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 30, -1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_A_1"))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_ATTACK2"), LW(14), 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    SUBTRACT(LW(1), 10)
    IF_INT_LESS(LW(1), 0)
        SET_VALUE(LW(1), 0)
    END_IF
    CALL(btlevtcmd_JumpPartsSetting, -2, 1, 0, FLOAT(16.0f), FLOAT(0.0f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), LW(1), 10, 0, -1)
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), PTR(weapon_wanwan_attack), 0, LW(5))
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
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT1"), LW(14), 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_S_1A"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 1, 0, FLOAT(8.0f), FLOAT(0.0f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), LW(1), LW(2), 0, -1)
    GOTO(98)
    LABEL(91)
    CALL(evt_btl_camera_shake_h, 0, 1, 1, 8, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_add_zoom, 0, -50)
    CALL(btlevtcmd_ResultACDefence, LW(3), PTR(weapon_wanwan_attack))
    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(weapon_wanwan_attack), 256, LW(5))
    WAIT_FRAMES(30)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT1"), LW(14), 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_S_1A"))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_JumpPartsSetting, -2, 1, 0, FLOAT(8.0f), FLOAT(0.25f))
    CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), LW(1), LW(2), 0, -1)
    GOTO(98)
    LABEL(98)
    CALL(evt_btl_camera_set_mode, 0, 0)
    WAIT_MS(250)
    LABEL(99)
    CALL(btlevtcmd_SetUnitWork, -2, 0, 1)
    CALL(btlevtcmd_ChangePartsFaceDirection, -2, 1, -1)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_CheckStatus, -2, 1, LW(15))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_N_1"))
        CALL(btlevtcmd_SetPartsRotateOffset, -2, 1, 0, 10, 0)
        BEGIN_SIBLING_EVENT
            LABEL(1)
            CALL_CHILD(sound_gensui_event)
            CALL(btlevtcmd_SetPartsRotate, -2, 1, 0, 0, 0)
            WAIT_FRAMES(20)
            LOOP(10)
                WAIT_FRAMES(4)
                CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 0, 1)
            END_LOOP
            LOOP(10)
                WAIT_FRAMES(4)
                CALL(btlevtcmd_AddPartsRotate, -2, 1, 0, 0, -1)
            END_LOOP
            GOTO(1)
        END_SIBLING_EVENT
        GOTO(98)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 2, LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 9, LW(15))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_SetUnitWork, -2, 2, 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_Y_1"))
        BEGIN_SIBLING_EVENT
            LABEL(5)
            CALL_CHILD(sound_gensui_event)
            CALL(evt_sub_random, 20, LW(0))
            WAIT_FRAMES(LW(0))
            GOTO(5)
        END_SIBLING_EVENT
        GOTO(98)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 5, LW(13))
    CALL(btlevtcmd_CheckStatus, -2, 4, LW(14))
    CALL(btlevtcmd_CheckStatus, -2, 3, LW(15))
    OR(LW(15), LW(13))
    OR(LW(15), LW(14))
    IF_INT_NOT_EQUAL(LW(15), 0)
        BEGIN_SIBLING_EVENT
            LABEL(3)
            CALL_CHILD(sound_gensui_event)
            CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
            CALL(evt_sub_random, 20, LW(0))
            WAIT_FRAMES(LW(0))
            SET_VALUE(LW(10), 15)
            SPAWN_EVENT(wait_sound_event1)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpPartsSetting, -2, 1, 30, 0, FLOAT(0.25f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 30, 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
            SET_VALUE(LW(10), 10)
            SPAWN_EVENT(wait_sound_event2)
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
            CALL(btlevtcmd_JumpPartsSetting, -2, 1, 20, 0, FLOAT(0.2f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 20, 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
            SET_VALUE(LW(10), 7)
            SPAWN_EVENT(wait_sound_event2)
            CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
            CALL(btlevtcmd_JumpPartsSetting, -2, 1, 15, 0, FLOAT(0.4f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 15, 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
            SET_VALUE(LW(10), 7)
            SPAWN_EVENT(wait_sound_event2)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_JumpPartsSetting, -2, 1, 15, 0, FLOAT(0.2f))
            CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), LW(1), LW(2), 15, 0)
            CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
            GOTO(3)
        END_SIBLING_EVENT
        GOTO(98)
    END_IF
    BEGIN_SIBLING_EVENT
        LABEL(4)
        CALL_CHILD(sound_gensui_event)
        CALL(evt_sub_random, 20, LW(0))
        WAIT_FRAMES(LW(0))
        SET_VALUE(LW(10), 15)
        SPAWN_EVENT(wait_sound_event1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_A_1"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, 1, 30, 0, FLOAT(0.25f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 30, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(10), 10)
        SPAWN_EVENT(wait_sound_event2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_S_1A"))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 5)
        CALL(btlevtcmd_JumpPartsSetting, -2, 1, 20, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 20, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(10), 7)
        SPAWN_EVENT(wait_sound_event2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_A_1"))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 20)
        CALL(btlevtcmd_JumpPartsSetting, -2, 1, 15, 0, FLOAT(0.4f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), 0, LW(2), 15, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
        SET_VALUE(LW(10), 7)
        SPAWN_EVENT(wait_sound_event2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("WAN_S_1A"))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_JumpPartsSetting, -2, 1, 15, 0, FLOAT(0.2f))
        CALL(btlevtcmd_JumpPartsPosition, -2, 1, LW(0), LW(1), LW(2), 15, 0)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_LANDING1"), LW(11), 0, EVTDAT_POINTER_MAX)
        GOTO(4)
    END_SIBLING_EVENT
    GOTO(98)
    LABEL(98)
    LABEL(99)
    WAIT_FRAMES(1)
    GOTO(99)
    RETURN
EVT_END()

static EVT_BEGIN(sound_gensui_event)
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(11))
    MULTIPLY_FLOAT(LW(11), FLOAT(0.7f))
    IF_INT_LESS(LW(11), 32)
        SET_VALUE(LW(11), 32)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, LW(11))
    RETURN
EVT_END()

static EVT_BEGIN(wait_sound_event1)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT1"), LW(11), 0, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT3"), LW(11), 0, EVTDAT_POINTER_MAX)
    WAIT_FRAMES(LW(10))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT2"), LW(11), 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()

static EVT_BEGIN(wait_sound_event2)
    WAIT_FRAMES(LW(10))
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_WAN_WAIT4"), LW(11), 0, EVTDAT_POINTER_MAX)
    RETURN
EVT_END()
// clang-format on
