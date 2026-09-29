#include "dou_11.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_damage.h"
#include "evt/evt_eff.h"
#include "evt/evt_hit.h"
#include "evt/evt_img.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_mobj.h"
#include "evt/evt_msg.h"
#include "evt/evt_nannpc.h"
#include "evt/evt_npc.h"
#include "evt/evt_offscreen.h"
#include "evt/evt_paper.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "driver/hitdrv.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "math_ext.h"
#include "party/party_motion.h"
#include "system.h"

#include <string.h>

#pragma pool_data off

#pragma force_active on
extern s32 dou_normal_puka[];
extern s32 dou_puka_event[];
extern s32 evt_dou_normal_puka[];
static BOOL hitIsGroup(HitEntry* hit, HitEntry* group);
static BOOL _hitIsGroupSub(HitEntry* hit, HitEntry* group, BOOL rec);
s32 suifu_orei[339];
s32 bombwall_after[410];
s32 pukapuka_watari_init01[70];
s32 pukapuka_watari_init02[70];

// clang-format off
extend_data(48,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(49,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 ext_entry_data[93] = {
    (u32)"c_boro_k", (u32)"S_1", 0x00000000, (u32)"c_boro_k",
    (u32)"S_1", 0x40000000, (u32)"c_boro_k", (u32)"S_1",
    0x40800000, (u32)"c_boro_k", (u32)"S_1", 0x40C00000,
    (u32)"c_boro_k", (u32)"S_1", 0x41000000, (u32)"c_boro_k",
    (u32)"S_1", 0x41200000, (u32)"c_boro_k", (u32)"S_1",
    0x41400000, (u32)"c_boro_k", (u32)"S_1", 0x41600000,
    (u32)"c_boro_k", (u32)"S_1", 0x41800000, (u32)"c_boro_k",
    (u32)"S_1", 0x41900000, (u32)"c_boro_k", (u32)"S_1",
    0x41A00000, (u32)"c_boro_k", (u32)"S_1", 0x41B00000,
    (u32)"c_boro_k", (u32)"S_1", 0x41C00000, (u32)"c_boro_k",
    (u32)"S_1", 0x41D00000, (u32)"c_boro_k", (u32)"S_1",
    0x41E00000, (u32)"c_boro_k", (u32)"S_1", 0x41F00000,
    (u32)"c_boro_k", (u32)"S_1", 0x42000000, (u32)"c_boro_k",
    (u32)"S_1", 0x42080000, (u32)"c_boro_k", (u32)"S_1",
    0x42100000, (u32)"c_boro_k", (u32)"S_1", 0x42180000,
    (u32)"c_boro_k", (u32)"R_1", 0x00000000, (u32)"c_boro_k",
    (u32)"R_1", 0x40000000, (u32)"c_boro_k", (u32)"R_1",
    0x40800000, (u32)"c_boro_k", (u32)"R_1", 0x40C00000,
    (u32)"c_boro_k", (u32)"R_1", 0x41000000, (u32)"c_boro_k",
    (u32)"R_1", 0x41200000, (u32)"c_boro_k", (u32)"R_1",
    0x41400000, (u32)"c_boro_k", (u32)"R_1", 0x41600000,
    (u32)"c_boro_k", (u32)"R_1", 0x41800000, (u32)"c_boro_k",
    (u32)"R_1", 0x41900000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
static u32 suifu_animtbl[21] = {
    0x00000000, 0x00010001, 0x00020002, 0x00030003,
    0x00040004, 0x00050005, 0x00060006, 0x00070007,
    0x00080008, 0x00090009, 0x000A000A, 0x000B000B,
    0x000C000C, 0x000D000D, 0x000E000E, 0x000F000F,
    0x00100010, 0x00110011, 0x00120012, 0x00130013,
    0xFFFF0000,
};

// TODO: type
static u32 suifu_run_animtbl[11] = {
    0x00140014, 0x00150015, 0x00160016, 0x00170017,
    0x00180018, 0x00190019, 0x001A001A, 0x001B001B,
    0x001C001C, 0x001D001D, 0xFFFF0000,
};

static EVT_BEGIN(suifu_c_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 255)
        CALL(evt_npc_set_position, STRING("me"), 825, 100, -125)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 254)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(suifu_c_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 254)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_09"), 0, STRING("me"))
        CASE_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_09"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 255)
        CALL(evt_npc_set_position, STRING("me"), 775, 50, 50)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 254)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(suifu_d_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 254)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_07"), 0, STRING("me"))
        CASE_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_07"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(suifu_e_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 255)
        CALL(evt_npc_set_position, STRING("me"), 750, 50, -25)
    END_SWITCH
    IF_INT_EQUAL(GSW(0), 254)
        CALL(evt_npc_set_ry, STRING("me"), 90)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(suifu_e_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 254)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_08"), 0, STRING("me"))
        CASE_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_08"), 0, STRING("me"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 255)
        CALL(evt_npc_set_position, STRING("me"), 700, 50, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(mony_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 254)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_10"), 0, STRING("モニー"))
        CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_11"), 0, STRING("ピートン"))
        CASE_EQUAL(255)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_10"), 0, STRING("モニー"))
        CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_11"), 0, STRING("ピートン"))
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_init)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 255)
        CALL(evt_npc_set_position, STRING("me"), 750, 50, 0)
    END_SWITCH
    RETURN
EVT_END()

