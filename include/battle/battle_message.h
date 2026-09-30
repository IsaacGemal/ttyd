#pragma once

#include "evt/evt_cmd.h"

USER_FUNC(btlevtcmd_AnnounceMessage);
USER_FUNC(btlevtcmd_AnnounceSetParam);
#include "driver/camdrv.h"
void _disp(CameraId cameraId, void* param);
