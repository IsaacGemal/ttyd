#include "gor_irai.h"
#include "driver/icondrv.h"
#include "driver/msgdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor* color);
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_door.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_pouch.h"
#include "forcedata.h"
#include <dolphin/gx.h>
#include <dolphin/pad.h>
#include <string.h>

// The trouble-center (irai) work area, allocated by irai_init_func.
typedef struct IraiWork {
    s32 state;                   // 0x000
    s32 count;                   // 0x004
    s32 iraiId[30];              // 0x008
    s32 cursor;                  // 0x080
    s32 prevCursor;              // 0x084
    s32 top;                     // 0x088
    s32 cursorX;                 // 0x08C, finger icon screen X
    s32 cursorY;                 // 0x090, finger icon screen Y
    s32 unk_0x94;                // 0x094
    f32 scroll;                  // 0x098
    char clientName[30][0x15];   // 0x09C
    char contentsName[30][0x1F]; // 0x312
} IraiWork;

#define IRAI_WORK_SIZE 0x416C

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
    "irai_client_01", "irai_client_02", "irai_client_03", "irai_client_04",
    "irai_client_05", "irai_client_06", "irai_client_07", "irai_client_08",
    "irai_client_09", "irai_client_10", "irai_client_11", "irai_client_12",
    "irai_client_13", "irai_client_14", "irai_client_15", "irai_client_16",
    "irai_client_17", "irai_client_18", "irai_client_19", "irai_client_20",
    "irai_client_21", "irai_client_22", "irai_client_23", "irai_client_24",
    "irai_client_25", "irai_client_26", "irai_client_27", "irai_client_28",
    "irai_client_29", "irai_client_30",
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
    "irai_help_01", "irai_help_02", "irai_help_03", "irai_help_04",
    "irai_help_05", "irai_help_06", "irai_help_07", "irai_help_08",
    "irai_help_09", "irai_help_10", "irai_help_11", "irai_help_12",
    "irai_help_13", "irai_help_14", "irai_help_15", "irai_help_16",
    "irai_help_17", "irai_help_18", "irai_help_19", "irai_help_20",
    "irai_help_21", "irai_help_22", "irai_help_23", "irai_help_24",
    "irai_help_25", "irai_help_26", "irai_help_27", "irai_help_28",
    "irai_help_29", "irai_help_30",
};

char* irai_msg_kakunin[30] = {
    "irai_43", "irai_44", "irai_45", "irai_46",
    "irai_47", "irai_48", "irai_49", "irai_50",
    "irai_51", "irai_52", "irai_53", "irai_54",
    "irai_55", "irai_56", "irai_57", "irai_58",
    "irai_59", "irai_60", "irai_61", "irai_62",
    "irai_63", "irai_64", "irai_65", "irai_66",
    "irai_67", "irai_68", "irai_69", "irai_70",
    "irai_71", "irai_72",
};

char* irai_msg_hikiukechu[30] = {
    "irai_13", "irai_14", "irai_15", "irai_16",
    "irai_17", "irai_18", "irai_19", "irai_20",
    "irai_21", "irai_22", "irai_23", "irai_24",
    "irai_25", "irai_26", "irai_27", "irai_28",
    "irai_29", "irai_30", "irai_31", "irai_32",
    "irai_33", "irai_34", "irai_35", "irai_36",
    "irai_37", "irai_38", "irai_39", "irai_40",
    "irai_41", "irai_42",
};

s32 irai_item_data01[2] = {
    0x00000031, -1,
};

s32 irai_item_data02[2] = {
    0x0000005F, -1,
};

s32 irai_item_data13[2] = {
    0x0000006B, -1,
};

s32 irai_item_data16[2] = {
    0x0000006E, -1,
};

s32 irai_item_data17[2] = {
    0x00000062, -1,
};

s32 irai_item_data19[2] = {
    0x00000049, -1,
};

s32 irai_item_data23[2] = {
    0x00000063, -1,
};

s32 irai_item_data25[2] = {
    0x0000006D, -1,
};

s32 irai_item_data29[2] = {
    0x00000064, -1,
};

s32 irai_item_null[1] = {
    -1,
};

