#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"
void goryuugijo_init_slot(EventEntry* event, BOOL isFirstCall);
USER_FUNC(goryuugijo_main_slot);
void goryuugijo_disp_slot(CameraId cameraId, void* param);
USER_FUNC(goryuugijo_init_montemedal);
USER_FUNC(goryuugijo_main_montemedal);
void goryuugijo_disp_montemedal(CameraId cameraId, void* param);
