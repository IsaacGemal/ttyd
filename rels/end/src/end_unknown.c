#include "end_unknown.h"
#include "data/mapdata.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/msgdrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_mahojin.h"
#include "evt/evt_bg.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "evt/evt_ext.h"
#include "evt/evt_fade.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_memcard.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "forcedata.h"
#include "manager/filemgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "sdk/texPalette.h"
#include "system.h"
#include <dolphin/exi.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include "math_ext.h"

typedef void (*voidfunctionptr)(void);

extern voidfunctionptr _ctors[];
extern voidfunctionptr _dtors[];

typedef struct CreditsPoseState {
    /* 0x00 */ Vec position;
    /* 0x0C */ s32 idx;
    /* 0x10 */ s32 counter;
} CreditsPoseState;


// "end" REL credits work block (the target of dat_ptr_end_00003218)
typedef struct CreditsWork {
    /* 0x00 */ void* image;
    /* 0x04 */ s32 mode;
    /* 0x08 */ s32 field_08;
    /* 0x0C */ s32 field_0C;
    /* 0x10 */ s32 field_10;
    /* 0x14 */ s32 windowIdA[3];
    /* 0x20 */ s32 windowIdB[3];
    /* 0x2C */ s32 field_2C;
    /* 0x30 */ s32 entryIdx;
    /* 0x34 */ s32 eventId;
} CreditsWork;

// R_credits_npc_info entries
typedef struct CreditsNpcInfo {
    /* 0x00 */ const char* name;
    /* 0x04 */ const char* anim;
    /* 0x08 */ s32* event;
} CreditsNpcInfo;

// per-pose entry allocated by unk_end_00001d90 for the credits animation callback
typedef struct CreditsPoseEntry {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 poseId;
    /* 0x08 */ f32 frame;
    /* 0x0C */ f32 endFrame;
    /* 0x10 */ s32 field_10;
    /* 0x14 */ Mtx matrix;
} CreditsPoseEntry;

extern GXColor dat_end_00002f2c;
extern GXColor zero_end_00002f30;
extern GXColor zero_end_00002f34;
extern GXColor dat_end_00002f38;
extern GXColor zero_end_00002f3c;
extern GXColor dat_end_00002f64;

#pragma pool_data off

#pragma force_active on
s32 unk_evt_end_000045cc[49];
s32 unk_evt_end_0000478c[48];
s32 unk_evt_end_0000484c[89];
s32 unk_evt_end_000049b0[54];
s32 unk_evt_end_00004a88[60];
s32 unk_evt_end_00004b78[81];
s32 unk_evt_end_00004cbc[50];
s32 unk_evt_end_00004d84[83];
s32 unk_evt_end_00004ed0[89];
s32 unk_evt_end_0000503c[108];
// .bss emitted in reverse source order by the deferred-inline compiler, so declare them
// back-to-front to land at 0x00..0x10 as in the original.
u32 unk_end_bss00000010[14];
u32 unk_end_bss0000000c[1];
u32 unk_end_bss00000008[1];
u32 unk_end_bss00000004[1];
u32 unk_end_bss00000000[1];

// clang-format off
// TODO: type
u32 dat_ptr_end_00003218[1] = {
    (u32)&unk_end_bss00000010,
};

// TODO: type
u32 unk_end_0000321c[450] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)"a00", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, (u32)"a01", 0x00000000, (u32)"b00",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000002, 0x00000000, (u32)"a02", 0x00000000,
    (u32)"b01", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, (u32)"b02", (u32)"b03", 0x00000000,
    0x00000000, 0x00000000, 0x00000004, 0x00000000,
    (u32)"a03", 0x00000000, (u32)"b04", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000005,
    0x00000000, (u32)"a04", 0x00000000, (u32)"b05",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000006, 0x00000000, (u32)"a05", 0x00000000,
    (u32)"b06", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000007, 0x00000000, (u32)"a06",
    0x00000000, (u32)"b07", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000008, 0x00000000,
    (u32)"a07", 0x00000000, (u32)"b08", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000009,
    0x00000000, (u32)"a08", 0x00000000, (u32)"b09",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000000A, 0x00000000, 0x00000000, 0x00000000,
    (u32)"b10", (u32)"b11", 0x00000000, 0x00000000,
    0x00000001, 0x0000000B, 0x00000000, (u32)"a09",
    0x00000000, (u32)"b12", 0x00000000, 0x00000000,
    0x00000000, 0x00000001, 0x0000000C, 0x00000000,
    0x00000000, 0x00000000, (u32)"b13", 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x0000000D,
    0x00000000, 0x00000000, 0x00000000, (u32)"b14",
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x0000000E, 0x00000000, 0x00000000, 0x00000000,
    (u32)"b15", (u32)"b16", 0x00000000, 0x00000000,
    0x00000001, 0x0000000F, 0x00000000, (u32)"a10",
    0x00000000, (u32)"b17", 0x00000000, 0x00000000,
    0x00000000, 0x00000001, 0x00000010, 0x00000000,
    (u32)"a11", 0x00000000, (u32)"b18", 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x00000011,
    0x00000000, (u32)"a12", 0x00000000, (u32)"b19",
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x00000012, 0x00000000, (u32)"a13", 0x00000000,
    (u32)"b20", 0x00000000, 0x00000000, 0x00000000,
    0x00000001, 0x00000013, 0x00000000, 0x00000000,
    0x00000000, (u32)"b21", 0x00000000, 0x00000000,
    0x00000000, 0x00000002, 0x00000014, 0x00000000,
    (u32)"a14", 0x00000000, (u32)"b22", 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000015,
    0x00000000, 0x00000000, 0x00000000, (u32)"b23",
    0x00000000, 0x00000000, 0x00000000, 0x00000002,
    0x00000016, 0x00000000, 0x00000000, 0x00000000,
    (u32)"b24", 0x00000000, 0x00000000, 0x00000000,
    0x00000002, 0x00000017, 0x00000000, 0x00000000,
    0x00000000, (u32)"b25", 0x00000000, 0x00000000,
    0x00000000, 0x00000002, 0x00000018, 0x00000000,
    (u32)"a15", 0x00000000, (u32)"b26", 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x00000019,
    0x00000000, 0x00000000, 0x00000000, (u32)"b27",
    0x00000000, 0x00000000, 0x00000000, 0x00000002,
    0x0000001A, 0x00000000, (u32)"a17", 0x00000000,
    (u32)"b28", 0x00000000, 0x00000000, 0x00000000,
    0x00000002, 0x0000001B, 0x00000000, 0x00000000,
    0x00000000, (u32)"b29", 0x00000000, 0x00000000,
    0x00000000, 0x00000002, 0x0000001C, 0x00000000,
    (u32)"a18", 0x00000000, (u32)"b30", 0x00000000,
    0x00000000, 0x00000000, 0x00000002, 0x0000001D,
    0x00000000, 0x00000000, 0x00000000, (u32)"b31",
    (u32)"b32", 0x00000000, 0x00000000, 0x00000003,
    0x0000001E, 0x00000000, (u32)"a19", 0x00000000,
    (u32)"b33", 0x00000000, 0x00000000, 0x00000000,
    0x00000003, 0x0000001F, 0x00000000, (u32)"a20",
    0x00000000, (u32)"b34", (u32)"b35", (u32)"b36",
    0x00000000, 0x00000003, 0x00000020, 0x00000000,
    (u32)"a21", 0x00000000, (u32)"b37", (u32)"b38",
    0x00000000, 0x00000000, 0x00000003, 0x00000021,
    0x00000000, 0x00000000, 0x00000000, (u32)"b39",
    (u32)"b40", 0x00000000, 0x00000000, 0x00000003,
    0x00000022, 0x00000000, 0x00000000, 0x00000000,
    (u32)"b41", (u32)"b42", 0x00000000, 0x00000000,
    0x00000003, 0x00000023, 0x00000000, 0x00000000,
    0x00000000, (u32)"b43", (u32)"b44", 0x00000000,
    0x00000000, 0x00000003, 0x00000024, 0x00000000,
    0x00000000, 0x00000000, (u32)"b45", 0x00000000,
    0x00000000, 0x00000000, 0x00000003, 0x00000025,
    0x00000000, 0x00000000, 0x00000000, (u32)"b46",
    (u32)"b47", 0x00000000, 0x00000000, 0x00000003,
    0x00000026, 0x00000000, (u32)"a22", 0x00000000,
    (u32)"b48", (u32)"b49", 0x00000000, 0x00000000,
    0x00000003, 0x00000027, 0x00000000, 0x00000000,
    0x00000000, (u32)"b50", (u32)"b51", 0x00000000,
    0x00000000, 0x00000000, 0x00000028, 0x00000000,
    (u32)"a23", 0x00000000, (u32)"b52", (u32)"b53",
    0x00000000, 0x00000000, 0x00000000, 0x00000029,
    0x00000000, (u32)"a24", 0x00000000, (u32)"b54",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000002A, 0x00000000, (u32)"a25", 0x00000000,
    (u32)"b55", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0000002B, 0x00000000, (u32)"a26",
    0x00000000, (u32)"b56", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000002C, 0x00000000,
    (u32)"a27", 0x00000000, (u32)"b57", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0000002D,
    0x00000000, (u32)"a28", 0x00000000, (u32)"b58",
    (u32)"b59", 0x00000000, 0x00000000, 0x00000000,
    0x0000002E, 0x00000000, 0x00000000, 0x00000000,
    (u32)"b60", (u32)"b61", 0x00000000, 0x00000000,
    0x00000000, 0x0000002F, 0x00000000, (u32)"a29",
    0x00000000, (u32)"b62", (u32)"b63", 0x00000000,
    0x00000000, 0x00000000, 0x00000030, 0x00000000,
    (u32)"a30", 0x00000000, (u32)"b64", 0x00000000,
    0x00000000, 0x00000000, 0x00000004, 0x00000031,
    (u32)"c00", 0x00000000, 0x00000000, (u32)"c01",
    0x00000000, 0x00000000,
};

