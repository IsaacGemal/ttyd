#include <dolphin/types.h>

extern u8 btldefaultevt_Confuse[];
extern u8 btldefaultevt_Damage[];
extern u8 btlevtcmd_AfterReactionEntry[];
extern u8 btlevtcmd_AnimeChangePose[];
extern u8 btlevtcmd_AnimeChangePoseFromTable[];
extern u8 btlevtcmd_AnimeSetPoseTable[];
extern u8 btlevtcmd_CheckDamage[];
extern u8 btlevtcmd_CheckPhase[];
extern u8 btlevtcmd_ChoiceSamplingEnemy[];
extern u8 btlevtcmd_GetHitPos[];
extern u8 btlevtcmd_GetPos[];
extern u8 btlevtcmd_GetSelectNextEnemy[];
extern u8 btlevtcmd_GetUnitId[];
extern u8 btlevtcmd_GetUnitWork[];
extern u8 btlevtcmd_KillUnit[];
extern u8 btlevtcmd_OnAttribute[];
extern u8 btlevtcmd_OnPartsAttribute[];
extern u8 btlevtcmd_PhaseEventStartDeclare[];
extern u8 btlevtcmd_PreCheckDamage[];
extern u8 btlevtcmd_SamplingEnemy[];
extern u8 btlevtcmd_SetEventAttack[];
extern u8 btlevtcmd_SetEventConfusion[];
extern u8 btlevtcmd_SetEventDamage[];
extern u8 btlevtcmd_SetEventPhase[];
extern u8 btlevtcmd_SetEventWait[];
extern u8 btlevtcmd_SetMaxMoveCount[];
extern u8 btlevtcmd_SetUnitWork[];
extern u8 btlevtcmd_StageDispellFog[];
extern u8 btlevtcmd_StartWaitEvent[];
extern u8 btlevtcmd_snd_se[];
extern u8 evt_btl_camera_shake_h[];
extern u8 evt_eff[];
extern u8 evt_eff64[];
extern u8 evt_snd_sfxoff[];
extern u8 evt_sub_random[];
extern u8 weaponGetPowerDefault[];

extern const char str_802F9210[16];
extern const char str_802F9220[16];
extern const char str_802F9230[24];
extern const char str_802F9248[20];
extern const char str_802F925C[28];
extern const char str_802F9278[12];
extern const char str_802F9284[12];
extern const char str_802F9290[12];
extern const char str_802F929C[24];
static const u32 gap_04_802F92B4_rodata[1];
extern const char str_8041E490[4];
extern const char str_8041E494[4];
extern const char str_8041E498[4];
extern const char str_8041E49C[4];
extern const u32 zero_8041e4a0[1];
static const u32 gap_09_8041E4A4_sdata2[1];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 pose_table_3[8];
static u32 pose_table_2[8];
static u32 pose_table_1[8];
static u32 data_table[8];
static u32 damage_event[10];
static u32 attack_event[5];
static u32 dead_event[6];
static u32 gap_07_80412A94_sdata[1];
extern u32 unit_bomzou[49];
extern u32 weapon_bomzou_explosion[48];
static u32 parts[19];
static u32 init_event[70];
static u32 phase_event[67];
static u32 wait_event[57];
extern u32 end_battle_evt[19];
static u32 explosion_event[173];
extern u32 lbl_80398960[2];

const char str_802F9210[16] = "btl_un_bomzou\000\000";

const char str_802F9220[16] = "SFX_BTL_DAMAGE1";

const char str_802F9230[24] = "SFX_BTL_DAMAGE_FIRE1\000\000\000";

const char str_802F9248[20] = "SFX_BTL_DAMAGE_ICE1";

const char str_802F925C[28] = "SFX_BTL_DAMAGE_BIRIBIRI1\000\000\000";

const char str_802F9278[12] = "c_bakudan\000\000";

const char str_802F9284[12] = "sweat_n64\000\000";

const char str_802F9290[12] = "hibashira\000\000";

const char str_802F929C[24] = "SFX_BTL_THUNDERS_BOMB2\000";

static const u32 gap_04_802F92B4_rodata[1] = {
    0x00000000
};

const char str_8041E490[4] = "B_2";

const char str_8041E494[4] = "B_3";

const char str_8041E498[4] = "B_4";

const char str_8041E49C[4] = "B_1";

const u32 zero_8041e4a0[1] = {
    0x00000000
};

static const u32 gap_09_8041E4A4_sdata2[1] = {
    0x00000000
};

static u32 defence[2] = {
    0x00000000, 0x00000000
};

static u32 defence_attr[2] = {
    0x00000000, 0x00000000
};

static u32 regist[6] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

static u32 pose_table_3[8] = {
    0x0000001C, (u32)(void*)str_8041E490, 0x0000001F, (u32)(void*)str_8041E490, 0x00000027, (u32)(void*)str_8041E490, 0x00000045, (u32)(void*)str_8041E490
};

