#include <dolphin/types.h>

extern u8 btldefaultevt_Damage[];
extern u8 btlevtcmd_AddPartsDispOffset[];
extern u8 btlevtcmd_AddPartsRotate[];
extern u8 btlevtcmd_AddPartsRotateOffset[];
extern u8 btlevtcmd_AddRotate[];
extern u8 btlevtcmd_AnimeChangePose[];
extern u8 btlevtcmd_AnimeChangePoseFromTable[];
extern u8 btlevtcmd_AnimeChangePoseType[];
extern u8 btlevtcmd_AnimeSetPoseTable[];
extern u8 btlevtcmd_CheckPhase[];
extern u8 btlevtcmd_GetHp[];
extern u8 btlevtcmd_GetPos[];
extern u8 btlevtcmd_GetUnitWork[];
extern u8 btlevtcmd_KillUnit[];
extern u8 btlevtcmd_OffAttribute[];
extern u8 btlevtcmd_OffPartsAttribute[];
extern u8 btlevtcmd_OnAttribute[];
extern u8 btlevtcmd_OnPartsAttribute[];
extern u8 btlevtcmd_PhaseEventStartDeclare[];
extern u8 btlevtcmd_SetBaseScale[];
extern u8 btlevtcmd_SetEventAttack[];
extern u8 btlevtcmd_SetEventDamage[];
extern u8 btlevtcmd_SetEventPhase[];
extern u8 btlevtcmd_SetEventWait[];
extern u8 btlevtcmd_SetMaxMoveCount[];
extern u8 btlevtcmd_SetPartsRotateOffset[];
extern u8 btlevtcmd_SetRotate[];
extern u8 btlevtcmd_SetRotateOffset[];
extern u8 btlevtcmd_SetUnitWork[];
extern u8 btlevtcmd_StartWaitEvent[];
extern u8 btlevtcmd_snd_se[];
extern u8 evt_eff[];

extern const char str_802F93E0[16];
extern const char str_802F93F0[16];
extern const char str_802F9400[24];
extern const char str_802F9418[20];
extern const char str_802F942C[28];
extern const char str_802F9448[8];
extern const char str_802F9450[8];
extern const char str_802F9458[8];
extern const char str_802F9460[8];
extern const char str_802F9468[8];
extern const char str_802F9470[8];
extern const char str_802F9478[8];
extern const char str_802F9480[8];
extern const char str_802F9488[8];
extern const char str_802F9490[8];
extern const char str_802F9498[20];
extern const char str_802F94AC[20];
extern const char str_802F94C0[8];
extern const char str_802F94C8[8];
extern const char str_802F94D0[12];
static const u32 gap_04_802F94DC_rodata[1];
extern const u32 zero_8041e5d0[1];
static const u32 gap_09_8041E5D4_sdata2[1];
static u32 defence[2];
static u32 defence_attr[2];
static u32 regist[6];
static u32 pose_table[10];
extern u32 pose_table_crack_lv1[10];
extern u32 pose_table_crack_lv2[10];
static u32 damage_event[10];
static u32 wait_event[6];
static u32 attack_event[2];
extern u32 unit_koura[49];
static u32 data_table[14];
static u32 parts[38];
static u32 init_event[46];
static u32 phase_event[38];
extern u32 koura_open[72];
extern u32 koura_close[79];
extern u32 damage_core[92];
extern u32 pose_tbl_reset[58];
static u32 dead_event[243];
extern u32 destroy_event[47];
extern u32 lbl_80399940[2];

const char str_802F93E0[16] = "btl_un_koura\000\000\000";

const char str_802F93F0[16] = "SFX_BTL_DAMAGE1";

const char str_802F9400[24] = "SFX_BTL_DAMAGE_FIRE1\000\000\000";

const char str_802F9418[20] = "SFX_BTL_DAMAGE_ICE1";

const char str_802F942C[28] = "SFX_BTL_DAMAGE_BIRIBIRI1\000\000\000";

const char str_802F9448[8] = "PKO_S_1";

const char str_802F9450[8] = "PKO_D_1";

const char str_802F9458[8] = "PKO_Z_1";

const char str_802F9460[8] = "PKO_S_2";

