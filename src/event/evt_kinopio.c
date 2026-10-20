#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/npcdrv.h"
#include "driver/animdrv.h"
#include "driver/itemdrv.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "pmario_sound.h"
#include <string.h>
#include "evt/evt_kinopio.h"
#include "evt/evt_cam.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "party/party_motion.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "evt/evt_window.h"
#include "math_ext.h"
static USER_FUNC(paper_off);  // local to this file in the original
static USER_FUNC(paper_on);  // local to this file in the original


// itemEntry is defined in driver/itemdrv.c but not declared in any header yet.

// shared .sdata2 constants (named by their original addresses)
__declspec(section ".sdata2") char str_mario[8] = "mario";
__declspec(section ".sdata2") f32 float_6p2832_8041bc70 = 6.2831855f;
__declspec(section ".sdata2") f32 float_90_8041bc74 = 90.0f;
__declspec(section ".sdata2") f32 float_360_8041bc78 = 360.0f;
__declspec(section ".sdata2") char str_M_S_1[8] = "M_S_1";

// force_active: these strings are referenced only through the unit's other data (and by other units), the linker would drop the rest.
#pragma force_active on
// shared .rodata string pool (named by their original addresses)
const char str_BGM_FF_GO_SLEEPING1[0x14] = "BGM_FF_GO_SLEEPING1";
const char str_802ECE64[0x14] = "\217h\211\256\202\314\203J\203\201\203\211\203}\203\223";
const char str_c_jugem_c[0x0C] = "c_jugem_c";
const char str_802ECE84[0x0C] = "\203J\203\201\203\211\203}\203\223";
const char str_bom_02[0x08] = "bom_02";
const char str_gor_01[0x08] = "gor_01";
const char str_muj_01[0x08] = "muj_01";
const char str_nok_00[0x08] = "nok_00";
const char str_pik_04[0x08] = "pik_04";
const char str_usu_00[0x08] = "usu_00";
const char str_confetti[0x0C] = "confetti";
const char str_SFX_VOICE_MARIO_FIND1[0x18] = "SFX_VOICE_MARIO_FIND1";
const char str_SFX_EVT_50_HOTEL_SHUTTER1[0x1C] = "SFX_EVT_50_HOTEL_SHUTTER1";
const char str_breakfast[0x0C] = "breakfast";
const char str_p_slit[0x08] = "p_slit";
const char str_PM_S_3A[0x08] = "PM_S_3A";
#pragma force_active reset

// event function prototypes referenced by the script tables

