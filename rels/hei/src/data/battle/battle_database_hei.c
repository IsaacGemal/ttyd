#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_heart_drop_param_default[];
extern ItemDropData battle_item_chorobon[9];
extern ItemDropData battle_item_gold_chorobon[1];
extern ItemDropData battle_item_kuriboo[10];
extern ItemDropData battle_item_monochrome_sinemon[8];
extern ItemDropData battle_item_nokonoko[10];
extern ItemDropData battle_item_patakuri[10];
extern ItemDropData battle_item_sinnosuke[1];
extern ItemDropData battle_item_togedaruma[1];
extern ItemDropData battle_item_togekuri[10];
extern BattleUnitKind unit_chorobon_12_data_E9A0;
extern BattleUnitKind unit_gold_chorobon;
extern BattleUnitKind unit_kuriboo_12_data_11CF0;
extern BattleUnitKind unit_monochrome_sinemon_12_data_1B9C0;
extern BattleUnitKind unit_nokonoko_12_data_12888;
extern BattleUnitKind unit_patakuri_12_data_13CD0;
extern BattleUnitKind unit_sinnosuke;
extern BattleUnitKind unit_togedaruma_12_data_1A788;
extern BattleUnitKind unit_togekuri_12_data_17668;

// clang-format off
extend_data(64,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(65,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 hei_btlobject_data[354] = {
    (u32)"ki_1", 0x00020000, 0x43080000, 0x00000000,
    0xC27C0000, 0x003C0000, (u32)"ki_2", 0x00020000,
    0x42AC0000, 0x00000000, 0xC1F00000, 0x0A320000,
    (u32)"ki_3", 0x00020000, 0x42200000, 0x00000000,
    0xC2480000, 0x14280000, (u32)"kusa_1", 0x00020001,
    0xC3160000, 0x00000000, 0xC1F00000, 0x003C0000,
    (u32)"kusa_2", 0x00020001, 0x43380000, 0x00000000,
    0x42820000, 0x0A320000, (u32)"kusa_3", 0x00020001,
    0xC3250000, 0x00000000, 0x42440000, 0x14280000,
    (u32)"hana_1", 0x00020001, 0x42C80000, 0x00000000,
    0x42980000, 0x1E1E0000, (u32)"hana_2", 0x00020001,
    0xC2300000, 0x00000000, 0x42940000, 0x003C0000,
    (u32)"saku_1", 0x00020001, 0xC1C00000, 0x00000000,
    0xC21C0000, 0x0A320000, (u32)"saku_2", 0x00020001,
    0x432C0000, 0x00000000, 0xC2040000, 0x14280000,
    (u32)"saku_3", 0x00020001, 0xC2C60000, 0x00000000,
    0xC28C0000, 0x1E1E0000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0x43030000, 0x00000000, 0xC2000000, 0x003C0000,
    (u32)"kusa_1", 0x00020001, 0x41D80000, 0x00000000,
    0xC2200000, 0x003C0000, (u32)"gareki_1_1", 0x00020001,
    0x42FE0000, 0x00000000, 0xC2600000, 0x0A320000,
    (u32)"gareki_1_2", 0x00020001, 0xC2500000, 0x00000000,
    0xC22C0000, 0x14280000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0xC2F40000, 0x00000000, 0xC2240000, 0x003C0000,
    (u32)"toride_1", 0x00020000, 0x42BA0000, 0x00000000,
    0xC25C0000, 0x0A320000, (u32)"kusa_1", 0x00020001,
    0xC2920000, 0x00000000, 0xC1F80000, 0x003C0000,
    (u32)"gareki_1_1", 0x00020001, 0xC3710000, 0x00000000,
    0xC2300000, 0x0A320000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0x43030000, 0x00000000, 0xC2000000, 0x003C0000,
    (u32)"kusa_1", 0x00020001, 0x41D80000, 0x00000000,
    0xC2200000, 0x003C0000, (u32)"gareki_2_1", 0x00020001,
    0x42FE0000, 0x00000000, 0xC2600000, 0x0A320000,
    (u32)"gareki_2_2", 0x00020001, 0xC2500000, 0x00000000,
    0xC22C0000, 0x14280000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0xC2F40000, 0x00000000, 0xC2240000, 0x003C0000,
    (u32)"toride_2", 0x00020000, 0x42BA0000, 0x00000000,
    0xC25C0000, 0x0A320000, (u32)"kusa_1", 0x00020001,
    0xC3430000, 0x00000000, 0xC1300000, 0x003C0000,
    (u32)"gareki_2_1", 0x00020001, 0xC3898000, 0x00000000,
    0xC2380000, 0x0A320000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0x43030000, 0x00000000, 0xC2000000, 0x003C0000,
    (u32)"kusa_1", 0x00020001, 0x41D80000, 0x00000000,
    0xC2200000, 0x003C0000, (u32)"gareki_3_1", 0x00020001,
    0x42FE0000, 0x00000000, 0xC2600000, 0x0A320000,
    (u32)"gareki_3_2", 0x00020001, 0xC2500000, 0x00000000,
    0xC22C0000, 0x14280000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"ki_1", 0x00020000,
    0xC2F40000, 0x00000000, 0xC2240000, 0x003C0000,
    (u32)"toride_3", 0x00020000, 0x42BA0000, 0x00000000,
    0xC25C0000, 0x0A320000, (u32)"kusa_1", 0x00020001,
    0xC2AA0000, 0x00000000, 0xC1D80000, 0x003C0000,
    (u32)"gareki_3_1", 0x00020001, 0xC3700000, 0x00000000,
    0xC2400000, 0x0A320000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"S_dai_01_3", 0x00020000,
    0xC2FE0000, 0x00000000, 0xC2540000, 0x003C0000,
    (u32)"S_dai_02_3", 0x00020000, 0x42FE0000, 0x00000000,
    0xC2540000, 0x0A320000, (u32)"C_hei_03", 0x00020003,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"S_dai_01_5", 0x00020000, 0xC2FE0000, 0x00000000,
    0xC2540000, 0x003C0000, (u32)"S_dai_02_5", 0x00020000,
    0x42FE0000, 0x00000000, 0xC2540000, 0x0A320000,
    (u32)"C_hei_05", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"B_hei_07", 0x00020000,
    0x42BE0000, 0x00000000, 0xC2500000, 0x003C0000,
    (u32)"C_hei_07", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"B", 0x00020002,
    0x00000000, 0x00000000, 0x00000000, 0x003C0000,
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000,
};

// TODO: type
u32 hei_btlstage_data[1199] = {
    (u32)"bti_01", (u32)"stg_01_0", 0x0000000D, (u32)&hei_btlobject_data,
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
    0x00000000, (u32)"bti_01", (u32)"stg_01_4", 0x00000006,
    (u32)((u8*)&hei_btlobject_data + 0x138), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (u32)weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0210006F, 0x20000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_01_4",
    0x00000006, (u32)((u8*)&hei_btlobject_data + 0x1c8), 0x00000000, 0x00000000,
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
    (u32)"stg_01_4", 0x00000006, (u32)((u8*)&hei_btlobject_data + 0x258), 0x00000000,
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
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"bti_01", (u32)"stg_01_4", 0x00000006, (u32)((u8*)&hei_btlobject_data + 0x2e8),
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
    0x00000000, (u32)"bti_01", (u32)"stg_01_4", 0x00000006,
    (u32)((u8*)&hei_btlobject_data + 0x378), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (u32)weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0210006F, 0x20000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_01_4",
    0x00000006, (u32)((u8*)&hei_btlobject_data + 0x408), 0x00000000, 0x00000000,
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
    (u32)"stg_01_1", 0x00000003, (u32)((u8*)&hei_btlobject_data + 0x498), 0x00000000,
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
    (u32)"bti_01", (u32)"stg_01_1", 0x00000003, (u32)((u8*)&hei_btlobject_data + 0x4e0),
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
    0x00000000, (u32)"bti_01", (u32)"stg_01_1", 0x00000002,
    (u32)((u8*)&hei_btlobject_data + 0x528), 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (u32)weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0210006F, 0x20000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_01_2",
    0x00000002, (u32)((u8*)&hei_btlobject_data + 0x558), 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, 0x00000000,
};

BattleUnitSetup btlparty_hei_hei_01_01_off_1[4] = {
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_hei_hei_01_01_off_1 = { 4, btlparty_hei_hei_01_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_02_02_off_1[2] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_02_02_off_1 = { 2, btlparty_hei_hei_02_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_02_03_off_1[2] = {
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_hei_hei_02_03_off_1 = { 2, btlparty_hei_hei_02_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_02_04_off_1[2] = {
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_02_04_off_1 = { 2, btlparty_hei_hei_02_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_03_01_off_1[2] = {
    { &unit_sinnosuke, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinnosuke },
    { &unit_sinnosuke, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinnosuke },
};

BattleGroupSetup btlgrp_hei_hei_03_01_off_1 = { 2, btlparty_hei_hei_03_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_04_01_off_1[3] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_hei_hei_04_01_off_1 = { 3, btlparty_hei_hei_04_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_04_01_off_2[2] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_04_01_off_2 = { 2, btlparty_hei_hei_04_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_04_02_off_1[2] = {
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_04_02_off_1 = { 2, btlparty_hei_hei_04_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_04_02_off_2[3] = {
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_04_02_off_2 = { 3, btlparty_hei_hei_04_02_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_05_01_off_1[2] = {
    { &unit_togedaruma_12_data_1A788, 1, 0x04000004, { 80.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togedaruma },
    { &unit_togedaruma_12_data_1A788, 1, 0x04000004, { 150.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togedaruma },
};

BattleGroupSetup btlgrp_hei_hei_05_01_off_1 = { 2, btlparty_hei_hei_05_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_06_02_off_1[2] = {
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_hei_hei_06_02_off_1 = { 2, btlparty_hei_hei_06_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_06_03_off_1[3] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_06_03_off_1 = { 3, btlparty_hei_hei_06_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_06_03_off_2[3] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_nokonoko_12_data_12888, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_nokonoko },
};

BattleGroupSetup btlgrp_hei_hei_06_03_off_2 = { 3, btlparty_hei_hei_06_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_07_01_off_1[4] = {
    { &unit_monochrome_sinemon_12_data_1B9C0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_sinemon_12_data_1B9C0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_sinemon_12_data_1B9C0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
    { &unit_monochrome_sinemon_12_data_1B9C0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_sinemon },
};

BattleGroupSetup btlgrp_hei_hei_07_01_off_1 = { 4, btlparty_hei_hei_07_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_08_01_off_1[4] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_08_01_off_1 = { 4, btlparty_hei_hei_08_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_09_01_off_1[2] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_09_01_off_1 = { 2, btlparty_hei_hei_09_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_09_01_off_2[3] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_09_01_off_2 = { 3, btlparty_hei_hei_09_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_09_03_off_1[2] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_09_03_off_1 = { 2, btlparty_hei_hei_09_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_09_03_off_2[3] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_09_03_off_2 = { 3, btlparty_hei_hei_09_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_10_01_off_1[1] = {
    { &unit_gold_chorobon, 1, 0x04000004, { 125.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_gold_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_10_01_off_1 = { 1, btlparty_hei_hei_10_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_11_01_off_1[2] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_11_01_off_1 = { 2, btlparty_hei_hei_11_01_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_11_01_off_2[3] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_11_01_off_2 = { 3, btlparty_hei_hei_11_01_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_11_03_off_1[2] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_11_03_off_1 = { 2, btlparty_hei_hei_11_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_11_03_off_2[3] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_11_03_off_2 = { 3, btlparty_hei_hei_11_03_off_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_12_01_off_1[4] = {
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_12_data_E9A0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_hei_hei_12_01_off_1 = { 4, btlparty_hei_hei_12_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_02_off_1[2] = {
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_hei_hei_13_02_off_1 = { 2, btlparty_hei_hei_13_02_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_02_on_1[3] = {
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_hei_hei_13_02_on_1 = { 3, btlparty_hei_hei_13_02_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_02_on_2[4] = {
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_hei_hei_13_02_on_2 = { 4, btlparty_hei_hei_13_02_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_02_on_3[5] = {
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
    { &unit_kuriboo_12_data_11CF0, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_kuriboo },
};

BattleGroupSetup btlgrp_hei_hei_13_02_on_3 = { 5, btlparty_hei_hei_13_02_on_3, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_03_off_1[2] = {
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_hei_hei_13_03_off_1 = { 2, btlparty_hei_hei_13_03_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_03_on_1[2] = {
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_hei_hei_13_03_on_1 = { 2, btlparty_hei_hei_13_03_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_03_on_2[3] = {
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
    { &unit_togekuri_12_data_17668, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togekuri },
};

BattleGroupSetup btlgrp_hei_hei_13_03_on_2 = { 3, btlparty_hei_hei_13_03_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_04_off_1[2] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_hei_hei_13_04_off_1 = { 2, btlparty_hei_hei_13_04_off_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_04_on_1[2] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_hei_hei_13_04_on_1 = { 2, btlparty_hei_hei_13_04_on_1, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_hei_hei_13_04_on_2[4] = {
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
    { &unit_patakuri_12_data_13CD0, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patakuri },
};

BattleGroupSetup btlgrp_hei_hei_13_04_on_2 = { 4, btlparty_hei_hei_13_04_on_2, 25, 25, 50, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_test1_off[2] = {
    { 50, &btlgrp_hei_hei_01_01_off_1, (struct BattleStageData*)hei_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_test2_off[2] = {
    { 50, &btlgrp_hei_hei_01_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xBEC) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_test3_off[2] = {
    { 50, &btlgrp_hei_hei_01_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_test4_off[2] = {
    { 50, &btlgrp_hei_hei_01_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_01_01_off[2] = {
    { 10, &btlgrp_hei_hei_01_01_off_1, (struct BattleStageData*)hei_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_02_02_off[2] = {
    { 10, &btlgrp_hei_hei_02_02_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_02_03_off[2] = {
    { 10, &btlgrp_hei_hei_02_03_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_02_04_off[2] = {
    { 10, &btlgrp_hei_hei_02_04_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_03_01_off[2] = {
    { 10, &btlgrp_hei_hei_03_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xBEC) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_04_01_off[3] = {
    { 10, &btlgrp_hei_hei_04_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x6D0) },
    { 10, &btlgrp_hei_hei_04_01_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x6D0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_04_02_off[3] = {
    { 10, &btlgrp_hei_hei_04_02_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x51C) },
    { 10, &btlgrp_hei_hei_04_02_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_05_01_off[2] = {
    { 10, &btlgrp_hei_hei_05_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xDA0) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_06_02_off[2] = {
    { 10, &btlgrp_hei_hei_06_02_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x884) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_06_03_off[3] = {
    { 10, &btlgrp_hei_hei_06_03_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xA38) },
    { 10, &btlgrp_hei_hei_06_03_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xA38) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_07_01_off[2] = {
    { 10, &btlgrp_hei_hei_07_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0xF54) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_08_01_off[2] = {
    { 10, &btlgrp_hei_hei_08_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry R_btlgrpsel_hei_btlno_hei_08_02_off[2] = {
    { 10, &btlgrp_hei_hei_08_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_09_01_off[3] = {
    { 10, &btlgrp_hei_hei_09_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 10, &btlgrp_hei_hei_09_01_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_09_03_off[3] = {
    { 10, &btlgrp_hei_hei_09_03_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 10, &btlgrp_hei_hei_09_03_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_10_01_off[2] = {
    { 10, &btlgrp_hei_hei_10_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_11_01_off[3] = {
    { 10, &btlgrp_hei_hei_11_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 10, &btlgrp_hei_hei_11_01_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_11_03_off[3] = {
    { 10, &btlgrp_hei_hei_11_03_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 10, &btlgrp_hei_hei_11_03_off_2, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_12_01_off[2] = {
    { 10, &btlgrp_hei_hei_12_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry R_btlgrpsel_hei_btlno_hei_12_02_off[2] = {
    { 10, &btlgrp_hei_hei_12_01_off_1, (struct BattleStageData*)(void*)((u8*)hei_btlstage_data + 0x1108) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_13_02_off[5] = {
    { 10, &btlgrp_hei_hei_13_02_on_1, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_02_on_2, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_02_on_3, (struct BattleStageData*)hei_btlstage_data },
    { 50, &btlgrp_hei_hei_13_02_off_1, (struct BattleStageData*)hei_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_13_03_off[4] = {
    { 50, &btlgrp_hei_hei_13_03_on_1, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_03_on_2, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_03_off_1, (struct BattleStageData*)hei_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_hei_btlno_hei_13_04_off[4] = {
    { 50, &btlgrp_hei_hei_13_04_on_1, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_04_on_2, (struct BattleStageData*)hei_btlstage_data },
    { 10, &btlgrp_hei_hei_13_04_off_1, (struct BattleStageData*)hei_btlstage_data },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_hei_tbl[28] = {
    {
        "マップstg_01_0",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_test1_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_01_1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_test2_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_01_2",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_test3_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マップstg_01_4",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_test4_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei01クリボー",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_01_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei02パタクリ２",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_02_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei02ノコノコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_02_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei02トゲクリ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_02_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei03シンノスケ",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_03_01_off,                                                                  // offLoadouts
        536870927,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                               // music
    },
    {
        "hei04パタクリ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_04_01_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei04ノコノコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_04_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei05トゲダルマー",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_05_01_off,                                                                  // offLoadouts
        536870927,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                               // music
    },
    {
        "hei06ノコノコ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_06_02_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 10, 10, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei06パタクリ２",                                                                                         // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_06_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei07モノクロシンエモン",                                                                                   // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_07_01_off,                                                                  // offLoadouts
        536870943,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                               // music
    },
    {
        "hei08チョロボン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_08_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei08チョロボン２",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        R_btlgrpsel_hei_btlno_hei_08_02_off,                                                                // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei09チョロボン1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_09_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei09チョロボン3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_09_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei10ゴールドチョロボン",                                                                                   // name
        "hei_10",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_10_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_KOBOSS_BATTLE1",                                                                               // music
    },
    {
        "hei11チョロボン1",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_11_01_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei11チョロボン3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_11_03_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei12チョロボン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_12_01_off,                                                                  // offLoadouts
        536870928,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei12チョロボン２",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        R_btlgrpsel_hei_btlno_hei_12_02_off,                                                                // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei13クリボー2",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_hei_btlno_hei_13_02_off,                                                                  // offLoadouts
        0,                                                                                                  // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "hei13トゲクリ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_13_03_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "hei13パタクリ",                                                                                          // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_hei_btlno_hei_13_04_off,                                                                    // offLoadouts
        0,                                                                                                    // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 }, // field_0x20
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
u32 btl_hei_setup_no_tbl[56] = {
    (u32)"BTLNO_HEI_TEST1", 0x00000000, (u32)"BTLNO_HEI_TEST2", 0x00000001,
    (u32)"BTLNO_HEI_TEST3", 0x00000002, (u32)"BTLNO_HEI_TEST4", 0x00000003,
    (u32)"BTLNO_HEI_01_01", 0x00000004, (u32)"BTLNO_HEI_02_02", 0x00000005,
    (u32)"BTLNO_HEI_02_03", 0x00000006, (u32)"BTLNO_HEI_02_04", 0x00000007,
    (u32)"BTLNO_HEI_03_01", 0x00000008, (u32)"BTLNO_HEI_04_01", 0x00000009,
    (u32)"BTLNO_HEI_04_02", 0x0000000A, (u32)"BTLNO_HEI_05_01", 0x0000000B,
    (u32)"BTLNO_HEI_06_02", 0x0000000C, (u32)"BTLNO_HEI_06_03", 0x0000000D,
    (u32)"BTLNO_HEI_07_01", 0x0000000E, (u32)"BTLNO_HEI_08_01", 0x0000000F,
    (u32)"BTLNO_HEI_08_02", 0x00000010, (u32)"BTLNO_HEI_09_01", 0x00000011,
    (u32)"BTLNO_HEI_09_03", 0x00000012, (u32)"BTLNO_HEI_10_01", 0x00000013,
    (u32)"BTLNO_HEI_11_01", 0x00000014, (u32)"BTLNO_HEI_11_03", 0x00000015,
    (u32)"BTLNO_HEI_12_01", 0x00000016, (u32)"BTLNO_HEI_12_02", 0x00000017,
    (u32)"BTLNO_HEI_13_02", 0x00000018, (u32)"BTLNO_HEI_13_03", 0x00000019,
    (u32)"BTLNO_HEI_13_04", 0x0000001A, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