const char str_802F9468[8] = "PKO_D_2";

const char str_802F9470[8] = "PKO_Z_2";

const char str_802F9478[8] = "PKO_S_3";

const char str_802F9480[8] = "PKO_D_3";

const char str_802F9488[8] = "PKO_Z_3";

const char str_802F9490[8] = "c_koura";

const char str_802F9498[20] = "SFX_BTL_NOKO_GUARD3";

const char str_802F94AC[20] = "SFX_BTL_NOKO_GUARD4";

const char str_802F94C0[8] = "PKO_H_1";

const char str_802F94C8[8] = "PKO_H_2";

const char str_802F94D0[12] = "kemuri_test";

static const u32 gap_04_802F94DC_rodata[1] = {
    0x00000000
};

const u32 zero_8041e5d0[1] = {
    0x00000000
};

static const u32 gap_09_8041E5D4_sdata2[1] = {
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

static u32 pose_table[10] = {
    0x0000001C, (u32)(void*)str_802F9448, 0x0000001F, (u32)(void*)str_802F9448, 0x00000027, (u32)(void*)str_802F9450, 0x00000044, (u32)(void*)str_802F9458,
    0x00000045, (u32)(void*)str_802F9448
};

u32 pose_table_crack_lv1[10] = {
    0x0000001C, (u32)(void*)str_802F9460, 0x0000001F, (u32)(void*)str_802F9460, 0x00000027, (u32)(void*)str_802F9468, 0x00000044, (u32)(void*)str_802F9470,
    0x00000045, (u32)(void*)str_802F9460
};

u32 pose_table_crack_lv2[10] = {
    0x0000001C, (u32)(void*)str_802F9478, 0x0000001F, (u32)(void*)str_802F9478, 0x00000027, (u32)(void*)str_802F9480, 0x00000044, (u32)(void*)str_802F9488,
    0x00000045, (u32)(void*)str_802F9478
};

static u32 damage_event[10] = {
    0x00020032, 0xFE363C8A, 0xFFFFFFFE, 0x00020032, 0xFE363C8B, 0x00000001, 0x0001005E, (u32)(void*)btldefaultevt_Damage,
    0x00000002, 0x00000001
};

static u32 wait_event[6] = {
    0x0003005B, (u32)(void*)btlevtcmd_AnimeChangePoseFromTable, 0xFFFFFFFE, 0x00000001, 0x00000002, 0x00000001
};

static u32 attack_event[2] = {
    0x00000002, 0x00000001
};

u32 unit_koura[49] = {
    0x000000DF, (u32)(void*)str_802F93E0, 0x00080000, 0x05010100, 0x00000064, 0x00090018, 0x00180018, 0x00180000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x41C80000,
    0x00000000, 0x00000000, 0x41C80000, 0x4179999A, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000, 0x00000000, 0x00000000, 0x00000000, (u32)(void*)str_802F93F0,
    (u32)(void*)str_802F9400, (u32)(void*)str_802F9418, (u32)(void*)str_802F942C, 0x10000000, (u32)(void*)regist, 0x02000000, (u32)(void*)parts, (u32)(void*)init_event,
    (u32)(void*)data_table
};

static u32 data_table[14] = {
    0x0000003B, (u32)(void*)koura_open, 0x0000003C, (u32)(void*)koura_close, 0x00000009, (u32)(void*)damage_core, 0x0000003D, (u32)(void*)pose_tbl_reset,
    0x0000003E, (u32)(void*)destroy_event, 0x00000031, (u32)(void*)dead_event, 0x00000000, 0x00000000
};

static u32 parts[38] = {
    0x00000001, (u32)(void*)str_802F93E0, (u32)(void*)str_802F9490, 0x00000000, 0x00000000, 0x00000000, 0x41A00000, 0x42200000,
    0x00000000, 0x00000000, 0x42200000, 0x00000000, 0x0014001E, 0x00FF0000, (u32)(void*)defence, (u32)(void*)defence_attr,
    0x00080009, 0x00000000, (u32)(void*)pose_table, 0x00000002, (u32)(void*)str_802F93E0, (u32)(void*)str_802F9490, 0x00000000, 0x00000000,
    0x00000000, 0x41700000, 0x41C80000, 0x00000000, 0x00000000, 0x41F00000, 0x00000000, 0x0014001E,
    0x00FF0000, (u32)(void*)defence, (u32)(void*)defence_attr, 0x03010000, 0x00000000, (u32)(void*)pose_table
};

static u32 init_event[46] = {
    0x0005005B, (u32)(void*)btlevtcmd_SetBaseScale, 0xFFFFFFFE, 0xF24A8080, 0xF24A8080, 0xF24A8080, 0x0003005B, (u32)(void*)btlevtcmd_SetEventWait,
    0xFFFFFFFE, (u32)(void*)wait_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventPhase, 0xFFFFFFFE, (u32)(void*)phase_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventAttack,
    0xFFFFFFFE, (u32)(void*)attack_event, 0x0003005B, (u32)(void*)btlevtcmd_SetEventDamage, 0xFFFFFFFE, (u32)(void*)damage_event, 0x0003005B, (u32)(void*)btlevtcmd_SetMaxMoveCount,
    0xFFFFFFFE, 0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x0004005B,
    (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000002, 0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003,
    0x00000000, 0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001
};

static u32 phase_event[38] = {
    0x0003005B, (u32)(void*)btlevtcmd_CheckPhase, 0xFE363C80, 0x04000001, 0x00020019, 0xFE363C80, 0x00000000, 0x0003005B,
    (u32)(void*)btlevtcmd_GetHp, 0xFFFFFFFE, 0xFE363C80, 0x0002001B, 0xFE363C80, 0x00000000, 0x0001005E, (u32)(void*)koura_open,
    0x00000021, 0x00000021, 0x0003005B, (u32)(void*)btlevtcmd_CheckPhase, 0xFE363C80, 0x04000003, 0x00020019, 0xFE363C80,
    0x00000000, 0x0003005B, (u32)(void*)btlevtcmd_GetHp, 0xFFFFFFFE, 0xFE363C80, 0x0002001B, 0xFE363C80, 0x00000000,
    0x0001005E, (u32)(void*)koura_close, 0x00000021, 0x00000021, 0x00000002, 0x00000001
};

u32 koura_open[72] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000001, 0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x00000002, 0x00000021, 0x0002005B, (u32)(void*)btlevtcmd_PhaseEventStartDeclare, 0xFFFFFFFE, 0x0005005B, (u32)(void*)btlevtcmd_SetRotateOffset, 0xFFFFFFFE,
    0xFFFFFFE9, 0x00000000, 0x00000000, 0x0005005B, (u32)(void*)btlevtcmd_SetRotate, 0xFFFFFFFE, 0x00000000, 0x00000000,
    0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C83, 0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute,
    0xFE363C83, 0x00000001, 0x01000000, 0x00010005, 0x0000001E, 0x0005005B, (u32)(void*)btlevtcmd_AddRotate, 0xFFFFFFFE,
    0x00000000, 0x00000000, 0x00000002, 0x00010009, 0x00000001, 0x00000006, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork,
    0xFFFFFFFE, 0x00000001, 0x00000001, 0x0003005B, (u32)(void*)btlevtcmd_OffAttribute, 0xFE363C83, 0x00000040, 0x0004005B,
    (u32)(void*)btlevtcmd_OnPartsAttribute, 0xFFFFFFFE, 0x00000001, 0x00010000, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000002,
    0x00000001, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x00000044, 0x00000002, 0x00000001
};

u32 koura_close[79] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000001, 0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x00000002, 0x00000021, 0x0002005B, (u32)(void*)btlevtcmd_PhaseEventStartDeclare, 0xFFFFFFFE, 0x0005005B, (u32)(void*)btlevtcmd_SetRotateOffset, 0xFFFFFFFE,
    0xFFFFFFE9, 0x00000000, 0x00000000, 0x0005005B, (u32)(void*)btlevtcmd_SetRotate, 0xFFFFFFFE, 0x00000000, 0x00000000,
    0x0000003C, 0x00010005, 0x0000001E, 0x0005005B, (u32)(void*)btlevtcmd_AddRotate, 0xFFFFFFFE, 0x00000000, 0x00000000,
    0xFFFFFFFE, 0x00010009, 0x00000001, 0x00000006, 0x0005005B, (u32)(void*)btlevtcmd_SetRotate, 0xFFFFFFFE, 0x00000000,
    0x00000000, 0x00000000, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x0004005B,
    (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C83, 0x0003005B, (u32)(void*)btlevtcmd_OnAttribute, 0xFE363C83, 0x00000040,
    0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000001, 0x00010000, 0x0004005B, (u32)(void*)btlevtcmd_OnPartsAttribute, 0xFE363C83,
    0x00000001, 0x01000000, 0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE,
    0x00000002, 0x00000000, 0x0002005B, (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001
};

u32 damage_core[92] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000003, 0xFE363C81, 0x0003005B, (u32)(void*)btlevtcmd_GetHp, 0xFFFFFFFE,
    0xFE363C80, 0x00010022, 0xFE363C80, 0x00010028, 0x00000003, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE,
    0x00000001, (u32)(void*)pose_table_crack_lv2, 0x00020019, 0xFE363C81, 0x00000002, 0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE,
    (u32)(void*)str_802F9498, 0xF1194D80, 0x00000000, 0xF1194D80, 0x00000021, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE,
    0x00000003, 0x00000002, 0x00010028, 0x00000006, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001,
    (u32)(void*)pose_table_crack_lv1, 0x00020019, 0xFE363C81, 0x00000001, 0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)str_802F9498,
    0xF1194D80, 0x00000000, 0xF1194D80, 0x00000021, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003,
    0x00000001, 0x0000002A, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table, 0x00020019,
    0xFE363C81, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)str_802F9498, 0xF1194D80, 0x00000000,
    0xF1194D80, 0x00000021, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003, 0x00000000, 0x00000031,
    0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePoseType, 0xFFFFFFFE, 0x00000001, 0x00000027, 0x0001000A, 0x000001F4, 0x0002005B,
    (u32)(void*)btlevtcmd_StartWaitEvent, 0xFFFFFFFE, 0x00000002, 0x00000001
};