// script tables (defined before the functions so the string pool order matches)
extern s32 evt_50[];
s32 kinopio_house_talk_evt[] = {
	0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0002005B,
	(s32)get_ryoukin, 0xFE363C8A, 0x0002005B, (s32)get_npcname,
	0xFE363C8B, 0x0003005B, (s32)evt_win_coin_on, 0x00000000,
	0xFE363C8C, 0x0003005B, (s32)msg_no, 0x00000000,
	0xFE363C80, 0x0006005B, (s32)evt_msg_print_insert, 0x00000000,
	0xFE363C80, 0x00000000, 0xFE363C8B, 0xFE363C8A,
	0x0003005B, (s32)msg_no, 0x00000006, 0xFE363C80,
	0x0003005B, (s32)evt_msg_select, 0x00000000, 0xFE363C80,
	0x00020018, 0xFE363C80, 0x00000001, 0x0003005B,
	(s32)msg_no, 0x00000002, 0xFE363C80, 0x0003005B,
	(s32)evt_msg_print_add, 0x00000000, 0xFE363C80, 0x0002005B,
	(s32)evt_mario_key_onoff, 0x00000001, 0x0002005B, (s32)evt_win_coin_off,
	0xFE363C8C, 0x00000002, 0x00000021, 0x0002005B,
	(s32)power_chk, 0xFE363C80, 0x00020018, 0xFE363C80,
	0x00000001, 0x0003005B, (s32)msg_no, 0x00000001,
	0xFE363C80, 0x0003005B, (s32)evt_msg_print_add, 0x00000000,
	0xFE363C80, 0x0003005B, (s32)msg_no, 0x00000006,
	0xFE363C80, 0x0003005B, (s32)evt_msg_select, 0x00000000,
	0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
	0x0003005B, (s32)msg_no, 0x00000002, 0xFE363C80,
	0x0003005B, (s32)evt_msg_print_add, 0x00000000, 0xFE363C80,
	0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x0002005B,
	(s32)evt_win_coin_off, 0xFE363C8C, 0x00000002, 0x00000021,
	0x00000021, 0x00020032, 0xFBD3E280, 0x00000000,
	0x0003005B, (s32)evt_pouch_check_item, 0x000000A4, 0xFE363C80,
	0x0002001B, 0xFE363C80, 0x00000000, 0x0003005B,
	(s32)msg_no, 0x00000007, 0xFE363C80, 0x0003005B,
	(s32)evt_msg_print_add, 0x00000000, 0xFE363C80, 0x0003005B,
	(s32)msg_no, 0x00000008, 0xFE363C80, 0x0003005B,
	(s32)evt_msg_select, 0x00000000, 0xFE363C80, 0x00020018,
	0xFE363C80, 0x00000000, 0x00020032, 0xFBD3E280,
	0x00000001, 0x0003005B, (s32)evt_pouch_remove_item, 0x000000A4,
	0xFE363C80, 0x00000021, 0x00000021, 0x00020018,
	0xFBD3E280, 0x00000000, 0x0002005B, (s32)coin_chk,
	0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
	0x0003005B, (s32)msg_no, 0x00000004, 0xFE363C80,
	0x0003005B, (s32)evt_msg_print_add, 0x00000000, 0xFE363C80,
	0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x0002005B,
	(s32)evt_win_coin_off, 0xFE363C8C, 0x00000002, 0x00000021,
	0x0002005B, (s32)get_ryoukin, 0xFE363C8A, 0x00020037,
	0xFE363C8A, 0xFFFFFFFF, 0x0002005B, (s32)evt_pouch_add_coin,
	0xFE363C8A, 0x0001000A, 0x000003E8, 0x00000021,
	0x0002005B, (s32)evt_win_coin_off, 0xFE363C8C, 0x0003005B,
	(s32)msg_no, 0x00000003, 0xFE363C80, 0x0003005B,
	(s32)evt_msg_print_add, 0x00000000, 0xFE363C80, 0x0002005B,
	(s32)get_npcname, 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
	0x00000001, 0xFE363C80, 0x00000020, 0x0002005B,
	(s32)evt_mario_get_party, 0xFE363C8B, 0x0002005B, (s32)evt_mario_get_exparty,
	0xFE363C8C, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
	0xFE363C8D, 0xFE363C8E, 0xFE363C8F, 0x0002005B,
	(s32)evt_mario_goodbye_party, 0x00000001, 0x0002005B, (s32)evt_mario_goodbye_party,
	0x00000000, 0x0002005B, (s32)get_to_bed_evt, 0xFE363C80,
	0x00020018, 0xFE363C80, 0x00000000, 0x0000006B,
	0x0001000A, 0x000003E8, 0x0007005B, (s32)get_cam,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
	0xFE363C84, 0xFE363C85, 0x0009005B, (s32)evt_cam3d_evt_set,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
	0xFE363C84, 0xFE363C85, 0x00000000, 0x0000000B,
	0x0000006D, 0x0004005B, (s32)get_bedside, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x0004005B, (s32)evt_mario_mov_pos2,
	0xFE363C80, 0xFE363C82, 0xF24BE280, 0x00000020,
	0x0001006C, 0xFE363C80, 0x0001005E, 0xFE363C80,
	0x0000006D, 0x00010005, 0x00000000, 0x0002006A,
	0xFE363C80, 0xFE363C81, 0x00020018, 0xFE363C81,
	0x00000000, 0x00000007, 0x00000021, 0x00010009,
	0x00000001, 0x00000006, 0x0007005B, (s32)get_cam,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
	0xFE363C84, 0xFE363C85, 0x0009005B, (s32)evt_cam3d_evt_set,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0xFE363C83,
	0xFE363C84, 0xFE363C85, 0x00000000, 0x0000000B,
	0x0001000A, 0x000001F4, 0x00000021, 0x0005005B,
	(s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d, 0x0002009E,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
	0x0003005B, (s32)evt_mario_flag_onoff, 0x00000001, 0x00000700,
	0x0004005B, (s32)get_bed, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0007005B, (s32)evt_mario_jump_pos, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0xF250BA80, 0x00000000,
	0xFFFFFC18, 0x0004005B, (s32)evt_mario_set_pos, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x0001005B, (s32)evt_mario_adjust_dir,
	0x0001000A, 0x000000C8, 0x0003005B, (s32)evt_mario_dispflag_onoff,
	0x00000001, 0x00001000, 0x0001006C, 0xFE363C8A,
	0x0002005B, (s32)get_beddeg, 0xFE363C80, 0x0002005B,
	(s32)paper_on, 0xFE363C80, 0x0000006D, 0x0000006B,
	0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
	0x000203BA, 0xFE363C80, 0xFE363C81, 0xFE363C82,
	0x00000000, 0x0001000A, 0x000000C8, 0x0006005B,
	(s32)evt_snd_sfxon_3d, 0x000203BB, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x00000000, 0x0000006D, 0x0001005B,
	(s32)snd_off, 0x0002005B, (s32)evt_snd_bgmoff, 0x00000400,
	0x0003005B, (s32)evt_snd_bgmon, 0x00000001, (s32)(char*)str_BGM_FF_GO_SLEEPING1,
	0x0006005B, (s32)evt_fade_entry, 0x00000004, 0x00000BB8,
	0x00000000, 0x00000000, 0x00000000, 0x0005005B,
	(s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0004005B, (s32)evt_fade_set_spot_pos, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x0001005B, (s32)evt_fade_end_wait,
	0x0006005B, (s32)evt_fade_entry, 0x0000000C, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x0001005B,
	(s32)evt_fade_end_wait, 0x0001005B, (s32)evt_pouch_mario_recovery, 0x0001005B,
	(s32)evt_pouch_all_party_recovery, 0x0001005B, (s32)breakfast, 0x0001000A,
	0x00000BB8, 0x0001005F, 0xFE363C8A, 0x0001005B,
	(s32)paper_off, 0x0003005B, (s32)evt_mario_dispflag_onoff, 0x00000000,
	0x00001000, 0x00020019, 0xFE363C8B, 0x00000000,
	0x0004005B, (s32)get_bed, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0006005B, (s32)evt_mario_set_party_pos, 0x00000000,
	0xFE363C8B, 0xFE363C80, 0xFE363C81, 0xFE363C82,
	0x00000021, 0x00020019, 0xFE363C8C, 0x00000000,
	0x0004005B, (s32)get_bed, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0004005B, (s32)offset, 0xF24AF280,
	0xFE363C83, 0xFE363C84, 0x0002003A, 0xFE363C80,
	0xFE363C83, 0x0002003A, 0xFE363C82, 0xFE363C84,
	0x0006005B, (s32)evt_mario_set_party_pos, 0x00000001, 0xFE363C8C,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000021,
	0x0003005B, (s32)evt_mario_flag_onoff, 0x00000000, 0x00000700,
	0x0004005B, (s32)get_bedside, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0004005B, (s32)evt_mario_set_pos, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x0002005B, (s32)get_npcname,
	0xFE363C80, 0x0003005B, (s32)evt_set_dir_to_target, 0xFE363C80,
	(s32)str_mario, 0x0002005B, (s32)evt_mario_set_dir_npc, 0xFE363C80,
	0x0003005B, (s32)evt_party_set_dir_npc, 0x00000000, 0xFE363C80,
	0x0003005B, (s32)evt_party_set_dir_npc, 0x00000001, 0xFE363C80,
	0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, 0xFE363C80,
	0x00008000, 0x0003005B, (s32)evt_cam3d_evt_off, 0x00000000,
	0x0000000B, 0x00010009, 0x00000001, 0x0004005B,
	(s32)evt_npc_flag_onoff, 0x00000000, 0xFE363C80, 0x00008000,
	0x0002005B, (s32)evt_snd_bgmoff, 0x00000201, 0x0003005B,
	(s32)evt_snd_bgmon, 0x00000120, 0x00000000, 0x0001005B,
	(s32)snd_on, 0x00020035, 0xF5DE0181, 0x00000001,
	0x0002001B, 0xF5DE0181, 0x00000064, 0x00020032,
	0xF5DE0181, 0x00000064, 0x00000021, 0x00020018,
	0xF5DE0181, 0x00000032, 0x0001005E, (s32)evt_50,
	0x0002005B, (s32)get_npcname, 0xFE363C80, 0x0004005B,
	(s32)evt_npc_flag_onoff, 0x00000000, 0xFE363C80, 0x00000020,
	0x00000002, 0x00000021, 0x0001005B, (s32)evt_mario_adjust_dir,
	0x0006005B, (s32)evt_fade_entry, 0x0000000B, 0x000003E8,
	0x00000000, 0x00000000, 0x00000000, 0x0001005B,
	(s32)evt_fade_end_wait, 0x0002005B, (s32)get_from_bed_evt, 0xFE363C80,
	0x00020019, 0xFE363C80, 0x00000000, 0x0001006C,
	0xFE363C8A, 0x0001005E, 0xFE363C80, 0x0000006D,
	0x00010005, 0x00000000, 0x0002006A, 0xFE363C8A,
	0xFE363C8B, 0x00020018, 0xFE363C8B, 0x00000000,
	0x00000007, 0x00000021, 0x00010009, 0x00000001,
	0x00000006, 0x00000021, 0x0002005B, (s32)evt_party_run,
	0x00000000, 0x0002005B, (s32)evt_party_run, 0x00000001,
	0x0004005B, (s32)get_npctalk, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0004005B, (s32)evt_mario_mov_pos2, 0xFE363C80,
	0xFE363C82, 0xF24BE280, 0x0001005B, (s32)evt_fade_end_wait,
	0x0002005B, (s32)get_npcname, 0xFE363C8B, 0x0002005B,
	(s32)evt_mario_set_dir_npc, 0xFE363C8B, 0x0003005B, (s32)evt_party_set_dir_npc,
	0x00000000, 0xFE363C8B, 0x0002005B, (s32)get_npcname,
	0xFE363C8B, 0x0003005B, (s32)msg_no, 0x00000005,
	0xFE363C80, 0x0005005B, (s32)evt_msg_print, 0x00000000,
	0xFE363C80, 0x00000000, 0xFE363C8B, 0x0002005B,
	(s32)get_npcname, 0xFE363C80, 0x0004005B, (s32)evt_npc_flag_onoff,
	0x00000000, 0xFE363C80, 0x00000020, 0x0002005B,
	(s32)evt_mario_key_onoff, 0x00000001, 0x00000002, 0x00000001,
};

s32 evt_50[] = {
	0x0002005B, (s32)get_npcname, 0xFE363C8A, 0x0005005B,
	(s32)evt_npc_get_position, 0xFE363C8A, 0xFE363C8D, 0xFE363C8E,
	0xFE363C8F, 0x0003005B, (s32)evt_npc_entry, (s32)(char*)str_802ECE64,
	(s32)(char*)str_c_jugem_c, 0x0003005B, (s32)evt_npc_set_tribe, (s32)(char*)str_802ECE64,
	(s32)(char*)str_802ECE84, 0x0001005B, (s32)N_cameraman_on, 0x0004005B,
	(s32)evt_npc_flag_onoff, 0x00000001, (s32)(char*)str_802ECE64, 0x00008000,
	0x0002005B, (s32)evt_sub_get_mapname, 0xFE363C80, 0x0002000C,
	0xFE363C80, (s32)(char*)str_bom_02, 0x0005005B, (s32)evt_npc_set_position,
	(s32)(char*)str_802ECE64, 0xFFFFFE93, 0x0000001E, 0xFFFFFF56,
	0x00000021, 0x0002000C, 0xFE363C80, (s32)(char*)str_gor_01,
	0x0005005B, (s32)evt_npc_set_position, (s32)(char*)str_802ECE64, 0x000000A8,
	0x0000009A, 0xFFFFFF1B, 0x00000021, 0x0002000C,
	0xFE363C80, (s32)(char*)str_muj_01, 0x0005005B, (s32)evt_npc_set_position,
	(s32)(char*)str_802ECE64, 0x0000010E, 0x00000014, 0xFFFFFF6F,
	0x00000021, 0x0002000C, 0xFE363C80, (s32)(char*)str_nok_00,
	0x0005005B, (s32)evt_npc_set_position, (s32)(char*)str_802ECE64, 0xFFFFFE7F,
	0x00000019, 0xFFFFFF6F, 0x00000021, 0x0002000C,
	0xFE363C80, (s32)(char*)str_pik_04, 0x0005005B, (s32)evt_npc_set_position,
	(s32)(char*)str_802ECE64, 0x00000136, 0x00000531, 0xFFFFFF9C,
	0x0005005B, (s32)evt_npc_set_position, 0xFE363C8A, 0x00000136,
	0x00000531, 0xFFFFFFBF, 0x00000021, 0x0002000C,
	0xFE363C80, (s32)(char*)str_usu_00, 0x0005005B, (s32)evt_npc_set_position,
	(s32)(char*)str_802ECE64, 0x0000010C, 0x00000000, 0xFFFFFF2E,
	0x00000021, 0x0003005B, (s32)evt_set_dir_to_target, (s32)(char*)str_802ECE64,
	(s32)str_mario, 0x0003005B, (s32)evt_set_dir_to_target, 0xFE363C8A,
	(s32)str_mario, 0x0002005B, (s32)evt_mario_set_dir_npc, (s32)(char*)str_802ECE64,
	0x0003005B, (s32)evt_party_set_dir_npc, 0x00000000, (s32)(char*)str_802ECE64,
	0x0003005B, (s32)msg_no, 0x00000009, 0xFE363C80,
	0x0005005B, (s32)evt_msg_print, 0x00000000, 0xFE363C80,
	0x00000000, 0xFE363C8A, 0x0004005B, (s32)get_bed,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
	(s32)evt_mario_set_pos, 0xFE363C80, 0xFE363C81, 0xFE363C82,
	0x00020035, 0xFE363C82, 0x00000005, 0x0005005B,
	(s32)evt_party_set_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0006005B, (s32)evt_fade_entry, 0x00000009,
	0x000003E8, 0x00000000, 0x00000000, 0x00000000,
	0x0001005B, (s32)evt_fade_end_wait, 0x0001000A, 0x000001F4,
	0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
	0x00000064, 0x000F005B, (s32)evt_eff, 0x00000000,
	(s32)(char*)str_confetti, 0x00000003, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x00000258, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0000006B, 0x0005005B, (s32)evt_mario_get_pos,
	0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
	0x0006005B, (s32)evt_snd_sfxon_3d, (s32)(char*)str_SFX_VOICE_MARIO_FIND1, 0xFE363C80,
	0xFE363C81, 0xFE363C82, 0x00000000, 0x0000006D,
	0x000C005B, (s32)evt_eff_fukidashi, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x000C005B, (s32)evt_eff_fukidashi, 0x00000003,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x0001000A, 0x000003E8,
	0x0003005B, (s32)msg_no, 0x0000000A, 0xFE363C80,
	0x0005005B, (s32)evt_msg_print, 0x00000000, 0xFE363C80,
	0x00000000, 0xFE363C8A, 0x0004005B, (s32)get_bedside,
	0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0004005B,
	(s32)evt_mario_mov_pos2, 0xFE363C80, 0xFE363C82, 0xF24BE280,
	0x0004005B, (s32)get_npctalk, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0004005B, (s32)evt_mario_mov_pos2, 0xFE363C80,
	0xFE363C82, 0xF24BE280, 0x0002005B, (s32)evt_mario_set_dir_npc,
	0xFE363C8A, 0x0003005B, (s32)msg_no, 0x0000000B,
	0xFE363C80, 0x0005005B, (s32)evt_msg_print, 0x00000000,
	0xFE363C80, 0x00000000, 0xFE363C8A, 0x0002005B,
	(s32)evt_mario_set_dir_npc, (s32)(char*)str_802ECE64, 0x0003005B, (s32)evt_party_set_dir_npc,
	0x00000000, (s32)(char*)str_802ECE64, 0x0001000A, 0x0000012C,
	0x0003005B, (s32)evt_snd_sfxon, (s32)(char*)str_SFX_EVT_50_HOTEL_SHUTTER1, 0x00000000,
	0x0006005B, (s32)evt_fade_entry, 0x00000008, 0x00000096,
	0x000000FF, 0x000000FF, 0x000000FF, 0x0005005B,
	(s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x00020035, 0xFE363C81, 0x00000028,
	0x0004005B, (s32)evt_fade_set_spot_pos, 0xFE363C80, 0xFE363C81,
	0xFE363C82, 0x0001005B, (s32)evt_fade_end_wait, 0x0006005B,
	(s32)evt_fade_entry, 0x00000007, 0x00000096, 0x000000FF,
	0x000000FF, 0x000000FF, 0x0001005B, (s32)evt_fade_end_wait,
	0x0003005B, (s32)msg_no, 0x0000000C, 0xFE363C80,
	0x0005005B, (s32)evt_msg_print, 0x00000000, 0xFE363C80,
	0x00000000, 0xFE363C8A, 0x0002005B, (s32)evt_mario_set_dir_npc,
	0xFE363C8A, 0x0003005B, (s32)evt_party_set_dir_npc, 0x00000000,
	0xFE363C8A, 0x0001000A, 0x0000012C, 0x0002005B,
	(s32)evt_sub_get_coin, 0x000000C8, 0x0003005B, (s32)msg_no,
	0x0000000D, 0xFE363C80, 0x0005005B, (s32)evt_msg_print,
	0x00000000, 0xFE363C80, 0x00000000, 0xFE363C8A,
	0x0006005B, (s32)evt_fade_entry, 0x0000000A, 0x000003E8,
	0x00000000, 0x00000000, 0x00000000, 0x0001005B,
	(s32)evt_fade_end_wait, 0x0002005B, (s32)evt_npc_check_delete, (s32)(char*)str_802ECE64,
	0x0005005B, (s32)evt_npc_set_position, 0xFE363C8A, 0xFE363C8D,
	0xFE363C8E, 0xFE363C8F, 0x0003005B, (s32)evt_set_dir_to_target,
	0xFE363C8A, (s32)str_mario, 0x0006005B, (s32)evt_fade_entry,
	0x00000009, 0x000003E8, 0x00000000, 0x00000000,
	0x00000000, 0x0001005B, (s32)evt_fade_end_wait, 0x0002005B,
	(s32)evt_party_run, 0x00000000, 0x0002005B, (s32)evt_mario_key_onoff,
	0x00000001, 0x00000002, 0x00000001,
};




typedef struct KinopioData {
    s32 field_0;        // 0x00
    f32 field_4;        // 0x04
    f32 field_8;        // 0x08
    f32 field_C;        // 0x0C
    s32 ryoukin;        // 0x10
    f32 bedX;           // 0x14
    f32 bedY;           // 0x18
    f32 bedZ;           // 0x1C
    f32 bedsideX;       // 0x20
    f32 bedsideY;       // 0x24
    f32 bedsideZ;       // 0x28
    f32 camX;           // 0x2C
    f32 camY;           // 0x30
    f32 camZ;           // 0x34
    f32 camX2;          // 0x38
    f32 camY2;          // 0x3C
    f32 camZ2;          // 0x40
    f32 beddeg;         // 0x44
    f32 npctalkX;       // 0x48
    f32 npctalkY;       // 0x4C
    f32 npctalkZ;       // 0x50
    const char* npcname; // 0x54
    s32 msgs[14];       // 0x58 (0x58..0x8F)
    s32 to_bed_evt;     // 0x90
    s32 from_bed_evt;   // 0x94
} KinopioData;

static KinopioData* work;
static KinopioData** wp = &work;
USER_FUNC(evt_kinopio_setup) {
    KinopioData* data = (KinopioData*)evtGetValue(event, event->args[0]);
    NpcEntry* npc = npcNameToPtr(data->npcname);

    if (animGroupBaseAsync(str_p_slit, 0, NULL) == 0) {
        return 0;
    }
    npc->talkEvt = kinopio_house_talk_evt;
    *wp = data;
    return EVT_RETURN_DONE;
}

USER_FUNC(msg_no) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);

    evtSetValue(event, args[1], (*wp)->msgs[id]);
    return EVT_RETURN_DONE;
}

USER_FUNC(power_chk) {
    s32* args = event->args;
    PouchData* pouch = pouchGetPtr();
    BOOL full = TRUE;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (pouch->partyData[i].flags & 1) {
            if (pouch->partyData[i].currentHP != pouch->partyData[i].maximumHP) {
                full = FALSE;
                break;
            }
        }
    }
    if (pouchGetHP() == pouchGetMaxHP() && (*(s32(*)(void))pouchGetFP)() == pouchGetMaxFP() && (*(s32(*)(void))pouchGetAP)() == pouchGetMaxAP() && full) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(coin_chk) {
    s32* args = event->args;

    if (pouchGetCoin() < (*wp)->ryoukin) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(get_ryoukin) {
    evtSetValue(event, event->args[0], (*wp)->ryoukin);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_npcname) {
    evtSetValue(event, event->args[0], (s32)(*wp)->npcname);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_bed) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->bedX);
    evtSetFloat(event, args[1], (*wp)->bedY);
    evtSetFloat(event, args[2], (*wp)->bedZ);
    return EVT_RETURN_DONE;
}
USER_FUNC(get_bedside) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->bedsideX);
    evtSetFloat(event, args[1], (*wp)->bedsideY);
    evtSetFloat(event, args[2], (*wp)->bedsideZ);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_beddeg) {
    evtSetFloat(event, event->args[0], (*wp)->beddeg);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_npctalk) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->npctalkX);
    evtSetFloat(event, args[1], (*wp)->npctalkY);
    evtSetFloat(event, args[2], (*wp)->npctalkZ);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_cam) {
    s32* args = event->args;

    evtSetFloat(event, args[0], (*wp)->camX);
    evtSetFloat(event, args[1], (*wp)->camY);
    evtSetFloat(event, args[2], (*wp)->camZ);
    evtSetFloat(event, args[3], (*wp)->camX2);
    evtSetFloat(event, args[4], (*wp)->camY2);
    evtSetFloat(event, args[5], (*wp)->camZ2);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_to_bed_evt) {
    evtSetValue(event, event->args[0], (*wp)->to_bed_evt);
    return EVT_RETURN_DONE;
}