static EVT_BEGIN(peton_talk)
    SWITCH(GSW(0))
        CASE_BETWEEN(250, 254)
        CALL(evt_set_dir_to_target, STRING("モニー"), STRING("mario"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_10"), 0, STRING("モニー"))
        CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg5_dou_16_11"), 0, STRING("ピートン"))
        CASE_EQUAL(255)
        CALL(evt_set_dir_to_target, STRING("モニー"), STRING("mario"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_10"), 0, STRING("モニー"))
        CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("モニー"))
        WAIT_MS(300)
        CALL(evt_msg_print, 0, STRING("stg5_dou_17_11"), 0, STRING("ピートン"))
    END_SWITCH
    RETURN
EVT_END()

static NpcSetupInfo npcEnt[6] = {
    { "水夫C", 0x00000600, 0, &suifu_c_init, NULL, &suifu_c_talk },
    { "水夫D", 0x00000600, 0, &suifu_d_init, NULL, &suifu_d_talk },
    { "水夫E", 0x00000600, 0, &suifu_e_init, NULL, &suifu_e_talk },
    { "モニー", 0x00000600, 0, &mony_init, NULL, &mony_talk },
    { "ピートン", 0x00000600, 0, &peton_init, NULL, &peton_talk },
    { NULL },
};

static BeroEntry bero_entry_data[12] = {
    { "w_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "dou_10", "e_bero", 1, 1, 0, 0 },
    { "e_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "muj_10", "w_bero", 1, 1, 0, 0 },
    { "n_bero", 0, 0, 0x4E20, 0x186A0, 0, 0, -1, NULL, -1, NULL, "muj_00", "n_bero_02", 1, 1, 0, 0 },
    { "dokan_1", 2, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_4", 1, 1, 0, 0 },
    { "dokan_4", 0x1002, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_1", 1, 1, 0, 0 },
    { "dokan_2", 2, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_6", 1, 1, 0, 0 },
    { "dokan_6", 0x1002, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_2", 1, 1, 0, 0 },
    { "dokan_3", 2, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_5", 1, 1, 0, 0 },
    { "dokan_5", 0x1002, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_3", 1, 1, 0, 0 },
    { "dokan_7", 2, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_8", 1, 1, 0, 0 },
    { "dokan_8", 0x1002, 0, 9, 0x186A0, 0, 0, -1, NULL, 6, NULL, NULL, "dokan_7", 1, 1, 0, 0 },
    { NULL },
};

// TODO: type
u32 suifudata[400] = {
    0x0000009B, 0x0000003F, 0xFFFFFFFD, 0x0000010E,
    0x000000A2, 0x0000003C, 0xFFFFFFCE, 0x0000010E,
    0x0000007A, 0x00000056, 0xFFFFFFEF, 0x0000005A,
    0x000000B4, 0x00000060, 0x00000002, 0x0000005A,
    0x00000082, 0x0000005A, 0xFFFFFFF7, 0x0000010E,
    0x000000BF, 0x00000068, 0x00000003, 0x0000005A,
    0x000000AA, 0x00000026, 0xFFFFFFEC, 0x0000005A,
    0x000000B8, 0x0000003F, 0xFFFFFFF4, 0x0000010E,
    0x000000C0, 0x00000030, 0x00000002, 0x0000005A,
    0x00000089, 0x00000070, 0x0000000E, 0x0000010E,
    0x000000BD, 0x00000049, 0xFFFFFFF6, 0x0000005A,
    0x0000008E, 0x00000042, 0xFFFFFFCE, 0x0000010E,
    0x000000A6, 0x00000061, 0x00000013, 0x0000010E,
    0x0000008B, 0x0000004F, 0x00000013, 0x0000005A,
    0x00000080, 0x00000029, 0xFFFFFFE5, 0x0000005A,
    0x000000A5, 0x00000055, 0xFFFFFFFE, 0x0000010E,
    0x00000098, 0x00000048, 0xFFFFFFF6, 0x0000010E,
    0x000000A9, 0x0000002B, 0x00000016, 0x0000005A,
    0x00000083, 0x00000058, 0xFFFFFFD6, 0x0000005A,
    0x000000B2, 0x00000052, 0xFFFFFFD5, 0x0000010E,
    0x000000B2, 0x00000057, 0x00000010, 0x0000005A,
    0x000000BB, 0x0000002F, 0xFFFFFFDD, 0x0000005A,
    0x0000008E, 0x00000045, 0xFFFFFFF6, 0x0000010E,
    0x000000A7, 0x00000066, 0x00000010, 0x0000005A,
    0x000000A6, 0x00000044, 0xFFFFFFEA, 0x0000010E,
    0x0000008D, 0x00000059, 0x00000010, 0x0000005A,
    0x000000B8, 0x00000050, 0xFFFFFFF4, 0x0000010E,
    0x000000C2, 0x00000047, 0xFFFFFFC7, 0x0000010E,
    0x00000087, 0x00000051, 0x0000000E, 0x0000005A,
    0x00000082, 0x00000072, 0xFFFFFFD5, 0x0000005A,
    0x000000A2, 0x0000003E, 0x00000003, 0x0000010E,
    0x00000086, 0x00000046, 0xFFFFFFE1, 0x0000010E,
    0x000000B9, 0x0000003F, 0xFFFFFFF0, 0x0000005A,
    0x000000BC, 0x00000032, 0xFFFFFFCF, 0x0000005A,
    0x000000A6, 0x0000003C, 0x00000013, 0x0000010E,
    0x0000008C, 0x00000025, 0x0000000E, 0x0000005A,
    0x000000B3, 0x0000006F, 0xFFFFFFEC, 0x0000005A,
    0x0000008C, 0x00000060, 0xFFFFFFDF, 0x0000010E,
    0x000000B3, 0x00000073, 0xFFFFFFCE, 0x0000005A,
    0x00000087, 0x00000057, 0xFFFFFFD2, 0x0000010E,
    0x000000AA, 0x0000002E, 0xFFFFFFD2, 0x0000005A,
    0x00000099, 0x0000004A, 0xFFFFFFE9, 0x0000010E,
    0x00000094, 0x0000006F, 0xFFFFFFD2, 0x0000005A,
    0x00000089, 0x0000006B, 0xFFFFFFD1, 0x0000005A,
    0x0000009E, 0x0000004E, 0x00000003, 0x0000010E,
    0x000000B3, 0x0000003A, 0xFFFFFFC8, 0x0000005A,
    0x000000B4, 0x0000004F, 0xFFFFFFD2, 0x0000010E,
    0x00000089, 0x00000047, 0xFFFFFFDF, 0x0000010E,
    0x00000091, 0x00000063, 0xFFFFFFD8, 0x0000005A,
    0x000000B4, 0x00000034, 0x00000012, 0x0000010E,
    0x00000092, 0x0000004C, 0x0000000B, 0x0000005A,
    0x000000A1, 0x00000035, 0xFFFFFFC5, 0x0000010E,
    0x0000008C, 0x00000074, 0xFFFFFFD8, 0x0000010E,
    0x0000008B, 0x00000060, 0xFFFFFFDA, 0x0000005A,
    0x00000075, 0x0000003A, 0xFFFFFFF9, 0x0000005A,
    0x00000098, 0x00000060, 0xFFFFFFD3, 0x0000010E,
    0x00000080, 0x0000002E, 0xFFFFFFF6, 0x0000010E,
    0x00000097, 0x0000004A, 0xFFFFFFF9, 0x0000010E,
    0x000000AF, 0x0000006F, 0x00000006, 0x0000005A,
    0x00000084, 0x0000006B, 0xFFFFFFE7, 0x0000010E,
    0x0000008E, 0x00000037, 0xFFFFFFED, 0x0000005A,
    0x000000A5, 0x0000006B, 0x0000000D, 0x0000010E,
    0x000000C1, 0x00000035, 0x00000006, 0x0000010E,
    0x0000009E, 0x00000066, 0x00000011, 0x0000005A,
    0x000000B7, 0x00000027, 0xFFFFFFDC, 0x0000005A,
    0x0000009A, 0x00000027, 0xFFFFFFDC, 0x0000010E,
    0x00000083, 0x00000071, 0xFFFFFFF4, 0x0000010E,
    0x000000A3, 0x00000040, 0xFFFFFFE4, 0x0000010E,
    0x000000A1, 0x00000037, 0x00000010, 0x0000005A,
    0x0000007F, 0x00000033, 0xFFFFFFF0, 0x0000010E,
    0x000000A1, 0x00000051, 0xFFFFFFFB, 0x0000005A,
    0x0000007D, 0x0000004C, 0xFFFFFFDB, 0x0000010E,
    0x0000008E, 0x00000046, 0xFFFFFFE5, 0x0000005A,
    0x00000080, 0x0000004D, 0x00000003, 0x0000005A,
    0x00000085, 0x00000048, 0x00000005, 0x0000010E,
    0x00000095, 0x00000045, 0x00000001, 0x0000005A,
    0x0000007D, 0x0000006F, 0xFFFFFFFA, 0x0000010E,
    0x000000A2, 0x0000006B, 0xFFFFFFD4, 0x0000010E,
    0x000000A8, 0x00000069, 0x00000004, 0x0000005A,
    0x00000096, 0x00000040, 0xFFFFFFCF, 0x0000010E,
    0x0000008E, 0x0000002C, 0x00000008, 0x0000005A,
    0x00000078, 0x0000002F, 0xFFFFFFE0, 0x0000010E,
    0x000000B6, 0x00000074, 0xFFFFFFF3, 0x0000010E,
    0x000000B2, 0x00000054, 0xFFFFFFE3, 0x0000005A,
    0x00000087, 0x00000058, 0xFFFFFFF1, 0x0000005A,
    0x00000093, 0x00000040, 0xFFFFFFD1, 0x0000010E,
    0x00000075, 0x00000073, 0xFFFFFFDA, 0x0000010E,
    0x000000A6, 0x0000005A, 0xFFFFFFD5, 0x0000010E,
    0x00000079, 0x00000057, 0xFFFFFFDA, 0x0000005A,
    0x0000009D, 0x00000059, 0x00000005, 0x0000010E,
    0x00000084, 0x00000053, 0xFFFFFFC4, 0x0000005A,
    0x0000008C, 0x00000055, 0x00000008, 0x0000010E,
    0x00000080, 0x00000036, 0xFFFFFFD8, 0x0000010E,
    0x000000A4, 0x0000004E, 0xFFFFFFDF, 0x0000005A,
    0x00000084, 0x00000063, 0xFFFFFFF2, 0x0000005A,
    0x000000AA, 0x00000031, 0xFFFFFFD9, 0x0000005A,
    0x00000087, 0x00000031, 0xFFFFFFD9, 0x0000005A,
    0x0000007E, 0x00000018, 0x00000019, 0x0000010E,
    0x0000008C, 0x00000018, 0x00000014, 0x0000010E,
    0x000000BE, 0x00000018, 0x00000019, 0x0000010E,
};

// TODO: type
u32 suifudata_after[400] = {
    0x0000027D, 0x00000046, 0xFFFFFF93, 0x0000010E,
    0x000002B6, 0x00000046, 0xFFFFFF75, 0x0000010E,
    0x000002B0, 0x00000046, 0xFFFFFF7E, 0x0000010E,
    0x00000290, 0x00000046, 0xFFFFFF76, 0x0000005A,
    0x00000287, 0x00000046, 0xFFFFFF7F, 0x0000010E,
    0x0000027D, 0x00000046, 0xFFFFFF85, 0x0000010E,
    0x000002A2, 0x00000046, 0xFFFFFF77, 0x0000005A,
    0x0000028F, 0x00000046, 0xFFFFFF87, 0x0000005A,
    0x0000029F, 0x00000046, 0xFFFFFF89, 0x0000010E,
    0x000002B2, 0x00000046, 0xFFFFFF92, 0x0000005A,
    0x000002BE, 0x00000046, 0xFFFFFF94, 0x0000005A,
    0x000002B0, 0x00000046, 0xFFFFFF9D, 0x0000005A,
    0x000002A9, 0x00000046, 0xFFFFFFA2, 0x0000010E,
    0x0000027B, 0x00000046, 0xFFFFFF9D, 0x0000005A,
    0x0000028F, 0x00000046, 0xFFFFFF94, 0x0000010E,
    0x00000293, 0x00000046, 0xFFFFFFA0, 0x0000005A,
    0x000002AF, 0x00000032, 0xFFFFFFCE, 0x0000010E,
    0x00000287, 0x00000032, 0xFFFFFFDD, 0x0000010E,
    0x0000026F, 0x00000032, 0xFFFFFFD9, 0x0000010E,
    0x00000271, 0x00000032, 0xFFFFFFC8, 0x0000005A,
    0x00000276, 0x00000032, 0xFFFFFFBC, 0x0000005A,
    0x0000027E, 0x00000032, 0xFFFFFFAD, 0x0000010E,
    0x00000292, 0x00000032, 0xFFFFFFB0, 0x0000010E,
    0x0000028E, 0x00000032, 0xFFFFFFC2, 0x0000005A,
    0x00000293, 0x00000032, 0xFFFFFFD8, 0x0000005A,
    0x000002A0, 0x00000032, 0xFFFFFFD2, 0x0000010E,
    0x000002A5, 0x00000032, 0xFFFFFFC7, 0x0000005A,
    0x000002A2, 0x00000032, 0xFFFFFFB9, 0x0000005A,
    0x000002A8, 0x00000032, 0xFFFFFFAE, 0x0000010E,
    0x000002B5, 0x00000032, 0xFFFFFFBF, 0x0000010E,
    0x0000029B, 0x00000032, 0xFFFFFFC1, 0x0000005A,
    0x00000287, 0x00000032, 0xFFFFFFBA, 0x0000010E,
    0x0000027F, 0x00000032, 0xFFFFFFBE, 0x0000005A,
    0x00000286, 0x00000032, 0xFFFFFFD3, 0x0000010E,
    0x000002B7, 0x00000032, 0xFFFFFFAC, 0x0000005A,
    0x000002AD, 0x00000032, 0xFFFFFFE1, 0x0000005A,
    0x000002BB, 0x00000032, 0xFFFFFFCA, 0x0000010E,
    0x0000033A, 0x00000032, 0xFFFFFFB0, 0x0000010E,
    0x00000327, 0x00000032, 0xFFFFFFAD, 0x0000010E,
    0x0000032C, 0x00000032, 0xFFFFFFE2, 0x0000005A,
    0x00000343, 0x00000032, 0xFFFFFFE2, 0x0000005A,
    0x0000032B, 0x00000032, 0xFFFFFFCE, 0x0000010E,
    0x0000034D, 0x00000032, 0xFFFFFFD1, 0x0000005A,
    0x00000335, 0x00000032, 0xFFFFFFD2, 0x0000005A,
    0x00000335, 0x00000032, 0xFFFFFFD2, 0x0000010E,
    0x00000336, 0x00000032, 0xFFFFFFB8, 0x0000010E,
    0x00000343, 0x00000032, 0xFFFFFFB3, 0x0000010E,
    0x00000323, 0x00000032, 0xFFFFFFBD, 0x0000005A,
    0x00000325, 0x00000032, 0xFFFFFFDC, 0x0000010E,
    0x00000331, 0x00000032, 0xFFFFFFC8, 0x0000010E,
    0x00000342, 0x00000032, 0xFFFFFFBE, 0x0000010E,
    0x0000033D, 0x00000032, 0xFFFFFFA7, 0x0000005A,
    0x00000332, 0x00000032, 0xFFFFFFA8, 0x0000010E,
    0x0000032D, 0x00000032, 0xFFFFFFB3, 0x0000005A,
    0x000002BE, 0x00000032, 0xFFFFFFBD, 0x0000010E,
    0x000002BE, 0x00000032, 0xFFFFFFBD, 0x0000010E,
    0x000002D6, 0x00000032, 0xFFFFFFC1, 0x0000010E,
    0x000002EB, 0x00000032, 0xFFFFFFBD, 0x0000005A,
    0x00000301, 0x00000032, 0xFFFFFFC2, 0x0000010E,
    0x0000030C, 0x00000032, 0xFFFFFFCA, 0x0000005A,
    0x0000030D, 0x00000032, 0xFFFFFFB1, 0x0000010E,
    0x000002F8, 0x00000032, 0xFFFFFFBE, 0x0000005A,
    0x000002DC, 0x00000032, 0xFFFFFFBB, 0x0000005A,
    0x0000031C, 0x00000032, 0xFFFFFFB2, 0x0000010E,
    0x00000304, 0x00000032, 0xFFFFFFB3, 0x0000005A,
    0x000002F5, 0x00000032, 0xFFFFFFB5, 0x0000010E,
    0x000002E4, 0x00000032, 0xFFFFFFC0, 0x0000010E,
    0x000002D3, 0x00000032, 0xFFFFFFD8, 0x0000005A,
    0x000002C1, 0x00000032, 0xFFFFFFD5, 0x0000005A,
    0x0000031C, 0x00000032, 0xFFFFFFD3, 0x0000005A,
    0x00000312, 0x00000032, 0xFFFFFFE5, 0x0000010E,
    0x000002FA, 0x00000032, 0xFFFFFFE1, 0x0000005A,
    0x000002F2, 0x00000032, 0xFFFFFFC7, 0x0000010E,
    0x000002ED, 0x00000032, 0xFFFFFFDA, 0x0000005A,
    0x000002D6, 0x00000032, 0xFFFFFFAE, 0x0000010E,
    0x00000308, 0x00000032, 0xFFFFFFC1, 0x0000010E,
    0x00000317, 0x00000032, 0xFFFFFFC7, 0x0000005A,
    0x000002E2, 0x00000032, 0xFFFFFFAD, 0x0000005A,
    0x00000307, 0x00000032, 0xFFFFFFAA, 0x0000010E,
    0x000002ED, 0x00000032, 0xFFFFFFAB, 0x0000005A,
    0x000002E2, 0x00000032, 0xFFFFFFDF, 0x0000005A,
    0x000002D0, 0x00000032, 0xFFFFFFC7, 0x0000010E,
    0x000002C8, 0x00000032, 0xFFFFFFD9, 0x0000005A,
    0x000002C8, 0x00000032, 0xFFFFFFB5, 0x0000005A,
    0x000002C8, 0x00000032, 0xFFFFFFC2, 0x0000010E,
    0x000002C3, 0x00000032, 0xFFFFFFE2, 0x0000005A,
    0x000002D6, 0x00000032, 0xFFFFFFE3, 0x0000010E,
    0x000002DD, 0x00000032, 0xFFFFFFD5, 0x0000005A,
    0x000002EB, 0x00000032, 0xFFFFFFB4, 0x0000005A,
    0x000002FE, 0x00000032, 0xFFFFFFAA, 0x0000010E,
    0x00000311, 0x00000032, 0xFFFFFFA9, 0x0000005A,
    0x00000314, 0x00000032, 0xFFFFFFBA, 0x0000010E,
    0x00000310, 0x00000032, 0xFFFFFFDC, 0x0000010E,
    0x000002FB, 0x00000032, 0xFFFFFFD8, 0x0000005A,
    0x00000306, 0x00000032, 0xFFFFFFE2, 0x0000010E,
    0x000002A9, 0x00000032, 0xFFFFFFB5, 0x0000005A,
    0x000002CA, 0x00000032, 0xFFFFFFB2, 0x0000005A,
    0x00000000, 0xFFFFFC18, 0x00000000, 0x00000000,
    0x00000000, 0xFFFFFC18, 0x00000000, 0x00000000,
    0x00000000, 0xFFFFFC18, 0x00000000, 0x00000000,
};

EVT_BEGIN(watarikiri_suifu_init)
    CALL(evt_nannpc_init, PTR(ext_entry_data), 0, 0, 0, 101)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    CALL(evt_nannpc_set_subfunc, 2, PTR(nanNPCOption))
    SET_VALUE(LW(0), 0)
    LOOP(95)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        SET_INT_READ_ADDR(PTR(suifudata_after))
        SET_VALUE(LW(10), LW(0))
        MULTIPLY(LW(10), 4)
        READ_INT_INDEXED(LW(1), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(2), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(3), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(4), LW(10))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 4)
        CALL(evt_nannpc_set_size, LW(0), 30, 8)
        IF_INT_EQUAL(GSW(0), 254)
            SET_VALUE(LW(4), 90)
        END_IF
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        ADD(LW(0), 1)
    END_LOOP
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 100)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 6)
        CALL(evt_nannpc_set_size, LW(0), 30, 15)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
            CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(ukishima_watari_run)
    CALL(evt_nannpc_set_work, LW(0), 0, 0)
    CALL(evt_nannpc_set_dir, LW(0), 90)
    CALL(evt_sub_random, 20, LW(10))
    ADD(LW(10), 160)
    CALL(evt_sub_random, 10, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 37, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_sub_random, 15, LW(10))
    ADD(LW(10), 250)
    CALL(evt_sub_random, 20, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 25, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_sub_random, 15, LW(10))
    ADD(LW(10), 300)
    CALL(evt_sub_random, 20, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 25, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_sub_random, 15, LW(10))
    ADD(LW(10), 395)
    CALL(evt_sub_random, 20, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 25, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_sub_random, 20, LW(10))
    ADD(LW(10), 490)
    CALL(evt_sub_random, 20, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 25, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_sub_random, 15, LW(10))
    ADD(LW(10), 550)
    CALL(evt_sub_random, 20, LW(12))
    ADD(LW(12), -10)
    CALL(evt_sub_random, 30, LW(13))
    ADD(LW(13), 30)
    CALL(evt_sub_random, 300, LW(14))
    ADD(LW(14), 300)
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), LW(10), 25, LW(12), LW(14), LW(13))
    CALL(evt_sub_random, 30, LW(10))
    ADD(LW(10), 50)
    WAIT_MS(LW(10))
    CALL(evt_nannpc_get_position, LW(0), LW(7), LW(8), LW(9))
    CALL(evt_nannpc_jump_position, LW(0), 625, 50, 0, 250, 50)
    SWITCH(LW(0))
        CASE_EQUAL(97)
        CALL(evt_nannpc_set_position, 97, 0, -1000, 0)
        CALL(evt_nannpc_ppflag_onoff, 97, 0, 4)
        CALL(evt_npc_set_position, STRING("水夫C"), 825, 100, -125)
        CALL(evt_npc_set_ry, STRING("水夫C"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("水夫C"), 1073741856)
        CASE_EQUAL(98)
        CALL(evt_nannpc_set_position, 98, 0, -1000, 0)
        CALL(evt_nannpc_ppflag_onoff, 98, 0, 4)
        CALL(evt_npc_set_position, STRING("水夫D"), 775, 50, 50)
        CALL(evt_npc_set_ry, STRING("水夫D"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("水夫D"), 1073741856)
        CASE_EQUAL(99)
        CALL(evt_nannpc_set_position, 99, 0, -1000, 0)
        CALL(evt_nannpc_ppflag_onoff, 99, 0, 4)
        CALL(evt_npc_set_position, STRING("水夫E"), 750, 50, -25)
        CALL(evt_npc_set_ry, STRING("水夫E"), 270)
        CALL(evt_npc_flag_onoff, 0, STRING("水夫E"), 1073741856)
        CASE_DEFAULT
        SET_INT_READ_ADDR(PTR(suifudata_after))
        SET_VALUE(LW(10), LW(0))
        MULTIPLY(LW(10), 4)
        READ_INT_INDEXED(LW(1), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(2), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(3), LW(10))
        ADD(LW(10), 1)
        READ_INT_INDEXED(LW(4), LW(10))
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(ukishima_watari)
    CALL(evt_cam3d_evt_set, 160, 174, 527, 160, 79, -14, 500, 11)
    WAIT_MS(1000)
    CALL(evt_nannpc_get_position, 99, LW(1), LW(2), LW(3))
    CALL(evt_nannpc_set_dir, 99, 90)
    CALL(evt_msg_toge, 2, LW(1), LW(2), LW(3))
    CALL(evt_msg_print, 0, STRING("stg5_dou_14"), 0, 0)
    SET_VALUE(LW(0), 0)
    LOOP(100)
        CALL(evt_nannpc_set_dir, LW(0), 90)
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_nannpc_get_position, 98, LW(1), LW(2), LW(3))
    CALL(evt_nannpc_set_dir, 98, 90)
    CALL(evt_msg_toge, 2, LW(1), LW(2), LW(3))
    CALL(evt_msg_print, 0, STRING("stg5_dou_15"), 0, 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KINOPIO_MOVE1_2"), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 110)
    LOOP(0)
        SET_VALUE(LW(0), 0)
        LOOP(100)
            CALL(evt_nannpc_get_work, LW(0), 1, LW(4))
            CALL(evt_nannpc_get_position, LW(0), LW(10), LW(11), LW(12))
            IF_INT_GREATER_OR_EQUAL(LW(11), LW(6))
                IF_INT_EQUAL(LW(4), 0)
                    CALL(evt_nannpc_set_work, LW(0), 1, 1)
                    CALL(evt_sub_random, 50, LW(10))
                    ADD(LW(10), 100)
                    WAIT_MS(LW(10))
                    SPAWN_EVENT(ukishima_watari_run)
                    ADD(LW(5), 1)
                END_IF
            END_IF
            IF_INT_EQUAL(LW(5), 100)
                GOTO(100)
            END_IF
            ADD(LW(0), 1)
        END_LOOP
        ADD(LW(6), -10)
    END_LOOP
    LABEL(100)
    WAIT_MS(1500)
    CALL(evt_nannpc_set_subfunc, 2, PTR(nanNPCOption))
    SET_VALUE(LW(0), 0)
    LOOP(95)
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 4)
        CALL(evt_nannpc_set_size, LW(0), 30, 8)
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    SET_VALUE(GSWF(2992), 1)
    CALL(evt_hitobj_onoff, STRING("fune_atari"), 1, 0)
    CALL(evt_mario_key_onoff, 1)
    SPAWN_EVENT(suifu_orei)
    RETURN
EVT_END()

EVT_BEGIN(ukishima_check)
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_NOT_EQUAL(LW(0), 2)
        CALL(evt_cam3d_evt_off, 500, 11)
        WAIT_MS(1000)
    ELSE
        CALL(evt_mario_key_onoff, 0)
        CALL(evt_snd_bgmoff, 1024)
        CALL(evt_snd_bgmon, 1, STRING("BGM_FF_ENV_SUCCESS1"))
        WAIT_MS(2000)
        CALL(evt_snd_bgmoff, 513)
        CALL(evt_snd_bgmon, 288, 0)
        SPAWN_EVENT(ukishima_watari)
    END_IF
    RETURN
EVT_END()

static EVT_BEGIN(ukishima_apper)
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_APPEAR1"), LW(10), LW(11), LW(12), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(2), LW(10), LW(11), LW(12))
        CALL(evt_eff64, STRING(""), STRING("ripple_n64"), 0, LW(10), 5, LW(12), 0, 0, 0, 0, 0, 0, 0, 0)
        WAIT_MS(400)
        CALL(evt_eff64, STRING(""), STRING("ripple_n64"), 0, LW(10), 5, LW(12), 0, 0, 0, 0, 0, 0, 0, 0)
    END_INLINE_EVENT
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(2), 1)
    CALL(evt_hitobj_onoff, LW(3), 1, 1)
    CALL(evt_hit_bind_mapobj, LW(3), LW(2))
    CALL(evt_sub_intpl_msec_init, 1, -100, 0, 800)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -10, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -10, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -5, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -5, 0, 200)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, 0, -1, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_sub_intpl_msec_init, 11, -1, 0, 100)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
        CALL(evt_mapobj_trans, 1, LW(2), 0, LW(0), 0)
        CALL(evt_hit_bind_update, LW(3))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(ukishima_init1)
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_LESS(LW(0), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n01"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n01"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n02"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n02"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ukishima_part1)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 280, 174, 527, 280, 79, -14, 500, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(2), STRING("S_taru_n01"))
    SET_VALUE(LW(3), STRING("A_taru_n01"))
    SPAWN_EVENT(ukishima_apper)
    BEGIN_INLINE_EVENT
        WAIT_MS(700)
        SET_VALUE(LW(2), STRING("S_taru_n02"))
        SET_VALUE(LW(3), STRING("A_taru_n02"))
        SPAWN_EVENT(ukishima_apper)
    END_INLINE_EVENT
    WAIT_MS(2500)
    SPAWN_EVENT(pukapuka_watari_init01)
    WAIT_MS(1000)
    CALL_CHILD(ukishima_check)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(ukishima_init2)
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_LESS(LW(0), 2)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n03"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n03"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n04"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n04"), 1, 0)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(ukishima_part2)
    CALL(evt_mario_key_onoff, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_set, 525, 174, 527, 525, 79, -14, 500, 11)
    WAIT_MS(1000)
    SET_VALUE(LW(2), STRING("S_taru_n03"))
    SET_VALUE(LW(3), STRING("A_taru_n03"))
    SPAWN_EVENT(ukishima_apper)
    BEGIN_INLINE_EVENT
        WAIT_MS(700)
        SET_VALUE(LW(2), STRING("S_taru_n04"))
        SET_VALUE(LW(3), STRING("A_taru_n04"))
        CALL_CHILD(ukishima_apper)
    END_INLINE_EVENT
    WAIT_MS(2500)
    SPAWN_EVENT(pukapuka_watari_init02)
    WAIT_MS(1000)
    CALL_CHILD(ukishima_check)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_dou_00011744)
    SET_VALUE(GSWF(2983), 1)
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_EQUAL(LW(0), 1)
        SPAWN_EVENT(ukishima_part1)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        SPAWN_EVENT(ukishima_part2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_dou_000117a0)
    SET_VALUE(GSWF(2984), 1)
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_EQUAL(LW(0), 1)
        SPAWN_EVENT(ukishima_part1)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        SPAWN_EVENT(ukishima_part2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(torinoko_suifu_pukapuka)
    LOOP(0)
        SET_VALUE(LW(0), 0)
        LOOP(100)
            CALL(evt_nannpc_get_work, LW(0), 0, LW(10))
            IF_INT_EQUAL(LW(10), 1)
                SET_FLOAT_VALUE(LW(10), GW(0))
                CALL(evt_nannpc_set_hosei_position, LW(0), 0, LW(10), 0)
                CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
                IF_INT_LESS_OR_EQUAL(LW(3), 0)
                    SET_FLOAT_VALUE(LW(1), FLOAT(39.0f))
                ELSE
                    SET_FLOAT_VALUE(LW(1), FLOAT(26.0f))
                END_IF
                ADD_FLOAT(LW(1), LW(10))
                CALL(evt_nannpc_set_shadow_position, LW(0), LW(1))
            ELSE
                CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
                CALL(evt_nannpc_set_shadow_position, LW(0), LW(2))
            END_IF
            ADD(LW(0), 1)
        END_LOOP
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(_suifu_sakebu)
    LOOP(0)
        WAIT_MS(700)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 115, 130, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_01"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(600)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 105, 80, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_03"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(600)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 123, 30, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_05"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(500)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 210, 135, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_02"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(800)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 225, 85, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_06"), 0, 0)
        END_INLINE_EVENT
        WAIT_MS(400)
        BEGIN_INLINE_EVENT
            CALL(evt_msg_toge, 2, 195, 37, 0)
            CALL(evt_msg_print, 0, STRING("stg5_dou_13_04"), 0, 0)
        END_INLINE_EVENT
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(torinoko_suifu_sakebu)
    LOOP(0)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                IF_INT_LESS_OR_EQUAL(LW(0), 300)
                    IF_INT_GREATER_OR_EQUAL(LW(1), -2)
                        IF_INT_LESS_OR_EQUAL(LW(1), 2)
                            IF_INT_GREATER_OR_EQUAL(LW(2), -140)
                                IF_INT_LESS_OR_EQUAL(LW(2), 140)
                                    BREAK_LOOP
                                END_IF
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(GSWF(2983), 1)
                IF_INT_EQUAL(GSWF(2984), 1)
                    RETURN
                END_IF
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(_suifu_sakebu), LW(10))
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
            SET_VALUE(LF(0), 1)
            IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                IF_INT_LESS_OR_EQUAL(LW(0), 300)
                    IF_INT_GREATER_OR_EQUAL(LW(1), -2)
                        IF_INT_LESS_OR_EQUAL(LW(1), 2)
                            IF_INT_GREATER_OR_EQUAL(LW(2), -140)
                                IF_INT_LESS_OR_EQUAL(LW(2), 140)
                                    SET_VALUE(LF(0), 0)
                                END_IF
                            END_IF
                        END_IF
                    END_IF
                END_IF
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(10))
        WAIT_MS(1000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(torinoko_suifu_init)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫C"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫D"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫E"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_change_interrupt, STRING("モニー"), 6, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_npc_change_interrupt, STRING("ピートン"), 6, 0)
    CALL(evt_nannpc_init, PTR(ext_entry_data), 0, 0, 0, 101)
    CALL(evt_nannpc_set_subfunc, 1, PTR(nannpc_ext_main_sub_fast))
    SPAWN_EVENT(torinoko_suifu_pukapuka)
    SET_INT_READ_ADDR(PTR(suifudata))
    SET_VALUE(LW(0), 0)
    LOOP(100)
        READ_INT3(LW(1), LW(2), LW(3))
        READ_INT(LW(4))
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_set_func, LW(0), 0)
        CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
        SWITCH(LW(0))
            CASE_EQUAL(96)
            CALL(evt_nannpc_flag_onoff, LW(0), 0, 2)
            CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
            CASE_EQUAL(95)
            CALL(evt_nannpc_flag_onoff, LW(0), 0, 2)
            CALL(evt_nannpc_ppflag_onoff, LW(0), 0, 4)
        END_SWITCH
        CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_animtbl))
        CALL(evt_nannpc_set_dir, LW(0), LW(4))
        CALL(evt_nannpc_set_work, LW(0), 0, 1)
        CALL(evt_nannpc_set_work, LW(0), 1, 0)
        ADD(LW(0), 1)
    END_LOOP
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), 100)
        CALL(evt_nannpc_entry, LW(0))
        CALL(evt_nannpc_flag_onoff, LW(0), 0, 6)
        CALL(evt_nannpc_set_size, LW(0), 30, 15)
        LOOP(0)
            CALL(evt_mario_get_pos, 0, LW(1), LW(2), LW(3))
            CALL(evt_nannpc_set_position, LW(0), LW(1), LW(2), LW(3))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT_GET_ID(GW(10))
        LOOP(0)
            CALL(evt_nannpc_get_position, 96, LW(1), LW(2), LW(3))
            CALL(evt_npc_set_position, STRING("ピートン"), LW(1), LW(2), LW(3))
            CALL(evt_nannpc_get_position, 95, LW(1), LW(2), LW(3))
            CALL(evt_npc_set_position, STRING("モニー"), LW(1), LW(2), LW(3))
            WAIT_FRAMES(1)
        END_LOOP
    END_INLINE_EVENT
    RETURN
