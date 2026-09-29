#pragma once

#include "evt/evt_cmd.h"

void firebar_cb(void);  // TODO: real signature
USER_FUNC(coin_entry);
void firebar_disp(void);  // TODO: real signature
USER_FUNC(firebar_disp_evt);
USER_FUNC(firebar_entry);
void _firebar_trans(void);  // TODO: real signature
void _firebar_rot(void);  // TODO: real signature
void _firebar_hitCheck2(void);  // TODO: real signature
void _firebar_hitCheck(void);  // TODO: real signature
EVT_DEF(firebar_coin);
EVT_DEF(firebar_evt);
EVT_DEF(las_04_init_evt);
