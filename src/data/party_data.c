#include <dolphin/types.h>

extern u8 bomhei_bye[];
extern u8 bomhei_exit[];
extern u8 bomhei_init_800BFBB0[];
extern u8 bomhei_use[];
extern u8 bomhei_use_post[];
extern u8 chuchu_exit[];
extern u8 chuchu_init[];
extern u8 chuchu_move[];
extern u8 chuchu_use[];
extern u8 cloud_exit[];
extern u8 cloud_init[];
extern u8 cloud_move[];
extern u8 cloud_use[];
extern u8 kuribo_bye[];
extern u8 kuribo_init_800C23B4[];
extern u8 kuribo_use[];
extern u8 kuribo_use_post[];
extern u8 nokonoko_bye[];
extern u8 nokonoko_init[];
extern u8 nokonoko_move[];
extern u8 nokonoko_use[];
extern u8 nokonoko_use_post[];
extern u8 partyMoveWalk[];
extern u8 vivian_init_801809C0[];
extern u8 vivian_move[];
extern u8 vivian_use[];
extern u8 yoshi_bye[];
extern u8 yoshi_exit[];
extern u8 yoshi_init[];
extern u8 yoshi_use[];

const char str_802CD020[12] = "c_bomt_n\000\000\000";

const char str_802CD02C[12] = "c_babyyoshi";

const char str_802CD038[8] = "c_windy";

const char str_802CD040[12] = "c_vivian\000\000\000";

const char str_802CD04C[12] = "c_tyutyu\000\000\000";

const char str_802CD058[8] = "c_marco";

const char str_802CD060[12] = "c_pguide\000\000\000";

const char str_802CD06C[12] = "c_kuri_h\000\000\000";

const char str_802CD078[12] = "c_monban\000\000\000";

const char str_802CD084[8] = "PKR_Z_1";

const char str_802CD08C[8] = "PKR_S_1";

const char str_802CD094[8] = "PKR_W_1";

const char str_802CD09C[8] = "PKR_R_1";

const char str_802CD0A4[12] = "PKR_J_1A\000\000\000";

const char str_802CD0B0[12] = "PKR_J_1B\000\000\000";

const char str_802CD0BC[8] = "PKR_N_1";

const char str_802CD0C4[8] = "PKR_T_1";

const char str_802CD0CC[8] = "PKR_D_1";

const char str_802CD0D4[8] = "PKR_D_4";

const char str_802CD0DC[8] = "PKR_D_2";

const char str_802CD0E4[8] = "PKR_W_2";

const char str_802CD0EC[8] = "PKR_R_2";

const char str_802CD0F4[8] = "PNK_Z_1";

const char str_802CD0FC[8] = "PNK_S_1";

const char str_802CD104[8] = "PNK_W_1";

const char str_802CD10C[8] = "PNK_R_1";

const char str_802CD114[12] = "PNK_J_1A\000\000\000";

const char str_802CD120[12] = "PNK_J_1B\000\000\000";

const char str_802CD12C[8] = "PNK_N_1";

const char str_802CD134[8] = "PNK_T_1";

const char str_802CD13C[8] = "PNK_D_1";

const char str_802CD144[8] = "PNK_D_4";

const char str_802CD14C[8] = "PNK_D_2";

const char str_802CD154[8] = "PNK_W_2";

const char str_802CD15C[8] = "PNK_R_2";

const char str_802CD164[8] = "PYS_Z_1";

const char str_802CD16C[8] = "PYS_S_1";

const char str_802CD174[8] = "PYS_W_1";

const char str_802CD17C[8] = "PYS_R_1";

const char str_802CD184[12] = "PYS_J_1A\000\000\000";

const char str_802CD190[12] = "PYS_J_1B\000\000\000";

const char str_802CD19C[8] = "PYS_N_1";

const char str_802CD1A4[8] = "PYS_T_1";

const char str_802CD1AC[8] = "PYS_D_1";

const char str_802CD1B4[8] = "PYS_D_4";

const char str_802CD1BC[8] = "PYS_D_2";

const char str_802CD1C4[8] = "PYS_W_3";

