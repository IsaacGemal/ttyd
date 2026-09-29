#pragma once

#include <dolphin/types.h>
#include "evt/evt_cmd.h"

typedef union BeroINFO {
	const char* name;
	s32 value;
} BeroINFO;

typedef struct BeroEntry {
	const char* hitName; //0x0
	u16 kinddirArg1; //0x4
	u16 kinddirArg2; //0x6
	s32 kinddirArg3; //0x8
	s32 field_0xC; //0xC
	s32 unknown1; //0x10
	s32 unknown2; //0x14
	s32 field_0x18; //0x18
	s32* enterEvent; //0x1C;
	s32 field_0x20; //0x20
	s32* exitEvent; //0x24
	const char* nextareaArg1; //0x28 destination map
	const char* nextareaArg2; //0x2C
	u16 animeArg1; //0x30
	u16 animeArg2; //0x32
	void* animeArg3; //0x34, an event or a string, by anime type
	void* animeArg4; //0x38
} BeroEntry;

EVT_DEF(evt_bero_info_run);

USER_FUNC(evt_bero_set_reset_position);
USER_FUNC(evt_bero_set_reset_position_current);
s32 bero_get_BeroEXEC(void);
s32 bero_get_BeroNUM(void);
f32 bero_get_BeroSX(void);
f32 bero_get_BeroSY(void);
f32 bero_get_BeroSZ(void);
f32 bero_get_BeroEX(void);
f32 bero_get_BeroEY(void);
f32 bero_get_BeroEZ(void);
BeroEntry** bero_get_ptr(void);
void bero_clear_Offset(void);
s32 bero_id_filter(s32 id);
USER_FUNC(evt_bero_mapchange);
USER_FUNC(evt_bero_get_entername);
USER_FUNC(evt_bero_exec_onoff);
USER_FUNC(evt_bero_exec_get);
USER_FUNC(evt_bero_exec_wait);
USER_FUNC(evt_bero_get_start_position);
USER_FUNC(evt_bero_get_end_position);
USER_FUNC(evt_bero_get_info_anime);
USER_FUNC(evt_bero_get_info_length);
USER_FUNC(evt_bero_get_info_kinddir);
USER_FUNC(evt_bero_get_info_nextarea);
USER_FUNC(evt_bero_set_now_number);
USER_FUNC(evt_bero_get_now_number);
USER_FUNC(evt_bero_get_info);
USER_FUNC(evt_bero_get_into_info);
void bero_get_position_pipe_sub(s32 a, s32 b, s32 c, s32 d, Vec* start, Vec* end);

USER_FUNC(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry);
USER_FUNC(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos);
EVT_DEF(bero_case_entry);
EVT_DEF(bero_case_exec_pipe);
EVT_DEF(bero_case_switch_off);
EVT_DEF(bero_case_switch_on);
EVT_DEF(bero_close_door_play_se);
USER_FUNC(bero_get_position_normal);
EVT_DEF(bero_open_door_play_se);
EVT_DEF(bero_play_enter_anime);
USER_FUNC(evt_bero_get_info_num);
USER_FUNC(evt_bero_id_filter);
USER_FUNC(evt_bero_mario_go);
USER_FUNC(evt_bero_mario_go_init);
USER_FUNC(evt_bero_mario_go_wait);
USER_FUNC(evt_bero_overwrite);
