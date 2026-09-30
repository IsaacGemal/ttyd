#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"
#include "memory.h"

typedef struct GonbabaProj {
    GXTexObj texObj;       // 0x00
    smartEntry* texBuffer; // 0x20
} GonbabaProj;

USER_FUNC(unk_hei_000002a8);
USER_FUNC(dist_check);
USER_FUNC(cam_test2);
USER_FUNC(cam_test);
USER_FUNC(shake_reset);
USER_FUNC(shake);
USER_FUNC(make_gonbaba_projection_evt);
void make_gonbaba_projection(CameraId cameraId, void* param);
EVT_DEF(make_proj_evt);
EVT_DEF(evt_gonbaba_shadow);
EVT_DEF(gonbaba_evt);
EVT_DEF(evt_snd);
EVT_DEF(moji_msg);
EVT_DEF(setting_sun_stone);
EVT_DEF(sun_stone_set_event);
EVT_DEF(setting_moon_stone);
EVT_DEF(moon_stone_set_event);
EVT_DEF(ki_data_evt_02);
EVT_DEF(hei_00_koopa_evt);
EVT_DEF(hei_00_init_evt);
