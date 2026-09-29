#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_item_atmic_teresa[];
extern s32 battle_item_basabasa[];
extern s32 battle_item_faker_mario[];
extern s32 battle_item_met[];
extern s32 battle_item_rampell[];
extern s32 battle_item_teresa[];
extern s32 battle_item_togemet[];
extern s32 unit_atmic_teresa[];
extern s32 unit_basabasa_14_data_1DE40[];
extern s32 unit_boss_rampell[];
extern s32 unit_faker_mario[];
extern s32 unit_met[];
extern s32 unit_teresa_14_data_1EF28[];
extern s32 unit_togemet[];

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 jin_btlobject_data[108] = {
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"if_wire", 0x00050006, 0x00000000, 0x43020000,
    0x00000000, 0x00000000, (u32)"if_body", 0x00050006,
    0x00000000, 0x43020000, 0x00000000, 0x00000000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"ranpu", 0x00020000, 0xC2AA0000, 0x00000000,
    0xC2100000, 0x003C0000, (u32)"tukue", 0x00020000,
    0xC1D80000, 0x00000000, 0xC2380000, 0x14280000,
    (u32)"reizouko", 0x00020000, 0xC2F80000, 0x00000000,
    0xC2700000, 0x14280000, (u32)"furo", 0x00020000,
    0x42B80000, 0x00000000, 0xC22C0000, 0x1E1E0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x005A0000,
};

