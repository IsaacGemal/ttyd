#pragma once

#include "evt/evt_cmd.h"
#include <dolphin/mtx.h>
#include "memory.h"

extern s32 __float_nan[];
long __fpclassifyf(float x);

typedef struct CamKaiten {
    Vec pos;   // 0x00
    Vec at;    // 0x0C
    f32 angle; // 0x18
    f32 dist;  // 0x1C
} CamKaiten;

typedef struct BrokenEntry {
    s32 state;        // 0x00, 0 while flying, 1 once it has left the screen
    s32 timer;        // 0x04, frames to wait before the piece moves and draws
    s32 quadrant;     // 0x08, 0-3: diagonal the piece is drawn turned to
    f32 impulse_x;    // 0x0C, decays by param_tbl[2] each frame
    f32 impulse_y;    // 0x10, decays by param_tbl[2] each frame
    f32 impulse_z;    // 0x14, decays by param_tbl[2] each frame
    f32 x;            // 0x18
    f32 y;            // 0x1C
    f32 z;            // 0x20
    f32 spin_angle;   // 0x24, degrees about y
    f32 spin_speed;   // 0x28, grows by param_tbl[4] each frame
    f32 vel_x;        // 0x2C, constant per-frame x
    f32 vel_y;        // 0x30, constant per-frame y
} BrokenEntry;

typedef struct BrokenWork {
    s32 tex_copied;             // 0x00, set once the frame has been copied into tex_buffer
    smartEntry* tex_buffer;     // 0x04
    smartEntry* entry_buffer;   // 0x08, the BrokenEntry array
} BrokenWork;

USER_FUNC(is_dol_viewer);
USER_FUNC(stardust_setting);
USER_FUNC(pcharge_set_position);
USER_FUNC(kemuri_set_color);
USER_FUNC(kemuri_set_position);
USER_FUNC(starstone_setting);
USER_FUNC(cam_mahojin_kaiten_main);
USER_FUNC(map_bgmode_end);
USER_FUNC(map_bgmode);
USER_FUNC(crystal_bgmode);
USER_FUNC(map_disp_set);
USER_FUNC(tiz_broken);
void tiz_broken_draw(CameraId cameraId, void* param);
USER_FUNC(tiz_stone);
USER_FUNC(img_alpha);
USER_FUNC(effect_position);
USER_FUNC(mahojin_effect_alpha);
USER_FUNC(disp_treasure_map_func);
USER_FUNC(disp_lighteffect_func);
USER_FUNC(eff_starstone_sound_off);
USER_FUNC(starstone_circle_link_func);
USER_FUNC(starstone_set_scale);
USER_FUNC(starstone_set_alpha);
USER_FUNC(starstone_get_position);
USER_FUNC(starstone_set_position);
EVT_DEF(mahojin_alpha);
EVT_DEF(mahojin_alpha_up);
EVT_DEF(mahojin_alpha_down);
EVT_DEF(large_door_stg0_init);
EVT_DEF(large_door_stg0);
EVT_DEF(look_large_door);
EVT_DEF(kaidan_event);
EVT_DEF(evt_mahojin_move);
EVT_DEF(evt_mahojin_move2);
EVT_DEF(evt_fogset);
EVT_DEF(evt_fogreset);
EVT_DEF(mahojin_se_event);
EVT_DEF(mahojin_se_event_end);
EVT_DEF(disp_lighteffect);
EVT_DEF(disp_treasure_map);
EVT_DEF(evt_mapwrite);
EVT_DEF(fake_fade);
EVT_DEF(evt_starstone_up);
EVT_DEF(evt_starstone_set);
EVT_DEF(mahojin_event);
EVT_DEF(mahojin_event1);
EVT_DEF(mahojin_event2);
EVT_DEF(mahojin_event3);
EVT_DEF(mahojin_event4);
EVT_DEF(mahojin_event5);
EVT_DEF(mahojin_event6);
EVT_DEF(starstone_circle_link);
EVT_DEF(lastdungeon_anim_init);
EVT_DEF(lastdungeon_init);
EVT_DEF(lastdungeon);
EVT_DEF(mahojin_event7);
EVT_DEF(tik_05_init_evt);
