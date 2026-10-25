#pragma once

#include "evt/evt_cmd.h"
#include "evt_yuuminigame.h"
#include <dolphin/mtx.h>

typedef struct Yuuminigame4Data {
    u32 flags;                     // 0x00
    s32 phase;                     // 0x04
    s32 counterData;               // 0x08, yuucounter_get_data() at the goal, 0x2BD after a fall; the ranking's result
    s32 mult;                      // 0x0C, the ranking's mult (0x105/0x103/0x101 by counterData)
    s32 state;                     // 0x10, yuuminigame4_gamerule's timed state: 1-4 spin puro_*, 5-8 pera_*
    s32 stateTimer;                // 0x14, frames left in the state
    f32 puroAngle;                 // 0x18, z rotation of the puro_* map objects
    f32 peraAngle;                 // 0x1C, z rotation of the pera_* map objects
    s32 unk20;                     // 0x20
    EventEntry* subEvent;          // 0x24
} Yuuminigame4Data;

typedef struct Yuuminigame4WorkField8 {
    f32 field_0x0;              // 0x00
    Vec position;               // 0x04
    f32 field_0x10;             // 0x10
    u8 pad_14[0x38 - 0x14];     // 0x14
} Yuuminigame4WorkField8;       // 0x38

typedef struct Yuuminigame4Work {
    u8 hidden;                  // 0x00, yuuminigame4_entry_disp draws the icon only while 0
    u8 pad_01;                  // 0x01
    s16 count;                  // 0x02
    u8 state;                   // 0x04, yuuminigame4_entry_init: 0 keeps the icon 40 above Mario, 1 leaves it
    u8 pad_05[0x08 - 0x05];     // 0x05
    Yuuminigame4WorkField8 field_0x8; // 0x08
    YuuminigameWork* work;      // 0x40
} Yuuminigame4Work;

typedef struct RyushiEntry {
    s32 unk0;                    // 0x00
    s32 unk04;                   // 0x04
    f32 unk08;                  // 0x08
    f32 unk0C;                  // 0x0C
    f32 unk10;                  // 0x10
    f32 x;                      // 0x14
    f32 y;                      // 0x18
    f32 z;                      // 0x1C
    s32 unk20;                  // 0x20
    s32 unk24;                  // 0x24
    f32 scale;                  // 0x28
    s32 unk2C;                  // 0x2C
    s32 unk30;                  // 0x30
    s32 unk34;                  // 0x34
    s32 unk38;                  // 0x38
    s32 unk3C;                  // 0x3C
    s32 unk40;                  // 0x40
} RyushiEntry;

s32 yuuminigame4_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame4Data* data);
// yuuminigame_disp_4slit: this unit has its own copy; a header already declares it.
// yuuminigame_main_4slit: this unit has its own copy; a header already declares it.
// yuuminigame_init_4slit: this unit has its own copy; a header already declares it.
USER_FUNC(evt_ryushi);
USER_FUNC(evt_ryushi_clr);
USER_FUNC(evt_ryushi_init);
void ryushi_disp(CameraId cameraId, void* param);
USER_FUNC(yuuminigame4_opening_chk);
USER_FUNC(yuuminigame4_opening_on_off);
USER_FUNC(yuunpc_go);
USER_FUNC(re_init_yuunpc);
USER_FUNC(evt_slit_cancel);
USER_FUNC(yuuminigame4_set_phase);
USER_FUNC(unk_yuu_0000341c);
USER_FUNC(yuuminigame4_entry_init);
void yuuminigame4_entry_disp(CameraId cameraId, void* param);
USER_FUNC(minigame04_cam_reset);
EVT_DEF(yuuminigame4_entry_event);
EVT_DEF(ryushi_event);
EVT_DEF(yuu_03_kaijo_init);
EVT_DEF(yuu_03_kaijo_init2);
EVT_DEF(minigame04_main);
EVT_DEF(minigame04_start_cam);
EVT_DEF(minigame04_start_cam_cancel);
EVT_DEF(minigame04_start);
EVT_DEF(minigame04_end);
EVT_DEF(yuu_03_init_evt);