USER_FUNC(get_from_bed_evt) {
    evtSetValue(event, event->args[0], (*wp)->from_bed_evt);
    return EVT_RETURN_DONE;
}

// opt_propagation off is required: with it on MWCC puts the string-pool base temp in r6 and
// event->args in r5; the original has the string-pool base in r5 and event->args in r6.
#pragma opt_propagation off
static USER_FUNC(paper_on) {
    s32* args = event->args;
    const char* str = str_BGM_FF_GO_SLEEPING1;
    f32 val = evtGetFloat(event, args[0]);
    MarioWork* mario = marioGetPtr();

    if (isFirstCall) {
        marioPaperOn(str + 0xBC);
        marioChgPose(str_M_S_1);
        marioChgPaper(str + 0xC4);
        event->userdata[0] = 0;
    }
    if (event->userdata[0] < 0x1E) {
        if (strcmp(gp->currentMapName, str + 0x60) == 0) {
            f32 deg = float_6p2832_8041bc70 * val / float_360_8041bc78;
            f32 c;
            f32 s;

            c = cos(deg);
            mario->position.x -= 0.2f * c;
            s = sin(deg);
            mario->position.z -= 0.2f * s;
        } else {
            f32 deg = float_6p2832_8041bc70 * val / float_360_8041bc78;
            f32 c;
            f32 s;

            c = cos(deg);
            mario->position.x += 0.2f * c;
            s = sin(deg);
            mario->position.z += 0.2f * s;
        }
        event->userdata[0]++;
    }
    mario->dispReqDir = val;
    mario->unk1A8 = val;
    return 0;
}
#pragma opt_propagation reset


