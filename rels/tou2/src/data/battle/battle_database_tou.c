#include "battle_database_tou.h"
#include "battle/battle_stage.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_heart_drop_param_default[];
extern s32 battle_item_basabasa[];
extern s32 battle_item_bllizard[];
extern s32 battle_item_bomhei[];
extern s32 battle_item_boomerang_bros[];
extern s32 battle_item_borodo[];
extern s32 battle_item_borodo_king[];
extern s32 battle_item_burst_wanwan[];
extern s32 battle_item_champion[];
extern s32 battle_item_dokugassun[];
extern s32 battle_item_fire_bros[];
extern s32 battle_item_hammer_bros[];
extern s32 battle_item_hyper_sinnosuke[];
extern s32 battle_item_hyper_togemet[];
extern s32 battle_item_iron_sinemon[];
extern s32 battle_item_iron_sinemon2[];
extern s32 battle_item_kamec[];
extern s32 battle_item_kurikuri[];
extern s32 battle_item_kurokumorn[];
extern s32 battle_item_macho_gance[];
extern s32 battle_item_monban[];
extern s32 battle_item_nokonoko_fighter[];
extern s32 battle_item_patapata_fighter[];
extern s32 battle_item_sambo[];
extern s32 battle_item_togezo[];
extern s32 battle_item_ura_noko[];
extern s32 battle_item_ura_pata[];
extern s32 tou_gamen_screen_tev_init[];
extern s32 unit_basabasa_28_data_16B60[];
extern s32 unit_bomhei_28_data_17C48[];
extern s32 unit_boomerang_bros[];
extern s32 unit_borodo_28_data_1AEF0[];
extern s32 unit_borodo_king[];
extern s32 unit_boss_champion[];
extern s32 unit_boss_koopa_tou[];
extern s32 unit_boss_macho_gance[];
extern s32 unit_burst_wanwan_28_data_4AED8[];
extern s32 unit_chorobon_28_data_1C808[];
extern s32 unit_crimson_togemet[];
extern s32 unit_dark_keeper_28_data_1DE40[];
extern s32 unit_fire_bros[];
extern s32 unit_flower_chorobon_28_data_21138[];
extern s32 unit_green_chorobon_28_data_22E58[];
extern s32 unit_hammer_bros_28_data_24A10[];
extern s32 unit_hannya_28_data_3BF68[];
extern s32 unit_hinnya_28_data_27050[];
extern s32 unit_honenoko_28_data_27C08[];
extern s32 unit_hyper_sinnosuke[];
extern s32 unit_iron_sinemon[];
extern s32 unit_iron_sinemon2[];
extern s32 unit_jyugem_28_data_2B678[];
extern s32 unit_kamec_green[];
extern s32 unit_kamec_red[];
extern s32 unit_kamec_white[];
extern s32 unit_kurikuri[];
extern s32 unit_monochrome_kurokumorn_28_data_59A08[];
extern s32 unit_monochrome_pakkun_28_data_5B1C8[];
extern s32 unit_nokonoko_fighter[];
extern s32 unit_patapata_fighter[];
extern s32 unit_piders_28_data_2E140[];
extern s32 unit_sambo_28_data_30280[];
extern s32 unit_togedaruma_28_data_34998[];
extern s32 unit_togenoko_ace_28_data_48988[];
extern s32 unit_togezo_28_data_35BD0[];
extern s32 unit_ura_noko_28_data_37580[];
extern s32 unit_ura_pata[];
s32 bgset_tou_screen_evt[22];

// clang-format off
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
u32 tou_btlobject_data[84] = {
    (u32)"gamen", 0x00020000, 0x00000000, 0x00000000,
    0xC2480000, 0x003C0000, (u32)"etc", 0x00020000,
    0x00000000, 0x00000000, 0xC2580000, 0x0A320000,
    (u32)"side__L", 0x00020000, 0xC3090000, 0x00000000,
    0xC2200000, 0x14280000, (u32)"side__R", 0x00020000,
    0x43090000, 0x00000000, 0xC2200000, 0x1E1E0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"gamen", 0x00020000, 0x00000000, 0x00000000,
    0xC2480000, 0x003C0000, (u32)"etc", 0x00020000,
    0x00000000, 0x00000000, 0xC2580000, 0x0A320000,
    (u32)"side__L", 0x00020000, 0xC3090000, 0x00000000,
    0xC2200000, 0x14280000, (u32)"side__R", 0x00020000,
    0x43090000, 0x00000000, 0xC2200000, 0x1E1E0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"if_wire", 0x00050006, 0x00000000, 0x43020000,
    0x00000000, 0x00000000, (u32)"if_body", 0x00050006,
    0x00000000, 0x43020000, 0x00000000, 0x00000000,
};

