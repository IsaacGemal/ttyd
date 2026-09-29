#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(is_dol_viewer);
USER_FUNC(stardust_setting);
USER_FUNC(pcharge_set_position);
USER_FUNC(kemuri_set_color);
USER_FUNC(kemuri_set_position);
USER_FUNC(starstone_setting);
USER_FUNC(cam_mahojin_kaiten_main);
USER_FUNC(map_bgmode_end);
USER_FUNC(map_bgmode);
USER_FUNC(crystal_bgmode);
USER_FUNC(unk_tik_0000247c);
USER_FUNC(tiz_broken);
void tiz_broken_draw(void);  // TODO: real signature
USER_FUNC(tiz_stone);
USER_FUNC(img_alpha);
USER_FUNC(effect_position);
USER_FUNC(mahojin_effect_alpha);
USER_FUNC(disp_treasure_map_func);
USER_FUNC(disp_lighteffect_func);
USER_FUNC(eff_starstone_sound_off);
USER_FUNC(starstone_circle_link_func);
void starstone_set_scale(void);  // TODO: real signature
USER_FUNC(starstone_set_alpha);
USER_FUNC(starstone_get_position);
USER_FUNC(starstone_set_position);
EVT_DEF(mahojin_alpha);
EVT_DEF(mahojin_alpha_up);
EVT_DEF(mahojin_alpha_down);
EVT_DEF(large_door_stg0_init);
EVT_DEF(large_door_stg0);
EVT_DEF(look_large_door);
EVT_DEF(kaidan_event);
EVT_DEF(evt_mahojin_move);
EVT_DEF(evt_mahojin_move2);
EVT_DEF(evt_fogset);
EVT_DEF(evt_fogreset);
EVT_DEF(mahojin_se_event);
EVT_DEF(mahojin_se_event_end);
EVT_DEF(disp_lighteffect);
EVT_DEF(disp_treasure_map);
EVT_DEF(evt_mapwrite);
EVT_DEF(fake_fade);
EVT_DEF(evt_starstone_up);
EVT_DEF(evt_starstone_set);
EVT_DEF(mahojin_event);
EVT_DEF(mahojin_event1);
EVT_DEF(mahojin_event2);
EVT_DEF(mahojin_event3);
EVT_DEF(mahojin_event4);
EVT_DEF(mahojin_event5);
EVT_DEF(mahojin_event6);
EVT_DEF(starstone_circle_link);
EVT_DEF(lastdungeon_anim_init);
EVT_DEF(lastdungeon_init);
EVT_DEF(lastdungeon);
EVT_DEF(mahojin_event7);
EVT_DEF(tik_05_init_evt);
