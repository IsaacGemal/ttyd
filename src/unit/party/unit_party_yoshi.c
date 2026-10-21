#include "unit/party/unit_party_yoshi.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "evt/evt_cmd.h"
#include "battle/battle.h"
#include "battle/battle_sub.h"
#include "battle/battle_unit.h"
#include "battle/battle_disp.h"
#include "driver/animdrv.h"
#include "driver/msgdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mario_party.h"
#include "system.h"
#include "eff/n64/eff_sweat_n64.h"
#include "eff/eff_kemutest.h"
#include "driver/dispdrv.h"
#include "pmario_sound.h"
#include <string.h>
#include "battle/battle_attack_audience.h"
#include "battle/battle_camera.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_event_default.h"
#include "battle/battle_seq_command.h"
#include "battle/battle_weapon_power.h"
#include "eff/eff_funemizu.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "evt/evt_audience.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"
#include "unit/party/unit_party_chuchurina.h"
#pragma pool_data on // the original addresses its .rodata through one base register

// A pose-sound table entry. No header declares this type; the layout matches the
// 0x14-byte entries of `posesound_normal_attack` and the fields BtlUnit_ControlPoseSoundMain
// reads (pose name 0x0, volume 0x4, callback 0x8, sound name 0xC, pitch 0x10, flags 0x12).
typedef struct PoseSoundEntry {
    const char* poseName;                                   // 0x0
    f32 volume;                                             // 0x4
    s32 (*callback)(BattleUnit*, struct PoseSoundEntry*, void*); // 0x8
    const char* soundName;                                  // 0xC
    s16 pitch;                                              // 0x10
    u16 flags;                                              // 0x12
} PoseSoundEntry;

extern u32 lbl_80379A00[2];
#pragma force_active on
extern s32 negone_one[];
extern s32 one_negone[];
extern s32 weaponGetPowerFromPartyAttackLv[];
static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count);
static s32 init_event[54];
static s32 damage_event[16];
u32 partyWeapon_YoshiNormalAttack[48];
u32 partyWeapon_YoshiNomikomi_Shot[48];
u32 partyWeapon_YoshiEggAttack_Minimini[48];
u32 partyWeapon_YoshiCallGuard[48];
s32 partyYoshiAttack_NormalAttack[1025];
s32 partyYoshiAttack_Nomikomi[1000];
s32 partyYoshiAttack_EggAttack[1266];
s32 _egg_attack_event[195];
s32 partyYoshiAttack_CallGuard[553];
s32 yoshi_appeal_event[39];
static s32 party_win_reaction[25];
static s32 attack_audience_event[290];
static s32 attack_event[2];
static s32 wait_event[6];

// clang-format off


#define TECH_MENU_ENTRY(table)                                                        \
    cmd->mWeaponTable[*count].mIndex = -1;                                            \
    cmd->mWeaponTable[*count].mItem = 0;                                              \
    cmd->mWeaponTable[*count].mWeaponParams = (BattleWeapon*)(table);                 \
    cmd->mWeaponTable[*count].field_0x4 = 0;                                          \
    cmd->mWeaponTable[*count].mIcon =                                                 \
        *(u16*)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 4);                    \
    cmd->mWeaponTable[*count].mName =                                                 \
        (char*)msgSearch(*(char**)((u8*)cmd->mWeaponTable[*count].mWeaponParams + 0)); \
    (*count)++;

