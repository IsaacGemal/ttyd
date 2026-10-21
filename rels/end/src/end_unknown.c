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


// "end" REL credits work block (the target of credits_work_ptr)
typedef struct CreditsWork {
    /* 0x00 */ FileEntry* image;  // ending.tpl; *data is its TEXPalette*
    /* 0x04 */ s32 mode;
    /* 0x08 */ s32 matAlpha;      // channel material alpha (setChanMatColorGX) of the main image passes
    /* 0x0C */ s32 overlayAlpha;  // material alpha of the last overlay pass, which runs only when > 0
    /* 0x10 */ s32 tevAlpha;      // TEV colour 1 alpha for modes 1 and 2 (setTevColorGX)
    /* 0x14 */ s32 windowIdA[3];
    /* 0x20 */ s32 windowIdB[3];
    /* 0x2C */ s32 pad_0x2C;
    /* 0x30 */ s32 layout;        // CreditsEndingRecord.layout of the record on screen
    /* 0x34 */ s32 eventId;
} CreditsWork;

// R_credits_npc_info entries
typedef struct CreditsNpcInfo {
    /* 0x00 */ const char* name;
    /* 0x04 */ const char* anim;
    /* 0x08 */ s32* event;
} CreditsNpcInfo;

// per-pose entry allocated by credits_pose_evt for the credits animation callback
typedef struct CreditsPoseEntry {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 poseId;
    /* 0x08 */ f32 frame;
    /* 0x0C */ f32 endFrame;
    /* 0x10 */ s32 layout;
    /* 0x14 */ Mtx matrix;
} CreditsPoseEntry;


#pragma pool_data off

#pragma force_active on
s32 evt_credits_end[49];
s32 evt_credits_gesso[48];
s32 evt_credits_gonbaba[89];
s32 evt_credits_batten2[54];
s32 evt_credits_atomic[60];
s32 evt_credits_korutesu[81];
s32 evt_credits_para[50];
s32 evt_credits_batten[83];
s32 evt_credits_queen[89];
s32 evt_credits_moamoa[108];
// .bss emitted in reverse source order by the deferred-inline compiler, so declare them
// back-to-front to land at 0x00..0x10 as in the original.
u32 credits_work[14];
u32 credits_chain_groups[1];
u32 credits_chain_counts[1];
u32 credits_ext_setup[1];
u32 credits_pose_states[1];

// clang-format off
// TODO: type
u32 credits_work_ptr[1] = {
    (u32)&credits_work,
};

// TODO: type
u32 credits_ending_records[450] = {
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
    (u32)"c_gesso", (u32)"GSO_S2_1", (u32)&evt_credits_gesso, (u32)"c_joho_b",
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
    (u32)"c_gonbaba", (u32)"GNB_S_3", (u32)&evt_credits_gonbaba, (u32)"c_flower",
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
    (u32)&evt_credits_batten, (u32)"c_puni", (u32)"W_1", 0x00000000,
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
    0x00000000, (u32)"c_atmic_trs", (u32)"W_1", (u32)&evt_credits_atomic,
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
    (u32)&evt_credits_korutesu, (u32)"c_dance_r", (u32)"W_1", 0x00000000,
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
    (u32)&evt_credits_moamoa, (u32)"c_papab", (u32)"W_1", 0x00000000,
    (u32)"c_mamab", (u32)"W_1", 0x00000000, (u32)"c_konari",
    (u32)"W_1", 0x00000000, (u32)"c_unten", (u32)"W_1",
    0x00000000, (u32)"c_shasho_r", (u32)"W_1", 0x00000000,
    (u32)"c_smoko", (u32)"W_1", 0x00000000, (u32)"c_rose",
    (u32)"W_1", 0x00000000, (u32)"c_kinodebu", (u32)"W_1",
    0x00000000, (u32)"c_ghost_k", (u32)"W_1", 0x00000000,
    (u32)"c_bom_sen", (u32)"S_2", (u32)&evt_credits_para, (u32)"c_waitress",
    (u32)"W_1", 0x00000000, (u32)"c_puku_c", (u32)"W_1",
    0x00000000, (u32)"c_kinopiko", (u32)"KNF_W_1", 0x00000000,
    (u32)"c_n_moa_a", (u32)"W_1", 0x00000000, (u32)"c_met",
    (u32)"PMT_W_1", 0x00000000, (u32)"c_sanbo", (u32)"SNB_W_1",
    0x00000000, (u32)"c_bomson", (u32)"W_1", 0x00000000,
    (u32)"c_mb_robo_2a", (u32)"J_2", (u32)&evt_credits_batten2, (u32)"c_bwhite",
    (u32)"W_1", 0x00000000, (u32)"c_hito_b", (u32)"W_1",
    0x00000000, (u32)"c_bom_n", (u32)"W_1", 0x00000000,
    (u32)"c_kmoon", (u32)"W_1", 0x00000000, (u32)"c_baria",
    (u32)"W_1", 0x00000000, (u32)"c_basabasa", (u32)"BSA_W_1",
    0x00000000, (u32)"c_wanwan", (u32)"WAN_A_1", 0x00000000,
    (u32)"c_maho", (u32)"MAH_W_1", 0x00000000, (u32)"c_zako_n",
    (u32)"W_1", 0x00000000, (u32)"c_zako_m", (u32)"W_1",
    0x00000000, (u32)"c_shuryo", (u32)"W_1", 0x00000000,
    (u32)"c_q_kage_m", (u32)"KRT_Z_1", (u32)&evt_credits_queen, (u32)"c_gazigazi",
    (u32)"GAZ_A_1", 0x00000000, (u32)"a_mario", (u32)"M_W_1",
    0x00000000, (u32)"c_peach", (u32)"P_W_1", 0x00000000,
};

