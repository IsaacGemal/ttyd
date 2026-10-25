#pragma once

#include "evt/evt_cmd.h"
#include "evt_yuuminigame.h"
#include <dolphin/mtx.h>

typedef struct Yuuminigame2Data {
    u32 flags;                  // 0x00
    s32 phase;                  // 0x04
    s32 counterData;            // 0x08, yuucounter_get_data() at the finish, the ranking's result (minigame02_end LW(0))
    s32 unk0C;                  // 0x0C, decremented each time a dai_uzu or uzu catches Mario; at the end turned into the ranking's mult
    s32 objIndex;               // 0x10, s_nametbl/turn_tbl/hoko_tbl index of the uzu Mario is in (1-4 S_dai_uzu, 5-12 S_uzu), 0x15 for none
    s32 step;                   // 0x14, step of the dai_uzu sequence (objIndex 1-4)
    f32 startDist;              // 0x18, Mario's distance from the dai_uzu when caught, shrunk to 0 as timer runs
    Vec dir;                    // 0x1C, direction from the dai_uzu to Mario, turned by turn_tbl each frame
    s32 timer;                  // 0x28, frames left in the current step
    f32 startY;                 // 0x2C, Mario's y when caught
    f32 startX;                 // 0x30, Mario's x when step 1 starts
    s32 monteCount;             // 0x34, chips picked up (fall_rtn), the ranking's bonus (minigame02_end LW(2))
    EventEntry* viewEvent;      // 0x38, the minigame_view_monte event
} Yuuminigame2Data;


typedef struct MonteTable {
    f32 xmin;                   // 0x00
    f32 xmax;                   // 0x04
    f32 zmin;                   // 0x08
    f32 zmax;                   // 0x0C
    s32 count;                  // 0x10
} MonteTable;                    // 0x14

s32 yuuminigame2_gamerule(EventEntry* event, BOOL isFirstCall, void* work, Yuuminigame2Data* data);
// yuuminigame_disp_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_main_2fune: this unit has its own copy; a header already declares it.
// yuuminigame_init_2fune: this unit has its own copy; a header already declares it.
USER_FUNC(yuuminigame2_opening_chk);
USER_FUNC(yuuminigame2_opening_on_off);
USER_FUNC(yuuminigame2_get_name);
USER_FUNC(yuuminigame2_set_phase);
USER_FUNC(unk_yuu_00005268);
EVT_DEF(yuu_01_kaijo_init);
EVT_DEF(yuu_01_kaijo_init2);
EVT_DEF(minigame02_main);
EVT_DEF(minigame02_start_cam);
EVT_DEF(minigame02_start_cam_cancel);
EVT_DEF(minigame02_start);
EVT_DEF(minigame02_end);
EVT_DEF(yuu_01_init_evt);