// TODO: type
u32 tou_btlstage_data[218] = {
    (u32)"bti_01", (u32)"stg_03_0", 0x00000006, (u32)&tou_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, (u32)&bgset_tou_screen_evt,
    0x00010101, (u32)"bti_01", (u32)"stg_03_0", 0x00000008,
    (u32)((u8*)&tou_btlobject_data + 0x90), 0x00000000, 0x00000000, 0x00000000,
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
    (u32)&bgset_tou_screen_evt, 0x00010101,
};

// TODO: type
u32 btlparty_tou_tou_rank_19[60] = {
    (u32)unit_kurikuri, 0x01000000, 0x04000004, 0x41200000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
    (u32)unit_kurikuri, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
    (u32)unit_kurikuri, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
    (u32)unit_kurikuri, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
    (u32)unit_kurikuri, 0x01000000, 0x04000004, 0x432A0000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurikuri,
};

// TODO: type
u32 btlgrp_tou_tou_rank_19[8] = {
    0x00000005, (u32)&btlparty_tou_tou_rank_19, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_18[36] = {
    (u32)unit_nokonoko_fighter, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_nokonoko_fighter,
    (u32)unit_nokonoko_fighter, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_nokonoko_fighter,
    (u32)unit_patapata_fighter, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_patapata_fighter,
};

// TODO: type
u32 btlgrp_tou_tou_rank_18[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_18, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_17[36] = {
    (u32)unit_sambo_28_data_30280, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_28_data_30280, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
    (u32)unit_sambo_28_data_30280, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_sambo,
};

// TODO: type
u32 btlgrp_tou_tou_rank_17[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_17, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_16[36] = {
    (u32)unit_honenoko_28_data_27C08, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_honenoko_28_data_27C08, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_honenoko_28_data_27C08, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
};

// TODO: type
u32 btlgrp_tou_tou_rank_16[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_16, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_15[36] = {
    (u32)unit_togezo_28_data_35BD0, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kurokumorn,
    (u32)unit_togezo_28_data_35BD0, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_dokugassun,
    (u32)unit_jyugem_28_data_2B678, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bllizard,
};

// TODO: type
u32 btlgrp_tou_tou_rank_15[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_15, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_14[36] = {
    (u32)unit_monochrome_pakkun_28_data_5B1C8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togezo,
    (u32)unit_monochrome_kurokumorn_28_data_59A08, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togezo,
    (u32)unit_piders_28_data_2E140, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_togezo,
};

// TODO: type
u32 btlgrp_tou_tou_rank_14[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_14, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_13[36] = {
    (u32)unit_hyper_sinnosuke, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinnosuke,
    (u32)unit_hyper_sinnosuke, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinnosuke,
    (u32)unit_hyper_sinnosuke, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_sinnosuke,
};

// TODO: type
u32 btlgrp_tou_tou_rank_13[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_13, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_12[48] = {
    (u32)unit_bomhei_28_data_17C48, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_28_data_17C48, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_28_data_17C48, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_bomhei_28_data_17C48, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
};

// TODO: type
u32 btlgrp_tou_tou_rank_12[8] = {
    0x00000004, (u32)&btlparty_tou_tou_rank_12, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_11[48] = {
    (u32)unit_borodo_28_data_1AEF0, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_28_data_1AEF0, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo,
    (u32)unit_borodo_king, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo_king,
    (u32)unit_borodo_king, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_borodo_king,
};

// TODO: type
u32 btlgrp_tou_tou_rank_11[8] = {
    0x00000004, (u32)&btlparty_tou_tou_rank_11, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_10[24] = {
    (u32)unit_iron_sinemon, 0x01000000, 0x04000004, 0x42A00000,
    0x00000000, 0x00000000, 0x00000000, 0x00000032,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_iron_sinemon,
    (u32)unit_iron_sinemon2, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41200000, 0x00000000, 0x00000082,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_iron_sinemon2,
};

// TODO: type
u32 btlgrp_tou_tou_rank_10[8] = {
    0x00000002, (u32)&btlparty_tou_tou_rank_10, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_9[24] = {
    (u32)unit_crimson_togemet, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togemet,
    (u32)unit_crimson_togemet, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hyper_togemet,
};

// TODO: type
u32 btlgrp_tou_tou_rank_9[8] = {
    0x00000002, (u32)&btlparty_tou_tou_rank_9, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_8[24] = {
    (u32)unit_togedaruma_28_data_34998, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_togedaruma_28_data_34998, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tou_tou_rank_8[8] = {
    0x00000002, (u32)&btlparty_tou_tou_rank_8, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_7[36] = {
    (u32)unit_ura_noko_28_data_37580, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_pata, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_noko,
    (u32)unit_ura_noko_28_data_37580, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_ura_pata,
};

// TODO: type
u32 btlgrp_tou_tou_rank_7[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_7, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_6[36] = {
    (u32)unit_chorobon_28_data_1C808, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_green_chorobon_28_data_22E58, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
    (u32)unit_flower_chorobon_28_data_21138, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kamec,
};

// TODO: type
u32 btlgrp_tou_tou_rank_6[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_6, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_5[36] = {
    (u32)unit_kamec_red, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_hammer_bros,
    (u32)unit_kamec_white, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_fire_bros,
    (u32)unit_kamec_green, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_boomerang_bros,
};

// TODO: type
u32 btlgrp_tou_tou_rank_5[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_5, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_4[12] = {
    (u32)unit_dark_keeper_28_data_1DE40, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tou_tou_rank_4[8] = {
    0x00000001, (u32)&btlparty_tou_tou_rank_4, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_3[36] = {
    (u32)unit_hammer_bros_28_data_24A10, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_fire_bros, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
    (u32)unit_boomerang_bros, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_bomhei,
};

// TODO: type
u32 btlgrp_tou_tou_rank_3[8] = {
    0x00000003, (u32)&btlparty_tou_tou_rank_3, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_2[24] = {
    (u32)unit_burst_wanwan_28_data_4AED8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
    (u32)unit_burst_wanwan_28_data_4AED8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_burst_wanwan,
};

// TODO: type
u32 btlgrp_tou_tou_rank_2[8] = {
    0x00000002, (u32)&btlparty_tou_tou_rank_2, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_rank_1[12] = {
    (u32)unit_togenoko_ace_28_data_48988, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tou_tou_rank_1[8] = {
    0x00000001, (u32)&btlparty_tou_tou_rank_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_add_1[48] = {
    (u32)unit_basabasa_28_data_16B60, 0x01000000, 0x04000004, 0x41F00000,
    0x43020000, 0xC1200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_28_data_16B60, 0x01000000, 0x04000004, 0x428C0000,
    0x43020000, 0x00000000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_28_data_16B60, 0x01000000, 0x04000004, 0x42DC0000,
    0x43020000, 0x41200000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
    (u32)unit_basabasa_28_data_16B60, 0x01000000, 0x04000004, 0x43160000,
    0x43020000, 0x41A00000, 0x00000000, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_basabasa,
};

// TODO: type
u32 btlgrp_tou_tou_add_1[8] = {
    0x00000004, (u32)&btlparty_tou_tou_add_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_add_2[48] = {
    (u32)unit_hinnya_28_data_27050, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monban,
    (u32)unit_hannya_28_data_3BF68, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monban,
    (u32)unit_hinnya_28_data_27050, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_monban,
    (u32)unit_hannya_28_data_3BF68, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tou_tou_add_2[8] = {
    0x00000004, (u32)&btlparty_tou_tou_add_2, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_boss[12] = {
    (u32)unit_boss_macho_gance, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_macho_gance,
};

// TODO: type
u32 btlgrp_tou_tou_boss[8] = {
    0x00000001, (u32)&btlparty_tou_tou_boss, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_champ[12] = {
    (u32)unit_boss_champion, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_champion,
};

// TODO: type
u32 btlgrp_tou_tou_champ[8] = {
    0x00000001, (u32)&btlparty_tou_tou_champ, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_tou_tou_koopa[12] = {
    (u32)unit_boss_koopa_tou, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_tou_tou_koopa[8] = {
    0x00000001, (u32)&btlparty_tou_tou_koopa, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_test1_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_19, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_01_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_19, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_02_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_18, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_03_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_17, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_04_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_16, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_05_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_15, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_06_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_14, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_07_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_13, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_08_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_12, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_09_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_11, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_10_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_10, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_11_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_9, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_12_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_8, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_13_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_7, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_14_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_6, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_15_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_5, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_16_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_4, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_17_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_3, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_18_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_2, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_19_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_rank_1, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_20_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_add_1, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_21_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_add_2, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_22_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_champ, (u32)((u8*)&tou_btlstage_data + 0x1b4), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_23_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_boss, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_tou_btlno_tou_01_24_off[6] = {
    0x00000032, (u32)&btlgrp_tou_tou_koopa, (u32)&tou_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_tou_tbl[442] = {
    (u32)"マップstg_03_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"クリクリブラザーズ", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_01_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x0A0A0404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"コウラーズ", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_02_off,
    0x00000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x0A0A0000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"サンボ三兄弟",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_03_off, 0x00000040, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ボンバーズ", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_04_off, 0x00000040, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"クモーンズ", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_05_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ジュゲムとイタい仲間たち", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_06_off,
    0x00000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ハイパーズ",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_07_off, 0x00000040, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ボロドーズ", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_08_off, 0x00000040, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"クッパぐんだんズ", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_09_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"こうてつウォリアーズ", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_10_off,
    0x20000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_CHUBOSS_BATTLE1", (u32)"ザ・トゲトゲン",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_11_off, 0x00000040, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ヤミトゲダルマー", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_12_off, 0x00000040, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"ザ・ウラウラン", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_13_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x0A0A0000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"レッドデビルズ", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_14_off,
    0x00000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"トリプルブロス",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_15_off, 0x00000040, 0x00000000, 0x04040404,
    0x00000A0A, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ザ・マホーンズ", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_16_off, 0x00000040, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"キラーボムヘイズ", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_17_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"バーストワンワン", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_18_off,
    0x00000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"トゲノコエース",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_19_off, 0x00000040, 0x00000000, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x0A0A0000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"ナイトウィングス", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_20_off, 0x00000040, 0x00000000,
    0x04040404, 0x00000000, 0x04040404, 0x00000404,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"スネイルズ", (u32)"tou_03", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_21_off, 0x00000040,
    0x00000000, 0x04040404, 0x00000000, 0x04040404,
    0x00000404, 0x04040000, 0x04040404, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"ブレッドハート", (u32)"tou_03",
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_22_off,
    0x20000040, 0x00000000, 0x04040404, 0x00000000,
    0x04040404, 0x00000404, 0x04040000, 0x04040404,
    0x01010101, 0x01010101, (u32)"BGM_BOSS_STG3_CHAMP1", (u32)"マッチョガンス",
    (u32)"tou_03", 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_tou_btlno_tou_01_23_off, 0x20000010, 0x00000006, 0x04040404,
    0x00000000, 0x04040404, 0x00000404, 0x04040000,
    0x04040404, 0x01010101, 0x01010101, (u32)"BGM_BOSS_STG3_GANCE1",
    (u32)"乱入クッパ", (u32)"tou_03", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_tou_btlno_tou_01_24_off, 0x20000000, 0x00000000,
    0x00000000, 0x00000000, 0x04040404, 0x00000000,
    0x04040000, 0x04040404, 0x01010101, 0x01010101,
    (u32)"BGM_BOSS_STG8_KOOPA", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btl_tou_setup_no_tbl[52] = {
    (u32)"BTLNO_TOU_TEST1", 0x00000000, (u32)"BTLNO_TOU_01_01", 0x00000001,
    (u32)"BTLNO_TOU_01_02", 0x00000002, (u32)"BTLNO_TOU_01_03", 0x00000003,
    (u32)"BTLNO_TOU_01_04", 0x00000004, (u32)"BTLNO_TOU_01_05", 0x00000005,
    (u32)"BTLNO_TOU_01_06", 0x00000006, (u32)"BTLNO_TOU_01_07", 0x00000007,
    (u32)"BTLNO_TOU_01_08", 0x00000008, (u32)"BTLNO_TOU_01_09", 0x00000009,
    (u32)"BTLNO_TOU_01_10", 0x0000000A, (u32)"BTLNO_TOU_01_11", 0x0000000B,
    (u32)"BTLNO_TOU_01_12", 0x0000000C, (u32)"BTLNO_TOU_01_13", 0x0000000D,
    (u32)"BTLNO_TOU_01_14", 0x0000000E, (u32)"BTLNO_TOU_01_15", 0x0000000F,
    (u32)"BTLNO_TOU_01_16", 0x00000010, (u32)"BTLNO_TOU_01_17", 0x00000011,
    (u32)"BTLNO_TOU_01_18", 0x00000012, (u32)"BTLNO_TOU_01_19", 0x00000013,
    (u32)"BTLNO_TOU_01_20", 0x00000014, (u32)"BTLNO_TOU_01_21", 0x00000015,
    (u32)"BTLNO_TOU_01_22", 0x00000016, (u32)"BTLNO_TOU_01_23", 0x00000017,
    (u32)"BTLNO_TOU_01_24", 0x00000018, 0x00000000, 0x00000000,
};

EVT_BEGIN(bgset_tou_screen_evt)
    CALL(tou_gamen_screen_tev_init)
    WAIT_FRAMES(1)
    CALL(evt_map_set_tevcallback, 8, PTR(tou_gamen_screen_tev))
    CALL(evt_mapobj_flag_onoff, 1, 1, STRING("gamen"), 134217728)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("gamen"), 256)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
