#include "battle_database_aji.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default3[];
extern ItemDropData battle_item_barriern[1];
extern ItemDropData battle_item_barriern_custom[1];
extern ItemDropData battle_item_barriern_z[1];
extern ItemDropData battle_item_default_param_list[1];
extern ItemDropData battle_item_gundan_zako[10];
extern ItemDropData battle_item_gundan_zako_elite[10];
extern ItemDropData battle_item_gundan_zako_magician[10];
extern BattleUnitKind unit_barriern_2_data_3A830;
extern BattleUnitKind unit_barriern_custom;
extern BattleUnitKind unit_barriern_z_2_data_303C8;
extern BattleUnitKind unit_boss_magnum_battender_mkII;
extern BattleUnitKind unit_gundan_zako_2_data_2E898;
extern BattleUnitKind unit_gundan_zako_elite;
extern BattleUnitKind unit_gundan_zako_magician_2_data_34AD8;
static USER_FUNC(_space_rotate);
s32 bgset_aji_space_event[35];

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

BattleStageObjectData aji_btlobject_data[27] = {
    { "dentou_01", 2, 0, { 144.0f, 0.0f, -54.0f }, 0, 60, { 0, 0 } },
    { "dentou_02", 2, 0, { 64.0f, 0.0f, -47.0f }, 10, 50, { 0, 0 } },
    { "dentou_03", 2, 0, { -64.0f, 0.0f, -47.0f }, 20, 40, { 0, 0 } },
    { "dentou_04", 2, 0, { -144.0f, 0.0f, -54.0f }, 30, 30, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_a", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A2", 2, 1, { 0.0f, 0.0f, -74.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_b", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A2", 2, 1, { 0.0f, 0.0f, -74.0f }, 0, 60, { 0, 0 } },
    { "haikei_C_c", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A2", 2, 1, { 0.0f, 0.0f, -74.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A1", 2, 0, { 0.0f, 0.0f, -30.0f }, 0, 60, { 0, 0 } },
    { "C", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "A1", 2, 0, { 0.0f, 0.0f, -52.0f }, 0, 60, { 0, 0 } },
    { "c_ten", 2, 3, { 0.0f, 0.0f, 16.0f }, 0, 60, { 0, 0 } },
    { "c_kabe", 2, 3, { 0.0f, 0.0f, 16.0f }, 0, 60, { 0, 0 } },
    { "c_o", 2, 3, { 0.0f, 0.0f, 7.0f }, 0, 60, { 0, 0 } },
    { "space", 2, 3, { 0.0f, 0.0f, 7.0f }, 0, 60, { 0, 0 } },
    { "star_01", 2, 3, { 0.0f, 0.0f, 39.0f }, 0, 60, { 0, 0 } },
    { "star_02", 2, 3, { 0.0f, 0.0f, 25.0f }, 0, 60, { 0, 0 } },
    { "star_03", 2, 3, { 0.0f, 0.0f, 31.0f }, 0, 60, { 0, 0 } },
};

// TODO: type
u32 aji_btlstage_data[763] = {
    (u32)"bti_01", (u32)"stg_07_5", 0x00000005, (u32)&aji_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"bti_01", (u32)"stg_07_1", 0x00000003,
    (u32)&aji_btlobject_data[5], 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_07_1",
    0x00000003, (u32)&aji_btlobject_data[8], 0x00000000, 0x00000000,
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
    (u32)"stg_07_1", 0x00000003, (u32)&aji_btlobject_data[11], 0x00000000,
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
    (u32)"bti_01", (u32)"stg_07_2", 0x00000003, (u32)&aji_btlobject_data[17],
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)"bti_01", (u32)"stg_07_3", 0x00000003,
    (u32)&aji_btlobject_data[14], 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_07_6",
    0x00000007, (u32)&aji_btlobject_data[20], 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, (u32)&bgset_aji_space_event, 0x00000000,
};