void _prolog(void) {
    voidfunctionptr* ctor;

    for (ctor = _ctors; *ctor != NULL; ctor++) {
        (*ctor)();
    }
    relSetEvtAddr("end_00", evt_credits_start);
}

void _epilog(void) {
    voidfunctionptr* dtor;

    for (dtor = _dtors; *dtor != NULL; dtor++) {
        (*dtor)();
    }
    credits_free();
}

void _unresolved(void) {
}

void credits_tev_callback(MapTevCallback* param) {
    GXSetTevColorIn(param->tevStage - 1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
}

void credits_pose_gx(s32 wXluStage) {
    AnimationWork* anim = animGetPtr();

    anim->currDispMode = wXluStage;
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
}

void credits_pose_disp(CameraId cameraId, void* param) {
    CreditsPoseEntry* entry = (CreditsPoseEntry*)param;

    if (evtGetValue(NULL, GF(10)) == 0) {
        mapSetTevCallback(1, credits_tev_callback);
        animPoseSetMaterialFlagOn(entry->poseId, 0x1000000);
        animPoseSetGXFunc(entry->poseId, credits_pose_gx, FALSE);
        animPoseDrawMtx(entry->poseId, entry->matrix, 1, 0.0f, 2.0f);
        animPoseDrawMtx(entry->poseId, entry->matrix, 2, 0.0f, 2.0f);
        animPoseDrawMtx(entry->poseId, entry->matrix, 3, 0.0f, 2.0f);
        animPoseSetMaterialFlagOff(entry->poseId, 0x1000000);
    }
}

s32 credits_npc_kind(EventEntry* event, BOOL isFirstCall) {
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

// The early accesses write `off` (idx*0xC) so it stays in r29; the later name accesses use the local `npc` base
// + off (compiled to `lwzx`) while the `anim` access uses the constant R_credits_npc_info + off, which is what
// makes retail rematerialise the base instead of CSE-ing it with the name accesses.
s32 credits_pose_evt(EventEntry* event, BOOL isFirstCall) {
    s32 idx = evtGetValue(event, event->args[0]);
    s32 off = idx * 0xC;
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
            animGroupBaseAsync(((CreditsNpcInfo*)((u8*)R_credits_npc_info + off + 0x3C))->name, inBattle, NULL);
        }
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        entry = _mapAlloc(sizeof(CreditsPoseEntry));
        memset(entry, 0, sizeof(CreditsPoseEntry));
        entry->state = 0;
        entry->layout = ((CreditsWork*)credits_work_ptr[0])->layout;
        switch (entry->layout) {
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
            animGroupBaseAsync(((CreditsNpcInfo*)((u8*)R_credits_npc_info + off + 0x3C))->name, inBattle, NULL);
        }
        event->userdata[0] = (s32)entry;
    }
    entry = (CreditsPoseEntry*)event->userdata[0];
    switch (entry->layout) {
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
    {
        CreditsNpcInfo* npc = (CreditsNpcInfo*)R_credits_npc_info;

        if (animGroupBaseAsync(((CreditsNpcInfo*)((u8*)npc + off))->name, inBattle, NULL) == 0) {
            return EVT_RETURN_BLOCK;
        }
        switch (entry->state) {
        case 0:
            {
                u32 type;

                EXIGetType(0, 2, &type);
                if (type != 0x05070000) {
                    entry->poseId = animPoseEntry(((CreditsNpcInfo*)((u8*)npc + off))->name, inBattle);
                    animPoseSetAnim(entry->poseId, ((CreditsNpcInfo*)((u8*)R_credits_npc_info + off))->anim, 1);
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
    }
    PSMTXTrans(mtx1, 0.0f, f29, 0.0f);
    PSMTXRotRad(mtx2, 'z', 0.017453292f * entry->frame);
    PSMTXConcat(mtx2, mtx1, mtx1);
    PSMTXTrans(mtx2, f31 / 2.0f, f30 / 2.0f, 0.0f);
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
    dispEntry(4, 0.0f, 1, credits_pose_disp, entry);
    return EVT_RETURN_BLOCK;
}

EVT_BEGIN(evt_credits_pose_loop)
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(credits_npc_kind, LW(0), LW(1))
        BEGIN_INLINE_EVENT
            CALL(credits_pose_evt, LW(0))
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

#define WORK ((CreditsWork*)credits_work_ptr[0])


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

const GXColor credits_image_mat = {0xFF, 0xFF, 0xFF, 0x00};
const GXColor credits_mode1_tev = {0x00, 0x00, 0x00, 0x00};
const GXColor credits_mode2_tev = {0x00, 0x00, 0x00, 0x00};
const GXColor credits_pass0_mat = {0xFF, 0xFF, 0xFF, 0x00};
const GXColor credits_pass1_mat = {0x00, 0x00, 0x00, 0x00};
const Vec credits_overlay_cam_pos = {0.0f, 0.0f, 0.0f};
const Vec credits_overlay_cam_up = {0.0f, 1.0f, 0.0f};
const Vec credits_overlay_target = {0.0f, 0.0f, 0.0f};
const GXColor credits_overlay_mat = {0xFF, 0xFF, 0xFF, 0x00};

// The second GXBegin block uses its own width/height locals: with the loop's
// variables reused the width temp lands in f30, fresh ones put it in f29 like retail.
void credits_draw(CameraId cameraId, void* param) {
    s32 i;
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    Vec camPos;
    Vec camUp;
    Vec target;
    f32 width;
    f32 height;
    f32 width2;
    f32 height2;
    CameraEntry* cam = camGetPtr(cameraId);
    s32 var1 = evtGetValue(NULL, GW(11));
    s32 var2 = evtGetValue(NULL, GW(12));

    if (WORK->image == NULL) {
        return;
    }
    if ((u32)var1 >= (*(TEXPalette**)WORK->image->data)->numDescriptors) {
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

    setChanMatColorGX(&credits_image_mat, WORK->matAlpha);

    switch (WORK->mode) {
        case 0:
            TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, var1);
            GXLoadTexObj(&texObj, 0);
            break;
        case 1:
            setTevColorGX(&credits_mode1_tev, WORK->tevAlpha);
            TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, 0x31);
            GXLoadTexObj(&texObj, 0);
            TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, 0x32);
            GXLoadTexObj(&texObj, 1);
            break;
        case 2:
            setTevColorGX(&credits_mode2_tev, WORK->tevAlpha);
            TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, 0x32);
            GXLoadTexObj(&texObj, 0);
            TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, 0x33);
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
            setChanMatColorGX(&credits_pass0_mat, WORK->matAlpha);
            GXSetZMode(1, 3, 0);
        } else {
            setChanMatColorGX(&credits_pass1_mat, WORK->matAlpha);
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

    if (WORK->overlayAlpha > 0) {
        Vec camPosBuild;

        camPosBuild = credits_overlay_cam_pos;
        {
            f32 tanVal = (f32)tan(0.21816615760326385);
            camPosBuild.z = 240.0f / tanVal;
        }
        camPos = camPosBuild;
        camUp = credits_overlay_cam_up;
        target = credits_overlay_target;
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 0);
        TEXGetGXTexObjFromPalette(*(TEXPalette**)WORK->image->data, &texObj, 0x34);
        GXLoadTexObj(&texObj, 0);
        setChanMatColorGX(&credits_overlay_mat, WORK->overlayAlpha);
        GXSetZMode(1, 3, 0);
        C_MTXLookAt(mtx3, &camPos, &camUp, &target);
        PSMTXTrans(mtx1, 0.0f, -138.0f, 0.0f);
        PSMTXScale(mtx2, 1.0f, 1.0f, 1.0f);
        PSMTXConcat(mtx3, mtx1, mtx1);
        PSMTXConcat(mtx1, mtx2, mtx1);
        GXLoadPosMtxImm(mtx1, 0);
        GXSetCurrentMtx(0);
        GXBegin(0x80, 0, 4);
        width2 = (f32)GXGetTexObjWidth(&texObj);
        height2 = (f32)GXGetTexObjHeight(&texObj);
        GXPosition3f32(-width2 / 2.0f, height2 / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(width2 / 2.0f, height2 / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(width2 / 2.0f, -height2 / 2.0f, 0.0f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(-width2 / 2.0f, -height2 / 2.0f, 0.0f);
        GXTexCoord2f32(0.0f, 1.0f);
    }
}

EVT_BEGIN(evt_credits_fade)
    LOOP(0)
        CALL(credits_set_mode, 1)
        CALL(credits_set_tev_alpha, 0)
        WAIT_MS(18000)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_tev_alpha, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(credits_set_tev_alpha, 255)
        WAIT_MS(4000)
        CALL(credits_set_mode, 2)
        CALL(credits_set_tev_alpha, 0)
        WAIT_MS(4000)
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_tev_alpha, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(credits_set_tev_alpha, 255)
        WAIT_MS(18000)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_tev_alpha, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
        CALL(credits_set_tev_alpha, 0)
        WAIT_MS(4000)
        CALL(credits_set_mode, 1)
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 4000)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_tev_alpha, LW(0))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
        END_LOOP
    END_LOOP
    RETURN
EVT_END()
s32 credits_disp_start(EventEntry* event, BOOL isFirstCall) {
    dispEntry(4, 0.0f, 7, credits_draw, NULL);
    return EVT_RETURN_BLOCK;
}

s32 credits_set_mat_alpha(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)credits_work_ptr[0])->matAlpha = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 credits_set_overlay_alpha(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)credits_work_ptr[0])->overlayAlpha = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 credits_set_mode(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)credits_work_ptr[0])->mode = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

