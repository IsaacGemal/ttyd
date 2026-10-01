#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>

s32 marioGetCamId(void);
void marioCamZoomOffReq2(s32);
BOOL marioCamZoomUp(void);
f32 marioGetCamFollowRate(void);
void marioResetCamFollowRate(void);
void marioInitCamId(void);
void marioUpdateCamPos(void);
void marioNoUpdateCamPos(void);
void camFollowYOn(void);
void camFollowYOff(void);
void marioCamMain(void);
void marioUpdateCam(void);
BOOL marioCamZoomUpLevelMain(void);
void marioSetCamId(CameraId camId);
void marioSetCamFollowRate(f32 followRate);
void marioResetCamShift(void);
void marioResetCamShiftRate(void);
void marioCamZoomOffReq(void);
