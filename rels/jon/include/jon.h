#pragma once

#include "evt/evt_cmd.h"

void iri_12_msgcb(void);  // TODO: real signature
USER_FUNC(wait_landon);
USER_FUNC(iri_12_makkino_set_y);
USER_FUNC(iri_12_paper_init);
USER_FUNC(shikakeFlag);
USER_FUNC(wanwan_set_home_pos);
USER_FUNC(setup_npc_ex_para);
USER_FUNC(yattukeFlag);
USER_FUNC(enemy_info);
USER_FUNC(evt_raster);
void raster_disp(void);  // TODO: real signature
void _unresolved(void);  // TODO: real signature
void _epilog(void);  // TODO: real signature
void _prolog(void);  // TODO: real signature
EVT_DEF(jon_init_evt);
EVT_DEF(enemy_setup);
EVT_DEF(evt_open_box);
EVT_DEF(get_map_name);
EVT_DEF(talk_idouya);
EVT_DEF(idouya_setup);
EVT_DEF(talk_gyousyou);
EVT_DEF(gyousyou_setup);
EVT_DEF(shikake_setup);
EVT_DEF(dokan_open);
EVT_DEF(dokan_open2);
EVT_DEF(floor_inc);
EVT_DEF(floor_clear);
EVT_DEF(setup_boss);
EVT_DEF(setup_return);
EVT_DEF(setup_prev_return);
EVT_DEF(setup_normal);
EVT_DEF(iri_12_dokan_in_sub);
EVT_DEF(iri_12_dokan_in);
EVT_DEF(iri_12_makkino_return_dokan);
EVT_DEF(iri_12_makkino_return);
EVT_DEF(iri_12_makkino_return_b100);
EVT_DEF(iri_12_makkino_return_zonbaba);
EVT_DEF(iri_12_makkino_move_dokan_side);
EVT_DEF(iri_12_enemy_dead_event);
EVT_DEF(iri_12_makkino_urouro);
EVT_DEF(iri_12_makkino_buruburu);
EVT_DEF(iri_12_makkino_okiru);
EVT_DEF(iri_12_makkino_fall);
EVT_DEF(iri_12_makkino_fall_return);
EVT_DEF(iri_12_init_fall);
EVT_DEF(evt_iri_30_touch_rakugaki);
EVT_DEF(evt_iri_30_bomb_rakugaki);
EVT_DEF(zonbaba_first_event);
