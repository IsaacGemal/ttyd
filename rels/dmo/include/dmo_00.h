#pragma once

#include "driver/camdrv.h"
#include "evt/evt_cmd.h"

USER_FUNC(make_msg);
USER_FUNC(eff_cam1);
USER_FUNC(eff_cam);
USER_FUNC(adjust);
USER_FUNC(book_r);
USER_FUNC(demo_scale);
USER_FUNC(demo_winonoff);
USER_FUNC(demo_draw);
void demo_window(CameraId cameraId, void* param);
void demo_book(CameraId cameraId, void* param);
void demo_img(CameraId cameraId, void* param);
void demo_desk(CameraId cameraId, void* param);
void demo_cap(void);
void demo_exit(void);
USER_FUNC(demo_init);
EVT_DEF(buttonCheck);
EVT_DEF(demo_evt);
EVT_DEF(roten_move);
EVT_DEF(kuribo_move);
EVT_DEF(chusan_move);
EVT_DEF(dmo_00_init_evt);