EVT_END()

EVT_BEGIN(torinoko_suifu)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_npc_set_position, STRING("水夫C"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("水夫D"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("水夫E"), 0, -1000, 0)
    CALL(evt_seq_wait, 2)
    WAIT_MS(500)
    CALL(evt_msg_toge, 2, -400, 60, 30)
    CALL(evt_msg_print, 0, STRING("stg5_dou_11"), 0, 0)
    CALL(evt_cam3d_evt_set, 160, 180, 559, 160, 85, 17, 500, 11)
    BEGIN_INLINE_EVENT
        WAIT_MS(1800)
        CALL(evt_msg_toge, 2, 115, 130, 0)
        CALL(evt_msg_print, 0, STRING("small_00"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1200)
        CALL(evt_msg_toge, 2, 105, 80, 0)
        CALL(evt_msg_print, 0, STRING("small_01"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(900)
        CALL(evt_msg_toge, 2, 123, 30, 0)
        CALL(evt_msg_print, 0, STRING("small_02"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(600)
        CALL(evt_msg_toge, 2, 210, 135, 0)
        CALL(evt_msg_print, 0, STRING("small_03"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(1500)
        CALL(evt_msg_toge, 2, 225, 85, 0)
        CALL(evt_msg_print, 0, STRING("small_04"), 0, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        WAIT_MS(2100)
        CALL(evt_msg_toge, 2, 195, 37, 0)
        CALL(evt_msg_print, 0, STRING("small_05"), 0, 0)
    END_INLINE_EVENT
    WAIT_MS(4000)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_msg_toge, 2, -400, 60, 30)
    CALL(evt_msg_print, 0, STRING("stg5_dou_13"), 0, 0)
    CALL(evt_mario_key_onoff, 1)
    SET_VALUE(GSWF(2982), 1)
    RETURN
EVT_END()

EVT_BEGIN(suifu_orei)
    CALL(evt_npc_set_position, STRING("水夫C"), 825, 100, -125)
    CALL(evt_npc_set_ry, STRING("水夫C"), 270)
    CALL(evt_npc_set_position, STRING("水夫D"), 775, 50, 50)
    CALL(evt_npc_set_ry, STRING("水夫D"), 270)
    CALL(evt_npc_set_position, STRING("水夫E"), 750, 50, -25)
    CALL(evt_npc_set_ry, STRING("水夫E"), 270)
    LOOP(0)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_name_hitobj_ride, LW(4))
        IF_STR_EQUAL(LW(4), STRING("mod_01"))
            IF_INT_GREATER_OR_EQUAL(LW(0), 630)
                BREAK_LOOP
            END_IF
            IF_INT_LESS_OR_EQUAL(LW(2), -20)
                IF_INT_GREATER_OR_EQUAL(LW(2), -140)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    DELETE_EVENT(GW(10))
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_set_dir, 90, 200, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_party_wait_landon, 0)
        CALL(evt_party_stop, 0)
        CALL(evt_party_set_dir, 0, 90, 200)
    END_INLINE_EVENT
    CALL(evt_snd_bgmoff_f, 512, 3000)
    CALL(evt_set_dir_to_target, STRING("水夫D"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_16"), 0, STRING("水夫D"))
    CALL(evt_set_dir_to_target, STRING("水夫E"), STRING("mario"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_01"), 0, STRING("水夫E"))
    SET_INT_READ_ADDR(PTR(suifudata_after))
    SET_VALUE(LW(10), 95)
    MULTIPLY(LW(10), 4)
    READ_INT_INDEXED(LW(1), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(2), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(3), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(4), LW(10))
    CALL(evt_npc_set_position, STRING("モニー"), LW(1), LW(2), LW(3))
    CALL(evt_eff_fukidashi, 1, STRING(""), STRING("モニー"), 0, 0, 0, 0, 0, 0, 0, 60)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_02"), 0, STRING("モニー"))
    CALL(evt_npc_move_position, STRING("モニー"), 700, 0, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_03"), 0, STRING("モニー"))
    SET_INT_READ_ADDR(PTR(suifudata_after))
    SET_VALUE(LW(10), 96)
    MULTIPLY(LW(10), 4)
    READ_INT_INDEXED(LW(1), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(2), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(3), LW(10))
    ADD(LW(10), 1)
    READ_INT_INDEXED(LW(4), LW(10))
    CALL(evt_npc_set_position, STRING("ピートン"), LW(1), LW(2), LW(3))
    CALL(evt_npc_move_position, STRING("ピートン"), 750, 0, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("ピートン"), 270)
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_04"), 0, STRING("モニー"))
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_05"), 0, STRING("モニー"))
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_NO1"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_N_B"))
    WAIT_MS(1000)
    CALL(evt_mario_set_pose, STRING("M_S_1"))
    WAIT_MS(500)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_3"), LW(0), LW(1), LW(2), 0)
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_16_06"), 0, STRING("モニー"))
    CALL(evt_npc_flag_onoff, 0, STRING("モニー"), 1073741856)
    CALL(evt_npc_change_interrupt, STRING("モニー"), 6, PTR(mony_talk))
    CALL(evt_npc_flag_onoff, 0, STRING("ピートン"), 1073741856)
    CALL(evt_npc_change_interrupt, STRING("ピートン"), 6, PTR(peton_talk))
    SET_VALUE(GSW(0), 250)
    CALL(evt_party_run, 0)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(escapeway_event)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_wait, 65536)
    CALL(evt_mario_normalize)
    WAIT_MS(500)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_01"), 0, STRING("水夫D"))
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_VOICE_MARIO_HAND_UP1_4"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    CALL(evt_mario_set_pose, STRING("M_I_2"))
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_02"), 0, STRING("水夫D"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_03"), 0, STRING("水夫E"))
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_04"), 0, STRING("水夫C"))
    CALL(evt_cam3d_evt_set, 775, 220, 331, 775, 150, -63, 500, 11)
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("水夫C"), 270)
    WAIT_MS(300)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_05"), 0, STRING("水夫C"))
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(1000)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_06"), 0, STRING("水夫E"))
    SET_VALUE(GSW(0), 255)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_dou_000128c4)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫C"), 1073741856)
    BEGIN_INLINE_EVENT
        CALL(evt_party_get_pos, 0, LW(0), LW(1), LW(2))
        IF_INT_LESS(LW(1), 100)
            RETURN
        END_IF
        CALL(evt_party_stop, 0)
        CALL(evt_party_move_pos2, 0, 712, -126, FLOAT(120.0f))
        CALL(evt_party_set_dir, 0, 90, 200)
        CALL(evt_party_run, 0)
    END_INLINE_EVENT
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS(LW(1), 100)
        RETURN
    END_IF
    CALL(evt_mario_mov_pos2, 730, -120, FLOAT(120.0f))
    CALL(evt_mario_set_dir, 90, 200, 0)
    RETURN
EVT_END()

static EVT_BEGIN(bombwall_bomb)
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(200)
        CALL(evt_mapobj_get_position, STRING("S_ana_before"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_OFF_WALL_BREAK1"), LW(0), LW(1), LW(2), 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_mario_wait_landon)
        CALL(evt_mario_normalize)
        SPAWN_EVENT(unk_evt_dou_000128c4)
    END_INLINE_EVENT
    CALL(evt_img_entry, STRING("capture_img"))
    CALL(evt_img_set_paper, STRING("capture_img"), STRING("OFF_d_bom_ver2"))
    CALL(evt_offscreen_entry, STRING("offscreen"))
    CALL(evt_mapobj_set_offscreen, 1, STRING("S_ana_before"), STRING("offscreen"))
    CALL(evt_offscreen_get_boundingbox2, STRING("offscreen"), LW(10), LW(11), LW(12), LW(13))
    SET_VALUE(LF(0), 0)
    IF_INT_EQUAL(LW(12), 0)
        IF_INT_EQUAL(LW(13), 0)
            SET_VALUE(LF(0), 1)
        END_IF
    END_IF
    IF_INT_EQUAL(LF(0), 0)
        CALL(evt_img_alloc_capture, STRING("capture_img"), 0, STRING("offscreen"), 1, LW(10), LW(11), LW(12), LW(13))
        CALL(evt_img_clear_virtual_point, STRING("capture_img"))
        CALL(evt_img_set_paper_anim, STRING("capture_img"), STRING("A_1"))
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after"), 1)
        CALL(evt_img_wait_animend, STRING("capture_img"))
    ELSE
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after"), 1)
    END_IF
    CALL(evt_mapobj_clear_offscreen, 1, STRING("S_ana_before"))
    CALL(evt_img_free_capture, STRING("capture_img"), 0)
    CALL(evt_img_free_capture, STRING("capture_img"), 1)
    CALL(evt_img_onoff, STRING("capture_img"), 0)
    CALL(evt_img_release, STRING("capture_img"))
    CALL(evt_offscreen_delete, STRING("offscreen"))
    LOOP(0)
        CALL(evt_mario_party_use_check, LW(0))
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_mario_wait_landon)
    SPAWN_EVENT(bombwall_after)
    SET_VALUE(LW(0), STRING("n_bero"))
    CALL_CHILD(bero_case_switch_on)
    CALL(evt_hitobj_attr_onoff, 1, 1, STRING("n_bero"), 4)
    SET_VALUE(GSW(0), 256)
    WAIT_FRAMES(2)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_exit_case_evt)
    RETURN
EVT_END()

static EVT_BEGIN(bombwall_init)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_after"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_ana_before1"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hibiato"), 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_hibimae"), 1)
    SWITCH(GSW(0))
        CASE_LESS_OR_EQUAL(253)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hibimae"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before"), 1)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("n_bero"), 4)
        CASE_BETWEEN(254, 255)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hibiato"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_before1"), 1)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_off)
        CALL(evt_hitobj_attr_onoff, 1, 0, STRING("n_bero"), 4)
        CALL(evt_paper_entry, STRING("OFF_d_bom_ver2"))
        CALL(evt_run_case_evt, 15, 1, STRING("n_bero"), 0, PTR(bombwall_bomb), 0)
        CASE_GREATER_OR_EQUAL(256)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_hibiato"), 1)
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ana_after"), 1)
        SET_VALUE(LW(0), STRING("n_bero"))
        CALL_CHILD(bero_case_switch_on)
        CALL(evt_hitobj_attr_onoff, 1, 1, STRING("n_bero"), 4)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(bombwall_suifuout_secheck)
    LOOP(0)
        SET_VALUE(LW(0), 0)
        SET_VALUE(LW(11), 1)
        SET_VALUE(LW(12), 1)
        LOOP(0)
            CALL(evt_nannpc_get_work, LW(0), 0, LW(10))
            IF_INT_EQUAL(LW(10), 1)
                SET_VALUE(LW(11), 1)
                SET_VALUE(LW(14), LW(0))
            END_IF
            ADD(LW(0), 1)
            IF_INT_EQUAL(LW(0), 95)
                BREAK_LOOP
            END_IF
        END_LOOP
        IF_INT_EQUAL(LW(11), 1)
            IF_INT_EQUAL(LW(12), 0)
                CALL(evt_nannpc_get_position, LW(14), LW(0), LW(1), LW(2))
                CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_KINOPIO_MOVE1"), LW(0), LW(1), LW(2), LW(13))
                SET_VALUE(LW(12), 1)
            END_IF
        ELSE
            IF_INT_EQUAL(LW(12), 1)
                CALL(evt_snd_sfxoff, LW(13))
                SET_VALUE(LW(12), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(bombwall_suifuout)
    CALL(evt_nannpc_set_work, LW(0), 0, 0)
    CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
    IF_INT_EQUAL(LW(2), 50)
        IF_INT_GREATER_OR_EQUAL(LW(1), 710)
            GOTO(10)
        ELSE
            GOTO(20)
        END_IF
    ELSE
        GOTO(30)
    END_IF
    LABEL(10)
    CALL(evt_nannpc_set_dir, LW(0), 270)
    CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_random, 50, LW(1))
    ADD(LW(1), 630)
    CALL(evt_nannpc_set_work, LW(0), 0, 1)
    CALL(evt_nannpc_move_position2, LW(0), LW(1), LW(2), LW(3), FLOAT(240.0f))
    CALL(evt_nannpc_set_work, LW(0), 0, 0)
    CALL(evt_sub_random, 150, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    LABEL(20)
    CALL(evt_nannpc_set_dir, LW(0), 90)
    CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_random, 25, LW(3))
    ADD(LW(3), -125)
    CALL(evt_sub_random, 200, LW(10))
    ADD(LW(10), 400)
    CALL(evt_sub_random, 20, LW(11))
    ADD(LW(11), 30)
    CALL(evt_nannpc_jump_position, LW(0), LW(1), 70, LW(3), LW(10), LW(11))
    CALL(evt_sub_random, 150, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    LABEL(30)
    CALL(evt_nannpc_set_dir, LW(0), 90)
    CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
    CALL(evt_sub_random, 60, LW(1))
    ADD(LW(1), 750)
    CALL(evt_sub_random, 200, LW(10))
    ADD(LW(10), 400)
    CALL(evt_sub_random, 30, LW(11))
    ADD(LW(11), 40)
    CALL(evt_nannpc_jump_position, LW(0), LW(1), 100, LW(3), LW(10), LW(11))
    CALL(evt_sub_random, 150, LW(10))
    ADD(LW(10), 100)
    WAIT_MS(LW(10))
    LABEL(40)
    CALL(evt_nannpc_set_animtbl, LW(0), PTR(suifu_run_animtbl))
    CALL(evt_nannpc_get_position, LW(0), LW(1), LW(2), LW(3))
    ADD(LW(3), -50)
    CALL(evt_nannpc_set_work, LW(0), 0, 1)
    CALL(evt_nannpc_move_position2, LW(0), LW(1), LW(2), LW(3), FLOAT(240.0f))
    CALL(evt_nannpc_set_work, LW(0), 0, 0)
    CALL(evt_nannpc_set_position, LW(0), 0, -1000, 0)
    RETURN
EVT_END()

EVT_BEGIN(bombwall_after)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    IF_INT_LESS_OR_EQUAL(LW(0), 780)
        CALL(evt_mario_set_dir, 90, 200, 0)
    ELSE
        CALL(evt_mario_set_dir, 270, 200, 0)
    END_IF
    CALL(evt_npc_flag_onoff, 1, STRING("水夫C"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫D"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("水夫E"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("モニー"), 1073741856)
    CALL(evt_npc_flag_onoff, 1, STRING("ピートン"), 1073741856)
    CALL(evt_cam3d_evt_set, 742, 233, 429, 742, 147, -63, 500, 11)
    CALL(evt_npc_move_position, STRING("水夫C"), 780, -135, 0, FLOAT(180.0f), 0)
    CALL(evt_set_dir_to_target, STRING("水夫C"), STRING("mario"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_12"), 0, STRING("水夫C"))
    CALL(evt_npc_move_position, STRING("水夫C"), 780, -185, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("水夫C"), 0, -1000, 0)
    CALL(evt_npc_set_ry, STRING("水夫D"), 270)
    WAIT_MS(200)
    CALL(evt_npc_move_position, STRING("水夫D"), 673, -38, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("水夫D"), 90)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("水夫D"), 676, 70, -110, 400, 50)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("水夫D"), 780, 100, -135, 500, 70)
    CALL(evt_set_dir_to_target, STRING("水夫D"), STRING("mario"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_13"), 0, STRING("水夫D"))
    CALL(evt_npc_move_position, STRING("水夫D"), 780, -185, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("水夫D"), 0, -1000, 0)
    SET_VALUE(GF(0), 1)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_set_ry, STRING("ピートン"), 270)
        WAIT_MS(200)
        CALL(evt_npc_move_position, STRING("ピートン"), 685, -48, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_ry, STRING("ピートン"), 90)
        WAIT_MS(200)
        CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 680, 70, -110, 400, 50)
        WAIT_MS(200)
        CALL(evt_npc_jump_position_nohit, STRING("ピートン"), 800, 100, -135, 500, 50)
        CALL(evt_set_dir_to_target, STRING("ピートン"), STRING("mario"))
        SET_VALUE(GF(0), 0)
    END_INLINE_EVENT
    CALL(evt_npc_set_ry, STRING("モニー"), 270)
    WAIT_MS(200)
    CALL(evt_npc_move_position, STRING("モニー"), 655, -48, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("モニー"), 90)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 650, 70, -110, 400, 50)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("モニー"), 770, 100, -135, 500, 50)
    CALL(evt_set_dir_to_target, STRING("モニー"), STRING("mario"))
    WAIT_MS(300)
    WAIT_WHILE(GF(0))
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_14"), 0, STRING("モニー"))
    CALL(evt_nannpc_set_subfunc, 2, 0)
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("ピートン"), 800, -185, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("ピートン"), 0, -1000, 0)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        CALL(evt_npc_move_position, STRING("モニー"), 770, -185, 0, FLOAT(180.0f), 0)
        CALL(evt_npc_set_position, STRING("モニー"), 0, -1000, 0)
    END_INLINE_EVENT
    SPAWN_EVENT(bombwall_suifuout_secheck)
    SET_VALUE(LW(0), 0)
    CALL(evt_snd_sfxon, STRING("SFX_STG5_KINOPIO_MOVE3"), 0)
    LOOP(0)
        SPAWN_EVENT(bombwall_suifuout)
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 95)
            BREAK_LOOP
        END_IF
        WAIT_MS(60)
        SPAWN_EVENT(bombwall_suifuout)
        ADD(LW(0), 1)
        IF_INT_EQUAL(LW(0), 95)
            BREAK_LOOP
        END_IF
        WAIT_MS(60)
    END_LOOP
    WAIT_MS(1000)
    CALL(evt_npc_set_ry, STRING("水夫E"), 270)
    WAIT_MS(200)
    CALL(evt_npc_move_position, STRING("水夫E"), 673, -38, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_ry, STRING("水夫E"), 90)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("水夫E"), 676, 70, -110, 400, 50)
    WAIT_MS(200)
    CALL(evt_npc_jump_position_nohit, STRING("水夫E"), 780, 100, -135, 500, 70)
    CALL(evt_set_dir_to_target, STRING("水夫E"), STRING("mario"))
    WAIT_MS(200)
    CALL(evt_msg_print, 0, STRING("stg5_dou_17_15"), 0, STRING("水夫E"))
    CALL(evt_npc_move_position, STRING("水夫E"), 780, -185, 0, FLOAT(180.0f), 0)
    CALL(evt_npc_set_position, STRING("水夫E"), 0, -1000, 0)
    WAIT_MS(500)
    CALL(evt_cam3d_evt_off, 500, 11)
    WAIT_MS(500)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