// TODO: type
u32 R_credits_npc_info[456] = {
    (u32)"c_luigi", (u32)"W_1", 0x00000000, (u32)"c_pkr",
    (u32)"PKR_W_1", 0x00000000, (u32)"c_kuri_h", (u32)"W_1",
    0x00000000, (u32)"c_kino_ji", (u32)"W_1", 0x00000000,
    (u32)"c_cook", (u32)"W_1", 0x00000000, (u32)"c_deale",
    (u32)"W_1", 0x00000000, (u32)"c_levela", (u32)"W_1",
    0x00000000, (u32)"c_uranai", (u32)"W_1", 0x00000000,
    (u32)"c_majinai", (u32)"W_1", 0x00000000, (u32)"c_botta",
    (u32)"W_1", 0x00000000, (u32)"c_mania", (u32)"MNA_W_1",
    0x00000000, (u32)"c_joho_a", (u32)"W_1", 0x00000000,
    (u32)"c_gesso", (u32)"GSO_S2_1", (u32)&unk_evt_end_0000478c, (u32)"c_joho_b",
    (u32)"W_1", 0x00000000, (u32)"c_gshop_m", (u32)"W_2",
    0x00000000, (u32)"c_gshop_m", (u32)"W_1", 0x00000000,
    (u32)"c_pub_m", (u32)"W_1", 0x00000000, (u32)"c_roten",
    (u32)"W_1", 0x00000000, (u32)"c_tyusan", (u32)"CHU_W_1",
    0x00000000, (u32)"c_borodo_n", (u32)"W_1", 0x00000000,
    (u32)"c_bom_deb", (u32)"W_1", 0x00000000, (u32)"c_kuribo_n",
    (u32)"W_1", 0x00000000, (u32)"c_hannya", (u32)"W_1",
    0x00000000, (u32)"c_hannya_n", (u32)"W_1", 0x00000000,
    (u32)"c_pnk", (u32)"PNK_W_1", 0x00000000, (u32)"c_nokorin",
    (u32)"W_1", 0x00000000, (u32)"c_nokopapa", (u32)"NKP_W_1",
    0x00000000, (u32)"c_nokoson", (u32)"NKS_W_1", 0x00000000,
    (u32)"c_nokonoko", (u32)"NOK_W_1", 0x00000000, (u32)"c_kinopio",
    (u32)"KNP_W_1", 0x00000000, (u32)"c_kinopiko", (u32)"KNF_W_1",
    0x00000000, (u32)"c_kokinopio", (u32)"CKP_W_1", 0x00000000,
    (u32)"c_gonbaba", (u32)"GNB_S_3", (u32)&unk_evt_end_0000484c, (u32)"c_flower",
    (u32)"W_1", 0x00000000, (u32)"c_kiza", (u32)"W_1",
    0x00000000, (u32)"c_kuribo", (u32)"KUR_W_1", 0x00000000,
    (u32)"c_kuribo", (u32)"TGK_W_1", 0x00000000, (u32)"c_kuribo",
    (u32)"PTK_W_1", 0x00000000, (u32)"c_nokoteki", (u32)"NKT_W_1",
    0x00000000, (u32)"c_nokoteki", (u32)"PTP_W_1", 0x00000000,
    (u32)"c_sinnosuke", (u32)"SIS_W_1", 0x00000000, (u32)"c_togedaruma",
    (u32)"TGD_S_1", 0x00000000, (u32)"c_chorobon", (u32)"CBN_W_1",
    0x00000000, (u32)"c_karon_h", (u32)"KRN_W_1", 0x00000000,
    (u32)"c_windy", (u32)"PWD_W_1", 0x00000000, (u32)"c_pguide",
    (u32)"W_1", 0x00000000, (u32)"c_psister", (u32)"W_1",
    0x00000000, (u32)"c_psoncho", (u32)"W_1", 0x00000000,
    (u32)"c_pshop", (u32)"W_1", 0x00000000, (u32)"c_puni",
    (u32)"W_1", 0x00000000, (u32)"c_puni", (u32)"W_1",
    0x00000000, (u32)"c_puni", (u32)"W_1", 0x00000000,
    (u32)"c_puni", (u32)"W_1", 0x00000000, (u32)"c_puni",
    (u32)"W_1", 0x00000000, (u32)"c_mb_robo_1a", (u32)"W_1",
    (u32)&unk_evt_end_00004d84, (u32)"c_puni", (u32)"W_1", 0x00000000,
    (u32)"c_prival", (u32)"W_1", 0x00000000, (u32)"c_toge",
    (u32)"W_1", 0x00000000, (u32)"c_tchibi", (u32)"W_1",
    0x00000000, (u32)"c_sinemon", (u32)"SIN_W_1", 0x00000000,
    (u32)"c_paid", (u32)"PAI_W_1", 0x00000000, (u32)"c_m_boss",
    (u32)"W_1", 0x00000000, (u32)"c_m_buka", (u32)"W_1",
    0x00000000, (u32)"c_m_waka", (u32)"W_1", 0x00000000,
    (u32)"c_m_ojo", (u32)"W_1", 0x00000000, (u32)"c_binbo",
    (u32)"W_1", 0x00000000, (u32)"c_monban", (u32)"W_1",
    0x00000000, (u32)"c_bteresa", (u32)"BTS_W_1", 0x00000000,
    (u32)"c_babyyoshi", (u32)"PYS_W_1", 0x00000000, (u32)"c_ksikowa",
    (u32)"W_1", 0x00000000, (u32)"c_k_chof", (u32)"W_1",
    0x00000000, (u32)"c_gans", (u32)"W_1", 0x00000000,
    (u32)"c_bred_h", (u32)"W_1", 0x00000000, (u32)"c_report",
    (u32)"W_1", 0x00000000, (u32)"c_hotdog", (u32)"W_1",
    0x00000000, (u32)"c_shasho_g", (u32)"W_1", 0x00000000,
    (u32)"c_jugem_n", (u32)"W_1", 0x00000000, (u32)"c_togezo",
    (u32)"TGZ_W_1", 0x00000000, (u32)"c_heavy", (u32)"W_1",
    0x00000000, (u32)"c_kamek", (u32)"KMK_W_1", 0x00000000,
    (u32)"c_togenoko_b", (u32)"W_1", 0x00000000, (u32)"c_burosu_h",
    (u32)"BRO_W_1", 0x00000000, (u32)"c_koopa", (u32)"KPA_W_1",
    0x00000000, (u32)"c_atmic_trs", (u32)"W_1", (u32)&unk_evt_end_00004a88,
    (u32)"c_kamek_be", (u32)"W_1", 0x00000000, (u32)"c_f_son",
    (u32)"W_1", 0x00000000, (u32)"c_bubusan", (u32)"W_1",
    0x00000000, (u32)"c_f_mura", (u32)"W_1", 0x00000000,
    (u32)"c_f_mama", (u32)"W_1", 0x00000000, (u32)"c_f_child_a",
    (u32)"W_1", 0x00000000, (u32)"c_f_oji", (u32)"W_1",
    0x00000000, (u32)"c_f_oba", (u32)"W_1", 0x00000000,
    (u32)"c_f_karas", (u32)"A_1", 0x00000000, (u32)"c_teresa",
    (u32)"W_1", 0x00000000, (u32)"c_ranpel", (u32)"W_1",
    0x00000000, (u32)"c_f_owm", (u32)"A_1", 0x00000000,
    (u32)"c_met", (u32)"MET_W_1", 0x00000000, (u32)"c_met",
    (u32)"TMT_W_1", 0x00000000, (u32)"c_pansy", (u32)"PAN_W_1",
    0x00000000, (u32)"c_majyorin", (u32)"MJR_W_1", 0x00000000,
    (u32)"c_maririn", (u32)"MRR_W_1", 0x00000000, (u32)"c_vivian",
    (u32)"PTR_W_1", 0x00000000, (u32)"c_korutesu1", (u32)"KRT_S_1",
    (u32)&unk_evt_end_00004b78, (u32)"c_dance_r", (u32)"W_1", 0x00000000,
    (u32)"c_dance_p", (u32)"W_1", 0x00000000, (u32)"c_dance_k",
    (u32)"W_1", 0x00000000, (u32)"c_bomt_n", (u32)"W_1",
    0x00000000, (u32)"c_marco", (u32)"W_1", 0x00000000,
    (u32)"c_gara", (u32)"W_1", 0x00000000, (u32)"c_megane",
    (u32)"W_1", 0x00000000, (u32)"c_odo_k", (u32)"W_1",
    0x00000000, (u32)"c_suifu_k", (u32)"W_1", 0x00000000,
    (u32)"c_suifu_b", (u32)"W_1", 0x00000000, (u32)"c_elmos",
    (u32)"W_1", 0x00000000, (u32)"c_kilr", (u32)"KIL_W_1",
    0x00000000, (u32)"c_pakflwr", (u32)"PKF_S_1", 0x00000000,
    (u32)"c_tyutyu", (u32)"PCH_W_1", 0x00000000, (u32)"c_powan",
    (u32)"W_1", 0x00000000, (u32)"c_g_moamoa", (u32)"S_3",
    (u32)&unk_evt_end_0000503c, (u32)"c_papab", (u32)"W_1", 0x00000000,
    (u32)"c_mamab", (u32)"W_1", 0x00000000, (u32)"c_konari",
    (u32)"W_1", 0x00000000, (u32)"c_unten", (u32)"W_1",
    0x00000000, (u32)"c_shasho_r", (u32)"W_1", 0x00000000,
    (u32)"c_smoko", (u32)"W_1", 0x00000000, (u32)"c_rose",
    (u32)"W_1", 0x00000000, (u32)"c_kinodebu", (u32)"W_1",
    0x00000000, (u32)"c_ghost_k", (u32)"W_1", 0x00000000,
    (u32)"c_bom_sen", (u32)"S_2", (u32)&unk_evt_end_00004cbc, (u32)"c_waitress",
    (u32)"W_1", 0x00000000, (u32)"c_puku_c", (u32)"W_1",
    0x00000000, (u32)"c_kinopiko", (u32)"KNF_W_1", 0x00000000,
    (u32)"c_n_moa_a", (u32)"W_1", 0x00000000, (u32)"c_met",
    (u32)"PMT_W_1", 0x00000000, (u32)"c_sanbo", (u32)"SNB_W_1",
    0x00000000, (u32)"c_bomson", (u32)"W_1", 0x00000000,
    (u32)"c_mb_robo_2a", (u32)"J_2", (u32)&unk_evt_end_000049b0, (u32)"c_bwhite",
    (u32)"W_1", 0x00000000, (u32)"c_hito_b", (u32)"W_1",
    0x00000000, (u32)"c_bom_n", (u32)"W_1", 0x00000000,
    (u32)"c_kmoon", (u32)"W_1", 0x00000000, (u32)"c_baria",
    (u32)"W_1", 0x00000000, (u32)"c_basabasa", (u32)"BSA_W_1",
    0x00000000, (u32)"c_wanwan", (u32)"WAN_A_1", 0x00000000,
    (u32)"c_maho", (u32)"MAH_W_1", 0x00000000, (u32)"c_zako_n",
    (u32)"W_1", 0x00000000, (u32)"c_zako_m", (u32)"W_1",
    0x00000000, (u32)"c_shuryo", (u32)"W_1", 0x00000000,
    (u32)"c_q_kage_m", (u32)"KRT_Z_1", (u32)&unk_evt_end_00004ed0, (u32)"c_gazigazi",
    (u32)"GAZ_A_1", 0x00000000, (u32)"a_mario", (u32)"M_W_1",
    0x00000000, (u32)"c_peach", (u32)"P_W_1", 0x00000000,
};

