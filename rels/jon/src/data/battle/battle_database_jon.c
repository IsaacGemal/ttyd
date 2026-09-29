#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_flower_drop_param_default4[];
extern s32 battle_flower_drop_param_default5[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_heart_drop_param_default3[];
extern s32 battle_heart_drop_param_default4[];
extern s32 battle_heart_drop_param_default5[];
extern s32 battle_item_badge_borodo[];
extern s32 battle_item_basabasa_chururu[];
extern s32 battle_item_basabasa_green[];
extern s32 battle_item_bllizard[];
extern s32 battle_item_bomhei[];
extern s32 battle_item_borodo[];
extern s32 battle_item_bubble[];
extern s32 battle_item_burst_wanwan[];
extern s32 battle_item_chorobon[];
extern s32 battle_item_churantalar[];
extern s32 battle_item_dark_keeper[];
extern s32 battle_item_dokugassun[];
extern s32 battle_item_flower_chorobon[];
extern s32 battle_item_giant_bomb[];
extern s32 battle_item_hannya[];
extern s32 battle_item_heavy_bom[];
extern s32 battle_item_hennya[];
extern s32 battle_item_hinnya[];
extern s32 battle_item_honenoko[];
extern s32 battle_item_hyper_jyugem[];
extern s32 battle_item_hyper_sinemon[];
extern s32 battle_item_ice_pakkun[];
extern s32 battle_item_jyugem[];
extern s32 battle_item_karon[];
extern s32 battle_item_mahorn[];
extern s32 battle_item_mahorn_custom[];
extern s32 battle_item_monochrome_kurokumorn[];
extern s32 battle_item_pakkun_flower[];
extern s32 battle_item_patamet[];
extern s32 battle_item_patatogemet[];
extern s32 battle_item_phantom[];
extern s32 battle_item_piders[];
extern s32 battle_item_purple_teresa[];
extern s32 battle_item_sambo[];
extern s32 battle_item_sambo_mummy[];
extern s32 battle_item_sinemon[];
extern s32 battle_item_super_mahorn[];
extern s32 battle_item_teresa[];
extern s32 battle_item_togenoko_ace[];
extern s32 battle_item_twinkling_pansy[];
extern s32 battle_item_ura_noko[];
extern s32 battle_item_wanwan[];
extern s32 battle_item_yami_kuriboo[];
extern s32 battle_item_yami_noko[];
extern s32 battle_item_yami_patakuri[];
extern s32 battle_item_yami_togekuri[];
extern s32 battle_item_yamitogedaruma[];
extern s32 battle_item_zonbaba[];
extern s32 unit_badge_borodo[];
extern s32 unit_basabasa_chururu_15_data_62428[];
extern s32 unit_basabasa_green[];
extern s32 unit_bllizard_15_data_60E68[];
extern s32 unit_bomhei_15_data_24850[];
extern s32 unit_borodo_15_data_211F8[];
extern s32 unit_boss_zonbaba[];
extern s32 unit_bubble_15_data_35518[];
extern s32 unit_burst_wanwan_15_data_5F390[];
extern s32 unit_chorobon_15_data_127B8[];
extern s32 unit_churantalar[];
extern s32 unit_dark_keeper_15_data_5E0F0[];
extern s32 unit_dokugassun_15_data_5CC90[];
extern s32 unit_flower_chorobon_15_data_2FE08[];
extern s32 unit_giant_bomb[];
extern s32 unit_hannya_15_data_13DF0[];
extern s32 unit_heavy_bom_15_data_33E08[];
extern s32 unit_hennya_15_data_5A9C0[];
extern s32 unit_hinnya_15_data_149B0[];
extern s32 unit_honenoko_15_data_15568[];
extern s32 unit_hyper_jyugem_15_data_58998[];
extern s32 unit_hyper_sinemon_15_data_2A3A8[];
extern s32 unit_ice_pakkun_15_data_577D8[];
extern s32 unit_jyugem_15_data_22B10[];
extern s32 unit_karon_15_data_55368[];
extern s32 unit_mahorn[];
extern s32 unit_mahorn_custom[];
extern s32 unit_monochrome_kurokumorn_15_data_1E998[];
extern s32 unit_monochrome_sinemon_15_data_191F8[];
extern s32 unit_pakkun_flower_15_data_4C6D0[];
extern s32 unit_patamet_15_data_2B728[];
extern s32 unit_patatogemet_15_data_3B6C8[];
extern s32 unit_phantom_15_data_4AC38[];
extern s32 unit_piders_15_data_66660[];
extern s32 unit_purple_teresa_15_data_493B8[];
extern s32 unit_sambo_15_data_1A280[];
extern s32 unit_sambo_mummy_15_data_36FB0[];
extern s32 unit_sinemon_15_data_48330[];
extern s32 unit_super_mahorn_15_data_43F68[];
extern s32 unit_teresa_15_data_25D30[];
extern s32 unit_togenoko_ace_15_data_41A18[];
extern s32 unit_twinkling_pansy_15_data_40EC8[];
extern s32 unit_ura_noko_15_data_2E060[];
extern s32 unit_wanwan_15_data_3F3F0[];
extern s32 unit_yami_kuriboo[];
extern s32 unit_yami_noko[];
extern s32 unit_yami_pata[];
extern s32 unit_yami_patakuri[];
extern s32 unit_yami_togekuri[];
extern s32 unit_yamitogedaruma[];

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 jon_btlobject_data[48] = {
    (u32)"B_0", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"jon_0", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B_1", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"jon_1", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B_2", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"jon_2", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"B_6", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"jon_6", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
};

// TODO: type
u32 jon_btlstage_data[436] = {
    (u32)"bti_01", (u32)"stg_00_3", 0x00000002, (u32)&jon_btlobject_data,
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
    0x00000000, (u32)"bti_01", (u32)"stg_00_3", 0x00000002,
    (u32)((u8*)&jon_btlobject_data + 0x30), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000002, (u32)((u8*)&jon_btlobject_data + 0x60), 0x00000000, 0x00000000,
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
    (u32)"stg_00_3", 0x00000002, (u32)((u8*)&jon_btlobject_data + 0x90), 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
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
u32 btlparty_jon_jon_100_01_off_1[12] = {
    (u32)unit_boss_zonbaba, 0x01000000, 0x04000004, 0x43B68000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_zonbaba,
};

// TODO: type
u32 btlgrp_jon_jon_100_01_off_1[8] = {
    0x00000001, (u32)&btlparty_jon_jon_100_01_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_01_off_1[12] = {
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
};

// TODO: type
u32 btlgrp_jon_jon_01_01_off_1[8] = {
    0x00000001, (u32)&btlparty_jon_jon_01_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_01_off_2[24] = {
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_jon_jon_01_01_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_01_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_01_off_3[24] = {
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
};

// TODO: type
u32 btlgrp_jon_jon_01_01_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_01_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_02_off_1[60] = {
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_jon_jon_01_02_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_01_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_02_off_2[24] = {
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
};

// TODO: type
u32 btlgrp_jon_jon_01_02_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_01_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_02_off_3[60] = {
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_jon_jon_01_02_off_3[8] = {
    0x00000005, (u32)&btlparty_jon_jon_01_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_03_off_1[48] = {
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
};

// TODO: type
u32 btlgrp_jon_jon_01_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_01_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_03_off_2[24] = {
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
};

// TODO: type
u32 btlgrp_jon_jon_01_03_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_01_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_03_off_3[48] = {
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_hannya_15_data_13DF0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hannya,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_01_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_01_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_04_off_1[48] = {
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
};

// TODO: type
u32 btlgrp_jon_jon_01_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_01_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_04_off_2[24] = {
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
};

// TODO: type
u32 btlgrp_jon_jon_01_04_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_01_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_04_off_3[36] = {
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
    (u32)unit_hinnya_15_data_149B0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hinnya,
};

// TODO: type
u32 btlgrp_jon_jon_01_04_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_01_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_05_off_1[60] = {
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_01_05_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_01_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_05_off_2[36] = {
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_yami_kuriboo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_kuriboo,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_01_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_01_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_01_05_off_3[48] = {
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
    (u32)unit_chorobon_15_data_127B8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_chorobon,
    (u32)unit_honenoko_15_data_15568, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_honenoko,
};

// TODO: type
u32 btlgrp_jon_jon_01_05_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_01_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_01_off_1[24] = {
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_11_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jon_jon_11_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_01_off_2[36] = {
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
};

// TODO: type
u32 btlgrp_jon_jon_11_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_01_off_3[36] = {
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
};

// TODO: type
u32 btlgrp_jon_jon_11_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_02_off_1[48] = {
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_jon_jon_11_02_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_11_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_02_off_2[36] = {
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_11_02_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_02_off_3[36] = {
    (u32)unit_monochrome_sinemon_15_data_191F8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_jon_jon_11_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_03_off_1[48] = {
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
};

// TODO: type
u32 btlgrp_jon_jon_11_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_11_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_03_off_2[24] = {
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_jon_jon_11_03_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_11_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_03_off_3[48] = {
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
};

// TODO: type
u32 btlgrp_jon_jon_11_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_11_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_04_off_1[36] = {
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_jon_jon_11_04_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_04_off_2[36] = {
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
};

// TODO: type
u32 btlgrp_jon_jon_11_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_11_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_04_off_3[24] = {
    (u32)unit_monochrome_kurokumorn_15_data_1E998, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monochrome_kurokumorn,
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_11_04_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_11_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_05_off_1[48] = {
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
};

// TODO: type
u32 btlgrp_jon_jon_11_05_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_11_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_05_off_2[24] = {
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_yami_patakuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_11_05_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_11_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_11_05_off_3[48] = {
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_piders_15_data_66660, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_piders,
    (u32)unit_sambo_15_data_1A280, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
};

// TODO: type
u32 btlgrp_jon_jon_11_05_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_11_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_01_off_1[36] = {
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
};

// TODO: type
u32 btlgrp_jon_jon_21_01_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_21_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_01_off_2[48] = {
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
};

// TODO: type
u32 btlgrp_jon_jon_21_01_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_01_off_3[48] = {
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_21_01_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_02_off_1[60] = {
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
};

// TODO: type
u32 btlgrp_jon_jon_21_02_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_21_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_02_off_2[24] = {
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
};

// TODO: type
u32 btlgrp_jon_jon_21_02_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_21_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_02_off_3[60] = {
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
};

// TODO: type
u32 btlgrp_jon_jon_21_02_off_3[8] = {
    0x00000005, (u32)&btlparty_jon_jon_21_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_03_off_1[48] = {
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
};

// TODO: type
u32 btlgrp_jon_jon_21_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_03_off_2[24] = {
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
};

// TODO: type
u32 btlgrp_jon_jon_21_03_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_21_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_03_off_3[48] = {
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_jyugem_15_data_22B10, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_jyugem,
};

// TODO: type
u32 btlgrp_jon_jon_21_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_04_off_1[48] = {
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
};

// TODO: type
u32 btlgrp_jon_jon_21_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_04_off_2[36] = {
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
};

// TODO: type
u32 btlgrp_jon_jon_21_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_21_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_04_off_3[48] = {
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_bomhei_15_data_24850, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_21_04_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_05_off_1[48] = {
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_21_05_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_05_off_2[36] = {
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_yami_togekuri, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_togekuri,
};

// TODO: type
u32 btlgrp_jon_jon_21_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_21_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_21_05_off_3[48] = {
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_teresa_15_data_25D30, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_teresa,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_15_data_211F8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
};

// TODO: type
u32 btlgrp_jon_jon_21_05_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_21_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_01_off_1[24] = {
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
};

// TODO: type
u32 btlgrp_jon_jon_31_01_off_1[8] = {
    0x00000002, (u32)&btlparty_jon_jon_31_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_01_off_2[36] = {
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
};

// TODO: type
u32 btlgrp_jon_jon_31_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_01_off_3[36] = {
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_31_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_02_off_1[36] = {
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_jon_jon_31_02_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_02_off_2[36] = {
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_jon_jon_31_02_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_02_off_3[36] = {
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
};

// TODO: type
u32 btlgrp_jon_jon_31_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_03_off_1[48] = {
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
};

// TODO: type
u32 btlgrp_jon_jon_31_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_31_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_03_off_2[48] = {
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
};

// TODO: type
u32 btlgrp_jon_jon_31_03_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_31_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_03_off_3[24] = {
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_31_03_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_31_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_04_off_1[48] = {
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
};

// TODO: type
u32 btlgrp_jon_jon_31_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_31_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_04_off_2[36] = {
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
};

// TODO: type
u32 btlgrp_jon_jon_31_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_04_off_3[36] = {
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_noko_15_data_2E060, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_patamet_15_data_2B728, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patamet,
};

// TODO: type
u32 btlgrp_jon_jon_31_04_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_05_off_1[60] = {
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_jon_jon_31_05_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_31_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_05_off_2[36] = {
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_yami_noko, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_noko,
};

// TODO: type
u32 btlgrp_jon_jon_31_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_31_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_31_05_off_3[24] = {
    (u32)unit_flower_chorobon_15_data_2FE08, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_hyper_sinemon_15_data_2A3A8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinemon,
};

// TODO: type
u32 btlgrp_jon_jon_31_05_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_31_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_01_off_1[48] = {
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_41_01_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_01_off_2[36] = {
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
};

// TODO: type
u32 btlgrp_jon_jon_41_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_41_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_01_off_3[36] = {
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
};

// TODO: type
u32 btlgrp_jon_jon_41_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_41_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_02_off_1[48] = {
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
};

// TODO: type
u32 btlgrp_jon_jon_41_02_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_02_off_2[24] = {
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
};

// TODO: type
u32 btlgrp_jon_jon_41_02_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_41_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_02_off_3[36] = {
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
};

// TODO: type
u32 btlgrp_jon_jon_41_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_41_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_03_off_1[36] = {
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
};

// TODO: type
u32 btlgrp_jon_jon_41_03_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_41_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_03_off_2[36] = {
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_41_03_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_41_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_03_off_3[48] = {
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_heavy_bom_15_data_33E08, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_heavy_bom,
};

// TODO: type
u32 btlgrp_jon_jon_41_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_04_off_1[48] = {
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
};

// TODO: type
u32 btlgrp_jon_jon_41_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_04_off_2[48] = {
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
};

// TODO: type
u32 btlgrp_jon_jon_41_04_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_04_off_3[24] = {
    (u32)unit_sambo_mummy_15_data_36FB0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo_mummy,
    (u32)unit_yami_pata, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yami_patakuri,
};

// TODO: type
u32 btlgrp_jon_jon_41_04_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_41_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_05_off_1[24] = {
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
};

// TODO: type
u32 btlgrp_jon_jon_41_05_off_1[8] = {
    0x00000002, (u32)&btlparty_jon_jon_41_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_05_off_2[48] = {
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
};

// TODO: type
u32 btlgrp_jon_jon_41_05_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_41_05_off_3[48] = {
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_patatogemet_15_data_3B6C8, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patatogemet,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
    (u32)unit_bubble_15_data_35518, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bubble,
};

// TODO: type
u32 btlgrp_jon_jon_41_05_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_41_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_01_off_1[36] = {
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
};

// TODO: type
u32 btlgrp_jon_jon_51_01_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_01_off_2[36] = {
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_jon_jon_51_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_01_off_3[36] = {
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_51_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_02_off_1[48] = {
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_jon_jon_51_02_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_51_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_02_off_2[36] = {
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_jon_jon_51_02_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_02_off_3[24] = {
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_jon_jon_51_02_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_51_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_03_off_1[48] = {
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_51_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_51_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_03_off_2[36] = {
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_badge_borodo, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_badge_borodo,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_51_03_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_03_off_3[36] = {
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_51_03_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_04_off_1[36] = {
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_51_04_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_04_off_2[36] = {
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_51_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_04_off_3[36] = {
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
    (u32)unit_bllizard_15_data_60E68, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_jon_jon_51_04_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_05_off_1[48] = {
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
};

// TODO: type
u32 btlgrp_jon_jon_51_05_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_51_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_05_off_2[36] = {
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_burst_wanwan_15_data_5F390, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_51_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_51_05_off_3[36] = {
    (u32)unit_sinemon_15_data_48330, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sinemon,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_15_data_493B8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_jon_jon_51_05_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_51_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_01_off_1[36] = {
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
};

// TODO: type
u32 btlgrp_jon_jon_61_01_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_01_off_2[36] = {
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
};

// TODO: type
u32 btlgrp_jon_jon_61_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_01_off_3[24] = {
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_61_01_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_61_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_02_off_1[48] = {
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
};

// TODO: type
u32 btlgrp_jon_jon_61_02_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_61_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_02_off_2[24] = {
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
};

// TODO: type
u32 btlgrp_jon_jon_61_02_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_61_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_02_off_3[36] = {
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_jon_jon_61_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_03_off_1[36] = {
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_61_03_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_03_off_2[36] = {
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_61_03_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_03_off_3[36] = {
    (u32)unit_super_mahorn_15_data_43F68, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_super_mahorn,
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
    (u32)unit_karon_15_data_55368, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_karon,
};

// TODO: type
u32 btlgrp_jon_jon_61_03_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_04_off_1[48] = {
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
};

// TODO: type
u32 btlgrp_jon_jon_61_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_61_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_04_off_2[36] = {
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
};

// TODO: type
u32 btlgrp_jon_jon_61_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_04_off_3[24] = {
    (u32)unit_ice_pakkun_15_data_577D8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ice_pakkun,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
};

// TODO: type
u32 btlgrp_jon_jon_61_04_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_61_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_05_off_1[36] = {
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
};

// TODO: type
u32 btlgrp_jon_jon_61_05_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_61_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_05_off_2[24] = {
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
    (u32)unit_hyper_jyugem_15_data_58998, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_jyugem,
};

// TODO: type
u32 btlgrp_jon_jon_61_05_off_2[8] = {
    0x00000002, (u32)&btlparty_jon_jon_61_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_61_05_off_3[24] = {
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
    (u32)unit_dark_keeper_15_data_5E0F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dark_keeper,
};

// TODO: type
u32 btlgrp_jon_jon_61_05_off_3[8] = {
    0x00000002, (u32)&btlparty_jon_jon_61_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_01_off_1[36] = {
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_01_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_01_off_2[36] = {
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_01_off_3[36] = {
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_71_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_02_off_1[36] = {
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
};

// TODO: type
u32 btlgrp_jon_jon_71_02_off_1[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_02_off_2[36] = {
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_71_02_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_02_off_3[36] = {
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_03_off_1[60] = {
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
};

// TODO: type
u32 btlgrp_jon_jon_71_03_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_71_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_03_off_2[48] = {
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_03_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_03_off_3[48] = {
    (u32)unit_phantom_15_data_4AC38, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_phantom,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
};

// TODO: type
u32 btlgrp_jon_jon_71_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_04_off_1[48] = {
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x43160000,
    0x43020000, 0x41A00000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
};

// TODO: type
u32 btlgrp_jon_jon_71_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_04_off_2[48] = {
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_04_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_04_off_3[48] = {
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_basabasa_chururu_15_data_62428, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_chururu,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
};

// TODO: type
u32 btlgrp_jon_jon_71_04_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default2, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_05_off_1[48] = {
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
};

// TODO: type
u32 btlgrp_jon_jon_71_05_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_71_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_05_off_2[36] = {
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
    (u32)unit_togenoko_ace_15_data_41A18, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togenoko_ace,
};

// TODO: type
u32 btlgrp_jon_jon_71_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_71_05_off_3[36] = {
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_wanwan_15_data_3F3F0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_wanwan,
    (u32)unit_mahorn, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn,
};

// TODO: type
u32 btlgrp_jon_jon_71_05_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_71_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_01_off_1[48] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_01_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_01_off_2[48] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
};

// TODO: type
u32 btlgrp_jon_jon_81_01_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_01_off_3[36] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
};

// TODO: type
u32 btlgrp_jon_jon_81_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_02_off_1[24] = {
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
};

// TODO: type
u32 btlgrp_jon_jon_81_02_off_1[8] = {
    0x00000002, (u32)&btlparty_jon_jon_81_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_02_off_2[48] = {
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
};

// TODO: type
u32 btlgrp_jon_jon_81_02_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_02_off_3[36] = {
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_02_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_03_off_1[48] = {
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
};

// TODO: type
u32 btlgrp_jon_jon_81_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_03_off_2[36] = {
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
};

// TODO: type
u32 btlgrp_jon_jon_81_03_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_03_off_3[36] = {
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_churantalar, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_churantalar,
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
};

// TODO: type
u32 btlgrp_jon_jon_81_03_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_04_off_1[48] = {
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
};

// TODO: type
u32 btlgrp_jon_jon_81_04_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_04_off_2[36] = {
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_yamitogedaruma, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_yamitogedaruma,
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
};

// TODO: type
u32 btlgrp_jon_jon_81_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_04_off_3[48] = {
    (u32)unit_pakkun_flower_15_data_4C6D0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_pakkun_flower,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_04_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_05_off_1[48] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_05_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_81_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_05_off_2[36] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_05_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_81_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_81_05_off_3[60] = {
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
    (u32)unit_hennya_15_data_5A9C0, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hennya,
};

// TODO: type
u32 btlgrp_jon_jon_81_05_off_3[8] = {
    0x00000005, (u32)&btlparty_jon_jon_81_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_01_off_1[60] = {
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
};

// TODO: type
u32 btlgrp_jon_jon_91_01_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_91_01_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_01_off_2[36] = {
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
};

// TODO: type
u32 btlgrp_jon_jon_91_01_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_91_01_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_01_off_3[36] = {
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x43020000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
};

// TODO: type
u32 btlgrp_jon_jon_91_01_off_3[8] = {
    0x00000003, (u32)&btlparty_jon_jon_91_01_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default4, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_02_off_1[48] = {
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x43160000,
    0x42200000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
};

// TODO: type
u32 btlgrp_jon_jon_91_02_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_02_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_02_off_2[48] = {
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x428C0000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
};

// TODO: type
u32 btlgrp_jon_jon_91_02_off_2[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_02_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_02_off_3[48] = {
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x41F00000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x42DC0000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_02_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_02_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_03_off_1[48] = {
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
};

// TODO: type
u32 btlgrp_jon_jon_91_03_off_1[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_03_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default5, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_03_off_2[36] = {
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
};

// TODO: type
u32 btlgrp_jon_jon_91_03_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_91_03_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default5, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_03_off_3[48] = {
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_giant_bomb, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_giant_bomb,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_03_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_03_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default5, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_04_off_1[60] = {
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x41200000,
    0x43020000, 0xC1A00000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x42480000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x42B40000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x43020000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x432A0000,
    0x43020000, 0x41A00000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
};

// TODO: type
u32 btlgrp_jon_jon_91_04_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_91_04_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_04_off_2[36] = {
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x42480000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_dokugassun_15_data_5CC90, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
};

// TODO: type
u32 btlgrp_jon_jon_91_04_off_2[8] = {
    0x00000003, (u32)&btlparty_jon_jon_91_04_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_04_off_3[48] = {
    (u32)unit_basabasa_green, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa_green,
    (u32)unit_mahorn_custom, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_mahorn_custom,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_04_off_3[8] = {
    0x00000004, (u32)&btlparty_jon_jon_91_04_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default4, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_05_off_1[60] = {
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_05_off_1[8] = {
    0x00000005, (u32)&btlparty_jon_jon_91_05_off_1, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_05_off_2[60] = {
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_05_off_2[8] = {
    0x00000005, (u32)&btlparty_jon_jon_91_05_off_2, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlparty_jon_jon_91_05_off_3[60] = {
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
    (u32)unit_twinkling_pansy_15_data_40EC8, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_twinkling_pansy,
};

// TODO: type
u32 btlgrp_jon_jon_91_05_off_3[8] = {
    0x00000005, (u32)&btlparty_jon_jon_91_05_off_3, 0x00000014, 0x00000014,
    0x0000003C, (u32)battle_heart_drop_param_default3, (u32)battle_flower_drop_param_default5, 0x00000005,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_test1_off[6] = {
    0x00000032, (u32)&btlgrp_jon_jon_01_01_off_1, (u32)&jon_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_01_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_01_01_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_01_01_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_01_01_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_02_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_01_02_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_01_02_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_01_02_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_03_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_01_03_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_01_03_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_01_03_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_04_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_01_04_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_01_04_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_01_04_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_05_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_01_05_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_01_05_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_01_05_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_11_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_11_01_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_11_01_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_11_01_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_12_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_11_02_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_11_02_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_11_02_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_13_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_11_03_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_11_03_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_11_03_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_14_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_11_04_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_11_04_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_11_04_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_15_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_11_05_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_11_05_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_11_05_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_21_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_21_01_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_21_01_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_21_01_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_22_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_21_02_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_21_02_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_21_02_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_23_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_21_03_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_21_03_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_21_03_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_24_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_21_04_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_21_04_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_21_04_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_25_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_21_05_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_21_05_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_21_05_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_31_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_31_01_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_31_01_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_31_01_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_32_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_31_02_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_31_02_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_31_02_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_33_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_31_03_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_31_03_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_31_03_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_34_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_31_04_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_31_04_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_31_04_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_35_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_31_05_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_31_05_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_31_05_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_41_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_41_01_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_41_01_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_41_01_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_42_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_41_02_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_41_02_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_41_02_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_43_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_41_03_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_41_03_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_41_03_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_44_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_41_04_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_41_04_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_41_04_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_45_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_41_05_off_1, (u32)&jon_btlstage_data, 0x0000000A,
    (u32)&btlgrp_jon_jon_41_05_off_2, (u32)&jon_btlstage_data, 0x0000000A, (u32)&btlgrp_jon_jon_41_05_off_3,
    (u32)&jon_btlstage_data, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_51_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_51_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_51_01_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_51_01_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_52_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_51_02_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_51_02_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_51_02_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_53_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_51_03_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_51_03_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_51_03_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_54_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_51_04_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_51_04_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_51_04_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_55_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_51_05_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_51_05_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_51_05_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_61_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_61_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_61_01_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_61_01_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_62_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_61_02_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_61_02_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_61_02_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_63_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_61_03_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_61_03_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_61_03_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_64_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_61_04_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_61_04_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_61_04_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_65_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_61_05_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_61_05_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_61_05_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_71_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_71_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_71_01_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_71_01_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_72_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_71_02_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_71_02_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_71_02_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_73_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_71_03_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_71_03_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_71_03_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_74_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_71_04_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_71_04_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_71_04_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_75_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_71_05_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_71_05_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_71_05_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_81_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_81_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_81_01_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_81_01_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_82_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_81_02_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_81_02_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_81_02_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_83_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_81_03_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_81_03_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_81_03_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_84_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_81_04_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_81_04_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_81_04_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_85_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_81_05_off_1, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_jon_jon_81_05_off_2, (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_jon_jon_81_05_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_91_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_91_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A,
    (u32)&btlgrp_jon_jon_91_01_off_2, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jon_jon_91_01_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x368), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_92_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_91_02_off_1, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A,
    (u32)&btlgrp_jon_jon_91_02_off_2, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jon_jon_91_02_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x368), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_93_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_91_03_off_1, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A,
    (u32)&btlgrp_jon_jon_91_03_off_2, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jon_jon_91_03_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x368), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_94_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_91_04_off_1, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A,
    (u32)&btlgrp_jon_jon_91_04_off_2, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jon_jon_91_04_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x368), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_95_off[12] = {
    0x0000000A, (u32)&btlgrp_jon_jon_91_05_off_1, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A,
    (u32)&btlgrp_jon_jon_91_05_off_2, (u32)((u8*)&jon_btlstage_data + 0x368), 0x0000000A, (u32)&btlgrp_jon_jon_91_05_off_3,
    (u32)((u8*)&jon_btlstage_data + 0x368), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_jon_btlno_jon_100_zonbaba_off[6] = {
    0x0000000A, (u32)&btlgrp_jon_jon_100_01_off_1, (u32)((u8*)&jon_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_jon_tbl[901] = {
    (u32)"マップstg_00_3", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_test1_off, 0x10000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ヤミクリボー", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_01_off, 0x10000000,
    0x00000000, 0x05050000, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x0A0A0000, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ヒンニャー", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_02_off,
    0x10000000, 0x00000000, 0x05050000, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ハンニャー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_03_off, 0x10000000, 0x00000000, 0x05050000,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ホネノコ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_04_off, 0x10000000, 0x00000000,
    0x05050000, 0x00000000, 0x0A0A0505, 0x00000000,
    0x05050000, 0x05050000, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"チョロボン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_05_off, 0x10000000,
    0x00000000, 0x05050000, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x05050000, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ヤミパタクリ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_11_off,
    0x10000000, 0x00000000, 0x05050505, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x0A0A0000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"モノクロシンエモン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_12_off, 0x10000000, 0x00000000, 0x05050505,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x05050000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"サンボ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_13_off, 0x10000000, 0x00000000,
    0x05050505, 0x00000000, 0x05050505, 0x00000000,
    0x05050000, 0x05050000, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"モノクロクロクモーン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_14_off, 0x10000000,
    0x00000000, 0x05050505, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x05050000, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"パイダース", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_15_off,
    0x10000000, 0x00000000, 0x05050505, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ヤミトゲクリ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_21_off, 0x10000000, 0x00000000, 0x05050505,
    0x00000000, 0x05050505, 0x00000000, 0x05050000,
    0x0A0A0000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ボロドー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_22_off, 0x10000000, 0x00000000,
    0x05050505, 0x00000000, 0x05050505, 0x00000000,
    0x05050000, 0x05050000, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ジュゲム", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_23_off, 0x10000000,
    0x00000000, 0x05050505, 0x00000000, 0x05050505,
    0x00000000, 0x05050000, 0x05050000, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ボムへい", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_24_off,
    0x10000000, 0x00000000, 0x05050505, 0x00000000,
    0x05050505, 0x00000000, 0x05050000, 0x05050000,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"テレサ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_25_off, 0x10000000, 0x00000000, 0x05050505,
    0x0A0A0000, 0x05050505, 0x00000000, 0x05050000,
    0x05050000, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ヤミノコノコ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_31_off, 0x10000000, 0x00000000,
    0x04040404, 0x04040000, 0x04040404, 0x04040404,
    0x0A0A0000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ハイパーシンエモン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_32_off, 0x10000000,
    0x00000000, 0x04040404, 0x04040000, 0x04040404,
    0x04040404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"パタメット", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_33_off,
    0x10000000, 0x00000000, 0x04040404, 0x04040000,
    0x04040404, 0x04040404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ウラノコ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_34_off, 0x10000000, 0x00000000, 0x04040404,
    0x04040000, 0x04040404, 0x04040404, 0x0A0A0000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"フラワーチョロボン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_35_off, 0x10000000, 0x00000000,
    0x04040404, 0x04040000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ヤミパタパタ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_41_off, 0x10000000,
    0x00000000, 0x04040404, 0x04040000, 0x04040404,
    0x04040404, 0x0A0A0000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ヘビーボム", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_42_off,
    0x10000000, 0x00000000, 0x04040404, 0x04040000,
    0x04040404, 0x04040404, 0x04040A0A, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"バブル",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_43_off, 0x10000000, 0x00000000, 0x04040404,
    0x04040000, 0x04040404, 0x04040404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"サンボマミー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_44_off, 0x10000000, 0x00000000,
    0x04040404, 0x04040000, 0x04040404, 0x04040404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"パタトゲメット", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_45_off, 0x10000000,
    0x00000000, 0x04040404, 0x04040000, 0x04040404,
    0x04040404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"バッジボロドー", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_51_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ブリザード",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_52_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"パープルテレサ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_53_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"バーストワンワン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_54_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"シンエモン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_55_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ハイパージュゲム",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_61_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"カロン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_62_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"スーパーマホーン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_63_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"アイスパックン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_64_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ダークキーパー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_65_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"マホーン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_71_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"トゲノコエース", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_72_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ファントム", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_73_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"バサバサチュルル",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_74_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ワンワン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_75_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ヘンニャー", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_81_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ヤミトゲダルマー", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_82_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"チュランタラー",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_83_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"パックンフラワー", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_84_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ヘンニャー", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_85_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マホーンカスタム", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_91_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ドクガッスン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_92_off, 0x10000000, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ジャイアントボム", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_jon_btlno_jon_93_off, 0x10000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"バサバサグリーン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_94_off, 0x10000000,
    0x00000000, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"きらめくパンジーさん", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_jon_btlno_jon_95_off,
    0x10000000, 0x00000000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ゾンババ",
    (u32)"jon_06", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_jon_btlno_jon_100_zonbaba_off, 0x20100010, 0x00000000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_BOSS_STG1_GONBABA1",
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btl_jon_setup_no_tbl[106] = {
    (u32)"BTLNO_JON_TEST1", 0x00000000, (u32)"BTLNO_JON_01", 0x00000001,
    (u32)"BTLNO_JON_02", 0x00000002, (u32)"BTLNO_JON_03", 0x00000003,
    (u32)"BTLNO_JON_04", 0x00000004, (u32)"BTLNO_JON_05", 0x00000005,
    (u32)"BTLNO_JON_11", 0x00000006, (u32)"BTLNO_JON_12", 0x00000007,
    (u32)"BTLNO_JON_13", 0x00000008, (u32)"BTLNO_JON_14", 0x00000009,
    (u32)"BTLNO_JON_15", 0x0000000A, (u32)"BTLNO_JON_21", 0x0000000B,
    (u32)"BTLNO_JON_22", 0x0000000C, (u32)"BTLNO_JON_23", 0x0000000D,
    (u32)"BTLNO_JON_24", 0x0000000E, (u32)"BTLNO_JON_25", 0x0000000F,
    (u32)"BTLNO_JON_31", 0x00000010, (u32)"BTLNO_JON_32", 0x00000011,
    (u32)"BTLNO_JON_33", 0x00000012, (u32)"BTLNO_JON_34", 0x00000013,
    (u32)"BTLNO_JON_35", 0x00000014, (u32)"BTLNO_JON_41", 0x00000015,
    (u32)"BTLNO_JON_42", 0x00000016, (u32)"BTLNO_JON_43", 0x00000017,
    (u32)"BTLNO_JON_44", 0x00000018, (u32)"BTLNO_JON_45", 0x00000019,
    (u32)"BTLNO_JON_51", 0x0000001A, (u32)"BTLNO_JON_52", 0x0000001B,
    (u32)"BTLNO_JON_53", 0x0000001C, (u32)"BTLNO_JON_54", 0x0000001D,
    (u32)"BTLNO_JON_55", 0x0000001E, (u32)"BTLNO_JON_61", 0x0000001F,
    (u32)"BTLNO_JON_62", 0x00000020, (u32)"BTLNO_JON_63", 0x00000021,
    (u32)"BTLNO_JON_64", 0x00000022, (u32)"BTLNO_JON_65", 0x00000023,
    (u32)"BTLNO_JON_71", 0x00000024, (u32)"BTLNO_JON_72", 0x00000025,
    (u32)"BTLNO_JON_73", 0x00000026, (u32)"BTLNO_JON_74", 0x00000027,
    (u32)"BTLNO_JON_75", 0x00000028, (u32)"BTLNO_JON_81", 0x00000029,
    (u32)"BTLNO_JON_82", 0x0000002A, (u32)"BTLNO_JON_83", 0x0000002B,
    (u32)"BTLNO_JON_84", 0x0000002C, (u32)"BTLNO_JON_85", 0x0000002D,
    (u32)"BTLNO_JON_91", 0x0000002E, (u32)"BTLNO_JON_92", 0x0000002F,
    (u32)"BTLNO_JON_93", 0x00000030, (u32)"BTLNO_JON_94", 0x00000031,
    (u32)"BTLNO_JON_95", 0x00000032, (u32)"BTLNO_JON_100_ZONBABA", 0x00000033,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
