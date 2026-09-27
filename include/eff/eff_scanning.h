#pragma once

#include "driver/camdrv.h"

#include <dolphin/types.h>
#include "driver/effdrv.h"
void effScanningMain(EffectEntry* effect);
void effScanningDisp(CameraId camId, void* param);
