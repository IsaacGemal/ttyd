#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_item_faker_mario[];
extern s32 battle_item_hyper_kuriboo[];
extern s32 battle_item_hyper_patakuri[];
extern s32 battle_item_hyper_sinemon[];
extern s32 battle_item_hyper_togekuri[];
extern s32 battle_item_pansy[];
extern s32 battle_item_twinkling_pansy[];
extern s32 unit_faker_mario_gra[];
extern s32 unit_hyper_kuriboo[];
extern s32 unit_hyper_patakuri[];
extern s32 unit_hyper_sinemon_11_data_DA78[];
extern s32 unit_hyper_togekuri[];
extern s32 unit_pansy[];
extern s32 unit_twinkling_pansy_11_data_F890[];

// clang-format off
extend_data(40,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(41,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 gra_btlobject_data[132] = {
    (u32)"ki_1", 0x00020000, 0xC2C80000, 0x00000000,
    0xC1F00000, 0x003C0000, (u32)"isi_A", 0x00020000,
    0x43160000, 0x00000000, 0xC1F00000, 0x0A320000,
    (u32)"isi_B", 0x00020000, 0x42480000, 0x00000000,
    0xC2080000, 0x14280000, (u32)"isigaki", 0x00020001,
    0x40000000, 0x00000000, 0xC1B80000, 0x003C0000,
    (u32)"kusa_1", 0x00020001, 0xC3380000, 0x00000000,
    0x42140000, 0x0A320000, (u32)"kusa_2", 0x00020001,
    0x43340000, 0x00000000, 0x42700000, 0x14280000,
    (u32)"kusa_3", 0x00020001, 0x428C0000, 0x00000000,
    0x42820000, 0x1E1E0000, (u32)"kusa_4", 0x00020001,
    0xC2AE0000, 0x00000000, 0x42600000, 0x003C0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"kusa_1", 0x00020001, 0x43130000, 0x00000000,
    0x42980000, 0x003C0000, (u32)"kusa_2", 0x00020001,
    0x420C0000, 0x00000000, 0x42A00000, 0x0A320000,
    (u32)"kusa_3", 0x00020001, 0xC29A0000, 0x00000000,
    0x429A0000, 0x14280000, (u32)"kusa_4", 0x00020001,
    0xC31B0000, 0x00000000, 0x428E0000, 0x1E1E0000,
    (u32)"kusamura_mae", 0x00020001, 0x00000000, 0x00000000,
    0x42900000, 0x003C0000, (u32)"kusamura_usiro", 0x00020001,
    0x00000000, 0x00000000, 0xC2820000, 0x0A320000,
    (u32)"iwa", 0x00020001, 0x00000000, 0x00000000,
    0xC28E0000, 0x14280000, (u32)"ki_1", 0x00020000,
    0x42C80000, 0x00000000, 0xC24C0000, 0x003C0000,
    (u32)"ki_2", 0x00020000, 0xC2F60000, 0x00000000,
    0xC2400000, 0x0A320000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"murasaki", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ao", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
};

// TODO: type
u32 gra_btlstage_data[218] = {
    (u32)"bti_01", (u32)"stg_04_0", 0x0000000A, (u32)&gra_btlobject_data,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x001A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x001A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"bti_01", (u32)"stg_04_1", 0x0000000B,
    (u32)((u8*)&gra_btlobject_data + 0xf0), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (u32)weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x02100060, 0x20000000,
    0x001A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (u32)weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x02100060, 0x20000000,
    0x001A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000101,
};

// TODO: type
u32 btlparty_gra_gra_00_01_off_1[12] = {
    (u32)unit_faker_mario_gra, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_faker_mario,
};

// TODO: type
u32 btlgrp_gra_gra_00_01_off_1[8] = {
    0x00000001, (u32)&btlparty_gra_gra_00_01_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_01_off_1[36] = {
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
};

// TODO: type
u32 btlgrp_gra_gra_01_01_off_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_01_on_1[36] = {
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
};

// TODO: type
u32 btlgrp_gra_gra_01_01_on_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_01_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_01_on_2[36] = {
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_01_01_on_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_01_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_01_on_3[36] = {
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_01_01_on_3[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_01_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_03_off_1[36] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_03_off_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_03_on_1[36] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_03_on_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_03_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_03_on_2[48] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_03_on_2[8] = {
    0x00000004, (u32)&btlparty_gra_gra_01_03_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_03_on_3[48] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_01_03_on_3[8] = {
    0x00000004, (u32)&btlparty_gra_gra_01_03_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_04_off_1[36] = {
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_04_off_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_04_on_1[36] = {
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_04_on_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_04_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_04_on_2[48] = {
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_01_04_on_2[8] = {
    0x00000004, (u32)&btlparty_gra_gra_01_04_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_01_04_on_3[36] = {
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_kuriboo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_kuriboo,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_01_04_on_3[8] = {
    0x00000003, (u32)&btlparty_gra_gra_01_04_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_01_off_1[24] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_03_01_off_1[8] = {
    0x00000002, (u32)&btlparty_gra_gra_03_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_01_off_2[36] = {
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_03_01_off_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_03_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_04_off_1[24] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_03_04_off_1[8] = {
    0x00000002, (u32)&btlparty_gra_gra_03_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_04_off_2[36] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_twinkling_pansy_11_data_F890, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_03_04_off_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_03_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_04_off_3[36] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
};

// TODO: type
u32 btlgrp_gra_gra_03_04_off_3[8] = {
    0x00000003, (u32)&btlparty_gra_gra_03_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_05_off_1[48] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_03_05_off_1[8] = {
    0x00000004, (u32)&btlparty_gra_gra_03_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_05_off_2[36] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_twinkling_pansy_11_data_F890, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_03_05_off_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_03_05_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_03_05_off_3[48] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_hyper_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_patakuri,
    (u32)unit_hyper_togekuri, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togekuri,
};

// TODO: type
u32 btlgrp_gra_gra_03_05_off_3[8] = {
    0x00000004, (u32)&btlparty_gra_gra_03_05_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_04_02_off_1[36] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_04_02_off_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_04_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_04_02_off_2[24] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_gra_gra_04_02_off_2[8] = {
    0x00000002, (u32)&btlparty_gra_gra_04_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_04_02_off_3[36] = {
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pansy,
    (u32)unit_twinkling_pansy_11_data_F890, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_04_02_off_3[8] = {
    0x00000003, (u32)&btlparty_gra_gra_04_02_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_05_01_off_1[24] = {
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_gra_gra_05_01_off_1[8] = {
    0x00000002, (u32)&btlparty_gra_gra_05_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_05_01_off_2[36] = {
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_twinkling_pansy_11_data_F890, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_05_01_off_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_05_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_05_03_off_1[36] = {
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_gra_gra_05_03_off_1[8] = {
    0x00000003, (u32)&btlparty_gra_gra_05_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_05_03_off_2[36] = {
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_twinkling_pansy_11_data_F890, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_gra_gra_05_03_off_2[8] = {
    0x00000003, (u32)&btlparty_gra_gra_05_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_gra_gra_05_03_off_3[48] = {
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_pansy, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_hyper_sinemon_11_data_DA78, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_gra_gra_05_03_off_3[8] = {
    0x00000004, (u32)&btlparty_gra_gra_05_03_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_test1_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_01_01_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_test2_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_01_01_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_00_01_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_00_01_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_00_02_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_00_01_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_01_on[12] = {
    0x0000000A, (u32)&btlgrp_gra_gra_01_01_on_1, (u32)&gra_btlstage_data, 0x0000000A,
    (u32)&btlgrp_gra_gra_01_01_on_2, (u32)&gra_btlstage_data, 0x0000000A, (u32)&btlgrp_gra_gra_01_01_on_3,
    (u32)&gra_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_01_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_01_01_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_03_on[12] = {
    0x0000000A, (u32)&btlgrp_gra_gra_01_03_on_1, (u32)&gra_btlstage_data, 0x0000000A,
    (u32)&btlgrp_gra_gra_01_03_on_2, (u32)&gra_btlstage_data, 0x0000000A, (u32)&btlgrp_gra_gra_01_03_on_3,
    (u32)&gra_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_03_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_01_03_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_04_on[12] = {
    0x0000000A, (u32)&btlgrp_gra_gra_01_04_on_1, (u32)&gra_btlstage_data, 0x0000000A,
    (u32)&btlgrp_gra_gra_01_04_on_2, (u32)&gra_btlstage_data, 0x0000000A, (u32)&btlgrp_gra_gra_01_04_on_3,
    (u32)&gra_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_01_04_off[6] = {
    0x00000032, (u32)&btlgrp_gra_gra_01_04_off_1, (u32)&gra_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_03_01_off[9] = {
    0x0000000A, (u32)&btlgrp_gra_gra_03_01_off_1, (u32)&gra_btlstage_data, 0x0000000A,
    (u32)&btlgrp_gra_gra_03_01_off_2, (u32)&gra_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_03_04_off[12] = {
    0x00000032, (u32)&btlgrp_gra_gra_03_04_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_gra_gra_03_04_off_2, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000028, (u32)&btlgrp_gra_gra_03_04_off_3,
    (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_03_05_off[12] = {
    0x0000001E, (u32)&btlgrp_gra_gra_03_05_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_gra_gra_03_05_off_2, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000001E, (u32)&btlgrp_gra_gra_03_05_off_3,
    (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_04_02_off[12] = {
    0x0000003C, (u32)&btlgrp_gra_gra_04_02_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000001E,
    (u32)&btlgrp_gra_gra_04_02_off_2, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_gra_gra_04_02_off_3,
    (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_05_01_off[9] = {
    0x00000064, (u32)&btlgrp_gra_gra_05_01_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_gra_gra_05_01_off_2, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_gra_btlno_gra_05_03_off[12] = {
    0x00000032, (u32)&btlgrp_gra_gra_05_03_off_1, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_gra_gra_05_03_off_2, (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000028, (u32)&btlgrp_gra_gra_05_03_off_3,
    (u32)((u8*)&gra_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_gra_tbl[238] = {
    (u32)"マップstg_04_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_gra_btlno_gra_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_04_1", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_gra_btlno_gra_test2_off, 0x00000000,
    0x00000000, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA00ランペルその１", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_gra_btlno_gra_00_01_off,
    0x20000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_BOSS_STG4_RUNPELL1", (u32)"GRA00ランペルその２",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_gra_btlno_gra_00_02_off, 0x20000000, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_BOSS_STG4_RUNPELL1",
    (u32)"GRA01ハイパークリボー", 0x00000000, 0x00000000, 0xF84063A1,
    (u32)&btlgrpsel_gra_btlno_gra_01_01_on, (u32)&btlgrpsel_gra_btlno_gra_01_01_off, 0x00000000, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x0A0A0404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA01ハイパーパタクリ", 0x00000000, 0x00000000,
    0xF84063A1, (u32)&btlgrpsel_gra_btlno_gra_01_03_on, (u32)&btlgrpsel_gra_btlno_gra_01_03_off, 0x00000000,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x0A0A0404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA01ハイパートゲクリ", 0x00000000,
    0x00000000, 0xF84063A1, (u32)&btlgrpsel_gra_btlno_gra_01_04_on, (u32)&btlgrpsel_gra_btlno_gra_01_04_off,
    0x00000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x0A0A0404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA03ハイパークリボー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_gra_btlno_gra_03_01_off, 0x00000000, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x0A0A0404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"GRA03パンジーさん", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_gra_btlno_gra_03_04_off, 0x00000000, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x0A0A0404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA03パンジーさん", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_gra_btlno_gra_03_05_off, 0x00000000,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x0A0A0404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA04パンジーさん", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_gra_btlno_gra_04_02_off,
    0x00000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x0A0A0404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"GRA05ハイパーシンエモン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_gra_btlno_gra_05_01_off, 0x00000000, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x04040404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"GRA05ハイパーシンエモン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_gra_btlno_gra_05_03_off, 0x00000000, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btl_gra_setup_no_tbl[28] = {
    (u32)"BTLNO_GRA_TEST1", 0x00000000, (u32)"BTLNO_GRA_TEST2", 0x00000001,
    (u32)"BTLNO_GRA_00_01", 0x00000002, (u32)"BTLNO_GRA_00_02", 0x00000003,
    (u32)"BTLNO_GRA_01_01", 0x00000004, (u32)"BTLNO_GRA_01_03", 0x00000005,
    (u32)"BTLNO_GRA_01_04", 0x00000006, (u32)"BTLNO_GRA_03_01", 0x00000007,
    (u32)"BTLNO_GRA_03_04", 0x00000008, (u32)"BTLNO_GRA_03_05", 0x00000009,
    (u32)"BTLNO_GRA_04_02", 0x0000000A, (u32)"BTLNO_GRA_05_01", 0x0000000B,
    (u32)"BTLNO_GRA_05_03", 0x0000000C, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