static u32 pose_table_2[8] = {
    0x0000001C, (u32)(void*)str_8041E494, 0x0000001F, (u32)(void*)str_8041E494, 0x00000027, (u32)(void*)str_8041E494, 0x00000045, (u32)(void*)str_8041E494
};

static u32 pose_table_1[8] = {
    0x0000001C, (u32)(void*)str_8041E498, 0x0000001F, (u32)(void*)str_8041E498, 0x00000027, (u32)(void*)str_8041E498, 0x00000045, (u32)(void*)str_8041E498
};

static u32 data_table[8] = {
    0x00000031, (u32)(void*)dead_event, 0x00000035, (u32)(void*)explosion_event, 0x0000003F, (u32)(void*)end_battle_evt, 0x00000000, 0x00000000
};

static u32 damage_event[10] = {
    0x00020032, 0xFE363C8A, 0xFFFFFFFE, 0x00020032, 0xFE363C8B, 0x00000001, 0x0001005E, (u32)(void*)btldefaultevt_Damage,
    0x00000002, 0x00000001
};

static u32 attack_event[5] = {
    0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001
};

static u32 dead_event[6] = {
    0x0003005B, (u32)(void*)btlevtcmd_AfterReactionEntry, 0xFFFFFFFE, 0x00000035, 0x00000002, 0x00000001
};

static u32 gap_07_80412A94_sdata[1] = {
    0x00000000
};

u32 unit_bomzou[49] = {
    0x000000DB, (u32)(void*)str_802F9210, 0x00010000, 0x01010100, 0x00000064, 0x00090018, 0x00180000, 0x00140000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41400000,
    0x00000000, 0x00000000, 0x41400000, 0x4179999A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802F9220,
    (u32)(void*)str_802F9230, (u32)(void*)str_802F9248, (u32)(void*)str_802F925C, 0x12400000, (u32)(void*)regist, 0x01000000, (u32)(void*)parts, (u32)(void*)init_event,
    (u32)(void*)data_table
};

u32 weapon_bomzou_explosion[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x64000000, 0x00000000, 0x00000000, (u32)(void*)weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02200020, 0x20000000, 0x03000302, 0x00000000, 0x00020080, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000A0A64, 0x1E050505, 0x05000000
};

static u32 parts[19] = {
    0x00000001, (u32)(void*)str_802F9210, (u32)(void*)str_802F9278, 0x00000000, 0x00000000, 0x00000000, 0x41700000, 0x41C80000,
    0x00000000, 0x00000000, 0x41F00000, 0x00000000, 0x0014001E, 0x00FF0000, (u32)(void*)defence, (u32)(void*)defence_attr,
    0x00080009, 0x00000000, (u32)(void*)pose_table_3
};

