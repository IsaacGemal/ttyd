#include "battle_database_rsh.h"
#include "battle/battle.h"
#include "battle/battle_weapon_power.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "manager/evtmgr_cmd.h"

#pragma pool_data off

#pragma force_active on
extern s32 battle_heart_drop_param_default[];
extern s32 battle_item_moamoa[];
extern s32 unit_boss_moamoa[];
s32 bgset_rsh_bg_event[30];

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

BattleStageObjectData rsh_btlobject_data[7] = {
    {"r_mae", 2, 1, {170.0f, 0.0f, -50.0f}, 0, 60},
    {"r_oku", 2, 1, {170.0f, -1000.0f, -50.0f}, 10, 50},
    {"l_mae", 2, 1, {-170.0f, 0.0f, -50.0f}, 20, 40},
    {"l_oku", 2, 1, {-170.0f, -1000.0f, -50.0f}, 30, 30},
    {"iwayama_1", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"iwayama_2", 2, 2, {0.0f, 0.0f, 0.0f}, 0, 60},
    {"C", 2, 3, {0.0f, 0.0f, 0.0f}, 0, 60},
};

// TODO: type
u32 rsh_btlstage_data[109] = {
    (u32)"bti_01", (u32)"stg_06_0", 0x00000007, (u32)&rsh_btlobject_data,
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
    0x00000000, 0x00000000, (u32)&bgset_rsh_bg_event, 0x00000000,
    0x00000000,
};

// TODO: type
u32 btlparty_rsh_rsh_06_01_off_1[12] = {
    (u32)unit_boss_moamoa, 0x01000000, 0x04000004, 0x428C0000,
    0xC1A00000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, (u32)battle_item_moamoa,
};

// TODO: type
u32 btlgrp_rsh_rsh_06_01_off_1[8] = {
    0x00000001, (u32)&btlparty_rsh_rsh_06_01_off_1, 0x00000000, 0x00000000,
    0x00000064, (u32)battle_heart_drop_param_default, (u32)battle_heart_drop_param_default, 0x00000005,
};

// TODO: type
u32 btlgrpsel_rsh_btlno_rsh_test1_off[6] = {
    0x00000032, (u32)&btlgrp_rsh_rsh_06_01_off_1, (u32)&rsh_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlgrpsel_rsh_btlno_rsh_06_01_off[6] = {
    0x00000032, (u32)&btlgrp_rsh_rsh_06_01_off_1, (u32)&rsh_btlstage_data, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 btlsetup_rsh_tbl[51] = {
    (u32)"マップstg_06_0", 0x00000000, 0x00000000, 0xFFFFFFFF,
    0x00000000, (u32)&btlgrpsel_rsh_btlno_rsh_test1_off, 0x00000000, 0x00000000,
    0x03030303, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x01010101, 0x01010101,
    (u32)"BGM_ZAKO_BATTLE1", (u32)"rsh_06 もあもあ", 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, (u32)&btlgrpsel_rsh_btlno_rsh_06_01_off, 0x20000010,
    0x00000006, 0x03030303, 0x03030303, 0x03030303,
    0x03030303, 0x03030303, 0x03030303, 0x01010101,
    0x01010101, (u32)"BGM_BOSS_STG6_GREAT_MOAMOA1", 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 btl_rsh_setup_no_tbl[6] = {
    (u32)"BTLNO_RSH_TEST1", 0x00000000, (u32)"BTLNO_RSH_06_01", 0x00000001,
    0x00000000, 0x00000000,
};

s32 _set_offset(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 outIdx = args[1];
    s32 a = evtGetValue(event, outIdx);
    s32 b = evtGetValue(event, args[2]);
    s32 c = evtGetValue(event, args[3]);
    s32 d = evtGetValue(event, args[4]);
    BattleStageObject* obj = BattleSearchObjectPtr((const char*)id);

    obj->field_0x10.x = c + (d * a) / b;
    a = a - 1;
    if (a < 0) {
        a = b;
    }
    evtSetValue(event, outIdx, a);
    return 2;
}

s32 _set_destroy_flag(EventEntry* event, BOOL isFirstCall) {
    BattleWork* wp = _battleWorkPointer;

    *(u8*)((u8*)wp + 0x180DC) |= 0x8;
    return 2;
}

EVT_BEGIN(bgset_rsh_bg_event)
    CALL(_set_destroy_flag)
    SET_VALUE(LW(0), 0)
    SET_VALUE(LW(1), 200)
    LABEL(0)
    CALL(_set_offset, STRING("iwayama_1"), LW(0), 400, -300, 800)
    CALL(_set_offset, STRING("iwayama_2"), LW(1), 400, -400, 800)
    WAIT_FRAMES(1)
    GOTO(0)
    RETURN
EVT_END()
// clang-format on
#pragma force_active off
