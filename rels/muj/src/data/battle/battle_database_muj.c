#include "battle_database_muj.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

// This REL's cflags omit `-fp_contract on`, but the original was built with it (the _wave_offset
// y term is a fused `fmadds`); without it MWCC emits separate `fmuls`/`fadds`. The proper fix is a
// `-fp_contract on` extra_cflag for data/battle/battle_database_muj.c.
#pragma fp_contract on

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_item_cortez[];
extern s32 battle_item_flower_chorobon[];
extern s32 battle_item_green_chorobon[];
extern s32 battle_item_gundan_zako[];
extern s32 battle_item_kanbu1[];
extern s32 battle_item_poison_pakkun[];
extern s32 unit_boss_cortez[];
extern s32 unit_boss_gundan_zako_group1[];
extern s32 unit_boss_kanbu3[];
extern s32 unit_flower_chorobon_20_data_347E8[];
extern s32 unit_green_chorobon_20_data_36508[];
extern s32 unit_hermos_20_data_39998[];
extern s32 unit_kuriboo_20_data_2C5C8[];
extern s32 unit_poison_pakkun[];
static USER_FUNC(_wave_offset);
s32 bgset_muj_sun_event[18];

// clang-format off
extend_data(44,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(45,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 muj_btlobject_data[186] = {
    (u32)"A1", 0x00020000, 0x00000000, 0x00000000,
    0xC2480000, 0x003C0000, (u32)"hata", 0x00020001,
    0x43320000, 0x00000000, 0xC2440000, 0x003C0000,
    (u32)"iwa", 0x00020001, 0xC3370000, 0x00000000,
    0xC1E00000, 0x0A320000, (u32)"zai", 0x00020001,
    0x42FC0000, 0x00000000, 0xC1D00000, 0x14280000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"A1", 0x00020000, 0x00000000, 0x00000000,
    0xC2600000, 0x003C0000, (u32)"kusa_mae", 0x00020001,
    0x00000000, 0x00000000, 0x428C0000, 0x003C0000,
    (u32)"kusa_usiro", 0x00020001, 0x00000000, 0x00000000,
    0xC2300000, 0x0A320000, (u32)"kusa_01", 0x00020001,
    0xC29E0000, 0x00000000, 0xC1E00000, 0x14280000,
    (u32)"kusa_02", 0x00020001, 0x43000000, 0x00000000,
    0xC2040000, 0x1E1E0000, (u32)"necco", 0x00020001,
    0x00000000, 0x00000000, 0xC2080000, 0x003C0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"takara_01", 0x00020001, 0x42DE0000, 0x00000000,
    0xC28A0000, 0x003C0000, (u32)"takara_02", 0x00020001,
    0xC2AE0000, 0x00000000, 0xC2900000, 0x0A320000,
    (u32)"takara_03", 0x00020001, 0x43080000, 0x00000000,
    0x42640000, 0x14280000, (u32)"takara_04", 0x00020001,
    0xC3360000, 0x00000000, 0x42640000, 0x1E1E0000,
    (u32)"B", 0x00020002, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"C_kabe", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"nami_00", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"nami_01", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"A1", 0x00020000, 0x42D60000, 0x00000000,
    0xC2540000, 0x003C0000, (u32)"S_mae_l", 0x00020001,
    0xC2B40000, 0x00000000, 0x42400000, 0x003C0000,
    (u32)"S_mae_r", 0x00020001, 0x42A80000, 0x00000000,
    0x42400000, 0x003C0000, (u32)"S_oku_l", 0x00020001,
    0xC20C0000, 0x00000000, 0xC2280000, 0x003C0000,
    (u32)"S_oku_r", 0x00020001, 0x420C0000, 0x00000000,
    0xC2280000, 0x003C0000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"if_wire", 0x00050006,
    0x00000000, 0x43020000, 0x00000000, 0x00000000,
    (u32)"if_body", 0x00050006, 0x00000000, 0x43020000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 muj_btlstage_data[436] = {
    (u32)"bti_01", (u32)"stg_05_1", 0x00000006, (u32)&muj_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, (u32)&bgset_muj_sun_event,
    0x00000000, (u32)"bti_01", (u32)"stg_05_2", 0x00000008,
    (u32)((u8*)&muj_btlobject_data + 0x90), 0x00000000, 0x00000000, 0x00000000,
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
    (u32)&bgset_muj_sun_event, 0x00000000, (u32)"bti_01", (u32)"stg_05_5",
    0x00000008, (u32)((u8*)&muj_btlobject_data + 0x150), 0x00000000, 0x00000000,
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
    (u32)"stg_05_0", 0x00000009, (u32)((u8*)&muj_btlobject_data + 0x210), 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (u32)weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0210006F, 0x20000000, 0x001A0302, 0x00000000,
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
    0x00000000, 0x00000000, (u32)&bgset_muj_sun_event, 0x00000000,
};

// TODO: type
u32 btlparty_muj_muj_cortez[12] = {
    (u32)unit_boss_cortez, 0x01000000, 0x04000004, 0x43120000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_cortez,
};

// TODO: type
u32 btlgrp_muj_muj_cortez[8] = {
    0x00000001, (u32)&btlparty_muj_muj_cortez, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_kanbu[24] = {
    (u32)unit_boss_gundan_zako_group1, 0x01000000, 0x04000004, 0x42A00000,
    0x00000000, 0xC1A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_gundan_zako,
    (u32)unit_boss_kanbu3, 0x01000000, 0x04000004, 0x43200000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_kanbu1,
};

// TODO: type
u32 btlgrp_muj_muj_kanbu[8] = {
    0x00000002, (u32)&btlparty_muj_muj_kanbu, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_01_01[36] = {
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_muj_muj_01_01[8] = {
    0x00000003, (u32)&btlparty_muj_muj_01_01, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_05_01[36] = {
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_muj_muj_05_01[8] = {
    0x00000003, (u32)&btlparty_muj_muj_05_01, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_05_02[36] = {
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42480000,
    0x42200000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x42B40000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_hermos_20_data_39998, 0x01000000, 0x04000004, 0x43020000,
    0x42200000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_muj_muj_05_02[8] = {
    0x00000003, (u32)&btlparty_muj_muj_05_02, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_00_01[36] = {
    (u32)unit_kuriboo_20_data_2C5C8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_kuriboo_20_data_2C5C8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)unit_kuriboo_20_data_2C5C8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrp_muj_muj_00_01[8] = {
    0x00000003, (u32)&btlparty_muj_muj_00_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_01_off_01[12] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_01_off_01[8] = {
    0x00000001, (u32)&btlparty_muj_muj_02_01_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_01_off_02[24] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_01_off_02[8] = {
    0x00000002, (u32)&btlparty_muj_muj_02_01_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_01_off_03[36] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_01_off_03[8] = {
    0x00000003, (u32)&btlparty_muj_muj_02_01_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_01_off_04[48] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_01_off_04[8] = {
    0x00000004, (u32)&btlparty_muj_muj_02_01_off_04, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_04_off_01[12] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_04_off_01[8] = {
    0x00000001, (u32)&btlparty_muj_muj_02_04_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_04_off_02[24] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_04_off_02[8] = {
    0x00000002, (u32)&btlparty_muj_muj_02_04_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_04_off_03[36] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_04_off_03[8] = {
    0x00000003, (u32)&btlparty_muj_muj_02_04_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_07_off_01[24] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_07_off_01[8] = {
    0x00000002, (u32)&btlparty_muj_muj_02_07_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_07_off_02[36] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_07_off_02[8] = {
    0x00000003, (u32)&btlparty_muj_muj_02_07_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_02_07_off_03[48] = {
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_02_07_off_03[8] = {
    0x00000004, (u32)&btlparty_muj_muj_02_07_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_01_off_01[24] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
};

// TODO: type
u32 btlgrp_muj_muj_03_01_off_01[8] = {
    0x00000002, (u32)&btlparty_muj_muj_03_01_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_01_off_02[36] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_03_01_off_02[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_01_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_01_off_03[36] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
};

// TODO: type
u32 btlgrp_muj_muj_03_01_off_03[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_01_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_02_off_01[24] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
};

// TODO: type
u32 btlgrp_muj_muj_03_02_off_01[8] = {
    0x00000002, (u32)&btlparty_muj_muj_03_02_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_02_off_02[36] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_03_02_off_02[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_02_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_02_off_03[36] = {
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
};

// TODO: type
u32 btlgrp_muj_muj_03_02_off_03[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_02_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default3, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_04_off_01[24] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
};

// TODO: type
u32 btlgrp_muj_muj_03_04_off_01[8] = {
    0x00000002, (u32)&btlparty_muj_muj_03_04_off_01, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_04_off_02[36] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_green_chorobon_20_data_36508, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_green_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_03_04_off_02[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_04_off_02, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlparty_muj_muj_03_04_off_03[36] = {
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
    (u32)unit_poison_pakkun, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_poison_pakkun,
    (u32)unit_flower_chorobon_20_data_347E8, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_flower_chorobon,
};

// TODO: type
u32 btlgrp_muj_muj_03_04_off_03[8] = {
    0x00000003, (u32)&btlparty_muj_muj_03_04_off_03, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_test1_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_01_01, (u32)((u8*)&muj_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_test2_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_01_01, (u32)&muj_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_test3_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_01_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_test4_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_01_01, (u32)((u8*)&muj_btlstage_data + 0x368), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_cortez_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_cortez, (u32)((u8*)&muj_btlstage_data + 0x368), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_kanbu_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_kanbu, (u32)((u8*)&muj_btlstage_data + 0x51c), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_01_01_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_01_01, (u32)&muj_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_05_01_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_05_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_05_02_off[6] = {
    0x00000032, (u32)&btlgrp_muj_muj_05_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_02_01_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_02_01_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000014,
    (u32)&btlgrp_muj_muj_02_01_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_02_01_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_02_02_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_02_04_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000014,
    (u32)&btlgrp_muj_muj_02_04_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_02_04_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_02_07_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_02_07_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000014,
    (u32)&btlgrp_muj_muj_02_07_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_02_07_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_03_01_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_03_01_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000014,
    (u32)&btlgrp_muj_muj_03_01_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_03_01_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_03_02_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_03_02_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000014,
    (u32)&btlgrp_muj_muj_03_02_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_03_02_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_muj_btlno_muj_03_04_off[12] = {
    0x0000000A, (u32)&btlgrp_muj_muj_03_04_off_01, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A,
    (u32)&btlgrp_muj_muj_03_04_off_02, (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x0000000A, (u32)&btlgrp_muj_muj_03_04_off_03,
    (u32)((u8*)&muj_btlstage_data + 0x1b4), 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_muj_tbl[272] = {
    (u32)"マップstg_05_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_muj_btlno_muj_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_05_1", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_test2_off, 0x00000000,
    0x00000000, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_05_2", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_test3_off,
    0x00000000, 0x00000000, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"マップstg_05_5",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_muj_btlno_muj_test4_off, 0x00000000, 0x00000000, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"コルテス", (u32)"muj_12", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_muj_btlno_muj_cortez_off, 0x20000010, 0x00000006,
    0x03030303, 0x03030000, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_BOSS_STG5_KORTESS1", (u32)"幹部その３", (u32)"muj_00", 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_kanbu_off, 0x20000010,
    0x00000000, 0x03030303, 0x03030000, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_BOSS_STG3_3RDLEADER1", (u32)"MUJ01おんりょう１", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_01_01_off,
    0x20000010, 0x00000000, 0x03030303, 0x03030000,
    0x03030303, 0x03030303, 0x03030000, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_CHUBOSS_BATTLE1", (u32)"MUJ01おんりょう２",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_muj_btlno_muj_05_01_off, 0x20000010, 0x00000000, 0x03030303,
    0x03030000, 0x03030303, 0x03030303, 0x03030000,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_CHUBOSS_BATTLE1",
    (u32)"MUJ01おんりょう３", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_muj_btlno_muj_05_02_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030000, 0x03030303, 0x03030303,
    0x03030000, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_CHUBOSS_BATTLE1", (u32)"MUJ02グリーンチョロボン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_02_01_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030000, 0x03030303,
    0x03030303, 0x03030000, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"MUJ02フラワーチョロボン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_02_02_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030000,
    0x03030303, 0x03030303, 0x03030000, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"MUJ02グリーンチョロボン",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_muj_btlno_muj_02_07_off, 0x00000000, 0x00000000, 0x03030303,
    0x03030000, 0x03030303, 0x03030303, 0x03030000,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"MUJ03ポイズンパックン", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_muj_btlno_muj_03_01_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030000, 0x03030303, 0x03030303,
    0x03030000, 0x03030A0A, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"MUJ03ポイズンパックン", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_03_02_off, 0x00000000,
    0x00000000, 0x03030303, 0x03030000, 0x03030303,
    0x03030303, 0x03030000, 0x03030A0A, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"MUJ03フラワーチョロボン", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_muj_btlno_muj_03_04_off,
    0x00000000, 0x00000000, 0x03030303, 0x03030000,
    0x03030303, 0x03030303, 0x03030000, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btl_muj_setup_no_tbl[32] = {
    (u32)"BTLNO_MUJ_TEST1", 0x00000000, (u32)"BTLNO_MUJ_TEST2", 0x00000001,
    (u32)"BTLNO_MUJ_TEST3", 0x00000002, (u32)"BTLNO_MUJ_TEST4", 0x00000003,
    (u32)"BTLNO_MUJ_CORTEZ", 0x00000004, (u32)"BTLNO_MUJ_KANBU", 0x00000005,
    (u32)"BTLNO_MUJ_01_01", 0x00000006, (u32)"BTLNO_MUJ_05_01", 0x00000007,
    (u32)"BTLNO_MUJ_05_02", 0x00000008, (u32)"BTLNO_MUJ_02_01", 0x00000009,
    (u32)"BTLNO_MUJ_02_02", 0x0000000A, (u32)"BTLNO_MUJ_02_07", 0x0000000B,
    (u32)"BTLNO_MUJ_03_01", 0x0000000C, (u32)"BTLNO_MUJ_03_02", 0x0000000D,
    (u32)"BTLNO_MUJ_03_04", 0x0000000E, 0x00000000, 0x00000000,
};

USER_FUNC(_wave_offset) {
    s32* args = event->args;
    s32 name = evtGetValue(event, args[0]);
    s32 arg1 = args[1];
    s32 frame1 = evtGetValue(event, arg1);
    s32 period1 = evtGetValue(event, args[2]);
    s32 amp1 = evtGetValue(event, args[3]);
    s32 arg4 = args[4];
    s32 frame2 = evtGetValue(event, arg4);
    s32 period2 = evtGetValue(event, args[5]);
    s32 amp2 = evtGetValue(event, args[6]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)name);

    obj->field_0x10.x = amp1 * sinfd((f32)((frame1 * 360) / period1));
    obj->field_0x10.y = amp2 + amp2 * sinfd((f32)((frame2 * 360) / period2 + 270));
    frame1++;
    if (frame1 >= period1) {
        frame1 = 0;
    }
    evtSetValue(event, arg1, frame1);
    frame2++;
    if (frame2 >= period2) {
        frame2 = 0;
    }
    evtSetValue(event, arg4, frame2);
    return 2;
}

EVT_BEGIN(bgset_muj_wave_event)
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

EVT_BEGIN(bgset_muj_sun_event)
    CALL(evt_eff64, STRING("sunlight_bt"), STRING("sunlight_n64"), 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
