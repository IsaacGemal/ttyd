#include "gor_irai.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_pouch.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
extern s32 door_data_05_npc[];
extern s32 door_data_05_npc_01[];
s32 gor_irai_main[105];
s32 gor_irai_tenshu_main[258];

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

char* irai_msg_client[30] = {
    "irai_client_01", "irai_client_02", "irai_client_03", "irai_client_04", "irai_client_05",
    "irai_client_06", "irai_client_07", "irai_client_08", "irai_client_09", "irai_client_10",
    "irai_client_11", "irai_client_12", "irai_client_13", "irai_client_14", "irai_client_15",
    "irai_client_16", "irai_client_17", "irai_client_18", "irai_client_19", "irai_client_20",
    "irai_client_21", "irai_client_22", "irai_client_23", "irai_client_24", "irai_client_25",
    "irai_client_26", "irai_client_27", "irai_client_28", "irai_client_29", "irai_client_30",
};

char* irai_msg_contents[30] = {
    "irai_contents_01", "irai_contents_02", "irai_contents_03", "irai_contents_04",
    "irai_contents_05", "irai_contents_06", "irai_contents_07", "irai_contents_08",
    "irai_contents_09", "irai_contents_10", "irai_contents_11", "irai_contents_12",
    "irai_contents_13", "irai_contents_14", "irai_contents_15", "irai_contents_16",
    "irai_contents_17", "irai_contents_18", "irai_contents_19", "irai_contents_20",
    "irai_contents_21", "irai_contents_22", "irai_contents_23", "irai_contents_24",
    "irai_contents_25", "irai_contents_26", "irai_contents_27", "irai_contents_28",
    "irai_contents_29", "irai_contents_30",
};

char* irai_msg_exposition[30] = {
    "irai_help_01", "irai_help_02", "irai_help_03", "irai_help_04", "irai_help_05", "irai_help_06",
    "irai_help_07", "irai_help_08", "irai_help_09", "irai_help_10", "irai_help_11", "irai_help_12",
    "irai_help_13", "irai_help_14", "irai_help_15", "irai_help_16", "irai_help_17", "irai_help_18",
    "irai_help_19", "irai_help_20", "irai_help_21", "irai_help_22", "irai_help_23", "irai_help_24",
    "irai_help_25", "irai_help_26", "irai_help_27", "irai_help_28", "irai_help_29", "irai_help_30",
};

char* irai_msg_kakunin[30] = {
    "irai_43", "irai_44", "irai_45", "irai_46", "irai_47", "irai_48", "irai_49", "irai_50",
    "irai_51", "irai_52", "irai_53", "irai_54", "irai_55", "irai_56", "irai_57", "irai_58",
    "irai_59", "irai_60", "irai_61", "irai_62", "irai_63", "irai_64", "irai_65", "irai_66",
    "irai_67", "irai_68", "irai_69", "irai_70", "irai_71", "irai_72",
};

char* irai_msg_hikiukechu[30] = {
    "irai_13", "irai_14", "irai_15", "irai_16", "irai_17", "irai_18", "irai_19", "irai_20",
    "irai_21", "irai_22", "irai_23", "irai_24", "irai_25", "irai_26", "irai_27", "irai_28",
    "irai_29", "irai_30", "irai_31", "irai_32", "irai_33", "irai_34", "irai_35", "irai_36",
    "irai_37", "irai_38", "irai_39", "irai_40", "irai_41", "irai_42",
};