static u32 init_event[70] = {
    0x0003005B, (u32)(void*)btlevtcmd_SetEventWait, 0xFFFFFFFE, (u32)(void*)wait_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventPhase, 0xFFFFFFFE, (u32)(void*)phase_event,
    0x0003005B, (u32)(void*)btlevtcmd_SetEventAttack, 0xFFFFFFFE, (u32)(void*)attack_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventDamage, 0xFFFFFFFE, (u32)(void*)damage_event,
    0x0003005B, (u32)(void*)btlevtcmd_SetEventConfusion, 0xFFFFFFFE, (u32)(void*)btldefaultevt_Confuse, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000000,
    0x00000002, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x0003005B, (u32)(void*)btlevtcmd_SetMaxMoveCount,
    0xFFFFFFFE, 0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C80, 0x00010022,
    0xFE363C80, 0x00010029, 0x00000003, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_3,
    0x00010029, 0x00000002, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_2, 0x00010029,
    0x00000001, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_1, 0x00000031, 0x0004005B,
    (u32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (u32)(void*)str_8041E49C, 0x00000002, 0x00000001
};

static u32 phase_event[67] = {
    0x0003005B, (u32)(void*)btlevtcmd_CheckPhase, 0xFE363C80, 0x04000001, 0x00020019, 0xFE363C80, 0x00000000, 0x0004005B,
    (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C80, 0x00020036, 0xFE363C80, 0x00000001, 0x0004005B,
    (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C80, 0x00010022, 0xFE363C80, 0x00010029, 0x00000003,
    0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_3, 0x00010029, 0x00000002, 0x0004005B,
    (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_2, 0x00010029, 0x00000001, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable,
    0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_1, 0x00000031, 0x0002001C, 0xFE363C80, 0x00000000, 0x0002005B,
    (u32)(void*)btlevtcmd_PhaseEventStartDeclare, 0xFFFFFFFE, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (u32)(void*)str_8041E490, 0x00010009,
    0x00000024, 0x0001005E, (u32)(void*)explosion_event, 0x00000020, 0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000021,
    0x00000021, 0x00000002, 0x00000001
};

static u32 wait_event[57] = {
    0x0003005B, (u32)(void*)btlevtcmd_AnimeChangePoseFromTable, 0xFFFFFFFE, 0x00000001, 0x00010003, 0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork,
    0xFFFFFFFE, 0x00000000, 0xFE363C80, 0x0002001B, 0xFE363C80, 0x00000001, 0x00010009, 0x00000001,
    0x00010004, 0x00000000, 0x00000021, 0x0005005B, (u32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00020035, 0xFE363C81, 0x0000000A, 0x000F005B, (u32)(void*)evt_eff64, 0x00000000, (u32)(void*)str_802F9284,
    0x00000001, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0000000F, 0xFFFFFFD3, 0x0000000F, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0003005B, (u32)(void*)evt_sub_random, 0x0000002D, 0xFE363C80,
    0x00020035, 0xFE363C80, 0x0000002D, 0x00010009, 0xFE363C80, 0x00010004, 0x00000000, 0x00000002,
    0x00000001
};

u32 end_battle_evt[19] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000001, 0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0002005B, (u32)(void*)evt_snd_sfxoff, 0xFE363C80, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001, 0x00000000,
    0x00000021, 0x00000002, 0x00000001
};

static u32 explosion_event[173] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000001, 0xFE363C80, 0x00020019, 0xFE363C80, 0x00000000,
    0x0002005B, (u32)(void*)evt_snd_sfxoff, 0xFE363C80, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001, 0x00000000,
    0x00000021, 0x0005005B, (u32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0x000F005B,
    (u32)(void*)evt_eff, (u32)(void*)zero_8041e4a0, (u32)(void*)str_802F9290, 0x00000002, 0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000001, 0xF24A7E80, 0x0000003C, 0x00000000, 0x00000000, 0x0006005B,
    (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)str_802F929C, 0xF1194D80, 0x00000000, 0xF1194D80, 0x0001005B, (u32)(void*)btlevtcmd_StageDispellFog,
    0x0003005B, (u32)(void*)btlevtcmd_OnAttribute, 0xFFFFFFFE, 0x01000000, 0x0004005B, (u32)(void*)btlevtcmd_OnPartsAttribute, 0xFFFFFFFE, 0x00000001,
    0x03000000, 0x0006005B, (u32)(void*)evt_btl_camera_shake_h, 0x00000000, 0x0000000A, 0x00000000, 0x0000003C, 0x00000004,
    0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000001, (u32)(void*)str_8041E490, 0x0004005B, (u32)(void*)btlevtcmd_SamplingEnemy, 0xFFFFFFFE,
    0x00000001, (u32)(void*)weapon_bomzou_explosion, 0x0004005B, (u32)(void*)btlevtcmd_ChoiceSamplingEnemy, (u32)(void*)weapon_bomzou_explosion, 0xFE363C83, 0xFE363C84, 0x00020018,
    0xFE363C83, 0xFFFFFFFF, 0x00010004, 0x00000063, 0x00000021, 0x00020032, 0xFE363C8A, 0x00000000,
    0x00010003, 0x0000000A, 0x0003005B, (u32)(void*)btlevtcmd_GetUnitId, 0xFFFFFFFE, 0xFE363C80, 0x00020018, 0xFE363C80,
    0xFE363C83, 0x00010004, 0x00000050, 0x00000021, 0x0005005B, (u32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80,
    0xF1194D80, 0xF1194D80, 0x0006005B, (u32)(void*)btlevtcmd_GetHitPos, 0xFE363C83, 0xFE363C84, 0xFE363C81, 0xF1194D80,
    0xF1194D80, 0x00020036, 0xFE363C80, 0xFE363C81, 0x0002001C, 0xFE363C80, 0xFFFFFFFF, 0x00020037,
    0xFE363C80, 0xFFFFFFFF, 0x00000021, 0x0002001B, 0xFE363C80, 0x00000032, 0x00010004, 0x00000050,
    0x00000021, 0x0007005B, (u32)(void*)btlevtcmd_PreCheckDamage, 0xFFFFFFFE, 0xFE363C83, 0xFE363C84, (u32)(void*)weapon_bomzou_explosion, 0x00000100,
    0xFE363C85, 0x00020018, 0xFE363C85, 0x00000001, 0x0007005B, (u32)(void*)btlevtcmd_CheckDamage, 0xFFFFFFFE, 0xFE363C83,
    0xFE363C84, (u32)(void*)weapon_bomzou_explosion, 0x00000100, 0xFE363C85, 0x00000021, 0x00010003, 0x00000050, 0x0003005B,
    (u32)(void*)btlevtcmd_GetSelectNextEnemy, 0xFE363C83, 0xFE363C84, 0x00020019, 0xFE363C83, 0xFFFFFFFF, 0x00020035, 0xFE363C8A,
    0x00000001, 0x00010004, 0x0000000A, 0x00000021, 0x00010003, 0x00000063, 0x0003005B, (u32)(void*)btlevtcmd_KillUnit,
    0xFFFFFFFE, 0x00000000, 0x00000002, 0x00000001, 0x00000000
};

__declspec(section ".data") u32 lbl_80398960[2] = {
    0xBF800000, 0x3F800000
};
