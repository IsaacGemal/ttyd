#include "battle/battle_weapon_power.h"
#include "battle/battle_stage_object.h"
#include "driver/npcdrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_heart_drop_param_default[];
extern s32 unit_act_mario[];

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData nok_btlobject_data[10] = {
    { "ki_1", 0x0002, 0x0000, { -100.0f, 0.0f, -30.0f }, 0x00, 0x3C },
    { "isi_A", 0x0002, 0x0000, { 150.0f, 0.0f, -30.0f }, 0x0A, 0x32 },
    { "isi_B", 0x0002, 0x0000, { 50.0f, 0.0f, -34.0f }, 0x14, 0x28 },
    { "isigaki", 0x0002, 0x0001, { 2.0f, 0.0f, -23.0f }, 0x00, 0x3C },
    { "kusa_1", 0x0002, 0x0001, { -184.0f, 0.0f, 37.0f }, 0x0A, 0x32 },
    { "kusa_2", 0x0002, 0x0001, { 180.0f, 0.0f, 60.0f }, 0x14, 0x28 },
    { "kusa_3", 0x0002, 0x0001, { 70.0f, 0.0f, 65.0f }, 0x1E, 0x1E },
    { "kusa_4", 0x0002, 0x0001, { -87.0f, 0.0f, 56.0f }, 0x00, 0x3C },
    { "B", 0x0002, 0x0002, { 0.0f, 0.0f, 0.0f }, 0x00, 0x3C },
    { "C", 0x0002, 0x0003, { 0.0f, 0.0f, 0.0f }, 0x00, 0x3C },
};

// TODO: type
u32 nok_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_04_0", 0x0000000A, (u32)&nok_btlobject_data,
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
    0x00000000,
};

// BattleUnitSetup only names mUnitKindParams (0x0), field_1C (0x1C) and field_2C (0x2C); the original also
// stores values at 0x4 (0x01000000), 0x8 (0x04000004) and 0xC (spawn x = 90.0), which fall inside the
// header's field_4[] padding. Written out byte-for-byte until the header names them (same idiom as
// src/battle/battle_party.c).
// clang-format off
BattleUnitSetup btlparty_nok_hei_ep[1] = {
    { (BattleUnitKind*)unit_act_mario,
      { 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04,
        0x42, 0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0, 0, 0, 0 }, { 0x00, 0x00, 0x00, 0x00 } },
};
// clang-format on

// TODO: type
u32 btlgrp_nok_hei_ep[8] = {
    0x00000001, (u32)&btlparty_nok_hei_ep, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// The weighted setup list npcSetupBattleInfo() walks: entries are { weight, groupSetup, stageData }, and the
// zero-weight entry terminates the list.
NpcBattleSetupEntry btlgrpsel_nok_btlno_hei_test1_off[2] = {
    { 0x32, (BattleGroupSetup*)&btlgrp_nok_hei_ep, (s32)&nok_btlstage_data },
    { 0x00, NULL, 0x00000000 },
};

// TODO: type
u32 btlsetup_nok_tbl[34] = {
    (u32)"マップstg_01_0", (u32)"nok_01", 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_nok_btlno_hei_test1_off, 0x00100000, 0x00000006,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btl_nok_setup_no_tbl[4] = {
    (u32)"BTLNO_HEI_TEST1", 0x00000000, 0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