s32* irai_item_table[30] = {
    irai_item_data01, irai_item_data02, irai_item_null, irai_item_null,
    irai_item_null, irai_item_null, irai_item_null, irai_item_null,
    irai_item_null, irai_item_null, irai_item_null, irai_item_null,
    irai_item_data13, irai_item_null, irai_item_null, irai_item_data16,
    irai_item_data17, irai_item_null, irai_item_data19, irai_item_null,
    irai_item_null, irai_item_null, irai_item_data23, irai_item_null,
    irai_item_data25, irai_item_null, irai_item_null, irai_item_null,
    irai_item_data29, irai_item_null,
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

// NOTE: functions are defined in the reverse of the order they appear in the object's .text
// (deferred inlining emits a file's functions in reverse source order), so flag_clear is first here.

void irai_flag_clear(void) {
    s32 i;

    for (i = (s32)0xF840709D; i <= (s32)0xF84070AD; i++) {
        swClear(i + EVTDAT_GSWF_BASE);
    }
    for (i = (s32)0xF5DE073D; i <= (s32)0xF5DE0742; i++) {
        swByteSet(i + EVTDAT_GSW_BASE, 0);
    }
}

void irai_flag_disp(void) {
    s32 i;

    for (i = (s32)0xF840709D; i <= (s32)0xF84070AD; i++) {
    }
    for (i = (s32)0xF5DE073D; i <= (s32)0xF5DE0742; i++) {
    }
}

USER_FUNC(irai_start_func) {
    s32 i;

    for (i = (s32)0xF840709D; i <= (s32)0xF84070AD; i++) {
        swClear(i + EVTDAT_GSWF_BASE);
    }
    for (i = (s32)0xF5DE073D; i <= (s32)0xF5DE0742; i++) {
        swByteSet(i + EVTDAT_GSW_BASE, 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(irai_cancel_func) {
    s32 i;
    s32* items;
    ItemType val = evtGetValue(event, event->args[0]) - 1;
    ItemType item;

    for (i = (s32)0xF840709D; i <= (s32)0xF84070AD; i++) {
        swClear(i + EVTDAT_GSWF_BASE);
    }
    for (i = (s32)0xF5DE073D; i <= (s32)0xF5DE0742; i++) {
        swByteSet(i + EVTDAT_GSW_BASE, 0);
    }
    items = irai_item_table[val];
load:
    item = *items;
    if ((s32)item == -1) {
        goto end;
    }
loop:
#ifdef MUST_MATCH
    // Redundant self-cast: makes MWCC reuse item's callee-saved register for the
    // u32 conversion at the pouchCheckItem call instead of emitting a copy.
    item = (ItemType)item;
#endif
    if ((s32)pouchCheckItem(item) <= 0) {
        goto next;
    }
    pouchRemoveItem(item);
    goto loop;
next:
    items++;
    goto load;
end:;
    return EVT_RETURN_DONE;
}

USER_FUNC(irai_init_func) {
    IraiWork* work;
    s32 count;
    s32 i;
    s32 j;
    s32 level;
    const char* str;

    for (i = (s32)0xF840709D; i <= (s32)0xF84070AD; i++) {
    }
    for (i = (s32)0xF5DE073D; i <= (s32)0xF5DE0742; i++) {
    }
    work = _mapAlloc(IRAI_WORK_SIZE);
    memset(work, 0, IRAI_WORK_SIZE);
    work->state = 0;
    level = swByteGet(0);
    work->iraiId[0] = 1;
    work->iraiId[1] = 2;
    work->iraiId[2] = 3;
    work->iraiId[3] = 4;
    work->iraiId[4] = 5;
    work->iraiId[5] = 6;
    if (level >= 0x38) {
        work->iraiId[6] = 7;
        work->iraiId[7] = 8;
        work->iraiId[8] = 9;
    }
    if (level >= 0x70) {
        work->iraiId[9] = 0xA;
        work->iraiId[10] = 0xB;
        work->iraiId[11] = 0xC;
    }
    if (level >= 0xA4) {
        work->iraiId[12] = 0xD;
        work->iraiId[13] = 0xE;
        work->iraiId[14] = 0xF;
    }
    if (level >= 0xD7) {
        work->iraiId[15] = 0x10;
        work->iraiId[16] = 0x11;
        work->iraiId[17] = 0x12;
    }
    if (level >= 0x105) {
        work->iraiId[18] = 0x14;
        work->iraiId[19] = 0x15;
    }
    if (level >= 0x119) {
        work->iraiId[20] = 0x13;
    }
    if (level >= 0x153) {
        work->iraiId[21] = 0x16;
        work->iraiId[22] = 0x17;
        work->iraiId[23] = 0x18;
    }
    if (level >= 0x175) {
        work->iraiId[24] = 0x19;
        work->iraiId[25] = 0x1A;
        work->iraiId[26] = 0x1B;
    }
    if (level >= 0x191) {
        work->iraiId[27] = 0x1C;
        work->iraiId[28] = 0x1D;
        work->iraiId[29] = 0x1E;
    }
    count = 0;
    for (j = 0; j < 30; j++) {
        if (work->iraiId[j] != 0) {
            count++;
        }
    }
    work->count = count;
    for (i = 0; i < 30; i++) {
    }
    for (i = 0; i < 30; i++) {
        str = msgSearch((const char*)irai_msg_client[i]);
        strcpy(work->clientName[i], str);
        str = msgSearch((const char*)irai_msg_contents[i]);
        strcpy(work->contentsName[i], str);
    }
    work->cursor = 0;
    work->prevCursor = work->cursor;
    work->top = 0;
    work->unk_0x94 = 0;
    work->scroll = 0.0f;
    event->lwData[10] = (s32)work;
    return EVT_RETURN_DONE;
}

USER_FUNC(irai_main_func) {
    IraiWork* work = (IraiWork*)event->lwData[10];
    s32 prevCursor;
    s32 n;

    switch (work->state) {
    case 0:
        iconEntry2D("iraiYubi", 0x1F0);
        iconFlagOn("iraiYubi", 4);
        iconEntry2D("iraiArrowUp", 0x205);
        iconFlagOn("iraiArrowUp", 8);
        iconEntry2D("iraiArrowDown", 0x205);
        iconEntry2D("iraiLButton", 0x7E);
        iconSetScale("iraiLButton", 0.8f);
        iconEntry2D("iraiRButton", 0x80);
        iconSetScale("iraiRButton", 0.8f);
        iconFlagOn("iraiYubi", 2);
        iconFlagOn("iraiArrowUp", 2);
        iconFlagOn("iraiArrowDown", 2);
        iconFlagOn("iraiLButton", 2);
        iconFlagOn("iraiRButton", 2);
        work->state = 1;
        psndSFXOn((const char*)0x2A);
        break;
    case 1:
        if (keyGetButtonTrg(0) & PAD_BUTTON_A) {
            if (swByteGet(0x58C) == 0) {
                if (swGet(work->iraiId[work->cursor] + 0x14FE) == 0) {
                    psndSFXOn((const char*)0x12);
                    work->state = 100;
                    break;
                }
            }
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_B) {
            psndSFXOn((const char*)0x13);
            work->state = 0x65;
            break;
        }
        prevCursor = work->cursor;
        if (keyGetDirRep(0) & 0x2000) {
            if (work->count - 1 <= work->cursor) {
                if (keyGetDirTrg(0) & 0x2000) {
                    psndSFXOn((const char*)5);
                    work->cursor++;
                }
            } else {
                psndSFXOn((const char*)5);
                work->cursor++;
            }
        }
        if (keyGetDirRep(0) & 0x1000) {
            if (work->cursor <= 0) {
                if (keyGetDirTrg(0) & 0x1000) {
                    psndSFXOn((const char*)5);
                    work->cursor--;
                }
            } else {
                psndSFXOn((const char*)5);
                work->cursor--;
            }
        }
        if (work->cursor >= work->count) {
            work->cursor = 0;
            work->top = 0;
        }
        if (work->cursor < 0) {
            work->cursor = work->count - 1;
            n = work->count - 10;
            if (n > 0) {
                work->top = n;
            }
        }
        if (prevCursor < work->cursor) {
            if (work->cursor - work->top > 7) {
                if (work->top < work->count - 10) {
                    work->top++;
                }
            }
        } else if (prevCursor > work->cursor) {
            if (work->cursor - work->top < 3) {
                if (work->top > 0) {
                    work->top--;
                }
            }
        }
        if (work->top > 0 && (keyGetButtonTrg(0) & PAD_TRIGGER_L)) {
            s32 amt = 10;
            if (work->top < amt) {
                amt = work->top;
            }
            work->top -= amt;
            work->cursor -= amt;
            psndSFXOn((const char*)5);
        }
        if (work->top < work->count - 10 && (keyGetButtonTrg(0) & PAD_TRIGGER_R)) {
            s32 amt = 10;
            if (work->count - 10 - work->top < amt) {
                amt = work->count - 10 - work->top;
            }
            work->top += amt;
            work->cursor += amt;
            psndSFXOn((const char*)5);
        }
        if (work->cursor != work->prevCursor) {
            work->unk_0x94 = 0;
        }
        work->prevCursor = work->cursor;
        work->scroll += ((f32)(work->top * 30) - work->scroll) / 6.0f;
        work->cursorX = -0x81;
        work->cursorY = 0x78 - (work->cursor - work->top) * 0x1E;
        iconSetPos("iraiYubi", (f32)work->cursorX, (f32)work->cursorY, 0.0f);
        iconFlagOff("iraiYubi", 2);
        iconSetPos("iraiArrowUp", 231.0f, 178.0f, 0.0f);
        iconSetPos("iraiArrowDown", 231.0f, -190.0f, 0.0f);
        iconSetPos("iraiLButton", 261.0f, 146.0f, 0.0f);
        iconSetPos("iraiRButton", 261.0f, -190.0f, 0.0f);
        if (work->top == 0) {
            iconFlagOn("iraiArrowUp", 2);
            iconFlagOn("iraiLButton", 2);
        } else {
            iconFlagOff("iraiArrowUp", 2);
            iconFlagOff("iraiLButton", 2);
        }
        if (work->top >= work->count - 10) {
            iconFlagOn("iraiArrowDown", 2);
            iconFlagOn("iraiRButton", 2);
        } else {
            iconFlagOff("iraiArrowDown", 2);
            iconFlagOff("iraiRButton", 2);
        }
        break;
    case 100:
        event->lwData[0] = work->iraiId[work->cursor];
        work->state = 0;
        iconDelete("iraiYubi");
        iconDelete("iraiArrowUp");
        iconDelete("iraiArrowDown");
        iconDelete("iraiLButton");
        iconDelete("iraiRButton");
        psndSFXOn((const char*)0x2B);
        return EVT_RETURN_DONE;
    case 0x65:
        event->lwData[0] = -1;
        work->state = 0;
        iconDelete("iraiYubi");
        iconDelete("iraiArrowUp");
        iconDelete("iraiArrowDown");
        iconDelete("iraiLButton");
        iconDelete("iraiRButton");
        psndSFXOn((const char*)0x2B);
        return EVT_RETURN_DONE;
    }
    dispEntry(8, 300.0f, 0, irai_window_disp, work);
    return EVT_RETURN_BLOCK;
}

USER_FUNC(irai_check_func) {
    if (event->lwData[0] != -1) {
        event->lwData[12] = (s32)irai_msg_kakunin[event->lwData[0] - 1];
        event->lwData[11] = event->lwData[0];
    } else {
        event->lwData[12] = 0;
        event->lwData[11] = -1;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(irai_end_func) {
    _mapFree((void*)event->lwData[10]);
    return EVT_RETURN_DONE;
}

void irai_window_disp(CameraId cameraId, void* param) {
    IraiWork* work = (IraiWork*)param;
    const char* name;
    u32 scissorX, scissorY, scissorW, scissorH;
    s32 idx;
    s32 i;
    f32 y;

    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    windowDispGX_Waku_col(-280.0f, 170.0f, 135.0f, 90.0f, 12.0f, 0, (GXColor){0x2B, 0xA3, 0xFF, 0xFF});
    FontDrawStart();
    FontDrawColor(&(GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    FontDrawScale(0.85f);
    name = msgSearch("irai_name1_win");
    FontDrawEdge();
    FontDrawString(name, -270.0f, 152.0f);
    FontDrawString(msgSearch("irai_name2_win"), -270.0f, 127.0f);
    FontDrawEdgeOff();
    windowDispGX_Waku_col(-129.0f, 170.0f, 410.0f, 340.0f, 12.0f, 0, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    windowDispGX_Waku_col(-103.0f, 190.0f, 120.0f, 40.0f, 12.0f, 0, (GXColor){0x2B, 0xA3, 0xFF, 0xFF});
    windowDispGX_Waku_col(91.0f, 190.0f, 120.0f, 40.0f, 12.0f, 0, (GXColor){0x2B, 0xA3, 0xFF, 0xFF});
    FontDrawStart();
    FontDrawColor(&(GXColor){0xFF, 0xFF, 0xFF, 0xFF});
    FontDrawScale(0.85f);
    name = msgSearch("irai_client_win");
    FontDrawEdge();
    FontDrawString(name, -77.0f, 180.0f);
    FontDrawEdgeOff();
    name = msgSearch("irai_contents_win");
    FontDrawEdge();
    FontDrawString(name, 115.0f, 180.0f);
    FontDrawEdgeOff();
    GXGetScissor(&scissorX, &scissorY, &scissorW, &scissorH);
    GXSetScissor(0xAF, 0x62, 0x19A, 0x130);
    FontDrawStart();
    FontDrawColor(&(GXColor){0, 0, 0, 0xFF});
    FontDrawScale(0.85f);
    for (i = 0; i < work->count; i++) {
        if (swGet(work->iraiId[i] + 0x14FE)) {
            FontDrawColor(&(GXColor){0x80, 0x80, 0x80, 0xFF});
        } else {
            FontDrawColor(&(GXColor){0, 0, 0, 0xFF});
        }
        if (work->iraiId[i] == swByteGet(0x58C)) {
            FontDrawColor(&(GXColor){0xFF, 0, 0, 0xFF});
        }
        idx = work->iraiId[i] - 1;
        y = work->scroll + (f32)(0x8C - i * 0x1E);
        FontDrawString(work->clientName[idx], -119.0f, y);
        y = work->scroll + (f32)(0x8C - i * 0x1E);
        FontDrawString(work->contentsName[idx], 51.0f, y);
    }
    GXSetScissor(scissorX, scissorY, scissorW, scissorH);
}
// clang-format on
