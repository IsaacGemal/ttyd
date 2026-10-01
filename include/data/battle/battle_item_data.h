#pragma once

#include "evt/evt_cmd.h"
#include "driver/dispdrv.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// Work block allocated by _throw_item_icon.
typedef struct ThrowItemWork {
    Vec start;   // 0x00
    Vec pos;     // 0x0C
    Vec target;  // 0x18
    s32 counter; // 0x24
    f32 speed;   // 0x28
    f32 gravity; // 0x2C
    f32 velY;    // 0x30
} ThrowItemWork;

// Mystery-item ids used by _naniga_okorukana_check.
extern u32 R_mystery_item_table[14];

// .sdata2 colour constant referenced by _nancy_disp.
extern const GXColor dat_8041c504;

USER_FUNC(_full_ap_recover);
void _nancy_disp(CameraId camera, void* param);
USER_FUNC(_nancy_mirage);
void _disp_item_icon(CameraId camera, void* param);
USER_FUNC(_throw_item_icon);
USER_FUNC(_get_throw_first_pos);
USER_FUNC(_naniga_okorukana_check);
USER_FUNC(_hpfp_change_declare_2);
USER_FUNC(_hpfp_change_declare_1);
USER_FUNC(_rank_up);
USER_FUNC(_cam_rot);
USER_FUNC(_sampling_random_target);