__declspec(section ".data") u32 lbl_80379A00[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A10[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A20[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_80379A30[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 pose_table_yoshi_stay[50] = {
    0x00000009, (u32)"PYS_D_5", 0x00000002, (u32)"PYS_D_5",
    0x00000003, (u32)"PYS_X_1", 0x00000004, (u32)"PYS_D_2",
    0x00000005, (u32)"PYS_X_1", 0x00000001, (u32)"PYS_N_1",
    0x0000001B, (u32)"PYS_D_3", 0x0000001C, (u32)"PYS_S_1",
    0x00000025, (u32)"PYS_G_2", 0x0000001F, (u32)"PYS_G_1",
    0x00000027, (u32)"PYS_D_1", 0x00000028, (u32)"PYS_W_1",
    0x0000002A, (u32)"PYS_R_1", 0x00000039, (u32)"PYS_D_2",
    0x00000038, (u32)"PYS_D_2", 0x0000003C, (u32)"PYS_D_4",
    0x0000003D, (u32)"PYS_D_2", 0x00000041, (u32)"PYS_T_1",
    0x00000034, (u32)"PYS_I_1", 0x00000043, (u32)"PYS_C_1",
    0x00000042, (u32)"PYS_O_1", 0x00000040, (u32)"PYS_F_1",
    0x00000031, (u32)"PYS_F_1", 0x0000002D, (u32)"PYS_G_1",
    0x00000045, (u32)"PYS_S_1",
};

// TODO: type
static u32 data_table[16] = {
    0x00000005, (u32)&yoshi_appeal_event, 0x00000006, (u32)&attack_audience_event,
    0x00000001, (u32)__makeTechMenuFunc, 0x00000030, (u32)btldefaultevt_Dummy,
    0x00000040, (u32)&party_win_reaction, 0x00000007, (u32)subset_attack_ready_jump_event,
    0x0000002F, (u32)btldefaultevt_avoid_counter_event, 0x00000000, 0x00000000,
};

// TODO: type
static u32 defence[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 defence_attr[2] = {
    0x00000000, 0x00000000,
};

// TODO: type
static u32 regist[6] = {
    0x64646464, 0x64646464, 0x64646464, 0x64646464,
    0x64646464, 0x64640000,
};

// TODO: type
u32 pose_table_egg_g[8] = {
    0x0000001C, (u32)"CHU_C_1", 0x0000001F, (u32)"CHU_C_1",
    0x00000027, (u32)"CHU_C_1", 0x00000045, (u32)"CHU_C_1",
};

// TODO: type
u32 pose_table_egg_y[8] = {
    0x0000001C, (u32)"CHU_C_2", 0x0000001F, (u32)"CHU_C_2",
    0x00000027, (u32)"CHU_C_2", 0x00000045, (u32)"CHU_C_2",
};

// TODO: type
u32 pose_table_egg_p[8] = {
    0x0000001C, (u32)"CHU_C_3", 0x0000001F, (u32)"CHU_C_3",
    0x00000027, (u32)"CHU_C_3", 0x00000045, (u32)"CHU_C_3",
};

// TODO: type
u32 unitpartsdata_Party_Yoshi[133] = {
    0x00000001, (u32)"btl_un_yoshi", (u32)"c_babyyoshi", 0x00000000,
    0x00000000, 0x00000000, 0x41800000, 0x41C00000,
    0x40A00000, 0x00000000, 0x41C00000, 0x00000000,
    0x00140018, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x00000009, 0x00000000, (u32)&pose_table_yoshi_stay, 0x00000002,
    (u32)"btl_un_yoshi", (u32)"c_tamago", 0x00000000, 0x00000000,
    0x00000000, 0x41800000, 0x41C00000, 0x40A00000,
    0x00000000, 0x41C00000, 0x00000000, 0x00140018,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table_egg_g, 0x00000003, (u32)"btl_un_yoshi",
    (u32)"c_tamago", 0x00000000, 0x00000000, 0x00000000,
    0x41800000, 0x41C00000, 0x40A00000, 0x00000000,
    0x41C00000, 0x00000000, 0x00140018, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table_egg_g, 0x00000004, (u32)"btl_un_yoshi", (u32)"c_tamago",
    0x00000000, 0x00000000, 0x00000000, 0x41800000,
    0x41C00000, 0x40A00000, 0x00000000, 0x41C00000,
    0x00000000, 0x00140018, 0x00FF0000, (u32)&defence,
    (u32)&defence_attr, 0x13010000, 0x00000000, (u32)&pose_table_egg_g,
    0x00000005, (u32)"btl_un_yoshi", (u32)"c_tamago", 0x00000000,
    0x00000000, 0x00000000, 0x41800000, 0x41C00000,
    0x40A00000, 0x00000000, 0x41C00000, 0x00000000,
    0x00140018, 0x00FF0000, (u32)&defence, (u32)&defence_attr,
    0x13010000, 0x00000000, (u32)&pose_table_egg_g, 0x00000006,
    (u32)"btl_un_yoshi", (u32)"c_tamago", 0x00000000, 0x00000000,
    0x00000000, 0x41800000, 0x41C00000, 0x40A00000,
    0x00000000, 0x41C00000, 0x00000000, 0x00140018,
    0x00FF0000, (u32)&defence, (u32)&defence_attr, 0x13010000,
    0x00000000, (u32)&pose_table_egg_g, 0x00000007, (u32)"btl_un_yoshi",
    (u32)"c_tamago", 0x00000000, 0x00000000, 0x00000000,
    0x41800000, 0x41C00000, 0x40A00000, 0x00000000,
    0x41C00000, 0x00000000, 0x00140018, 0x00FF0000,
    (u32)&defence, (u32)&defence_attr, 0x13010000, 0x00000000,
    (u32)&pose_table_egg_g,
};

// TODO: type
u32 unitdata_Party_Yoshi[49] = {
    0x000000E2, (u32)"btl_un_yoshi", 0x000A0005, 0x05010100,
    0x00000064, 0x00090014, 0x00210008, 0x001C0000,
    0x00000000, 0x41400000, 0x00000000, 0x00000000,
    0x00000000, 0x42200000, 0x00000000, 0x00000000,
    0x42200000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x41200000,
    0x00000000, 0x00000000, 0x41200000, 0x41AB999A,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x0000FF00, 0x64140000,
    0x00000000, 0x00000000, 0x00000000, (u32)"SFX_PARTY_DAMAGE1",
    (u32)"SFX_MARIO_DAMAGE_FIRE1", (u32)"SFX_MARIO_DAMAGE_ICE1", (u32)"SFX_MARIO_DAMAGE_BIRIBIRI1", 0x02000000,
    (u32)&regist, 0x07000000, (u32)&unitpartsdata_Party_Yoshi, (u32)&init_event,
    (u32)&data_table,
};

static EVT_BEGIN(battle_entry_event)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 250)
    CALL(btlevtcmd_SetPos, -2, LW(0), LW(1), LW(2))
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_SetMoveSoundDataWork, -2, STRING("SFX_PARTY_BATTLE_SUBMOVE1L"), STRING("SFX_PARTY_BATTLE_SUBMOVE1R"), 0, 6, 6, 0, 0)
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -2, 0)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

static EVT_BEGIN(init_event)
    CALL(btlevtcmd_SetEventWait, -2, PTR(wait_event))
    CALL(btlevtcmd_SetEventAttack, -2, PTR(attack_event))
    CALL(btlevtcmd_SetEventDamage, -2, PTR(damage_event))
    CALL(btlevtcmd_SetEventConfusion, -2, PTR(btldefaultevt_Confuse))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    CALL(btlevtcmd_SetWalkSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 15, 13)
    CALL(btlevtcmd_SetRunSound, -2, STRING("SFX_PARTY_BATTLE_MOVE1L"), STRING("SFX_PARTY_BATTLE_MOVE1R"), 0, 6, 6)
    CALL(btlevtcmd_SetJumpSound, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), STRING("SFX_PARTY_BATTLE_LANDING1"))
    CALL(yoshi_original_color_anim_set, -2)
    CALL(btlevtcmd_SetEventEntry, -2, PTR(battle_entry_event))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(damage_event)
    CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
    SET_VALUE(LW(10), -2)
    SET_VALUE(LW(11), 1)
    CALL_CHILD(btldefaultevt_Damage)
    RETURN
EVT_END()

// TODO: type
u32 partyWeapon_YoshiNormalAttack[48] = {
    (u32)"btl_wn_pys_normal", 0x01500000, 0x00000000, (u32)"msg_pys_hip_drop",
    0x64000000, 0x3F800000, 0x01016432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000001, 0x00000001, 0x00000001,
    0x00000001, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01001260, 0x20001004, 0x00000302,
    (u32)"msg_ac_hip_drop", 0x00035097, 0x0000000C, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_NormalAttack, 0x000A0A64, 0x0A050505, 0x05050000,
};

// TODO: type
u32 partyWeapon_YoshiNomikomi_Shot[48] = {
    (u32)"btl_wn_pys_lv1", 0x01510000, 0x00000000, (u32)"msg_pys_nomikomi",
    0x64040000, 0x3F800000, 0x01056432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000002, 0x00000004, 0x00000003, 0x00000005,
    0x00000003, 0x00000006, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00060302,
    (u32)"msg_ac_nomikomi", 0x00020047, 0x000007FB, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_Nomikomi, 0x00000064, 0x00050500, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiNomikomi_Spew[48] = {
    (u32)"btl_wn_pys_lv1", 0x01510000, 0x00000000, (u32)"msg_pys_nomikomi",
    0x64040000, 0x3F800000, 0x01056432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000002, 0x00000004, 0x00000003, 0x00000005,
    0x00000003, 0x00000006, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00050302,
    (u32)"msg_ac_nomikomi", 0x00020047, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_Nomikomi, 0x00000064, 0x00050505, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiNomikomi_Dmg0[48] = {
    (u32)"btl_wn_pys_lv1", 0x01510000, 0x00000000, (u32)"msg_pys_nomikomi",
    0x64020000, 0x3F800000, 0x01056432, (u32)weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01101260, 0x21002000, 0x00000302,
    (u32)"msg_ac_nomikomi", 0x00020040, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_Nomikomi, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiNomikomi_Fire[48] = {
    (u32)"btl_wn_pys_lv1", 0x01510000, 0x00000000, (u32)"msg_pys_nomikomi",
    0x64020000, 0x3F800000, 0x01056432, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x01000302,
    (u32)"msg_ac_nomikomi", 0x00020040, 0x000007FF, 0x80000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_Nomikomi, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiNomikomi_Involved[48] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000000, 0x00000000, 0x00000000, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000004, 0x00000002, 0x00000005,
    0x00000003, 0x00000006, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100260, 0x20102000, 0x00000302,
    0x00000000, 0x00020040, 0x000007FF, 0x00000004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiEggAttack_Minimini[48] = {
    (u32)"btl_wn_pys_lv2", 0x01520000, 0x00000000, (u32)"msg_pys_wonder_egg",
    0x64030000, 0x3F800000, 0x02056432, (u32)weaponGetPowerDefault,
    0x00000001, 0x00000001, 0x00000001, 0x00000001,
    0x00000001, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    (u32)"msg_ac_wonder_egg", 0x00020003, 0x000007FF, 0x80002000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x03FE0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_EggAttack, 0x00000064, 0x00030300, 0x00000000,
};

// TODO: type
u32 partyWeapon_YoshiCallGuard[48] = {
    (u32)"btl_wn_pys_lv3", 0x01530000, 0x00000000, (u32)"msg_pys_taigun_yoshi",
    0x64060000, 0x3F800000, 0x01056432, (u32)weaponGetPowerFromPartyAttackLv,
    0x00000001, 0x00000001, 0x00000001, 0x00000001,
    0x00000001, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02101260, 0x20000000, 0x00000302,
    (u32)"msg_ac_taigun_yoshi", 0x00020017, 0x000007FF, 0x80002004,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&partyYoshiAttack_CallGuard, 0x00141464, 0x14140505, 0x14050000,
};

// TODO: type
u32 posesound_normal_attack[75] = {
    (u32)"PYS_W_1", 0x3C23D70A, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L",
    0x00000001, (u32)"PYS_W_1", 0x3E800000, (u32)_yoshi_slide_move_sound,
    (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x00000001, (u32)"PYS_R_1", 0x3C23D70A,
    (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L", 0x00000001, (u32)"PYS_R_1",
    0x3DCCCCCD, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x00000001,
    (u32)"PYS_R_4", 0x3C23D70A, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L",
    0x05540001, (u32)"PYS_R_4", 0x3D4CCCCD, (u32)_yoshi_slide_move_sound,
    (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x05540001, (u32)"PYS_R_4", 0x3DCCCCCD,
    (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L", 0x05540001, (u32)"PYS_R_4",
    0x3E19999A, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x05540001,
    (u32)"PYS_R_5", 0x3C23D70A, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L",
    0x09530001, (u32)"PYS_R_5", 0x3D088889, (u32)_yoshi_slide_move_sound,
    (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x09530001, (u32)"PYS_R_5", 0x3DAAAAAB,
    (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L", 0x09530001, (u32)"PYS_R_5",
    0x3DEEEEEF, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x09530001,
    (u32)"PYS_A_3", 0x3C23D70A, (u32)_yoshi_slide_move_sound, (u32)"SFX_PARTY_BATTLE_MOVE1L",
    0x09530001, (u32)"PYS_A_3", 0x3DAAAAAB, (u32)_yoshi_slide_move_sound,
    (u32)"SFX_PARTY_BATTLE_MOVE1R", 0x09530001, 0x00000000, 0x00000000,
    (u32)_yoshi_slide_move_sound, 0x00000000, 0x00000001,
};

EVT_BEGIN(partyYoshiAttack_NormalAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, -1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_SetUnitWork, -2, 2, 1)
    CALL(btlevtcmd_SetUnitWork, -2, 3, -1)
    CALL(btlevtcmd_SetUnitWork, -2, 4, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 5, 100)
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, PTR(posesound_normal_attack))
    CALL(btlevtcmd_AcSetOutputParam, 1, 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_W_1"))
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(0), 0)
        LOOP(25)
            ADD(LW(0), 4)
            SET_VALUE(LW(1), 2387)
            MULTIPLY(LW(1), LW(0))
            DIVIDE(LW(1), 100)
            CALL(btlevtcmd_SetUnitWork, -2, 4, LW(1))
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_R_5"))
    WAIT_FRAMES(25)
    CALL(evt_btl_camera_set_mode, 0, 11)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, LW(3), LW(4))
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetTakeoffPosition, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1A"))
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.3f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 25)
    ADD(LW(1), 35)
    CALL(btlevtcmd_snd_se, -2, STRING("SFX_PARTY_BATTLE_JUMP1"), EVTDAT_POINTER_MAX, 0, EVTDAT_POINTER_MAX)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_CommandGetWeaponActionLv, LW(15))
        CALL(btlevtcmd_AcSetDifficulty, -2, LW(15))
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 20, 61, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
            CASE_EQUAL(1)
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 25, 76, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 75, 100, 100, 100)
            CASE_DEFAULT
            CALL(btlevtcmd_AcSetParamAll, 5, 180, 178, 23, 25, 34, 100, 1)
            CALL(btlevtcmd_AcSetGaugeParam, 100, 100, 100, 100)
        END_SWITCH
        CALL(btlevtcmd_AcSetFlag, 72)
        CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 25, -1)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_3"))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(6), LW(7), LW(8))
    ADD(LW(7), 40)
    SET_VALUE(LW(10), FLOAT(1.0f))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL(btlevtcmd_StartAC, 1)
        CALL(btlevtcmd_ResultAC)
    END_SIBLING_EVENT
    CALL(btl_yoshi_yoroyoro_jump_calc_param, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    CALL(btl_yoshi_yoroyoro_jump_move, -2, LW(6), LW(7), LW(8), LW(10), LW(9))
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    SET_VALUE(LW(13), 1)
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(14))
    LABEL(50)
    IF_INT_GREATER_OR_EQUAL(LW(13), 2)
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 1048832, LW(5))
    ELSE
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    SWITCH(LW(5))
        CASE_OR_EQUAL(2)
        CASE_OR_EQUAL(3)
        CASE_OR_EQUAL(6)
        CASE_OR_EQUAL(4)
        CASE_END
        CASE_EQUAL(1)
        CASE_DEFAULT
    END_SWITCH
    CALL(btlevtcmd_AnimeSetPoseSoundTable, -2, 1, 0)
    CALL(btlevtcmd_SetUnitWork, -2, 2, 0)
    IF_INT_LESS(LW(13), 2)
        CALL(btlevtcmd_ACRStart, -2, 0, 20, 20, 0)
        CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    ELSE
        SET_VALUE(LW(6), 1)
    END_IF
    IF_INT_EQUAL(LW(13), 1)
        CALL(btlevtcmd_GetFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1B"))
        IF_INT_EQUAL(LW(6), 2)
            SET_VALUE(LW(0), 3)
            SET_VALUE(LW(1), 6)
            MULTIPLY(LW(15), -60)
        ELSE
            SET_VALUE(LW(0), 1)
            SET_VALUE(LW(1), 9)
            MULTIPLY(LW(15), -40)
        END_IF
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 12, 0)
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        WAIT_FRAMES(15)
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, -60, -25, 0)
        END_IF
        LOOP(LW(0))
            SET_VALUE(LW(2), LW(1))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_ATTACK1"), 0)
            LOOP(LW(2))
                CALL(btlevtcmd_AddRotate, -2, 0, 0, LW(15))
                WAIT_FRAMES(1)
            END_LOOP
        END_LOOP
        CALL(btlevtcmd_SetRotate, -2, 0, 0, 0)
        CALL(btlevtcmd_SetRotateOffset, -2, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
        WAIT_FRAMES(15)
    ELSE
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_1"))
    END_IF
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), 4, 0, 0, 0, -1)
    IF_INT_NOT_EQUAL(LW(5), 1)
        CALL(btlevtcmd_StopAC)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        SET_VALUE(LW(15), LW(1))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        SUBTRACT(LW(15), LW(1))
        DIVIDE(LW(15), 10)
        CALL(btlevtcmd_DivePosition, -2, LW(0), LW(1), LW(2), LW(15), 0, 0, 0, -1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_D_1"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, -7, 0)
        CALL(evt_btl_camera_shake_h, 0, 4, 0, 10, 13)
        WAIT_FRAMES(40)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
        CALL(btlevtcmd_SetDispOffset, -2, 0, 0, 0)
        WAIT_FRAMES(54)
        CALL(evt_audience_ap_recovery)
        CALL(btlevtcmd_InviteApInfoReport)
        GOTO(95)
    END_IF
    CALL(btlevtcmd_ac_timing_flag_onoff, 1, 1)
    WAIT_FRAMES(2)
    CALL(btlevtcmd_ac_timing_flag_onoff, 0, 1)
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_SET(LW(0), 2)
        IF_INT_GREATER_OR_EQUAL(LW(13), LW(14))
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131328, LW(5))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 131072, LW(5))
        END_IF
        CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
        SWITCH(LW(0))
            CASE_EQUAL(2)
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(6)
                SET_VALUE(LW(0), 2)
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
            CASE_EQUAL(1)
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
            CASE_DEFAULT
            CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
            SWITCH(LW(0))
                CASE_GREATER_OR_EQUAL(4)
                SET_VALUE(LW(0), 1)
                CASE_GREATER_OR_EQUAL(2)
                SET_VALUE(LW(0), 0)
                CASE_DEFAULT
                SET_VALUE(LW(0), -1)
            END_SWITCH
        END_SWITCH
        IF_INT_GREATER_OR_EQUAL(LW(0), 0)
            SET_VALUE(LW(1), LW(0))
            SET_VALUE(LW(2), LW(13))
            SUBTRACT(LW(2), 1)
            IF_INT_GREATER(LW(1), LW(2))
                SET_VALUE(LW(1), LW(2))
            END_IF
            CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(1), LW(6))
            CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(9), LW(1), LW(2))
            CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(9), LW(1), LW(2))
            SET_VALUE(LW(1), LW(13))
            ADD(LW(1), 1)
            IF_INT_GREATER(LW(1), LW(14))
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
            END_IF
        ELSE
            CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
        END_IF
    ELSE
        IF_INT_GREATER_OR_EQUAL(LW(13), LW(14))
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
        ELSE
            CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 0, LW(5))
        END_IF
        CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
    END_IF
    LABEL(90)
    ADD(LW(13), 1)
    IF_INT_LESS_OR_EQUAL(LW(13), LW(14))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.2f))
        ADD(LW(1), 40)
        CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 20, -1)
        GOTO(50)
    END_IF
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_SetFallAccel, -2, FLOAT(0.7f))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetHomePos, -2, EVTDAT_POINTER_MAX, LW(1), EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 60)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 30, -1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
    END_SIBLING_EVENT
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), 25)
    CALL(btlevtcmd_JumpPosition, -2, LW(0), LW(1), LW(2), 16, -1)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyYoshiAttack_Nomikomi)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 0)
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(12))
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(15))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(15))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 160)
        SET_VALUE(LW(1), 40)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 140)
        SET_VALUE(LW(1), 30)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 120)
        SET_VALUE(LW(1), 25)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 100)
        SET_VALUE(LW(1), 20)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 80)
        SET_VALUE(LW(1), 10)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 50)
        SET_VALUE(LW(1), 5)
        CASE_DEFAULT
        SET_VALUE(LW(0), 30)
        SET_VALUE(LW(1), 2)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 3, LW(0), LW(1), 0, 100, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 15)
    CALL(btlevtcmd_SetupAC, -2, 15, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 250)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_StartAC, 1)
    LABEL(5)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(0))
    IF_INT_EQUAL(LW(0), 0)
        WAIT_FRAMES(1)
        GOTO(5)
    END_IF
    CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    IF_INT_EQUAL(LW(5), 5)
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        MULTIPLY_FLOAT(LW(6), 40)
        CALL(_get_nomikomi_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
        CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
        CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 80, -1, 0)
        CALL(btlevtcmd_CommandCheckDamage, -2, LW(3), LW(4), 256, LW(5))
    END_IF
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 40)
    CALL(_get_nomikomi_hit_position, -2, LW(3), LW(4), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, LW(3), LW(0), LW(6))
    CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 40)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(3.0f))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 80, -1, 0)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_2"))
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW1"), 0)
    IF_INT_NOT_EQUAL(LW(5), 1)
        IF_INT_EQUAL(LW(5), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(5), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(5), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        WAIT_FRAMES(50)
        GOTO(90)
    END_IF
    WAIT_FRAMES(7)
    CALL(_get_swallow_param, LW(3), LW(7))
    IF_INT_EQUAL(LW(7), -1)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(partyWeapon_YoshiNomikomi_Dmg0), 0x80000100, LW(5))
        WAIT_FRAMES(43)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_GetResultAC, LW(0))
    IF_BITS_CLEAR(LW(0), 2)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(partyWeapon_YoshiNomikomi_Dmg0), 0x80000100, LW(5))
        WAIT_FRAMES(43)
        GOTO(90)
    END_IF
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW2"), 0)
    CALL(_check_swallow_attribute, LW(3), 8, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 7, 1)
        WAIT_FRAMES(13)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_7"))
        CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW5"), 0)
        WAIT_FRAMES(34)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
        WAIT_FRAMES(54)
        GOTO(90)
    END_IF
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 350)
    CALL(btlevtcmd_DamageDirect, LW(3), LW(4), 0, 0, 2, 1)
    WAIT_FRAMES(3)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 12)
    CALL(btlevtcmd_DivePosition, LW(3), LW(0), LW(1), LW(2), 5, 0, 0, 0, -1)
    CALL(btlevtcmd_OnAttribute, LW(3), 16777216)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW3"), 0)
    WAIT_FRAMES(6)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_3"))
    WAIT_FRAMES(7)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_sub_random, 99, LW(0))
    IF_INT_GREATER_OR_EQUAL(LW(0), LW(7))
        BEGIN_INLINE_EVENT
            CALL(evt_env_blur_on, 0, 600)
        END_INLINE_EVENT
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_4"))
        CALL(btlevtcmd_ftomsec, 22, LW(0))
        WAIT_MS(LW(0))
        BEGIN_INLINE_EVENT_GET_ID(LW(15))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_NOMIKOMI6"), 0)
            CALL(btlevtcmd_OffAttribute, LW(3), 16777216)
            CALL(_check_swallow_attribute, LW(3), 2, LW(0))
            IF_INT_EQUAL(LW(0), 0)
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(partyWeapon_YoshiNomikomi_Spew), 0x80000100, LW(5))
            ELSE
                CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), PTR(partyWeapon_YoshiNomikomi_Shot), 0x80000100, LW(5))
            END_IF
            CALL(btlevtcmd_GetResultAC, LW(0))
            IF_BITS_SET(LW(0), 2)
                CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
                CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
            END_IF
            CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
            CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
            CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
            SWITCH(LW(6))
                CASE_GREATER_OR_EQUAL(2)
                CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
                CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
                CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
                CASE_DEFAULT
                CALL(evt_audience_acrobat_notry)
            END_SWITCH
        END_INLINE_EVENT
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        GOTO(90)
    END_IF
    CALL(btlevtcmd_SetUnitWork, -2, 1, 1)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetResultPrizeLv, LW(3), 0, LW(6))
        CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
        CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW5"), 0)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_7"))
    WAIT_FRAMES(26)
    CALL(_check_swallow_attribute, LW(3), 1, LW(1))
    CALL(_check_swallow_attribute, LW(3), 4, LW(2))
    CALL(btlevtcmd_GetExp, LW(3), LW(0))
    CALL(btlevtcmd_StoreExp, LW(0))
    CALL(btlevtcmd_KillUnit, LW(3), 0)
    WAIT_FRAMES(8)
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_6"))
        CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW7"), 0)
        WAIT_FRAMES(50)
        GOTO(90)
    ELSE
        IF_INT_EQUAL(LW(2), 0)
            CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
            CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_SWALLOW4"), 0)
            WAIT_FRAMES(54)
            GOTO(90)
        END_IF
    END_IF
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_4"))
    WAIT_FRAMES(22)
    BEGIN_SIBLING_EVENT
        WAIT_FRAMES(18)
    END_SIBLING_EVENT
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(0), 15)
    ADD(LW(1), 15)
    SET_VALUE(LW(3), LW(0))
    SET_VALUE(LW(4), LW(1))
    SET_VALUE(LW(5), LW(2))
    CALL(btlevtcmd_FaceDirectionAdd, -2, LW(3), 240)
    CALL(evt_eff, STRING("eff"), STRING("breath_fire"), 1, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), 1, 2, 60, FLOAT(0.5f), FLOAT(30.0f))
    WAIT_FRAMES(60)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiNomikomi_Fire))
    CALL(btlevtcmd_GetSelectEnemy, LW(10), LW(11))
    LABEL(10)
    IF_INT_EQUAL(LW(10), -1)
        GOTO(90)
    END_IF
    CALL(btlevtcmd_CheckDamage, -2, LW(10), LW(11), PTR(partyWeapon_YoshiNomikomi_Fire), 0x80000100, LW(5))
    CALL(btlevtcmd_GetSelectNextEnemy, LW(10), LW(11))
    GOTO(10)
    LABEL(90)
    CALL(btlevtcmd_StopAC)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(4.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_GetUnitWork, -2, 1, LW(5))
    IF_INT_EQUAL(LW(5), 0)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(4.0f), LW(5))
    IF_INT_LESS_OR_EQUAL(LW(5), 20)
        CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
        GOTO(99)
    END_IF
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        SUBTRACT(LW(5), 1)
        SET_VALUE(LW(6), LW(5))
        SUBTRACT(LW(6), 10)
        CALL(btlevtcmd_ACRStart, -2, LW(6), LW(5), LW(5), 0)
    END_SIBLING_EVENT
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), LW(5), -1, 0)
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    IF_INT_EQUAL(LW(6), 2)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_O_1"))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        WAIT_FRAMES(20)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_J_1B"))
        CALL(evt_sub_intpl_init, 4, 0, 360, 30)
        LOOP(30)
            CALL(evt_sub_intpl_get_value)
            CALL(btlevtcmd_SetRotate, -2, 0, LW(0), 0)
            WAIT_FRAMES(1)
        END_LOOP
    END_IF
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(partyYoshiAttack_EggAttack)
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        GOTO(99)
    END_IF
    CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
    CALL(btlevtcmd_WeaponAftereffect, LW(0))
    CALL(btlevtcmd_AttackDeclare, -2, LW(3), LW(4))
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_CommandPayWeaponCost, -2)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(3), LW(4))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 3, LW(3))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 4, LW(4))
    CALL(btlevtcmd_SetPartsWork, -2, 2, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 7, 0)
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 7, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 6, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 5, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 4, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 7, 0)
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), PTR(partyWeapon_YoshiEggAttack_Minimini))
    CALL(btlevtcmd_ChoiceSamplingEnemy, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(10), LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 3, LW(10))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 4, LW(11))
    CALL(btlevtcmd_SetPartsWork, -2, 3, 5, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 6, 0)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 7, 0)
    CALL(evt_btl_camera_set_mode, 0, 10)
    CALL(evt_btl_camera_set_homing_unitparts, 0, -2, 1, -1, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    CALL(evt_btl_camera_set_zoom, 0, 300)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_GetPartyTechLv, -2, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(2)
        SET_VALUE(LW(1), 4)
        CASE_DEFAULT
        SET_VALUE(LW(1), 3)
    END_SWITCH
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 480)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 360)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 180)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 150)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 100)
        CASE_DEFAULT
        SET_VALUE(LW(0), 60)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 0, LW(0), LW(1), 0, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetFlag, 0)
    CALL(btlevtcmd_AcSetOutputParam, 1, 0)
    CALL(btlevtcmd_SetupAC, -2, 20, 1, 60)
    WAIT_FRAMES(22)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 272, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_StartAC, 1)
    SET_VALUE(LW(12), 2)
    LABEL(5)
    CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
    ADD(LW(13), 1)
    IF_INT_GREATER(LW(12), LW(13))
        CALL(btlevtcmd_AcGetOutputParam, 1, LW(0))
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(btlevtcmd_StopAC)
            WAIT_MS(500)
            CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
            IF_INT_GREATER_OR_EQUAL(LW(13), 1)
                CALL(btlevtcmd_AcGetOutputParam, 0, LW(13))
                ADD(LW(13), 1)
                GOTO(25)
            ELSE
                CALL(btlevtcmd_AudienceDeclareACResult, PTR(partyWeapon_YoshiEggAttack_Minimini), -1)
                CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_5"))
                WAIT_FRAMES(54)
                CALL(evt_audience_ap_recovery)
                CALL(btlevtcmd_InviteApInfoReport)
                GOTO(95)
            END_IF
        END_IF
        WAIT_FRAMES(1)
        GOTO(5)
    END_IF
    SET_VALUE(LW(0), LW(12))
    SUBTRACT(LW(0), 1)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(2)
        SET_VALUE(LW(0), 0)
        CASE_LESS_OR_EQUAL(3)
        SET_VALUE(LW(0), 1)
        CASE_DEFAULT
        SET_VALUE(LW(0), 2)
    END_SWITCH
    CALL(btlevtcmd_GetResultPrizeLv, -5, LW(0), LW(6))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 40)
    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_AudienceDeclareACResult, PTR(partyWeapon_YoshiEggAttack_Minimini), LW(6))
    LABEL(10)
    CALL(evt_sub_random, 2, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_y))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_p))
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_g))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(12), 69)
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 1, 4)
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 2, PTR(partyWeapon_YoshiEggAttack_Minimini))
    LABEL(20)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    CALL(btlevtcmd_SetPartsScale, -2, LW(12), FLOAT(1.0f), FLOAT(1.0f), FLOAT(1.0f))
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_8"))
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_12"))
    CALL(btlevtcmd_GetStatusMg, -2, LW(5))
    MULTIPLY_FLOAT(LW(5), 10)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(5))
    ADD(LW(1), LW(5))
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_SetPartsDispOffset, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_SetPartsRotate, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 0, 0)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG1"), 0)
    SET_VALUE(LW(15), 0)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 6, 0)
        LOOP(30)
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
        CALL(btlevtcmd_SetPartsRotate, -2, LW(12), 0, 0, 0)
    END_SIBLING_EVENT
    BEGIN_SIBLING_EVENT
        SET_VALUE(LW(5), LW(12))
        SUBTRACT(LW(5), 2)
        MULTIPLY_FLOAT(LW(5), 10)
        ADD_FLOAT(LW(5), 15)
        CALL(btlevtcmd_GetStatusMg, -2, LW(0))
        MULTIPLY_FLOAT(LW(5), LW(0))
        CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_FaceDirectionSub, -2, LW(0), LW(5))
        CALL(btlevtcmd_TransStageFloorPosition, LW(0), LW(1), LW(2))
        CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.2f))
        CALL(btlevtcmd_JumpPartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 30, -1)
    END_SIBLING_EVENT
    WAIT_FRAMES(10)
    ADD(LW(12), 1)
    GOTO(5)
    LABEL(25)
    CALL(btlevtcmd_SetPartsWork, -2, 2, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 3, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 4, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 5, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 6, 6, -1)
    CALL(btlevtcmd_SetPartsWork, -2, 7, 6, -1)
    SET_VALUE(LW(12), LW(13))
    LABEL(26)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(0))
    IF_INT_EQUAL(LW(0), -1)
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 6, 1)
        SET_VALUE(LW(14), LW(12))
        LABEL(27)
        SUBTRACT(LW(14), 1)
        IF_INT_GREATER_OR_EQUAL(LW(14), 2)
            CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
            CALL(btlevtcmd_GetPartsWork, -2, LW(14), 3, LW(1))
            IF_INT_EQUAL(LW(0), LW(1))
                CALL(btlevtcmd_SetPartsWork, -2, LW(14), 6, 0)
            END_IF
            GOTO(27)
        END_IF
    END_IF
    SUBTRACT(LW(12), 1)
    IF_INT_GREATER_OR_EQUAL(LW(12), 2)
        GOTO(26)
    END_IF
    SET_VALUE(LW(12), 2)
    LABEL(28)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(0)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 256, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
        IF_INT_EQUAL(LW(6), 1)
            SET_VALUE(LW(14), LW(12))
            LABEL(29)
            ADD(LW(14), 1)
            IF_INT_LESS_OR_EQUAL(LW(14), LW(13))
                CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
                CALL(btlevtcmd_GetPartsWork, -2, LW(14), 3, LW(1))
                IF_INT_EQUAL(LW(0), LW(1))
                    CALL(btlevtcmd_GetPartsWork, -2, LW(14), 6, LW(0))
                    IF_INT_NOT_EQUAL(LW(0), 0)
                        CALL(btlevtcmd_SetPartsWork, -2, LW(14), 6, 2)
                    END_IF
                END_IF
                GOTO(29)
            END_IF
        END_IF
        CASE_EQUAL(1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 256, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
        CASE_DEFAULT
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(0))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(1))
        CALL(btlevtcmd_CommandPreCheckDamage, -2, LW(0), LW(1), 1048832, LW(6))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 5, LW(6))
    END_SWITCH
    IF_INT_LESS(LW(12), LW(13))
        ADD(LW(12), 1)
        GOTO(28)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
    SET_VALUE(LW(12), 2)
    LABEL(30)
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(3), LW(4), 16, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(6), STRING("PYS_A3_1"))
        SET_VALUE(LW(7), STRING("PYS_A3_2"))
        SET_VALUE(LW(8), STRING("PYS_A3_3"))
        CASE_EQUAL(2)
        SET_VALUE(LW(6), STRING("PYS_A4_1"))
        SET_VALUE(LW(7), STRING("PYS_A4_2"))
        SET_VALUE(LW(8), STRING("PYS_A4_3"))
        CASE_DEFAULT
        SET_VALUE(LW(6), STRING("PYS_A2_1"))
        SET_VALUE(LW(7), STRING("PYS_A2_2"))
        SET_VALUE(LW(8), STRING("PYS_A2_3"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(6))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(8))
    WAIT_FRAMES(7)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        CALL_CHILD(_egg_attack_event)
    END_SIBLING_EVENT
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 7, LW(15))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    WAIT_FRAMES(10)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 69)
    LABEL(90)
    IF_INT_NOT_EQUAL(LW(12), LW(13))
        ADD(LW(12), 1)
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 3, LW(3))
        CALL(btlevtcmd_GetPartsWork, -2, LW(12), 4, LW(4))
        GOTO(30)
    END_IF
    CALL(btlevtcmd_StopAC)
    CALL(btlevtcmd_ACRStart, -2, 0, 15, 15, 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_CommandGetWeaponAddress, -2, LW(0))
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(0), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
    END_SWITCH
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    LABEL(95)
    SET_VALUE(LW(12), 2)
    LABEL(96)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 7, LW(15))
    IF_INT_NOT_EQUAL(LW(15), 0)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        CALL(btlevtcmd_SetPartsWork, -2, LW(12), 7, 0)
    END_IF
    ADD(LW(12), 1)
    IF_INT_LESS_OR_EQUAL(LW(12), LW(13))
        GOTO(96)
    END_IF
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 42)
    CALL(btlevtcmd_SetMoveSpeed, -2, FLOAT(6.0f))
    CALL(btlevtcmd_GetHomePos, -2, LW(0), LW(1), LW(2))
    CALL(btlevtcmd_MovePosition, -2, LW(0), LW(1), LW(2), 0, -1, 0)
    LABEL(99)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    WAIT_MS(2000)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(_egg_attack_event)
    CALL(btlevtcmd_GetStatusMg, -2, LW(6))
    MULTIPLY_FLOAT(LW(6), 10)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(12), FLOAT(5.0f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.1f))
    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(5.0f), LW(5))
        CALL(btlevtcmd_GetStatusMg, -2, LW(6))
        MULTIPLY_FLOAT(LW(6), 6)
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, LW(6), 0)
        LOOP(LW(5))
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG3"), 0)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG2"), 0)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(0.6f), 0, 0, 0, 0, 0, 0, 0)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 5, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(89)
    END_IF
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 2, LW(0))
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 6, LW(1))
    IF_INT_EQUAL(LW(1), 0)
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(0), 1073872896, LW(5))
    ELSE
        CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(0), 1073873152, LW(5))
    END_IF
    LABEL(89)
    RETURN
