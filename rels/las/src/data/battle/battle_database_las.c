#include "battle_database_las.h"
#include "battle/battle.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_weapon_power.h"
#include "driver/effdrv.h"
#include "driver/mapdrv.h"
#include "eff/eff_fire.h"
#include "evt/evt_cmd.h"
#include "evt/evt_eff.h"
#include "evt/evt_map.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma pool_data off

EffectEntry* unk_las_bss0000009c[6];

s8 unk_las_bss00000098[4];

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_heart_drop_param_default3[];
extern s32 battle_heart_drop_param_default4[];
extern ItemDropData battle_item_NONE[1];
extern ItemDropData battle_item_basabasa_chururu[10];
extern ItemDropData battle_item_default_param_list[1];
extern ItemDropData battle_item_karon[9];
extern ItemDropData battle_item_phantom[8];
extern ItemDropData battle_item_super_killer[1];
extern ItemDropData battle_item_super_killer_cannon[5];
extern ItemDropData battle_item_super_mahorn[9];
extern ItemDropData battle_item_wanwan[7];
extern BattleUnitKind unit_basabasa_chururu_17_data_528D0;
extern BattleUnitKind unit_black_karon;
extern BattleUnitKind unit_boss_batten_leader;
extern BattleUnitKind unit_boss_black_peach;
extern BattleUnitKind unit_boss_bunbaba;
extern BattleUnitKind unit_boss_koopa;
extern BattleUnitKind unit_boss_rampell_las;
extern BattleUnitKind unit_honenoko_17_data_57250;
extern BattleUnitKind unit_karon_17_data_3F680;
extern BattleUnitKind unit_phantom_17_data_540A8;
extern BattleUnitKind unit_red_honenoko_17_data_58C90;
extern BattleUnitKind unit_super_killer;
extern BattleUnitKind unit_super_killer_cannon;
extern BattleUnitKind unit_super_mahorn_17_data_4CA30;
extern BattleUnitKind unit_wanwan_17_data_50DF8;
s32 bgset_las_gear_event[38];
s32 stg_08_1_init_event[4];
s32 stg_08_6_init_event[4];