// TODO: type
u32 jin_btlstage_data[654] = {
    (u32)"bti_01", (u32)"stg_04_3", 0x00000002, (u32)&jin_btlobject_data,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, (u32)"bti_01", (u32)"stg_04_3", 0x00000004,
    (u32)((u8*)&jin_btlobject_data + 0x30), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_04_2",
    0x00000002, (u32)((u8*)&jin_btlobject_data + 0x90), 0x00000000, 0x00000000,
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
    0x00000000, (u32)weaponGetPowerDefault, 0x00000001, 0x00000000,
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
    (u32)"stg_04_4", 0x00000006, (u32)((u8*)&jin_btlobject_data + 0xc0), 0x00000000,
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
    (u32)"bti_01", (u32)"stg_04_5", 0x00000002, (u32)((u8*)&jin_btlobject_data + 0x150),
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, (u32)"bti_01", (u32)"stg_04_6", 0x00000002,
    (u32)((u8*)&jin_btlobject_data + 0x180), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlparty_jin_jin_04_ramper[12] = {
    (u32)unit_boss_rampell, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_rampell,
};

// TODO: type
u32 btlgrp_jin_jin_04_ramper[8] = {
    0x00000001, (u32)&btlparty_jin_jin_04_ramper, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_faker_mario[12] = {
    (u32)unit_faker_mario, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_faker_mario,
};

// TODO: type
u32 btlgrp_jin_jin_01_faker_mario[8] = {
    0x00000001, (u32)&btlparty_jin_jin_01_faker_mario, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_00_atmic_teresa[12] = {
    (u32)unit_atmic_teresa, 0x01000000, 0x04000004, 0x428C0000,
    0x41A00000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_atmic_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_00_atmic_teresa[8] = {
    0x00000001, (u32)&btlparty_jin_jin_00_atmic_teresa, 0x00000000, 0x0000000A,
    0x00000000, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_00_01_off_1[12] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_00_01_off_1[8] = {
    0x00000001, (u32)&btlparty_jin_jin_00_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_00_01_off_2[24] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_00_01_off_2[8] = {
    0x00000002, (u32)&btlparty_jin_jin_00_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_00_01_off_3[36] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_00_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jin_jin_00_01_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_00_01_off_4[48] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_00_01_off_4[8] = {
    0x00000004, (u32)&btlparty_jin_jin_00_01_off_4, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_01_off_1[24] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_01_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_01_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_01_off_2[36] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42480000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42B40000,
    0x43020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x43020000,
    0x43020000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_01_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jin_jin_01_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_01_off_3[36] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42480000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x43020000,
    0x43020000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_01_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jin_jin_01_01_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_03_off_1[24] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_01_03_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_01_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_03_off_2[48] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x43160000,
    0x43020000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_01_03_off_2[8] = {
    0x00000004, (u32)&btlparty_jin_jin_01_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_01_03_off_3[24] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_01_03_off_3[8] = {
    0x00000002, (u32)&btlparty_jin_jin_01_03_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_03_01_off_1[24] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_03_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_03_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_03_02_off_1[24] = {
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_14_data_1DE40, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_jin_jin_03_02_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_03_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_07_01_off_1[24] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_07_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_07_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_07_01_off_2[36] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_07_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jin_jin_07_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_07_02_off_1[24] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_07_02_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_07_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_07_02_off_2[36] = {
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_14_data_1EF28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jin_jin_07_02_off_2[8] = {
    0x00000003, (u32)&btlparty_jin_jin_07_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_09_01_off_1[12] = {
    (u32)unit_met, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
};

// TODO: type
u32 btlgrp_jin_jin_09_01_off_1[8] = {
    0x00000001, (u32)&btlparty_jin_jin_09_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_01_off_1[24] = {
    (u32)unit_met, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_met, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
};

// TODO: type
u32 btlgrp_jin_jin_10_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_10_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_02_off_1[12] = {
    (u32)unit_togemet, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togemet,
};

// TODO: type
u32 btlgrp_jin_jin_10_02_off_1[8] = {
    0x00000001, (u32)&btlparty_jin_jin_10_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_03_off_1[12] = {
    (u32)unit_met, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
};

// TODO: type
u32 btlgrp_jin_jin_10_03_off_1[8] = {
    0x00000001, (u32)&btlparty_jin_jin_10_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_04_off_1[48] = {
    (u32)unit_met, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_met, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_met, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_met, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
};

// TODO: type
u32 btlgrp_jin_jin_10_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jin_jin_10_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_05_off_1[24] = {
    (u32)unit_togemet, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togemet,
    (u32)unit_togemet, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togemet,
};

// TODO: type
u32 btlgrp_jin_jin_10_05_off_1[8] = {
    0x00000002, (u32)&btlparty_jin_jin_10_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jin_jin_10_06_off_1[36] = {
    (u32)unit_met, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_met, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_met,
    (u32)unit_togemet, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togemet,
};

// TODO: type
u32 btlgrp_jin_jin_10_06_off_1[8] = {
    0x00000003, (u32)&btlparty_jin_jin_10_06_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_test1_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_test2_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)&jin_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_test3_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_test4_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x6d0), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_test5_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_01_01_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_04_ramper, (u32)((u8*)&jin_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_01_02_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_01_faker_mario, (u32)((u8*)&jin_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_01_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_00_atmic_teresa, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_02_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_03_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_04_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_05_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_06_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_00_09_off[15] = {
    0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x368), 0x00000014,
    (u32)&btlgrp_jin_jin_00_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jin_jin_00_01_off_4, (u32)((u8*)&jin_btlstage_data + 0x368),
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_01_03_off[12] = {
    0x00000014, (u32)&btlgrp_jin_jin_01_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jin_jin_01_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jin_jin_01_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_01_05_off[12] = {
    0x0000000A, (u32)&btlgrp_jin_jin_01_03_off_1, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jin_jin_01_03_off_2, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000014, (u32)&btlgrp_jin_jin_01_03_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_02_01_off[12] = {
    0x00000032, (u32)&btlgrp_jin_jin_01_03_off_1, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jin_jin_01_03_off_2, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000014, (u32)&btlgrp_jin_jin_01_03_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_02_03_off[12] = {
    0x00000032, (u32)&btlgrp_jin_jin_01_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jin_jin_01_01_off_2, (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jin_jin_01_01_off_3,
    (u32)((u8*)&jin_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_09_01_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_09_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_01_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_01_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_02_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_02_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_03_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_03_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_04_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_04_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_05_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_05_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jin_btlno_jin_10_06_off[6] = {
    0x00000032, (u32)&btlgrp_jin_jin_10_06_off_1, (u32)((u8*)&jin_btlstage_data + 0x884), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_jin_tbl[442] = {
    (u32)"マップstg_04_2", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_04_3", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_test2_off, 0x00000000,
    0x00000000, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_04_4", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_test3_off,
    0x00000000, 0x00000000, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_04_5",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_test4_off, 0x00000000, 0x00000000, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"マップstg_04_6", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_test5_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_04ランペルその１", (u32)"jin_04", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_01_01_off, 0x20000010,
    0x00000000, 0x03030303, 0x03030000, 0x03030303,
    0x03030303, 0x03030000, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_BOSS_STG4_RUNPELL1", (u32)"JIN_04ランペルその２", (u32)"jin_04",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_01_02_off,
    0x20000010, 0x00000006, 0x03030303, 0x03030000,
    0x03030303, 0x03030303, 0x03030000, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_BOSS_STG4_FAKEMARIO1", (u32)"JIN_00アトミックテレサ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_00_01_off, 0x20000010, 0x00000000, 0x00000000,
    0x0A0A0000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01010101, 0x01010101, (u32)"BGM_CHUBOSS_BATTLE1",
    (u32)"JIN_00テレサ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_00_02_off, 0x00000000, 0x00000000,
    0x04040404, 0x0A0A0000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_00テレサ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_00_03_off, 0x00000000,
    0x00000000, 0x04040404, 0x0A0A0000, 0x04040404,
    0x04040404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_00テレサ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_00_04_off,
    0x00000000, 0x00000000, 0x04040404, 0x0A0A0000,
    0x04040404, 0x04040404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_00テレサ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_00_05_off, 0x00000000, 0x00000000, 0x04040404,
    0x0A0A0000, 0x04040404, 0x04040404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"JIN_00テレサ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_00_06_off, 0x00000000, 0x00000000,
    0x04040404, 0x0A0A0000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_00テレサ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_00_09_off, 0x00000000,
    0x00000000, 0x04040404, 0x0A0A0000, 0x04040404,
    0x04040404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_01バサバサ", (u32)"jin_04",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_01_03_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030000,
    0x03030303, 0x03030303, 0x03030000, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_01バサバサ",
    (u32)"jin_04", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_01_05_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030000, 0x03030303, 0x03030303, 0x03030000,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"JIN_02バサバサ", (u32)"jin_04", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_02_01_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030000, 0x03030303, 0x03030303,
    0x03030000, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_02バサバサ", (u32)"jin_04", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_02_03_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030000, 0x03030303,
    0x03030303, 0x03030000, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_09メット", (u32)"jin_04",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_09_01_off,
    0x00000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x04040404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_10メット",
    (u32)"jin_04", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_10_01_off, 0x00000000, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x04040404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"JIN_10トゲメット", (u32)"jin_04", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_10_02_off, 0x00000000, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_10メット", (u32)"jin_04", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_10_03_off, 0x00000000,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x04040404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_10メット", (u32)"jin_04",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jin_btlno_jin_10_04_off,
    0x00000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x04040404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"JIN_10トゲメット",
    (u32)"jin_04", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jin_btlno_jin_10_05_off, 0x00000000, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x04040404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"JIN_10メット", (u32)"jin_04", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jin_btlno_jin_10_06_off, 0x00000000, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btl_jin_setup_no_tbl[52] = {
    (u32)"BTLNO_JIN_TEST1", 0x00000000, (u32)"BTLNO_JIN_TEST2", 0x00000001,
    (u32)"BTLNO_JIN_TEST3", 0x00000002, (u32)"BTLNO_JIN_TEST4", 0x00000003,
    (u32)"BTLNO_JIN_TEST5", 0x00000004, (u32)"BTLNO_JIN_01_01", 0x00000005,
    (u32)"BTLNO_JIN_01_02", 0x00000006, (u32)"BTLNO_JIN_00_01", 0x00000007,
    (u32)"BTLNO_JIN_00_02", 0x00000008, (u32)"BTLNO_JIN_00_03", 0x00000009,
    (u32)"BTLNO_JIN_00_04", 0x0000000A, (u32)"BTLNO_JIN_00_05", 0x0000000B,
    (u32)"BTLNO_JIN_00_06", 0x0000000C, (u32)"BTLNO_JIN_00_09", 0x0000000D,
    (u32)"BTLNO_JIN_01_03", 0x0000000E, (u32)"BTLNO_JIN_01_05", 0x0000000F,
    (u32)"BTLNO_JIN_02_01", 0x00000010, (u32)"BTLNO_JIN_02_03", 0x00000011,
    (u32)"BTLNO_JIN_09_01", 0x00000012, (u32)"BTLNO_JIN_10_01", 0x00000013,
    (u32)"BTLNO_JIN_10_02", 0x00000014, (u32)"BTLNO_JIN_10_03", 0x00000015,
    (u32)"BTLNO_JIN_10_04", 0x00000016, (u32)"BTLNO_JIN_10_05", 0x00000017,
    (u32)"BTLNO_JIN_10_06", 0x00000018, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
