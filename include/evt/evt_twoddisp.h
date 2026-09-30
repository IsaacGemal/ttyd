#pragma once

#include "evt/evt_cmd.h"

#include "driver/camdrv.h"

#include <dolphin/types.h>
void twoddisp_disp(CameraId cameraId, void* param);

USER_FUNC(evt_twoddisp_entry);
USER_FUNC(evt_twoddisp_flag_onoff);
USER_FUNC(evt_twoddisp_init);
USER_FUNC(evt_twoddisp_set_base_position);
USER_FUNC(evt_twoddisp_set_position);
USER_FUNC(twoddisp_main);
