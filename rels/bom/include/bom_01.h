#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(bom1000_jump);
USER_FUNC(bom1000_msg);
void ext_dispent_minnnanokoe(void);  // TODO: real signature
void ext_disp_minnnanokoe(void);  // TODO: real signature
void jump_minnnanokoe(void);  // TODO: real signature
void ext_init_minnnanokoe(void);  // TODO: real signature
USER_FUNC(make_ext_table_minnnanokoe);
USER_FUNC(taihou_r);
void jump(void);  // TODO: real signature
USER_FUNC(make_ext_table);
USER_FUNC(bom_fall);
USER_FUNC(bom_flag);
USER_FUNC(sight_dir);
USER_FUNC(sight);
void disp_sight(void);  // TODO: real signature
void drawNum(void);  // TODO: real signature
void drawBG(void);  // TODO: real signature
EVT_DEF(evt_door_1);
EVT_DEF(evt_bom_01_karakara);
EVT_DEF(evt_bom1000);
EVT_DEF(gamen_shake);
EVT_DEF(gamen_shake2);
EVT_DEF(mario_tobideru);
EVT_DEF(mario_shot);
EVT_DEF(effect_delete);
EVT_DEF(evt_depth);
EVT_DEF(cam_outside);
EVT_DEF(cam_inside);
EVT_DEF(cam_inside2);
EVT_DEF(effect_1);
EVT_DEF(open_hatch);
EVT_DEF(init_nitoro);
EVT_DEF(talk_nitoro);
EVT_DEF(init_pentrit);
EVT_DEF(talk_pentrit);
EVT_DEF(init_surary);
EVT_DEF(talk_surary);
EVT_DEF(init_murabito_d);
EVT_DEF(talk_murabito_d);
EVT_DEF(bom1000_minnnanokoe);
EVT_DEF(bom_01_init_evt);
