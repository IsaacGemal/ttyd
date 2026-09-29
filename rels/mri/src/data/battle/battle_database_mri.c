#include "battle_database_mri.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_barriern[1];
extern ItemDropData battle_item_gundan_zako[10];
extern ItemDropData battle_item_magnum_battender[1];
extern ItemDropData battle_item_monochrome__pakkun[5];
extern ItemDropData battle_item_piders[4];
extern BattleUnitKind unit_barriern_19_data_2B8A8;
extern BattleUnitKind unit_boss_magnum_battender;
extern BattleUnitKind unit_gundan_zako_19_data_2A2B8;
extern BattleUnitKind unit_monochrome_pakkun_19_data_389E0;
extern BattleUnitKind unit_piders_19_data_2E9C8;
s32 stg_02_1_init_evt[8];

// clang-format off
extend_data(80,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(81,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData mri_btlobject_data[43] = {
    { "S_isk_01", 2, 0, { 19.0f, 0.0f, -55.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_01", 2, 1, { -188.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_02", 2, 1, { -147.0f, 0.0f, -52.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_03", 2, 1, { -56.0f, 0.0f, -36.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_04", 2, 1, { 38.0f, 0.0f, -28.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_05", 2, 1, { 157.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_06", 2, 1, { 198.0f, 0.0f, 57.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_07", 2, 1, { -204.0f, 0.0f, -64.0f }, 0, 60, { 0, 0 } },
    { "S_kusa_01", 2, 1, { -1.0f, 0.0f, 76.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "S_isk_02", 2, 0, { 25.0f, 0.0f, -55.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_01", 2, 1, { -188.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_02", 2, 1, { -147.0f, 0.0f, -52.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_03", 2, 1, { -56.0f, 0.0f, -36.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_04", 2, 1, { 38.0f, 0.0f, -28.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_05", 2, 1, { 157.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_06", 2, 1, { 198.0f, 0.0f, 57.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_07", 2, 1, { -204.0f, 0.0f, -64.0f }, 0, 60, { 0, 0 } },
    { "S_kusa_01", 2, 1, { -1.0f, 0.0f, 76.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "S_isk_03", 2, 0, { 12.0f, 0.0f, -55.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_01", 2, 1, { -188.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_02", 2, 1, { -147.0f, 0.0f, -52.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_03", 2, 1, { -56.0f, 0.0f, -36.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_04", 2, 1, { 38.0f, 0.0f, -28.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_05", 2, 1, { 157.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_06", 2, 1, { 198.0f, 0.0f, 57.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_07", 2, 1, { -204.0f, 0.0f, -64.0f }, 0, 60, { 0, 0 } },
    { "S_kusa_01", 2, 1, { -1.0f, 0.0f, 76.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_01", 2, 1, { -188.0f, 0.0f, 48.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_02", 2, 1, { -147.0f, 0.0f, -52.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_03", 2, 1, { -56.0f, 0.0f, -46.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_04", 2, 1, { 38.0f, 0.0f, -57.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_05", 2, 1, { 157.0f, 0.0f, -50.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_06", 2, 1, { 198.0f, 0.0f, 57.0f }, 0, 60, { 0, 0 } },
    { "S_ueki_07", 2, 1, { -204.0f, 0.0f, -64.0f }, 0, 60, { 0, 0 } },
    { "S_kusa_01", 2, 1, { -1.0f, 0.0f, 76.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

// TODO: type
u32 mri_btlstage_data[436] = {
    (u32)"bti_01", (u32)"stg_02_1", 0x0000000B, (u32)&mri_btlobject_data,
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
    (u32)&stg_02_1_init_evt, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00010001, (u32)"bti_01", (u32)"stg_02_1", 0x0000000B,
    (u32)((u8*)&mri_btlobject_data + 0x108), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, (u32)&stg_02_1_init_evt, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00010001, (u32)"bti_01", (u32)"stg_02_1",
    0x0000000B, (u32)((u8*)&mri_btlobject_data + 0x210), 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)&stg_02_1_init_evt, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00010001, (u32)"bti_01",
    (u32)"stg_02_1", 0x0000000A, (u32)((u8*)&mri_btlobject_data + 0x318), 0x00000000,
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
    0x00000064, 0x00000000, 0x00000000, (u32)&stg_02_1_init_evt,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00010001,
};

BattleUnitSetup btlparty_mri_mri_01_01[2] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_mri_mri_01_01 = { 2, btlparty_mri_mri_01_01, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_04_01[1] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_mri_mri_04_01 = { 1, btlparty_mri_mri_04_01, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_05_01[2] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_mri_mri_05_01 = { 2, btlparty_mri_mri_05_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_06_2F_02[3] = {
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_06_2F_02 = { 3, btlparty_mri_mri_06_2F_02, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_06_2F_03[4] = {
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_06_2F_03 = { 4, btlparty_mri_mri_06_2F_03, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_12_01[2] = {
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_mri_mri_12_01 = { 2, btlparty_mri_mri_12_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_mri_mri_12_02[4] = {
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
};

BattleGroupSetup btlgrp_mri_mri_12_02 = { 4, btlparty_mri_mri_12_02, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_mri_mri_15_01[2] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_mri_mri_15_01 = { 2, btlparty_mri_mri_15_01, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_17_01[2] = {
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_17_01 = { 2, btlparty_mri_mri_17_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_18_02[3] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
};

BattleGroupSetup btlgrp_mri_mri_18_02 = { 3, btlparty_mri_mri_18_02, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_18_03[3] = {
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 0.0f, 40.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 75.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
};

BattleGroupSetup btlgrp_mri_mri_18_03 = { 3, btlparty_mri_mri_18_03, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_19_01[2] = {
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_19_01 = { 2, btlparty_mri_mri_19_01, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_19_02[2] = {
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_19_02 = { 2, btlparty_mri_mri_19_02, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_mri_mri_19_04[4] = {
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_monochrome_pakkun_19_data_389E0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome__pakkun },
    { &unit_piders_19_data_2E9C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_mri_mri_19_04 = { 4, btlparty_mri_mri_19_04, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_mri_mri_20_02[2] = {
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
};

BattleGroupSetup btlgrp_mri_mri_20_02 = { 2, btlparty_mri_mri_20_02, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_20_03[3] = {
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 10.0f, 40.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
    { &unit_barriern_19_data_2B8A8, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
    { &unit_gundan_zako_19_data_2A2B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_mri_mri_20_03 = { 3, btlparty_mri_mri_20_03, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_mri_mri_mb[1] = {
    { &unit_boss_magnum_battender, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_magnum_battender },
};

BattleGroupSetup btlgrp_mri_mri_mb = { 1, btlparty_mri_mri_mb, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_test1_off[2] = {
    { 50, &btlgrp_mri_mri_01_01, (struct BattleStageData*)mri_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_test2_off[2] = {
    { 50, &btlgrp_mri_mri_01_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_test3_off[2] = {
    { 50, &btlgrp_mri_mri_01_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_test4_off[2] = {
    { 50, &btlgrp_mri_mri_01_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_01_01_off[2] = {
    { 10, &btlgrp_mri_mri_01_01, (struct BattleStageData*)mri_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_04_01_off[2] = {
    { 10, &btlgrp_mri_mri_04_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_05_01_off[2] = {
    { 10, &btlgrp_mri_mri_05_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_06_2f_02_off[2] = {
    { 10, &btlgrp_mri_mri_06_2F_02, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_06_2f_03_off[2] = {
    { 10, &btlgrp_mri_mri_06_2F_03, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_12_01_off[2] = {
    { 10, &btlgrp_mri_mri_12_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_12_02_off[2] = {
    { 10, &btlgrp_mri_mri_12_02, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_15_01_off[2] = {
    { 10, &btlgrp_mri_mri_15_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_17_01_off[2] = {
    { 10, &btlgrp_mri_mri_17_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_18_02_off[2] = {
    { 10, &btlgrp_mri_mri_18_02, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_18_03_off[2] = {
    { 10, &btlgrp_mri_mri_18_03, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_19_01_off[2] = {
    { 10, &btlgrp_mri_mri_19_01, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_19_02_off[2] = {
    { 10, &btlgrp_mri_mri_19_02, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_19_04_off[2] = {
    { 10, &btlgrp_mri_mri_19_04, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_20_02_off[2] = {
    { 10, &btlgrp_mri_mri_20_02, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_20_03_off[2] = {
    { 10, &btlgrp_mri_mri_20_03, (struct BattleStageData*)(void*)((u8*)mri_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_mri_btlno_mri_mb_off[2] = {
    { 10, &btlgrp_mri_mri_mb, (struct BattleStageData*)mri_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_mri_tbl[22] = {
    {
        "マップstg_02_1_1",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_02_1_2",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_02_1_3",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_02_1_3",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "mri01ぐんだんザコ",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_01_01_off,                                                                    // offLoadouts
        536870928,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                                 // music
    },
    {
        "mri04ぐんだんザコ",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_04_01_off,                                                                    // offLoadouts
        536870928,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                                 // music
    },
    {
        "mri05ぐんだんザコ",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_05_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri06パイダース2F_２",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_06_2f_02_off,                                                               // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "mri06パイダース2F_３",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_06_2f_03_off,                                                               // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "mri12パックンフラワー",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_12_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri12パックンフラワー",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_12_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri15ぐんだんザコ",                                                                                          // name
        NULL,                                                                                                   // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_mri_btlno_mri_15_01_off,                                                                      // offLoadouts
        536870928,                                                                                              // field_0x18
        0,                                                                                                      // field_0x1C
        { 4, 4, 10, 10, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                                   // music
    },
    {
        "mri17パイダース",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_17_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri18軍団ザコ",                                                                                            // name
        NULL,                                                                                                   // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_mri_btlno_mri_18_02_off,                                                                      // offLoadouts
        0,                                                                                                      // field_0x18
        0,                                                                                                      // field_0x1C
        { 4, 4, 10, 10, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                     // music
    },
    {
        "mri18バリアーン",                                                                                           // name
        NULL,                                                                                                   // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_mri_btlno_mri_18_03_off,                                                                      // offLoadouts
        0,                                                                                                      // field_0x18
        0,                                                                                                      // field_0x1C
        { 4, 4, 10, 10, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 10, 10, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                     // music
    },
    {
        "mri19パイダース",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_19_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri19パックン１",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_19_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "mri19パックン３",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_mri_btlno_mri_19_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "mri20軍団ザコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_20_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "mri20バリアーン２",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_mri_btlno_mri_20_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "マグナムバッテンダー",                                                                                           // name
        "mri_01",                                                                                               // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_mri_btlno_mri_mb_off,                                                                         // offLoadouts
        536870928,                                                                                              // field_0x18
        2,                                                                                                      // field_0x1C
        { 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_3RDLEADER1",                                                                             // music
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
u32 btl_mri_setup_no_tbl[44] = {
    (u32)"BTLNO_MRI_TEST1", 0x00000000, (u32)"BTLNO_MRI_TEST2", 0x00000001,
    (u32)"BTLNO_MRI_TEST3", 0x00000002, (u32)"BTLNO_MRI_TEST4", 0x00000003,
    (u32)"BTLNO_MRI_01_01", 0x00000004, (u32)"BTLNO_MRI_04_01", 0x00000005,
    (u32)"BTLNO_MRI_05_01", 0x00000006, (u32)"BTLNO_MRI_06_2F_02", 0x00000007,
    (u32)"BTLNO_MRI_06_2F_03", 0x00000008, (u32)"BTLNO_MRI_12_01", 0x00000009,
    (u32)"BTLNO_MRI_12_02", 0x0000000A, (u32)"BTLNO_MRI_15_01", 0x0000000B,
    (u32)"BTLNO_MRI_17_01", 0x0000000C, (u32)"BTLNO_MRI_18_02", 0x0000000D,
    (u32)"BTLNO_MRI_18_03", 0x0000000E, (u32)"BTLNO_MRI_19_01", 0x0000000F,
    (u32)"BTLNO_MRI_19_02", 0x00000010, (u32)"BTLNO_MRI_19_04", 0x00000011,
    (u32)"BTLNO_MRI_20_02", 0x00000012, (u32)"BTLNO_MRI_20_03", 0x00000013,
    (u32)"BTLNO_MRI_MB", 0x00000014, 0x00000000, 0x00000000,
};

EVT_BEGIN(stg_02_1_init_evt)
    CALL(evt_mapobj_flag_onoff, 1, 0, STRING("S_ueki"), 1)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