BattleUnitSetup btlparty_aji_aji_mbmkII[1] = {
    { &unit_boss_magnum_battender_mkII, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_aji_aji_mbmkII = { 1, btlparty_aji_aji_mbmkII, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_zako[2] = {
    { &unit_gundan_zako_elite, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_zako = { 2, btlparty_aji_aji_zako, 0, 0, 100, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_01_off_1[2] = {
    { &unit_gundan_zako_elite, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_01_01_off_1 = { 2, btlparty_aji_aji_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_02_off_1[3] = {
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_01_02_off_1 = { 3, btlparty_aji_aji_01_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_03_off_1[4] = {
    { &unit_gundan_zako_elite, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
};

BattleGroupSetup btlgrp_aji_aji_01_03_off_1 = { 4, btlparty_aji_aji_01_03_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_04_off_1[2] = {
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_01_04_off_1 = { 2, btlparty_aji_aji_01_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_aji_aji_01_05_off_1[2] = {
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
};

BattleGroupSetup btlgrp_aji_aji_01_05_off_1 = { 2, btlparty_aji_aji_01_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_06_off_1[4] = {
    { &unit_gundan_zako_elite, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_01_06_off_1 = { 4, btlparty_aji_aji_01_06_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_07_off_1[3] = {
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_aji_aji_01_07_off_1 = { 3, btlparty_aji_aji_01_07_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_01_08_off_1[4] = {
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_elite, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_01_08_off_1 = { 4, btlparty_aji_aji_01_08_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_04_01_off_1[2] = {
    { &unit_barriern_custom, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_custom },
    { &unit_barriern_custom, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_custom },
};

BattleGroupSetup btlgrp_aji_aji_04_01_off_1 = { 2, btlparty_aji_aji_04_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_06_01_off_1[2] = {
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
};

BattleGroupSetup btlgrp_aji_aji_06_01_off_1 = { 2, btlparty_aji_aji_06_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_aji_aji_08_01_off_1[3] = {
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_barriern_2_data_3A830, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern },
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_aji_aji_08_01_off_1 = { 3, btlparty_aji_aji_08_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_aji_aji_09_01_off_1[5] = {
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_2_data_2E898, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
};

BattleGroupSetup btlgrp_aji_aji_09_01_off_1 = { 5, btlparty_aji_aji_09_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_10_01_off_1[1] = {
    { &unit_barriern_custom, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_custom },
};

BattleGroupSetup btlgrp_aji_aji_10_01_off_1 = { 1, btlparty_aji_aji_10_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_13_01_off_1[4] = {
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
    { &unit_gundan_zako_magician_2_data_34AD8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_magician },
};

BattleGroupSetup btlgrp_aji_aji_13_01_off_1 = { 4, btlparty_aji_aji_13_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_aji_aji_13_02_off_1[2] = {
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
    { &unit_barriern_z_2_data_303C8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_barriern_z },
};

BattleGroupSetup btlgrp_aji_aji_13_02_off_1 = { 2, btlparty_aji_aji_13_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_aji_aji_15_01_off_1[1] = {
    { &unit_gundan_zako_elite, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gundan_zako_elite },
};

BattleGroupSetup btlgrp_aji_aji_15_01_off_1 = { 1, btlparty_aji_aji_15_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_test1_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_test2_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_test3_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x884) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_test4_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)aji_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_test5_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_mbmkii_off[2] = {
    { 50, &btlgrp_aji_aji_mbmkII, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x884) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_00_00_off[2] = {
    { 50, &btlgrp_aji_aji_zako, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_01_off[2] = {
    { 20, &btlgrp_aji_aji_01_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_02_off[2] = {
    { 20, &btlgrp_aji_aji_01_02_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_03_off[2] = {
    { 20, &btlgrp_aji_aji_01_03_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_04_off[2] = {
    { 20, &btlgrp_aji_aji_01_04_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_05_off[2] = {
    { 20, &btlgrp_aji_aji_01_05_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_06_off[2] = {
    { 20, &btlgrp_aji_aji_01_06_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_07_off[2] = {
    { 20, &btlgrp_aji_aji_01_07_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_01_08_off[2] = {
    { 20, &btlgrp_aji_aji_01_08_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_04_01_off[2] = {
    { 20, &btlgrp_aji_aji_04_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_06_01_off[2] = {
    { 20, &btlgrp_aji_aji_06_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_08_01_off[2] = {
    { 20, &btlgrp_aji_aji_08_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_09_01_off[2] = {
    { 20, &btlgrp_aji_aji_09_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_10_01_off[2] = {
    { 20, &btlgrp_aji_aji_10_01_off_1, (struct BattleStageData*)aji_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_13_01_off[2] = {
    { 20, &btlgrp_aji_aji_13_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_13_02_off[2] = {
    { 20, &btlgrp_aji_aji_13_02_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_aji_btlno_aji_15_01_off[2] = {
    { 20, &btlgrp_aji_aji_15_01_off_1, (struct BattleStageData*)(void*)((u8*)aji_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_aji_tbl[24] = {
    {
        "マップstg_07_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_aji_btlno_aji_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_07_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_aji_btlno_aji_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_07_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_aji_btlno_aji_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_07_5",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_aji_btlno_aji_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_07_6",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_aji_btlno_aji_test5_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マグナムバッテンダー幕痛",                                                                                       // name
        "aji_14",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_mbmkii_off,                                                                   // offLoadouts
        536870928,                                                                                            // field_0x18
        6,                                                                                                    // field_0x1C
        { 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_3RDLEADER1",                                                                           // music
    },
    {
        "軍団ザコエリート",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_00_00_off,                                                                    // offLoadouts
        536870928,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                                // music
    },
    {
        "aji01軍団ザコエリート",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコエリート",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコ魔法使い",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01バリアーンＺ",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_05_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコエリート",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_06_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_07_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji01軍団ザコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_01_08_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji04バリアーンカスタム",                                                                                     // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_04_01_off,                                                                    // offLoadouts
        16,                                                                                                   // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji06軍団ザコ魔法使い",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_06_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji08軍団ザコ魔法使い",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_08_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji09軍団ザコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_09_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji10バリアーンカスタム",                                                                                     // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_10_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji13軍団ザコ魔法使い",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_13_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji13バリアーンＺ",                                                                                        // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_13_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "aji15軍団ザコエリート",                                                                                      // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_aji_btlno_aji_15_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
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
u32 btl_aji_setup_no_tbl[48] = {
    (u32)"BTLNO_AJI_TEST1", 0x00000000, (u32)"BTLNO_AJI_TEST2", 0x00000001,
    (u32)"BTLNO_AJI_TEST3", 0x00000002, (u32)"BTLNO_AJI_TEST4", 0x00000003,
    (u32)"BTLNO_AJI_TEST5", 0x00000004, (u32)"BTLNO_AJI_MBMKII", 0x00000005,
    (u32)"BTLNO_AJI_00_00", 0x00000006, (u32)"BTLNO_AJI_01_01", 0x00000007,
    (u32)"BTLNO_AJI_01_02", 0x00000008, (u32)"BTLNO_AJI_01_03", 0x00000009,
    (u32)"BTLNO_AJI_01_04", 0x0000000A, (u32)"BTLNO_AJI_01_05", 0x0000000B,
    (u32)"BTLNO_AJI_01_06", 0x0000000C, (u32)"BTLNO_AJI_01_07", 0x0000000D,
    (u32)"BTLNO_AJI_01_08", 0x0000000E, (u32)"BTLNO_AJI_04_01", 0x0000000F,
    (u32)"BTLNO_AJI_06_01", 0x00000010, (u32)"BTLNO_AJI_08_01", 0x00000011,
    (u32)"BTLNO_AJI_09_01", 0x00000012, (u32)"BTLNO_AJI_10_01", 0x00000013,
    (u32)"BTLNO_AJI_13_01", 0x00000014, (u32)"BTLNO_AJI_13_02", 0x00000015,
    (u32)"BTLNO_AJI_15_01", 0x00000016, 0x00000000, 0x00000000,
};

static USER_FUNC(_space_rotate) {
    s32* args = event->args;
    const char* name = (const char*)evtGetValue(event, args[0]);
    s32 index = args[1];
    s32 start = evtGetValue(event, index);
    s32 end = evtGetValue(event, args[2]);
    BattleStageObject* obj = BattleSearchObjectPtr(name);
    s32 next;

    obj->mRotation.z = 360.0f * (f32)start / (f32)end;
    next = start + 1;
    if (next >= end) {
        next = 0;
    }
    evtSetValue(event, index, next);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(bgset_aji_space_event)
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
