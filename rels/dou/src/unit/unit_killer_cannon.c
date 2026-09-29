#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_sub.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "math_trig.h"
#include "rel/dou/unit_killer.h"

#pragma pool_data off

#pragma force_active on
static USER_FUNC(killer_get_angle);
static USER_FUNC(killer_copy_status);
static u32 regist[6];
static u32 parts[57];
static u32 pose_table[32];
static u32 data_table[2];
static s32 init_event[63];
static s32 damage_event[10];
static s32 attack_event[738];
static s32 friend_event[244];
static s32 friend_event_sky[318];
static s32 wait_event[6];

// clang-format off
extend_data(68,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(69,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 unit_killer_cannon[49] = {
    0x0000005A, (u32)"btl_un_killer_cannon", 0x00050000, 0x01011600,
    0x0346033C, 0x270F0034, 0x00260005, 0x00260000,
    0x00000000, 0x41980000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41D00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x41A00000, 0x41200000, 0x41D00000,
    0x00000000, 0x00000000, 0x41D00000, 0x41C5999A,
    0x00000000, 0xC1200000, 0x41980000, 0x00000000,
    0x42500000, 0x42180000, 0x00000002, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_ENM_KILLER_CANON_DAMAGED1",
    (u32)"SFX_BTL_DAMAGE_FIRE1", (u32)"SFX_BTL_DAMAGE_ICE1", (u32)"SFX_BTL_DAMAGE_BIRIBIRI1", 0x00000000,
    (u32)&regist, 0x03000000, (u32)&parts, (u32)&init_event,
    (u32)&data_table,
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
    0x00000000, 0x00966464, 0x645A645A, 0x645F3200,
    0x0F643264, 0x645F0000,
};

// TODO: type
static u32 parts[57] = {
    0x00000001, (u32)"btl_un_killer_cannon", (u32)"c_killtai", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41F00000,
    0x00000000, 0x00000000, 0x420C0000, 0x00000000,
    0x0023001E, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table, 0x00000002,
    (u32)"btl_un_killer_cannon", (u32)"c_killtai", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x00010000,
    0x00000000, (u32)&pose_table, 0x00000003, (u32)"btl_un_killer_cannon",
    (u32)"c_killtai", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x00010000, 0x00000000,
    (u32)&pose_table,
};

// TODO: type
static u32 entry_killer[12] = {
    (u32)unit_killer, 0x01000000, 0x04000004, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 pose_table[32] = {
    0x00000001, (u32)"KTA_N_1", 0x00000002, (u32)"KTA_Y_1",
    0x00000009, (u32)"KTA_Y_1", 0x00000005, (u32)"KTA_K_1",
    0x00000004, (u32)"KTA_X_1", 0x00000003, (u32)"KTA_X_1",
    0x0000001C, (u32)"KTA_S_1", 0x0000001D, (u32)"KTA_Q_1",
    0x0000001E, (u32)"KTA_Q_1", 0x0000001F, (u32)"KTA_A_1",
    0x00000027, (u32)"KTA_D_1", 0x00000038, (u32)"KTA_X_1",
    0x00000039, (u32)"KTA_X_1", 0x00000032, (u32)"KTA_A_1",
    0x00000041, (u32)"KTA_T_1", 0x00000045, (u32)"KTA_S_1",
};

// TODO: type
static u32 data_table[2] = {
    0x00000000, 0x00000000,
};

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(attack_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    CALL(btlevtcmd_AddPos, -2, 20, 0, 0)
    CALL(btlevtcmd_AddHomePos, -2, 20, 0, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
    CALL(btlevtcmd_AnimeChangePose, -2, 3, STRING("KTA_Z_2"))
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 33554432)
    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 33554432)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    CALL(btlevtcmd_EnemyItemUseCheck, -2, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL_CHILD(LW(0))
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    SET_VALUE(LW(0), 15)
    ADD(LW(0), 0)
    ADD(LW(0), 10)
    ADD(LW(0), 20)
    ADD(LW(0), 0)
    ADD(LW(0), 10)
    SUBTRACT(LW(0), 1)
    CALL(evt_sub_random, LW(0), LW(1))
    SET_VALUE(LW(0), 15)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 0)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 1)
                    CALL_CHILD(friend_event)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 20)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event_sky)
                    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
                    LOOP(0)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 2)
                        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 0)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 1)
                    CALL_CHILD(friend_event_sky)
                    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
                    LOOP(0)
                        IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                            BREAK_LOOP
                        END_IF
                        ADD(LW(11), 2)
                        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
                        WAIT_FRAMES(1)
                    END_LOOP
                    CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
                    CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
                    GOTO(99)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        GOTO(99)
    END_IF
    ADD(LW(0), 10)
    IF_INT_LESS(LW(1), LW(0))
        SET_VALUE(LW(15), 0)
        SET_VALUE(LW(14), 160)
        SET_VALUE(LW(13), 30)
        CALL(btlevtcmd_OnPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OffPartsAttribute, -2, 3, 16777216)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, 0)
        LOOP(0)
            CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
            CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), LW(14))
            SET_VALUE(LW(4), LW(0))
            CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
            MULTIPLY(LW(4), LW(5))
            IF_INT_LESS(LW(4), LW(5))
                CALL(btlevtcmd_CheckSpace, LW(3), LW(0), LW(1), LW(2), 40, 0, 0, 0)
                ADD(LW(2), LW(13))
                IF_INT_EQUAL(LW(3), -1)
                    SET_VALUE(LW(12), 0)
                    CALL_CHILD(friend_event_sky)
                END_IF
            END_IF
            ADD(LW(15), 1)
            SUBTRACT(LW(14), 40)
            SUBTRACT(LW(13), 10)
            IF_INT_GREATER_OR_EQUAL(LW(15), 3)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
        LOOP(0)
            IF_INT_GREATER_OR_EQUAL(LW(11), 0)
                BREAK_LOOP
            END_IF
            ADD(LW(11), 2)
            CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_OffPartsAttribute, -2, 1, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 2, 16777216)
        CALL(btlevtcmd_OnPartsAttribute, -2, 3, 16777216)
        GOTO(99)
    END_IF
    LABEL(99)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static s32 killer_copy_status(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id1 = BattleTransID(event, evtGetValue(event, args[0]));
    s32 id2 = BattleTransID(event, evtGetValue(event, args[1]));
    BattleWork* work = _battleWorkPointer;
    BattleUnit* unit1 = BattleGetUnitPtr(work, id1);
    BattleUnit* unit2 = BattleGetUnitPtr(work, id2);
    unit2->mSizeChangeTurns = unit1->mSizeChangeTurns;
    unit2->mSizeChangeStrength = unit1->mSizeChangeStrength;
    return 2;
}