static EVT_BEGIN(dai3_shurai_after)
    LOOP(0)
        CALL(evt_sub_random, 2500, LW(10))
        WAIT_MS(LW(10))
        WAIT_MS(1500)
        BEGIN_INLINE_EVENT
            CALL(evt_cam_shake, 4, FLOAT(0.005f), FLOAT(0.0f), 500)
        END_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        ADD(LW(1), 250)
        CALL(evt_eff64, STRING(""), STRING("hokori_n64"), 0, LW(0), LW(1), LW(2), 60, 0, 0, 0, 0, 0, 0, 0)
    END_LOOP
    RETURN
EVT_END()



static BOOL _hitIsGroupSub(HitEntry* hit, HitEntry* group, BOOL rec) {
    if (strcmp(hit->joint->name, group->joint->name) == 0) {
        return TRUE;
    }
    if (group->child != NULL) {
        return _hitIsGroupSub(hit, group->child, TRUE);
    }
    if (rec) {
        if (group->sibling != NULL) {
            return _hitIsGroupSub(hit, group->sibling, TRUE);
        }
    }
    return FALSE;
}

static BOOL hitIsGroup(HitEntry* hit, HitEntry* group) {
    return _hitIsGroupSub(hit, group, FALSE);
}

USER_FUNC(dou_mario_kaiten_hosei) {
    MarioWork* mario;
    HitEntry* hit;
    HitEntry* hit2;
    Vec vec;
    Mtx mtx;
    f32 dist;
    f32 angle;
    f32 diff;
    f32 tmp;
    const char* name;

    name = (const char*)evtGetValue(event, event->args[0]);
    hit = hitNameToPtr(name);
    mario = marioGetPtr();

    if (isFirstCall) {
        event->userdataf[1] = 0.0f;
        event->userdata[2] = 0;
    }
    if (mario->hitobjStandOn == NULL) {
        event->userdataf[1] = 0.0f;
        event->userdata[2] = 0;
        return 0;
    }
    if (hitIsGroup(mario->hitobjStandOn, hit)) {
        vec = (Vec){ 0.0f, 1.0f, 0.0f };
        mtxGetRotationElement(hit->mapObj->unkAC, mtx, 'x', 'y');
        PSMTXMultVec(mtx, &vec, &vec);
        angle = angleABf(0.0f, 0.0f, vec.x, vec.y);
        diff = angle - event->userdataf[1];

        hitObjGetPos(name, &vec);
        dist = distABf(mario->position.x, mario->position.y, vec.x, vec.y);
        tmp = angleABf(vec.x, vec.y, mario->position.x, mario->position.y);

        hit2 = (HitEntry*)event->userdata[2];
        if (hit2 != NULL && strcmp(hit->joint->name, hit2->joint->name) == 0) {
            diff = 6.2831855f * (tmp + diff) / 360.0f;
            mario->riderDelta.x = (dist * (f32)sin(diff) + vec.x) - mario->position.x;
            mario->riderDelta.y = (vec.y - dist * (f32)cos(diff)) - mario->position.y;
        }
        event->userdataf[1] = angle;
        event->userdata[2] = (s32)hit;
    }
    return 0;
}

