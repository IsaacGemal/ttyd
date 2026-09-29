#include "battle_database_tik.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off
// the original REL was compiled with fp contraction enabled (fmadds in _wave_offset)
#pragma fp_contract on

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_flower_drop_param_default4[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default3[];
extern s32 battle_heart_drop_param_default4[];
extern s32 battle_item_gesso[];
extern s32 battle_item_hammer_bros[];
extern s32 battle_item_hannya[];
extern s32 battle_item_hennya[];
extern s32 battle_item_hinnya[];
extern s32 battle_item_kamec[];
extern s32 battle_item_kuriboo[];
extern s32 battle_item_kurikuri[];
extern s32 battle_item_nokonoko[];
extern s32 battle_item_patakuri[];
extern s32 battle_item_patapata[];
extern s32 battle_item_togekuri[];
extern s32 battle_item_togenoko[];
extern s32 unit_boss_gesso[];
extern s32 unit_hammer_bros_26_data_240F0[];
extern s32 unit_hannya_26_data_20220[];
extern s32 unit_hennya_26_data_30370[];
extern s32 unit_hinnya_26_data_20DE0[];
extern s32 unit_kamec[];
extern s32 unit_kuriboo_26_data_1CBF8[];
extern s32 unit_lecture_frankli_kuriboo[];
extern s32 unit_nokonoko_26_data_2DCC8[];
extern s32 unit_patakuri_26_data_21998[];
extern s32 unit_patapata_26_data_29498[];
extern s32 unit_togekuri_26_data_23050[];
extern s32 unit_togenoko[];
static USER_FUNC(_wave_offset);

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 tik_btlobject_data[120] = {
    (u32)"isi_2", 0x00020000, 0xC3160000, 0x00000000,
    0xC1B80000, 0x0A320000, (u32)"isi_3", 0x00020000,
    0xC2700000, 0x00000000, 0xC2580000, 0x14280000,
    (u32)"isi_4", 0x00020000, 0x43300000, 0x00000000,
    0xC2780000, 0x1E1E0000, (u32)"nami_0", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"nami_1", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"nami_2", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"nami_3", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"nami_0", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"nami_1", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"nami_2", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"nami_3", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"B_0", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"jon_0", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"isi_1", 0x00020001,
    0xC34A0000, 0x00000000, 0xC1880000, 0x003C0000,
    (u32)"isi_2", 0x00020001, 0x43480000, 0x00000000,
    0xC1F00000, 0x0A320000, (u32)"A1", 0x00020000,
    0x00000000, 0x00000000, 0xC2480000, 0x003C0000,
    (u32)"B_mati", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
};

// TODO: type
u32 tik_btlstage_data[436] = {
    (u32)"bti_01", (u32)"stg_00_2", 0x00000008, (u32)&tik_btlobject_data,
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
    0x00010101, (u32)"bti_01", (u32)"stg_00_2", 0x00000005,
    (u32)((u8*)&tik_btlobject_data + 0xc0), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_00_3",
    0x00000002, (u32)((u8*)&tik_btlobject_data + 0x138), 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x02100060,
    0x20000000, 0x001A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x02100060,
    0x20000000, 0x001A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)"bti_01",
    (u32)"stg_00_1", 0x00000005, (u32)((u8*)&tik_btlobject_data + 0x168), 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (u32)weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100060, 0x20000000, 0x001A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (u32)weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100060, 0x20000000, 0x001A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlparty_tik_tik_gesso[12] = {
    (u32)unit_boss_gesso, 0x01000000, 0x04000004, 0x42F40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_gesso,
};

// TODO: type
u32 btlgrp_tik_tik_gesso[8] = {
    0x00000001, (u32)&btlparty_tik_tik_gesso, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_01_01_off_1[48] = {
    (u32)unit_kuriboo_26_data_1CBF8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
    (u32)unit_togekuri_26_data_23050, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togekuri,
    (u32)unit_patakuri_26_data_21998, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patakuri,
    (u32)unit_lecture_frankli_kuriboo, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tik_tik_01_01_off_1[8] = {
    0x00000004, (u32)&btlparty_tik_tik_01_01_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_01_02_off_1[24] = {
    (u32)unit_togekuri_26_data_23050, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togekuri,
    (u32)unit_togekuri_26_data_23050, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togekuri,
};

// TODO: type
u32 btlgrp_tik_tik_01_02_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_01_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_01_off_1[12] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_01_off_1[8] = {
    0x00000001, (u32)&btlparty_tik_tik_04_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_01_on_1[12] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_01_on_1[8] = {
    0x00000001, (u32)&btlparty_tik_tik_04_01_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_01_on_2[24] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_01_on_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_01_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_02_off_1[24] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_02_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_02_on_1[24] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_02_on_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_02_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_02_on_2[36] = {
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_04_02_on_2[8] = {
    0x00000003, (u32)&btlparty_tik_tik_04_02_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_03_off_1[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_patapata_26_data_29498, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patapata,
};

// TODO: type
u32 btlgrp_tik_tik_04_03_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_03_off_2[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_04_03_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_03_off_3[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_04_03_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_03_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_04_off_1[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_togekuri_26_data_23050, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togekuri,
};

// TODO: type
u32 btlgrp_tik_tik_04_04_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_04_off_2[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_04_04_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_04_04_off_3[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_04_04_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_04_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_01_off_1[36] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_07_01_off_1[8] = {
    0x00000003, (u32)&btlparty_tik_tik_07_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_01_off_2[24] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_07_01_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_02_off_1[24] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_07_02_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_02_off_2[36] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_07_02_off_2[8] = {
    0x00000003, (u32)&btlparty_tik_tik_07_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_03_off_1[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_nokonoko_26_data_2DCC8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_nokonoko,
};

// TODO: type
u32 btlgrp_tik_tik_07_03_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_03_off_2[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_07_03_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_03_off_3[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
};

// TODO: type
u32 btlgrp_tik_tik_07_03_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_03_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_04_off_1[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_07_04_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_04_off_2[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
};

// TODO: type
u32 btlgrp_tik_tik_07_04_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_07_04_off_3[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_07_04_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_07_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_01_off_1[12] = {
    (u32)unit_kuriboo_26_data_1CBF8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kuriboo,
};

// TODO: type
u32 btlgrp_tik_tik_15_01_off_1[8] = {
    0x00000001, (u32)&btlparty_tik_tik_15_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_02_off_1[12] = {
    (u32)unit_patakuri_26_data_21998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patakuri,
};

// TODO: type
u32 btlgrp_tik_tik_15_02_off_1[8] = {
    0x00000001, (u32)&btlparty_tik_tik_15_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_03_off_1[12] = {
    (u32)unit_togekuri_26_data_23050, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togekuri,
};

// TODO: type
u32 btlgrp_tik_tik_15_03_off_1[8] = {
    0x00000001, (u32)&btlparty_tik_tik_15_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_04_off_1[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_kuriboo_26_data_1CBF8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kuriboo,
};

// TODO: type
u32 btlgrp_tik_tik_15_04_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_04_off_2[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
};

// TODO: type
u32 btlgrp_tik_tik_15_04_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_04_off_3[24] = {
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_15_04_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_05_off_1[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_patakuri_26_data_21998, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patakuri,
};

// TODO: type
u32 btlgrp_tik_tik_15_05_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_05_off_2[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_15_05_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_05_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_15_05_off_3[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_hammer_bros_26_data_240F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
};

// TODO: type
u32 btlgrp_tik_tik_15_05_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_15_05_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_01_off_1[24] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_18_01_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_01_off_2[36] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_18_01_off_2[8] = {
    0x00000003, (u32)&btlparty_tik_tik_18_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_02_off_1[48] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_18_02_off_1[8] = {
    0x00000004, (u32)&btlparty_tik_tik_18_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_02_off_2[24] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_26_data_20DE0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_tik_tik_18_02_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_03_off_1[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_patakuri_26_data_21998, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patakuri,
};

// TODO: type
u32 btlgrp_tik_tik_18_03_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_03_off_2[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_18_03_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_03_off_3[24] = {
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_18_03_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_03_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_04_off_1[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_18_04_off_1[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_04_off_2[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
};

// TODO: type
u32 btlgrp_tik_tik_18_04_off_2[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_18_04_off_3[24] = {
    (u32)unit_togenoko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko,
    (u32)unit_kamec, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tik_tik_18_04_off_3[8] = {
    0x00000002, (u32)&btlparty_tik_tik_18_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_01_off_1[60] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_01_off_1[8] = {
    0x00000005, (u32)&btlparty_tik_tik_20_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_02_off_1[48] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_02_off_1[8] = {
    0x00000004, (u32)&btlparty_tik_tik_20_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_03_off_1[36] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_03_off_1[8] = {
    0x00000003, (u32)&btlparty_tik_tik_20_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_04_off_1[48] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_04_off_1[8] = {
    0x00000004, (u32)&btlparty_tik_tik_20_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_05_off_1[60] = {
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_05_off_1[8] = {
    0x00000005, (u32)&btlparty_tik_tik_20_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tik_tik_20_06_off_1[60] = {
    (u32)unit_hennya_26_data_30370, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_26_data_20220, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_tik_tik_20_06_off_1[8] = {
    0x00000005, (u32)&btlparty_tik_tik_20_06_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_test1_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_01_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_test2_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_01_01_off_1, (u32)&tik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_test3_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_01_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x368), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_gesso_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_gesso, (u32)&tik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_01_01_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_01_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_01_02_off[6] = {
    0x00000032, (u32)&btlgrp_tik_tik_01_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_01_on[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_01_on_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_04_01_on_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_01_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_02_on[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_02_on_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_04_02_on_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_02_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_03_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_03_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_04_03_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_04_03_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_04_04_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_04_04_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_04_04_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_04_04_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_07_01_off[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_07_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_07_01_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_07_02_off[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_07_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_07_02_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_07_03_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_07_03_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_07_03_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_07_03_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_07_04_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_07_04_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_07_04_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_07_04_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_15_01_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_15_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_15_02_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_15_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_15_03_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_15_03_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_15_04_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_15_04_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_15_04_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_15_04_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_15_05_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_15_05_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_15_05_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_15_05_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_18_01_off[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_18_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_18_01_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_18_02_off[9] = {
    0x00000014, (u32)&btlgrp_tik_tik_18_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_18_02_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_18_03_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_18_03_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_18_03_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_18_03_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_18_04_off[12] = {
    0x00000014, (u32)&btlgrp_tik_tik_18_04_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014,
    (u32)&btlgrp_tik_tik_18_04_off_2, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000014, (u32)&btlgrp_tik_tik_18_04_off_3,
    (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_01_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_01_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_02_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_02_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_03_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_03_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_04_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_04_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_05_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_05_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tik_btlno_tik_20_06_off[6] = {
    0x00000014, (u32)&btlgrp_tik_tik_20_06_off_1, (u32)((u8*)&tik_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_tik_tbl[510] = {
    (u32)"マップstg_00_1", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_00_2", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_test2_off, 0x00000000,
    0x00000000, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_00_3", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_test3_off,
    0x00000000, 0x00000000, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik12ゲッソー",
    (u32)"tik_02", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_gesso_off, 0x20000010, 0x00000000, 0x05050000,
    0x00000000, 0x00000505, 0x00000000, 0x00000000,
    0x05050000, 0x01010101, 0x01010101, (u32)"BGM_CHUBOSS_BATTLE1",
    (u32)"tik01クリボー３人衆", (u32)"tik_01", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_01_01_off, 0x20000010, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x05050000, 0x00000101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik01トゲクリボー", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_01_02_off, 0x00000000,
    0x00000000, 0x05050000, 0x00000000, 0x00000505,
    0x00000000, 0x00000000, 0x0A0A0000, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik04ヒンニャー1", 0x00000000,
    0x00000000, 0xF840605E, (u32)&btlgrpsel_tik_btlno_tik_04_01_on, (u32)&btlgrpsel_tik_btlno_tik_04_01_off,
    0x00000000, 0x00000000, 0x05050000, 0x00000000,
    0x00000505, 0x00000000, 0x00000000, 0x05050000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik04ヒンニャー2",
    0x00000000, 0x00000000, 0xF840605E, (u32)&btlgrpsel_tik_btlno_tik_04_02_on,
    (u32)&btlgrpsel_tik_btlno_tik_04_02_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x00000505, 0x00000000, 0x00000000,
    0x05050000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik04カメック", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_04_03_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik04トゲノコ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_04_04_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik07ハンニャー1", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_07_01_off,
    0x00000000, 0x00000000, 0x05050000, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050505,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik07ハンニャー2",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_07_02_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050505, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik07トゲノコ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_07_03_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik07ハンマーブロス", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_07_04_off, 0x00000000,
    0x00000000, 0x03030303, 0x0303030A, 0x0A030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik15クリボー", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_15_01_off,
    0x00000000, 0x00000000, 0x05050000, 0x00000000,
    0x00000505, 0x00000000, 0x00000000, 0x0A0A0000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik15パタクリボー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_15_02_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x00000505, 0x00000000, 0x00000000,
    0x0A0A0000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik15トゲクリボー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_15_03_off, 0x00000000, 0x00000000,
    0x05050000, 0x00000000, 0x00000505, 0x00000000,
    0x00000000, 0x0A0A0000, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik15ハンマーブロス", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_15_04_off, 0x00000000,
    0x00000000, 0x03030303, 0x0303030A, 0x0A030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik15カメック", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_15_05_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik18ハンニャー1",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_18_01_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050505, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik18ハンニャー2", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_18_02_off, 0x00000000, 0x00000000,
    0x05050000, 0x00000000, 0x05050505, 0x00000000,
    0x05050000, 0x05050505, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik18カメック", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_18_03_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik18トゲノコ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_18_04_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik20ハンニャー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_20_01_off, 0x00000000, 0x00000000, 0x02020202,
    0x02020202, 0x02020202, 0x02020202, 0x02020202,
    0x02020202, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik20ハンニャー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_20_02_off, 0x00000000, 0x00000000,
    0x02020202, 0x02020202, 0x02020202, 0x02020202,
    0x02020202, 0x02020202, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"tik20ハンニャー", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_20_03_off, 0x00000000,
    0x00000000, 0x02020202, 0x02020202, 0x02020202,
    0x02020202, 0x02020202, 0x02020202, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik20ハンニャー", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tik_btlno_tik_20_04_off,
    0x00000000, 0x00000000, 0x02020202, 0x02020202,
    0x02020202, 0x02020202, 0x02020202, 0x02020202,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"tik20ハンニャー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tik_btlno_tik_20_05_off, 0x00000000, 0x00000000, 0x02020202,
    0x02020202, 0x02020202, 0x02020202, 0x02020202,
    0x02020202, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"tik20ヘンニャー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tik_btlno_tik_20_06_off, 0x00000000, 0x00000000,
    0x02020202, 0x02020202, 0x02020202, 0x02020202,
    0x02020202, 0x02020202, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btl_tik_setup_no_tbl[60] = {
    (u32)"BTLNO_TIK_TEST1", 0x00000000, (u32)"BTLNO_TIK_TEST2", 0x00000001,
    (u32)"BTLNO_TIK_TEST3", 0x00000002, (u32)"BTLNO_TIK_GESSO", 0x00000003,
    (u32)"BTLNO_TIK_01_01", 0x00000004, (u32)"BTLNO_TIK_01_02", 0x00000005,
    (u32)"BTLNO_TIK_04_01", 0x00000006, (u32)"BTLNO_TIK_04_02", 0x00000007,
    (u32)"BTLNO_TIK_04_03", 0x00000008, (u32)"BTLNO_TIK_04_04", 0x00000009,
    (u32)"BTLNO_TIK_07_01", 0x0000000A, (u32)"BTLNO_TIK_07_02", 0x0000000B,
    (u32)"BTLNO_TIK_07_03", 0x0000000C, (u32)"BTLNO_TIK_07_04", 0x0000000D,
    (u32)"BTLNO_TIK_15_01", 0x0000000E, (u32)"BTLNO_TIK_15_02", 0x0000000F,
    (u32)"BTLNO_TIK_15_03", 0x00000010, (u32)"BTLNO_TIK_15_04", 0x00000011,
    (u32)"BTLNO_TIK_15_05", 0x00000012, (u32)"BTLNO_TIK_18_01", 0x00000013,
    (u32)"BTLNO_TIK_18_02", 0x00000014, (u32)"BTLNO_TIK_18_03", 0x00000015,
    (u32)"BTLNO_TIK_18_04", 0x00000016, (u32)"BTLNO_TIK_20_01", 0x00000017,
    (u32)"BTLNO_TIK_20_02", 0x00000018, (u32)"BTLNO_TIK_20_03", 0x00000019,
    (u32)"BTLNO_TIK_20_04", 0x0000001A, (u32)"BTLNO_TIK_20_05", 0x0000001B,
    (u32)"BTLNO_TIK_20_06", 0x0000001C, 0x00000000, 0x00000000,
};

static USER_FUNC(_wave_offset) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 a1 = args[1];
    s32 v1 = evtGetValue(event, a1);
    s32 v2 = evtGetValue(event, args[2]);
    s32 v3 = evtGetValue(event, args[3]);
    s32 a4 = args[4];
    s32 v4 = evtGetValue(event, a4);
    s32 v5 = evtGetValue(event, args[5]);
    s32 v6 = evtGetValue(event, args[6]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)name);
    s32 next1;
    s32 next4;

    obj->field_0x10.x = v3 * sinfd((f32)(v1 * 360 / v2));
    obj->field_0x10.y = v6 * sinfd((f32)(v4 * 360 / v5 + 270)) + v6;

    next1 = v1 + 1;
    if (next1 >= v2) {
        next1 = 0;
    }
    evtSetValue(event, a1, next1);

    next4 = v4 + 1;
    if (next4 >= v5) {
        next4 = 0;
    }
    evtSetValue(event, a4, next4);

    return EVT_RETURN_DONE;
}

EVT_BEGIN(bgset_tik_wave_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 0)
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    SET_VALUE(LW(7), 0)
    LABEL(0)
    CALL(_wave_offset, STRING("nami_1"), LW(2), 600, 20, LW(3), 250, 6)
    CALL(_wave_offset, STRING("nami_2"), LW(4), 1100, 40, LW(5), 360, 7)
    CALL(_wave_offset, STRING("nami_3"), LW(6), 1250, 50, LW(7), 450, 8)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