s32 credits_set_tev_alpha(EventEntry* event, BOOL isFirstCall) {
    ((CreditsWork*)credits_work_ptr[0])->tevAlpha = evtGetValue(event, event->args[0]);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_credits_sequence)
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
            CALL(credits_disp_start)
        END_INLINE_EVENT
        CALL(evt_sub_intpl_msec_init, 11, 0, 255, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_mat_alpha, LW(0))
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
                CALL(credits_set_overlay_alpha, LW(0))
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
            SPAWN_EVENT_GET_ID(PTR(evt_credits_fade), LW(10))
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
            CALL(credits_set_mode, 0)
            CALL(credits_set_mat_alpha, LW(0))
            CALL(credits_set_overlay_alpha, LW(0))
            CALL(evt_fade_entry, 9, 0, 0, 0, 0)
            CALL(evt_fade_end_wait)
            SET_VALUE(GW(10), 0)
            SPAWN_EVENT(evt_credits_end)
            RETURN
        END_IF
        CALL(evt_sub_intpl_msec_init, 11, 255, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value)
            CALL(credits_set_mat_alpha, LW(0))
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
    /* 0x00 */ s32 field_0x00;  // 0 in all 50 records and never read
    /* 0x04 */ s32 layout;      // screen placement 0-4: the text x/y in credits_record_evt, the pose path in
                                // credits_pose_evt, and GW(12)
    /* 0x08 */ s32 texId;       // GW(11), the texture index credits_draw draws
    /* 0x0C */ const char* msgA[3];
    /* 0x18 */ const char* msgB[3];
} CreditsEndingRecord;