EVT_END()

EVT_BEGIN(partyYoshiAttack_CallGuard)
    SET_VALUE(LW(12), PTR(partyWeapon_YoshiCallGuard))
    CALL(btlevtcmd_GetEnemyBelong, -2, LW(0))
    CALL(btlevtcmd_SamplingEnemy, -2, LW(0), LW(12))
    CALL(btlevtcmd_GetSelectEnemy, LW(3), LW(4))
    IF_INT_EQUAL(LW(3), -1)
        CALL(btlevtcmd_StartWaitEvent, -2)
        RETURN
    END_IF
    CALL(btlevtcmd_WeaponAftereffect, LW(12))
    CALL(btlevtcmd_AttackDeclareAll, -2)
    CALL(btlevtcmd_WaitGuardMove)
    CALL(btlevtcmd_PayWeaponCost, -2, LW(12))
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_CommandGetWeaponActionLv, LW(0))
    CALL(btlevtcmd_AcSetDifficulty, -2, LW(0))
    CALL(btlevtcmd_AcGetDifficulty, LW(0))
    SUBTRACT(LW(0), 3)
    SWITCH(LW(0))
        CASE_LESS_OR_EQUAL(-3)
        SET_VALUE(LW(0), 280)
        SET_VALUE(LW(1), 18)
        SET_VALUE(LW(2), 25)
        CASE_EQUAL(-2)
        SET_VALUE(LW(0), 250)
        SET_VALUE(LW(1), 17)
        SET_VALUE(LW(2), 27)
        CASE_EQUAL(-1)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 16)
        SET_VALUE(LW(2), 29)
        CASE_EQUAL(0)
        SET_VALUE(LW(0), 240)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_EQUAL(1)
        SET_VALUE(LW(0), 200)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_EQUAL(2)
        SET_VALUE(LW(0), 180)
        SET_VALUE(LW(1), 15)
        SET_VALUE(LW(2), 30)
        CASE_DEFAULT
        SET_VALUE(LW(0), 150)
        SET_VALUE(LW(1), 14)
        SET_VALUE(LW(2), 30)
    END_SWITCH
    CALL(btlevtcmd_AcSetParamAll, 15, LW(0), 178, LW(1), LW(2), 20, 61, EVTDAT_POINTER_MAX)
    CALL(btlevtcmd_AcSetGaugeParam, 60, 80, 100, 100)
    CALL(btlevtcmd_AcSetFlag, 64)
    CALL(btlevtcmd_SetupAC, -2, 6, 1, 0)
    CALL(evt_btl_camera_set_mode, 0, 7)
    CALL(evt_btl_camera_set_homing_unit, 0, -2, 1)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 4)
    CALL(evt_btl_camera_set_zoom, 0, 400)
    WAIT_FRAMES(20)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_10"))
    CALL(btlevtcmd_ftof, 24, LW(0))
    WAIT_FRAMES(LW(0))
    CALL(btlevtcmd_StartAC, 1)
    CALL(btlevtcmd_ResultAC)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_A_11"))
    CALL(btlevtcmd_GetHeight, -2, LW(6))
    CALL(btlevtcmd_GetStatusMg, -2, LW(0))
    MULTIPLY_FLOAT(LW(6), LW(0))
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), LW(6))
    CALL(btlevtcmd_GetFaceDirection, -2, LW(5))
    MULTIPLY(LW(5), 30)
    CALL(evt_eff, 0, STRING("kiss"), 3, LW(0), LW(1), LW(2), LW(5), 0, 0, 0, 0, 0, 0, 0)
    CALL(evt_snd_sfxon_3d, STRING("SFX_ENM_TOGENOKO_CALL1"), LW(0), LW(1), LW(2), 0)
    CALL(btlevtcmd_ftof, 33, LW(0))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_FRAMES(LW(0))
    END_SIBLING_EVENT
    CALL(btlevtcmd_ACRStart, -2, 0, LW(0), LW(0), 20)
    CALL(btlevtcmd_ACRGetResult, LW(6), LW(7))
    SWITCH(LW(6))
        CASE_GREATER_OR_EQUAL(2)
        CALL(btlevtcmd_AudienceDeclareAcrobatResult, LW(12), 1, 0, 0, 0)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
        CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        CASE_DEFAULT
        CALL(evt_audience_acrobat_notry)
        CALL(btlevtcmd_WaitEventEnd, LW(15))
        BEGIN_SIBLING_EVENT
            WAIT_MS(166)
            CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
        END_SIBLING_EVENT
    END_SWITCH
    BEGIN_INLINE_EVENT_GET_ID(LW(9))
        WAIT_MS(1000)
        CALL(_gundan_yoshi_run_effect, -2)
    END_INLINE_EVENT
    WAIT_MS(166)
    CALL(evt_btl_camera_set_mode, 0, 0)
    CALL(evt_btl_camera_set_moveSpeedLv, 0, 1)
    CALL(evt_btl_camera_shake_h, 0, 1, 0, 6000, 0)
    WAIT_MS(1000)
    CALL(_wait_yoshig_run, -2)
    SET_VALUE(LW(15), 0)
    LABEL(70)
    IF_INT_EQUAL(LW(3), -1)
        IF_INT_NOT_EQUAL(LW(15), 0)
            CALL(btlevtcmd_WaitEventEnd, LW(15))
        END_IF
        CALL(btlevtcmd_WaitEventEnd, LW(9))
        GOTO(90)
    END_IF
    CALL(btlevtcmd_PreCheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(6))
    IF_INT_NOT_EQUAL(LW(6), 1)
        IF_INT_EQUAL(LW(6), 3)
            CALL(btlevtcmd_StartAvoid, LW(3), 38)
        END_IF
        IF_INT_EQUAL(LW(6), 6)
            CALL(btlevtcmd_StartAvoid, LW(3), 39)
        END_IF
        IF_INT_EQUAL(LW(6), 2)
            CALL(btlevtcmd_StartAvoid, LW(3), 40)
        END_IF
        GOTO(75)
    END_IF
    CALL(btlevtcmd_AcGetOutputParam, 2, LW(14))
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        LOOP(LW(14))
            IF_INT_GREATER_OR_EQUAL(LW(14), 2)
                CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
                IF_INT_GREATER_OR_EQUAL(LW(0), 2)
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131072, LW(5))
                ELSE
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 0, LW(5))
                END_IF
            ELSE
                CALL(btlevtcmd_AcGetOutputParam, 2, LW(0))
                SWITCH(LW(0))
                    CASE_GREATER_OR_EQUAL(6)
                    SET_VALUE(LW(0), 2)
                    CASE_GREATER_OR_EQUAL(4)
                    SET_VALUE(LW(0), 1)
                    CASE_GREATER_OR_EQUAL(2)
                    SET_VALUE(LW(0), 0)
                    CASE_DEFAULT
                    SET_VALUE(LW(0), -1)
                END_SWITCH
                IF_INT_GREATER_OR_EQUAL(LW(0), 0)
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 131328, LW(5))
                    CALL(btlevtcmd_GetResultPrizeLv, LW(3), LW(0), LW(6))
                    CALL(btlevtcmd_GetHitPos, LW(3), LW(4), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_ACSuccessEffect, LW(6), LW(0), LW(1), LW(2))
                    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), LW(6))
                ELSE
                    CALL(btlevtcmd_CheckDamage, -2, LW(3), LW(4), LW(12), 256, LW(5))
                    CALL(btlevtcmd_AudienceDeclareACResult, LW(12), -1)
                END_IF
            END_IF
            WAIT_FRAMES(10)
        END_LOOP
    END_SIBLING_EVENT
    LABEL(75)
    CALL(btlevtcmd_GetSelectNextEnemy, LW(3), LW(4))
    GOTO(70)
    LABEL(90)
    CALL(_wait_yoshig_complete, -2)
    CALL(evt_btl_camera_noshake, 1)
    WAIT_MS(500)
    CALL(evt_audience_ap_recovery)
    CALL(btlevtcmd_InviteApInfoReport)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    CALL_CHILD(btldefaultevt_SuitoruBadgeEffect)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

