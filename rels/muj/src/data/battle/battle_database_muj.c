#include "battle_database_muj.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_sub.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

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
extern ItemDropData battle_item_cortez[1];
extern ItemDropData battle_item_flower_chorobon[9];
extern ItemDropData battle_item_green_chorobon[9];
extern ItemDropData battle_item_gundan_zako[10];
extern ItemDropData battle_item_kanbu1[1];
extern ItemDropData battle_item_poison_pakkun[5];
extern BattleUnitKind unit_boss_cortez;
extern BattleUnitKind unit_boss_gundan_zako_group1;
extern BattleUnitKind unit_boss_kanbu3;
extern BattleUnitKind unit_flower_chorobon_20_data_347E8;
extern BattleUnitKind unit_green_chorobon_20_data_36508;
extern BattleUnitKind unit_hermos_20_data_39998;
extern BattleUnitKind unit_kuriboo_20_data_2C5C8;
extern BattleUnitKind unit_poison_pakkun;
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

BattleStageObjectData muj_btlobject_data[31] = {
    { "A1", 2, 0, { 0.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "hata", 2, 1, { 178.0f, 0.0f, -49.0f }, 0, 60, { 0, 0 } },
    { "iwa", 2, 1, { -183.0f, 0.0f, -28.0f }, 10, 50, { 0, 0 } },
    { "zai", 2, 1, { 126.0f, 0.0f, -26.0f }, 20, 40, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A1", 2, 0, { 0.0f, 0.0f, -56.0f }, 0, 60, { 0, 0 } },
    { "kusa_mae", 2, 1, { 0.0f, 0.0f, 70.0f }, 0, 60, { 0, 0 } },
    { "kusa_usiro", 2, 1, { 0.0f, 0.0f, -44.0f }, 10, 50, { 0, 0 } },
    { "kusa_01", 2, 1, { -79.0f, 0.0f, -28.0f }, 20, 40, { 0, 0 } },
    { "kusa_02", 2, 1, { 128.0f, 0.0f, -33.0f }, 30, 30, { 0, 0 } },
    { "necco", 2, 1, { 0.0f, 0.0f, -34.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "takara_01", 2, 1, { 111.0f, 0.0f, -69.0f }, 0, 60, { 0, 0 } },
    { "takara_02", 2, 1, { -87.0f, 0.0f, -72.0f }, 10, 50, { 0, 0 } },
    { "takara_03", 2, 1, { 136.0f, 0.0f, 57.0f }, 20, 40, { 0, 0 } },
    { "takara_04", 2, 1, { -182.0f, 0.0f, 57.0f }, 30, 30, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C_kabe", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_00", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "nami_01", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A1", 2, 0, { 107.0f, 0.0f, -53.0f }, 0, 60, { 0, 0 } },
    { "S_mae_l", 2, 1, { -90.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_mae_r", 2, 1, { 84.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_oku_l", 2, 1, { -35.0f, 0.0f, -42.0f }, 0, 60, { 0, 0 } },
    { "S_oku_r", 2, 1, { 35.0f, 0.0f, -42.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "if_wire", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
    { "if_body", 5, 6, { 0.0f, 130.0f, 0.0f }, 0, 0, { 0, 0 } },
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

BattleUnitSetup btlparty_muj_muj_cortez[1] = {
    { &unit_boss_cortez, 1, 0x04000004, { 146.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_cortez },
};

BattleGroupSetup btlgrp_muj_muj_cortez = { 1, btlparty_muj_muj_cortez, 0, 0, 100, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_kanbu[2] = {
    { &unit_boss_gundan_zako_group1, 1, 0x04000004, { 80.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_boss_kanbu3, 1, 0x04000004, { 160.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kanbu1 },
};

BattleGroupSetup btlgrp_muj_muj_kanbu = { 2, btlparty_muj_muj_kanbu, 0, 0, 100, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_01_01[3] = {
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_muj_muj_01_01 = { 3, btlparty_muj_muj_01_01, 0, 0, 100, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_muj_muj_05_01[3] = {
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_muj_muj_05_01 = { 3, btlparty_muj_muj_05_01, 0, 0, 100, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_muj_muj_05_02[3] = {
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_hermos_20_data_39998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_muj_muj_05_02 = { 3, btlparty_muj_muj_05_02, 0, 0, 100, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_muj_muj_00_01[3] = {
    { &unit_kuriboo_20_data_2C5C8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_kuriboo_20_data_2C5C8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, NULL },
    { &unit_kuriboo_20_data_2C5C8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, NULL },
};

BattleGroupSetup btlgrp_muj_muj_00_01 = { 3, btlparty_muj_muj_00_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_01_off_01[1] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_01_off_01 = { 1, btlparty_muj_muj_02_01_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_01_off_02[2] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_01_off_02 = { 2, btlparty_muj_muj_02_01_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_01_off_03[3] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_01_off_03 = { 3, btlparty_muj_muj_02_01_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_01_off_04[4] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_01_off_04 = { 4, btlparty_muj_muj_02_01_off_04, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_04_off_01[1] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_04_off_01 = { 1, btlparty_muj_muj_02_04_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_04_off_02[2] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_04_off_02 = { 2, btlparty_muj_muj_02_04_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_04_off_03[3] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_04_off_03 = { 3, btlparty_muj_muj_02_04_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_07_off_01[2] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_07_off_01 = { 2, btlparty_muj_muj_02_07_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_07_off_02[3] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_07_off_02 = { 3, btlparty_muj_muj_02_07_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_02_07_off_03[4] = {
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_02_07_off_03 = { 4, btlparty_muj_muj_02_07_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_03_01_off_01[2] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
};

BattleGroupSetup btlgrp_muj_muj_03_01_off_01 = { 2, btlparty_muj_muj_03_01_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_01_off_02[3] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_03_01_off_02 = { 3, btlparty_muj_muj_03_01_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_01_off_03[3] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
};

BattleGroupSetup btlgrp_muj_muj_03_01_off_03 = { 3, btlparty_muj_muj_03_01_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_02_off_01[2] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
};

BattleGroupSetup btlgrp_muj_muj_03_02_off_01 = { 2, btlparty_muj_muj_03_02_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_02_off_02[3] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_03_02_off_02 = { 3, btlparty_muj_muj_03_02_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_02_off_03[3] = {
    { &unit_poison_pakkun, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_poison_pakkun, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
};

BattleGroupSetup btlgrp_muj_muj_03_02_off_03 = { 3, btlparty_muj_muj_03_02_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_muj_muj_03_04_off_01[2] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_poison_pakkun, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
};

BattleGroupSetup btlgrp_muj_muj_03_04_off_01 = { 2, btlparty_muj_muj_03_04_off_01, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_03_04_off_02[3] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_green_chorobon_20_data_36508, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_green_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_03_04_off_02 = { 3, btlparty_muj_muj_03_04_off_02, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_muj_muj_03_04_off_03[3] = {
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_poison_pakkun, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_poison_pakkun },
    { &unit_flower_chorobon_20_data_347E8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_muj_muj_03_04_off_03 = { 3, btlparty_muj_muj_03_04_off_03, 25, 25, 50, (u32*)battle_heart_drop_param_default, (u32*)battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_test1_off[2] = {
    { 50, &btlgrp_muj_muj_01_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_test2_off[2] = {
    { 50, &btlgrp_muj_muj_01_01, (struct BattleStageData*)muj_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_test3_off[2] = {
    { 50, &btlgrp_muj_muj_01_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_test4_off[2] = {
    { 50, &btlgrp_muj_muj_01_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_cortez_off[2] = {
    { 50, &btlgrp_muj_muj_cortez, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_kanbu_off[2] = {
    { 50, &btlgrp_muj_muj_kanbu, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_01_01_off[2] = {
    { 50, &btlgrp_muj_muj_01_01, (struct BattleStageData*)muj_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_05_01_off[2] = {
    { 50, &btlgrp_muj_muj_05_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_05_02_off[2] = {
    { 50, &btlgrp_muj_muj_05_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_02_01_off[4] = {
    { 10, &btlgrp_muj_muj_02_01_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 20, &btlgrp_muj_muj_02_01_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_02_01_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_02_02_off[4] = {
    { 10, &btlgrp_muj_muj_02_04_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 20, &btlgrp_muj_muj_02_04_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_02_04_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_02_07_off[4] = {
    { 10, &btlgrp_muj_muj_02_07_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 20, &btlgrp_muj_muj_02_07_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_02_07_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_03_01_off[4] = {
    { 10, &btlgrp_muj_muj_03_01_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 20, &btlgrp_muj_muj_03_01_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_03_01_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_03_02_off[4] = {
    { 10, &btlgrp_muj_muj_03_02_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 20, &btlgrp_muj_muj_03_02_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_03_02_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_muj_btlno_muj_03_04_off[4] = {
    { 10, &btlgrp_muj_muj_03_04_off_01, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_03_04_off_02, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 10, &btlgrp_muj_muj_03_04_off_03, (struct BattleStageData*)(void*)((u8*)muj_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_muj_tbl[16] = {
    {
        "マップstg_05_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_05_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_05_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_05_5",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "コルテス",                                                                                             // name
        "muj_12",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_cortez_off,                                                                 // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG5_KORTESS1",                                                                           // music
    },
    {
        "幹部その３",                                                                                            // name
        "muj_00",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_kanbu_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_3RDLEADER1",                                                                         // music
    },
    {
        "MUJ01おんりょう１",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_01_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "MUJ01おんりょう２",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_05_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "MUJ01おんりょう３",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_05_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "MUJ02グリーンチョロボン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_02_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "MUJ02フラワーチョロボン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_02_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "MUJ02グリーンチョロボン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_02_07_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "MUJ03ポイズンパックン",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_muj_btlno_muj_03_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "MUJ03ポイズンパックン",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_muj_btlno_muj_03_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "MUJ03フラワーチョロボン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_muj_btlno_muj_03_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        NULL,                                                                                               // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        0,                                                                                                  // flag
        NULL,                                                                                               // onLoadouts
        NULL,                                                                                               // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_0x20
        NULL,                                                                                               // music
    },
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
