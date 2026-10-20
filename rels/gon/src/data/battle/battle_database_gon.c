#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_gonbaba[1];
extern ItemDropData battle_item_honenoko[9];
extern ItemDropData battle_item_nokonoko[10];
extern ItemDropData battle_item_patakuri[10];
extern ItemDropData battle_item_patapata[10];
extern ItemDropData battle_item_red_honenoko[1];
extern ItemDropData battle_item_togekuri[10];
extern BattleUnitKind unit_boss_gonbaba;
extern BattleUnitKind unit_honenoko_9_data_14C90;
extern BattleUnitKind unit_nokonoko_9_data_18CA8;
extern BattleUnitKind unit_patakuri_9_data_1A0F0;
extern BattleUnitKind unit_patapata_9_data_1B7A8;
extern BattleUnitKind unit_red_honenoko_9_data_17268;
extern BattleUnitKind unit_togekuri_9_data_1DA88;

// clang-format off
extend_data(60,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(61,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData gon_btlobject_data[10] = {
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_B_a", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_a", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_B_b", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_b", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_B_c", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_c", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

// TODO: type
u32 gon_btlstage_data[545] = {
    (u32)"bti_01", (u32)"stg_01_5", 0x00000002, (u32)&gon_btlobject_data,
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
    0x00000000, (u32)"bti_01", (u32)"stg_01_6", 0x00000002,
    (u32)&gon_btlobject_data[8], 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_01_3",
    0x00000002, (u32)&gon_btlobject_data[2], 0x00000000, 0x00000000,
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
    (u32)"stg_01_3", 0x00000002, (u32)&gon_btlobject_data[4], 0x00000000,
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
    (u32)"bti_01", (u32)"stg_01_3", 0x00000002, (u32)&gon_btlobject_data[6],
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
    0x00000000,
};

BattleUnitSetup btlparty_gon_gon_02_02_off_1[3] = {
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_02_02_off_1 = { 3, btlparty_gon_gon_02_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_02_03_off_1[2] = {
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_02_03_off_1 = { 2, btlparty_gon_gon_02_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_02_04_off_1[3] = {
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_gon_gon_02_04_off_1 = { 3, btlparty_gon_gon_02_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_03_01_off_1[5] = {
    { &unit_red_honenoko_9_data_17268, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_red_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_03_01_off_1 = { 5, btlparty_gon_gon_03_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_04_01_off_1[2] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_04_01_off_1 = { 2, btlparty_gon_gon_04_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_04_02_off_1[4] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_04_02_off_1 = { 4, btlparty_gon_gon_04_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_04_03_off_1[3] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_04_03_off_1 = { 3, btlparty_gon_gon_04_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_06_03_off_1[3] = {
    { &unit_patakuri_9_data_1A0F0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_06_03_off_1 = { 3, btlparty_gon_gon_06_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_06_04_off_1[3] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_patakuri_9_data_1A0F0, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_9_data_1A0F0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_06_04_off_1 = { 3, btlparty_gon_gon_06_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_08_01_off_1[2] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_08_01_off_1 = { 2, btlparty_gon_gon_08_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_08_03_off_1[2] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_togekuri_9_data_1DA88, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_gon_gon_08_03_off_1 = { 2, btlparty_gon_gon_08_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_08_05_off_1[2] = {
    { &unit_patakuri_9_data_1A0F0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_08_05_off_1 = { 2, btlparty_gon_gon_08_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_09_01_off_1[3] = {
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_gon_gon_09_01_off_1 = { 3, btlparty_gon_gon_09_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_09_02_off_1[3] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_gon_gon_09_02_off_1 = { 3, btlparty_gon_gon_09_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_09_04_off_1[4] = {
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_9_data_14C90, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_gon_gon_09_04_off_1 = { 4, btlparty_gon_gon_09_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_10_01_off_1[4] = {
    { &unit_togekuri_9_data_1DA88, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_togekuri_9_data_1DA88, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_gon_gon_10_01_off_1 = { 4, btlparty_gon_gon_10_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_10_02_off_1[3] = {
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_10_02_off_1 = { 3, btlparty_gon_gon_10_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_10_03_off_1[4] = {
    { &unit_nokonoko_9_data_18CA8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_togekuri_9_data_1DA88, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_patakuri_9_data_1A0F0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patapata_9_data_1B7A8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patapata },
};

BattleGroupSetup btlgrp_gon_gon_10_03_off_1 = { 4, btlparty_gon_gon_10_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_gon_gon_11_01_off_1[1] = {
    { &unit_boss_gonbaba, 1, 0x04000004, { 365.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gonbaba },
};

BattleGroupSetup btlgrp_gon_gon_11_01_off_1 = { 1, btlparty_gon_gon_11_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_test1_off[2] = {
    { 50, &btlgrp_gon_gon_02_02_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_test2_off[2] = {
    { 50, &btlgrp_gon_gon_02_02_off_1, (struct BattleStageData*)gon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_test3_off[2] = {
    { 50, &btlgrp_gon_gon_02_02_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_02_02_off[2] = {
    { 50, &btlgrp_gon_gon_02_02_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_02_03_off[2] = {
    { 10, &btlgrp_gon_gon_02_03_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_02_04_off[2] = {
    { 10, &btlgrp_gon_gon_02_04_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_03_01_off[2] = {
    { 10, &btlgrp_gon_gon_03_01_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_04_02_off[2] = {
    { 10, &btlgrp_gon_gon_04_02_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_04_03_off[2] = {
    { 10, &btlgrp_gon_gon_04_03_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_06_03_off[2] = {
    { 10, &btlgrp_gon_gon_06_03_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_06_04_off[2] = {
    { 10, &btlgrp_gon_gon_06_04_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_08_01_off[2] = {
    { 10, &btlgrp_gon_gon_08_01_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_08_03_off[2] = {
    { 10, &btlgrp_gon_gon_08_03_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_08_05_off[2] = {
    { 10, &btlgrp_gon_gon_08_05_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_09_01_off[2] = {
    { 10, &btlgrp_gon_gon_09_01_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_09_02_off[2] = {
    { 10, &btlgrp_gon_gon_09_02_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_09_04_off[2] = {
    { 10, &btlgrp_gon_gon_09_04_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_10_01_off[2] = {
    { 10, &btlgrp_gon_gon_10_01_off_1, (struct BattleStageData*)gon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_10_02_off[2] = {
    { 10, &btlgrp_gon_gon_10_02_off_1, (struct BattleStageData*)gon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_10_03_off[2] = {
    { 10, &btlgrp_gon_gon_10_03_off_1, (struct BattleStageData*)gon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_gon_btlno_gon_11_01_off[2] = {
    { 20, &btlgrp_gon_gon_11_01_off_1, (struct BattleStageData*)(void*)((u8*)gon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_gon_tbl[22] = {
    {
        "マップstg_01_03",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gon_btlno_gon_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_01_05",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gon_btlno_gon_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_01_06",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gon_btlno_gon_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "gon02パタパタ1",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_02_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon02ノコノコ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_02_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon02パタパタ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_02_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon03赤ホネノコ",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gon_btlno_gon_03_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        3,                                                                                                  // field_0x1C
        { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "gon04ホネノコ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_04_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon04ホネノコ3",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_04_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon06トゲクリボー",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_06_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon06ホネノコ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_06_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon08ホネノコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_08_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon08ホネノコ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_08_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon08パタクリ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_08_05_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon09ノコノコ1",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_09_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon09ホネノコ1",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_09_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon09ホネノコ2",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_09_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 10, 10, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon10トゲクリボー",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_10_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon10パタパタ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_10_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon10ノコノコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_gon_btlno_gon_10_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "gon11ゴンババ",                                                                                        // name
        "gon_11",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_gon_btlno_gon_11_01_off,                                                                  // offLoadouts
        537919504,                                                                                          // field_0x18
        3,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_BOSS_STG1_GONBABA1",                                                                           // music
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
u32 btl_gon_setup_no_tbl[44] = {
    (u32)"BTLNO_GON_TEST1", 0x00000000, (u32)"BTLNO_GON_TEST2", 0x00000001,
    (u32)"BTLNO_GON_TEST3", 0x00000002, (u32)"BTLNO_GON_02_02", 0x00000003,
    (u32)"BTLNO_GON_02_03", 0x00000004, (u32)"BTLNO_GON_02_04", 0x00000005,
    (u32)"BTLNO_GON_03_01", 0x00000006, (u32)"BTLNO_GON_04_02", 0x00000007,
    (u32)"BTLNO_GON_04_03", 0x00000008, (u32)"BTLNO_GON_06_03", 0x00000009,
    (u32)"BTLNO_GON_06_04", 0x0000000A, (u32)"BTLNO_GON_08_01", 0x0000000B,
    (u32)"BTLNO_GON_08_03", 0x0000000C, (u32)"BTLNO_GON_08_05", 0x0000000D,
    (u32)"BTLNO_GON_09_01", 0x0000000E, (u32)"BTLNO_GON_09_02", 0x0000000F,
    (u32)"BTLNO_GON_09_04", 0x00000010, (u32)"BTLNO_GON_10_01", 0x00000011,
    (u32)"BTLNO_GON_10_02", 0x00000012, (u32)"BTLNO_GON_10_03", 0x00000013,
    (u32)"BTLNO_GON_11_01", 0x00000014, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