u32 pose_tbl_reset[58] = {
    0x0003005B, (u32)(void*)btlevtcmd_GetHp, 0xFFFFFFFE, 0xFE363C80, 0x00010022, 0xFE363C80, 0x00010028, 0x00000003,
    0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_crack_lv2, 0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE,
    (u32)(void*)str_802F9498, 0xF1194D80, 0x00000000, 0xF1194D80, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003,
    0x00000002, 0x00010028, 0x00000006, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE, 0x00000001, (u32)(void*)pose_table_crack_lv1,
    0x0006005B, (u32)(void*)btlevtcmd_snd_se, 0xFFFFFFFE, (u32)(void*)str_802F9498, 0xF1194D80, 0x00000000, 0xF1194D80, 0x0004005B,
    (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003, 0x00000001, 0x0000002A, 0x0004005B, (u32)(void*)btlevtcmd_AnimeSetPoseTable, 0xFFFFFFFE,
    0x00000001, (u32)(void*)pose_table, 0x0004005B, (u32)(void*)btlevtcmd_SetUnitWork, 0xFFFFFFFE, 0x00000003, 0x00000000, 0x00000031,
    0x00000002, 0x00000001
};

static u32 dead_event[243] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C83, 0x0003005B, (u32)(void*)btlevtcmd_OffAttribute, 0xFE363C83,
    0x00000060, 0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFE363C83, 0x00000001, 0x01000000, 0x0006005B, (u32)(void*)btlevtcmd_snd_se,
    0xFFFFFFFE, (u32)(void*)str_802F94AC, 0xF1194D80, 0x00000000, 0xF1194D80, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE,
    0x00000001, (u32)(void*)str_802F94C0, 0x0004005B, (u32)(void*)btlevtcmd_AnimeChangePose, 0xFFFFFFFE, 0x00000002, (u32)(void*)str_802F94C8, 0x0004005B,
    (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000002, 0x01000000, 0x0006005B, (u32)(void*)btlevtcmd_SetPartsRotateOffset, 0xFFFFFFFE, 0x00000001,
    0x00000017, 0x00000000, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_SetPartsRotateOffset, 0xFFFFFFFE, 0x00000002, 0xFFFFFFE9,
    0x00000000, 0x00000000, 0x0000006E, 0x00010005, 0x00000005, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE,
    0x00000001, 0x00000000, 0x00000001, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000002,
    0x00000000, 0x00000001, 0x00000000, 0x00010009, 0x00000002, 0x00000006, 0x00010005, 0x00000006,
    0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x00000001, 0x00000000, 0x0006005B,
    (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000002, 0x00000000, 0x00000001, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00010005, 0x00000002, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000001, 0x00000000,
    0x00000001, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000002, 0x00000000, 0x00000001,
    0x00000000, 0x00010009, 0x00000002, 0x00000006, 0x00000070, 0x0000006E, 0x00010005, 0x0000000A,
    0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000001, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x0006005B,
    (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x00010009, 0x00000001,
    0x00000006, 0x00010005, 0x00000005, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000001, 0xFFFFFFFF,
    0x00000000, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsDispOffset, 0xFFFFFFFE, 0x00000002, 0x00000001, 0x00000000,
    0x00000000, 0x00010009, 0x00000002, 0x00000006, 0x00000070, 0x0000006E, 0x00010005, 0x0000000A,
    0x0006005B, (u32)(void*)btlevtcmd_AddPartsRotateOffset, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x00000001, 0x00000000, 0x0006005B,
    (u32)(void*)btlevtcmd_AddPartsRotateOffset, 0xFFFFFFFE, 0x00000002, 0x00000000, 0x00000001, 0x00000000, 0x00010009, 0x00000002,
    0x00000006, 0x00000070, 0x0000006E, 0x00010005, 0x0000000E, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsRotateOffset, 0xFFFFFFFE,
    0x00000001, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsRotateOffset, 0xFFFFFFFE, 0x00000002,
    0x00000001, 0x00000000, 0x00000000, 0x00010009, 0x00000001, 0x00000006, 0x00000070, 0x00010005,
    0x00000014, 0x0006005B, (u32)(void*)btlevtcmd_AddPartsRotate, 0xFFFFFFFE, 0x00000001, 0x00000000, 0x00000000, 0x00000002,
    0x0006005B, (u32)(void*)btlevtcmd_AddPartsRotate, 0xFFFFFFFE, 0x00000002, 0x00000000, 0x00000000, 0xFFFFFFFE, 0x00010009,
    0x00000001, 0x00000006, 0x00010005, 0x00000008, 0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000001,
    0x01000000, 0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFFFFFFFE, 0x00000002, 0x01000000, 0x00010009, 0x00000002,
    0x0004005B, (u32)(void*)btlevtcmd_OnPartsAttribute, 0xFFFFFFFE, 0x00000001, 0x01000000, 0x0004005B, (u32)(void*)btlevtcmd_OnPartsAttribute, 0xFFFFFFFE,
    0x00000002, 0x01000000, 0x00010009, 0x00000002, 0x00000006, 0x0003005B, (u32)(void*)btlevtcmd_KillUnit, 0xFFFFFFFE,
    0x00000000, 0x00000002, 0x00000001
};

u32 destroy_event[47] = {
    0x0004005B, (u32)(void*)btlevtcmd_GetUnitWork, 0xFFFFFFFE, 0x00000000, 0xFE363C83, 0x0003005B, (u32)(void*)btlevtcmd_OffAttribute, 0xFE363C83,
    0x00000060, 0x0004005B, (u32)(void*)btlevtcmd_OffPartsAttribute, 0xFE363C83, 0x00000001, 0x01000000, 0x0004005B, (u32)(void*)btlevtcmd_OnPartsAttribute,
    0xFFFFFFFE, 0x00000001, 0x01000000, 0x0005005B, (u32)(void*)btlevtcmd_GetPos, 0xFFFFFFFE, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x000F005B, (u32)(void*)evt_eff, (u32)(void*)zero_8041e5d0, (u32)(void*)str_802F94D0, 0x00000002, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0xF24A7E80, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x0003005B, (u32)(void*)btlevtcmd_KillUnit, 0xFFFFFFFE, 0x00000000, 0x00000002, 0x00000001
};

__declspec(section ".data") u32 lbl_80399940[2] = {
    0xBF800000, 0x3F800000
};
