#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

USER_FUNC(cam_yure);
USER_FUNC(mabuta);
void mabutaDisp(CameraId cameraId, void* param);
void tegami_delete(void);  // TODO: real signature
USER_FUNC(tegami);
EVT_DEF(tegami_init_evt);
EVT_DEF(evt_mezame);
EVT_DEF(mezame_init_evt);
EVT_DEF(gor_10_init_evt);
