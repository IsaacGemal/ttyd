#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "battle/battle_stage_object.h"
#include "battle/battle_unit.h"
#include "driver/npcdrv.h"

#pragma force_active on
extern s32 battle_flower_drop_param_default[];
extern s32 battle_flower_drop_param_default2[];
extern s32 battle_flower_drop_param_default3[];
extern s32 battle_flower_drop_param_default4[];
extern s32 battle_flower_drop_param_default5[];
extern s32 battle_heart_drop_param_default[];
extern s32 battle_heart_drop_param_default2[];
extern s32 battle_heart_drop_param_default3[];
extern s32 battle_heart_drop_param_default4[];
extern s32 battle_heart_drop_param_default5[];
extern ItemDropData battle_item_badge_borodo[10];
extern ItemDropData battle_item_basabasa_chururu[10];
extern ItemDropData battle_item_basabasa_green[10];
extern ItemDropData battle_item_bllizard[9];
extern ItemDropData battle_item_bomhei[10];
extern ItemDropData battle_item_borodo[10];
extern ItemDropData battle_item_bubble[8];
extern ItemDropData battle_item_burst_wanwan[7];
extern ItemDropData battle_item_chorobon[9];
extern ItemDropData battle_item_churantalar[6];
extern ItemDropData battle_item_dark_keeper[10];
extern ItemDropData battle_item_dokugassun[9];
extern ItemDropData battle_item_flower_chorobon[9];
extern ItemDropData battle_item_giant_bomb[9];
extern ItemDropData battle_item_hannya[10];
extern ItemDropData battle_item_heavy_bom[9];
extern ItemDropData battle_item_hennya[10];
extern ItemDropData battle_item_hinnya[10];
extern ItemDropData battle_item_honenoko[9];
extern ItemDropData battle_item_hyper_jyugem[10];
extern ItemDropData battle_item_hyper_sinemon[9];
extern ItemDropData battle_item_ice_pakkun[5];
extern ItemDropData battle_item_jyugem[10];
extern ItemDropData battle_item_karon[9];
extern ItemDropData battle_item_mahorn[9];
extern ItemDropData battle_item_mahorn_custom[9];
extern ItemDropData battle_item_monochrome_kurokumorn[9];
extern ItemDropData battle_item_pakkun_flower[7];
extern ItemDropData battle_item_patamet[10];
extern ItemDropData battle_item_patatogemet[10];
extern ItemDropData battle_item_phantom[8];
extern ItemDropData battle_item_piders[4];
extern ItemDropData battle_item_purple_teresa[10];
extern ItemDropData battle_item_sambo[7];
extern ItemDropData battle_item_sambo_mummy[8];
extern ItemDropData battle_item_sinemon[8];
extern ItemDropData battle_item_super_mahorn[9];
extern ItemDropData battle_item_teresa[10];
extern ItemDropData battle_item_togenoko_ace[10];
extern ItemDropData battle_item_twinkling_pansy[10];
extern ItemDropData battle_item_ura_noko[10];
extern ItemDropData battle_item_wanwan[7];
extern ItemDropData battle_item_yami_kuriboo[10];
extern ItemDropData battle_item_yami_noko[10];
extern ItemDropData battle_item_yami_patakuri[10];
extern ItemDropData battle_item_yami_togekuri[10];
extern ItemDropData battle_item_yamitogedaruma[5];
extern ItemDropData battle_item_zonbaba[1];
extern s32 unit_badge_borodo[];
extern BattleUnitKind unit_basabasa_chururu_15_data_62428;
extern BattleUnitKind unit_basabasa_green;
extern BattleUnitKind unit_bllizard_15_data_60E68;
extern BattleUnitKind unit_bomhei_15_data_24850;
extern BattleUnitKind unit_borodo_15_data_211F8;
extern BattleUnitKind unit_boss_zonbaba;
extern BattleUnitKind unit_bubble_15_data_35518;
extern BattleUnitKind unit_burst_wanwan_15_data_5F390;
extern BattleUnitKind unit_chorobon_15_data_127B8;
extern BattleUnitKind unit_churantalar;
extern BattleUnitKind unit_dark_keeper_15_data_5E0F0;
extern BattleUnitKind unit_dokugassun_15_data_5CC90;
extern BattleUnitKind unit_flower_chorobon_15_data_2FE08;
extern BattleUnitKind unit_giant_bomb;
extern BattleUnitKind unit_hannya_15_data_13DF0;
extern BattleUnitKind unit_heavy_bom_15_data_33E08;
extern BattleUnitKind unit_hennya_15_data_5A9C0;
extern BattleUnitKind unit_hinnya_15_data_149B0;
extern BattleUnitKind unit_honenoko_15_data_15568;
extern BattleUnitKind unit_hyper_jyugem_15_data_58998;
extern BattleUnitKind unit_hyper_sinemon_15_data_2A3A8;
extern BattleUnitKind unit_ice_pakkun_15_data_577D8;
extern BattleUnitKind unit_jyugem_15_data_22B10;
extern BattleUnitKind unit_karon_15_data_55368;
extern s32 unit_mahorn[];
extern s32 unit_mahorn_custom[];
extern BattleUnitKind unit_monochrome_kurokumorn_15_data_1E998;
extern BattleUnitKind unit_monochrome_sinemon_15_data_191F8;
extern BattleUnitKind unit_pakkun_flower_15_data_4C6D0;
extern BattleUnitKind unit_patamet_15_data_2B728;
extern BattleUnitKind unit_patatogemet_15_data_3B6C8;
extern BattleUnitKind unit_phantom_15_data_4AC38;
extern BattleUnitKind unit_piders_15_data_66660;
extern s32 unit_purple_teresa_15_data_493B8[];
extern BattleUnitKind unit_sambo_15_data_1A280;
extern BattleUnitKind unit_sambo_mummy_15_data_36FB0;
extern BattleUnitKind unit_sinemon_15_data_48330;
extern BattleUnitKind unit_super_mahorn_15_data_43F68;
extern BattleUnitKind unit_teresa_15_data_25D30;
extern BattleUnitKind unit_togenoko_ace_15_data_41A18;
extern BattleUnitKind unit_twinkling_pansy_15_data_40EC8;
extern BattleUnitKind unit_ura_noko_15_data_2E060;
extern BattleUnitKind unit_wanwan_15_data_3F3F0;
extern BattleUnitKind unit_yami_kuriboo;
extern BattleUnitKind unit_yami_noko;
extern BattleUnitKind unit_yami_pata;
extern BattleUnitKind unit_yami_patakuri;
extern s32 unit_yami_togekuri[];
extern BattleUnitKind unit_yamitogedaruma;