EVT_BEGIN(yoshi_appeal_event)
    CALL(btlevtcmd_RunDataEventChild, -2, 7)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 66)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_APPEAL1"), LW(0), LW(1), LW(2), 0)
    WAIT_FRAMES(40)
    CALL(btlevtcmd_AudienceAppeal, -2)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    WAIT_FRAMES(30)
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(party_win_reaction)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, STRING("PYS_Y_1"))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    RETURN
EVT_END()

static EVT_BEGIN(attack_audience_event)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    CALL(btlevtcmd_CalculateFaceDirection, -2, -1, LW(0), LW(2), 1, LW(15))
    CALL(btlevtcmd_ChangeFaceDirection, -2, LW(15))
    SET_VALUE(LW(12), 2)
    CALL(btlevtcmd_GetPos, -2, LW(0), LW(1), LW(2))
    ADD(LW(1), 10)
    CALL(btlevtcmd_SetPartsPos, -2, LW(12), LW(0), LW(1), LW(2))
    BEGIN_SIBLING_EVENT
        CALL(evt_btl_camera_set_mode, 0, 13)
        CALL(evt_btl_camera_set_homing_unit_audience, 0, -2, LW(12), LW(3))
        CALL(evt_btl_camera_set_moveSpeedLv, 0, 2)
        CALL(evt_btl_camera_set_zoom, 0, 200)
    END_SIBLING_EVENT
    CALL(evt_sub_random, 2, LW(0))
    CALL(btlevtcmd_SetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_y))
        CASE_EQUAL(2)
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_p))
        CASE_DEFAULT
        CALL(btlevtcmd_AnimeSetPoseTable, -2, LW(12), PTR(pose_table_egg_g))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePoseType, -2, LW(12), 69)
    CALL(btlevtcmd_SetPartsMoveSpeed, -2, LW(12), FLOAT(7.5f))
    CALL(btlevtcmd_SetPartsFallAccel, -2, LW(12), FLOAT(0.1f))
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(btlevtcmd_GetPartsWork, -2, LW(12), 0, LW(0))
    SWITCH(LW(0))
        CASE_EQUAL(1)
        SET_VALUE(LW(6), STRING("PYS_A3_1"))
        SET_VALUE(LW(7), STRING("PYS_A3_2"))
        SET_VALUE(LW(8), STRING("PYS_A3_3"))
        CASE_EQUAL(2)
        SET_VALUE(LW(6), STRING("PYS_A4_1"))
        SET_VALUE(LW(7), STRING("PYS_A4_2"))
        SET_VALUE(LW(8), STRING("PYS_A4_3"))
        CASE_DEFAULT
        SET_VALUE(LW(6), STRING("PYS_A2_1"))
        SET_VALUE(LW(7), STRING("PYS_A2_2"))
        SET_VALUE(LW(8), STRING("PYS_A2_3"))
    END_SWITCH
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(6))
    CALL(btlevtcmd_AnimeWaitPlayComplete, -2, 1)
    CALL(btlevtcmd_AnimeChangePose, -2, 1, LW(8))
    BEGIN_SIBLING_EVENT
        WAIT_MS(1000)
        CALL(btlevtcmd_AnimeChangePoseType, -2, 1, 43)
    END_SIBLING_EVENT
    WAIT_FRAMES(7)
    CALL(btlevtcmd_OffPartsAttribute, -2, LW(12), 50331648)
    CALL(_get_attack_aud_no, LW(3), LW(0), LW(1), LW(2))
    ADD(LW(1), 28)
    BEGIN_SIBLING_EVENT
        CALL(btlevtcmd_GetMoveFrame, -2, LW(0), LW(1), LW(2), FLOAT(5.0f), LW(5))
        CALL(btlevtcmd_SetPartsRotateOffset, -2, LW(12), 0, 6, 0)
        LOOP(LW(5))
            CALL(btlevtcmd_AddPartsRotate, -2, LW(12), 0, 0, 48)
            WAIT_FRAMES(1)
        END_LOOP
    END_SIBLING_EVENT
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG3"), 0)
    CALL(btlevtcmd_DivePartsPosition, -2, LW(12), LW(0), LW(1), LW(2), 0, 25, 0, 0, -1)
    CALL(btlevtcmd_OnPartsAttribute, -2, LW(12), 50331648)
    CALL(evt_snd_sfxon, STRING("SFX_BTL_YOSHI_EGG2"), 0)
    CALL(evt_eff, STRING(""), STRING("bomb"), 0, LW(0), LW(1), LW(2), FLOAT(0.6f), 0, 0, 0, 0, 0, 0, 0)
    CALL(_attack_aud, LW(3), 0)
    CALL(evt_btl_camera_set_mode, 0, 3)
    CALL(btlevtcmd_ResetFaceDirection, -2)
    BEGIN_SIBLING_EVENT_GET_ID(LW(15))
        WAIT_MS(1500)
        CALL(evt_btl_camera_set_mode, 0, 0)
        WAIT_MS(500)
    END_SIBLING_EVENT
    CALL(btlevtcmd_WaitEventEnd, LW(15))
    CALL(btlevtcmd_StartWaitEvent, -2)
    RETURN
