#include "end_unknown.h"
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
#include "party/party_motion.h"

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
u32 unk_end_bss00000010[14];

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

// TODO: write unk_end_00001d90 (global, 0x558 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000022e8 (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_000011f0 (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_0000122c (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_00001268 (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000012a4 (global, 0x3c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000012e0 (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_000009cc (global, 0x48 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_00000a6c (global, 0x15c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00000bc8 (global, 0x628 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_00000938 (global, 0x6c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: write unk_end_000000c4 (global, 0x68 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_0000012c (global, 0x230 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_0000035c (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00000440 (global, 0x2b8 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000006f8 (global, 0x1c4 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

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

// TODO: type
u32 unk_end_bss00000000[1] = {
    0x00000000,
};

// TODO: type
u32 unk_end_bss00000004[1] = {
    0x00000000,
};

// TODO: type
u32 unk_end_bss00000008[1] = {
    0x00000000,
};

// TODO: type
u32 unk_end_bss0000000c[1] = {
    0x00000000,
};

// TODO: type
u32 unk_end_bss00000010[14] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: write unk_end_0000039c (global, 0xa4 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000008bc (global, 0x7c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000009a4 (global, 0x28 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00000a14 (global, 0x58 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00001324 (global, 0xa6c bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_000023ac (global, 0xe8 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00002494 (global, 0x74 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write unk_end_00002508 (global, 0x38 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write _unresolved (global, 0x4 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write _epilog (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write _prolog (global, 0x50 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it

// TODO: write gap_00_00002520_text (global, 0x10 bytes) here:
// its asm is in build/G8MJ01/end/asm/end_unknown.s; this is where the original has it, just before the first script that uses it
// clang-format on
