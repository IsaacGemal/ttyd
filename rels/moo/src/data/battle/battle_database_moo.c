#include "battle_database_moo.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_item_barriern_z[];
extern s32 battle_item_default_param_list[];
extern s32 battle_item_sinemon[];
extern s32 unit_barriern_z_18_data_72F0[];
extern s32 unit_kuriboo_18_data_4608[];
extern s32 unit_sinemon_18_data_5D28[];
static USER_FUNC(_space_rotate);
s32 bgset_moo_space_event[35];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData moo_btlobject_data[5] = {
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"space", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_01", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_02", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
    {"star_03", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 0x3C, {0, 0}},
};

// TODO: type
u32 moo_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_07_4", 0x00000005, (u32)&moo_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, (u32)&bgset_moo_space_event,
    0x00000000,
};

// TODO: type
u32 btlparty_moo_moo_00_01_off_1[36] = {
    (u32)unit_kuriboo_18_data_4608, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_default_param_list,
    (u32)unit_kuriboo_18_data_4608, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_kuriboo_18_data_4608, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_moo_moo_00_01_off_1[8] = {
    0x00000003, (u32)&btlparty_moo_moo_00_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_01_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_01_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_01_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_02_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_02_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_01_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_02_off_2[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_02_off_2[8] = {
    0x00000003, (u32)&btlparty_moo_moo_01_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_03_off_1[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_03_off_1[8] = {
    0x00000003, (u32)&btlparty_moo_moo_01_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_04_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_04_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_01_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_04_off_2[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_04_off_2[8] = {
    0x00000003, (u32)&btlparty_moo_moo_01_04_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_04_off_3[48] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_04_off_3[8] = {
    0x00000004, (u32)&btlparty_moo_moo_01_04_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_01_05_off_1[48] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_01_05_off_1[8] = {
    0x00000004, (u32)&btlparty_moo_moo_01_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_02_01_off_1[24] = {
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
};

// TODO: type
u32 btlgrp_moo_moo_02_01_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_02_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_02_02_off_1[24] = {
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_02_02_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_02_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_02_03_off_1[24] = {
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
};

// TODO: type
u32 btlgrp_moo_moo_02_03_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_02_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_02_04_off_1[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_02_04_off_1[8] = {
    0x00000003, (u32)&btlparty_moo_moo_02_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_02_05_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_barriern_z_18_data_72F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_barriern_z,
};

// TODO: type
u32 btlgrp_moo_moo_02_05_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_02_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_01_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_01_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_05_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_01_off_2[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_01_off_2[8] = {
    0x00000003, (u32)&btlparty_moo_moo_05_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_01_off_3[48] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_01_off_3[8] = {
    0x00000004, (u32)&btlparty_moo_moo_05_01_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_02_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_02_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_05_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_03_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_03_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_05_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_03_off_2[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_03_off_2[8] = {
    0x00000003, (u32)&btlparty_moo_moo_05_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_04_off_1[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_04_off_1[8] = {
    0x00000003, (u32)&btlparty_moo_moo_05_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_05_off_1[24] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_05_off_1[8] = {
    0x00000002, (u32)&btlparty_moo_moo_05_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_05_off_2[36] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_05_off_2[8] = {
    0x00000003, (u32)&btlparty_moo_moo_05_05_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_moo_moo_05_05_off_3[48] = {
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_18_data_5D28, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_moo_moo_05_05_off_3[8] = {
    0x00000004, (u32)&btlparty_moo_moo_05_05_off_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_test1_off[6] = {
    0x00000032, (u32)&btlgrp_moo_moo_00_01_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_01_01_off[6] = {
    0x0000000A, (u32)&btlgrp_moo_moo_01_01_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_01_02_off[9] = {
    0x0000000A, (u32)&btlgrp_moo_moo_01_02_off_1, (u32)&moo_btlstage_data, 0x0000000A,
    (u32)&btlgrp_moo_moo_01_02_off_2, (u32)&moo_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_01_03_off[6] = {
    0x0000000A, (u32)&btlgrp_moo_moo_01_03_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_01_04_off[12] = {
    0x0000000A, (u32)&btlgrp_moo_moo_01_04_off_1, (u32)&moo_btlstage_data, 0x0000000A,
    (u32)&btlgrp_moo_moo_01_04_off_2, (u32)&moo_btlstage_data, 0x00000005, (u32)&btlgrp_moo_moo_01_04_off_3,
    (u32)&moo_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_01_05_off[6] = {
    0x00000005, (u32)&btlgrp_moo_moo_01_05_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_02_01_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_02_01_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_02_02_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_02_02_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_02_03_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_02_03_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_02_04_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_02_04_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_02_05_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_02_05_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_05_01_off[12] = {
    0x0000000A, (u32)&btlgrp_moo_moo_05_01_off_1, (u32)&moo_btlstage_data, 0x0000000A,
    (u32)&btlgrp_moo_moo_05_01_off_2, (u32)&moo_btlstage_data, 0x00000005, (u32)&btlgrp_moo_moo_05_01_off_3,
    (u32)&moo_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_05_02_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_05_02_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_05_03_off[9] = {
    0x0000000A, (u32)&btlgrp_moo_moo_05_03_off_1, (u32)&moo_btlstage_data, 0x0000000A,
    (u32)&btlgrp_moo_moo_05_03_off_2, (u32)&moo_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_05_04_off[6] = {
    0x00000014, (u32)&btlgrp_moo_moo_05_04_off_1, (u32)&moo_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_moo_btlno_moo_05_05_off[12] = {
    0x0000000A, (u32)&btlgrp_moo_moo_05_05_off_1, (u32)&moo_btlstage_data, 0x0000000A,
    (u32)&btlgrp_moo_moo_05_05_off_2, (u32)&moo_btlstage_data, 0x00000005, (u32)&btlgrp_moo_moo_05_05_off_3,
    (u32)&moo_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_moo_tbl[289] = {
    (u32)"マップstg_07_4", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_moo_btlno_moo_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"moo01シンエモン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_01_01_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo01シンエモン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_01_02_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo01シンエモン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_moo_btlno_moo_01_03_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"moo01シンエモン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_moo_btlno_moo_01_04_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"moo01シンエモン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_01_05_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo02バリアーンZ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_02_01_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo02バリアーンZ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_moo_btlno_moo_02_02_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"moo02バリアーンZ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_moo_btlno_moo_02_03_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"moo02シンエモン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_02_04_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo02シンエモン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_02_05_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo05シンエモン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_moo_btlno_moo_05_01_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"moo05シンエモン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_moo_btlno_moo_05_02_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"moo05シンエモン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_05_03_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo05シンエモン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_moo_btlno_moo_05_04_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"moo05シンエモン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_moo_btlno_moo_05_05_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btl_moo_setup_no_tbl[34] = {
    (u32)"BTLNO_MOO_TEST1", 0x00000000, (u32)"BTLNO_MOO_01_01", 0x00000001,
    (u32)"BTLNO_MOO_01_02", 0x00000002, (u32)"BTLNO_MOO_01_03", 0x00000003,
    (u32)"BTLNO_MOO_01_04", 0x00000004, (u32)"BTLNO_MOO_01_05", 0x00000005,
    (u32)"BTLNO_MOO_02_01", 0x00000006, (u32)"BTLNO_MOO_02_02", 0x00000007,
    (u32)"BTLNO_MOO_02_03", 0x00000008, (u32)"BTLNO_MOO_02_04", 0x00000009,
    (u32)"BTLNO_MOO_02_05", 0x0000000A, (u32)"BTLNO_MOO_05_01", 0x0000000B,
    (u32)"BTLNO_MOO_05_02", 0x0000000C, (u32)"BTLNO_MOO_05_03", 0x0000000D,
    (u32)"BTLNO_MOO_05_04", 0x0000000E, (u32)"BTLNO_MOO_05_05", 0x0000000F,
    0x00000000, 0x00000000,
};

USER_FUNC(_space_rotate) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 angle = evtGetValue(event, index);
    s32 total = evtGetValue(event, args[2]);
    BattleStageObject* object = BattleSearchObjectPtr((const char*)name);

    object->mRotation.z = 360.0f * (f32)angle / (f32)total;
    angle++;
    if (angle >= total) {
        angle = 0;
    }
    evtSetValue(event, index, angle);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(bgset_moo_space_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    SET_VALUE(LW(3), 0)
    LABEL(0)
    CALL(_space_rotate, STRING("star_01"), LW(1), 7500)
    CALL(_space_rotate, STRING("star_02"), LW(2), 6750)
    CALL(_space_rotate, STRING("star_03"), LW(3), 6000)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