static USER_FUNC(paper_off) {
    marioPaperOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(offset) {
    s32* args = event->args;
    f32 val = evtGetFloat(event, args[0]);
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 deg;
    f32 s;
    f32 c;

    deg = cam->viewYaw;
    deg = float_90_8041bc74 + deg;
    deg = float_6p2832_8041bc70 * deg / float_360_8041bc78;
    s = sin(deg);
    evtSetFloat(event, args[1], val * -s);
    c = cos(deg);
    evtSetFloat(event, args[2], val * c);
    return EVT_RETURN_DONE;
}

USER_FUNC(breakfast) {
    ItemEntry* item = itemNameToPtr((char*)str_breakfast);
    if (item == NULL || !(item->flags & 1)) {
        if ((*wp)->field_0 != 0) {
            itemEntry(str_breakfast, (*wp)->field_0, 0x10, (*wp)->field_4, (*wp)->field_8, (*wp)->field_C, -1, 0);
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(snd_off) {
    psndSetFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(snd_on) {
    psndClearFlag(0x80);
    return EVT_RETURN_DONE;
}

USER_FUNC(N_cameraman_on) {
    animPoseSetMaterialFlagOn(npcNameToPtr(str_802ECE64)->poseId, 0x1800);
    return EVT_RETURN_DONE;
}

// 4-byte zero words that end the unit's .data, .sdata and .sbss: all-zero objects need explicit_zero_data to be emitted and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8033CA64_data = 0;
__declspec(section ".sdata") u32 gap_07_8041083C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
#pragma force_active on
u32 gap_08_8041894C_sbss;
#pragma force_active reset