// clang-format off
extend_data(32,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(33,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

BattleStageObjectData jon_btlobject_data[8] = {
    { "B_0", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "jon_0", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_1", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "jon_1", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_2", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "jon_2", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "B_6", 2, 2, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
    { "jon_6", 2, 3, { 0.0f, 0.0f, 0.0f }, 0, 60, { 0, 0 } },
};

// TODO: type
u32 jon_btlstage_data[436] = {
    (u32)"bti_01", (u32)"stg_00_3", 0x00000002, (u32)&jon_btlobject_data,
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
    0x00000000, (u32)"bti_01", (u32)"stg_00_3", 0x00000002,
    (u32)((u8*)&jon_btlobject_data + 0x30), 0x00000000, 0x00000000, 0x00000000,
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
    0x00000000, 0x00000000, (u32)"bti_01", (u32)"stg_00_3",
    0x00000002, (u32)((u8*)&jon_btlobject_data + 0x60), 0x00000000, 0x00000000,
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
    (u32)"stg_00_3", 0x00000002, (u32)((u8*)&jon_btlobject_data + 0x90), 0x00000000,
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
};

BattleUnitSetup btlparty_jon_jon_100_01_off_1[1] = {
    { &unit_boss_zonbaba, 1, 0x04000004, { 365.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_zonbaba },
};

BattleGroupSetup btlgrp_jon_jon_100_01_off_1 = { 1, btlparty_jon_jon_100_01_off_1, 0, 0, 100, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_01_off_1[1] = {
    { &unit_yami_kuriboo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
};

BattleGroupSetup btlgrp_jon_jon_01_01_off_1 = { 1, btlparty_jon_jon_01_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_01_off_2[2] = {
    { &unit_yami_kuriboo, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_jon_jon_01_01_off_2 = { 2, btlparty_jon_jon_01_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_01_off_3[2] = {
    { &unit_yami_kuriboo, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_jon_jon_01_01_off_3 = { 2, btlparty_jon_jon_01_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_02_off_1[5] = {
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_jon_jon_01_02_off_1 = { 5, btlparty_jon_jon_01_02_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_02_off_2[2] = {
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_yami_kuriboo, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
};

BattleGroupSetup btlgrp_jon_jon_01_02_off_2 = { 2, btlparty_jon_jon_01_02_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_02_off_3[5] = {
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_jon_jon_01_02_off_3 = { 5, btlparty_jon_jon_01_02_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_03_off_1[4] = {
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
};

BattleGroupSetup btlgrp_jon_jon_01_03_off_1 = { 4, btlparty_jon_jon_01_03_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_03_off_2[2] = {
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_yami_kuriboo, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
};

BattleGroupSetup btlgrp_jon_jon_01_03_off_2 = { 2, btlparty_jon_jon_01_03_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_03_off_3[4] = {
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_hannya_15_data_13DF0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hannya },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_01_03_off_3 = { 4, btlparty_jon_jon_01_03_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_04_off_1[4] = {
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_jon_jon_01_04_off_1 = { 4, btlparty_jon_jon_01_04_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_04_off_2[2] = {
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_yami_kuriboo, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
};

BattleGroupSetup btlgrp_jon_jon_01_04_off_2 = { 2, btlparty_jon_jon_01_04_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_04_off_3[3] = {
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
    { &unit_hinnya_15_data_149B0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hinnya },
};

BattleGroupSetup btlgrp_jon_jon_01_04_off_3 = { 3, btlparty_jon_jon_01_04_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_05_off_1[5] = {
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_01_05_off_1 = { 5, btlparty_jon_jon_01_05_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_05_off_2[3] = {
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_yami_kuriboo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_kuriboo },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_01_05_off_2 = { 3, btlparty_jon_jon_01_05_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_01_05_off_3[4] = {
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
    { &unit_chorobon_15_data_127B8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_chorobon },
    { &unit_honenoko_15_data_15568, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_honenoko },
};

BattleGroupSetup btlgrp_jon_jon_01_05_off_3 = { 4, btlparty_jon_jon_01_05_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_01_off_1[2] = {
    { &unit_yami_patakuri, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_11_01_off_1 = { 2, btlparty_jon_jon_11_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_01_off_2[3] = {
    { &unit_yami_patakuri, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_jon_jon_11_01_off_2 = { 3, btlparty_jon_jon_11_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_01_off_3[3] = {
    { &unit_yami_patakuri, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
};

BattleGroupSetup btlgrp_jon_jon_11_01_off_3 = { 3, btlparty_jon_jon_11_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_02_off_1[4] = {
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_jon_jon_11_02_off_1 = { 4, btlparty_jon_jon_11_02_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_02_off_2[3] = {
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_yami_patakuri, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_11_02_off_2 = { 3, btlparty_jon_jon_11_02_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_02_off_3[3] = {
    { &unit_monochrome_sinemon_15_data_191F8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_jon_jon_11_02_off_3 = { 3, btlparty_jon_jon_11_02_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_03_off_1[4] = {
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
};

BattleGroupSetup btlgrp_jon_jon_11_03_off_1 = { 4, btlparty_jon_jon_11_03_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_03_off_2[2] = {
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_jon_jon_11_03_off_2 = { 2, btlparty_jon_jon_11_03_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_03_off_3[4] = {
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
};

BattleGroupSetup btlgrp_jon_jon_11_03_off_3 = { 4, btlparty_jon_jon_11_03_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_04_off_1[3] = {
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_jon_jon_11_04_off_1 = { 3, btlparty_jon_jon_11_04_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_04_off_2[3] = {
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
};

BattleGroupSetup btlgrp_jon_jon_11_04_off_2 = { 3, btlparty_jon_jon_11_04_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_04_off_3[2] = {
    { &unit_monochrome_kurokumorn_15_data_1E998, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_monochrome_kurokumorn },
    { &unit_yami_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_11_04_off_3 = { 2, btlparty_jon_jon_11_04_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_05_off_1[4] = {
    { &unit_piders_15_data_66660, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
};

BattleGroupSetup btlgrp_jon_jon_11_05_off_1 = { 4, btlparty_jon_jon_11_05_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_05_off_2[2] = {
    { &unit_piders_15_data_66660, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_yami_patakuri, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_11_05_off_2 = { 2, btlparty_jon_jon_11_05_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_11_05_off_3[4] = {
    { &unit_piders_15_data_66660, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
    { &unit_piders_15_data_66660, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_piders },
    { &unit_sambo_15_data_1A280, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo },
};

BattleGroupSetup btlgrp_jon_jon_11_05_off_3 = { 4, btlparty_jon_jon_11_05_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_01_off_1[3] = {
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
};

BattleGroupSetup btlgrp_jon_jon_21_01_off_1 = { 3, btlparty_jon_jon_21_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_01_off_2[4] = {
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
};

BattleGroupSetup btlgrp_jon_jon_21_01_off_2 = { 4, btlparty_jon_jon_21_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_01_off_3[4] = {
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jon_jon_21_01_off_3 = { 4, btlparty_jon_jon_21_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_02_off_1[5] = {
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
};

BattleGroupSetup btlgrp_jon_jon_21_02_off_1 = { 5, btlparty_jon_jon_21_02_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_02_off_2[2] = {
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
};

BattleGroupSetup btlgrp_jon_jon_21_02_off_2 = { 2, btlparty_jon_jon_21_02_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_02_off_3[5] = {
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
};

BattleGroupSetup btlgrp_jon_jon_21_02_off_3 = { 5, btlparty_jon_jon_21_02_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_03_off_1[4] = {
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
};

BattleGroupSetup btlgrp_jon_jon_21_03_off_1 = { 4, btlparty_jon_jon_21_03_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_21_03_off_2[2] = {
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
};

BattleGroupSetup btlgrp_jon_jon_21_03_off_2 = { 2, btlparty_jon_jon_21_03_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_21_03_off_3[4] = {
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_jyugem_15_data_22B10, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_jyugem },
};

BattleGroupSetup btlgrp_jon_jon_21_03_off_3 = { 4, btlparty_jon_jon_21_03_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_21_04_off_1[4] = {
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
};

BattleGroupSetup btlgrp_jon_jon_21_04_off_1 = { 4, btlparty_jon_jon_21_04_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_04_off_2[3] = {
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
};

BattleGroupSetup btlgrp_jon_jon_21_04_off_2 = { 3, btlparty_jon_jon_21_04_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_04_off_3[4] = {
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_bomhei_15_data_24850, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bomhei },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jon_jon_21_04_off_3 = { 4, btlparty_jon_jon_21_04_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_21_05_off_1[4] = {
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
};

BattleGroupSetup btlgrp_jon_jon_21_05_off_1 = { 4, btlparty_jon_jon_21_05_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_21_05_off_2[3] = {
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { (struct BattleUnitKind*)unit_yami_togekuri, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_togekuri },
};

BattleGroupSetup btlgrp_jon_jon_21_05_off_2 = { 3, btlparty_jon_jon_21_05_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_21_05_off_3[4] = {
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_teresa_15_data_25D30, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_teresa },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
    { &unit_borodo_15_data_211F8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_borodo },
};

BattleGroupSetup btlgrp_jon_jon_21_05_off_3 = { 4, btlparty_jon_jon_21_05_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_31_01_off_1[2] = {
    { &unit_yami_noko, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
    { &unit_yami_noko, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
};

BattleGroupSetup btlgrp_jon_jon_31_01_off_1 = { 2, btlparty_jon_jon_31_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_01_off_2[3] = {
    { &unit_yami_noko, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_jon_jon_31_01_off_2 = { 3, btlparty_jon_jon_31_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_01_off_3[3] = {
    { &unit_yami_noko, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_31_01_off_3 = { 3, btlparty_jon_jon_31_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_02_off_1[3] = {
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_jon_jon_31_02_off_1 = { 3, btlparty_jon_jon_31_02_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_02_off_2[3] = {
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_jon_jon_31_02_off_2 = { 3, btlparty_jon_jon_31_02_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_02_off_3[3] = {
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_jon_jon_31_02_off_3 = { 3, btlparty_jon_jon_31_02_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_03_off_1[4] = {
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_jon_jon_31_03_off_1 = { 4, btlparty_jon_jon_31_03_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_03_off_2[4] = {
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
};

BattleGroupSetup btlgrp_jon_jon_31_03_off_2 = { 4, btlparty_jon_jon_31_03_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_03_off_3[2] = {
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_31_03_off_3 = { 2, btlparty_jon_jon_31_03_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_04_off_1[4] = {
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
};

BattleGroupSetup btlgrp_jon_jon_31_04_off_1 = { 4, btlparty_jon_jon_31_04_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_04_off_2[3] = {
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_yami_noko, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
};

BattleGroupSetup btlgrp_jon_jon_31_04_off_2 = { 3, btlparty_jon_jon_31_04_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_04_off_3[3] = {
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_ura_noko_15_data_2E060, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ura_noko },
    { &unit_patamet_15_data_2B728, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patamet },
};

BattleGroupSetup btlgrp_jon_jon_31_04_off_3 = { 3, btlparty_jon_jon_31_04_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_05_off_1[5] = {
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
};

BattleGroupSetup btlgrp_jon_jon_31_05_off_1 = { 5, btlparty_jon_jon_31_05_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_05_off_2[3] = {
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_yami_noko, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_noko },
};

BattleGroupSetup btlgrp_jon_jon_31_05_off_2 = { 3, btlparty_jon_jon_31_05_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_31_05_off_3[2] = {
    { &unit_flower_chorobon_15_data_2FE08, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_flower_chorobon },
    { &unit_hyper_sinemon_15_data_2A3A8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_sinemon },
};

BattleGroupSetup btlgrp_jon_jon_31_05_off_3 = { 2, btlparty_jon_jon_31_05_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_01_off_1[4] = {
    { &unit_yami_pata, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_pata, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_pata, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_pata, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_41_01_off_1 = { 4, btlparty_jon_jon_41_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_01_off_2[3] = {
    { &unit_yami_pata, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_pata, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
};

BattleGroupSetup btlgrp_jon_jon_41_01_off_2 = { 3, btlparty_jon_jon_41_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_01_off_3[3] = {
    { &unit_yami_pata, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_yami_pata, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_jon_jon_41_01_off_3 = { 3, btlparty_jon_jon_41_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_02_off_1[4] = {
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_jon_jon_41_02_off_1 = { 4, btlparty_jon_jon_41_02_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_02_off_2[2] = {
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_jon_jon_41_02_off_2 = { 2, btlparty_jon_jon_41_02_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_02_off_3[3] = {
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
};

BattleGroupSetup btlgrp_jon_jon_41_02_off_3 = { 3, btlparty_jon_jon_41_02_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_03_off_1[3] = {
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
};

BattleGroupSetup btlgrp_jon_jon_41_03_off_1 = { 3, btlparty_jon_jon_41_03_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_41_03_off_2[3] = {
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_yami_pata, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_41_03_off_2 = { 3, btlparty_jon_jon_41_03_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_41_03_off_3[4] = {
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_heavy_bom_15_data_33E08, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_heavy_bom },
};

BattleGroupSetup btlgrp_jon_jon_41_03_off_3 = { 4, btlparty_jon_jon_41_03_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_41_04_off_1[4] = {
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
};

BattleGroupSetup btlgrp_jon_jon_41_04_off_1 = { 4, btlparty_jon_jon_41_04_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_04_off_2[4] = {
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
};

BattleGroupSetup btlgrp_jon_jon_41_04_off_2 = { 4, btlparty_jon_jon_41_04_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_04_off_3[2] = {
    { &unit_sambo_mummy_15_data_36FB0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sambo_mummy },
    { &unit_yami_pata, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yami_patakuri },
};

BattleGroupSetup btlgrp_jon_jon_41_04_off_3 = { 2, btlparty_jon_jon_41_04_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_05_off_1[2] = {
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
};

BattleGroupSetup btlgrp_jon_jon_41_05_off_1 = { 2, btlparty_jon_jon_41_05_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_05_off_2[4] = {
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
};

BattleGroupSetup btlgrp_jon_jon_41_05_off_2 = { 4, btlparty_jon_jon_41_05_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_41_05_off_3[4] = {
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_patatogemet_15_data_3B6C8, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_patatogemet },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
    { &unit_bubble_15_data_35518, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_bubble },
};

BattleGroupSetup btlgrp_jon_jon_41_05_off_3 = { 4, btlparty_jon_jon_41_05_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_01_off_1[3] = {
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
};

BattleGroupSetup btlgrp_jon_jon_51_01_off_1 = { 3, btlparty_jon_jon_51_01_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_01_off_2[3] = {
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_jon_jon_51_01_off_2 = { 3, btlparty_jon_jon_51_01_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_01_off_3[3] = {
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_jon_jon_51_01_off_3 = { 3, btlparty_jon_jon_51_01_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_02_off_1[4] = {
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_jon_jon_51_02_off_1 = { 4, btlparty_jon_jon_51_02_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_02_off_2[3] = {
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_jon_jon_51_02_off_2 = { 3, btlparty_jon_jon_51_02_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_02_off_3[2] = {
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_jon_jon_51_02_off_3 = { 2, btlparty_jon_jon_51_02_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_03_off_1[4] = {
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_jon_jon_51_03_off_1 = { 4, btlparty_jon_jon_51_03_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_51_03_off_2[3] = {
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_badge_borodo, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_badge_borodo },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_jon_jon_51_03_off_2 = { 3, btlparty_jon_jon_51_03_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_51_03_off_3[3] = {
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_51_03_off_3 = { 3, btlparty_jon_jon_51_03_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_51_04_off_1[3] = {
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_51_04_off_1 = { 3, btlparty_jon_jon_51_04_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_04_off_2[3] = {
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_51_04_off_2 = { 3, btlparty_jon_jon_51_04_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_04_off_3[3] = {
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
    { &unit_bllizard_15_data_60E68, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_bllizard },
};

BattleGroupSetup btlgrp_jon_jon_51_04_off_3 = { 3, btlparty_jon_jon_51_04_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_05_off_1[4] = {
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
};

BattleGroupSetup btlgrp_jon_jon_51_05_off_1 = { 4, btlparty_jon_jon_51_05_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_05_off_2[3] = {
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { &unit_burst_wanwan_15_data_5F390, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_burst_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_51_05_off_2 = { 3, btlparty_jon_jon_51_05_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_51_05_off_3[3] = {
    { &unit_sinemon_15_data_48330, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_sinemon },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
    { (struct BattleUnitKind*)unit_purple_teresa_15_data_493B8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_purple_teresa },
};

BattleGroupSetup btlgrp_jon_jon_51_05_off_3 = { 3, btlparty_jon_jon_51_05_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_01_off_1[3] = {
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
};

BattleGroupSetup btlgrp_jon_jon_61_01_off_1 = { 3, btlparty_jon_jon_61_01_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_01_off_2[3] = {
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
};

BattleGroupSetup btlgrp_jon_jon_61_01_off_2 = { 3, btlparty_jon_jon_61_01_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_01_off_3[2] = {
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_61_01_off_3 = { 2, btlparty_jon_jon_61_01_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_02_off_1[4] = {
    { &unit_karon_15_data_55368, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_15_data_55368, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_15_data_55368, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_15_data_55368, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
};

BattleGroupSetup btlgrp_jon_jon_61_02_off_1 = { 4, btlparty_jon_jon_61_02_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_02_off_2[2] = {
    { &unit_karon_15_data_55368, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
};

BattleGroupSetup btlgrp_jon_jon_61_02_off_2 = { 2, btlparty_jon_jon_61_02_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_02_off_3[3] = {
    { &unit_karon_15_data_55368, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_jon_jon_61_02_off_3 = { 3, btlparty_jon_jon_61_02_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_03_off_1[3] = {
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_61_03_off_1 = { 3, btlparty_jon_jon_61_03_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_03_off_2[3] = {
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_61_03_off_2 = { 3, btlparty_jon_jon_61_03_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_03_off_3[3] = {
    { &unit_super_mahorn_15_data_43F68, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_super_mahorn },
    { &unit_karon_15_data_55368, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
    { &unit_karon_15_data_55368, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_karon },
};

BattleGroupSetup btlgrp_jon_jon_61_03_off_3 = { 3, btlparty_jon_jon_61_03_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_04_off_1[4] = {
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
};

BattleGroupSetup btlgrp_jon_jon_61_04_off_1 = { 4, btlparty_jon_jon_61_04_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_04_off_2[3] = {
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
};

BattleGroupSetup btlgrp_jon_jon_61_04_off_2 = { 3, btlparty_jon_jon_61_04_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_04_off_3[2] = {
    { &unit_ice_pakkun_15_data_577D8, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_ice_pakkun },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
};

BattleGroupSetup btlgrp_jon_jon_61_04_off_3 = { 2, btlparty_jon_jon_61_04_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_61_05_off_1[3] = {
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
};

BattleGroupSetup btlgrp_jon_jon_61_05_off_1 = { 3, btlparty_jon_jon_61_05_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_05_off_2[2] = {
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
    { &unit_hyper_jyugem_15_data_58998, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hyper_jyugem },
};

BattleGroupSetup btlgrp_jon_jon_61_05_off_2 = { 2, btlparty_jon_jon_61_05_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_61_05_off_3[2] = {
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
    { &unit_dark_keeper_15_data_5E0F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dark_keeper },
};

BattleGroupSetup btlgrp_jon_jon_61_05_off_3 = { 2, btlparty_jon_jon_61_05_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_01_off_1[3] = {
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_01_off_1 = { 3, btlparty_jon_jon_71_01_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_01_off_2[3] = {
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_01_off_2 = { 3, btlparty_jon_jon_71_01_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_01_off_3[3] = {
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_71_01_off_3 = { 3, btlparty_jon_jon_71_01_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_02_off_1[3] = {
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
};

BattleGroupSetup btlgrp_jon_jon_71_02_off_1 = { 3, btlparty_jon_jon_71_02_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_02_off_2[3] = {
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_71_02_off_2 = { 3, btlparty_jon_jon_71_02_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_02_off_3[3] = {
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_02_off_3 = { 3, btlparty_jon_jon_71_02_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default2, 5 };

BattleUnitSetup btlparty_jon_jon_71_03_off_1[5] = {
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
};

BattleGroupSetup btlgrp_jon_jon_71_03_off_1 = { 5, btlparty_jon_jon_71_03_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_71_03_off_2[4] = {
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_03_off_2 = { 4, btlparty_jon_jon_71_03_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_71_03_off_3[4] = {
    { &unit_phantom_15_data_4AC38, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_phantom },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
};

BattleGroupSetup btlgrp_jon_jon_71_03_off_3 = { 4, btlparty_jon_jon_71_03_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_71_04_off_1[4] = {
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 150.0f, 130.0f, 20.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
};

BattleGroupSetup btlgrp_jon_jon_71_04_off_1 = { 4, btlparty_jon_jon_71_04_off_1, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_04_off_2[4] = {
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_04_off_2 = { 4, btlparty_jon_jon_71_04_off_2, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_04_off_3[4] = {
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_basabasa_chururu_15_data_62428, 1, 0x04000004, { 110.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_chururu },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
};

BattleGroupSetup btlgrp_jon_jon_71_04_off_3 = { 4, btlparty_jon_jon_71_04_off_3, 20, 20, 60, battle_heart_drop_param_default2, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_05_off_1[4] = {
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
};

BattleGroupSetup btlgrp_jon_jon_71_05_off_1 = { 4, btlparty_jon_jon_71_05_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_05_off_2[3] = {
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
    { &unit_togenoko_ace_15_data_41A18, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_togenoko_ace },
};

BattleGroupSetup btlgrp_jon_jon_71_05_off_2 = { 3, btlparty_jon_jon_71_05_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_71_05_off_3[3] = {
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { &unit_wanwan_15_data_3F3F0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_wanwan },
    { (struct BattleUnitKind*)unit_mahorn, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn },
};

BattleGroupSetup btlgrp_jon_jon_71_05_off_3 = { 3, btlparty_jon_jon_71_05_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_01_off_1[4] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_01_off_1 = { 4, btlparty_jon_jon_81_01_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_01_off_2[4] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_churantalar, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_churantalar, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
};

BattleGroupSetup btlgrp_jon_jon_81_01_off_2 = { 4, btlparty_jon_jon_81_01_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_01_off_3[3] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_yamitogedaruma, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
};

BattleGroupSetup btlgrp_jon_jon_81_01_off_3 = { 3, btlparty_jon_jon_81_01_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_02_off_1[2] = {
    { &unit_yamitogedaruma, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
    { &unit_yamitogedaruma, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
};

BattleGroupSetup btlgrp_jon_jon_81_02_off_1 = { 2, btlparty_jon_jon_81_02_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_81_02_off_2[4] = {
    { &unit_yamitogedaruma, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
    { &unit_churantalar, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
};

BattleGroupSetup btlgrp_jon_jon_81_02_off_2 = { 4, btlparty_jon_jon_81_02_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_81_02_off_3[3] = {
    { &unit_yamitogedaruma, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
    { &unit_yamitogedaruma, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_02_off_3 = { 3, btlparty_jon_jon_81_02_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_81_03_off_1[4] = {
    { &unit_churantalar, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
};

BattleGroupSetup btlgrp_jon_jon_81_03_off_1 = { 4, btlparty_jon_jon_81_03_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_81_03_off_2[3] = {
    { &unit_churantalar, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_churantalar, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
};

BattleGroupSetup btlgrp_jon_jon_81_03_off_2 = { 3, btlparty_jon_jon_81_03_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_81_03_off_3[3] = {
    { &unit_churantalar, 1, 0x04000004, { 50.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_churantalar, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_churantalar },
    { &unit_yamitogedaruma, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
};

BattleGroupSetup btlgrp_jon_jon_81_03_off_3 = { 3, btlparty_jon_jon_81_03_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_81_04_off_1[4] = {
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
};

BattleGroupSetup btlgrp_jon_jon_81_04_off_1 = { 4, btlparty_jon_jon_81_04_off_1, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default5, 5 };

BattleUnitSetup btlparty_jon_jon_81_04_off_2[3] = {
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_yamitogedaruma, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_yamitogedaruma },
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
};

BattleGroupSetup btlgrp_jon_jon_81_04_off_2 = { 3, btlparty_jon_jon_81_04_off_2, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default5, 5 };

BattleUnitSetup btlparty_jon_jon_81_04_off_3[4] = {
    { &unit_pakkun_flower_15_data_4C6D0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_pakkun_flower },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_04_off_3 = { 4, btlparty_jon_jon_81_04_off_3, 20, 20, 60, battle_heart_drop_param_default, battle_flower_drop_param_default5, 5 };

BattleUnitSetup btlparty_jon_jon_81_05_off_1[4] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_05_off_1 = { 4, btlparty_jon_jon_81_05_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_05_off_2[3] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_05_off_2 = { 3, btlparty_jon_jon_81_05_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_81_05_off_3[5] = {
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
    { &unit_hennya_15_data_5A9C0, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_hennya },
};

BattleGroupSetup btlgrp_jon_jon_81_05_off_3 = { 5, btlparty_jon_jon_81_05_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_01_off_1[5] = {
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
};

BattleGroupSetup btlgrp_jon_jon_91_01_off_1 = { 5, btlparty_jon_jon_91_01_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_91_01_off_2[3] = {
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
};

BattleGroupSetup btlgrp_jon_jon_91_01_off_2 = { 3, btlparty_jon_jon_91_01_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_91_01_off_3[3] = {
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { &unit_basabasa_green, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
};

BattleGroupSetup btlgrp_jon_jon_91_01_off_3 = { 3, btlparty_jon_jon_91_01_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default4, 5 };

BattleUnitSetup btlparty_jon_jon_91_02_off_1[4] = {
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 150.0f, 40.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
};

BattleGroupSetup btlgrp_jon_jon_91_02_off_1 = { 4, btlparty_jon_jon_91_02_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_91_02_off_2[4] = {
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 70.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_giant_bomb, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
};

BattleGroupSetup btlgrp_jon_jon_91_02_off_2 = { 4, btlparty_jon_jon_91_02_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_91_02_off_3[4] = {
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 30.0f, 40.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_basabasa_green, 1, 0x04000004, { 70.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 110.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_02_off_3 = { 4, btlparty_jon_jon_91_02_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default3, 5 };

BattleUnitSetup btlparty_jon_jon_91_03_off_1[4] = {
    { &unit_giant_bomb, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
};

BattleGroupSetup btlgrp_jon_jon_91_03_off_1 = { 4, btlparty_jon_jon_91_03_off_1, 20, 20, 60, battle_heart_drop_param_default5, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_03_off_2[3] = {
    { &unit_giant_bomb, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
};

BattleGroupSetup btlgrp_jon_jon_91_03_off_2 = { 3, btlparty_jon_jon_91_03_off_2, 20, 20, 60, battle_heart_drop_param_default5, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_03_off_3[4] = {
    { &unit_giant_bomb, 1, 0x04000004, { 30.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_giant_bomb, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_giant_bomb },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_03_off_3 = { 4, btlparty_jon_jon_91_03_off_3, 20, 20, 60, battle_heart_drop_param_default5, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_04_off_1[5] = {
    { &unit_basabasa_green, 1, 0x04000004, { 10.0f, 130.0f, -20.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_basabasa_green, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_basabasa_green, 1, 0x04000004, { 90.0f, 130.0f, 0.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_basabasa_green, 1, 0x04000004, { 130.0f, 130.0f, 10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_basabasa_green, 1, 0x04000004, { 170.0f, 130.0f, 20.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
};

BattleGroupSetup btlgrp_jon_jon_91_04_off_1 = { 5, btlparty_jon_jon_91_04_off_1, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_04_off_2[3] = {
    { &unit_basabasa_green, 1, 0x04000004, { 50.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 90.0f, 40.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
    { &unit_dokugassun_15_data_5CC90, 1, 0x04000004, { 130.0f, 40.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_dokugassun },
};

BattleGroupSetup btlgrp_jon_jon_91_04_off_2 = { 3, btlparty_jon_jon_91_04_off_2, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_04_off_3[4] = {
    { &unit_basabasa_green, 1, 0x04000004, { 30.0f, 130.0f, -10.0f }, 0, { 1, 0, 0, 0 }, battle_item_basabasa_green },
    { (struct BattleUnitKind*)unit_mahorn_custom, 1, 0x04000004, { 70.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_mahorn_custom },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 110.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 150.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_04_off_3 = { 4, btlparty_jon_jon_91_04_off_3, 20, 20, 60, battle_heart_drop_param_default4, battle_flower_drop_param_default, 5 };

BattleUnitSetup btlparty_jon_jon_91_05_off_1[5] = {
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_05_off_1 = { 5, btlparty_jon_jon_91_05_off_1, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default5, 5 };

BattleUnitSetup btlparty_jon_jon_91_05_off_2[5] = {
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_05_off_2 = { 5, btlparty_jon_jon_91_05_off_2, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default5, 5 };

BattleUnitSetup btlparty_jon_jon_91_05_off_3[5] = {
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 10.0f, 0.0f, -20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 50.0f, 0.0f, -10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 90.0f, 0.0f, 0.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 130.0f, 0.0f, 10.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
    { &unit_twinkling_pansy_15_data_40EC8, 1, 0x04000004, { 170.0f, 0.0f, 20.0f }, 0, { 0, 0, 0, 0 }, battle_item_twinkling_pansy },
};

BattleGroupSetup btlgrp_jon_jon_91_05_off_3 = { 5, btlparty_jon_jon_91_05_off_3, 20, 20, 60, battle_heart_drop_param_default3, battle_flower_drop_param_default5, 5 };

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_test1_off[2] = {
    { 50, &btlgrp_jon_jon_01_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_01_off[4] = {
    { 10, &btlgrp_jon_jon_01_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_01_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_01_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_02_off[4] = {
    { 10, &btlgrp_jon_jon_01_02_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_02_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_02_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_03_off[4] = {
    { 10, &btlgrp_jon_jon_01_03_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_03_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_03_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_04_off[4] = {
    { 10, &btlgrp_jon_jon_01_04_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_04_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_04_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_05_off[4] = {
    { 10, &btlgrp_jon_jon_01_05_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_05_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_01_05_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_11_off[4] = {
    { 10, &btlgrp_jon_jon_11_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_01_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_01_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_12_off[4] = {
    { 10, &btlgrp_jon_jon_11_02_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_02_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_02_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_13_off[4] = {
    { 10, &btlgrp_jon_jon_11_03_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_03_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_03_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_14_off[4] = {
    { 10, &btlgrp_jon_jon_11_04_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_04_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_04_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_15_off[4] = {
    { 10, &btlgrp_jon_jon_11_05_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_05_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_11_05_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_21_off[4] = {
    { 10, &btlgrp_jon_jon_21_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_01_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_01_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_22_off[4] = {
    { 10, &btlgrp_jon_jon_21_02_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_02_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_02_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_23_off[4] = {
    { 10, &btlgrp_jon_jon_21_03_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_03_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_03_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_24_off[4] = {
    { 10, &btlgrp_jon_jon_21_04_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_04_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_04_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_25_off[4] = {
    { 10, &btlgrp_jon_jon_21_05_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_05_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_21_05_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_31_off[4] = {
    { 10, &btlgrp_jon_jon_31_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_01_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_01_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_32_off[4] = {
    { 10, &btlgrp_jon_jon_31_02_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_02_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_02_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_33_off[4] = {
    { 10, &btlgrp_jon_jon_31_03_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_03_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_03_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_34_off[4] = {
    { 10, &btlgrp_jon_jon_31_04_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_04_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_04_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_35_off[4] = {
    { 10, &btlgrp_jon_jon_31_05_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_05_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_31_05_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_41_off[4] = {
    { 10, &btlgrp_jon_jon_41_01_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_01_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_01_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_42_off[4] = {
    { 10, &btlgrp_jon_jon_41_02_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_02_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_02_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_43_off[4] = {
    { 10, &btlgrp_jon_jon_41_03_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_03_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_03_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_44_off[4] = {
    { 10, &btlgrp_jon_jon_41_04_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_04_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_04_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_45_off[4] = {
    { 10, &btlgrp_jon_jon_41_05_off_1, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_05_off_2, (struct BattleStageData*)jon_btlstage_data },
    { 10, &btlgrp_jon_jon_41_05_off_3, (struct BattleStageData*)jon_btlstage_data },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_51_off[4] = {
    { 10, &btlgrp_jon_jon_51_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_01_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_01_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_52_off[4] = {
    { 10, &btlgrp_jon_jon_51_02_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_02_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_02_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_53_off[4] = {
    { 10, &btlgrp_jon_jon_51_03_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_03_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_03_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_54_off[4] = {
    { 10, &btlgrp_jon_jon_51_04_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_04_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_04_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_55_off[4] = {
    { 10, &btlgrp_jon_jon_51_05_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_05_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_51_05_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_61_off[4] = {
    { 10, &btlgrp_jon_jon_61_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_01_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_01_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_62_off[4] = {
    { 10, &btlgrp_jon_jon_61_02_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_02_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_02_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_63_off[4] = {
    { 10, &btlgrp_jon_jon_61_03_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_03_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_03_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_64_off[4] = {
    { 10, &btlgrp_jon_jon_61_04_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_04_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_04_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_65_off[4] = {
    { 10, &btlgrp_jon_jon_61_05_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_05_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_61_05_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_71_off[4] = {
    { 10, &btlgrp_jon_jon_71_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_01_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_01_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_72_off[4] = {
    { 10, &btlgrp_jon_jon_71_02_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_02_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_02_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_73_off[4] = {
    { 10, &btlgrp_jon_jon_71_03_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_03_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_03_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_74_off[4] = {
    { 10, &btlgrp_jon_jon_71_04_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_04_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_04_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_75_off[4] = {
    { 10, &btlgrp_jon_jon_71_05_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_05_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_71_05_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_81_off[4] = {
    { 10, &btlgrp_jon_jon_81_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_01_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_01_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_82_off[4] = {
    { 10, &btlgrp_jon_jon_81_02_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_02_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_02_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_83_off[4] = {
    { 10, &btlgrp_jon_jon_81_03_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_03_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_03_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_84_off[4] = {
    { 10, &btlgrp_jon_jon_81_04_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_04_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_04_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_85_off[4] = {
    { 10, &btlgrp_jon_jon_81_05_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_05_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 10, &btlgrp_jon_jon_81_05_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x1B4) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_91_off[4] = {
    { 10, &btlgrp_jon_jon_91_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_01_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_01_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_92_off[4] = {
    { 10, &btlgrp_jon_jon_91_02_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_02_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_02_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_93_off[4] = {
    { 10, &btlgrp_jon_jon_91_03_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_03_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_03_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_94_off[4] = {
    { 10, &btlgrp_jon_jon_91_04_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_04_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_04_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_95_off[4] = {
    { 10, &btlgrp_jon_jon_91_05_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_05_off_2, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 10, &btlgrp_jon_jon_91_05_off_3, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x368) },
    { 0, NULL, NULL },
};

NpcBattleSetupEntry btlgrpsel_jon_btlno_jon_100_zonbaba_off[2] = {
    { 10, &btlgrp_jon_jon_100_01_off_1, (struct BattleStageData*)(void*)((u8*)jon_btlstage_data + 0x51C) },
    { 0, NULL, NULL },
};

BattleSetupData btlsetup_jon_tbl[53] = {
    {
        "マップstg_00_3",                                                                                      // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_test1_off,                                                                  // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミクリボー",                                                                                             // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_01_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ヒンニャー",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_02_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ハンニャー",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_03_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ホネノコ",                                                                                               // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_04_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 10, 10, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "チョロボン",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_05_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミパタクリ",                                                                                             // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_11_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "モノクロシンエモン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_12_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "サンボ",                                                                                              // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_13_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "モノクロクロクモーン",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_14_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "パイダース",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_15_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミトゲクリ",                                                                                             // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_21_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 10, 10, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ボロドー",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_22_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ジュゲム",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_23_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ボムへい",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_24_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "テレサ",                                                                                                // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_25_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 5, 5, 5, 5, 10, 10, 0, 0, 5, 5, 5, 5, 0, 0, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ヤミノコノコ",                                                                                             // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_31_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ハイパーシンエモン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_32_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "パタメット",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_33_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ウラノコ",                                                                                               // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_34_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "フラワーチョロボン",                                                                                        // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_35_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミパタパタ",                                                                                             // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_41_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 10, 10, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "ヘビーボム",                                                                                              // name
        NULL,                                                                                                 // field_0x4
        0,                                                                                                    // field_0x8
        -1,                                                                                                   // flag
        NULL,                                                                                                 // onLoadouts
        btlgrpsel_jon_btlno_jon_42_off,                                                                       // offLoadouts
        268435456,                                                                                            // field_0x18
        0,                                                                                                    // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 10, 10, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                   // music
    },
    {
        "バブル",                                                                                              // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_43_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "サンボマミー",                                                                                           // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_44_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "パタトゲメット",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_45_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "バッジボロドー",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_51_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ブリザード",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_52_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "パープルテレサ",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_53_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "バーストワンワン",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_54_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "シンエモン",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_55_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ハイパージュゲム",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_61_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "カロン",                                                                                              // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_62_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "スーパーマホーン",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_63_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "アイスパックン",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_64_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ダークキーパー",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_65_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マホーン",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_71_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "トゲノコエース",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_72_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ファントム",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_73_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "バサバサチュルル",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_74_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ワンワン",                                                                                             // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_75_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヘンニャー",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_81_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヤミトゲダルマー",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_82_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "チュランタラー",                                                                                          // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_83_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "パックンフラワー",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_84_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ヘンニャー",                                                                                            // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_85_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "マホーンカスタム",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_91_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ドクガッスン",                                                                                           // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_92_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ジャイアントボム",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_93_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "バサバサグリーン",                                                                                         // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_94_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "きらめくパンジーさん",                                                                                       // name
        NULL,                                                                                               // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_95_off,                                                                     // offLoadouts
        268435456,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
        "BGM_ZAKO_BATTLE1",                                                                                 // music
    },
    {
        "ゾンババ",                                                                                             // name
        "jon_06",                                                                                           // field_0x4
        0,                                                                                                  // field_0x8
        -1,                                                                                                 // flag
        NULL,                                                                                               // onLoadouts
        btlgrpsel_jon_btlno_jon_100_zonbaba_off,                                                            // offLoadouts
        537919504,                                                                                          // field_0x18
        0,                                                                                                  // field_0x1C
        { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1 }, // field_0x20
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
u32 btl_jon_setup_no_tbl[106] = {
    (u32)"BTLNO_JON_TEST1", 0x00000000, (u32)"BTLNO_JON_01", 0x00000001,
    (u32)"BTLNO_JON_02", 0x00000002, (u32)"BTLNO_JON_03", 0x00000003,
    (u32)"BTLNO_JON_04", 0x00000004, (u32)"BTLNO_JON_05", 0x00000005,
    (u32)"BTLNO_JON_11", 0x00000006, (u32)"BTLNO_JON_12", 0x00000007,
    (u32)"BTLNO_JON_13", 0x00000008, (u32)"BTLNO_JON_14", 0x00000009,
    (u32)"BTLNO_JON_15", 0x0000000A, (u32)"BTLNO_JON_21", 0x0000000B,
    (u32)"BTLNO_JON_22", 0x0000000C, (u32)"BTLNO_JON_23", 0x0000000D,
    (u32)"BTLNO_JON_24", 0x0000000E, (u32)"BTLNO_JON_25", 0x0000000F,
    (u32)"BTLNO_JON_31", 0x00000010, (u32)"BTLNO_JON_32", 0x00000011,
    (u32)"BTLNO_JON_33", 0x00000012, (u32)"BTLNO_JON_34", 0x00000013,
    (u32)"BTLNO_JON_35", 0x00000014, (u32)"BTLNO_JON_41", 0x00000015,
    (u32)"BTLNO_JON_42", 0x00000016, (u32)"BTLNO_JON_43", 0x00000017,
    (u32)"BTLNO_JON_44", 0x00000018, (u32)"BTLNO_JON_45", 0x00000019,
    (u32)"BTLNO_JON_51", 0x0000001A, (u32)"BTLNO_JON_52", 0x0000001B,
    (u32)"BTLNO_JON_53", 0x0000001C, (u32)"BTLNO_JON_54", 0x0000001D,
    (u32)"BTLNO_JON_55", 0x0000001E, (u32)"BTLNO_JON_61", 0x0000001F,
    (u32)"BTLNO_JON_62", 0x00000020, (u32)"BTLNO_JON_63", 0x00000021,
    (u32)"BTLNO_JON_64", 0x00000022, (u32)"BTLNO_JON_65", 0x00000023,
    (u32)"BTLNO_JON_71", 0x00000024, (u32)"BTLNO_JON_72", 0x00000025,
    (u32)"BTLNO_JON_73", 0x00000026, (u32)"BTLNO_JON_74", 0x00000027,
    (u32)"BTLNO_JON_75", 0x00000028, (u32)"BTLNO_JON_81", 0x00000029,
    (u32)"BTLNO_JON_82", 0x0000002A, (u32)"BTLNO_JON_83", 0x0000002B,
    (u32)"BTLNO_JON_84", 0x0000002C, (u32)"BTLNO_JON_85", 0x0000002D,
    (u32)"BTLNO_JON_91", 0x0000002E, (u32)"BTLNO_JON_92", 0x0000002F,
    (u32)"BTLNO_JON_93", 0x00000030, (u32)"BTLNO_JON_94", 0x00000031,
    (u32)"BTLNO_JON_95", 0x00000032, (u32)"BTLNO_JON_100_ZONBABA", 0x00000033,
    0x00000000, 0x00000000,
};
// clang-format on
#pragma force_active off
