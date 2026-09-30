#pragma once

#include "evt/evt_cmd.h"
#include "driver/camdrv.h"

USER_FUNC(flower_pos_report);
USER_FUNC(evt_flower_end_main);
USER_FUNC(evt_flower_end);
USER_FUNC(evt_flower);
USER_FUNC(evt_flower_init);
void flower_disp(CameraId camera, void* param);
void mirror_ball(void* param);
USER_FUNC(fade_alpha);
USER_FUNC(ss_ch_set);
USER_FUNC(ss_ch_check);
EVT_DEF(win_04w_door_open);
EVT_DEF(win_04w_door_close);
EVT_DEF(win_04e_door_open);
EVT_DEF(win_04e_door_close);
EVT_DEF(meet_clouda);
EVT_DEF(meet_clouda2);
EVT_DEF(clouda_inai);
EVT_DEF(clouda_irai);
EVT_DEF(clouda_irai_ato);
EVT_DEF(mirror_trans);
EVT_DEF(mirror_rotate);
EVT_DEF(mario_move_wait);
EVT_DEF(dummy1_jump);
EVT_DEF(dummy2_jump);
EVT_DEF(clouda_nakama);
EVT_DEF(touch_door);
EVT_DEF(mirror_ball_event);
EVT_DEF(flower_event);
EVT_DEF(flower_disp_pos);
EVT_DEF(win_04_init_evt);

// Runtime state of the flower effect, allocated by evt_flower_init.
typedef struct Flower {
    s16 state;    // 0x0
    s16 timer;    // 0x2
    f32 x;        // 0x4
    f32 y;        // 0x8
    f32 scale;    // 0xC
    f32 scaleMax; // 0x10
    s32 alpha;    // 0x14
} Flower;         // 0x18

typedef struct FlowerData {
    s32 count;       // 0x0
    Flower* flowers; // 0x4
} FlowerData;