// NONMATCHING (99.10%): register allocation only (retail colours `event` to r27, x to r29, y to r28; ours
// r29/r24/r23). The lwzu and the rematerialised record base now match; the two `#ifdef MUST_MATCH` tricks
// below break the pEntry/rec CSE that otherwise merges the two pointers.
s32 credits_record_evt(EventEntry* event, BOOL isFirstCall) {
    s32 state = event->userdata[0];
    s32 index = event->userdata[1];
    s32* pEntry;
    s32 i;
    s32 k;
    s32 n;
    s32 offset;
    s32 x;
    s32 y;
    char bufA[0x400];
    char bufB[0x400];

    EventEntry* ev = (EventEntry*)((u32)event);
    if (isFirstCall) {
        state = 0;
        index = 0;
        for (n = 0; n < 3; n++) {
            ((CreditsWork*)credits_work_ptr[0])->windowIdA[n] = -1;
        }
        for (n = 0; n < 3; n++) {
            ((CreditsWork*)credits_work_ptr[0])->windowIdB[n] = -1;
        }
        evtSetValue(ev, GW(10), 0);
        ((CreditsWork*)credits_work_ptr[0])->eventId = (s32)evtEntry(evt_credits_sequence, 0, 0);
    }
#ifdef MUST_MATCH
    // giving `offset` a fresh value before `pEntry` stops the compiler merging pEntry with rec, which is what
    // makes retail emit `lwzu` for `*++pEntry` and rematerialise the record base; the natural form is below.
    offset = index * 0x24;
    pEntry = (s32*)((u8*)credits_ending_records + offset);
#else
    pEntry = (s32*)((u8*)credits_ending_records + index * 0x24);
#endif
    ((CreditsWork*)credits_work_ptr[0])->layout = *++pEntry;
    offset = index * 0x24;
    switch (((CreditsWork*)credits_work_ptr[0])->layout) {
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
            for (i = 0; i < 3; i++) {
                const char* key = ((CreditsEndingRecord*)((u8*)credits_ending_records + offset))->msgA[i];

                if (key != NULL) {
                    const char* msg = msgSearch(key);
                    s32 ypos = y - i * 0x16;
                    s32 xpos = ypos - 4;
                    s32 xoff = x;

                    if (x == 0) {
#ifdef MUST_MATCH
                        // The original's .rodata pools 0.8 before 0.5; splitting the product into a local reproduces that order.
                        f32 fw = (f32)FontGetMessageWidth(msg) * 0.8f;
                        f32 half = 0.5f;
                        xoff = (s32)((f32)x - fw * half);
#else
                        xoff = (s32)((f32)x - 0.8f * (f32)FontGetMessageWidth(msg) * 0.5f);
#endif
                    } else if (x > 0) {
                        xoff = (s32)((f32)x - 0.8f * (f32)FontGetMessageWidth(msg));
                    }
                    sprintf(bufA, "<clear><dkey><wpos %d %d 400 200><col f0f0f0ff><speed 0><scale 0.8>%s</scale></speed></col><wait 500></dkey><once_stop>", xoff - 0x3E, xpos, msg);
                    ((CreditsWork*)credits_work_ptr[0])->windowIdA[i] = msgWindow_Entry(bufA, 0, 0);
                }
            }
            for (k = 0; k < 3; k++) {
                const char* key = ((CreditsEndingRecord*)((u8*)credits_ending_records + offset))->msgB[k];

                if (key != NULL) {
                    s32 ypos;
                    s32 xpos;
                    const char* msg;
                    s32 xoff;
                    msg = msgSearch(key);
                    ypos = y - k * 0x18;
                    xpos = ypos - 0x32;
                    xoff = x;

                    if (x == 0) {
                        f32 w = (f32)FontGetMessageWidth(msg);
                        f32 half = 0.5f;
                        xoff = (s32)((f32)x - w * half);
                    } else if (x > 0) {
                        xoff = x - FontGetMessageWidth(msg);
                    }
                    sprintf(bufB, "<clear><dkey><wpos %d %d 400 200><col f0f0f0ff><speed 0>%s</speed></col><wait 5200></dkey><once_stop>", xoff - 0x3E, xpos, msg);
                    ((CreditsWork*)credits_work_ptr[0])->windowIdB[k] = msgWindow_Entry(bufB, 0, 0);
                } else {
                    ((CreditsWork*)credits_work_ptr[0])->windowIdB[k] = -1;
                }
            }
            evtSetValue(ev, GW(10), 1);
            evtSetValue(ev, GW(11), ((CreditsEndingRecord*)credits_ending_records)[index].texId);
            evtSetValue(ev, GW(12), *pEntry);
            state++;
            break;
        case 1:
            for (n = 0; n < 3; n++) {
                if (((CreditsWork*)credits_work_ptr[0])->windowIdA[n] != -1 && windowCheckID(((CreditsWork*)credits_work_ptr[0])->windowIdA[n]) != 0) {
                    break;
                }
            }
            if (n >= 3) {
                for (n = 0; n < 3; n++) {
                    if (((CreditsWork*)credits_work_ptr[0])->windowIdB[n] != -1 && windowCheckID(((CreditsWork*)credits_work_ptr[0])->windowIdB[n]) != 0) {
                        break;
                    }
                }
                if (n >= 3) {
                    state++;
                }
            }
            break;
        case 2:
            {
                s32 next = index + 1;
                CreditsEndingRecord* nextRec;
                s32 nulls;

                if ((u32)next < 0x32) {
                    nextRec = &((CreditsEndingRecord*)credits_ending_records)[next];

                    nulls = 0;
                    for (n = 0; n < 3; n++) {
                        if (nextRec->msgA[n] != NULL) {
                            break;
                        }
                        nulls++;
                    }
                } else {
                    nulls = 0;
                }
                if (nulls < 3) {
                    for (n = 0; n < 3; n++) {
                        if (((CreditsWork*)credits_work_ptr[0])->windowIdA[n] != -1) {
                            msgWindow_Continue(((CreditsWork*)credits_work_ptr[0])->windowIdA[n]);
                        }
                    }
                }
                for (n = 0; n < 3; n++) {
                    if (((CreditsWork*)credits_work_ptr[0])->windowIdB[n] != -1) {
                        msgWindow_Continue(((CreditsWork*)credits_work_ptr[0])->windowIdB[n]);
                    }
                }
                evtSetValue(ev, GW(10), 2);
                if ((u32)next < 0x32) {
                    state++;
                } else {
                    state += 2;
                }
            }
            break;
        case 3:
            if (evtGetValue(ev, GW(10)) == 0) {
                state = 0;
                index++;
            }
            break;
        case 4:
            return EVT_RETURN_DONE;
    }
    ev->userdata[0] = state;
    ev->userdata[1] = index;
    return EVT_RETURN_BLOCK;
}


