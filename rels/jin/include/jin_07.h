#pragma once

#include "driver/extdrv.h"
#include "evt/evt_cmd.h"

// Per-entry state of the 200 "teresa" boxes managed by senteresa_ext_init/ext_main.
typedef struct TeresaEntry {
    f32 field_0x00;  //0x0
    f32 field_0x04;  //0x4
    f32 field_0x08;  //0x8
    f32 field_0x0C;  //0xC
    f32 field_0x10;  //0x10
    f32 field_0x14;  //0x14
    f32 field_0x18;  //0x18
    f32 field_0x1C;  //0x1C
    f32 field_0x20;  //0x20
    f32 field_0x24;  //0x24
    s32 field_0x28;  //0x28
    u8 field_0x2C;   //0x2C
    u8 pad_0x2D[3];  //0x2D
} TeresaEntry;

USER_FUNC(senteresa_okure_move);
USER_FUNC(senteresa_okure_move_init);
USER_FUNC(senteresa_get_position);
void computePosHermite(ExtPose* pose, f32* p);
EVT_DEF(jin_07_juyo_oumu_vivian);
EVT_DEF(evt_locked);
EVT_DEF(evt_teresa_box_open);
EVT_DEF(jin_07_init_evt);