const char str_802CD1CC[8] = "PYS_R_3";

const char str_802CD1D4[8] = "PWD_Z_1";

const char str_802CD1DC[8] = "PWD_S_1";

const char str_802CD1E4[8] = "PWD_W_1";

const char str_802CD1EC[8] = "PWD_R_1";

const char str_802CD1F4[8] = "PWD_N_1";

const char str_802CD1FC[8] = "PWD_T_1";

const char str_802CD204[8] = "PWD_D_1";

const char str_802CD20C[8] = "PWD_F_1";

const char str_802CD214[8] = "PWD_D_2";

const char str_802CD21C[8] = "PWD_W_2";

const char str_802CD224[8] = "PWD_R_2";

const char str_802CD22C[8] = "PTR_Z_1";

const char str_802CD234[8] = "PTR_S_1";

const char str_802CD23C[8] = "PTR_W_1";

const char str_802CD244[8] = "PTR_R_1";

const char str_802CD24C[8] = "PTR_N_1";

const char str_802CD254[8] = "PTR_T_1";

const char str_802CD25C[8] = "PTR_D_1";

const char str_802CD264[8] = "PTR_F_1";

const char str_802CD26C[8] = "PTR_D_2";

const char str_802CD274[8] = "PTR_W_2";

const char str_802CD27C[8] = "PTR_R_2";

const char str_802CD284[8] = "PCH_Z_1";

const char str_802CD28C[8] = "PCH_S_1";

const char str_802CD294[8] = "PCH_W_1";

const char str_802CD29C[8] = "PCH_R_1";

const char str_802CD2A4[12] = "PCH_J_1A\000\000\000";

const char str_802CD2B0[12] = "PCH_J_1B\000\000\000";

const char str_802CD2BC[8] = "PCH_N_1";

const char str_802CD2C4[8] = "PCH_T_1";

const char str_802CD2CC[8] = "PCH_D_1";

const char str_802CD2D4[8] = "PCH_F_1";

const char str_802CD2DC[8] = "PCH_D_2";

const char str_802CD2E4[8] = "PCH_R_2";

const char str_802CD2EC[8] = "PCH_W_2";

const char str_802CD2F4[8] = "PYS_E_1";

const char str_802CD2FC[8] = "PYS_E_2";

const char str_802CD304[8] = "PYS_E_3";

const char str_802CD30C[8] = "PYS_W_2";

const char str_802CD314[8] = "PYS_R_2";

const __declspec(section ".sdata2") char str_8041B2C8[8] = "c_pkr\000\000";

const __declspec(section ".sdata2") char str_8041B2D0[8] = "c_pnk\000\000";

const char str_8041B2D8[4] = "Z_1";

const char str_8041B2DC[4] = "S_1";

const char str_8041B2E0[4] = "W_1";

const char str_8041B2E4[4] = "R_1";

const __declspec(section ".sdata2") char str_8041B2E8[8] = "J_1A\000\000\000";

const __declspec(section ".sdata2") char str_8041B2F0[8] = "J_1B\000\000\000";

const char str_8041B2F8[4] = "N_1";

const char str_8041B2FC[4] = "T_1";

const char str_8041B300[4] = "D_1";

const char str_8041B304[4] = "F_1";

const char str_8041B308[4] = "D_2";

const char str_8041B30C[4] = "W_3";

const char str_8041B310[4] = "R_3";

const char str_8041B314[4] = "Z_2";

const char str_8041B318[4] = "W_2";

const char str_8041B31C[4] = "R_2";

const char str_8041B320[4] = "Z_3";

const __declspec(section ".sdata2") char str_8041B324[8] = "Z_13\000\000\000";

const __declspec(section ".sdata2") char str_8041B32C[8] = "W_13\000\000\000";

const __declspec(section ".sdata2") char str_8041B334[8] = "R_13\000\000\000";

const char str_8041B33C[4] = "Z_5";

const char str_8041B340[4] = "W_5";

const char str_8041B344[4] = "R_5";

const char str_8041B348[4] = "Z_4";

const char str_8041B34C[4] = "W_4";

