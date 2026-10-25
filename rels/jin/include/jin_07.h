#pragma once

#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// Per-entry state of the 200 "teresa" boxes managed by senteresa_ext_init/ext_main.
typedef struct TeresaEntry {
    f32 startX;  //0x0, Hermite start point (h00 term), the pose position once a move ends
    f32 startY;  //0x4
    f32 startZ;  //0x8
    f32 endX;    //0xC, Hermite end point (h01 term)
    f32 endY;    //0x10, also scaled by 3.4f into the start tangent's y
    f32 endZ;    //0x14
    f32 progress;  //0x18, Hermite parameter t: +0.015f per frame in state 1, the move ends at 1.0f
    f32 field_0x1C;  //0x1C
    f32 field_0x20;  //0x20
    f32 startTangentX;  //0x24, start tangent's x (the h10 term)
    s32 delay;   //0x28, frames before the box appears (state 0 counts it down to 0)
    u8 state;    //0x2C, senteresa_ext_main's switch: 0 waiting, 1 moving, 99 done
    u8 pad_0x2D[3];  //0x2D
} TeresaEntry;

USER_FUNC(senteresa_okure_move);
USER_FUNC(senteresa_okure_move_init);
USER_FUNC(senteresa_get_position);
void computePosHermite(ExtPose* pose, TeresaEntry* p);
EVT_DEF(jin_07_juyo_oumu_vivian);
EVT_DEF(evt_locked);
EVT_DEF(evt_teresa_box_open);
EVT_DEF(jin_07_init_evt);
