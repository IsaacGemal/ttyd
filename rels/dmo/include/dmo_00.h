#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(make_msg);
USER_FUNC(eff_cam1);
USER_FUNC(eff_cam);
USER_FUNC(adjust);
USER_FUNC(book_r);
USER_FUNC(demo_scale);
USER_FUNC(demo_winonoff);
USER_FUNC(demo_draw);
void demo_window(void);  // TODO: real signature
void demo_book(void);  // TODO: real signature
void demo_img(void);  // TODO: real signature
void demo_desk(void);  // TODO: real signature
void demo_cap(void);  // TODO: real signature
void demo_exit(void);  // TODO: real signature
USER_FUNC(demo_init);
EVT_DEF(buttonCheck);
EVT_DEF(demo_evt);
EVT_DEF(roten_move);
EVT_DEF(kuribo_move);
EVT_DEF(chusan_move);
EVT_DEF(dmo_00_init_evt);