const char str_8041B350[4] = "R_4";

const __declspec(section ".sdata2") char str_8041B354[8] = "Z_12\000\000\000";

const __declspec(section ".sdata2") char str_8041B35C[8] = "W_12\000\000\000";

const __declspec(section ".sdata2") char str_8041B364[8] = "R_12\000\000\000";

const __declspec(section ".sdata2") char str_8041B36C[8] = "Z_14\000\000\000";

const __declspec(section ".sdata2") char str_8041B374[8] = "W_14\000\000\000";

const __declspec(section ".sdata2") char str_8041B37C[8] = "R_14\000\000\000";

const char str_8041B384[4] = "Z_6";

const char str_8041B388[4] = "W_6";

const char str_8041B38C[4] = "R_6";

const char str_8041B390[4] = "Z_7";

const char str_8041B394[4] = "W_7";

const char str_8041B398[4] = "R_7";

const char str_8041B39C[4] = "Z_8";

const char str_8041B3A0[4] = "W_8";

const char str_8041B3A4[4] = "R_8";

const char str_8041B3A8[4] = "Z_9";

const char str_8041B3AC[4] = "W_9";

const char str_8041B3B0[4] = "R_9";

const __declspec(section ".sdata2") char str_8041B3B4[8] = "Z_10\000\000\000";

const __declspec(section ".sdata2") char str_8041B3BC[8] = "W_10\000\000\000";

const __declspec(section ".sdata2") char str_8041B3C4[8] = "R_10\000\000\000";

const __declspec(section ".sdata2") char str_8041B3CC[8] = "Z_11\000\000\000";

const __declspec(section ".sdata2") char str_8041B3D4[8] = "W_11\000\000\000";

const __declspec(section ".sdata2") char str_8041B3DC[8] = "R_11\000\000\000";

u32 partyDataTbl[200] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)(void*)str_8041B2C8, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, (u32)(void*)kuribo_use_post, (u32)(void*)kuribo_bye,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_8041B2D0, (u32)(void*)nokonoko_init, (u32)(void*)nokonoko_move, (u32)(void*)nokonoko_use,
    (u32)(void*)nokonoko_use_post, (u32)(void*)nokonoko_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD020, (u32)(void*)bomhei_init_800BFBB0,
    (u32)(void*)partyMoveWalk, (u32)(void*)bomhei_use, (u32)(void*)bomhei_use_post, (u32)(void*)bomhei_bye, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)bomhei_exit,
    (u32)(void*)str_802CD02C, (u32)(void*)yoshi_init, (u32)(void*)partyMoveWalk, (u32)(void*)yoshi_use, 0x00000000, (u32)(void*)yoshi_bye, 0x00000000, 0x00000000,
    0x00000000, (u32)(void*)yoshi_exit, (u32)(void*)str_802CD038, (u32)(void*)cloud_init, (u32)(void*)cloud_move, (u32)(void*)cloud_use, 0x00000000, (u32)(void*)kuribo_bye,
    0x00000000, 0x00000000, 0x00000000, (u32)(void*)cloud_exit, (u32)(void*)str_802CD040, (u32)(void*)vivian_init_801809C0, (u32)(void*)vivian_move, (u32)(void*)vivian_use,
    0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD04C, (u32)(void*)chuchu_init,
    (u32)(void*)chuchu_move, (u32)(void*)chuchu_use, 0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)chuchu_exit,
    (u32)(void*)str_802CD02C, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, 0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)(void*)str_802CD058, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, 0x00000000, (u32)(void*)kuribo_bye,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD060, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use,
    0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD06C, (u32)(void*)kuribo_init_800C23B4,
    (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, 0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)(void*)str_802CD078, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, 0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)(void*)str_8041B2C8, (u32)(void*)kuribo_init_800C23B4, (u32)(void*)partyMoveWalk, (u32)(void*)kuribo_use, 0x00000000, (u32)(void*)kuribo_bye,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_8041B2D0, (u32)(void*)nokonoko_init, (u32)(void*)nokonoko_move, (u32)(void*)nokonoko_use,
    0x00000000, (u32)(void*)nokonoko_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD020, (u32)(void*)bomhei_init_800BFBB0,
    (u32)(void*)partyMoveWalk, (u32)(void*)bomhei_use, (u32)(void*)bomhei_use_post, (u32)(void*)bomhei_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)(void*)str_802CD02C, (u32)(void*)yoshi_init, (u32)(void*)partyMoveWalk, (u32)(void*)yoshi_use, 0x00000000, (u32)(void*)yoshi_bye, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, (u32)(void*)str_802CD038, (u32)(void*)cloud_init, (u32)(void*)cloud_move, (u32)(void*)cloud_use, 0x00000000, (u32)(void*)kuribo_bye,
    0x00000000, 0x00000000, 0x00000000, (u32)(void*)cloud_exit, (u32)(void*)str_802CD040, (u32)(void*)vivian_init_801809C0, (u32)(void*)vivian_move, (u32)(void*)vivian_use,
    0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD04C, (u32)(void*)chuchu_init,
    (u32)(void*)partyMoveWalk, (u32)(void*)chuchu_use, 0x00000000, (u32)(void*)kuribo_bye, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