EVT_BEGIN(haikei_funeyure)
    BEGIN_INLINE_EVENT
        CALL(dou_mario_kaiten_hosei, STRING("A_h_fune"))
    END_INLINE_EVENT
    CALL(evt_hit_bind_mapobj, STRING("A_h_fune"), STRING("S_h_fune"))
    LOOP(0)
        CALL(evt_mapobj_get_position, STRING("S_h_fune"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_SHAKE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, -10, 10, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("S_h_fune"), 0, 0, LW(0))
            CALL(evt_hit_bind_update, STRING("A_h_fune"))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(evt_mapobj_get_position, STRING("S_h_fune"), LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_SHIP_SHAKE1"), LW(0), LW(1), LW(2), 0)
        CALL(evt_sub_intpl_msec_init, 11, 10, -10, 3000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, STRING("S_h_fune"), 0, 0, LW(0))
            CALL(evt_hit_bind_update, STRING("A_h_fune"))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(evt_ride_taru)
    CALL(evt_mapobj_get_position, LW(0), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_STG5_BARREL_ON1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main0)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), GW(0))
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(GW(0), LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main10)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main11)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main12)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main13)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main14)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main15)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main16)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main17)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    END_IF
    SET_VALUE(LW(10), LW(0))
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(evt_dou_normal_puka, LW(11))
        CALL(evt_mapobj_trans, 1, LW(10), 0, LW(11), 0)
        SET_RAW_VALUE(LW(12), 0)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_VALUE(0, LW(11))
        END_IF
        IF_INT_NOT_EQUAL(LW(1), 0)
            CALL(evt_hit_bind_update, LW(1))
        END_IF
        ADD(LW(0), 1)
        WAIT_MS(130)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init1)
    SET_VALUE(GW(1), 0)
    SET_VALUE(GW(2), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_1)
    IF_INT_NOT_EQUAL(GW(1), 0)
        RETURN
    END_IF
    SET_VALUE(GW(1), 1)
    SET_RAW_VALUE(LW(6), GW(2))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(1), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_1)
    IF_INT_NOT_EQUAL(GW(1), 2)
        RETURN
    END_IF
    SET_VALUE(GW(1), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main1)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(2), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub1)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(2))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(1), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(1), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init2)
    SET_VALUE(GW(3), 0)
    SET_VALUE(GW(4), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_2)
    IF_INT_NOT_EQUAL(GW(3), 0)
        RETURN
    END_IF
    SET_VALUE(GW(3), 1)
    SET_RAW_VALUE(LW(6), GW(4))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(3), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_2)
    IF_INT_NOT_EQUAL(GW(3), 2)
        RETURN
    END_IF
    SET_VALUE(GW(3), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main2)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(4), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub2)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(4))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(3), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(3), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_init3)
    SET_VALUE(GW(5), 0)
    SET_VALUE(GW(6), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_3)
    IF_INT_NOT_EQUAL(GW(5), 0)
        RETURN
    END_IF
    SET_VALUE(GW(5), 1)
    SET_RAW_VALUE(LW(6), GW(6))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(5), 2)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_rev_3)
    IF_INT_NOT_EQUAL(GW(5), 2)
        RETURN
    END_IF
    SET_VALUE(GW(5), 0)
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_main3)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(6), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static EVT_BEGIN(dou_puka_event_sub3)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(6))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(5), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(5), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_init4)
    SET_VALUE(GW(7), 0)
    SET_VALUE(GW(8), 0)
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_4)
    IF_INT_NOT_EQUAL(GW(7), 0)
        RETURN
    END_IF
    SET_VALUE(GW(7), 1)
    SET_RAW_VALUE(LW(6), GW(8))
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL_CHILD(LW(3))
    END_IF
    CALL_CHILD(dou_puka_event)
    SET_VALUE(GW(7), 2)
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_rev_4)
    IF_INT_NOT_EQUAL(GW(7), 2)
        RETURN
    END_IF
    SET_VALUE(GW(7), 0)
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_main4)
    CALL(evt_hit_bind_mapobj, LW(1), LW(0))
    LOOP(0)
        CALL(evt_mapobj_trans, 1, LW(0), 0, GW(8), 0)
        CALL(evt_hit_bind_update, LW(1))
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(dou_puka_event_sub4)
    CALL(evt_sub_random, 400, LW(0))
    ADD(LW(0), 200)
    WAIT_MS(LW(0))
    SET_RAW_VALUE(LW(6), GW(8))
    SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    LOOP(0)
        LOOP(0)
            IF_INT_EQUAL(GW(7), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        DELETE_EVENT(LW(0))
        LOOP(0)
            IF_INT_EQUAL(GW(7), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        SPAWN_EVENT_GET_ID(PTR(dou_normal_puka), LW(0))
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(pukapuka_watari_init01)
    SET_VALUE(LW(0), STRING("S_taru_n01"))
    SET_VALUE(LW(1), STRING("A_taru_n01"))
    SET_VALUE(LW(2), STRING("A_taru_n01"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init1)
    SPAWN_EVENT(dou_puka_event_main1)
    SPAWN_EVENT(dou_puka_event_sub1)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru_n01"), 0, PTR(dou_puka_event_1), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru_n01"), 0, PTR(dou_puka_event_rev_1), 0)
    SET_VALUE(LW(0), STRING("S_taru_n02"))
    SET_VALUE(LW(1), STRING("A_taru_n02"))
    SET_VALUE(LW(2), STRING("A_taru_n02"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init2)
    SPAWN_EVENT(dou_puka_event_main2)
    SPAWN_EVENT(dou_puka_event_sub2)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru_n02"), 0, PTR(dou_puka_event_2), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru_n02"), 0, PTR(dou_puka_event_rev_2), 0)
    RETURN
