#pragma once

#include "evt/evt_cmd.h"
#include "driver/extdrv.h"
#include "driver/npcdrv.h"
#include "mario/mario.h"

USER_FUNC(party_hit);
USER_FUNC(chk_mario_ctrloff);
USER_FUNC(party_land_on);
USER_FUNC(mario_land_on);
USER_FUNC(get_party_y_pos);
USER_FUNC(set_mario_y_pos);
USER_FUNC(get_mario_y_pos);
s32 party_hit_check(f32 x, f32 z, f32* out);
void calc_area(s32 index, f32 x, f32 z);
void move_check(ExtPose* pose, s32 index);
USER_FUNC(kurokaron_move_position);
void _kurokaron_next_move_index(void);
s32 kurokaron_move2(NpcEntry* npc, MarioWork* mario);
s32 _kurokaron_run_away(f32 x1, f32 z1, f32 x2, f32 z2);
USER_FUNC(kurokaron_get_move_pos);
USER_FUNC(kurokaron_flag_check);
EVT_DEF(kurokaron_move);
EVT_DEF(kurokaron_init);
EVT_DEF(kurokaron_talk);
EVT_DEF(las_05_init_evt);