s32 credits_init(EventEntry* event, BOOL isFirstCall) {
    s32 flag = (gp->inBattle != 0);
    s32 i;

    if (isFirstCall) {
        ((CreditsWork*)credits_work_ptr[0])->image = NULL;
        ((CreditsWork*)credits_work_ptr[0])->matAlpha = 0;
        ((CreditsWork*)credits_work_ptr[0])->mode = 0;
        ((CreditsWork*)credits_work_ptr[0])->layout = 0;
        psndSetFlag(4);
        gp->language = 1;
        JUTFont_Free();
        JUTFontSetup(1);
    }
    if ((void*)fileAsyncf(FILE_TYPE_TPL, NULL, "%s/ending.tpl", getMarioStDvdRoot()) == NULL) {
        return EVT_RETURN_BLOCK;
    }
    if (((CreditsWork*)credits_work_ptr[0])->image == NULL) {
        ((CreditsWork*)credits_work_ptr[0])->image = fileAllocf(FILE_TYPE_TPL, "%s/ending.tpl", getMarioStDvdRoot());
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

// dont_inline keeps credits_free a real `bl` from _epilog as in retail.
#pragma dont_inline on
void credits_free(void) {
    CreditsWork* work = (CreditsWork*)credits_work_ptr[0];

    fileFree(work->image);
    psndClearFlag(4);
    gp->language = 0;
    JUTFont_Free();
    JUTFontSetup(0);
}

#pragma dont_inline reset
s32 credits_font_reset(EventEntry* event, BOOL isFirstCall) {
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

s32 credits_npc_setup(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr((const char*)evtGetValue(event, event->args[0]));

    mapSetTevCallback(1, credits_tev_callback);
    animPoseSetMaterialFlagOn(npc->poseId, 0x1000000);
    animPoseSetGXFunc(npc->poseId, credits_pose_gx, FALSE);
    return EVT_RETURN_DONE;
}

f32 credits_rand_range(f32 min, f32 max) {
    f32 r = (f32)rand() / 32767.0f;

    return r * (max - min) + min;
}

EVT_BEGIN(evt_credits_end)
    CALL(evt_fade_entry, 12, 2000, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1500)
    SET_VALUE(GF(10), 1)
    CALL(credits_font_reset)
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

EVT_BEGIN(evt_credits_start)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_party_cont_onoff, 0, 0)
    CALL(evt_mario_set_pos, 0, -1000, 0)
    CALL(evt_party_set_pos, 0, 0, -1000, 0)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_cam3d_evt_set, 0, 0, 545, 0, 0, 0, 0, 11)
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("pPlane1"), 1)
    CALL(evt_bg_set_color, 0, 0, 0, 255)
    SET_VALUE(GF(10), 0)
    CALL(credits_init)
    SPAWN_EVENT(evt_credits_pose_loop)
    BEGIN_INLINE_EVENT
        CALL(credits_record_evt)
    END_INLINE_EVENT
    CALL(evt_snd_bgmon, 512, STRING("BGM_STAFF_ROLL1"))
    SET_VALUE(GSW(0), 404)
    RETURN
