#pragma once

#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"

typedef struct CamKaiten {
    Vec pos;   // 0x00
    Vec at;    // 0x0C
    f32 angle; // 0x18
    f32 dist;  // 0x1C
} CamKaiten;

USER_FUNC(camps25);
USER_FUNC(camps30);
EVT_DEF(welcome_wonderforest_init);
EVT_DEF(welcome_wonderforest);
EVT_DEF(win_06_init_evt);