// TODO: type
u32 irai_item_data01[2] = {
    0x00000031, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data02[2] = {
    0x0000005F, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data13[2] = {
    0x0000006B, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data16[2] = {
    0x0000006E, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data17[2] = {
    0x00000062, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data19[2] = {
    0x00000049, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data23[2] = {
    0x00000063, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data25[2] = {
    0x0000006D, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_data29[2] = {
    0x00000064, 0xFFFFFFFF,
};

// TODO: type
u32 irai_item_null[1] = {
    0xFFFFFFFF,
};

// TODO: type
u32 irai_item_table[30] = {
    (u32)&irai_item_data01, (u32)&irai_item_data02, (u32)&irai_item_null, (u32)&irai_item_null,
    (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_null,
    (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_null,
    (u32)&irai_item_data13, (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_data16,
    (u32)&irai_item_data17, (u32)&irai_item_null, (u32)&irai_item_data19, (u32)&irai_item_null,
    (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_data23, (u32)&irai_item_null,
    (u32)&irai_item_data25, (u32)&irai_item_null, (u32)&irai_item_null, (u32)&irai_item_null,
    (u32)&irai_item_data29, (u32)&irai_item_null,
};

EVT_BEGIN(irai_start_event_null)
    RETURN
EVT_END()

EVT_BEGIN(irai_start_event01)
    CALL(evt_npc_set_position, STRING("盗賊団２"), 445, 0, 120)
    CALL(evt_door_set_param, GW(11), 0, PTR(door_data_05_npc_01))
    RETURN
EVT_END()

EVT_BEGIN(irai_start_event04)
    CALL(evt_npc_set_position, STRING("ボロドー２"), -475, 0, -180)
    RETURN
EVT_END()

// TODO: type
u32 irai_start_event[30] = {
    (u32)&irai_start_event01, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event04,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null, (u32)&irai_start_event_null,
    (u32)&irai_start_event_null, (u32)&irai_start_event_null,
};

EVT_BEGIN(irai_end_event_null)
    RETURN
EVT_END()

EVT_BEGIN(irai_end_event01)
    CALL(evt_npc_set_position, STRING("盗賊団２"), 10, 0, -460)
    CALL(evt_door_set_param, GW(11), 0, PTR(door_data_05_npc))
    RETURN
EVT_END()

EVT_BEGIN(irai_end_event04)
    CALL(evt_npc_set_position, STRING("ボロドー２"), -475, 0, -180)
    RETURN
EVT_END()

// TODO: type
u32 irai_end_event[30] = {
    (u32)&irai_end_event01, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event04,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null, (u32)&irai_end_event_null,
    (u32)&irai_end_event_null, (u32)&irai_end_event_null,
};

EVT_BEGIN(gor_irai_init)
    CALL(evt_run_case_evt, 9, 1, STRING("A_i04_keizi"), 0, PTR(gor_irai_main), 0)
    CALL(evt_run_case_evt, 9, 1, STRING("A_i04_kunter"), 0, PTR(gor_irai_tenshu_main), 0)
    RETURN
EVT_END()

EVT_BEGIN(gor_irai_lecture)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_msg_toge, 2, 150, 12, -156)
    CALL(evt_msg_print, 0, STRING("irai_00"), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_01"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("irai_03"))
        SET_VALUE(GSWF(5374), 1)
    ELSE
        CALL(evt_msg_print_add, 0, STRING("irai_02"))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write irai_end_func (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_check_func (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_main_func (global, 0x724 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_init_func (global, 0x2a4 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_start_func (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(gor_irai_main)
    IF_INT_EQUAL(GSWF(5374), 0)
        CALL_CHILD(gor_irai_lecture)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(irai_init_func)
    LABEL(10)
    CALL(irai_main_func)
    CALL(irai_check_func)
    IF_INT_EQUAL(LW(0), -1)
        GOTO(100)
    END_IF
    SET_INT_READ_ADDR(PTR(irai_msg_exposition))
    SET_VALUE(LW(0), LW(11))
    ADD(LW(0), -1)
    READ_INT_INDEXED(LW(1), LW(0))
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, LW(1), 0, 0)
    CALL(evt_msg_toge, 1, 0, 0, 0)
    CALL(evt_msg_print, 0, LW(12), 0, 0)
    CALL(evt_msg_select, 0, STRING("irai_01"))
    IF_INT_EQUAL(LW(0), 0)
        CALL(evt_msg_print_add, 0, STRING("irai_73"))
        CALL(irai_start_func)
        SET_INT_READ_ADDR(PTR(irai_start_event))
        SET_VALUE(LW(0), LW(11))
        ADD(LW(0), -1)
        READ_INT_INDEXED(LW(1), LW(0))
        SPAWN_EVENT(LW(1))
        SET_VALUE(GSW(1420), LW(11))
        DEBUG_PUT_REG(GSW(1420))
    ELSE
        CALL(evt_msg_continue)
        GOTO(10)
    END_IF
    LABEL(100)
    CALL(irai_end_func)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write irai_cancel_func (global, 0xcc bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

EVT_BEGIN(gor_irai_tenshu_main)
    SET_VALUE(LW(0), GSWF(5375))
    ADD(LW(0), GSWF(5376))
    ADD(LW(0), GSWF(5377))
    ADD(LW(0), GSWF(5378))
    ADD(LW(0), GSWF(5379))
    ADD(LW(0), GSWF(5380))
    ADD(LW(0), GSWF(5381))
    ADD(LW(0), GSWF(5382))
    ADD(LW(0), GSWF(5383))
    ADD(LW(0), GSWF(5384))
    ADD(LW(0), GSWF(5385))
    ADD(LW(0), GSWF(5386))
    ADD(LW(0), GSWF(5387))
    ADD(LW(0), GSWF(5388))
    ADD(LW(0), GSWF(5389))
    ADD(LW(0), GSWF(5390))
    ADD(LW(0), GSWF(5391))
    ADD(LW(0), GSWF(5392))
    ADD(LW(0), GSWF(5393))
    ADD(LW(0), GSWF(5394))
    ADD(LW(0), GSWF(5395))
    ADD(LW(0), GSWF(5396))
    ADD(LW(0), GSWF(5397))
    ADD(LW(0), GSWF(5398))
    ADD(LW(0), GSWF(5399))
    ADD(LW(0), GSWF(5400))
    ADD(LW(0), GSWF(5401))
    ADD(LW(0), GSWF(5402))
    ADD(LW(0), GSWF(5403))
    ADD(LW(0), GSWF(5404))
    IF_INT_EQUAL(LW(0), 30)
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_msg_toge, 2, 150, 12, -156)
        CALL(evt_msg_print, 0, STRING("irai_74"), 0, 0)
        CALL(evt_mario_key_onoff, 1)
        RETURN
    END_IF
    IF_INT_EQUAL(GSWF(5374), 0)
        CALL_CHILD(gor_irai_lecture)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    IF_INT_EQUAL(GSW(1420), 0)
        CALL(evt_msg_toge, 2, 150, 12, -156)
        CALL(evt_msg_print, 0, STRING("irai_06"), 0, 0)
    ELSE
        CALL(evt_msg_toge, 2, 150, 12, -156)
        CALL(evt_msg_print, 0, STRING("irai_07"), 0, 0)
        CALL(evt_msg_select, 0, STRING("irai_08"))
        IF_INT_EQUAL(LW(0), -1)
            CALL(evt_msg_continue)
            DEBUG_PUT_MSG(STRING("依頼メニューキャンセル"))
        END_IF
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LW(10), GSW(1420))
            SET_INT_READ_ADDR(PTR(irai_msg_hikiukechu))
            ADD(LW(10), -1)
            READ_INT_INDEXED(LW(11), LW(10))
            CALL(evt_msg_print_add, 0, LW(11))
        END_IF
        IF_INT_EQUAL(LW(0), 1)
            CALL(evt_msg_print_add, 0, STRING("irai_09"))
            CALL(evt_msg_select, 0, STRING("irai_01"))
            IF_INT_EQUAL(LW(0), 0)
                CALL(evt_pouch_get_coin, LW(10))
                IF_INT_GREATER_OR_EQUAL(LW(10), 30)
                    CALL(evt_msg_print_add, 0, STRING("irai_10"))
                    CALL(evt_pouch_add_coin, -30)
                    SET_VALUE(LW(11), GSW(1420))
                    CALL(irai_cancel_func, GSW(1420))
                    SET_VALUE(GSW(1420), 0)
                    SET_INT_READ_ADDR(PTR(irai_end_event))
                    SET_VALUE(LW(0), LW(11))
                    ADD(LW(0), -1)
                    READ_INT_INDEXED(LW(1), LW(0))
                    SPAWN_EVENT(LW(1))
                ELSE
                    CALL(evt_msg_print_add, 0, STRING("irai_11"))
                END_IF
            ELSE
                CALL(evt_msg_print_add, 0, STRING("irai_12"))
            END_IF
        END_IF
        IF_INT_EQUAL(LW(0), 2)
            CALL(evt_msg_print_add, 0, STRING("irai_04"))
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

// TODO: write irai_window_disp (global, 0x438 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_flag_disp (global, 0x4c bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it

// TODO: write irai_flag_clear (global, 0x78 bytes) here:
// its asm is in build/G8MJ01/gor/asm/gor_irai.s; this is where the original has it, just before the first script that uses it
// clang-format on