EVT_END()

EVT_BEGIN(evt_credits_gesso)
    CALL(evt_npc_entry, STRING("gesso"), STRING("c_gesso"))
    CALL(evt_npc_set_anim, STRING("gesso"), STRING("GSO_S2_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("gesso"), 64)
    CALL(credits_npc_setup, STRING("gesso"))
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

EVT_BEGIN(evt_credits_gonbaba)
    CALL(evt_npc_entry, STRING("gonbaba"), STRING("c_gonbaba"))
    CALL(evt_npc_set_anim, STRING("gonbaba"), STRING("GNB_S_3"))
    CALL(evt_npc_flag_onoff, 1, STRING("gonbaba"), 64)
    CALL(credits_npc_setup, STRING("gonbaba"))
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

EVT_BEGIN(evt_credits_batten2)
    CALL(evt_npc_entry, STRING("batten2"), STRING("c_mb_robo_2a"))
    CALL(evt_npc_set_anim, STRING("batten2"), STRING("J_2"))
    CALL(evt_npc_flag_onoff, 1, STRING("batten2"), 64)
    CALL(credits_npc_setup, STRING("batten2"))
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

EVT_BEGIN(evt_credits_atomic)
    CALL(evt_npc_entry, STRING("atomic"), STRING("c_atmic_trs"))
    CALL(evt_npc_set_anim, STRING("atomic"), STRING("W_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("atomic"), 32832)
    CALL(credits_npc_setup, STRING("atomic"))
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

EVT_BEGIN(evt_credits_korutesu)
    CALL(evt_npc_entry, STRING("korutesu"), STRING("c_korutesu1"))
    CALL(evt_npc_set_anim, STRING("korutesu"), STRING("KRT_S_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("korutesu"), 32832)
    CALL(evt_npc_set_ry, STRING("korutesu"), 90)
    CALL(credits_npc_setup, STRING("korutesu"))
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

EVT_BEGIN(evt_credits_para)
    WAIT_MS(1000)
    CALL(evt_npc_entry, STRING("para"), STRING("c_bom_sen"))
    CALL(evt_npc_set_anim, STRING("para"), STRING("S_2"))
    CALL(evt_npc_flag_onoff, 1, STRING("para"), 32832)
    CALL(credits_npc_setup, STRING("para"))
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

EVT_BEGIN(evt_credits_batten)
    CALL(evt_npc_entry, STRING("batten"), STRING("c_mb_robo_1a"))
    CALL(evt_npc_set_anim, STRING("batten"), STRING("W_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("batten"), 64)
    CALL(credits_npc_setup, STRING("batten"))
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

EVT_BEGIN(evt_credits_queen)
    WAIT_MS(7000)
    CALL(evt_npc_entry, STRING("queen"), STRING("c_q_kage_m"))
    CALL(evt_npc_set_anim, STRING("queen"), STRING("KRT_Z_1"))
    CALL(evt_npc_flag_onoff, 1, STRING("queen"), 64)
    CALL(credits_npc_setup, STRING("queen"))
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

char* credits_ext_npc_name = "c_n_moa_a";

char* credits_ext_anim_name = "S_1";
void credits_ext_init(void) {
    ExtPose* pose = extGetPosePtr();
    CreditsPoseState* work = (CreditsPoseState*)credits_pose_states[0];
    s32 i;

    for (i = 0; i < extGetPoseNum(); i++) {
        pose->flags |= 5;
        PSMTXIdentity(*pose->matrix);
        {
            Vec pos = {0.0f, 0.0f, 0.0f};

            pos.x = credits_rand_range(-10.0f, 10.0f);
            pos.y = credits_rand_range(-10.0f, 10.0f);
            pos.z = credits_rand_range(-10.0f, 10.0f);
            work->position = pos;
        }
        work->idx = i % moamoa_chain_count;
        work->counter = ((s32*)credits_chain_counts[0])[work->idx]++;
        work++;
        pose++;
    }
}

// The declaration order of the loop induction vars (i, extPose, work) and the final
// zero-fill pointer (count before p) sets the callee-saved register assignment the
// original uses; the indices into credits_chain_groups are kept as a pointer pair.
void credits_ext_update(void) {
    NpcEntry* npc = npcNameToPtr("モアモア");
    AnimationPose* pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    Vec* positions = (Vec*)pose->groupWorldPos->address;
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
    work = (CreditsPoseState*)credits_pose_states[0];
    for (i = 0; i < extGetPoseNum(); i++) {
        s32* idxPair = (s32*)credits_chain_groups[0] + work->idx * 2;
        Vec* p0 = &positions[idxPair[0]];
        Vec* p1 = &positions[idxPair[1]];
        f32 ratio = (f32)work->counter / (f32)((s32*)credits_chain_counts[0])[work->idx];
        f32 c = 1.0f - ratio;

        if (p0->y == -1000.0f || p1->y == -1000.0f) {
            offset = (Vec){0.0f, -1000.0f, 0.0f};
            extPose->flags &= ~EXT_POSE_VISIBLE;
        } else {
            PSVECScale(p0, &offset, ratio);
            PSVECScale(p1, &tmp, c);
            PSVECAdd(&offset, &tmp, &offset);
            extPose->flags |= EXT_POSE_VISIBLE;
        }
        PSMTXTrans(mtx1, offset.x, offset.y, offset.z);
        PSMTXScale(mtx2, npc->scale.x, npc->scale.y, npc->scale.z);
        PSMTXConcat(mtx1, mtx2, mtx1);
        PSMTXTrans(mtx3, work->position.x, work->position.y, work->position.z);
        PSMTXConcat(mtx1, mtx3, *extPose->matrix);
        extPose->texId = (gp->retraceCount + i) % 40;
        work++;
        extPose++;
    }

    pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    {
        s32 count = pose->groupWorldPosCount;
        Vec* p = (Vec*)pose->groupWorldPos->address;

        for (i = 0; i < count; i++) {
            p[i] = (Vec){0.0f, -1000.0f, 0.0f};
        }
    }
}

// (bss arrays are declared uninitialized at the top of the file)
void credits_ext_disp(CameraId cameraId, void* param) {
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

void credits_ext_draw(void) {
    dispEntry(4, 0.0f, 1, credits_ext_disp, NULL);
}

s32 credits_moamoa_setup(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("モアモア");
    AnimationPose* pose;
    Vec* positions;
    void* p;
    s32 i;
    s32 count;

    p = _mapAlloc(0x1770);
    credits_pose_states[0] = (u32)p;
    memset(p, 0, 0x1770);
    animPoseWorldPositionEvalOn(npc->poseId);
    p = _mapAlloc(0x1EC);
    credits_ext_setup[0] = (u32)p;
    memset(p, 0, 0x1EC);
    for (i = 0; i < 0x28; i++) {
        ((s32*)credits_ext_setup[0])[i * 3 + 0] = (s32)credits_ext_npc_name;
        ((s32*)credits_ext_setup[0])[i * 3 + 1] = (s32)credits_ext_anim_name;
        ((f32*)credits_ext_setup[0])[i * 3 + 2] = (f32)i;
    }
    evtSetValue(event, LW(10), credits_ext_setup[0]);

    pose = (AnimationPose*)animPoseGetAnimPosePtr(npc->poseId);
    {
        smartEntry* entry = pose->groupWorldPos;

        positions = (Vec*)entry->address;
    }
    count = pose->groupWorldPosCount;
    for (i = 0; i < count; i++) {
        positions[i] = (Vec){0.0f, -1000.0f, 0.0f};
    }

    p = _mapAlloc(moamoa_chain_count * 4);
    credits_chain_counts[0] = (u32)p;
    memset(p, 0, moamoa_chain_count * 4);
    p = _mapAlloc(moamoa_chain_count * 8);
    credits_chain_groups[0] = (u32)p;
    memset(p, 0, moamoa_chain_count * 8);
    for (i = 0; i < moamoa_chain_count; i++) {
        ((s32*)credits_chain_groups[0])[i * 2 + 0] = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].name);
        ((s32*)credits_chain_groups[0])[i * 2 + 1] = animPoseGetGroupIdx(npc->poseId, g_moamoa_chain_tbl[i].anim);
    }
    return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_credits_moamoa)
    CALL(evt_npc_entry, STRING("モアモアダミー"), STRING("c_n_moa_a"))
    CALL(evt_npc_set_position, STRING("モアモアダミー"), 0, -1000, 0)
    CALL(credits_npc_setup, STRING("モアモアダミー"))
    CALL(evt_npc_entry, STRING("モアモア"), STRING("c_g_moamoa"))
    CALL(evt_npc_set_anim, STRING("モアモア"), STRING("S_3"))
    CALL(evt_npc_set_position, STRING("モアモア"), 0, -1000, 0)
    CALL(evt_npc_set_scale, STRING("モアモア"), FLOAT(0.5f), FLOAT(0.5f), FLOAT(0.5f))
    CALL(credits_moamoa_setup)
    CALL(evt_ext_entry, 300, LW(10), PTR(credits_ext_init), PTR(credits_ext_update), PTR(credits_ext_draw))
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
    CALL(credits_moamoa_free)
    CALL(evt_npc_delete, STRING("モアモアダミー"))
    CALL(evt_npc_delete, STRING("モアモア"))
    RETURN
EVT_END()
s32 credits_moamoa_free(EventEntry* event, BOOL isFirstCall) {
    NpcEntry* npc = npcNameToPtr("モアモア");

    L_animPoseWorldPositionEvalOff(npc->poseId);
    _mapFree((void*)credits_pose_states[0]);
    extReset();
    _mapFree((void*)credits_chain_groups[0]);
    _mapFree((void*)credits_chain_counts[0]);
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

// clang-format on