// NONMATCHING (80.00%): config/G8MJ01/end/symbols.txt gives _prolog size 0x50, but it is 0x60; its epilogue is split off
// into gap_00_00002520_text, so the target is cut short.
void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("end_00", unk_evt_end_00004690);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
    unk_end_00000a14();
}

void _unresolved(void) {
}

void unk_end_00002508(TevCallbackParam* param) {
    GXSetTevColorIn(param->field_C - 1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
}

void unk_end_00002494(s32 wXluStage) {
    AnimationWork* anim = animGetPtr();

    anim->currDispMode = wXluStage;
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
}

void unk_end_000023ac(CameraId cameraId, void* param) {
    CreditsPoseEntry* entry = (CreditsPoseEntry*)param;

    if (evtGetValue(NULL, GF(10)) == 0) {
        mapSetTevCallback(1, unk_end_00002508);
        animPoseSetMaterialFlagOn(entry->poseId, 0x1000000);
        animPoseSetGXFunc(entry->poseId, unk_end_00002494, FALSE);
        animPoseDrawMtx(entry->poseId, entry->matrix, 1, 0.0f, 2.0f);
        animPoseDrawMtx(entry->poseId, entry->matrix, 2, 0.0f, 2.0f);
        animPoseDrawMtx(entry->poseId, entry->matrix, 3, 0.0f, 2.0f);
        animPoseSetMaterialFlagOff(entry->poseId, 0x1000000);
    }
}

s32 unk_end_000022e8(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 idx = evtGetValue(event, args[0]);

    if ((u32)idx >= 0x98) {
        evtSetValue(event, args[1], 0);
        return EVT_RETURN_DONE;
    } else {
        CreditsNpcInfo* npc = (CreditsNpcInfo*)R_credits_npc_info;

        if (npc[idx].event != NULL) {
            evtSetValue(event, args[1], 0);
        } else if (strcmp(npc[idx].name, "c_gazigazi") == 0) {
            evtSetValue(event, args[1], 2);
        } else {
            evtSetValue(event, args[1], 1);
        }
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (95.69%): register allocation only; the R_credits_npc_info base is cached where the original
// recomputes it, and constant-pool symbols differ.
s32 unk_end_00001d90(EventEntry* event, BOOL isFirstCall) {
    s32 idx = evtGetValue(event, event->args[0]);
    CreditsPoseEntry* entry;
    s32 inBattle = (gp->inBattle != 0);
    f32 f31;
    f32 f30;
    f32 f29;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;

    if (((CreditsNpcInfo*)R_credits_npc_info)[idx].event != NULL) {
        evtEntry(((CreditsNpcInfo*)R_credits_npc_info)[idx].event, 0, 0);
        if ((u32)(idx + 5) < 0x98) {
            animGroupBaseAsync(((CreditsNpcInfo*)R_credits_npc_info)[idx + 5].name, inBattle, NULL);
        }
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        entry = _mapAlloc(0x44);
        memset(entry, 0, 0x44);
        entry->state = 0;
        entry->field_10 = ((CreditsWork*)dat_ptr_end_00003218[0])->entryIdx;
        switch (entry->field_10) {
            case 0:
                entry->frame = -20.0f;
                entry->endFrame = 30.0f + (90.0f + entry->frame);
                break;
            case 1:
            case 4:
                entry->frame = 160.0f;
                entry->endFrame = 30.0f + (90.0f + entry->frame);
                break;
            case 2:
                entry->frame = 250.0f;
                entry->endFrame = 30.0f + (90.0f + entry->frame);
                break;
            default:
                entry->frame = 70.0f;
                entry->endFrame = 30.0f + (90.0f + entry->frame);
                break;
        }
        if ((u32)(idx + 5) < 0x98) {
            animGroupBaseAsync(((CreditsNpcInfo*)R_credits_npc_info)[idx + 5].name, inBattle, NULL);
        }
        event->userdata[0] = (s32)entry;
    }
    entry = (CreditsPoseEntry*)event->userdata[0];
    switch (entry->field_10) {
        case 0:
            f31 = 304.0f;
            f30 = -240.0f;
            f29 = 100.0f;
            break;
        case 1:
        case 4:
            f31 = -304.0f;
            f30 = 240.0f;
            f29 = 100.0f;
            break;
        case 2:
            f31 = -304.0f;
            f30 = -240.0f;
            f29 = 100.0f;
            break;
        default:
            f31 = 304.0f;
            f30 = 240.0f;
            f29 = 100.0f;
            break;
    }
    if (animGroupBaseAsync(((CreditsNpcInfo*)R_credits_npc_info)[idx].name, inBattle, NULL) == 0) {
        return EVT_RETURN_BLOCK;
    }
    switch (entry->state) {
        case 0:
            {
                u32 type;

                EXIGetType(0, 2, &type);
                if (type != 0x05070000) {
                    entry->poseId = animPoseEntry(((CreditsNpcInfo*)R_credits_npc_info)[idx].name, inBattle);
                    animPoseSetAnim(entry->poseId, ((CreditsNpcInfo*)R_credits_npc_info)[idx].anim, 1);
                } else {
                    entry->poseId = animPoseEntry("a_mario", inBattle);
                    animPoseSetAnim(entry->poseId, "M_W_1", 1);
                }
            }
            entry->state++;
            break;
        case 1:
            entry->frame += 0.15f;
            if (entry->frame > entry->endFrame) {
                animPoseRelease(entry->poseId);
                _mapFree(entry);
                return EVT_RETURN_DONE;
            }
            break;
    }
    PSMTXTrans(mtx1, 0.0f, f29, 0.0f);
    PSMTXRotRad(mtx2, 'z', 0.017453292f * entry->frame);
    PSMTXConcat(mtx2, mtx1, mtx1);
    PSMTXTrans(mtx2, 0.5f * f31, 0.5f * f30, 0.0f);
    PSMTXConcat(mtx2, mtx1, entry->matrix);
    PSMTXScale(mtx3, 1.0f, 1.0f, 0.01f);
    PSMTXConcat(entry->matrix, mtx3, entry->matrix);
    if (animPoseGetVivianType(entry->poseId) != 0) {
        Vec pos = {0.0f, 0.0f, 0.0f};

        pos.x = entry->matrix[0][3];
        pos.y = entry->matrix[1][3];
        pos.z = entry->matrix[2][3];
        animPoseVivianMain(entry->poseId, &pos);
    }
    if (strcmp(animPoseGetCurrentAnim(entry->poseId)->name, "GAZ_A_1") == 0) {
        if (animPoseGetLoopTimes(entry->poseId) > 0.9f) {
            animPoseSetAnim(entry->poseId, "GAZ_A_1", 1);
        }
    }
    animPoseMain(entry->poseId);
    dispEntry(4, 0.0f, 1, unk_end_000023ac, entry);
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(unk_evt_end_00004044)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(unk_end_000022e8, LW(0), LW(1))
        BEGIN_INLINE_EVENT
            CALL(unk_end_00001d90, LW(0))
        END_INLINE_EVENT
        IF_INT_EQUAL(LW(1), 1)
            WAIT_MS(1920)
        END_IF
        IF_INT_EQUAL(LW(1), 2)
            WAIT_MS(6000)
        END_IF
        ADD(LW(0), 1)
        IF_INT_GREATER_OR_EQUAL(LW(0), 152)
            DEBUG_PUT_MSG(STRING("あにめおわり"))
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(GF(10), 1)
            RETURN
        END_IF
    END_LOOP
    RETURN
EVT_END()

#define WORK ((CreditsWork*)dat_ptr_end_00003218[0])

// The original builds each GXColor in a stack temp and copies it to the by-value
// argument slot; doing the build and the call in an inline reproduces that pair.
static inline void setChanMatColorGX(const GXColor* base, u8 a) {
    GXColor c = *base;
    c.a = a;
    GXSetChanMatColor(4, c);
}
static inline void setTevColorGX(const GXColor* base, u8 a) {
    GXColor c = *base;
    c.a = a;
    GXSetTevColor(1, c);
}

// NONMATCHING (98.72%): mode test compiles to the inverted diamond (`beq then; b else; then` in retail
// vs `bne else; then` here) and the original copies camPos through a second Vec temp (0x38 -> 0x5c)
// before C_MTXLookAt; a separate Vec copy fixes the frame but lands the other slots wrong.
void unk_end_00001324(CameraId cameraId, void* param) {
    CameraEntry* cam = camGetPtr(cameraId);
    s32 var1 = evtGetValue(NULL, GW(11));
    s32 var2 = evtGetValue(NULL, GW(12));
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    GXColor color;
    Vec camPos;
    Vec camUp;
    Vec target;
    f32 width;
    f32 height;
    s32 i;

    if (WORK->image == NULL) {
        return;
    }
    if ((u32)var1 >= (**(TEXPalette***)((u8*)WORK->image + 0xA0))->numDescriptors) {
        return;
    }
    if (var1 == 9 && swGet(0x5E7) != 0) {
        var1 = 0x35;
    }

    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(1, 3, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);

    color = (GXColor){0xFF, 0xFF, 0xFF, 0x00};
    color.a = WORK->field_08;
    GXSetChanMatColor(4, color);

    switch (WORK->mode) {
        case 0:
            TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, var1);
            GXLoadTexObj(&texObj, 0);
            break;
        case 1:
            color = (GXColor){0x00, 0x00, 0x00, 0x00};
            color.a = WORK->field_10;
            GXSetTevColor(1, color);
            TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, 0x31);
            GXLoadTexObj(&texObj, 0);
            TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, 0x32);
            GXLoadTexObj(&texObj, 1);
            break;
        case 2:
            color = (GXColor){0x00, 0x00, 0x00, 0x00};
            color.a = WORK->field_10;
            GXSetTevColor(1, color);
            TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, 0x32);
            GXLoadTexObj(&texObj, 0);
            TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, 0x33);
            GXLoadTexObj(&texObj, 1);
            break;
        default:
            break;
    }

    switch (WORK->mode) {
        case 0:
            GXSetNumTevStages(1);
            GXSetTevOrder(0, 0, 0, 4);
            GXSetTevOp(0, 0);
            GXSetTevSwapMode(0, 0, 0);
            break;
        default:
            GXSetNumTevStages(2);
            GXSetTevOrder(0, 0, 0, 4);
            GXSetTevColorOp(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
            GXSetTevAlphaIn(0, 7, 7, 7, 5);
            GXSetTevOrder(1, 0, 1, 0xFF);
            GXSetTevColorOp(1, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
            GXSetTevColorIn(1, 0, 8, 3, 0xF);
            GXSetTevAlphaIn(1, 7, 7, 7, 0);
            break;
    }

    switch (var2) {
        case 0:
            PSMTXTrans(mtx1, 51.0f, 30.0f, -30.0f);
            break;
        case 1:
            PSMTXTrans(mtx1, -51.0f, -30.0f, -30.0f);
            break;
        case 2:
            PSMTXTrans(mtx1, -51.0f, 30.0f, -30.0f);
            break;
        case 3:
            PSMTXTrans(mtx1, 51.0f, -30.0f, -30.0f);
            break;
        case 4:
            PSMTXTrans(mtx1, 0.0f, 30.0f, -30.0f);
            break;
        default:
            break;
    }

    PSMTXScale(mtx2, 0.6f, 0.6f, 1.0f);
    PSMTXConcat(mtx1, mtx2, mtx1);
    PSMTXConcat(cam->view, mtx1, mtx1);
    GXLoadPosMtxImm(mtx1, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            color = (GXColor){0xFF, 0xFF, 0xFF, 0x00};
            color.a = WORK->field_08;
            GXSetChanMatColor(4, color);
            GXSetZMode(1, 3, 0);
        } else {
            color = (GXColor){0x00, 0x00, 0x00, 0x00};
            color.a = WORK->field_08;
            GXSetChanMatColor(4, color);
            GXSetZMode(1, 4, 0);
        }
        GXBegin(0x80, 0, 4);
        width = (f32)GXGetTexObjWidth(&texObj);
        height = (f32)GXGetTexObjHeight(&texObj);
        GXPosition3f32(-width / 2.0f, height / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(width / 2.0f, height / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(width / 2.0f, -height / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-width / 2.0f, -height / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }

    if (WORK->field_0C > 0) {
        camPos = (Vec){0.0f, 0.0f, 0.0f};
        camPos.z = 240.0f / (f32)tan(0.21816615760326385);
        camUp = (Vec){0.0f, 1.0f, 0.0f};
        target = (Vec){0.0f, 0.0f, 0.0f};
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 0);
        TEXGetGXTexObjFromPalette(**(TEXPalette***)((u8*)WORK->image + 0xA0), &texObj, 0x34);
        GXLoadTexObj(&texObj, 0);
        color = (GXColor){0xFF, 0xFF, 0xFF, 0x00};
        color.a = WORK->field_0C;
        GXSetChanMatColor(4, color);
        GXSetZMode(1, 3, 0);
        C_MTXLookAt(mtx3, &camPos, &camUp, &target);
        PSMTXTrans(mtx1, 0.0f, -138.0f, 0.0f);
        PSMTXScale(mtx2, 1.0f, 1.0f, 1.0f);
        PSMTXConcat(mtx3, mtx1, mtx1);
        PSMTXConcat(mtx1, mtx2, mtx1);
        GXLoadPosMtxImm(mtx1, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x80, 0, 4);
        width = (f32)GXGetTexObjWidth(&texObj);
        height = (f32)GXGetTexObjHeight(&texObj);
        GXPosition3f32(-width / 2.0f, height / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(width / 2.0f, height / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(width / 2.0f, -height / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-width / 2.0f, -height / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}

EVT_BEGIN(unk_end_0000410c)
    LOOP(0)
        CALL(unk_end_0000122c, 1)
        CALL(unk_end_000011f0, 0)
        WAIT_MS(18000)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000011f0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(unk_end_000011f0, 255)
        WAIT_MS(4000)
        CALL(unk_end_0000122c, 2)
        CALL(unk_end_000011f0, 0)
        WAIT_MS(4000)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000011f0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(unk_end_000011f0, 255)
        WAIT_MS(18000)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000011f0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(unk_end_000011f0, 0)
        WAIT_MS(4000)
        CALL(unk_end_0000122c, 1)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000011f0, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()
s32 unk_end_000012e0(EventEntry* event, BOOL isFirstCall) {
    dispEntry(4, 0.0f, 7, unk_end_00001324, NULL);
    return EVT_RETURN_BLOCK;
}

s32 unk_end_000012a4(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)dat_ptr_end_00003218[0])->field_08 = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 unk_end_00001268(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)dat_ptr_end_00003218[0])->field_0C = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 unk_end_0000122c(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)dat_ptr_end_00003218[0])->mode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 unk_end_000011f0(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)dat_ptr_end_00003218[0])->field_10 = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(unk_evt_end_000042e0)
    LOOP(0)
        IF_INT_EQUAL(GF(10), 1)
            RETURN
        END_IF
        LOOP(0)
            IF_INT_EQUAL(GW(10), 1)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        BEGIN_INLINE_EVENT_GET_ID(LW(9))
            CALL(unk_end_000012e0)
        END_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000012a4, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        LOOP(0)
            IF_INT_EQUAL(GW(10), 2)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        IF_INT_EQUAL(GW(11), 49)
            WAIT_MS(500)
            CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value)
                CALL(unk_end_00001268, LW(0))
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(1), 0)
                    BREAK_LOOP
                END_IF
            END_LOOP
            WAIT_MS(3000)
            SET_VALUE(GF(0), 0)
            BEGIN_INLINE_EVENT
                CALL(evt_snd_bgmoff_f, 512, 5000)
                WAIT_MS(5000)
                IF_INT_EQUAL(GF(0), 0)
                    CALL(evt_snd_bgmon, 513, STRING("BGM_EVT_PEACH1"))
                END_IF
            END_INLINE_EVENT
            WAIT_MS(500)
            SPAWN_EVENT_GET_ID(PTR(unk_end_0000410c), LW(10))
            LOOP(0)
                CALL(evt_key_get_buttontrg, 0, LW(0))
                IF_BITS_SET(LW(0), 4352)
                    BREAK_LOOP
                END_IF
                WAIT_FRAMES(1)
            END_LOOP
            SET_VALUE(GF(0), 1)
            DELETE_EVENT(LW(10))
            CALL(evt_fade_entry, 10, 500, 0, 0, 0)
            CALL(evt_fade_end_wait)
            CALL(unk_end_0000122c, 0)
            CALL(unk_end_000012a4, LW(0))
            CALL(unk_end_00001268, LW(0))
            CALL(evt_fade_entry, 9, 0, 0, 0, 0)
            CALL(evt_fade_end_wait)
            SET_VALUE(GW(10), 0)
            SPAWN_EVENT(unk_evt_end_000045cc)
            RETURN
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(unk_end_000012a4, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        DELETE_EVENT(LW(9))
        SET_VALUE(GW(10), 0)
    END_LOOP
    RETURN
EVT_END()

typedef struct CreditsEndingRecord {
    /* 0x00 */ s32 field_0;
    /* 0x04 */ s32 field_4;
    /* 0x08 */ s32 field_8;
    /* 0x0C */ const char* msgA[3];
    /* 0x18 */ const char* msgB[3];
} CreditsEndingRecord;

// NONMATCHING (97.24%): frame 0x850 vs 0x860; MWCC folds `offset` into the live record
// pointer (a temp, not a callee-saved GPR), so only 12 of the original 13 callee-saved
// regs are used, `event` lands in r29 not r27, and GW(11) reads through rec not base+offset.
s32 unk_end_00000bc8(EventEntry* event, BOOL isFirstCall) {
    CreditsEndingRecord* rec;
    s32* pEntry;
    s32 i;
    s32 offset;
    s32 x;
    s32 y;
    char bufA[0x400];
    char bufB[0x400];
    s32 state = event->userdata[0];
    s32 index = event->userdata[1];

    if (isFirstCall) {
        state = 0;
        index = 0;
        for (i = 0; i < 3; i++) {
            ((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i] = -1;
        }
        for (i = 0; i < 3; i++) {
            ((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i] = -1;
        }
        evtSetValue(event, GW(10), 0);
        ((CreditsWork*)dat_ptr_end_00003218[0])->eventId = (s32)evtEntry(unk_evt_end_000042e0, 0, 0);
    }
    pEntry = (s32*)((u8*)unk_end_0000321c + index * 0x24);
    ((CreditsWork*)dat_ptr_end_00003218[0])->entryIdx = *++pEntry;
    offset = index * 0x24;
    switch (((CreditsWork*)dat_ptr_end_00003218[0])->entryIdx) {
        case 0:
            x = -274;
            y = -24;
            break;
        case 1:
            x = 274;
            y = 250;
            break;
        case 2:
            x = 274;
            y = -24;
            break;
        case 3:
            x = -274;
            y = 250;
            break;
        case 4:
            x = 0;
            y = -60;
            break;
        default:
            break;
    }
    switch (state) {
        case 0:
            rec = (CreditsEndingRecord*)((u8*)unk_end_0000321c + offset);
            for (i = 0; i < 3; i++) {
                const char* key = rec->msgA[i];

                if (key != NULL) {
                    const char* msg = msgSearch(key);
                    s32 ypos = y - i * 0x16;
                    s32 xpos = ypos - 4;
                    s32 xoff = x;

                    if (x == 0) {
                        xoff = (s32)((f32)x - 0.8f * (f32)FontGetMessageWidth(msg) * 0.5f);
                    } else if (x > 0) {
                        xoff = (s32)((f32)x - 0.8f * (f32)FontGetMessageWidth(msg));
                    }
                    sprintf(bufA, "<clear><dkey><wpos %d %d 400 200><col f0f0f0ff><speed 0><scale 0.8>%s</scale></speed></col><wait 500></dkey><once_stop>", xpos, xoff - 0x3E, msg);
                    ((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i] = msgWindow_Entry(bufA, 0, 0);
                }
            }
            for (i = 0; i < 3; i++) {
                const char* key = rec->msgB[i];

                if (key != NULL) {
                    const char* msg = msgSearch(key);
                    s32 ypos = y - i * 0x18;
                    s32 xpos = ypos - 0x32;
                    s32 xoff = x;

                    if (x == 0) {
                        xoff = (s32)((f32)x - 0.5f * (f32)FontGetMessageWidth(msg));
                    } else if (x > 0) {
                        xoff = x - FontGetMessageWidth(msg);
                    }
                    sprintf(bufB, "<clear><dkey><wpos %d %d 400 200><col f0f0f0ff><speed 0>%s</speed></col><wait 5200></dkey><once_stop>", xpos, xoff - 0x3E, msg);
                    ((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i] = msgWindow_Entry(bufB, 0, 0);
                } else {
                    ((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i] = -1;
                }
            }
            evtSetValue(event, GW(10), 1);
            evtSetValue(event, GW(11), ((CreditsEndingRecord*)((u8*)unk_end_0000321c + offset))->field_8);
            evtSetValue(event, GW(12), *pEntry);
            state++;
            break;
        case 1:
            for (i = 0; i < 3; i++) {
                if (((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i] != -1 && windowCheckID(((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i]) != 0) {
                    break;
                }
            }
            if (i >= 3) {
                for (i = 0; i < 3; i++) {
                    if (((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i] != -1 && windowCheckID(((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i]) != 0) {
                        break;
                    }
                }
                if (i >= 3) {
                    state++;
                }
            }
            break;
        case 2:
            {
                s32 next = index + 1;
                s32 nulls;

                if ((u32)next < 0x32) {
                    CreditsEndingRecord* nextRec = (CreditsEndingRecord*)((u8*)unk_end_0000321c + next * 0x24);

                    nulls = 0;
                    for (i = 0; i < 3; i++) {
                        if (nextRec->msgA[i] != NULL) {
                            break;
                        }
                        nulls++;
                    }
                } else {
                    nulls = 0;
                }
                if (nulls < 3) {
                    for (i = 0; i < 3; i++) {
                        if (((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i] != -1) {
                            msgWindow_Continue(((CreditsWork*)dat_ptr_end_00003218[0])->windowIdA[i]);
                        }
                    }
                }
                for (i = 0; i < 3; i++) {
                    if (((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i] != -1) {
                        msgWindow_Continue(((CreditsWork*)dat_ptr_end_00003218[0])->windowIdB[i]);
                    }
                }
                evtSetValue(event, GW(10), 2);
                if ((u32)next < 0x32) {
                    state++;
                } else {
                    state += 2;
                }
            }
            break;
        case 3:
            if (evtGetValue(event, GW(10)) == 0) {
                state = 0;
                index++;
            }
            break;
        case 4:
            return EVT_RETURN_DONE;
    }
    event->userdata[0] = state;
    event->userdata[1] = index;
    return EVT_RETURN_BLOCK;
}

s32 unk_end_00000a6c(EventEntry* event, BOOL isFirstCall) {
    s32 flag = (gp->inBattle != 0);
    s32 i;

    if (isFirstCall) {
        ((CreditsWork*)dat_ptr_end_00003218[0])->image = NULL;
        ((CreditsWork*)dat_ptr_end_00003218[0])->field_08 = 0;
        ((CreditsWork*)dat_ptr_end_00003218[0])->mode = 0;
        ((CreditsWork*)dat_ptr_end_00003218[0])->entryIdx = 0;
        psndSetFlag(4);
        gp->language = 1;
        JUTFont_Free();
        JUTFontSetup(1);
    }
    if ((void*)fileAsyncf(4, NULL, "%s/ending.tpl", getMarioStDvdRoot()) == NULL) {
        return EVT_RETURN_BLOCK;
    }
    if (((CreditsWork*)dat_ptr_end_00003218[0])->image == NULL) {
        ((CreditsWork*)dat_ptr_end_00003218[0])->image = fileAllocf(4, "%s/ending.tpl", getMarioStDvdRoot());
    }
    for (i = 0; i < 5; i++) {
        if (animGroupBaseAsync(((CreditsNpcInfo*)R_credits_npc_info)[i].name, flag, NULL) == 0) {
            return EVT_RETURN_BLOCK;
        }
    }
    for (i = 0; i < 5; i++) {
    }
    return EVT_RETURN_DONE;
}

// dont_inline keeps unk_end_00000a14 a real `bl` from _epilog as in retail.
#pragma dont_inline on
void unk_end_00000a14(void) {
    CreditsWork* work = (CreditsWork*)dat_ptr_end_00003218[0];

    fileFree(work->image);
    psndClearFlag(4);
    gp->language = 0;
    JUTFont_Free();
    JUTFontSetup(0);
}

#pragma dont_inline reset
s32 unk_end_000009cc(EventEntry* event, BOOL isFirstCall) {
    gp->language = 0;
    JUTFont_Free();
    JUTFontSetup(0);
    psndClearFlag(4);
    return EVT_RETURN_DONE;
}

s32 unk_end_000009a4(EventEntry* event, BOOL isFirstCall) {
    psndSetFlag(0x1000);
    return EVT_RETURN_DONE;
}

s32 unk_end_00000938(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));

    mapSetTevCallback(1, unk_end_00002508);
    animPoseSetMaterialFlagOn(npc->poseId, 0x1000000);
    animPoseSetGXFunc(npc->poseId, unk_end_00002494, FALSE);
    return EVT_RETURN_DONE;
}

f32 unk_end_000008bc(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}

EVT_BEGIN(unk_evt_end_000045cc)
    CALL(evt_fade_entry, 12, 2000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1500)
    SET_VALUE(GF(10), 1)
    CALL(unk_end_000009cc)
    SET_VALUE(GSW(0), 404)
    SET_VALUE(LF(10), 0)
    SET_VALUE(LW(0), 21)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_snd_bgmoff_f, 512, 2000)
    CALL(evt_snd_bgmoff_f, 513, 2000)
    CALL(evt_fade_set_mapchange_type, 1, 17, 2000, -1, -1)
    CALL(evt_seq_set_seq, 3, STRING("title"), 0)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004690)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam3d_evt_set, 0, 0, 545, 0, 0, 0, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("pPlane1"), 1)
    CALL(evt_bg_set_color, 0, 0, 0, 255)
    SET_VALUE(GF(10), 0)
    CALL(unk_end_00000a6c)
    SPAWN_EVENT(unk_evt_end_00004044)
    BEGIN_INLINE_EVENT
        CALL(unk_end_00000bc8)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 512, STRING("BGM_STAFF_ROLL1"))
    SET_VALUE(GSW(0), 404)
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_0000478c)
    CALL(evt_npc_entry, STRING("gesso"), STRING("c_gesso"))
    CALL(evt_npc_set_anim, STRING("gesso"), STRING("GSO_S2_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("gesso"), 64)
    CALL(unk_end_00000938, STRING("gesso"))
    CALL(evt_sub_intpl_msec_init, 11, -4000, 4000, 5000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("gesso"), 130, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("gesso"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_0000484c)
    CALL(evt_npc_entry, STRING("gonbaba"), STRING("c_gonbaba"))
    CALL(evt_npc_set_anim, STRING("gonbaba"), STRING("GNB_S_3"))
    CALL(evt_npc_flag_onoff, 1, STRING("gonbaba"), 64)
    CALL(unk_end_00000938, STRING("gonbaba"))
    CALL(evt_npc_set_scale, STRING("gonbaba"), FLOAT(0.5f), FLOAT(0.5f), FLOAT(0.5f))
    CALL(evt_npc_set_rotate, STRING("gonbaba"), 0, 180, 0)
    CALL(evt_sub_intpl_msec_init, 11, -10000, -1300, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("gonbaba"), LW(0), -20, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_sub_intpl_msec_init, 11, -1300, -10000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("gonbaba"), LW(0), -20, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("gonbaba"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_000049b0)
    CALL(evt_npc_entry, STRING("batten2"), STRING("c_mb_robo_2a"))
    CALL(evt_npc_set_anim, STRING("batten2"), STRING("J_2"))
    CALL(evt_npc_flag_onoff, 1, STRING("batten2"), 64)
    CALL(unk_end_00000938, STRING("batten2"))
    CALL(evt_npc_set_rotate, STRING("batten2"), 180, 0, 0)
    CALL(evt_sub_intpl_msec_init, 11, 4000, -4000, 4000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("batten2"), 100, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("batten2"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004a88)
    CALL(evt_npc_entry, STRING("atomic"), STRING("c_atmic_trs"))
    CALL(evt_npc_set_anim, STRING("atomic"), STRING("W_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("atomic"), 32832)
    CALL(unk_end_00000938, STRING("atomic"))
    CALL(evt_npc_set_rotate, STRING("atomic"), 0, 180, 0)
    CALL(evt_npc_set_scale, STRING("atomic"), FLOAT(0.8f), FLOAT(0.8f), FLOAT(0.8f))
    CALL(evt_sub_intpl_msec_init, 11, -5000, 5000, 6000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("atomic"), LW(0), 20, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("atomic"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004b78)
    CALL(evt_npc_entry, STRING("korutesu"), STRING("c_korutesu1"))
    CALL(evt_npc_set_anim, STRING("korutesu"), STRING("KRT_S_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("korutesu"), 32832)
    CALL(evt_npc_set_ry, STRING("korutesu"), 90)
    CALL(unk_end_00000938, STRING("korutesu"))
    CALL(evt_sub_intpl_msec_init, 11, -10000, -2000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("korutesu"), LW(0), -150, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_sub_intpl_msec_init, 11, -2000, -10000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("korutesu"), LW(0), -150, 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("korutesu"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004cbc)
    WAIT_MS(1000)
    CALL(evt_npc_entry, STRING("para"), STRING("c_bom_sen"))
    CALL(evt_npc_set_anim, STRING("para"), STRING("S_2"))
    CALL(evt_npc_flag_onoff, 1, STRING("para"), 32832)
    CALL(unk_end_00000938, STRING("para"))
    CALL(evt_sub_intpl_msec_init, 11, 3000, -3000, 6000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("para"), 50, LW(0), 0)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("para"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004d84)
    CALL(evt_npc_entry, STRING("batten"), STRING("c_mb_robo_1a"))
    CALL(evt_npc_set_anim, STRING("batten"), STRING("W_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("batten"), 64)
    CALL(unk_end_00000938, STRING("batten"))
    CALL(evt_npc_set_scale, STRING("batten"), FLOAT(0.6f), FLOAT(0.6f), FLOAT(0.6f))
    CALL(evt_sub_intpl_msec_init, 11, 10000, 500, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("batten"), LW(0), -70, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_sub_intpl_msec_init, 11, 500, 10000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("batten"), LW(0), -70, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("batten"))
    RETURN
EVT_END()

EVT_BEGIN(unk_evt_end_00004ed0)
    WAIT_MS(7000)
    CALL(evt_npc_entry, STRING("queen"), STRING("c_q_kage_m"))
    CALL(evt_npc_set_anim, STRING("queen"), STRING("KRT_Z_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("queen"), 64)
    CALL(unk_end_00000938, STRING("queen"))
    CALL(evt_npc_set_scale, STRING("queen"), FLOAT(0.5f), FLOAT(0.5f), FLOAT(0.5f))
    CALL(evt_npc_set_ry, STRING("queen"), 90)
    CALL(evt_sub_intpl_msec_init, 11, -10000, -1000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("queen"), LW(0), -20, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_sub_intpl_msec_init, 11, -1000, -10000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("queen"), LW(0), -20, 200)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(evt_npc_delete, STRING("queen"))
    RETURN
EVT_END()

char* dat_ptr_end_00005034 = "c_n_moa_a";

char* dat_ptr_end_00005038 = "S_1";
void unk_end_000006f8(void) {
    ExtPose* pose = extGetPosePtr();
    CreditsPoseState* work = (CreditsPoseState*)unk_end_bss00000000[0];
    s32 i;

    for (i = 0; i < extGetPoseNum(); i++) {
        pose->flags |= 5;
        PSMTXIdentity(*pose->matrix);
        {
            Vec pos = {0.0f, 0.0f, 0.0f};

            pos.x = unk_end_000008bc(-10.0f, 10.0f);
            pos.y = unk_end_000008bc(-10.0f, 10.0f);
            pos.z = unk_end_000008bc(-10.0f, 10.0f);
            work->position = pos;
        }
        work->idx = i % dat_80419a28;
        work->counter = ((s32*)unk_end_bss00000008[0])[work->idx]++;
        work++;
        pose++;
    }
}

// The declaration order of the loop induction vars (i, extPose, work) and the final
// zero-fill pointer (count before p) sets the callee-saved register assignment the
// original uses; the indices into unk_end_bss0000000c are kept as a pointer pair.
void unk_end_00000440(void) {
    NpcEntry* npc = npcNameToPtr("モアモア");
    AnimationPose* pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    Vec* positions = (Vec*)(*(smartEntry**)pose->field_0x15C)->address;
    s32 i;
    ExtPose* extPose;
    CreditsPoseState* work;
    Mtx mtx1;
    Mtx mtx3;
    Mtx mtx2;
    Vec offset;
    Vec tmp;

    if (positions == NULL) {
        return;
    }
    extPose = extGetPosePtr();
    work = (CreditsPoseState*)unk_end_bss00000000[0];
    for (i = 0; i < extGetPoseNum(); i++) {
        s32* idxPair = (s32*)unk_end_bss0000000c[0] + work->idx * 2;
        Vec* p0 = &positions[idxPair[0]];
        Vec* p1 = &positions[idxPair[1]];
        f32 ratio = (f32)work->counter / (f32)((s32*)unk_end_bss00000008[0])[work->idx];
        f32 c = 1.0f - ratio;

        if (p0->y == -1000.0f || p1->y == -1000.0f) {
            offset = (Vec){0.0f, -1000.0f, 0.0f};
            extPose->flags &= ~1;
        } else {
            PSVECScale(p0, &offset, ratio);
            PSVECScale(p1, &tmp, c);
            PSVECAdd(&offset, &tmp, &offset);
            extPose->flags |= 1;
        }
        PSMTXTrans(mtx1, offset.x, offset.y, offset.z);
        PSMTXScale(mtx2, npc->scale.x, npc->scale.y, npc->scale.z);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXTrans(mtx3, work->position.x, work->position.y, work->position.z);
        PSMTXConcat(mtx1, mtx3, *extPose->matrix);
        extPose->field_0x10 = (gp->retraceCount + i) % 40;
        work++;
        extPose++;
    }

    pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    {
        s32 count = *(s32*)(pose->field_0x15C + 4);
        Vec* p = (Vec*)(*(smartEntry**)pose->field_0x15C)->address;

        for (i = 0; i < count; i++) {
            p[i] = (Vec){0.0f, -1000.0f, 0.0f};
        }
    }
}

// (bss arrays are declared uninitialized at the top of the file)
void unk_end_0000039c(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xC);
    animGetPtr()->currDispMode = 0;
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
    extDraw();
    extLoadTextureExit();
}

void unk_end_0000035c(void) {
    dispEntry(4, 0.0f, 1, unk_end_0000039c, NULL);
}

s32 unk_end_0000012c(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("モアモア");
    AnimationPose* pose;
    Vec* positions;
    void* p;
    s32 i;
    s32 count;

    p = _mapAlloc(0x1770);
    unk_end_bss00000000[0] = (u32)p;
    memset(p, 0, 0x1770);
    animPoseWorldPositionEvalOn(npc->poseId);
    p = _mapAlloc(0x1EC);
    unk_end_bss00000004[0] = (u32)p;
    memset(p, 0, 0x1EC);
    for (i = 0; i < 0x28; i++) {
        ((s32*)unk_end_bss00000004[0])[i * 3 + 0] = (s32)dat_ptr_end_00005034;
        ((s32*)unk_end_bss00000004[0])[i * 3 + 1] = (s32)dat_ptr_end_00005038;
        ((f32*)unk_end_bss00000004[0])[i * 3 + 2] = (f32)i;
    }
    evtSetValue(event, LW(10), unk_end_bss00000004[0]);

    pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    {
        smartEntry* entry = *(smartEntry**)pose->field_0x15C;

        positions = (Vec*)entry->address;
    }
    count = *(s32*)(pose->field_0x15C + 4);
    for (i = 0; i < count; i++) {
        positions[i] = (Vec){0.0f, -1000.0f, 0.0f};
    }

    p = _mapAlloc(dat_80419a28 * 4);
    unk_end_bss00000008[0] = (u32)p;
    memset(p, 0, dat_80419a28 * 4);
    p = _mapAlloc(dat_80419a28 * 8);
    unk_end_bss0000000c[0] = (u32)p;
    memset(p, 0, dat_80419a28 * 8);
    for (i = 0; i < dat_80419a28; i++) {
        ((s32*)unk_end_bss0000000c[0])[i * 2 + 0] = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].name);
        ((s32*)unk_end_bss0000000c[0])[i * 2 + 1] = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].anim);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(unk_evt_end_0000503c)
    CALL(evt_npc_entry, STRING("モアモアダミー"), STRING("c_n_moa_a"))
    CALL(evt_npc_set_position, STRING("モアモアダミー"), 0, -1000, 0)
    CALL(unk_end_00000938, STRING("モアモアダミー"))
    CALL(evt_npc_entry, STRING("モアモア"), STRING("c_g_moamoa"))
    CALL(evt_npc_set_anim, STRING("モアモア"), STRING("S_3"))
    CALL(evt_npc_set_position, STRING("モアモア"), 0, -1000, 0)
    CALL(evt_npc_set_scale, STRING("モアモア"), FLOAT(0.5f), FLOAT(0.5f), FLOAT(0.5f))
    CALL(unk_end_0000012c)
    CALL(evt_ext_entry, 300, LW(10), PTR(unk_end_000006f8), PTR(unk_end_00000440), PTR(unk_end_0000035c))
    CALL(evt_sub_intpl_msec_init, 11, -10000, -1000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("モアモア"), 0, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    WAIT_MS(3000)
    CALL(evt_sub_intpl_msec_init, 11, -1000, -10000, 2000)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        DIVIDE_FLOAT(LW(0), FLOAT(10.0f))
        CALL(evt_npc_set_position, STRING("モアモア"), 0, LW(0), 100)
        WAIT_FRAMES(1)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
    END_LOOP
    CALL(unk_end_000000c4)
    CALL(evt_npc_delete, STRING("モアモアダミー"))
    CALL(evt_npc_delete, STRING("モアモア"))
    RETURN
EVT_END()
s32 unk_end_000000c4(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("モアモア");

    L_animPoseWorldPositionEvalOff(npc->poseId);
    _mapFree((void*)unk_end_bss00000000[0]);
    extReset();
    _mapFree((void*)unk_end_bss0000000c[0]);
    _mapFree((void*)unk_end_bss00000008[0]);
    return EVT_RETURN_DONE;
}

extend_data(4,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(5,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: write gap_00_00002520_text (global, 0x10 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it
// clang-format on
