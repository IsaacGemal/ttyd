#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

void irai_window_disp(CameraId cameraId, void* param);
USER_FUNC(irai_end_func);
USER_FUNC(irai_check_func);
USER_FUNC(irai_main_func);
USER_FUNC(irai_init_func);
USER_FUNC(irai_cancel_func);
USER_FUNC(irai_start_func);
void irai_flag_disp(void);  // TODO: real signature
void irai_flag_clear(void);  // TODO: real signature
EVT_DEF(irai_start_event_null);
EVT_DEF(irai_start_event01);
EVT_DEF(irai_start_event04);
EVT_DEF(irai_end_event_null);
EVT_DEF(irai_end_event01);
EVT_DEF(irai_end_event04);
EVT_DEF(gor_irai_init);
EVT_DEF(gor_irai_lecture);
EVT_DEF(gor_irai_main);
EVT_DEF(gor_irai_tenshu_main);