EVT_END()

EVT_BEGIN(pukapuka_watari_init02)
    SET_VALUE(LW(0), STRING("S_taru_n03"))
    SET_VALUE(LW(1), STRING("A_taru_n03"))
    SET_VALUE(LW(2), STRING("A_taru_n03"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init3)
    SPAWN_EVENT(dou_puka_event_main3)
    SPAWN_EVENT(dou_puka_event_sub3)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru_n03"), 0, PTR(dou_puka_event_3), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru_n03"), 0, PTR(dou_puka_event_rev_3), 0)
    SET_VALUE(LW(0), STRING("S_taru_n04"))
    SET_VALUE(LW(1), STRING("A_taru_n04"))
    SET_VALUE(LW(2), STRING("A_taru_n04"))
    SET_VALUE(LW(3), PTR(evt_ride_taru))
    CALL_CHILD(dou_puka_event_init4)
    SPAWN_EVENT(dou_puka_event_main4)
    SPAWN_EVENT(dou_puka_event_sub4)
    CALL(evt_run_case_evt, 0, 1, STRING("A_taru_n04"), 0, PTR(dou_puka_event_4), 0)
    CALL(evt_run_case_evt, 1, 1, STRING("A_taru_n04"), 0, PTR(dou_puka_event_rev_4), 0)
    RETURN