EVT_END()

static EVT_BEGIN(attack_event)
    RETURN
EVT_END()

static EVT_BEGIN(wait_event)
    CALL(btlevtcmd_AnimeChangePoseFromTable, -2, 1)
    RETURN
EVT_END()

// TODO: type
u8 _yoshig_tpl_no_tbl[8] = { 0x46, 0x49, 0x47, 0x46, 0x4A, 0x48, 0x00, 0x00 };
// clang-format on
#pragma force_active off

#ifdef MUST_MATCH
// An ordinary, immediately generated function pools the i2f double before the deferred functions' strings
// ("SFX_BTL_YOSHI_TAIGUN1"); nothing calls it and the linker strips the body.
#pragma defer_codegen off
static f32 yoshi_pool_i2f(s32 v) {
    return (f32)v;
}
#pragma defer_codegen reset
#endif

// Reusing one variable for both evtGetValue and BattleTransID (`id = evtGetValue(...);
// id = BattleTransID(event, id);`) removed the extra `mr r0` the two-variable form emitted.
// With propagation on MWCC puts the string-pool base in r6 and event->args in r5 (original: r5/r6).
#pragma opt_propagation off
USER_FUNC(yoshi_original_color_anim_set) {
    s32 id;
    const char* name;
    BattleWork* work;
    s32* args;
    BattleUnitPart* part;

    args = event->args;
    work = _battleWorkPointer;
    id = evtGetValue(event, args[0]);
    id = BattleTransID(event, id);
    BattleGetUnitPtr(work, id);
    switch (pouchGetPartyColor(4)) {
    case 1:
        name = "c_babyyoshi2";
        break;
    case 2:
        name = "c_babyyoshi3";
        break;
    case 3:
        name = "c_babyyoshi4";
        break;
    case 4:
        name = "c_babyyoshi5";
        break;
    case 5:
        name = "c_babyyoshi6";
        break;
    case 6:
        name = "c_babyyoshi7";
        break;
    case 0:
    default:
        name = NULL;
        break;
    }
    if (isFirstCall) {
        if (name == NULL) {
            return EVT_RETURN_DONE;
        }
        part = BattleGetUnitPartsPtr(id, 1);
        animPoseGetAnimPosePtr(part->poseId);
        if (part->poseId != -1) {
            animPoseRelease(part->poseId);
            part->poseId = -1;
        }
    }
    if (animEffectAsync(name, 1)) {
        part = BattleGetUnitPartsPtr(id, 1);
        part->poseId = animPoseEntry(name, 1);
        return EVT_RETURN_DONE;
    }
    return 0;
}


