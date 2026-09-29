#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_item_monochrome__pakkun[];
extern s32 battle_item_monochrome_kurokumorn[];
extern s32 battle_item_monochrome_sinemon[];
extern s32 battle_item_vivian1[];
extern s32 unit_boss_vivian[];
extern s32 unit_monochrome_kurokumorn_30_data_17868[];
extern s32 unit_monochrome_pakkun_30_data_166A8[];
extern s32 unit_monochrome_sinemon_30_data_15620[];

// clang-format off
extend_data(28,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(29,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData win_btlobject_data[5] = {
    { "A1",     0x0002, 0x0000, { 92.0f, 0.0f, -46.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "S_hana", 0x0002, 0x0001, {  2.0f, 0.0f, -46.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "S_kusa", 0x0002, 0x0001, {  2.0f, 0.0f,  64.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "B",      0x0002, 0x0002, {  0.0f, 0.0f,   0.0f }, 0, 0x3C, { 0x00, 0x00 } },
    { "C",      0x0002, 0x0003, {  0.0f, 0.0f,   0.0f }, 0, 0x3C, { 0x00, 0x00 } },
};

// TODO: type
u32 win_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_02_0", 0x00000005, (u32)&win_btlobject_data,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0210006F, 0x20000000, 0x001A0302,
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
    0x00000000,
};

// TODO: type
u32 btlparty_win_win_00_01_off_1[24] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_01_off_1[8] = {
    0x00000002, (u32)&btlparty_win_win_00_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_01_on_1[24] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_01_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_00_01_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_01_on_2[36] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_01_on_2[8] = {
    0x00000003, (u32)&btlparty_win_win_00_01_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_01_on_3[48] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_01_on_3[8] = {
    0x00000004, (u32)&btlparty_win_win_00_01_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_02_off_1[24] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_02_off_1[8] = {
    0x00000002, (u32)&btlparty_win_win_00_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_02_on_1[24] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_02_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_00_02_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_02_on_2[36] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_02_on_2[8] = {
    0x00000003, (u32)&btlparty_win_win_00_02_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_03_off_1[36] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_03_off_1[8] = {
    0x00000003, (u32)&btlparty_win_win_00_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_03_on_1[24] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_win_win_00_03_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_00_03_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_03_on_2[24] = {
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_00_03_on_2[8] = {
    0x00000002, (u32)&btlparty_win_win_00_03_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_00_04_off_1[12] = {
    (u32)unit_boss_vivian, 0x01000000, 0x04000004, 0x425C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_vivian1,
};

// TODO: type
u32 btlgrp_win_win_00_04_off_1[8] = {
    0x00000001, (u32)&btlparty_win_win_00_04_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_01_off_1[24] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_02_01_off_1[8] = {
    0x00000002, (u32)&btlparty_win_win_02_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_01_on_1[24] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_02_01_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_02_01_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_01_on_2[36] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_02_01_on_2[8] = {
    0x00000003, (u32)&btlparty_win_win_02_01_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_01_on_3[36] = {
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_02_01_on_3[8] = {
    0x00000003, (u32)&btlparty_win_win_02_01_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_02_off_1[24] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_02_02_off_1[8] = {
    0x00000002, (u32)&btlparty_win_win_02_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_02_on_1[24] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_02_02_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_02_02_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_02_on_2[36] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_02_02_on_2[8] = {
    0x00000003, (u32)&btlparty_win_win_02_02_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_02_02_on_3[36] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_02_02_on_3[8] = {
    0x00000003, (u32)&btlparty_win_win_02_02_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_03_01_off_1[48] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_03_01_off_1[8] = {
    0x00000004, (u32)&btlparty_win_win_03_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_03_01_on_1[24] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_03_01_on_1[8] = {
    0x00000002, (u32)&btlparty_win_win_03_01_on_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_03_01_on_2[36] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_03_01_on_2[8] = {
    0x00000003, (u32)&btlparty_win_win_03_01_on_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_03_01_on_3[36] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_kurokumorn_30_data_17868, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_30_data_15620, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_sinemon,
};

// TODO: type
u32 btlgrp_win_win_03_01_on_3[8] = {
    0x00000003, (u32)&btlparty_win_win_03_01_on_3, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_win_win_03_01_on_4[48] = {
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
    (u32)unit_monochrome_pakkun_30_data_166A8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome__pakkun,
};

// TODO: type
u32 btlgrp_win_win_03_01_on_4[8] = {
    0x00000004, (u32)&btlparty_win_win_03_01_on_4, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_test1_off[2] = {
    { 0x32, (BattleGroupSetup*)&btlgrp_win_win_00_01_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_01_on[4] = {
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_01_on_1, (s32)&win_btlstage_data },
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_01_on_2, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_00_01_on_3, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_01_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_00_01_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_02_on[3] = {
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_02_on_1, (s32)&win_btlstage_data },
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_02_on_2, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_02_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_00_02_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_03_on[3] = {
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_03_on_1, (s32)&win_btlstage_data },
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_00_03_on_2, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_03_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_00_03_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_00_04_off[2] = {
    { 0x32, (BattleGroupSetup*)&btlgrp_win_win_00_04_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_01_on[4] = {
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_02_01_on_1, (s32)&win_btlstage_data },
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_02_01_on_2, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_02_01_on_3, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_01_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_02_01_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_02_on[4] = {
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_02_02_on_1, (s32)&win_btlstage_data },
    { 0x14, (BattleGroupSetup*)&btlgrp_win_win_02_02_on_2, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_02_02_on_3, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_02_02_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_02_02_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_03_01_on[5] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_03_01_on_1, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_03_01_on_2, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_03_01_on_3, (s32)&win_btlstage_data },
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_03_01_on_4, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

NpcBattleSetupEntry btlgrpsel_win_btlno_win_03_01_off[2] = {
    { 0x0A, (BattleGroupSetup*)&btlgrp_win_win_03_01_off_1, (s32)&win_btlstage_data },
    { 0x00, 0, 0 },
};

// TODO: type
u32 btlsetup_win_tbl[153] = {
    (u32)"マップstg_02_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_win_btlno_win_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"win_00ドクガッスン１", 0x00000000, 0x00000000,
    0xF84065FA, (u32)&btlgrpsel_win_btlno_win_00_01_on, (u32)&btlgrpsel_win_btlno_win_00_01_off, 0x00000000,
    0x00000000, 0x05050000, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x05050505, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"win_00モノクロシンエモン", 0x00000000,
    0x00000000, 0xF84065FA, (u32)&btlgrpsel_win_btlno_win_00_02_on, (u32)&btlgrpsel_win_btlno_win_00_02_off,
    0x00000000, 0x00000000, 0x05050000, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050505,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"win_00ドクガッスン３",
    0x00000000, 0x00000000, 0xF84065FA, (u32)&btlgrpsel_win_btlno_win_00_03_on,
    (u32)&btlgrpsel_win_btlno_win_00_03_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050505, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"win00ビビアン３姉妹", (u32)"win_00", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_win_btlno_win_00_04_off, 0x20000010, 0x00000000,
    0x05050000, 0x00000000, 0x05050505, 0x00000000,
    0x05050000, 0x05050505, 0x01010101, 0x01010101,
    (u32)"BGM_BOSS_STG3_3WOMEN1", (u32)"win_02シンエモン１", 0x00000000, 0x00000000,
    0xF84065F8, (u32)&btlgrpsel_win_btlno_win_02_01_on, (u32)&btlgrpsel_win_btlno_win_02_01_off, 0x00000000,
    0x00000000, 0x05050000, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x05050505, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"win_02パックンフラワー", 0x00000000,
    0x00000000, 0xF84065F8, (u32)&btlgrpsel_win_btlno_win_02_02_on, (u32)&btlgrpsel_win_btlno_win_02_02_off,
    0x00000000, 0x00000000, 0x05050000, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050A0A,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"win_03パックン１",
    0x00000000, 0x00000000, 0xF84065F9, (u32)&btlgrpsel_win_btlno_win_03_01_on,
    (u32)&btlgrpsel_win_btlno_win_03_01_off, 0x00000000, 0x00000000, 0x05050000,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050A0A, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btl_win_setup_no_tbl[18] = {
    (u32)"BTLNO_WIN_TEST1", 0x00000000, (u32)"BTLNO_WIN_00_01", 0x00000001,
    (u32)"BTLNO_WIN_00_02", 0x00000002, (u32)"BTLNO_WIN_00_03", 0x00000003,
    (u32)"BTLNO_WIN_00_04", 0x00000004, (u32)"BTLNO_WIN_02_01", 0x00000005,
    (u32)"BTLNO_WIN_02_02", 0x00000006, (u32)"BTLNO_WIN_03_01", 0x00000007,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
