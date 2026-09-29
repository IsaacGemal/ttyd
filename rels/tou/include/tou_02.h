#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(evt_end_count);
void check_hammer(void);  // TODO: real signature
void ext_fan_dispent(void);  // TODO: real signature
void ext_fan_main(void);  // TODO: real signature
void fan_disp(void);  // TODO: real signature
void ext_fan_init(void);  // TODO: real signature
EVT_DEF(n_1_door_open);
EVT_DEF(n_1_door_close);
EVT_DEF(n_3_door_open);
EVT_DEF(n_3_door_close);
EVT_DEF(n_4_door_open);
EVT_DEF(n_4_door_close);
EVT_DEF(evt_move_g);
EVT_DEF(init_gard1);
EVT_DEF(talk_gard1);
EVT_DEF(init_gard2);
EVT_DEF(talk_gard2);
EVT_DEF(init_repo);
EVT_DEF(talk_repo);
EVT_DEF(init_cam1);
EVT_DEF(talk_cam1);
EVT_DEF(init_cam2);
EVT_DEF(talk_cam2);
EVT_DEF(init_kino4);
EVT_DEF(reg_kino4);
EVT_DEF(talk_kino4);
EVT_DEF(init_kino5);
EVT_DEF(talk_kino5);
EVT_DEF(init_kinopiko);
EVT_DEF(talk_kinopiko);
EVT_DEF(init_kinopio_f);
EVT_DEF(talk_kinopio_f);
EVT_DEF(init_chu);
EVT_DEF(talk_chu);
EVT_DEF(move_champ);
EVT_DEF(talk_champ);
EVT_DEF(init_shikowa);
EVT_DEF(talk_shikowa);
EVT_DEF(init_ironA);
EVT_DEF(talk_ironA);
EVT_DEF(init_ironB);
EVT_DEF(talk_ironB);
EVT_DEF(init_borodo);
EVT_DEF(talk_borodo);
EVT_DEF(evt_tougi2);
EVT_DEF(mario_momikucha_msg);
EVT_DEF(evt_momikucha);
EVT_DEF(tou_02_init_evt);