#pragma opt_propagation reset
// .sdata2 constants used by the yoroyoro jump functions.

// Matched: the `if (tx - px > 0.0f)` comparison must use the literal `0.0f` (pooled as its own
// symbol) rather than the `0.0f` extern; that flips the FP load/compare registers to
// the target's. The remaining `4503601774854144.0` relocation names are objdiff-normalised.

static void __makeTechMenuFunc(BattleWorkCommand* cmd, s32* count) {
    s32 techLv = partyGetTechLv(
        BattleTransPartyId(BattleGetPartyPtr(_battleWorkPointer)->currentKind));

    TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_YoshiNormalAttack);
    if (techLv >= 0) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_YoshiNomikomi_Shot);
    }
    if (techLv >= 1) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_YoshiEggAttack_Minimini);
    }
    if (techLv >= 2) {
        TECH_MENU_ENTRY((BattleWeapon*)partyWeapon_YoshiCallGuard);
    }
}


// One of the 256 particles drawn by _gundan_yoshi_run_effect: a 0x30-byte entry whose
// display callback argument is the entry's +4 sub-structure. No header declares it.
typedef struct YoshiGundanDispWork {
    f32 posX; // 0x00
    f32 posY; // 0x04
    f32 posZ; // 0x08
    f32 velX; // 0x0C
    f32 field_0x10; // 0x10
    f32 velZ; // 0x14
    u8  field_0x18[0x0c]; // 0x18
    u8  animTimer; // 0x24
    u8  tplIndex; // 0x25, index into _yoshig_tpl_no_tbl (0..5)
    u8  field_0x26[2]; // 0x26
    s32 startDelay; // 0x28
    s32 state; // 0x2C
} YoshiGundanDispWork;


