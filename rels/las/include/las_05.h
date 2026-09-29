#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(party_hit);
USER_FUNC(chk_mario_ctrloff);
USER_FUNC(party_land_on);
USER_FUNC(mario_land_on);
USER_FUNC(get_party_y_pos);
USER_FUNC(set_mario_y_pos);
USER_FUNC(get_mario_y_pos);
void party_hit_check(void);  // TODO: real signature
void calc_area(void);  // TODO: real signature
void move_check(void);  // TODO: real signature
USER_FUNC(kurokaron_move_position);
void _kurokaron_next_move_index(void);  // TODO: real signature
void kurokaron_move2(void);  // TODO: real signature
void _kurokaron_run_away(void);  // TODO: real signature
USER_FUNC(kurokaron_get_move_pos);
USER_FUNC(kurokaron_flag_check);
EVT_DEF(kurokaron_move);
EVT_DEF(kurokaron_init);
EVT_DEF(kurokaron_talk);
EVT_DEF(las_05_init_evt);
