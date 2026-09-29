#include "battle_database_bom.h"
#include "battle/battle_weapon_power.h"
#include "eff/n64/eff_snowfall_n64.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_item_bllizard[];
extern s32 battle_item_default_param_list[];
extern s32 battle_item_ice_pakkun[];
extern s32 unit_bllizard_3_data_B7F0[];
extern s32 unit_ice_pakkun_3_data_CDB0[];
extern s32 unit_kuriboo_3_data_AC58[];
s32 bgset_bom_snow_event[4];

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

// TODO: type
u32 bom_btlobject_data[66] = {
    (u32)"yuki_front", 0x00020001, 0x00000000, 0x00000000,
    0x42980000, 0x003C0000, (u32)"yuki_back", 0x00020001,
    0x00000000, 0x00000000, 0xC2200000, 0x0A320000,
    (u32)"kusa_01", 0x00020001, 0xC3270000, 0x00000000,
    0xC1A80000, 0x14280000, (u32)"kusa_02", 0x00020001,
    0x43220000, 0x00000000, 0xC1300000, 0x1E1E0000,
    (u32)"kabe_01", 0x00020001, 0x43420000, 0x00000000,
    0x42840000, 0x003C0000, (u32)"kabe_02", 0x00020001,
    0x42DE0000, 0x00000000, 0xC1C80000, 0x0A320000,
    (u32)"kabe_03", 0x00020000, 0xC28E0000, 0x00000000,
    0xC2700000, 0x003C0000, (u32)"ki_01", 0x00020000,
    0x43390000, 0x00000000, 0xC25C0000, 0x0A320000,
    (u32)"ki_02", 0x00020000, 0x42980000, 0x00000000,
    0xC2480000, 0x14280000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000,
};

// TODO: type
u32 bom_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_07_0", 0x0000000B, (u32)&bom_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, (u32)&bgset_bom_snow_event,
    0x00010101,
};

// TODO: type
u32 btlparty_bom_bom_00_01_off_1[12] = {
    (u32)unit_kuriboo_3_data_AC58, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_default_param_list,
};

// TODO: type
u32 btlgrp_bom_bom_00_01_off_1[8] = {
    0x00000001, (u32)&btlparty_bom_bom_00_01_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_03_01_off_1[24] = {
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_bom_bom_03_01_off_1[8] = {
    0x00000002, (u32)&btlparty_bom_bom_03_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_03_01_off_2[36] = {
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_bom_bom_03_01_off_2[8] = {
    0x00000003, (u32)&btlparty_bom_bom_03_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_03_03_off_1[24] = {
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_bom_bom_03_03_off_1[8] = {
    0x00000002, (u32)&btlparty_bom_bom_03_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_03_03_off_2[36] = {
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_bom_bom_03_03_off_2[8] = {
    0x00000003, (u32)&btlparty_bom_bom_03_03_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_04_01_off_1[24] = {
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_bom_bom_04_01_off_1[8] = {
    0x00000002, (u32)&btlparty_bom_bom_04_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_04_01_off_2[36] = {
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_bom_bom_04_01_off_2[8] = {
    0x00000003, (u32)&btlparty_bom_bom_04_01_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_04_02_off_1[24] = {
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_bom_bom_04_02_off_1[8] = {
    0x00000002, (u32)&btlparty_bom_bom_04_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_04_02_off_2[36] = {
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_bllizard_3_data_B7F0, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_bom_bom_04_02_off_2[8] = {
    0x00000003, (u32)&btlparty_bom_bom_04_02_off_2, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_bom_bom_04_03_off_1[48] = {
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_3_data_CDB0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_bom_bom_04_03_off_1[8] = {
    0x00000004, (u32)&btlparty_bom_bom_04_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_test1_off[6] = {
    0x00000032, (u32)&btlgrp_bom_bom_00_01_off_1, (u32)&bom_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_03_01_off[9] = {
    0x00000014, (u32)&btlgrp_bom_bom_03_01_off_1, (u32)&bom_btlstage_data, 0x0000000A,
    (u32)&btlgrp_bom_bom_03_01_off_2, (u32)&bom_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_03_02_off[9] = {
    0x0000000A, (u32)&btlgrp_bom_bom_03_01_off_1, (u32)&bom_btlstage_data, 0x00000014,
    (u32)&btlgrp_bom_bom_03_01_off_2, (u32)&bom_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_03_03_off[9] = {
    0x0000000A, (u32)&btlgrp_bom_bom_03_03_off_1, (u32)&bom_btlstage_data, 0x0000000A,
    (u32)&btlgrp_bom_bom_03_03_off_2, (u32)&bom_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_04_01_off[9] = {
    0x0000000A, (u32)&btlgrp_bom_bom_04_01_off_1, (u32)&bom_btlstage_data, 0x0000000A,
    (u32)&btlgrp_bom_bom_04_01_off_2, (u32)&bom_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_04_02_off[9] = {
    0x0000000A, (u32)&btlgrp_bom_bom_04_02_off_1, (u32)&bom_btlstage_data, 0x0000000A,
    (u32)&btlgrp_bom_bom_04_02_off_2, (u32)&bom_btlstage_data, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlgrpsel_bom_btlno_bom_04_03_off[6] = {
    0x0000000A, (u32)&btlgrp_bom_bom_04_03_off_1, (u32)&bom_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_bom_tbl[136] = {
    (u32)"マップstg_07_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_bom_btlno_bom_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"bom03ブリザード", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_bom_btlno_bom_03_01_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"bom03ブリザード", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_bom_btlno_bom_03_02_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"bom03アイスパックン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_bom_btlno_bom_03_03_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030A0A, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"bom04アイスパックン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_bom_btlno_bom_04_01_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030A0A, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"bom04ブリザード", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_bom_btlno_bom_04_02_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"bom04アイスパックン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_bom_btlno_bom_04_03_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030A0A,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btl_bom_setup_no_tbl[16] = {
    (u32)"BTLNO_BOM_TEST1", 0x00000000, (u32)"BTLNO_BOM_03_01", 0x00000001,
    (u32)"BTLNO_BOM_03_02", 0x00000002, (u32)"BTLNO_BOM_03_03", 0x00000003,
    (u32)"BTLNO_BOM_04_01", 0x00000004, (u32)"BTLNO_BOM_04_02", 0x00000005,
    (u32)"BTLNO_BOM_04_03", 0x00000006, 0x00000000, 0x00000000,
};

USER_FUNC(_snow_control) {
    if (isFirstCall) {
        effSnowfallN64Entry(0, 0x40);
    }
    return 0;
}

EVT_BEGIN(bgset_bom_snow_event)
    CALL(_snow_control)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