// .sdata/.sdata2 constants this unit references by name.
extern u8 _yoshig_tpl_no_tbl[8];


// The 0x300C-byte allocation `_gundan_yoshi_run_effect` keeps at unit+0x310: a state word,
// 256 0x30-byte particles, and a trailing accumulator. No header declares it.
typedef struct YoshiGundanArea {
    s32 state;                          // 0x0
    YoshiGundanDispWork field_0x4[256]; // 0x4
    u8 pad_0x3004[0x3008 - 0x3004];     // 0x3004
    f32 avgPosX;                        // 0x3008
} YoshiGundanArea;

USER_FUNC(btl_yoshi_yoroyoro_jump_calc_param) {
    BattleWork* work;
    s32* args;
    f32 tx;
    f32 ty;
    f32 tz;
    f32 px;
    f32 py;
    f32 pz;
    f32 speed;
    s32 id;
    BattleUnit* unit;
    s32 val;
    s32 speedVar;
    s32 frameVar;

    work = _battleWorkPointer;
    args = event->args;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    tx = (f32)evtGetValue(event, args[1]);
    ty = (f32)evtGetValue(event, args[2]);
    tz = (f32)evtGetValue(event, args[3]);
    speedVar = args[4];
    speed = evtGetFloat(event, speedVar);
    unit = BattleGetUnitPtr(work, id);
    frameVar = args[5];
    if (speed > 0.0f) {
        unit->mMoveSpeedXZ = speed;
    }
    if (unit->mMoveSpeedXZ <= 0.0f) {
        unit->mMoveSpeedXZ = 1.0f;
    }
    BtlUnit_SetMoveTargetPos(unit, tx, ty, tz);
    BtlUnit_GetPos(unit, &px, &py, &pz);
    BtlUnit_SetMoveStartPos(unit, px, py, pz);
    BtlUnit_SetMoveCurrentPos(unit, px, py, pz);
    unit->moveAngle = angleABf(px, pz, tx, tz);
    unit->moveDist = distABf(px, pz, tx, tz);
    if (tx - px > 0.0f) {
        unit->moveDirection = 1;
    } else if (tx - px < 0.0f) {
        unit->moveDirection = -1;
    }
    unit->moveDist = __fabs(tx - px);
    unit->moveFrames = 180;
    unit->moveProgress = (f32)unit->moveFrames;
    unit->mMoveSpeedY = 0.0f;
    unit->mMoveSpeedXZ = unit->moveDist / (f32)unit->moveFrames;
    evtSetFloat(event, speedVar, unit->mMoveSpeedXZ);
    evtSetValue(event, frameVar, unit->moveFrames);
    return 2;
}


// Matched. Two source shapes are load-bearing: the post-increment branch must be an `else { effect = ...;
// if (...) }` nest so the scheduler hoists the event->userdata[2] load between the `cmpwi 0x10` and
// `bge`, and the y computation needs the `t` temp plus `t = t` self-assignment (the evt_npc_set_ry_lr
// trick) so the contracted fmadds keeps the original's f1,f2 operand order. `height` must stay an f32
// local for the first block to match.
USER_FUNC(btl_yoshi_yoroyoro_jump_move) {
    s32 id;
    BattleWork* work;
    BattleUnit* unit;
    s32* args;
    EffectEntry* effect;
    f32 newSpeedY;
    f32 height;
    s32 val;

    work = _battleWorkPointer;
    args = event->args;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    evtGetFloat(event, args[4]);
    unit = BattleGetUnitPtr(work, id);
    evtGetValue(event, args[5]);
    if (isFirstCall) {
        event->blocked = 1;
        event->userdata[1] = 20;
        event->userdata[2] = 0;
    }
    if (unit->mMoveCurrentPos.x != unit->mMoveTargetPos.x) {
        if (__fabs(unit->mMoveTargetPos.x - unit->mMoveCurrentPos.x) < unit->mMoveSpeedXZ) {
            unit->mMoveCurrentPos.x = unit->mMoveTargetPos.x;
        } else {
            unit->mMoveCurrentPos.x += unit->mMoveSpeedXZ * (f32)unit->moveDirection;
        }
    }
    if (unit->mMoveCurrentPos.z != unit->mMoveTargetPos.z) {
        if (__fabs(unit->mMoveTargetPos.z - unit->mMoveCurrentPos.z) < unit->mMoveSpeedXZ) {
            unit->mMoveCurrentPos.z = unit->mMoveTargetPos.z;
        } else if (unit->mMoveTargetPos.z - unit->mMoveStartPos.z > 0.0f) {
            unit->mMoveCurrentPos.z += unit->mMoveSpeedXZ;
        } else {
            unit->mMoveCurrentPos.z -= unit->mMoveSpeedXZ;
        }
    }
    newSpeedY = 10.f *
                sinfd(180.f +
                      360.f *
                          (unit->moveProgress -
                           (f32)(unit->moveFrames % 90)) /
                          90.f);
    BtlUnit_GetPartsPtr(unit, 1);
    unit->mMoveCurrentPos.y =
        unit->mMoveSpeedY +
        (unit->mMoveStartPos.y +
         (unit->mMoveTargetPos.y - unit->mMoveStartPos.y) *
             (unit->moveProgress - (f32)unit->moveFrames) /
             unit->moveProgress);
    unit->mMoveSpeedY = newSpeedY;
    unit->work[5] =
        (s32)(75.0f -
              25.0f *
                  sinfd(360.f *
                            (unit->moveProgress -
                             (f32)(unit->moveFrames % 90)) /
                        90.f));
    BtlUnit_SetPos(unit, unit->mMoveCurrentPos.x, unit->mMoveCurrentPos.y, unit->mMoveCurrentPos.z);
    event->userdata[1]++;
    if (event->userdata[1] > 20) {
        event->userdata[1] = 0;
        if (unit->moveDirection >= 0) {
            newSpeedY = -50.f;
        } else {
            newSpeedY = 50.f;
        }
        height = BtlUnit_GetHeight(unit);
        event->userdata[2] = (s32)effSweatN64Entry(
            1, unit->mMoveCurrentPos.x,
            unit->mSizeMultiplier * (0.5f * height) + unit->mMoveCurrentPos.y,
            unit->mMoveCurrentPos.z, 5.0f * unit->mSizeMultiplier, newSpeedY, 16);
    } else {
        effect = (EffectEntry*)event->userdata[2];
        if (event->userdata[1] < 16) {
            ((effSweatData*)effect->userdata)->position.x = unit->mMoveCurrentPos.x;
            height = BtlUnit_GetHeight(unit);
#ifdef MUST_MATCH
            // The `t` temp keeps the inner `0.5f * height` product in f2, and the self-assignment
            // pins its load ahead of `mSizeMultiplier`'s so the contracted fmadds is f1,f2 as in the
            // original (same trick as evt_npc_set_ry_lr). The natural form is under #else.
            {
                f32 t = 0.5f * height;
                t = t;
                ((effSweatData*)effect->userdata)->position.y =
                    unit->mSizeMultiplier * t + unit->mMoveCurrentPos.y;
            }
#else
            ((effSweatData*)effect->userdata)->position.y =
                unit->mSizeMultiplier * (0.5f * height) + unit->mMoveCurrentPos.y;
#endif
            ((effSweatData*)effect->userdata)->position.z = unit->mMoveCurrentPos.z;
        }
    }
    unit->moveFrames -= 1;
    if (unit->moveFrames <= 0) {
        BtlUnit_SetPos(unit, unit->mMoveTargetPos.x, unit->mMoveTargetPos.y, unit->mMoveTargetPos.z);
        return 2;
    }
    return 0;
}


