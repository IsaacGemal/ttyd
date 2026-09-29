#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

USER_FUNC(countDisp);
void printCount(CameraId cameraId, void* param);
EVT_DEF(mony_peton_setup);
EVT_DEF(lost_wedding_ring_sub);
EVT_DEF(lost_wedding_ring);
EVT_DEF(wedding_ring_consign);
EVT_DEF(find_wedding_ring);
EVT_DEF(mony_peton_regl);
EVT_DEF(mony_regl);
EVT_DEF(peton_regl);
EVT_DEF(evt_treeb_01);
EVT_DEF(muj_02_init_evt);