u32 partyPoseTbl[300] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802CD084,
    (u32)(void*)str_802CD08C, (u32)(void*)str_802CD094, (u32)(void*)str_802CD09C, (u32)(void*)str_802CD0A4, (u32)(void*)str_802CD0B0, (u32)(void*)str_802CD0BC, (u32)(void*)str_802CD0C4, (u32)(void*)str_802CD0CC,
    (u32)(void*)str_802CD0D4, (u32)(void*)str_802CD08C, (u32)(void*)str_802CD08C, (u32)(void*)str_802CD0DC, (u32)(void*)str_802CD0E4, (u32)(void*)str_802CD0EC, (u32)(void*)str_802CD0F4, (u32)(void*)str_802CD0FC,
    (u32)(void*)str_802CD104, (u32)(void*)str_802CD10C, (u32)(void*)str_802CD114, (u32)(void*)str_802CD120, (u32)(void*)str_802CD12C, (u32)(void*)str_802CD134, (u32)(void*)str_802CD13C, (u32)(void*)str_802CD144,
    (u32)(void*)str_802CD0FC, (u32)(void*)str_802CD0FC, (u32)(void*)str_802CD14C, (u32)(void*)str_802CD154, (u32)(void*)str_802CD15C, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0,
    (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2E8, (u32)(void*)str_8041B2F0, (u32)(void*)str_8041B2F8, (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B300, (u32)(void*)str_8041B304, (u32)(void*)str_8041B2DC,
    (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B308, (u32)(void*)str_8041B30C, (u32)(void*)str_8041B310, (u32)(void*)str_802CD164, (u32)(void*)str_802CD16C, (u32)(void*)str_802CD174, (u32)(void*)str_802CD17C,
    (u32)(void*)str_802CD184, (u32)(void*)str_802CD190, (u32)(void*)str_802CD19C, (u32)(void*)str_802CD1A4, (u32)(void*)str_802CD1AC, (u32)(void*)str_802CD1B4, (u32)(void*)str_802CD16C, (u32)(void*)str_802CD16C,
    (u32)(void*)str_802CD1BC, (u32)(void*)str_802CD1C4, (u32)(void*)str_802CD1CC, (u32)(void*)str_802CD1D4, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1E4, (u32)(void*)str_802CD1EC, (u32)(void*)str_802CD1DC,
    (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1F4, (u32)(void*)str_802CD1FC, (u32)(void*)str_802CD204, (u32)(void*)str_802CD20C, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD214,
    (u32)(void*)str_802CD21C, (u32)(void*)str_802CD224, (u32)(void*)str_802CD22C, (u32)(void*)str_802CD234, (u32)(void*)str_802CD23C, (u32)(void*)str_802CD244, (u32)(void*)str_802CD234, (u32)(void*)str_802CD234,
    (u32)(void*)str_802CD24C, (u32)(void*)str_802CD254, (u32)(void*)str_802CD25C, (u32)(void*)str_802CD264, (u32)(void*)str_802CD22C, (u32)(void*)str_802CD22C, (u32)(void*)str_802CD26C, (u32)(void*)str_802CD274,
    (u32)(void*)str_802CD27C, (u32)(void*)str_802CD284, (u32)(void*)str_802CD28C, (u32)(void*)str_802CD294, (u32)(void*)str_802CD29C, (u32)(void*)str_802CD2A4, (u32)(void*)str_802CD2B0, (u32)(void*)str_802CD2BC,
    (u32)(void*)str_802CD2C4, (u32)(void*)str_802CD2CC, (u32)(void*)str_802CD2D4, (u32)(void*)str_802CD28C, (u32)(void*)str_802CD28C, (u32)(void*)str_802CD2DC, (u32)(void*)str_802CD2E4, (u32)(void*)str_802CD2EC,
    (u32)(void*)str_802CD2F4, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD304, (u32)(void*)str_802CD304, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC,
    (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD2FC, (u32)(void*)str_802CD304, (u32)(void*)str_802CD304, (u32)(void*)str_8041B2D8,
    (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2E8, (u32)(void*)str_8041B2F0, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B300,
    (u32)(void*)str_8041B300, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC,
    (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2E8, (u32)(void*)str_8041B2F0, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2D8,
    (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0,
    (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC,
    (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4,
    (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC,
    (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_802CD084, (u32)(void*)str_802CD08C, (u32)(void*)str_802CD094, (u32)(void*)str_802CD09C, (u32)(void*)str_802CD0A4,
    (u32)(void*)str_802CD0B0, (u32)(void*)str_802CD0BC, (u32)(void*)str_802CD0C4, (u32)(void*)str_802CD0CC, (u32)(void*)str_802CD0D4, (u32)(void*)str_802CD08C, (u32)(void*)str_802CD08C, (u32)(void*)str_802CD0DC,
    (u32)(void*)str_802CD0E4, (u32)(void*)str_802CD0EC, (u32)(void*)str_802CD0F4, (u32)(void*)str_802CD0FC, (u32)(void*)str_802CD104, (u32)(void*)str_802CD10C, (u32)(void*)str_802CD114, (u32)(void*)str_802CD120,
    (u32)(void*)str_802CD12C, (u32)(void*)str_802CD134, (u32)(void*)str_802CD13C, (u32)(void*)str_802CD144, (u32)(void*)str_802CD0FC, (u32)(void*)str_802CD0FC, (u32)(void*)str_802CD14C, (u32)(void*)str_802CD154,
    (u32)(void*)str_802CD15C, (u32)(void*)str_8041B2D8, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2E0, (u32)(void*)str_8041B2E4, (u32)(void*)str_8041B2E8, (u32)(void*)str_8041B2F0, (u32)(void*)str_8041B2F8,
    (u32)(void*)str_8041B2FC, (u32)(void*)str_8041B300, (u32)(void*)str_8041B304, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B2DC, (u32)(void*)str_8041B308, (u32)(void*)str_8041B30C, (u32)(void*)str_8041B310,
    (u32)(void*)str_802CD164, (u32)(void*)str_802CD16C, (u32)(void*)str_802CD174, (u32)(void*)str_802CD17C, (u32)(void*)str_802CD184, (u32)(void*)str_802CD190, (u32)(void*)str_802CD19C, (u32)(void*)str_802CD1A4,
    (u32)(void*)str_802CD1AC, (u32)(void*)str_802CD1B4, (u32)(void*)str_802CD16C, (u32)(void*)str_802CD16C, (u32)(void*)str_802CD1BC, (u32)(void*)str_802CD30C, (u32)(void*)str_802CD314, (u32)(void*)str_802CD1D4,
    (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1E4, (u32)(void*)str_802CD1EC, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1F4, (u32)(void*)str_802CD1FC, (u32)(void*)str_802CD204,
    (u32)(void*)str_802CD20C, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD1DC, (u32)(void*)str_802CD214, (u32)(void*)str_802CD21C, (u32)(void*)str_802CD224, (u32)(void*)str_802CD22C, (u32)(void*)str_802CD234,
    (u32)(void*)str_802CD23C, (u32)(void*)str_802CD244, (u32)(void*)str_802CD234, (u32)(void*)str_802CD234, (u32)(void*)str_802CD24C, (u32)(void*)str_802CD254, (u32)(void*)str_802CD25C, (u32)(void*)str_802CD264,
    (u32)(void*)str_802CD22C, (u32)(void*)str_802CD22C, (u32)(void*)str_802CD26C, (u32)(void*)str_802CD274, (u32)(void*)str_802CD27C, (u32)(void*)str_802CD284, (u32)(void*)str_802CD28C, (u32)(void*)str_802CD294,
    (u32)(void*)str_802CD29C, (u32)(void*)str_802CD2A4, (u32)(void*)str_802CD2B0, (u32)(void*)str_802CD2BC, (u32)(void*)str_802CD2C4, (u32)(void*)str_802CD2CC, (u32)(void*)str_802CD2D4, (u32)(void*)str_802CD28C,
    (u32)(void*)str_802CD28C, (u32)(void*)str_802CD2DC, (u32)(void*)str_802CD2E4, (u32)(void*)str_802CD2EC
};

u32 partyDotPoseTbl[120] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_8041B314,
    (u32)(void*)str_8041B314, (u32)(void*)str_8041B318, (u32)(void*)str_8041B31C, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314,
    (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B314, (u32)(void*)str_8041B318, (u32)(void*)str_8041B31C, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320,
    (u32)(void*)str_8041B30C, (u32)(void*)str_8041B310, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320,
    (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B320, (u32)(void*)str_8041B30C, (u32)(void*)str_8041B310, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B32C,
    (u32)(void*)str_8041B334, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B324,
    (u32)(void*)str_8041B324, (u32)(void*)str_8041B324, (u32)(void*)str_8041B32C, (u32)(void*)str_8041B334, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B340, (u32)(void*)str_8041B344,
    (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C,
    (u32)(void*)str_8041B33C, (u32)(void*)str_8041B340, (u32)(void*)str_8041B344, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B34C, (u32)(void*)str_8041B350, (u32)(void*)str_8041B348,
    (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348, (u32)(void*)str_8041B348,
    (u32)(void*)str_8041B34C, (u32)(void*)str_8041B350, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B35C, (u32)(void*)str_8041B364, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354,
    (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B354, (u32)(void*)str_8041B35C,
    (u32)(void*)str_8041B364, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B374, (u32)(void*)str_8041B37C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C,
    (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B36C, (u32)(void*)str_8041B374, (u32)(void*)str_8041B37C
};

u32 yoshiDotPoseTbl[105] = {
    (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B340, (u32)(void*)str_8041B344, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C,
    (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B33C, (u32)(void*)str_8041B340, (u32)(void*)str_8041B344, (u32)(void*)str_8041B384,
    (u32)(void*)str_8041B384, (u32)(void*)str_8041B388, (u32)(void*)str_8041B38C, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384,
    (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B384, (u32)(void*)str_8041B388, (u32)(void*)str_8041B38C, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390,
    (u32)(void*)str_8041B394, (u32)(void*)str_8041B398, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390,
    (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B390, (u32)(void*)str_8041B394, (u32)(void*)str_8041B398, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B3A0,
    (u32)(void*)str_8041B3A4, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C,
    (u32)(void*)str_8041B39C, (u32)(void*)str_8041B39C, (u32)(void*)str_8041B3A0, (u32)(void*)str_8041B3A4, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3AC, (u32)(void*)str_8041B3B0,
    (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3A8,
    (u32)(void*)str_8041B3A8, (u32)(void*)str_8041B3AC, (u32)(void*)str_8041B3B0, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3BC, (u32)(void*)str_8041B3C4, (u32)(void*)str_8041B3B4,
    (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4, (u32)(void*)str_8041B3B4,
    (u32)(void*)str_8041B3BC, (u32)(void*)str_8041B3C4, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3D4, (u32)(void*)str_8041B3DC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC,
    (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3CC, (u32)(void*)str_8041B3D4,
    (u32)(void*)str_8041B3DC
};

