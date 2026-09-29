#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(gans_white_off);
USER_FUNC(gans_white_on);
USER_FUNC(eff_pos);
USER_FUNC(evt_boss_stone_r);
USER_FUNC(starstone_sound_onoff);
USER_FUNC(getY);
USER_FUNC(item_jump);
USER_FUNC(confupose);
USER_FUNC(snd_sfx_stop);
USER_FUNC(burst_chain);
USER_FUNC(tribe_chk);
void screen_tev(void);  // TODO: real signature
// This unit has its own copy of GXSetTexCoordGen; the SDK header declares it.
USER_FUNC(screen_init);
void ext_kyaku_dispent(void);  // TODO: real signature
void ext_kyaku_main(void);  // TODO: real signature
void kyaku_disp(void);  // TODO: real signature
void ext_kyaku_init(void);  // TODO: real signature
void __tou_rand(void);  // TODO: real signature
void __tou_srand(void);  // TODO: real signature
USER_FUNC(_firstattack_setting);
EVT_DEF(x);
EVT_DEF(evt_kyaku);
EVT_DEF(open_main);
EVT_DEF(close_main);
EVT_DEF(s1_door_open);
EVT_DEF(s1_door_close);
EVT_DEF(s1_door_open2);
EVT_DEF(s2_door_open);
EVT_DEF(s2_door_close);
EVT_DEF(s2_door_open2);
EVT_DEF(evt_tougi);
EVT_DEF(init_kinosikowa);
EVT_DEF(evt_damage_pose);
EVT_DEF(npc_entry);
EVT_DEF(evt_random_battle);
EVT_DEF(evt_random_battle2);
EVT_DEF(evt_teki_nyuujyou);
EVT_DEF(evt_teki_nyuujyou_default);
EVT_DEF(burst_chain_entry);
EVT_DEF(burst_wangwang);
EVT_DEF(evt_match);
EVT_DEF(evt_kurikuri_opening);
EVT_DEF(evt_kurikuri_win);
EVT_DEF(evt_kurikuri_lose);
EVT_DEF(evt_shinemon_opening);
EVT_DEF(evt_shinemon_win);
EVT_DEF(evt_shinemon_lose);
EVT_DEF(evt_champ_opening);
EVT_DEF(evt_champ_opening_re);
EVT_DEF(evt_champ_win);
EVT_DEF(evt_champ_lose);
EVT_DEF(evt_default_opening);
EVT_DEF(evt_default_win);
EVT_DEF(evt_default_lose);
EVT_DEF(evt_champion2);
EVT_DEF(evt_boss_stone);
EVT_DEF(evt_watch_match);
EVT_DEF(evt_kyaku_1);
EVT_DEF(evt_kyaku_2);
EVT_DEF(evt_kyaku_3);
EVT_DEF(evt_kyaku_4);
EVT_DEF(evt_kyaku_5);
EVT_DEF(evt_kyaku_6);
EVT_DEF(evt_kyaku_7);
EVT_DEF(evt_kyaku_8);
EVT_DEF(tou_03_init_evt);