EVT_END()

HitDamageReturn hitDmgRetPoints_1st[17] = {
    { "mod_01", { 400.f, 15.f, 0.f } },
    { "mod_02", { -650.f, 200.f, 0.f } },
    { "mod_03", { 0.f, -1000.f, 0.f } },
    { "mod_04", { -208.f, 50.f, -96.f } },
    { "mod_010", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_01", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_02", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_03", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_04", { 0.f, -1000.f, 0.f } },
    { "a11_a_usiro", { 160.f, 37.f, 0.f } },
    { "a11_a_kaidana_01", { 160.f, 37.f, 0.f } },
    { "a11_a_kaidanb_01", { 160.f, 37.f, 0.f } },
    { "dokan_1", { 0.f, -1000.f, 0.f } },
    { "dokan_2", { 0.f, -1000.f, 0.f } },
    { "dokan_3", { 0.f, -1000.f, 0.f } },
    { "dokan_7", { 0.f, -1000.f, 0.f } },
    { NULL },
};

static HitDamageReturn hitDmgRetPoints[17] = {
    { "mod_01", { 725.f, 50.f, 90.f } },
    { "mod_02", { -650.f, 200.f, 0.f } },
    { "mod_03", { 0.f, -1000.f, 0.f } },
    { "mod_04", { -208.f, 50.f, -96.f } },
    { "mod_010", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_01", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_02", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_03", { 0.f, -1000.f, 0.f } },
    { "Ship_Change_04", { 0.f, -1000.f, 0.f } },
    { "a11_a_usiro", { 160.f, 37.f, 0.f } },
    { "a11_a_kaidana_01", { 160.f, 37.f, 0.f } },
    { "a11_a_kaidanb_01", { 160.f, 37.f, 0.f } },
    { "dokan_1", { 0.f, -1000.f, 0.f } },
    { "dokan_2", { 0.f, -1000.f, 0.f } },
    { "dokan_3", { 0.f, -1000.f, 0.f } },
    { "dokan_7", { 0.f, -1000.f, 0.f } },
    { NULL },
};

EVT_BEGIN(dou_11_init_evt)
    CALL(evt_snd_bgmon, 512, STRING("BGM_STG5_DOU1"))
    CALL(evt_snd_envon, 272, STRING("ENV_STG5_DOU12"))
    CALL(evt_snd_set_rev_mode, 2)
    SET_VALUE(LW(0), PTR(bero_entry_data))
    CALL(evt_bero_get_info)
    CALL_CHILD(evt_bero_info_run)
    CALL(evt_npc_setup, PTR(npcEnt))
    CALL(evt_npc_set_tribe, STRING("モニー"), STRING("モニー（指輪）"))
    CALL(evt_npc_set_tribe, STRING("ピートン"), STRING("ピートン（指輪）"))
    CALL(evt_map_playanim, STRING("nami"), 1, 0)
    CALL(evt_map_playanim, STRING("moku"), 1, 0)
    SPAWN_EVENT(bombwall_init)
    SWITCH(GSW(0))
        CASE_EQUAL(249)
        IF_INT_EQUAL(GSWF(2982), 0)
            SPAWN_EVENT(torinoko_suifu)
        END_IF
        IF_INT_EQUAL(GSWF(2992), 0)
            SPAWN_EVENT(torinoko_suifu_init)
            SPAWN_EVENT(torinoko_suifu_sakebu)
        END_IF
        CASE_EQUAL(254)
        SPAWN_EVENT(escapeway_event)
    END_SWITCH
    IF_INT_EQUAL(GSWF(2992), 1)
        CALL(evt_hitobj_onoff, STRING("fune_atari"), 1, 0)
    END_IF
    IF_INT_EQUAL(GSW(0), 249)
        IF_INT_EQUAL(GSWF(2992), 1)
            SPAWN_EVENT(watarikiri_suifu_init)
            CALL(evt_npc_set_position, STRING("モニー"), 681, 50, -75)
            CALL(evt_npc_set_ry_lr, STRING("モニー"), 1)
            CALL(evt_npc_set_position, STRING("ピートン"), 714, 50, -78)
            CALL(evt_npc_set_ry_lr, STRING("ピートン"), 1)
            SET_VALUE(GW(10), 0)
            SPAWN_EVENT(suifu_orei)
        END_IF
    END_IF
    IF_INT_GREATER_OR_EQUAL(GSW(0), 250)
        IF_INT_LESS_OR_EQUAL(GSW(0), 255)
            SPAWN_EVENT(watarikiri_suifu_init)
        END_IF
    END_IF
    CALL(evt_mobj_switch_blue, 2, STRING("mobj01"), -220, 120, -450, PTR(unk_evt_dou_00011744), GSWF(2983))
    CALL(evt_mobj_switch_blue, 2, STRING("mobj02"), 80, 120, -450, PTR(unk_evt_dou_000117a0), GSWF(2984))
    SPAWN_EVENT(ukishima_init1)
    SPAWN_EVENT(ukishima_init2)
    IF_INT_GREATER_OR_EQUAL(GSW(0), 253)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n03"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n03"), 1, 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, STRING("S_taru_n04"), 1)
        CALL(evt_hitobj_onoff, STRING("A_taru_n04"), 1, 0)
    END_IF
    SPAWN_EVENT(haikei_funeyure)
    CALL(evt_map_entry_airport_harbor, 0, STRING("S_hikou"), STRING("A_hikou"))
    IF_INT_LESS(GSW(0), 250)
        CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints_1st))
    ELSE
        CALL(evt_hit_damage_return_set, PTR(hitDmgRetPoints))
    END_IF
    CALL_CHILD(evt_gazigazi_entry)
    SET_VALUE(LW(0), STRING("s11fune1"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main0), LW(10))
    SET_VALUE(LW(0), STRING("S_taru01"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main10), LW(10))
    SET_VALUE(LW(0), STRING("S_taru02"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main11), LW(10))
    SET_VALUE(LW(0), STRING("S_taru03"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main12), LW(10))
    SET_VALUE(LW(0), STRING("S_taru04"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main13), LW(10))
    SET_VALUE(LW(0), STRING("S_bin01"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main14), LW(10))
    SET_VALUE(LW(0), STRING("S_bin02"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main15), LW(10))
    SET_VALUE(LW(0), STRING("S_bin03"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main16), LW(10))
    SET_VALUE(LW(0), STRING("S_bin04"))
    SET_VALUE(LW(1), 0)
    SPAWN_EVENT_GET_ID(PTR(dou_puka_event_main17), LW(10))
    SET_VALUE(LW(0), GSWF(2983))
    ADD(LW(0), GSWF(2984))
    IF_INT_GREATER_OR_EQUAL(LW(0), 1)
        SPAWN_EVENT(pukapuka_watari_init01)
    END_IF
    IF_INT_GREATER_OR_EQUAL(LW(0), 2)
        SPAWN_EVENT(pukapuka_watari_init02)
    END_IF
    CALL(evt_map_entry_airport_harbor, 1, STRING("S_Ship_Change_01"), STRING("Ship_Change_01"))
    IF_INT_GREATER_OR_EQUAL(GSW(0), 257)
        IF_INT_LESS(GSW(0), 258)
            SPAWN_EVENT(dai3_shurai_after)
        END_IF
    END_IF
    CALL(evt_hitobj_onoff, STRING("A_ana_before"), 1, 0)
    RETURN
EVT_END()

// clang-format on