// clang-format off
extend_data(124,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(125,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData las_btlobject_data[38] = {
    {"A1", 2, 0, {93.0f, 0.0f, -64.0f}, 0, 60},
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C_kabe", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_01a", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_01b", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_02a", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_02b", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_03a", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"gear_03b", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"if_wire", 5, 6, {0.0f, 130.0f, 0.0f}, 0, 0},
    {"if_body", 5, 6, {0.0f, 130.0f, 0.0f}, 0, 0},
    {"C", 2, 3, {0.0f, 0.0f, -99.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, -99.0f}, 0, 60},
    {"if_wire", 5, 6, {0.0f, 130.0f, 0.0f}, 0, 0},
    {"if_body", 5, 6, {0.0f, 130.0f, 0.0f}, 0, 0},
    {"daiza_01", 2, 0, {158.0f, 0.0f, -28.0f}, 0, 60},
    {"daiza_02", 2, 0, {95.0f, 0.0f, -50.0f}, 10, 50},
    {"daiza_03", 2, 0, {-158.0f, 0.0f, -28.0f}, 20, 40},
    {"daiza_04", 2, 0, {-95.0f, 0.0f, -50.0f}, 30, 30},
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"rou1", 2, 0, {58.0f, 0.0f, -81.0f}, 0, 60},
    {"rou2", 2, 0, {-109.0f, 0.0f, -69.0f}, 0, 60},
    {"rou3", 2, 0, {-58.0f, 0.0f, -81.0f}, 0, 60},
    {"rou4", 2, 0, {179.0f, 0.0f, -54.0f}, 0, 60},
    {"rou5", 2, 0, {109.0f, 0.0f, -69.0f}, 0, 60},
    {"rou6", 2, 0, {-179.0f, 0.0f, -54.0f}, 0, 60},
    {"B", 2, 2, {91.0f, 0.0f, -57.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"A2", 2, 1, {0.0f, 0.0f, 80.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"B", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
};

// TODO: type
u32 las_btlstage_data[1090] = {
    (u32)"bti_01", (u32)"stg_08_5", 0x00000009, (u32)&las_btlobject_data,
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
    0x00000000, 0x00000000, 0x00000000, (u32)&bgset_las_gear_event,
    0x00000000, (u32)"bti_01", (u32)"stg_08_0", 0x00000002,
    (u32)((u8*)&las_btlobject_data + 0xd8), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_08_0",
    0x00000004, (u32)((u8*)&las_btlobject_data + 0x108), 0x00000000, 0x00000000,
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
    (u32)"stg_08_2", 0x00000001, (u32)((u8*)&las_btlobject_data + 0x168), 0x00000000,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"bti_01", (u32)"stg_08_2", 0x00000003, (u32)((u8*)&las_btlobject_data + 0x180),
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
    0x00000000, (u32)"bti_01", (u32)"stg_08_4", 0x00000006,
    (u32)((u8*)&las_btlobject_data + 0x1c8), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_08_4",
    0x00000001, (u32)((u8*)&las_btlobject_data + 0x258), 0x00000000, 0x00000000,
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
    (u32)"stg_08_6", 0x00000008, (u32)((u8*)&las_btlobject_data + 0x270), 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (u32)weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100070, 0x10000000, 0x001A0302, 0x00000000,
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
    0x0210006F, 0x20000000, 0x001A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, (u32)&stg_08_6_init_event,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00010101,
    (u32)"bti_01", (u32)"stg_08_1", 0x00000002, (u32)((u8*)&las_btlobject_data + 0x330),
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
    (u32)&stg_08_1_init_event, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00010101, (u32)"bti_01", (u32)"stg_08_3", 0x00000002,
    (u32)((u8*)&las_btlobject_data + 0x360), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000,
};

BattleUnitSetup btlparty_las_las_05_01[5] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_black_karon, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
};

BattleGroupSetup btlgrp_las_las_05_01 = { 5, btlparty_las_las_05_01, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_17_01_off_1[1] = {
    { &unit_honenoko_17_data_57250, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_NONE },
};

BattleGroupSetup btlgrp_las_las_17_01_off_1 = { 1, btlparty_las_las_17_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_17_02_off_1[1] = {
    { &unit_red_honenoko_17_data_58C90, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_NONE },
};

BattleGroupSetup btlgrp_las_las_17_02_off_1 = { 1, btlparty_las_las_17_02_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_17_03_off_1[1] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_NONE },
};

BattleGroupSetup btlgrp_las_las_17_03_off_1 = { 1, btlparty_las_las_17_03_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_17_04_off_1[1] = {
    { &unit_black_karon, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_NONE },
};

BattleGroupSetup btlgrp_las_las_17_04_off_1 = { 1, btlparty_las_las_17_04_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_09_rampell[1] = {
    { &unit_boss_rampell_las, 1, 0x04000004, { 55.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_09_rampell = { 1, btlparty_las_las_09_rampell, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_bunbaba[1] = {
    { &unit_boss_bunbaba, 1, 0x04000004, { 365.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_bunbaba = { 1, btlparty_las_las_bunbaba, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_28_batten_leader[1] = {
    { &unit_boss_batten_leader, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_28_batten_leader = { 1, btlparty_las_las_28_batten_leader, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_28_koopa[1] = {
    { &unit_boss_koopa, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_28_koopa = { 1, btlparty_las_las_28_koopa, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_29_black_peach_1[1] = {
    { &unit_boss_black_peach, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_29_black_peach_1 = { 1, btlparty_las_las_29_black_peach_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_29_black_peach_2[1] = {
    { &unit_boss_black_peach, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_default_param_list },
};

BattleGroupSetup btlgrp_las_las_29_black_peach_2 = { 1, btlparty_las_las_29_black_peach_2, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_01_off_1[2] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_01_off_1 = { 2, btlparty_las_las_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_02_off_1[3] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_02_off_1 = { 3, btlparty_las_las_01_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_03_off_1[4] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 150.0f, 130.0f, 20.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_03_off_1 = { 4, btlparty_las_las_01_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_01_off_2[2] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_01_off_2 = { 2, btlparty_las_las_01_01_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_02_off_2[3] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_02_off_2 = { 3, btlparty_las_las_01_02_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_01_03_off_2[4] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 150.0f, 130.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_01_03_off_2 = { 4, btlparty_las_las_01_03_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_02_01_off_1[2] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
};

BattleGroupSetup btlgrp_las_las_02_01_off_1 = { 2, btlparty_las_las_02_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_02_02_off_1[3] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_02_02_off_1 = { 3, btlparty_las_las_02_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_02_03_off_1[3] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_02_03_off_1 = { 3, btlparty_las_las_02_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_02_04_off_1[4] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
};

BattleGroupSetup btlgrp_las_las_02_04_off_1 = { 4, btlparty_las_las_02_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_02_05_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_02_05_off_1 = { 2, btlparty_las_las_02_05_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_02_06_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_02_06_off_1 = { 2, btlparty_las_las_02_06_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_02_99_off_1[2] = {
    { &unit_super_killer, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer },
    { &unit_super_killer, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer },
};

BattleGroupSetup btlgrp_las_las_02_99_off_1 = { 2, btlparty_las_las_02_99_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_01_off_1[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_01_off_1 = { 1, btlparty_las_las_03_01_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_02_off_1[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_02_off_1 = { 1, btlparty_las_las_03_02_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_03_off_1[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_03_off_1 = { 1, btlparty_las_las_03_03_off_1, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_01_off_2[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_01_off_2 = { 1, btlparty_las_las_03_01_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_02_off_2[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_02_off_2 = { 1, btlparty_las_las_03_02_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_03_03_off_2[1] = {
    { &unit_basabasa_chururu_17_data_528D0, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_las_las_03_03_off_2 = { 1, btlparty_las_las_03_03_off_2, 25, 25, 50, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_04_01_off_1[1] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_04_01_off_1 = { 1, btlparty_las_las_04_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_06_01_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_06_01_off_1 = { 2, btlparty_las_las_06_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_06_02_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_06_02_off_1 = { 2, btlparty_las_las_06_02_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_06_03_off_1[2] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_06_03_off_1 = { 2, btlparty_las_las_06_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_06_04_off_1[4] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_06_04_off_1 = { 4, btlparty_las_las_06_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_06_05_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_06_05_off_1 = { 2, btlparty_las_las_06_05_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_06_06_off_1[2] = {
    { &unit_super_killer_cannon, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
    { &unit_super_killer_cannon, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer_cannon },
};

BattleGroupSetup btlgrp_las_las_06_06_off_1 = { 2, btlparty_las_las_06_06_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_06_99_off_1[2] = {
    { &unit_super_killer, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer },
    { &unit_super_killer, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_killer },
};

BattleGroupSetup btlgrp_las_las_06_99_off_1 = { 2, btlparty_las_las_06_99_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_07_01_off_1[3] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_07_01_off_1 = { 3, btlparty_las_las_07_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_07_02_off_1[3] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_07_02_off_1 = { 3, btlparty_las_las_07_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_07_03_off_1[3] = {
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_17_data_3F680, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_07_03_off_1 = { 3, btlparty_las_las_07_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_09_01_off_1[2] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_las_las_09_01_off_1 = { 2, btlparty_las_las_09_01_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_09_02_off_1[3] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_las_las_09_02_off_1 = { 3, btlparty_las_las_09_02_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_19_01_off_1[1] = {
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_19_01_off_1 = { 1, btlparty_las_las_19_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_19_02_off_1[2] = {
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_19_02_off_1 = { 2, btlparty_las_las_19_02_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_19_03_off_1[3] = {
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_19_03_off_1 = { 3, btlparty_las_las_19_03_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_20_01_off_1[1] = {
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_20_01_off_1 = { 1, btlparty_las_las_20_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_21_01_off_1[2] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_las_las_21_01_off_1 = { 2, btlparty_las_las_21_01_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_21_02_off_1[2] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_las_las_21_02_off_1 = { 2, btlparty_las_las_21_02_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_22_01_off_1[2] = {
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_22_01_off_1 = { 2, btlparty_las_las_22_01_off_1, 25, 25, 50, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_22_02_off_1[4] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_22_02_off_1 = { 4, btlparty_las_las_22_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_22_03_off_1[2] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_22_03_off_1 = { 2, btlparty_las_las_22_03_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_24_01_off_1[2] = {
    { &unit_wanwan_17_data_50DF8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_24_01_off_1 = { 2, btlparty_las_las_24_01_off_1, 25, 25, 50, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_las_las_25_01_off_1[1] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_25_01_off_1 = { 1, btlparty_las_las_25_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_25_02_off_1[3] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_25_02_off_1 = { 3, btlparty_las_las_25_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_25_03_off_1[4] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_25_03_off_1 = { 4, btlparty_las_las_25_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_25_04_off_1[2] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_25_04_off_1 = { 2, btlparty_las_las_25_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_25_05_off_1[3] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_super_mahorn_17_data_4CA30, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_las_las_25_05_off_1 = { 3, btlparty_las_las_25_05_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_las_las_25_06_off_1[3] = {
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_17_data_540A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_las_las_25_06_off_1 = { 3, btlparty_las_las_25_06_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test1_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test2_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test3_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test4_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xF54) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test5_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x884) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test6_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_test7_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xBEC) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_05_01_off[2] = {
    { 50, &btlgrp_las_las_05_01, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_09_rampell_off[2] = {
    { 50, &btlgrp_las_las_09_rampell, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xF54) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_17_01_off[2] = {
    { 50, &btlgrp_las_las_17_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_17_02_off[2] = {
    { 50, &btlgrp_las_las_17_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_17_03_off[2] = {
    { 50, &btlgrp_las_las_17_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_17_04_off[2] = {
    { 50, &btlgrp_las_las_17_04_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_28_koopa_off[2] = {
    { 50, &btlgrp_las_las_28_koopa, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_bunbaba_off[2] = {
    { 50, &btlgrp_las_las_bunbaba, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_28_batten_leader_off[2] = {
    { 50, &btlgrp_las_las_28_batten_leader, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_29_black_peach_1_off[2] = {
    { 50, &btlgrp_las_las_29_black_peach_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xBEC) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_29_black_peach_2_off[2] = {
    { 50, &btlgrp_las_las_29_black_peach_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xBEC) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_01_01_off[3] = {
    { 20, &btlgrp_las_las_01_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x1B4) },
    { 0, &btlgrp_las_las_01_01_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_01_02_off[3] = {
    { 20, &btlgrp_las_las_01_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x1B4) },
    { 0, &btlgrp_las_las_01_02_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_01_03_off[3] = {
    { 20, &btlgrp_las_las_01_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x1B4) },
    { 0, &btlgrp_las_las_01_03_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_01_off[2] = {
    { 20, &btlgrp_las_las_02_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_02_off[2] = {
    { 20, &btlgrp_las_las_02_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_03_off[2] = {
    { 20, &btlgrp_las_las_02_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_04_off[2] = {
    { 20, &btlgrp_las_las_02_04_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_05_off[2] = {
    { 20, &btlgrp_las_las_02_05_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_06_off[2] = {
    { 20, &btlgrp_las_las_02_06_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_02_99_off[2] = {
    { 20, &btlgrp_las_las_02_99_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_03_01_off[3] = {
    { 20, &btlgrp_las_las_03_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, &btlgrp_las_las_03_01_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_03_02_off[3] = {
    { 20, &btlgrp_las_las_03_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, &btlgrp_las_las_03_02_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_03_03_off[3] = {
    { 20, &btlgrp_las_las_03_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, &btlgrp_las_las_03_03_off_2, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_04_01_off[2] = {
    { 20, &btlgrp_las_las_04_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_01_off[2] = {
    { 20, &btlgrp_las_las_06_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_02_off[2] = {
    { 20, &btlgrp_las_las_06_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_03_off[2] = {
    { 20, &btlgrp_las_las_06_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_04_off[2] = {
    { 20, &btlgrp_las_las_06_04_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_05_off[2] = {
    { 20, &btlgrp_las_las_06_05_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_06_off[2] = {
    { 20, &btlgrp_las_las_06_06_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_06_99_off[2] = {
    { 20, &btlgrp_las_las_06_99_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_07_01_off[2] = {
    { 20, &btlgrp_las_las_07_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_07_02_off[2] = {
    { 20, &btlgrp_las_las_07_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_07_03_off[2] = {
    { 20, &btlgrp_las_las_07_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_09_01_off[2] = {
    { 20, &btlgrp_las_las_09_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xF54) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_09_02_off[2] = {
    { 20, &btlgrp_las_las_09_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0xF54) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_19_01_off[2] = {
    { 20, &btlgrp_las_las_19_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_19_02_off[2] = {
    { 20, &btlgrp_las_las_19_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_19_03_off[2] = {
    { 20, &btlgrp_las_las_19_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_20_01_off[2] = {
    { 20, &btlgrp_las_las_20_01_off_1, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_21_01_off[2] = {
    { 20, &btlgrp_las_las_21_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_21_02_off[2] = {
    { 20, &btlgrp_las_las_21_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_22_01_off[2] = {
    { 20, &btlgrp_las_las_22_01_off_1, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_22_02_off[2] = {
    { 20, &btlgrp_las_las_22_02_off_1, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_22_03_off[2] = {
    { 20, &btlgrp_las_las_22_03_off_1, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_24_01_off[2] = {
    { 20, &btlgrp_las_las_24_01_off_1, (struct BattleStageData*)las_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_01_off[2] = {
    { 20, &btlgrp_las_las_25_01_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_02_off[2] = {
    { 20, &btlgrp_las_las_25_02_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_03_off[2] = {
    { 20, &btlgrp_las_las_25_03_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_04_off[2] = {
    { 20, &btlgrp_las_las_25_04_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_05_off[2] = {
    { 20, &btlgrp_las_las_25_05_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_las_btlno_las_25_06_off[2] = {
    { 20, &btlgrp_las_las_25_06_off_1, (struct BattleStageData*)(void*)((u8*)las_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_las_tbl[61] = {
    {
        "マップstg_08_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test5_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_5",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test6_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_08_6",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_test7_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "黒カロン",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_05_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_CHUBOSS_BATTLE1",                                                                              // music
    },
    {
        "変身ランペル+魔女",                                                                                        // name
        "las_09",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_09_rampell_off,                                                             // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG3_3WOMEN1",                                                                            // music
    },
    {
        "las17クリボー",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_17_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las17トゲクリボー",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_17_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las17ノコノコ",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_17_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las17シンノスケ",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_17_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "クッパ＆カメックオババ",                                                                                      // name
        "las_28",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_28_koopa_off,                                                               // offLoadouts
        536870928,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG8_KOOPA",                                                                              // music
    },
    {
        "ブンババ",                                                                                             // name
        "las_26",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_bunbaba_off,                                                                // offLoadouts
        537919504,                                                                                          // field_0x18
        6,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG1_GONBABA1",                                                                           // music
    },
    {
        "バッテンリーダー",                                                                                           // name
        "las_28",                                                                                             // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_28_batten_leader_off,                                                         // offLoadouts
        536870928,                                                                                            // field_0x18
        5,                                                                                                    // field_0x1C
        { 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG8_3RDHEAD1",                                                                             // music
    },
    {
        "ブラックピーチ1回目",                                                                                           // name
        "las_29",                                                                                               // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_las_btlno_las_29_black_peach_1_off,                                                           // offLoadouts
        536870928,                                                                                              // field_0x18
        6,                                                                                                      // field_0x1C
        { 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG8_WITCH",                                                                                  // music
    },
    {
        "ブラックピーチ2回目",                                                                                           // name
        "las_29",                                                                                               // field_0x4
        0,                                                                                                      // field_0x8
        -1,                                                                                                     // flag
        NULL,                                                                                                   // onLoadouts
        btlgrpsel_las_btlno_las_29_black_peach_2_off,                                                           // offLoadouts
        536870928,                                                                                              // field_0x18
        6,                                                                                                      // field_0x1C
        { 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_BOSS_STG8_LAST_BOSS2",                                                                             // music
    },
    {
        "las01バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_01_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las01バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_01_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las01バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_01_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las02カロン",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_02_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "las02バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_02_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las02カロン",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_02_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "las02カロン",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_02_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "las02スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_02_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las02スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_02_06_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las02スーパーキラー",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_02_99_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las03バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_03_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las03バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_03_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las03バサバサチュルル",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_03_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las04ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_04_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06スーパーキラー大砲",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_06_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las06スーパーキラー",                                                                                     // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_06_99_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las07ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_07_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las07カロン",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_07_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "las07カロン",                                                                                           // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_las_btlno_las_07_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "las09ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_09_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las09ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_09_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las19スーパーマホーン",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_19_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las19スーパーマホーン",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_19_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las19スーパーマホーン",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_19_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las20スーパーマホーン",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_20_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las21ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_21_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las21ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_21_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las22スーパーマホーン",                                                                                    // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_22_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las22ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_22_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las22ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_22_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las24ワンワン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_24_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_04_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_05_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "las25ファントム",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_las_btlno_las_25_06_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_las_setup_no_tbl[122] = {
    (u32)"BTLNO_LAS_TEST1", 0x00000000, (u32)"BTLNO_LAS_TEST2", 0x00000001,
    (u32)"BTLNO_LAS_TEST3", 0x00000002, (u32)"BTLNO_LAS_TEST4", 0x00000003,
    (u32)"BTLNO_LAS_TEST5", 0x00000004, (u32)"BTLNO_LAS_TEST6", 0x00000005,
    (u32)"BTLNO_LAS_TEST7", 0x00000006, (u32)"BTLNO_LAS_05_01", 0x00000007,
    (u32)"BTLNO_LAS_09_RAMPELL", 0x00000008, (u32)"BTLNO_LAS_17_01", 0x00000009,
    (u32)"BTLNO_LAS_17_02", 0x0000000A, (u32)"BTLNO_LAS_17_03", 0x0000000B,
    (u32)"BTLNO_LAS_17_04", 0x0000000C, (u32)"BTLNO_LAS_28_KOOPA", 0x0000000D,
    (u32)"BTLNO_LAS_BUNBABA", 0x0000000E, (u32)"BTLNO_LAS_28_BATTEN_LEADER", 0x0000000F,
    (u32)"BTLNO_LAS_29_BLACK_PEACH_1", 0x00000010, (u32)"BTLNO_LAS_29_BLACK_PEACH_2", 0x00000011,
    (u32)"BTLNO_LAS_01_01", 0x00000012, (u32)"BTLNO_LAS_01_02", 0x00000013,
    (u32)"BTLNO_LAS_01_03", 0x00000014, (u32)"BTLNO_LAS_02_01", 0x00000015,
    (u32)"BTLNO_LAS_02_02", 0x00000016, (u32)"BTLNO_LAS_02_03", 0x00000017,
    (u32)"BTLNO_LAS_02_04", 0x00000018, (u32)"BTLNO_LAS_02_05", 0x00000019,
    (u32)"BTLNO_LAS_02_06", 0x0000001A, (u32)"BTLNO_LAS_02_99", 0x0000001B,
    (u32)"BTLNO_LAS_03_01", 0x0000001C, (u32)"BTLNO_LAS_03_02", 0x0000001D,
    (u32)"BTLNO_LAS_03_03", 0x0000001E, (u32)"BTLNO_LAS_04_01", 0x0000001F,
    (u32)"BTLNO_LAS_06_01", 0x00000020, (u32)"BTLNO_LAS_06_02", 0x00000021,
    (u32)"BTLNO_LAS_06_03", 0x00000022, (u32)"BTLNO_LAS_06_04", 0x00000023,
    (u32)"BTLNO_LAS_06_05", 0x00000024, (u32)"BTLNO_LAS_06_06", 0x00000025,
    (u32)"BTLNO_LAS_06_99", 0x00000026, (u32)"BTLNO_LAS_07_01", 0x00000027,
    (u32)"BTLNO_LAS_07_02", 0x00000028, (u32)"BTLNO_LAS_07_03", 0x00000029,
    (u32)"BTLNO_LAS_09_01", 0x0000002A, (u32)"BTLNO_LAS_09_02", 0x0000002B,
    (u32)"BTLNO_LAS_19_01", 0x0000002C, (u32)"BTLNO_LAS_19_02", 0x0000002D,
    (u32)"BTLNO_LAS_19_03", 0x0000002E, (u32)"BTLNO_LAS_20_01", 0x0000002F,
    (u32)"BTLNO_LAS_21_01", 0x00000030, (u32)"BTLNO_LAS_21_02", 0x00000031,
    (u32)"BTLNO_LAS_22_01", 0x00000032, (u32)"BTLNO_LAS_22_02", 0x00000033,
    (u32)"BTLNO_LAS_22_03", 0x00000034, (u32)"BTLNO_LAS_24_01", 0x00000035,
    (u32)"BTLNO_LAS_25_01", 0x00000036, (u32)"BTLNO_LAS_25_02", 0x00000037,
    (u32)"BTLNO_LAS_25_03", 0x00000038, (u32)"BTLNO_LAS_25_04", 0x00000039,
    (u32)"BTLNO_LAS_25_05", 0x0000003A, (u32)"BTLNO_LAS_25_06", 0x0000003B,
    0x00000000, 0x00000000,
};

USER_FUNC(_gear_rotate) {
    s32* args = event->args;
    const char* name1 = (const char*)evtGetValue(event, args[0]);
    const char* name2 = (const char*)evtGetValue(event, args[1]);
    s32 index = args[2];
    s32 start = evtGetValue(event, index);
    s32 end = evtGetValue(event, args[3]);
    s32 delta = evtGetValue(event, args[4]);
    BattleStageObject* obj1 = BattleSearchObjectPtr(name1);
    BattleStageObject* obj2 = BattleSearchObjectPtr(name2);
    f32 angle = 360.0f * (f32)start / (f32)end;
    s32 next;

    obj1->mRotation.z = angle;
    obj2->mRotation.z = angle;
    next = start + delta;
    if (next >= end) {
        next = 0;
    }
    if (next < 0) {
        next = end - 1;
    }
    evtSetValue(event, index, next);
    return EVT_RETURN_DONE;
}

EVT_BEGIN(bgset_las_gear_event)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 0)
    SET_VALUE(LW(2), 0)
    LABEL(0)
    CALL(_gear_rotate, STRING("gear_01a"), STRING("gear_01b"), LW(0), 400, -1)
    CALL(_gear_rotate, STRING("gear_02a"), STRING("gear_02b"), LW(1), 600, 1)
    CALL(_gear_rotate, STRING("gear_03a"), STRING("gear_03b"), LW(2), 800, 1)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()

USER_FUNC(check_battle_end) {
    return battleSeqEndCheck() ? EVT_RETURN_DONE : 0;
}

EVT_BEGIN(stg_08_1_init_sub_event)
    CALL(evt_mapobj_get_position, STRING("hi1"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("_hi1"), STRING("fire"), 3, LW(0), LW(1), LW(2), FLOAT(1.0f), 1000, 0, 0, 0, 0, 0, 0)
    CALL(evt_mapobj_get_position, STRING("hi2"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("_hi2"), STRING("fire"), 3, LW(0), LW(1), LW(2), FLOAT(1.0f), 1000, 0, 0, 0, 0, 0, 0)
    CALL(evt_mapobj_get_position, STRING("hi3"), LW(0), LW(1), LW(2))
    CALL(evt_eff, STRING("_hi3"), STRING("fire"), 3, LW(0), LW(1), LW(2), FLOAT(1.0f), 1000, 0, 0, 0, 0, 0, 0)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi1"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi2"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi3"), 1)
    CALL(check_battle_end)
    CALL(evt_eff_softdelete, STRING("_hi1"))
    CALL(evt_eff_softdelete, STRING("_hi2"))
    CALL(evt_eff_softdelete, STRING("_hi3"))
    RETURN
EVT_END()

EVT_BEGIN(stg_08_1_init_event)
    SPAWN_EVENT(stg_08_1_init_sub_event)
    RETURN
EVT_END()

USER_FUNC(unk_las_00007774) {
    Vec pos;
    const char* names[6] = {"hi1", "hi2", "hi3", "hi4", "hi5", "hi6"};
    s32 i;

    if (isFirstCall) {
        unk_las_bss00000098[0] = 0;
        for (i = 0; i < 6; i++) {
            unk_las_bss0000009c[i] = effFireEntry(8, 1000, 0.0f, -1000.0f, 0.0f, 1.0f);
        }
    }

    if (unk_las_bss00000098[0]) {
        for (i = 0; i < 6; i++) {
            effSoftDelete(unk_las_bss0000009c[i]);
        }
        return EVT_RETURN_DONE;
    }

    for (i = 0; i < 6; i++) {
        mapObjGetPos(names[i], &pos);
        if (pos.y > 0.0f) {
            effFireData* fire = unk_las_bss0000009c[i]->userdata;
            fire->position.x = pos.x;
            fire->position.y = pos.y - 15.0f;
            fire->position.z = pos.z;
        }
    }
    return 0;
}

USER_FUNC(unk_las_0000775c) {
    unk_las_bss00000098[0] = 1;
    return EVT_RETURN_DONE;
}
EVT_BEGIN(stg_08_6_init_sub_event)
    BEGIN_SIBLING_EVENT
        CALL(unk_las_00007774)
    END_SIBLING_EVENT
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi1"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi2"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi3"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi4"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi5"), 1)
    CALL(evt_mapobj_flag_onoff, 0, 1, STRING("hi6"), 1)
    CALL(check_battle_end)
    CALL(unk_las_0000775c)
    RETURN
EVT_END()

EVT_BEGIN(stg_08_6_init_event)
    SPAWN_EVENT(stg_08_6_init_sub_event)
    RETURN
EVT_END()
// clang-format on