s32 _yoshi_slide_move_sound(BattleUnit* unit, PoseSoundEntry* entry) {
    return BtlUnit_snd_se(unit, entry->soundName, 0xF1194D80, (s16)(unit->work[4] * unit->work[5] / 100));
}


// Matched by mirroring the identical, already-matched `_get_bomb_hit_position` in
// unit_party_sanders.c: same declaration order (the order fixes MWCC's callee-saved
// allocation: event r29, work r28, id1 r26, id2 r25, outX/outY/outZ r31/r30/r26) and the
// same `Vec pos` + inline `mSizeMultiplier` shape.
USER_FUNC(_get_nomikomi_hit_position) {
    BattleUnit* unit;
    BattleWork* work = _battleWorkPointer;
    Vec pos;
    s32* args = event->args;
    s32 id1;
    s32 id2;
    s32 dir;
    s32 outZ;
    s32 outX;
    s32 outY;

    id1 = evtGetValue(event, args[0]);
    id2 = evtGetValue(event, args[1]);
    evtGetValue(event, args[2]);
    outX = args[3];
    outY = args[4];
    outZ = args[5];
    BattleGetUnitPtr(work, BattleTransID(event, id1));
    unit = BattleGetUnitPtr(work, BattleTransID(event, id2));
    dir = unit->faceDirection;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    pos.x += (f32)dir * (unit->bintaHitOffset.x * unit->mSizeMultiplier);
    pos.y += unit->bintaHitOffset.y * unit->mSizeMultiplier;
    pos.z += unit->bintaHitOffset.z * unit->mSizeMultiplier;
    evtSetFloat(event, outX, pos.x);
    evtSetFloat(event, outY, pos.y);
    evtSetFloat(event, outZ, pos.z);
    return EVT_RETURN_DONE;
}


USER_FUNC(_get_swallow_param) {
    BattleWork* work = _battleWorkPointer;
    s32* args = event->args;
    BattleUnit* unit;
    s32 val;
    s32 id;
    s32 index;

    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    index = args[1];
    unit = BattleGetUnitPtr(work, id);
    evtSetValue(event, index, unit->swallowRate);
    return EVT_RETURN_DONE;
}


USER_FUNC(_check_swallow_attribute) {
    BattleWork* work;
    BattleUnit* unit;
    s32 val;
    u16 mask;
    s32* args;
    s32 index;
    s32 id;

    args = event->args;
    work = _battleWorkPointer;
    val = evtGetValue(event, args[0]);
    id = BattleTransID(event, val);
    mask = args[1];
    index = args[2];
    unit = BattleGetUnitPtr(work, id);
    if ((u8)unit->swallowAttribute & mask) {
        evtSetValue(event, index, 1);
    } else {
        evtSetValue(event, index, 0);
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_wait_yoshig_run) {
    BattleUnit* unit;
    s32 val;
    YoshiGundanArea* area;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    area = unit->mExtraWork;
    if (area == NULL) {
        return 0;
    }
    if (area->avgPosX <= 0.0f) {
        return 0;
    }
    return EVT_RETURN_DONE;
}


USER_FUNC(_wait_yoshig_complete) {
    BattleUnit* unit;
    s32 val;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    return unit->mExtraWork ? 0 : EVT_RETURN_DONE;
}


// Matched. The zone constants must be literals (`80.0f`/`-80.0f`/`300.0f`) while `-1.0f` stays the
// `-1.0f` extern, and `active` must be reached with a `goto` label placed after the
// `default:` case so MWCC shares the r0 `li` block instead of tail-duplicating it.
USER_FUNC(_gundan_yoshi_run_effect) {
    Vec pos;
    u8* cur;
    f32 scale;
    BattleUnit* unit;
    s32 active;
    YoshiGundanArea* area;
    s32 val;
    YoshiGundanDispWork* work;
    s32 i;
    s32 done;
    u8* cur2;
    s32 j;

    val = evtGetValue(event, event->args[0]);
    unit = BattleGetUnitPtr(_battleWorkPointer, BattleTransID(event, val));
    if (isFirstCall) {
        area = BattleAlloc(0x300C);
        unit->mExtraWork = area;
        memset(area, 0, 0x300C);
        area->state = 0;
        area->avgPosX = 0.0f;
        event->userdata[0] = 0;
        event->userdata[1] = psndSFXOn("SFX_BTL_YOSHI_TAIGUN1");
    } else {
        area = (YoshiGundanArea*)unit->mExtraWork;
    }
    switch (area->state) {
    case 0:
        area->state++;
        // fallthrough
    case 1:
        done = 1;
        area->avgPosX = 0.0f;
        cur = (u8*)area;
        for (i = 0; i < 256; i++, cur += 0x30) {
            work = (YoshiGundanDispWork*)(cur + 4);
            work->animTimer++;
            if (work->animTimer > 2) {
                work->animTimer = 0;
                work->tplIndex++;
                if (work->tplIndex >= 6) {
                    work->tplIndex = 0;
                }
            }
            switch (work->state) {
            case 0:
                work->state++;
                work->startDelay = irand(3);
                work->posX = (f32)(-(30 * (i / 16) + 300) + irand(10));
                work->posY = 0.0f;
                work->posZ = (f32)((i % 16) * 10 - 80);
                work->tplIndex = irand(6);
                work->animTimer = irand(5);
                work->velX = 3.0f + 0.1f * (f32)irand(10);
                work->field_0x10 = 0.0f;
                work->velZ = 0.001f * (f32)(irand(2000) - 1000);
                // fallthrough
            case 1:
                if (work->startDelay > 0) {
                    work->startDelay--;
                    goto yoshi_active; // shares case 2's `active = 1` tail; a second store changes the object
                }
                // fallthrough
            case 2:
                work->posZ += work->velZ;
                if (work->posZ >= 80.0f) {
                    work->posZ = 80.0f;
                    work->velZ *= -1.0f;
                }
                if (work->posZ <= -80.0f) {
                    work->posZ = -80.0f;
                    work->velZ *= -1.0f;
                }
                work->posX += work->velX;
                if (work->posX >= 300.0f) {
                    work->posX = 300.0f;
                    work->velX = 0.0f;
                    work->state = 999;
                }
                goto yoshi_active;
            default:
                active = 0;
                break;
            yoshi_active:
                active = 1;
                break;
            }
            if (active) {
                done = 0;
            }
            area->avgPosX += work->posX;
        }
        if (event->userdata[0] > 0) {
            event->userdata[0]--;
        } else {
            work = &area->field_0x4[irand(256)];
            scale = 1.0f + 0.05f * (f32)irand(20);
            effKemuTestEntry(0, work->posX,
                             5.0f + work->posY + (f32)irand(15),
                             65.0f, scale);
        }
        area->avgPosX *= 0.00390625f;
        pos.x = area->avgPosX;
        pos.y = 0.0f;
        pos.z = 0.0f;
        psndSFX_pos(event->userdata[1], &pos);
        if (done) {
            area->state = 999;
        }
        break;
    case 999:
        psndSFXOff(event->userdata[1]);
        BattleFree((YoshiGundanArea*)unit->mExtraWork);
        unit->mExtraWork = NULL;
        return 2;
    default:
        break;
    }
    cur2 = (u8*)area;
    for (j = 0; j < 256; j++, cur2 += 0x30) {
        dispEntry(4, dispCalcZ((Vec*)work), 2, _btlYoshiDisp,
                  (YoshiGundanDispWork*)(cur2 + 4));
    }
    return 0;
}


void _btlYoshiDisp(CameraId cameraId, void* param) {
    YoshiGundanDispWork* work = param;
    GXColor color;
    f32 x_scale;
    static const GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };

    x_scale = (work->velX < 0.0f) ? 0.4f : -0.4f;
    color = white;
    btlDispTexPlane(work->posX, 20.f + work->posY, work->posZ, x_scale, 0.4f, _yoshig_tpl_no_tbl[work->tplIndex],
                    color, 0);
}

// TODO: type
