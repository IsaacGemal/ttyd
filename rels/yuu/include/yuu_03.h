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
    s32 field_0x20;                // 0x20
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

// The ryushi particle buffer (xwp): entry 0 is the emitter, entries 1 to ryuwp - 1 the particles.
typedef struct RyushiEntry {
    s32 field_0x0;              // 0x00
    s32 direction;              // 0x04, emitter: evt_ryushi moves the particles toward -z when 0, +z when 1
    union {
        f32 spacingX;           // 0x08, emitter: particle grid spacing (evt_ryushi_clr)
        f32 homeX;              // 0x08, particle: its grid cell, where a wrapped particle restarts (+-0.3 spacing)
    };
    union {
        f32 spacingY;           // 0x0C
        f32 homeY;              // 0x0C
    };
    union {
        f32 spacingZ;           // 0x10
        f32 homeZ;              // 0x10, particle: set by evt_ryushi_clr only
    };
    f32 x;                      // 0x14
    f32 y;                      // 0x18
    f32 z;                      // 0x1C
    s32 alpha;                  // 0x20, particle: ryushi_disp's TEVREG0 alpha (the emitter's is written, not read)
    s32 age;                    // 0x24, emitter: counts evt_ryushi calls; the emitter alpha ramps in over the first 0x14
    f32 scale;                  // 0x28
    s32 field_0x2C;             // 0x2C
    s32 tev0G;                  // 0x30, emitter: ryushi_disp's TEVREG0 green
    s32 tev0B;                  // 0x34, emitter: TEVREG0 blue
    s32 tev1R;                  // 0x38, emitter: ryushi_disp's TEVREG1 colour
    s32 tev1G;                  // 0x3C
    s32 tev1B;                  // 0x40
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