static EVT_BEGIN(friend_event)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_2A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_Z_1"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_2A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_Z_1"))
        WAIT_FRAMES(10)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("KTA_A_1"))
    WAIT_FRAMES(40)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON3"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 70)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 50, LW(9), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 28)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 15, LW(9), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 40)
    ADD(LW(9), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 30, LW(9), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    GOTO(10)
    LABEL(10)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_killer), 0)
    CALL(killer_copy_status, -2, LW(3))
    CALL(btlevtcmd_SetPos, LW(3), LW(7), 10, LW(2))
    CALL(btlevtcmd_SetFallAccel, LW(3), 0)
    CALL(btlevtcmd_SetJumpSpeed, LW(3), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), 10, LW(2), FLOAT(7.0f), -1)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), 10, LW(2))
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_OnOffStatus, LW(3), 16, 1, 1, 1)
    END_IF
    RETURN
EVT_END()

static s32 killer_get_angle(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    f32 x1 = evtGetFloat(event, args[0]);
    f32 y1 = evtGetFloat(event, args[1]);
    f32 x2 = evtGetFloat(event, args[2]);
    f32 y2 = evtGetFloat(event, args[3]);
    f32 angle = atan((y2 - y1) / (x2 - x1));
    evtSetFloat(event, args[4], 360.0f * angle / 6.283184f);
    return 2;
}

static EVT_BEGIN(friend_event_sky)
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(evt_sub_get_dir, LW(7), LW(8), LW(0), 40, LW(10))
    CALL(killer_get_angle, LW(7), LW(8), LW(0), 40, LW(10))
    CALL(btlevtcmd_GetPartsRotate, -2, 2, LW(5), LW(6), LW(11))
    LOOP(0)
        IF_INT_GREATER(LW(11), 0)
            BREAK_LOOP
        END_IF
        IF_INT_LESS_OR_EQUAL(LW(11), LW(10))
            BREAK_LOOP
        END_IF
        SUBTRACT(LW(11), 2)
        CALL(btlevtcmd_SetPartsRotate, -2, 2, 0, 0, LW(11))
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON_CHARGE1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3A"))
        WAIT_FRAMES(10)
        CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_Z_3"))
        WAIT_FRAMES(10)
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 2, STRING("KTA_A_3"))
    WAIT_FRAMES(40)
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON4"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    ELSE
        CALL(btlevtcmd_snd_se, -2, STRING("SFX_ENM_KILLER_CANNON2"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 10, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 60)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 75, LW(9), FLOAT(1.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_CheckStatus, -2, 11, LW(5))
    IF_INT_NOT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
        CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 28)
        ADD(LW(9), 10)
        CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 20, LW(9), FLOAT(0.5f), 0, 0, 0, 0, 0, 0, 0)
        GOTO(10)
    END_IF
    CALL(btlevtcmd_GetPos, -2, LW(7), LW(8), LW(9))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(7), 40)
    ADD(LW(9), 10)
    CALL(evt_eff, STRING(""), STRING("kemuri_test"), 18, LW(7), 38, LW(9), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    GOTO(10)
    LABEL(10)
    CALL(evt_btl_camera_shake_w, 0, 5, 0, 20, 0)
    CALL(btlevtcmd_SpawnUnit, LW(3), PTR(entry_killer), 0)
    CALL(killer_copy_status, -2, LW(3))
    CALL(btlevtcmd_GetBodyId, LW(3), LW(4))
    CALL(btlevtcmd_SetPos, LW(3), LW(7), 10, LW(2))
    CALL(btlevtcmd_SetPartsRotate, LW(3), LW(4), 0, 0, LW(10))
    CALL(btlevtcmd_SetFallAccel, LW(3), 0)
    CALL(btlevtcmd_SetJumpSpeed, LW(3), 0)
    CALL(btlevtcmd_JumpPosition, LW(3), LW(0), 40, LW(2), FLOAT(7.0f), -1)
    CALL(btlevtcmd_SetPartsRotate, LW(3), LW(4), 0, 0, 0)
    CALL(btlevtcmd_SetHomePos, LW(3), LW(0), 40, LW(2))
    IF_INT_EQUAL(LW(12), 1)
        CALL(btlevtcmd_OnOffStatus, LW(3), 16, 1, 1, 1)
    END_IF
    CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 4194304)
    CALL(btlevtcmd_OnPartsAttribute, LW(3), LW(4), 2097152)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
