#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_item_purple_teresa[];
extern s32 unit_purple_teresa_22_data_E620[];

// clang-format off
extend_data(24,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(25,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 pik_btlobject_data[12] = {
    (u32)"C", 0x00020003, 0x00000000, 0x00000000,
    0x00000000, 0x003C0000, (u32)"A2", 0x00020001,
    0x00000000, 0x00000000, 0xC2940000, 0x003C0000,
};

// TODO: type
u32 pik_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_06_4", 0x00000002, (u32)&pik_btlobject_data,
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

// TODO: type
u32 btlparty_pik_pik_06_01_off_1[24] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_01_off_1[8] = {
    0x00000002, (u32)&btlparty_pik_pik_06_01_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_02_off_1[36] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_02_off_1[8] = {
    0x00000003, (u32)&btlparty_pik_pik_06_02_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_03_off_1[12] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_03_off_1[8] = {
    0x00000001, (u32)&btlparty_pik_pik_06_03_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_04_off_1[24] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_04_off_1[8] = {
    0x00000002, (u32)&btlparty_pik_pik_06_04_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_05_off_1[24] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_05_off_1[8] = {
    0x00000002, (u32)&btlparty_pik_pik_06_05_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_06_off_1[48] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_06_off_1[8] = {
    0x00000004, (u32)&btlparty_pik_pik_06_06_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_07_off_1[12] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_07_off_1[8] = {
    0x00000001, (u32)&btlparty_pik_pik_06_07_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_08_off_1[36] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42480000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42B40000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x43020000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_08_off_1[8] = {
    0x00000003, (u32)&btlparty_pik_pik_06_08_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlparty_pik_pik_06_09_off_1[48] = {
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x41F00000,
    0x00000000, 0xC1200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x428C0000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x42DC0000,
    0x00000000, 0x41200000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
    (u32)unit_purple_teresa_22_data_E620, 0x01000000, 0x04000004, 0x43160000,
    0x00000000, 0x41A00000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_purple_teresa,
};

// TODO: type
u32 btlgrp_pik_pik_06_09_off_1[8] = {
    0x00000004, (u32)&btlparty_pik_pik_06_09_off_1, 0x00000019, 0x00000019,
    0x00000032, (u32)battle_heart_drop_param_default, (u32)battle_flower_drop_param_default2, 0x00000005,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_test1_off[6] = {
    0x00000032, (u32)&btlgrp_pik_pik_06_01_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_01_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_01_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_02_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_02_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_03_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_03_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_04_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_04_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_05_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_05_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_06_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_06_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_07_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_07_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_08_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_08_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_pik_btlno_pik_03_09_off[6] = {
    0x00000014, (u32)&btlgrp_pik_pik_06_09_off_1, (u32)&pik_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_pik_tbl[187] = {
    (u32)"マップstg_06_4", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_pik_btlno_pik_test1_off, 0x00000000, 0x00000000,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    0x01010101, 0x01010101, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_01_off, 0x00000000,
    0x00000000, 0x03030303, 0x0A0A0303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_02_off,
    0x00000000, 0x00000000, 0x03030303, 0x0A0A0303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_pik_btlno_pik_03_03_off, 0x00000000, 0x00000000, 0x03030303,
    0x0A0A0303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"pik_03パープルテレサ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_04_off, 0x00000000, 0x00000000,
    0x03030303, 0x0A0A0303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_05_off, 0x00000000,
    0x00000000, 0x03030303, 0x0A0A0303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ", 0x00000000,
    0x00000000, 0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_06_off,
    0x00000000, 0x00000000, 0x03030303, 0x0A0A0303,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ",
    0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    (u32)&btlgrpsel_pik_btlno_pik_03_07_off, 0x00000000, 0x00000000, 0x03030303,
    0x0A0A0303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x01010101, 0x01010101, (u32)"BGM_ZAKO_BATTLE1",
    (u32)"pik_03パープルテレサ", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_08_off, 0x00000000, 0x00000000,
    0x03030303, 0x0A0A0303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"pik_03パープルテレサ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_pik_btlno_pik_03_09_off, 0x00000000,
    0x00000000, 0x03030303, 0x0A0A0303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_ZAKO_BATTLE1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btl_pik_setup_no_tbl[22] = {
    (u32)"BTLNO_PIK_TEST1", 0x00000000, (u32)"BTLNO_PIK_03_01", 0x00000001,
    (u32)"BTLNO_PIK_03_02", 0x00000002, (u32)"BTLNO_PIK_03_03", 0x00000003,
    (u32)"BTLNO_PIK_03_04", 0x00000004, (u32)"BTLNO_PIK_03_05", 0x00000005,
    (u32)"BTLNO_PIK_03_06", 0x00000006, (u32)"BTLNO_PIK_03_07", 0x00000007,
    (u32)"BTLNO_PIK_03_08", 0x00000008, (u32)"BTLNO_PIK_03_09", 0x00000009,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
