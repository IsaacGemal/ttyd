#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(cam_test_func);
USER_FUNC(cam_test_init_func);
USER_FUNC(ship_rotate_rev);
USER_FUNC(ship_rotate);
USER_FUNC(tobikakaru_end);
USER_FUNC(tobikakaru_init);
void tobikakari_seq00(void);  // TODO: real signature
void random_choice_target(void);  // TODO: real signature
USER_FUNC(mario_main_func);
USER_FUNC(mario_main_setup_func);
USER_FUNC(mario_main_init_func);
USER_FUNC(kaizoku_main_func);
USER_FUNC(dai3_butai_main_func);
void suifu_kyorokyoro_init(void);  // TODO: real signature
void suifu_kyorokyoro(void);  // TODO: real signature
USER_FUNC(koburon_get_encount_info);
EVT_DEF(suifu_dark);
EVT_DEF(nannpc_light);
EVT_DEF(koburon_init);
EVT_DEF(koburon_regp);
EVT_DEF(koburon_talk);
EVT_DEF(koburon_dead);
EVT_DEF(koburon_evt);
EVT_DEF(mujinto_hyochaku);
EVT_DEF(suifu_random_sub);
EVT_DEF(tama_shoot);
EVT_DEF(npc_kyorokyoro);
EVT_DEF(dai3_shurai_sub);
EVT_DEF(dai3_shurai);
EVT_DEF(iwapera_offscreen);
EVT_DEF(iwapera_offscreen_end);
EVT_DEF(dai3_kurukuru);
EVT_DEF(dai3_butai_setup);
EVT_DEF(dai3_butai_main);
EVT_DEF(dai3_kanbu_setup);
EVT_DEF(dai3_kanbu_main);
EVT_DEF(kaizoku_setup);
EVT_DEF(kaizoku_main);
EVT_DEF(mario_main_init);
EVT_DEF(mario_main);
EVT_DEF(mario_main2);
EVT_DEF(kaizoku_move_every);
EVT_DEF(dai3_move_every);
EVT_DEF(kaizoku_move_trace_se);
EVT_DEF(kaizoku_move_trace);
EVT_DEF(dai3_move_trace_se);
EVT_DEF(dai3_move_trace);
EVT_DEF(tobikakari_kemuri);
EVT_DEF(cam_dai3_vs_kaizoku);
EVT_DEF(kaizoku_vs_dai3);
EVT_DEF(senin_kansei);
EVT_DEF(nameraka_rotate);
EVT_DEF(dai3_taisan_move_trace00);
EVT_DEF(dai3_taisan_move_trace01);
EVT_DEF(dai3_beat_back);
EVT_DEF(move_ship_se00);
EVT_DEF(move_ship_se01);
EVT_DEF(option_ship_npc);
EVT_DEF(option_ship_korutesu);
EVT_DEF(option_ship_marco);
EVT_DEF(option_ship_main);
EVT_DEF(leave_island);
EVT_DEF(muj_gor_ship_sub);
EVT_DEF(muj_gor_ship_start);
EVT_DEF(into_muj_ship);
EVT_DEF(mony_peton_boarding_sub);
EVT_DEF(mony_peton_boarding);
EVT_DEF(muj_00_init_evt);
